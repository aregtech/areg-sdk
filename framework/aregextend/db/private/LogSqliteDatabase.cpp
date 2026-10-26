/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/private/LogSqliteDatabase.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/db/LogSqliteDatabase.hpp"

#include "aregextend/db/SqliteStatement.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

#include <atomic>
#include <string>
#include <string_view>


namespace {
    //////////////////////////////////////////////////////////////////////////
    // Log specific SQL scripts for SQLite3 database.
    //////////////////////////////////////////////////////////////////////////

    //! The file mode of a connection that writes: write-ahead log, synced at checkpoints, and a
    //! page cache of up to 64 MB that holds the pages one transaction changes.
    constexpr std::string_view  _sqlWriteMode
    {
        "PRAGMA journal_mode=WAL; PRAGMA synchronous=NORMAL; PRAGMA cache_size=-65536;"
    };

    //! On close of a writing connection: no waiting, the log moved into the file, one plain file left.
    constexpr std::string_view  _sqlCloseNoWait     { "PRAGMA busy_timeout=0;" };
    constexpr std::string_view  _sqlCloseCheckpoint { "PRAGMA wal_checkpoint(TRUNCATE);" };
    constexpr std::string_view  _sqlClosePlainFile  { "PRAGMA journal_mode=DELETE;" };

    //! Create a version table to keep information about start and stop of log observer.
    constexpr std::string_view  _sqlCreateTbVersion
    {
        "CREATE TABLE \"version\" ("
            "\"name\"	            TEXT,"
            "\"version\"	        TEXT,"
            "\"describe\"	        TEXT,"
            "\"created_by\"	        TEXT,"
            "\"db_name\"	        TEXT,"
            "\"time_created\"	    NUMERIC,"
            "\"time_closed\"	    NUMERIC"
            ");"
    };

    //! A statement to insert a new entry in the version table.
    //! Normally, this is the only entry in the version table.
    //! The text values are bound, never placed in the statement: the database name comes
    //! from the configuration and the command line, see CWE-89.
    constexpr std::string_view  _sqlInsertVersion
    {
        "INSERT INTO version (name, version, describe, created_by, db_name, time_created) VALUES (?, ?, ?, ?, ?, ?);"
    };

    //! Create a table with the information about connected log source instances.
    //! Each new entry should have unique cookie_id, as well as information when it
    //! is connected and when disconnected.
    constexpr std::string_view  _sqlCreateTbInstances
    {
        "CREATE TABLE \"instances\" ("
            "\"cookie_id\"	        INTEGER NOT NULL,"
            "\"inst_connect\"       NUMERIC,"
            "\"inst_type\"	        INTEGER,"
            "\"inst_bits\"	        INTEGER,"
            "\"inst_name\"	        TEXT,"
            "\"inst_location\"	    TEXT,"
            "\"time_connected\"     NUMERIC,"
            "\"time_disconnected\"	NUMERIC,"
            "\"time_updated\"	    NUMERIC"
            ");"
    };

    //! A statement to insert a new entry in the instances table.
    //! It is called when a new log source instance is connecting to the log collector service.
    //! Each entry is a logging source process with unique cookie ID and the field indicating
    //! when the instance was connected or disconnected.
    //! The name and the location are bound, never placed in the statement: they are sent by
    //! the remote log source, see CWE-89.
    constexpr std::string_view _sqlInsertInstance
    {
        "INSERT INTO instances "
        "(cookie_id, inst_connect, inst_type, inst_bits, inst_name, inst_location, time_connected, time_updated) "
        "VALUES "
        "(?, 1, ?, ?, ?, ?, ?, ?);"
    };

    //! A statement to update an instance entry.
    //! It is called when a log source instance disconnects from log collector service
    //! of when the log observer disconnects / stops running.
    //! It updates the disconnect time for the instance with specified cookie ID.
    constexpr std::string_view _sqlUpdInstance
    {
        "UPDATE instances SET inst_connect = 0, time_disconnected = ?, time_updated = ? WHERE cookie_id = ? AND time_disconnected IS NULL;"
    };

    //! Create a table with the information about scopes of connected instances.
    //! The table has list of all scopes of the log source application
    //! The table is updated each time when scope changes the priority.
    //! The combination of cookie ID, scope ID and time when the scope is updated
    //! or registered should be unique.
    constexpr std::string_view _sqlCreateTbScopes
    {
        "CREATE TABLE \"scopes\" ("
            "\"scope_id\"	        INTEGER NOT NULL,"
            "\"cookie_id\"	        INTEGER NOT NULL,"
            "\"scope_is_active\"    INTEGER,"
            "\"scope_prio\"	        INTEGER,"
            "\"scope_name\"	        TEXT,"
            "\"time_received\"	    NUMERIC,"
            "\"time_inactivated\"	NUMERIC,"
            "CONSTRAINT \"u_scope_id\" UNIQUE(\"scope_id\", \"cookie_id\", \"time_received\")"
            ");"
    };

    //! A statement to insert a new scope in the scopes table.
    //! The scope name is bound, never placed in the statement: it is sent by the remote
    //! log source, see CWE-89.
    constexpr std::string_view _sqlInsertScope
    {
        "INSERT INTO scopes (scope_id, cookie_id, scope_is_active, scope_prio, scope_name, time_received)  VALUES (?, ?, 1, ?, ?, ?);"
    };

    //! A statement to update the scope state of a connected instance.
    //! The script will mark all scopes of specified cookie ID as inactive.
    constexpr std::string_view _sqlUpdScopes
    {
        "UPDATE scopes SET time_inactivated = ?, scope_is_active = 0 WHERE cookie_id = ? AND scope_is_active = 1;"
    };

    //! A statement to update the activation state of a single scope.
    //! The script will mark the specified scope of the specified cookie as inactive.
    constexpr std::string_view _sqlUpdScope
    {
        "UPDATE scopes SET time_inactivated = ?, scope_is_active = 0 WHERE cookie_id = ? AND scope_id = ? AND scope_is_active = 1;"
    };

    //! Create a table with logs that contain information of application cookie ID,
    //! scope ID, log priority, log message, and information like thread.
    constexpr std::string_view  _sqlCreateTbLogs
    {
        "CREATE TABLE \"logs\" ("
            "\"id\"	                INTEGER NOT NULL,"
            "\"cookie_id\"	        INTEGER,"
            "\"scope_id\"	        INTEGER,"
            "\"session_id\"         INTEGER,"
            "\"msg_type\"	        INTEGER,"
            "\"msg_prio\"	        INTEGER,"
            "\"msg_module_id\"	    INTEGER,"
            "\"msg_thread_id\"	    INTEGER,"
            "\"msg_log\"	        TEXT,"
            "\"msg_len\"	        INTEGER,"
            "\"msg_thread\"	        TEXT,"
            "\"msg_module\"	        TEXT,"
            "\"time_created\"	    NUMERIC,"
            "\"time_received\"      NUMERIC,"
            "\"time_duration\"      NUMERIC,"
            "CONSTRAINT \"pk_msg_id\" PRIMARY KEY(\"id\")"
            ");"
    };

    //! A statement to insert a new log message in the logs table.
    //! The message, the thread name and the module name are bound, never placed in the
    //! statement: they are sent by the remote log source, see CWE-89.
    //! The last parameter is the row id; left unbound it is NULL and the database chooses it.
    constexpr std::string_view _sqlInsertLog
    {
        "INSERT INTO logs "
        "(cookie_id, scope_id, session_id, msg_type, msg_prio, msg_module_id, msg_thread_id, msg_log, msg_len, msg_thread, msg_module, time_created, time_received, time_duration, id)"
        "VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    };

    //! The position of the row id in _sqlInsertLog.
    constexpr int32_t   _bindLogId  { 14 };

    constexpr std::string_view _sqlMaxLogId     { "SELECT MAX(id) FROM logs;" };

    constexpr std::string_view _sqlMaxMarkerId  { "SELECT MAX(id) FROM markers;" };

    //! A statement to delete one log message from the logs table by its identifier.
    constexpr std::string_view _sqlDeleteLog
    {
        "DELETE FROM logs WHERE id = ?;"
    };

    //! The markers table. A marker belongs to the row `master_id` of the logs table and has no
    //! place of its own; a file without the table has no markers.
    constexpr std::string_view  _sqlCreateTbMarkers
    {
        "CREATE TABLE IF NOT EXISTS markers ("
            "id             INTEGER PRIMARY KEY AUTOINCREMENT,"
            "master_id      INTEGER NOT NULL,"
            "marker_prio    INTEGER NOT NULL,"
            "author         TEXT,"
            "note           TEXT,"
            "time_set       INTEGER"
            ");"
    };

    constexpr std::string_view  _sqlCreateIdxMarkers
    {
        "CREATE INDEX IF NOT EXISTS idx_markers_master ON markers(master_id);"
    };

    constexpr std::string_view _sqlLogExists
    {
        "SELECT 1 FROM logs WHERE id = ?;"
    };

    //! The author and the note are bound, never placed in the statement, see CWE-89.
    //! The last parameter is the marker id; left unbound it is NULL and the database chooses it.
    constexpr std::string_view _sqlInsertMarker
    {
        "INSERT INTO markers (master_id, marker_prio, author, note, time_set, id) VALUES (?, ?, ?, ?, ?, ?);"
    };

    constexpr std::string_view _sqlDeleteMarker
    {
        "DELETE FROM markers WHERE id = ?;"
    };

    constexpr std::string_view _sqlDeleteMasterMarkers
    {
        "DELETE FROM markers WHERE master_id = ?;"
    };

    constexpr std::string_view _sqlReadMarkers
    {
        "SELECT id, master_id, marker_prio, time_set, author, note FROM markers ORDER BY id;"
    };

    //! The rows that have a marker of the bound levels. The two `%s` are the filter condition and
    //! the condition of one instance, each one or empty.
    constexpr std::string_view _sqlMarkedLogs
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE l.id IN (SELECT m.master_id FROM markers AS m WHERE (m.marker_prio & ?) != 0)%s%s "
        "ORDER BY l.time_created, l.id;"
    };

    constexpr std::string_view _sqlMarkedLogsCount
    {
        "SELECT COUNT(*) FROM logs AS l "
        "WHERE l.id IN (SELECT m.master_id FROM markers AS m WHERE (m.marker_prio & ?) != 0)%s%s;"
    };

    //! A row of no scope, or a row whose scope and priority the filter rules pass.
    constexpr std::string_view _sqlFilterCondition
    {
        " AND (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))"
    };

    //! Removes a log and its markers together, inside or outside an open transaction.
    constexpr std::string_view _sqlSavepointRemove  { "SAVEPOINT remove_log;" };
    constexpr std::string_view _sqlRollbackRemove   { "ROLLBACK TO remove_log;" };
    constexpr std::string_view _sqlReleaseRemove    { "RELEASE remove_log;" };

    //! The longest note of a marker in bytes.
    constexpr uint32_t  MARKER_NOTE_LENGTH  { 1023u };

    //! A script to create index of the instances table.
    constexpr std::string_view  _sqlCraeteIdxCookie
    {
        "CREATE UNIQUE INDEX idx_inst_cookies ON instances (cookie_id, time_connected, time_disconnected);"
    };

    //! A script to create index of the scopes table.
    constexpr std::string_view _sqlCreateIdxScopes
    {
        "CREATE UNIQUE INDEX idx_scope_id ON scopes (scope_id, cookie_id, time_received, time_inactivated);"
    };

    //! An index to read the logs of one instance in the order they were generated.
    constexpr std::string_view  _sqlCreateIdxLogsCookie
    {
        "CREATE INDEX idx_logs_cookie ON logs(cookie_id, time_created);"
    };

    //! An index to read the logs in the order a viewer shows them, which is the order they
    //! were generated in and not the order they arrived in. The row id follows the time so
    //! that the key stays unique: a reader that pages by time never repeats or skips a row
    //! when two logs share a timestamp.
    constexpr std::string_view  _sqlCreateIdxLogsTime
    {
        "CREATE INDEX IF NOT EXISTS idx_logs_time ON logs(time_created, id);"
    };

    //! The stretches of time a reader leaves out, one row per stretch. It lives in the
    //! temporary database, so it is dropped with the connection and a read only log file
    //! can carry it.
    constexpr std::string_view _sqlCreateTempRefused
    {
        "CREATE TEMP TABLE IF NOT EXISTS refused_spans ("
        "   scope_id      INTEGER NOT NULL DEFAULT 0,"
        "   target_id     INTEGER NOT NULL DEFAULT 0,"
        "   from_ts       INTEGER NOT NULL DEFAULT 0,"
        "   to_ts         INTEGER NOT NULL DEFAULT 0);"
    };

    constexpr std::string_view _sqlCreateIdxRefused
    {
        "CREATE INDEX IF NOT EXISTS idx_refused_spans ON refused_spans(scope_id, target_id);"
    };

    constexpr std::string_view _sqlClearRefused
    {
        "DELETE FROM refused_spans;"
    };

    constexpr std::string_view _sqlInsertRefused
    {
        "INSERT INTO refused_spans(scope_id, target_id, from_ts, to_ts) VALUES (?, ?, ?, ?);"
    };

    //! Forward from the first log, and forward from a given place.
    constexpr std::string_view _sqlWindowFirst
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "WHERE NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created, id LIMIT ?;"
    };

    constexpr std::string_view _sqlWindowAfter
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "WHERE (time_created, id) > (?, ?) AND "
        "NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created, id LIMIT ?;"
    };

    //! Backward from the last log, and backward from a given place. The rows come back
    //! newest first, so a caller that shows them in time order reverses them.
    constexpr std::string_view _sqlWindowLast
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "WHERE NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created DESC, id DESC LIMIT ?;"
    };

    constexpr std::string_view _sqlWindowBefore
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "WHERE (time_created, id) < (?, ?) AND "
        "NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created DESC, id DESC LIMIT ?;"
    };

    //! Forward from a row number.
    constexpr std::string_view _sqlWindowAt
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "WHERE NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created, id LIMIT ? OFFSET ?;"
    };

    //! Forward from a row number, when no stretch of time is left out.
    constexpr std::string_view _sqlWindowAtAll
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module, msg_len, id FROM logs "
        "ORDER BY time_created, id LIMIT ? OFFSET ?;"
    };

    //! Answers 1 if a stretch of time is left out of the reading.
    constexpr std::string_view _sqlHasRefused
    {
        "SELECT EXISTS (SELECT 1 FROM refused_spans);"
    };

    //! Filtered windows, forward from the first log and from a given place: rows of no scope and
    //! rows the filter rules pass. `%s` is the condition of one instance, or empty.
    constexpr std::string_view _sqlFilterWindowFirst
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))%s "
        "ORDER BY l.time_created, l.id LIMIT ?;"
    };

    constexpr std::string_view _sqlFilterWindowAfter
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))%s "
        "AND (l.time_created, l.id) > (?, ?) "
        "ORDER BY l.time_created, l.id LIMIT ?;"
    };

    //! Backward from the last log, and backward from a given place, newest first.
    constexpr std::string_view _sqlFilterWindowLast
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))%s "
        "ORDER BY l.time_created DESC, l.id DESC LIMIT ?;"
    };

    constexpr std::string_view _sqlFilterWindowBefore
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))%s "
        "AND (l.time_created, l.id) < (?, ?) "
        "ORDER BY l.time_created DESC, l.id DESC LIMIT ?;"
    };

    //! Filtered window forward from a row number of the filtered rows.
    constexpr std::string_view _sqlFilterWindowAt
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, "
        "l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, "
        "l.msg_log, l.msg_thread, l.msg_module, l.msg_len, l.id FROM logs AS l "
        "WHERE (l.scope_id = 0 OR EXISTS (SELECT 1 FROM filter_rules r "
        "WHERE r.scope_id = l.scope_id AND r.target_id = l.cookie_id AND (l.msg_prio & r.log_mask) != 0))%s "
        "ORDER BY l.time_created, l.id LIMIT ? OFFSET ?;"
    };

    //! The condition that keeps the rows of one instance, bound to its cookie.
    constexpr std::string_view _sqlInstanceCondition
    {
        " AND l.cookie_id = ?"
    };

    //! A script to extract the names of connected log instances
    constexpr std::string_view _sqlGetInstanceName
    {
        "SELECT inst_name FROM instances GROUP BY inst_name;"
    };

    //! A script to extract the IDs of connected log instances
    constexpr std::string_view _sqlGetInstanceIds
    {
        "SELECT cookie_id FROM instances GROUP BY cookie_id;"
    };

    //! A script to extract the names of logging threads
    constexpr std::string_view _sqlGetThreadNames
    {
        "SELECT msg_thread FROM logs GROUP BY msg_thread;"
    };

    //! A script to extract the IDs of logging threads
    constexpr std::string_view _sqlGetThreadIds
    {
        "SELECT msg_thread_id FROM logs GROUP BY msg_thread_id;"
    };

    //! A script to extract the logging instances with their information.
    constexpr std::string_view _sqlGetLogInstances
    {
        "SELECT inst_type, inst_bits, cookie_id, time_connected, inst_name, inst_location FROM instances ORDER BY time_connected;"
    };        

    //! A script to extract the logging scopes of a certain instance with their information.
    constexpr std::string_view _sqlGetLogScopes
    {
        "SELECT scope_name, scope_id, scope_prio FROM scopes WHERE cookie_id = ? ORDER BY time_received;"
    };

    //! A script to extract the logging scopes of a certain instance with their information.
    constexpr std::string_view _sqlGetAllLogScopes
    {
        "SELECT scope_name, scope_id, scope_prio FROM scopes ORDER BY time_received;"
    };

    //! A script to extract all logged messages
    constexpr std::string_view _sqlGetAllLogMessagesPaged
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module "
        "FROM logs "
        "ORDER BY time_created "
        "LIMIT ? OFFSET ?;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetInstLogMessagesPaged
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, "
        "time_created, time_received, time_duration, scope_id, session_id, "
        "msg_log, msg_thread, msg_module "
        "FROM logs "
        "WHERE cookie_id = ? "
        "ORDER BY time_created "
        "LIMIT ? OFFSET ?;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetScopeLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module, msg_len FROM logs WHERE scope_id = ? ORDER BY time_created;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetInstScopeLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module, msg_len FROM logs WHERE scope_id = ? AND cookie_id = ? ORDER BY time_created;"
    };

    constexpr std::string_view _sqlCountInstanceLogs
    {
        "SELECT COUNT(id) FROM logs WHERE cookie_id = ?;"
    };

    constexpr std::string_view _sqlCountAllLogs
    {
        "SELECT COUNT(id) FROM logs;"
    };

    constexpr std::string_view _sqlCountInstanceScopes
    {
        "SELECT COUNT(scope_id) FROM scopes WHERE cookie_id = ?;"
    };

    constexpr std::string_view _sqlCountAllScopes
    {
        "SELECT COUNT(scope_id) FROM scopes;"
    };

    constexpr std::string_view _sqlCountInstances
    {
        "SELECT COUNT(cookie_id) from instances;"
    };

    //! A statement to check whether a table exists in the main schema.
    //! The table name is bound, never placed in the statement, see CWE-89.
    constexpr std::string_view _sqlCheckTable
    {
        "SELECT name FROM sqlite_master WHERE type = 'table' AND name = ?;"
    };

    //! The same check against the temporary schema. A schema name is an SQL identifier and
    //! cannot be bound, so the two names SQLite defines are spelled out instead.
    constexpr std::string_view _sqlCheckTempTable
    {
        "SELECT name FROM sqlite_temp_master WHERE type = 'table' AND name = ?;"
    };

    constexpr std::string_view _sqlCreateTempScopes
    {
        "CREATE TEMP TABLE filter_rules ("
        "   scope_id      INTEGER NOT NULL DEFAULT 0,"
        "   target_id     INTEGER NOT NULL DEFAULT 0,"
        "   log_mask      INTEGER NOT NULL DEFAULT 1008);"
    };

    constexpr std::string_view _sqlCreateTempIndex
    {
        "CREATE INDEX idx_filter_rules ON filter_rules(scope_id, target_id);"
    };

    constexpr std::string_view _sqlInitTempScopes
    {
        "INSERT INTO filter_rules(scope_id, target_id, log_mask) SELECT scope_id, cookie_id, 1008 FROM scopes;"
    };

    constexpr std::string_view _sqlFilterScopeLogsCountAll
    {
        "SELECT "
        "(SELECT COUNT(*) FROM logs AS l WHERE l.scope_id = 0) "
        "+ "
        "(SELECT COUNT(*) FROM logs AS l JOIN filter_rules AS r ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id WHERE ((l.msg_prio & r.log_mask) != 0)) "
        "AS total_rows;"
    };

    constexpr std::string_view _sqlFilterScopeLogsCount
    {
        "SELECT COUNT(l.id)"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE (l.cookie_id = ?) AND ((l.msg_prio & r.log_mask) != 0);"
    };

    constexpr std::string_view _sqlFilterScopeLogsAll
    {
        "SELECT * FROM ("
        "   SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, l.time_created AS tc, l.time_received, l.time_duration, l.scope_id, l.session_id, l.msg_log, l.msg_thread, l.msg_module"
        "       FROM logs AS l"
        "       WHERE l.scope_id = 0 "
        "   UNION ALL "
        "   SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, l.msg_log, l.msg_thread, l.msg_module"
        "       FROM logs AS l"
        "       JOIN filter_rules AS r"
        "           ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "       WHERE (l.msg_prio & r.log_mask) != 0 "
        ") ORDER BY tc;"
    };

    constexpr std::string_view _sqlFilterScopeLogsInst
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, l.time_created AS tc, l.time_received, l.time_duration, l.scope_id, l.session_id, l.msg_log, l.msg_thread, l.msg_module"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE (l.cookie_id = ?) AND ((l.msg_prio & r.log_mask) != 0)"
        "   ORDER BY tc;"
    };


    constexpr std::string_view _sqlUpdateFilterRuleInst
    {
        "UPDATE filter_rules SET log_mask = ? WHERE target_id = %u AND scope_id = ?;"
    };

    constexpr std::string_view _sqlUpdateFilterRuleAll
    {
        "UPDATE filter_rules SET log_mask = ? WHERE scope_id = ?;"
    };

    constexpr std::string_view _sqlResetFilterScopes
    {
        "UPDATE filter_rules SET log_mask = 1008 WHERE target_id = ?;"
    };

    constexpr std::string_view _sqlResetFilterScopesAll
    {
        "UPDATE filter_rules SET log_mask = 1008;"
    };

    constexpr std::string_view _sqlDisableFilterScopes
    {
        "UPDATE filter_rules SET log_mask = 0 WHERE target_id = ?;"
    };

    constexpr std::string_view _sqlDisableFilterScopesAll
    {
        "UPDATE filter_rules SET log_mask = 0;"
    };

    //! A statement to drop a table. A table name is an SQL identifier and cannot be bound,
    //! so it is quoted with _quote_identifier() before it is placed here, see CWE-89.
    constexpr std::string_view _sqlDropTable
    {
        "DROP TABLE IF EXISTS %s;"
    };

    /**
     * \brief   Returns the given name as a quoted SQL identifier, with every embedded quote
     *          doubled, so that the result is always a single identifier. Used where a name
     *          cannot be bound as a parameter, see CWE-89.
     *
     * \param   name    The identifier to quote. Must not be empty.
     **/
    inline areg::String _quote_identifier(const char* name)
    {
        areg::String result{ "\"" };
        for (const char* src = name; *src != '\0'; ++src)
        {
            if (*src == '\"')
            {
                result += '\"';
            }

            result += *src;
        }

        result += '\"';
        return result;
    }

    /**
     * \brief   Returns the statement with its first `%s` replaced by the given condition. The
     *          condition is one of the constant texts of this file, never a value.
     **/
    inline areg::String _with_condition(std::string_view sql, std::string_view condition)
    {
        std::string result{ sql };
        const std::string::size_type pos{ result.find("%s") };
        if (pos != std::string::npos)
        {
            result.replace(pos, 2u, condition);
        }

        return areg::String(std::move(result));
    }

    /**
     * \brief   Returns the text cut to at most `maxLength` bytes, never inside a UTF-8 sequence.
     **/
    inline areg::String _cut_text(const areg::String& text, uint32_t maxLength)
    {
        uint32_t length{ static_cast<uint32_t>(text.length()) };
        if (length <= maxLength)
            return text;

        const char* data{ text.as_string() };
        length = maxLength;
        while ((length > 0u) && ((static_cast<unsigned char>(data[length]) & 0xC0u) == 0x80u))
        {
            --length;
        }

        return areg::String(data, length);
    }

    constexpr char const _master[]   { "sqlite_master" };
    constexpr char const _temp[]     { "sqlite_temp_master" };

    //! Copies the committed pages of the write-ahead log into the database file without waiting for anyone.
    constexpr std::string_view  _sqlCheckpointPassive   { "PRAGMA wal_checkpoint(PASSIVE);" };

    //! The number given to the next checkpoint thread, so that each thread has its own name.
    std::atomic<uint32_t>       _checkpointThreadSeq    { 0u };
} // namespace

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// LogDbCheckpointer class declaration and implementation.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Copies the write-ahead log of a writing connection into the database file on its own
 *          thread and its own connection, so that a commit of the writer never runs a checkpoint.
 *          It checkpoints at once when the log reaches CHECKPOINT_PAGES, and IDLE_CHECKPOINT_MS
 *          after a commit that left pages to copy. It waits without a timeout while nothing is
 *          left to copy. From REWIND_PAGES on it is not woken, and the writer copies the pages left
 *          itself, so that the log starts over from its beginning.
 **/
class LogDbCheckpointer final : private ThreadConsumer
{
public:
    //!< The size of the write-ahead log in pages that starts a checkpoint at once.
    static constexpr int        CHECKPOINT_PAGES        { 1000 };

    //!< The size of the write-ahead log in pages at which the writer copies the rest itself.
    static constexpr int        REWIND_PAGES            { 4000 };

    //!< The time in milliseconds after which the committed pages not copied yet are checkpointed.
    static constexpr uint32_t   IDLE_CHECKPOINT_MS      { 1000u };

    LogDbCheckpointer();

    ~LogDbCheckpointer() override;

    /**
     * \brief   Opens the own connection to the database file and starts the checkpoint thread.
     *
     * \param   dbPath  The path of the database file the writer opened.
     * \return  Returns true if the checkpoint thread runs.
     **/
    bool start(const String& dbPath);

    /**
     * \brief   Stops the checkpoint thread and closes the own connection.
     **/
    void stop();

    /**
     * \brief   Called on the writing thread after each commit of the writer.
     *
     * \param   walPages    The number of pages in the write-ahead log.
     **/
    inline void on_commit(int walPages) noexcept;

private:
    /**
     * \brief   The loop of the checkpoint thread.
     **/
    void on_run() override;

    /**
     * \brief   Runs one passive checkpoint and remembers whether pages are left to copy.
     **/
    inline void _checkpoint() noexcept;

private:
    //!< The own connection to the database file.
    SqliteDatabase      mDatabase;

    //!< The prepared checkpoint statement of the own connection.
    SqliteStatement     mStatement;

    //!< The checkpoint thread.
    Thread              mThread;

    //!< Wakes the checkpoint thread.
    SyncEvent           mWakeEvent;

    //!< Set to stop the checkpoint thread.
    std::atomic_bool    mQuit;

    //!< True while the write-ahead log holds committed pages not copied yet.
    std::atomic_bool    mPending;

    //!< True when the write-ahead log reached CHECKPOINT_PAGES since the last checkpoint.
    std::atomic_bool    mRequested;
};

LogDbCheckpointer::LogDbCheckpointer()
    : ThreadConsumer( )
    , mDatabase     ( )
    , mStatement    ( mDatabase )
    , mThread       ( static_cast<ThreadConsumer &>(*this)
                    , String("LogDbCheckpoint_") + String::make_string(_checkpointThreadSeq.fetch_add(1u, std::memory_order_relaxed))
                    , areg::SYSTEM_THREAD_STACK_NORMAL )
    , mWakeEvent    ( true, true )
    , mQuit         ( false )
    , mPending      ( false )
    , mRequested    ( false )
{
}

LogDbCheckpointer::~LogDbCheckpointer()
{
    stop();
}

bool LogDbCheckpointer::start(const String& dbPath)
{
    if (mDatabase.connect(dbPath, areg::DatabaseRole::Viewer) == false)
        return false;

    mQuit.store(false, std::memory_order_relaxed);
    mPending.store(false, std::memory_order_relaxed);
    mRequested.store(false, std::memory_order_relaxed);
    if (mStatement.prepare(_sqlCheckpointPassive) && mThread.start(areg::WAIT_INFINITE))
        return true;

    mStatement.finalize();
    mDatabase.disconnect();
    return false;
}

void LogDbCheckpointer::stop()
{
    mQuit.store(true, std::memory_order_release);
    mWakeEvent.set_signaled();
    mThread.shutdown(areg::WAIT_INFINITE);
    mStatement.finalize();
    mDatabase.disconnect();
}

inline void LogDbCheckpointer::on_commit(int walPages) noexcept
{
    if (walPages >= LogDbCheckpointer::REWIND_PAGES)
        return;

    const bool wasPending{ mPending.exchange(true, std::memory_order_acq_rel) };
    if ((walPages >= LogDbCheckpointer::CHECKPOINT_PAGES) && (mRequested.exchange(true, std::memory_order_acq_rel) == false))
    {
        mWakeEvent.set_signaled();
    }
    else if (wasPending == false)
    {
        mWakeEvent.set_signaled();
    }
}

void LogDbCheckpointer::on_run()
{
    while (mQuit.load(std::memory_order_acquire) == false)
    {
        const uint32_t timeout{ mPending.load(std::memory_order_acquire) ? LogDbCheckpointer::IDLE_CHECKPOINT_MS : areg::WAIT_INFINITE };
        const bool signaled{ mWakeEvent.lock(timeout) };
        if (mQuit.load(std::memory_order_acquire))
            break;

        if (mRequested.load(std::memory_order_acquire) || ((signaled == false) && mPending.load(std::memory_order_acquire)))
        {
            _checkpoint();
        }
    }
}

inline void LogDbCheckpointer::_checkpoint() noexcept
{
    mRequested.store(false, std::memory_order_release);
    mPending.store(false, std::memory_order_release);

    int64_t logPages{ 0 };
    int64_t copiedPages{ 0 };
    const bool done{ mStatement.next() == SqliteStatement::QueryResult::HasMore };
    if (done)
    {
        logPages    = mStatement.as_int64(1);
        copiedPages = mStatement.as_int64(2);
    }

    mStatement.reset();
    if ((done == false) || (logPages > copiedPages))
    {
        mPending.store(true, std::memory_order_release);
    }
}

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class implementation.
//////////////////////////////////////////////////////////////////////////

String LogSqliteDatabase::read_scopes_query()
{
    return String(_sqlGetScopeLogMessages);
}

String LogSqliteDatabase::read_instances_query()
{
    return String(_sqlGetInstScopeLogMessages);
}

String LogSqliteDatabase::read_all_log_messages_query()
{
    return String(_sqlGetAllLogMessagesPaged);
}

LogSqliteDatabase::LogSqliteDatabase()
    : LogDatabaseEngine   ( )

    , mDatabase             ( )
    , mStmtLogs             (mDatabase)
    , mDbInitPath           ( )
    , mIsInitialized        ( false )
    , mDbLogEnabled         ( true )
    , mLock                 ( false )
    , mCheckpointer         ( )
{
}

LogSqliteDatabase::~LogSqliteDatabase()
{
    disconnect();
    _stop_checkpointer();
    mStmtLogs.finalize();
    mDatabase.disconnect();
    mIsInitialized = false;
}

inline void LogSqliteDatabase::_start_checkpointer()
{
    ASSERT(mCheckpointer == nullptr);
    std::unique_ptr<LogDbCheckpointer> checkpointer{ new (std::nothrow) LogDbCheckpointer() };
    if ((checkpointer != nullptr) && checkpointer->start(mDatabase.path()))
    {
        mCheckpointer = std::move(checkpointer);
        ::sqlite3_wal_hook(static_cast<sqlite3 *>(mDatabase.mDbObject), &LogSqliteDatabase::_on_wal_commit, mCheckpointer.get());
    }
}

inline void LogSqliteDatabase::_stop_checkpointer()
{
    if (mCheckpointer == nullptr)
        return;

    if (mDatabase.mDbObject != nullptr)
    {
        ::sqlite3_wal_hook(static_cast<sqlite3 *>(mDatabase.mDbObject), nullptr, nullptr);
    }

    mCheckpointer->stop();
    mCheckpointer.reset();
}

int LogSqliteDatabase::_on_wal_commit(void* context, ::sqlite3* db, const char* dbName, int walPages)
{
    if (walPages >= LogDbCheckpointer::REWIND_PAGES)
    {
        ::sqlite3_wal_checkpoint_v2(db, dbName, SQLITE_CHECKPOINT_PASSIVE, nullptr, nullptr);
    }

    if (context != nullptr)
    {
        static_cast<LogDbCheckpointer *>(context)->on_commit(walPages);
    }

    return SQLITE_OK;
}

inline bool LogSqliteDatabase::_open(const String& dbPath, areg::DatabaseRole role)
{
    if (mDbLogEnabled == false)
        return false;

    bool result{ true };
    _stop_checkpointer();
    mDatabase.disconnect();
    mIsInitialized = false;
    if (dbPath.is_empty() == false)
    {
        mDbInitPath = dbPath;
    }

    if (mDatabase.connect(mDbInitPath, role) == false)
    {
        mDatabase.disconnect();
        mIsInitialized = false;
        result = false;
    }
    else if (role == areg::DatabaseRole::Recorder)
    {
        mDatabase.execute(_sqlWriteMode);
        _start_checkpointer();
    }

    return result;
}

inline void LogSqliteDatabase::_create_tables() noexcept
{
    VERIFY(mDatabase.execute(_sqlCreateTbVersion));
    VERIFY(mDatabase.execute(_sqlCreateTbInstances));
    VERIFY(mDatabase.execute(_sqlCreateTbScopes));
    VERIFY(mDatabase.execute(_sqlCreateTbLogs));
    VERIFY(mDatabase.execute(_sqlCreateTbMarkers));
}

inline void LogSqliteDatabase::_create_indexes() noexcept
{
    VERIFY(mDatabase.execute(_sqlCraeteIdxCookie));
    VERIFY(mDatabase.execute(_sqlCreateIdxScopes));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogsCookie));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogsTime));
    VERIFY(mDatabase.execute(_sqlCreateIdxMarkers));
}

inline void LogSqliteDatabase::_initialize() noexcept
{
    Process& proc{ Process::instance() };
    DateTime now{ DateTime::now() };
    String module{ proc.app_name() };
    id_type threadId{ Thread::current_thread_id() };
    String thread{ Thread::thread_name(threadId) };

    {
        SqliteStatement stmt(mDatabase, _sqlInsertVersion);
        stmt.bind_text(  0, proc.name());
        stmt.bind_text(  1, String(areg::LOG_VERSION));
        stmt.bind_text(  2, String("Areg SDK database logging module. Visit https://areg.tech for more information."));
        stmt.bind_text(  3, String("Created by Areg log observer API module."));
        stmt.bind_text(  4, mDatabase.path());
        stmt.bind_uint64(5, static_cast<uint64_t>(now.time()));
        VERIFY(stmt.execute());
    }

    {
        // The header row. It carries no scope and the PrioIgnore bit, which is how a
        // reader separates it from an application log message.
        const String message{ "Starting database logging..." };
        SqliteStatement stmt(mDatabase, _sqlInsertLog);
        stmt.bind_uint32( 0, static_cast<uint32_t>(areg::COOKIE_LOCAL));
        stmt.bind_uint32( 1, static_cast<uint32_t>(areg::LOG_SCOPE_ID_NONE));
        stmt.bind_uint32( 2, static_cast<uint32_t>(0u));
        stmt.bind_uint32( 3, static_cast<uint32_t>(areg::LogMessageType::MessageText));
        stmt.bind_uint32( 4, static_cast<uint32_t>(areg::LogPriority::PrioIgnore));
        stmt.bind_uint64( 5, static_cast<uint64_t>(proc.id()));
        stmt.bind_uint64( 6, static_cast<uint64_t>(threadId));
        stmt.bind_text(   7, message);
        stmt.bind_uint32( 8, static_cast<uint32_t>(message.length()));
        stmt.bind_text(   9, thread);
        stmt.bind_text(  10, module);
        stmt.bind_uint64(11, static_cast<uint64_t>(now.time()));
        stmt.bind_uint64(12, static_cast<uint64_t>(now.time()));
        stmt.bind_uint32(13, static_cast<uint32_t>(0u));
        VERIFY(stmt.execute());
    }
}

inline void LogSqliteDatabase::_copy_log_message(SqliteStatement& stmt, SharedBuffer& buf)
{
    constexpr uint32_t _logSize{ static_cast<uint32_t>(sizeof(areg::LogEntry)) };
    buf.reserve(_logSize, false);
    buf.set_size_used(_logSize);
    buf.move_to_begin();
    areg::LogEntry* log = reinterpret_cast<areg::LogEntry*>(buf.buffer());

    ASSERT(log != nullptr);
    log->logDataType = areg::LogDataType::Remote;
    log->logSource = areg::COOKIE_ANY;
    log->logTarget = areg::COOKIE_ANY;

    log->logMsgType     = static_cast<areg::LogMessageType>(stmt.as_uint32(0));
    log->logMessagePrio = static_cast<areg::LogPriority>(stmt.as_uint32(1));
    log->logCookie      = static_cast<ITEM_ID>( stmt.as_uint32(2));
    log->logModuleId    = static_cast<ITEM_ID>( stmt.as_uint32(3));
    log->logThreadId    = static_cast<ITEM_ID>( stmt.as_uint32(4));
    log->logTimestamp   = static_cast<TIME64>(  stmt.as_uint64(5));
    log->logReceived    = static_cast<TIME64>(  stmt.as_uint64(6));
    log->logDuration    = static_cast<uint32_t>(stmt.as_uint32(7));
    log->logScopeId     = static_cast<uint32_t>(stmt.as_uint32(8));
    log->logSessionId   = static_cast<uint32_t>(stmt.as_uint32(9));
    String msg          = stmt.as_text(10);
    String thread       = stmt.as_text(11);
    String module       = stmt.as_text(12);

    const uint32_t copied{ areg::mem_copy(log->logMessage, areg::LOG_MSG_SIZE - 1u, msg.as_string(), static_cast<uint32_t>(msg.length())) };
    log->logThreadLen   = areg::mem_copy(log->logThread, areg::LOG_NAME_SIZE - 1u, thread.as_string(), static_cast<uint32_t>(thread.length()));
    log->logModuleLen   = areg::mem_copy(log->logModule, areg::LOG_NAME_SIZE - 1u, module.as_string(), static_cast<uint32_t>(module.length()));
    log->logMessage[copied] = String::EmptyChar;
    log->logThread[log->logThreadLen] = String::EmptyChar;
    log->logModule[log->logModuleLen] = String::EmptyChar;

    // msg_len holds the length before the cut; it is kept only when the stored text was cut.
    const uint32_t storedLen{ static_cast<uint32_t>(stmt.as_uint32(13)) };
    log->logMessageLen  = ((copied == areg::LOG_MSG_SIZE - 1u) && (storedLen > copied)) ? storedLen : copied;

    buf.set_size_used(areg::log_entry_size(*log));
}

inline void LogSqliteDatabase::_copy_log_instances(SqliteStatement& stmt, areg::ConnectedInstance& inst)
{
    inst.ciSource   = static_cast<areg::MessageSource>(  stmt.as_uint32(0));
    inst.ciBitness  = static_cast<areg::InstanceBitness>(stmt.as_uint32(1));
    inst.ciCookie   = static_cast<ITEM_ID>(stmt.as_int64(2));
    inst.ciTimestamp= static_cast<TIME64>( stmt.as_int64(3));
    inst.ciInstance = stmt.as_text(4);
    inst.ciLocation = stmt.as_text(5);
}

inline void LogSqliteDatabase::_copy_log_scopes(SqliteStatement& stmt, areg::ScopeEntry& scope)
{
    scope.scopeName = stmt.as_text(0);
    scope.scopeId   = static_cast<uint32_t>(stmt.as_uint32(1));
    scope.scopePrio = static_cast<uint32_t>(stmt.as_uint32(2));
}

bool LogSqliteDatabase::is_operable() const noexcept
{
    return mDatabase.is_operable();
}

bool LogSqliteDatabase::connect(const String& dbPath, areg::DatabaseRole role)
{
    Lock lock(mLock);
    if (mDbLogEnabled && mDatabase.is_operable() == false)
    {
        bool exists = File::has_file(dbPath);
        ASSERT(mIsInitialized == false);
        if (_open(dbPath, role))
        {
            if (exists == false)
            {
                _create_tables();
                _create_indexes();
                _initialize();
                commit(true);
            }

            mIsInitialized = true;
            // The temporary table always exists, so that a window read can be prepared before
            // anything is refused. It costs nothing while it is empty.
            _create_temp_refused();
            if (role == areg::DatabaseRole::Recorder)
            {
                mStmtLogs.prepare(_sqlInsertLog);
            }
        }
    }

    return mIsInitialized;
}

void LogSqliteDatabase::disconnect()
{
    Lock lock(mLock);
    _stop_checkpointer();
    if ((mDatabase.is_operable() == false) || (mIsInitialized == false))
        return;

    // Only a connection opened to write has the insert statement.
    const bool writes{ mStmtLogs.is_valid() };
    if (writes)
        mStmtLogs.finalize();

    drop_table("filter_rules");
    mDatabase.commit(true);
    if (writes)
    {
        mDatabase.execute(_sqlCloseNoWait);
        mDatabase.execute(_sqlCloseCheckpoint);
        mDatabase.execute(_sqlClosePlainFile);
    }

    mDatabase.disconnect();
    mIsInitialized = false;
}

bool LogSqliteDatabase::execute(const String& sql)
{
    Lock lock(mLock);
    return mDatabase.execute(sql);
}

bool LogSqliteDatabase::begin()
{
    Lock lock(mLock);
    return mDatabase.begin();
}

bool LogSqliteDatabase::commit(bool doCommit)
{
    Lock lock(mLock);
    return mDatabase.commit(doCommit);
}

bool LogSqliteDatabase::tables_initialized() const noexcept
{
    return mIsInitialized;
}

bool LogSqliteDatabase::log_message(const areg::LogEntry& message)
{
    return log_message(message, 0u);
}

bool LogSqliteDatabase::log_message(const areg::LogEntry& message, uint32_t logId)
{
    Lock lock(mLock);
    if (mStmtLogs.is_valid() == false)
    {
        return false;
    }

    mStmtLogs.bind_uint32( 0, static_cast<uint32_t>(message.logCookie));
    mStmtLogs.bind_uint32( 1, static_cast<uint32_t>(message.logScopeId));
    mStmtLogs.bind_uint32( 2, static_cast<uint32_t>(message.logSessionId));
    mStmtLogs.bind_uint32( 3, static_cast<uint32_t>(message.logMsgType));
    mStmtLogs.bind_uint32( 4, static_cast<uint32_t>(message.logMessagePrio));
    mStmtLogs.bind_uint32( 5, static_cast<uint32_t>(message.logModuleId));
    mStmtLogs.bind_uint32( 6, static_cast<uint32_t>(message.logThreadId));
    mStmtLogs.bind_text(   7, message.logMessage);
    // the length before the cut, so a reader can tell how much the text is missing
    mStmtLogs.bind_uint32( 8, message.logMessageLen);
    mStmtLogs.bind_text(   9, message.logThread);
    mStmtLogs.bind_text(  10, message.logModule);
    mStmtLogs.bind_uint64(11, static_cast<uint64_t>(message.logTimestamp));
    mStmtLogs.bind_uint64(12, static_cast<uint64_t>(message.logReceived));
    mStmtLogs.bind_uint32(13, static_cast<uint32_t>(message.logDuration));
    if (logId != 0u)
    {
        mStmtLogs.bind_uint32(_bindLogId, logId);
    }

    bool result{ mStmtLogs.next() == SqliteStatement::QueryResult::HasNoMore };
    mStmtLogs.reset();
    mStmtLogs.clear_bindings();
    return result;
}

uint32_t LogSqliteDatabase::last_log_id() const noexcept
{
    return mDatabase.last_inserted_id();
}

bool LogSqliteDatabase::log_dropped_messages(uint32_t dropped, uint32_t logId)
{
    Process& proc{ Process::instance() };
    const String& module{ proc.app_name() };
    const TIME64 now{ static_cast<TIME64>(DateTime::now()) };

    areg::LogEntry entry(areg::LogMessageType::MessageText);
    entry.logDataType    = areg::LogDataType::Local;
    entry.logMessagePrio = areg::LogPriority::PrioError;
    entry.logCookie      = areg::COOKIE_LOCAL;
    entry.logModuleId    = static_cast<ITEM_ID>(proc.id());
    entry.logTimestamp   = now;
    entry.logReceived    = now;
    entry.logMessageLen  = static_cast<uint32_t>(String::format_string( entry.logMessage
                                                                     , areg::LOG_MSG_SIZE
                                                                     , "Dropped [ %u ] log messages, the logging database cannot keep up with the incoming data rate."
                                                                     , dropped));
    entry.logModuleLen   = areg::mem_copy(entry.logModule, areg::LOG_NAME_SIZE - 1u, module.as_string(), static_cast<uint32_t>(module.length()));
    entry.logModule[entry.logModuleLen] = String::EmptyChar;

    return log_message(entry, logId);
}

uint32_t LogSqliteDatabase::max_log_id()
{
    Lock lock(mLock);
    return _max_id(_sqlMaxLogId);
}

uint32_t LogSqliteDatabase::max_marker_id()
{
    Lock lock(mLock);
    return (table_exists("markers") ? _max_id(_sqlMaxMarkerId) : 0u);
}

inline uint32_t LogSqliteDatabase::_max_id(std::string_view sql)
{
    if (mDatabase.is_operable() == false)
        return 0u;

    SqliteStatement stmt(mDatabase, sql);
    return (stmt.next() == SqliteStatement::QueryResult::HasMore ? stmt.as_uint32(0) : 0u);
}

bool LogSqliteDatabase::remove_log(uint32_t logId)
{
    Lock lock(mLock);
    if ((logId == 0u) || (mDatabase.is_operable() == false))
    {
        return false;
    }

    const bool hasMarkers{ table_exists("markers") };
    if (mDatabase.execute(_sqlSavepointRemove) == false)
    {
        return false;
    }

    bool result{ true };
    if (hasMarkers)
    {
        SqliteStatement stmt(mDatabase, _sqlDeleteMasterMarkers);
        result = stmt.bind_uint32(0, logId) && (stmt.next() == SqliteStatement::QueryResult::HasNoMore);
    }

    if (result)
    {
        SqliteStatement stmt(mDatabase, _sqlDeleteLog);
        result = stmt.bind_uint32(0, logId) && (stmt.next() == SqliteStatement::QueryResult::HasNoMore);
    }

    if (result == false)
    {
        mDatabase.execute(_sqlRollbackRemove);
    }

    mDatabase.execute(_sqlReleaseRemove);
    return result;
}

uint32_t LogSqliteDatabase::add_marker(uint32_t masterId, areg::LogPriority level, const String& author, const String& note)
{
    Lock lock(mLock);
    return _insert_marker(0u, masterId, level, author, note, static_cast<TIME64>(DateTime::now().time()));
}

bool LogSqliteDatabase::insert_marker(uint32_t markerId, uint32_t masterId, areg::LogPriority level, const String& author, const String& note, TIME64 timeSet)
{
    Lock lock(mLock);
    return ((markerId != 0u) && (_insert_marker(markerId, masterId, level, author, note, timeSet) != 0u));
}

inline uint32_t LogSqliteDatabase::_insert_marker(uint32_t markerId, uint32_t masterId, areg::LogPriority level, const String& author, const String& note, TIME64 timeSet)
{
    if ((masterId == 0u) || (LogSqliteDatabase::is_marker_level(level) == false) || (mDatabase.is_operable() == false))
    {
        return 0u;
    }

    {
        SqliteStatement stmt(mDatabase, _sqlLogExists);
        if ((stmt.bind_uint32(0, masterId) == false) || (stmt.next() != SqliteStatement::QueryResult::HasMore))
        {
            return 0u;
        }
    }

    if ((mDatabase.execute(_sqlCreateTbMarkers) == false) || (mDatabase.execute(_sqlCreateIdxMarkers) == false))
    {
        return 0u;
    }

    SqliteStatement stmt(mDatabase, _sqlInsertMarker);
    const bool result = stmt.bind_uint32(0, masterId)
                     && stmt.bind_uint32(1, static_cast<uint32_t>(level))
                     && stmt.bind_text(  2, _cut_text(author, areg::LOG_NAME_SIZE - 1u))
                     && stmt.bind_text(  3, _cut_text(note, MARKER_NOTE_LENGTH))
                     && stmt.bind_uint64(4, static_cast<uint64_t>(timeSet))
                     && ((markerId == 0u) || stmt.bind_uint32(5, markerId))
                     && stmt.execute();

    return (result ? mDatabase.last_inserted_id() : 0u);
}

bool LogSqliteDatabase::remove_marker(uint32_t markerId)
{
    Lock lock(mLock);
    if ((markerId == 0u) || (mDatabase.is_operable() == false) || (table_exists("markers") == false))
    {
        return false;
    }

    SqliteStatement stmt(mDatabase, _sqlDeleteMarker);
    return stmt.bind_uint32(0, markerId) && stmt.execute();
}

uint32_t LogSqliteDatabase::read_markers(std::vector<LogMarker>& markers)
{
    Lock lock(mLock);
    markers.clear();
    if ((mDatabase.is_operable() == false) || (table_exists("markers") == false))
    {
        return 0u;
    }

    SqliteStatement stmt(mDatabase, _sqlReadMarkers);
    while (stmt.next() == SqliteStatement::QueryResult::HasMore)
    {
        LogMarker marker;
        marker.markerId = stmt.as_uint32(0);
        marker.masterId = stmt.as_uint32(1);
        marker.level    = static_cast<areg::LogPriority>(stmt.as_uint32(2));
        marker.timeSet  = static_cast<TIME64>(stmt.as_uint64(3));
        marker.author   = stmt.as_text(4);
        marker.note     = stmt.as_text(5);
        markers.push_back(marker);
    }

    return static_cast<uint32_t>(markers.size());
}

uint32_t LogSqliteDatabase::setup_statement_read_marked_logs(SqliteStatement& stmt, ITEM_ID instId, uint16_t levelMask)
{
    Lock lock(mLock);
    stmt.finalize();
    if ((mDatabase.is_operable() == false) || (table_exists("markers") == false))
    {
        return 0u;
    }

    const bool oneInstance{ instId != areg::TARGET_ALL };
    const std::string_view filter{ table_exists("filter_rules", _temp) ? _sqlFilterCondition : std::string_view{ } };
    const std::string_view instance{ oneInstance ? _sqlInstanceCondition : std::string_view{ } };

    uint32_t result{ 0u };
    {
        SqliteStatement count(mDatabase, _with_condition(_with_condition(_sqlMarkedLogsCount, filter).as_string(), instance));
        if (count.bind_uint32(0, levelMask) && ((oneInstance == false) || count.bind_uint32(1, static_cast<uint32_t>(instId))) &&
            (count.next() == SqliteStatement::QueryResult::HasMore))
        {
            result = count.as_uint32(0);
        }
    }

    if ((stmt.prepare(_with_condition(_with_condition(_sqlMarkedLogs, filter).as_string(), instance)) == false) ||
        (stmt.bind_uint32(0, levelMask) == false) ||
        (oneInstance && (stmt.bind_uint32(1, static_cast<uint32_t>(instId)) == false)))
    {
        stmt.finalize();
        result = 0u;
    }

    return result;
}

bool LogSqliteDatabase::log_instance_connected(const areg::ConnectedInstance& instance, const DateTime& timestamp)
{
    Lock lock(mLock);
    Process& proc    { Process::instance() };
    String   module  { proc.app_name() };
    id_type  threadId{ Thread::current_thread_id() };
    String   thread  { Thread::thread_name(threadId) };

    SqliteStatement stmt(mDatabase, _sqlInsertInstance);
    stmt.bind_uint32(0, static_cast<uint32_t>(instance.ciCookie));
    stmt.bind_uint32(1, static_cast<uint32_t>(instance.ciSource));
    stmt.bind_uint32(2, static_cast<uint32_t>(instance.ciBitness));
    stmt.bind_text(  3, String(instance.ciInstance));
    stmt.bind_text(  4, String(instance.ciLocation));
    stmt.bind_uint64(5, static_cast<uint64_t>(instance.ciTimestamp));
    stmt.bind_uint64(6, static_cast<uint64_t>(timestamp.time()));
    return stmt.execute();
}

bool LogSqliteDatabase::log_instance_disconnected(const ITEM_ID& cookie, const DateTime& timestamp)
{
    Lock lock(mLock);
    log_scopes_deactivate(cookie, timestamp);

    Process& proc{ Process::instance() };
    String module{ proc.app_name() };
    id_type threadId{ Thread::current_thread_id() };
    String thread{ Thread::thread_name(threadId) };

    SqliteStatement stmt(mDatabase, _sqlUpdInstance);
    stmt.bind_uint64(0, static_cast<uint64_t>(timestamp.time()));
    stmt.bind_uint64(1, static_cast<uint64_t>(DateTime::now().time()));
    stmt.bind_uint32(2, static_cast<uint32_t>(cookie));
    return stmt.execute();
}

bool LogSqliteDatabase::log_scope_activate(const areg::ScopeEntry & scope, const ITEM_ID& cookie, const DateTime& timestamp)
{
    return log_scope_activate(scope.scopeName, scope.scopeId, scope.scopePrio, cookie, timestamp);
}

uint32_t LogSqliteDatabase::log_scopes_activate(const areg::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp)
{
    Lock lock(mLock);
    uint32_t result{ 0 };
    SqliteStatement stmt(mDatabase, _sqlInsertScope);
    for (const auto& scope : scopes.data())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(scope.scopeId));
        stmt.bind_uint32(1, static_cast<uint32_t>(cookie));
        stmt.bind_uint32(2, static_cast<uint32_t>(scope.scopePrio));
        stmt.bind_text(  3, scope.scopeName.as_string());
        stmt.bind_uint64(4, static_cast<uint64_t>(timestamp.time()));
        result += stmt.next() == SqliteStatement::QueryResult::HasMore ? 1 : 0;
        stmt.reset();
        stmt.clear_bindings();
    }

    return result;
}

bool LogSqliteDatabase::log_scope_activate(const String& scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID& cookie, const DateTime& timestamp)
{
    SqliteStatement stmt(mDatabase, _sqlInsertScope);
    stmt.bind_uint32(0, static_cast<uint32_t>(scopeId));
    stmt.bind_uint32(1, static_cast<uint32_t>(cookie));
    stmt.bind_uint32(2, static_cast<uint32_t>(scopePrio));
    stmt.bind_text(  3, scopeName);
    stmt.bind_uint64(4, static_cast<uint64_t>(timestamp.time()));
    return stmt.execute();
}

bool LogSqliteDatabase::log_scopes_deactivate(const ITEM_ID& cookie, const DateTime& timestamp)
{
    SqliteStatement stmt(mDatabase, _sqlUpdScopes);
    stmt.bind_uint64(0, static_cast<uint64_t>(timestamp.time()));
    stmt.bind_uint32(1, static_cast<uint32_t>(cookie));
    return stmt.execute();
}

bool LogSqliteDatabase::log_scope_deactivate(const ITEM_ID& cookie, uint32_t scopeId, const DateTime& timestamp)
{
    SqliteStatement stmt(mDatabase, _sqlUpdScope);
    stmt.bind_uint64(0, static_cast<uint64_t>(timestamp.time()));
    stmt.bind_uint32(1, static_cast<uint32_t>(cookie));
    stmt.bind_uint32(2, static_cast<uint32_t>(scopeId));
    return stmt.execute();
}

bool LogSqliteDatabase::rollback()
{
    Lock lock(mLock);
    return mDatabase.rollback();
}

std::vector<String> LogSqliteDatabase::log_instance_names()
{
    std::vector<String> result;
    log_instance_names(result);
    return result;
}

void LogSqliteDatabase::log_instance_names(std::vector<String>& names)
{
    Lock lock(mLock);
    names.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstanceName);
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            String instName{ stmt.as_text(0) };
            if (instName.is_empty() == false)
            {
                names.push_back(instName);
            }
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(names.size()));
}

std::vector<ITEM_ID> LogSqliteDatabase::log_instances()
{
    std::vector<ITEM_ID> result;
    log_instances(result);
    return result;
}

void LogSqliteDatabase::log_instances(std::vector<ITEM_ID>& ids)
{
    Lock lock(mLock);
    ids.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstanceIds);
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            ITEM_ID instId{ static_cast<ITEM_ID>(stmt.as_int64(0)) };
            ids.push_back(instId);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(ids.size()));
}

std::vector<String> LogSqliteDatabase::log_thread_names()
{
    std::vector<String> result;
    log_thread_names(result);
    return result;
}

void LogSqliteDatabase::log_thread_names(std::vector<String>& names)
{
    Lock lock(mLock);
    names.clear();
    SqliteStatement stmt(mDatabase, _sqlGetThreadNames);
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            String instName{ stmt.as_text(0) };
            names.push_back(instName);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(names.size()));
}

std::vector<ITEM_ID> LogSqliteDatabase::log_threads()
{
    std::vector<ITEM_ID> result;
    log_threads(result);
    return result;
}

void LogSqliteDatabase::log_threads(std::vector<ITEM_ID>& ids)
{
    Lock lock(mLock);
    ids.clear();
    SqliteStatement stmt(mDatabase, _sqlGetThreadIds);
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            ITEM_ID instId{ static_cast<ITEM_ID>(stmt.as_int64(0)) };
            ids.push_back(instId);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(ids.size()));
}

std::vector<String> LogSqliteDatabase::log_priority_names()
{
    std::vector<String> result{
        { 
          areg::priority_to_string(areg::LogPriority::PrioAny)
        , areg::priority_to_string(areg::LogPriority::PrioScope)
        , areg::priority_to_string(areg::LogPriority::PrioDebug)
        , areg::priority_to_string(areg::LogPriority::PrioInfo)
        , areg::priority_to_string(areg::LogPriority::PrioWarning)
        , areg::priority_to_string(areg::LogPriority::PrioError)
        , areg::priority_to_string(areg::LogPriority::PrioFatal)
        }
    };
    return result;
}

void LogSqliteDatabase::log_priority_names(std::vector<String>& names)
{
    names = log_priority_names();
}

std::vector<areg::ConnectedInstance> LogSqliteDatabase::log_instance_infos()
{
    std::vector<areg::ConnectedInstance> result;
    log_instance_infos(result);
    return result;
}

void LogSqliteDatabase::log_instance_infos(std::vector<areg::ConnectedInstance>& infos)
{
    Lock lock(mLock);
    infos.clear();
    SqliteStatement stmt(mDatabase, _sqlGetLogInstances);
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            areg::ConnectedInstance inst;
            _copy_log_instances(stmt, inst);
            infos.push_back(inst);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(infos.size()));
}

std::vector<areg::ScopeEntry> LogSqliteDatabase::log_inst_scopes(ITEM_ID instId)
{
    std::vector<areg::ScopeEntry> result;
    log_inst_scopes(result, instId);
    return result;
}

void LogSqliteDatabase::log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId)
{
    Lock lock(mLock);
    scopes.clear();
    SqliteStatement stmt(mDatabase, _sqlGetLogScopes);
    if (stmt.is_valid())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(instId));
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            areg::ScopeEntry scope;
            _copy_log_scopes(stmt, scope);
            scopes.push_back(scope);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(scopes.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::log_messages()
{
    std::vector<SharedBuffer> result;
    log_messages(result);
    return result;
}

void LogSqliteDatabase::log_messages(std::vector<SharedBuffer>& messages)
{
    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetAllLogMessagesPaged);
    if (stmt.is_valid())
    {
        stmt.bind_int32(0, -1);
        stmt.bind_uint32(1, 0u);
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer buf;
            _copy_log_message(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::log_inst_messages(ITEM_ID instId)
{
    std::vector<SharedBuffer> result;
    log_inst_messages(result, instId);
    return result;
}

void LogSqliteDatabase::log_inst_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId)
{
    if (instId == areg::COOKIE_ANY)
    {
        log_messages(messages);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstLogMessagesPaged);
    if (stmt.is_valid())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(instId));
        stmt.bind_int32(1, -1);
        stmt.bind_uint32(2, 0u);
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer buf;
            _copy_log_message(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::log_scope_messages(uint32_t scopeId)
{
    std::vector<SharedBuffer> result;
    log_scope_messages(result, scopeId);
    return result;
}

void LogSqliteDatabase::log_scope_messages(std::vector<SharedBuffer>& messages, uint32_t scopeId)
{
    if (scopeId == 0)
    {
        log_messages(messages);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetScopeLogMessages);
    if (stmt.is_valid())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(scopeId));
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer buf;
            _copy_log_message(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::log_messages(ITEM_ID instId, uint32_t scopeId)
{
    if (instId == areg::COOKIE_ANY)
    {
        return (scopeId == 0 ? log_messages() : log_scope_messages(scopeId));
    }
    else if (scopeId == 0)
    {
        return log_inst_messages(instId);
    }

    Lock lock(mLock);
    std::vector<SharedBuffer> result;
    SqliteStatement stmt(mDatabase, _sqlGetInstScopeLogMessages);
    if (stmt.is_valid())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(scopeId));
        stmt.bind_uint32(1, static_cast<uint32_t>(instId));
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer buf;
            _copy_log_message(stmt, buf);
            result.push_back(buf);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(result.size()));
    return result;
}

void LogSqliteDatabase::log_messages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId)
{
    if (instId == areg::COOKIE_ANY)
    {
        if (scopeId == 0)
            log_messages(messages);
        else
            log_scope_messages(messages, scopeId);
        return;
    }
    else if (scopeId == 0)
    {
        log_inst_messages(messages, instId);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstScopeLogMessages);
    if (stmt.is_valid())
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(scopeId));
        stmt.bind_uint32(1, static_cast<uint32_t>(instId));
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer buf;
            _copy_log_message(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.row_pos() == static_cast<uint32_t>(messages.size()));
}

int32_t LogSqliteDatabase::log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, SqliteStatement& stmt, int32_t maxEntries /*= -1*/)
{
    int32_t result{ 0 };
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            areg::ScopeEntry scope;
            _copy_log_scopes(stmt, scope);
            scopes.push_back(scope);
            ++ result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int32_t LogSqliteDatabase::log_messages(std::vector<SharedBuffer>& logs, SqliteStatement& stmt, int32_t maxEntries /*= -1*/)
{
    int32_t result{ 0 };
    if (stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            SharedBuffer log;
            _copy_log_message(stmt, log);
            logs.push_back(log);
            ++ result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int32_t LogSqliteDatabase::fill_log_instances(std::vector<areg::ConnectedInstance>& infos, SqliteStatement& stmt)
{
    int32_t result{ 0 };
    if ((static_cast<uint32_t>(infos.size()) != 0) && stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(infos.size()) > static_cast<uint32_t>(result));
            areg::ConnectedInstance& inst{ infos[static_cast<size_t>(result)] };
            _copy_log_instances(stmt, inst);
            ++result;
        }
    }

    return result;
}

int32_t LogSqliteDatabase::fill_inst_scopes(std::vector<areg::ScopeEntry>& scopes, SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries)
{
    int32_t result{ 0 };
    if ((static_cast<uint32_t>(scopes.size()) > startAt) && stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(scopes.size()) > (startAt + static_cast<uint32_t>(result)));
            areg::ScopeEntry& scope{ scopes[startAt + static_cast<uint32_t>(result)] };
            _copy_log_scopes(stmt, scope);
            ++result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int32_t LogSqliteDatabase::fill_log_messages(std::vector<SharedBuffer>& logs, SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries)
{
    int32_t result{ 0 };
    if ((static_cast<uint32_t>(logs.size()) > startAt) && stmt.is_valid())
    {
        while (stmt.next() == SqliteStatement::QueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(logs.size()) > (startAt + static_cast<uint32_t>(result)));
            SharedBuffer& log{ logs[startAt + static_cast<uint32_t>(result)] };
            _copy_log_message(stmt, log);
            ++result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

uint32_t LogSqliteDatabase::setup_statement_read_scopes(SqliteStatement& stmt, ITEM_ID instId)
{
    stmt.reset();
    if (instId == areg::TARGET_ALL)
    {
        return (stmt.prepare(_sqlGetAllLogScopes) ? count_scope_entries(instId) : 0u);
    }
    else
    {
        return (stmt.prepare(_sqlGetLogScopes) && stmt.bind_uint32(0, static_cast<uint32_t>(instId)) ? count_scope_entries(instId) : 0u);
    }
}

uint32_t LogSqliteDatabase::setup_statement_read_logs(
    SqliteStatement& stmt,
    ITEM_ID instId,
    int32_t limit,
    uint32_t offset)
{
    stmt.reset();

    if (instId == areg::TARGET_ALL)
    {
        return (
            stmt.prepare(_sqlGetAllLogMessagesPaged) &&
            stmt.bind_int32(0, limit) &&
            stmt.bind_uint32(1, offset)
        ) ? count_log_entries(instId) : 0u;
    }

    return (
        stmt.prepare(_sqlGetInstLogMessagesPaged) &&
        stmt.bind_uint32(0, static_cast<uint32_t>(instId)) &&
        stmt.bind_int32(1, limit) &&
        stmt.bind_uint32(2, offset)
    ) ? count_log_entries(instId) : 0u;
}

uint32_t LogSqliteDatabase::setup_filter_logs(ITEM_ID instId, const ArrayList<ScopeFilter>& filter)
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return 0u;
    else if (table_exists("scopes", _master) == false)
        return 0u;

    if (table_exists("filter_rules", _temp) == false)
    {
        mDatabase.begin();
        SqliteStatement stmt(mDatabase, _sqlCreateTempScopes);
        if (stmt.execute() == false)
        {
            commit(false);
            return 0u;
        }

        stmt.reset();
        if ((stmt.prepare(_sqlInitTempScopes) == false) || (stmt.execute() == false))
        {
            commit(false);
            return 0u;
        }

        stmt.reset();
        if ((stmt.prepare(_sqlCreateTempIndex) == false) || (stmt.execute() == false))
        {
            commit(false);
            return 0u;
        }

        commit(true);
    }

    return _update_filter_log_scopes(instId, filter);
}

uint32_t LogSqliteDatabase::setup_statement_read_filter_logs(SqliteStatement& stmt, ITEM_ID instId)
{
    Lock lock(mLock);
    stmt.reset();
    if (mDatabase.is_operable() == false)
        return 0u;
    else if (table_exists("scopes", _master) == false)
        return 0u;
    else if (table_exists("filter_rules", _temp) == false)
        return 0u;

    uint32_t result = count_filter_logs(instId);
    if (result > 0)
    {
        if (instId == areg::TARGET_ALL)
        {
            VERIFY(stmt.prepare(_sqlFilterScopeLogsAll));
        }
        else if (stmt.prepare(_sqlFilterScopeLogsInst))
        {
            stmt.bind_uint32(0, static_cast<uint32_t>(instId));
        }
    }

    return result;
}

uint32_t LogSqliteDatabase::_update_filter_log_scopes(ITEM_ID instId, const ArrayList<ScopeFilter>& filter)
{
    if (filter.is_empty() == false)
    {
        String sql;
        if (instId == areg::TARGET_ALL)
            sql = _sqlUpdateFilterRuleAll;
        else
            sql.format(_sqlUpdateFilterRuleInst.data(), instId);

        mDatabase.begin();
        SqliteStatement stmt(mDatabase, sql);
        for (const auto& scope : filter.data())
        {
            stmt.bind_uint32(0, scope.scopePrio);
            stmt.bind_uint32(1, scope.scopeId);
            if (stmt.execute() == false)
            {
                commit(false);
                return 0u;
            }

            stmt.reset();
            stmt.clear_bindings();
        }

        commit(true);
    }
    else if (reset(instId) == false)
    {
        return 0u;
    }

    return count_filter_logs(instId);
}

uint32_t LogSqliteDatabase::count_log_entries(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return 0u;

    SqliteStatement stmt(mDatabase);
    if (instId == areg::TARGET_ALL)
    {
        VERIFY(stmt.prepare(_sqlCountAllLogs));
    }
    else if (stmt.prepare(_sqlCountInstanceLogs))
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(instId));
    }

    return (stmt.next() != SqliteStatement::QueryResult::Failed ? stmt.as_uint32(0) : 0);
}

uint32_t LogSqliteDatabase::count_scope_entries(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return 0;

    SqliteStatement stmt(mDatabase);
    if (instId == areg::TARGET_ALL)
    {
        VERIFY(stmt.prepare(_sqlCountAllScopes));
    }
    else if (stmt.prepare(_sqlCountInstanceScopes))
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(instId));
    }

    return (stmt.next() != SqliteStatement::QueryResult::Failed ? stmt.as_uint32(0) : 0);
}

uint32_t LogSqliteDatabase::count_log_instances()
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return 0;

    SqliteStatement stmt(mDatabase, _sqlCountInstances);
    return (stmt.next() != SqliteStatement::QueryResult::Failed ? stmt.as_uint32(0) : 0);
}

uint32_t LogSqliteDatabase::count_filter_logs(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return 0;

    SqliteStatement stmt(mDatabase);
    if (instId == areg::TARGET_ALL)
    {
        VERIFY(stmt.prepare(_sqlFilterScopeLogsCountAll));
    }
    else if (stmt.prepare(_sqlFilterScopeLogsCount))
    {
        stmt.bind_uint32(0, static_cast<uint32_t>(instId));
    }

    return (stmt.next() != SqliteStatement::QueryResult::Failed ? stmt.as_uint32(0) : 0);
}

bool LogSqliteDatabase::reset(ITEM_ID instId /*= areg::TARGET_ALL*/)
{
    if (table_exists("filter_rules", _temp) == false)
        return false;

    SqliteStatement stmt(mDatabase);
    if (instId == areg::TARGET_ALL)
    {
        VERIFY(stmt.prepare(_sqlResetFilterScopesAll));
    }
    else
    {
        VERIFY(stmt.prepare(_sqlResetFilterScopes));
        stmt.bind_uint32(0, instId);
    }

    return stmt.execute();
}

bool LogSqliteDatabase::ensure_time_index()
{
    Lock lock(mLock);
    if ((mDatabase.is_operable() == false) || (table_exists("logs", _master) == false))
        return false;

    return mDatabase.execute(_sqlCreateIdxLogsTime);
}

bool LogSqliteDatabase::setup_statement_read_window(SqliteStatement& stmt, const LogPosition& after, int32_t limit)
{
    Lock lock(mLock);
    stmt.reset();
    if ((mDatabase.is_operable() == false) || (table_exists("refused_spans", _temp) == false))
        return false;

    if (after.timeCreated == 0)
    {
        return stmt.prepare(_sqlWindowFirst) && stmt.bind_int32(0, limit);
    }

    return stmt.prepare(_sqlWindowAfter)
        && stmt.bind_uint64(0, static_cast<uint64_t>(after.timeCreated))
        && stmt.bind_uint64(1, after.rowId)
        && stmt.bind_int32 (2, limit);
}

bool LogSqliteDatabase::setup_statement_read_window_back(SqliteStatement& stmt, const LogPosition& before, int32_t limit)
{
    Lock lock(mLock);
    stmt.reset();
    if ((mDatabase.is_operable() == false) || (table_exists("refused_spans", _temp) == false))
        return false;

    if (before.timeCreated == 0)
    {
        return stmt.prepare(_sqlWindowLast) && stmt.bind_int32(0, limit);
    }

    return stmt.prepare(_sqlWindowBefore)
        && stmt.bind_uint64(0, static_cast<uint64_t>(before.timeCreated))
        && stmt.bind_uint64(1, before.rowId)
        && stmt.bind_int32 (2, limit);
}

bool LogSqliteDatabase::setup_statement_read_filter_window(SqliteStatement& stmt, ITEM_ID instId, const LogPosition& after, int32_t limit)
{
    Lock lock(mLock);
    return _setup_filter_window(stmt, instId, after, limit, after.timeCreated == 0 ? _sqlFilterWindowFirst : _sqlFilterWindowAfter);
}

bool LogSqliteDatabase::setup_statement_read_filter_window_back(SqliteStatement& stmt, ITEM_ID instId, const LogPosition& before, int32_t limit)
{
    Lock lock(mLock);
    return _setup_filter_window(stmt, instId, before, limit, before.timeCreated == 0 ? _sqlFilterWindowLast : _sqlFilterWindowBefore);
}

bool LogSqliteDatabase::setup_statement_read_window_at(SqliteStatement& stmt, uint32_t offset, int32_t limit)
{
    Lock lock(mLock);
    stmt.reset();
    if ((mDatabase.is_operable() == false) || (table_exists("refused_spans", _temp) == false))
        return false;

    bool refused{ true };
    {
        SqliteStatement query(mDatabase, _sqlHasRefused);
        if (query.next() == SqliteStatement::QueryResult::HasMore)
        {
            refused = query.as_uint32(0) != 0u;
        }
    }

    return stmt.prepare(refused ? _sqlWindowAt : _sqlWindowAtAll)
        && stmt.bind_int32 (0, limit)
        && stmt.bind_uint32(1, offset);
}

bool LogSqliteDatabase::setup_statement_read_filter_window_at(SqliteStatement& stmt, ITEM_ID instId, uint32_t offset, int32_t limit)
{
    Lock lock(mLock);
    stmt.reset();
    if ((mDatabase.is_operable() == false) || (table_exists("filter_rules", _temp) == false))
        return false;

    const bool oneInstance{ instId != areg::TARGET_ALL };
    const String sql{ _with_condition(_sqlFilterWindowAt, oneInstance ? _sqlInstanceCondition : std::string_view{ }) };
    if (stmt.prepare(sql) == false)
        return false;

    int32_t index{ 0 };
    bool result{ true };
    if (oneInstance)
    {
        result = stmt.bind_uint32(index++, static_cast<uint32_t>(instId));
    }

    return result
        && stmt.bind_int32 (index, limit)
        && stmt.bind_uint32(index + 1, offset);
}

inline bool LogSqliteDatabase::_setup_filter_window(SqliteStatement& stmt, ITEM_ID instId, const LogPosition& place, int32_t limit, std::string_view sqlFormat)
{
    stmt.reset();
    if ((mDatabase.is_operable() == false) || (table_exists("filter_rules", _temp) == false))
        return false;

    const bool oneInstance{ instId != areg::TARGET_ALL };
    const String sql{ _with_condition(sqlFormat, oneInstance ? _sqlInstanceCondition : std::string_view{ }) };
    if (stmt.prepare(sql) == false)
        return false;

    int32_t index{ 0 };
    bool result{ true };
    if (oneInstance)
    {
        result = stmt.bind_uint32(index++, static_cast<uint32_t>(instId));
    }

    if (place.timeCreated != 0)
    {
        result = result
              && stmt.bind_uint64(index++, static_cast<uint64_t>(place.timeCreated))
              && stmt.bind_uint64(index++, place.rowId);
    }

    return result && stmt.bind_int32(index, limit);
}

bool LogSqliteDatabase::set_refused_spans(const ArrayList<RefusedSpan>& spans)
{
    Lock lock(mLock);
    if (mDatabase.is_operable() == false)
        return false;
    else if (_create_temp_refused() == false)
        return false;

    if (mDatabase.execute(_sqlClearRefused) == false)
        return false;
    else if (spans.is_empty())
        return true;

    mDatabase.begin();
    bool result{ true };
    for (uint32_t i = 0; result && (i < spans.size()); ++i)
    {
        const RefusedSpan& span{ spans[i] };
        SqliteStatement stmt(mDatabase, _sqlInsertRefused);
        result = stmt.is_valid()
              && stmt.bind_uint32(0, span.scopeId)
              && stmt.bind_uint64(1, static_cast<uint64_t>(span.targetId))
              && stmt.bind_uint64(2, static_cast<uint64_t>(span.fromTime))
              && stmt.bind_uint64(3, static_cast<uint64_t>(span.toTime))
              && stmt.execute();
    }

    mDatabase.commit(result);
    return result;
}

int32_t LogSqliteDatabase::fill_log_window(std::vector<SharedBuffer>& logs, SqliteStatement& stmt, int32_t maxEntries, LogPosition& lastRead)
{
    return _fill_window(logs, stmt, maxEntries, lastRead, nullptr);
}

int32_t LogSqliteDatabase::fill_log_window(std::vector<SharedBuffer>& logs, SqliteStatement& stmt, int32_t maxEntries, LogPosition& lastRead, std::vector<uint32_t>& rowIds)
{
    return _fill_window(logs, stmt, maxEntries, lastRead, &rowIds);
}

inline int32_t LogSqliteDatabase::_fill_window(std::vector<SharedBuffer>& logs, SqliteStatement& stmt, int32_t maxEntries, LogPosition& lastRead, std::vector<uint32_t>* rowIds)
{
    int32_t result{ 0 };
    if (stmt.is_valid() == false)
        return result;

    while (stmt.next() == SqliteStatement::QueryResult::HasMore)
    {
        SharedBuffer log;
        _copy_log_message(stmt, log);
        logs.push_back(log);

        // The place of the last row read is where the next window continues from.
        lastRead.timeCreated = static_cast<TIME64>(stmt.as_uint64(5));
        lastRead.rowId       = stmt.as_uint64(14);
        if (rowIds != nullptr)
        {
            rowIds->push_back(static_cast<uint32_t>(lastRead.rowId));
        }

        ++result;
        if ((maxEntries > 0) && (result >= maxEntries))
            break;
    }

    stmt.reset();
    return result;
}

inline bool LogSqliteDatabase::_create_temp_refused() noexcept
{
    if (table_exists("refused_spans", _temp))
        return true;

    return mDatabase.execute(_sqlCreateTempRefused) && mDatabase.execute(_sqlCreateIdxRefused);
}

bool LogSqliteDatabase::disable_filter_mask(ITEM_ID instId)
{
    if (table_exists("filter_rules", _temp) == false)
        return false;

    SqliteStatement stmt(mDatabase);
    if (instId == areg::TARGET_ALL)
    {
        VERIFY(stmt.prepare(_sqlDisableFilterScopesAll));
    }
    else
    {
        VERIFY(stmt.prepare(_sqlDisableFilterScopes));
        stmt.bind_uint32(0, instId);
    }

    return stmt.execute();
}

bool LogSqliteDatabase::table_exists(const char* table, const char* master /*= nullptr*/)
{
    bool result{ false };
    if (is_operable() && (areg::is_empty<char>(table) == false))
    {
        const bool isTemp{ (areg::is_empty<char>(master) == false) && (String(master).compare(_temp) == areg::Ordering::Equal) };
        SqliteStatement stmt(mDatabase, String(isTemp ? _sqlCheckTempTable : _sqlCheckTable));
        stmt.bind_text(0, String(table));
        result = (SqliteStatement::QueryResult::HasMore == stmt.next());
    }

    return result;
}

bool LogSqliteDatabase::drop_table(const char* table)
{
    if (areg::is_empty<char>(table))
        return false;

    String sql;
    sql.format(_sqlDropTable.data(), _quote_identifier(table).as_string());
    SqliteStatement stmt(mDatabase, sql);
    bool result = stmt.execute();
    stmt.finalize();
    return result;
}

} // namespace areg::ext
