/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
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
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)


namespace {
    //////////////////////////////////////////////////////////////////////////
    // Log specific SQL scripts for SQLite3 database.
    //////////////////////////////////////////////////////////////////////////

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
            "\"id\"	                INTEGER NOT NULL UNIQUE,"
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
            "CONSTRAINT \"pk_msg_id\" PRIMARY KEY(\"id\" AUTOINCREMENT)"
            ");"
    };

    //! A statement to insert a new log message in the logs table.
    //! The message, the thread name and the module name are bound, never placed in the
    //! statement: they are sent by the remote log source, see CWE-89.
    constexpr std::string_view _sqlInsertLog
    {
        "INSERT INTO logs "
        "(cookie_id, scope_id, session_id, msg_type, msg_prio, msg_module_id, msg_thread_id, msg_log, msg_len, msg_thread, msg_module, time_created, time_received, time_duration)"
        "VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    };

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

    //! A script to create index of the logs table
    constexpr std::string_view  _sqlCreateIdxLogs
    {
        "CREATE INDEX idx_logs ON logs(scope_id, msg_prio, cookie_id);"
    };

    //! An index to select the logs of one process, which the leading column of
    //! idx_logs cannot serve.
    constexpr std::string_view  _sqlCreateIdxLogsModule
    {
        "CREATE INDEX idx_logs_module ON logs(msg_module_id, msg_prio);"
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
        "WHERE ((time_created > ?) OR (time_created = ? AND id > ?)) AND "
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
        "WHERE ((time_created < ?) OR (time_created = ? AND id < ?)) AND "
        "NOT EXISTS (SELECT 1 FROM refused_spans r "
        "WHERE r.scope_id = logs.scope_id AND r.target_id = logs.cookie_id "
        "AND logs.time_created >= r.from_ts AND (r.to_ts = 0 OR logs.time_created < r.to_ts)) "
        "ORDER BY time_created DESC, id DESC LIMIT ?;"
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

    constexpr char const _master[]   { "sqlite_master" };
    constexpr char const _temp[]     { "sqlite_temp_master" };
} // namespace

namespace areg::ext {

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
{
}

LogSqliteDatabase::~LogSqliteDatabase()
{
    mStmtLogs.finalize();
    mDatabase.disconnect();
    mIsInitialized = false;
}

inline bool LogSqliteDatabase::_open(const String& dbPath, bool readOnly)
{
    if (mDbLogEnabled == false)
        return false;

    bool result{ true };
    mDatabase.disconnect();
    mIsInitialized = false;
    if (dbPath.is_empty() == false)
    {
        mDbInitPath = dbPath;
    }

    if (mDatabase.connect(mDbInitPath, readOnly) == false)
    {
        mDatabase.disconnect();
        mIsInitialized = false;
        result = false;
    }

    return result;
}

inline void LogSqliteDatabase::_create_tables() noexcept
{
    VERIFY(mDatabase.execute(_sqlCreateTbVersion));
    VERIFY(mDatabase.execute(_sqlCreateTbInstances));
    VERIFY(mDatabase.execute(_sqlCreateTbScopes));
    VERIFY(mDatabase.execute(_sqlCreateTbLogs));
}

inline void LogSqliteDatabase::_create_indexes() noexcept
{
    VERIFY(mDatabase.execute(_sqlCraeteIdxCookie));
    VERIFY(mDatabase.execute(_sqlCreateIdxScopes));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogs));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogsModule));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogsTime));
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
        SqliteStatement stmt(mDatabase, _sqlInsertLog);
        stmt.bind_uint32( 0, static_cast<uint32_t>(areg::COOKIE_LOCAL));
        stmt.bind_uint32( 1, static_cast<uint32_t>(areg::CHECKSUM_IGNORE));
        stmt.bind_uint32( 2, static_cast<uint32_t>(0u));
        stmt.bind_uint32( 3, static_cast<uint32_t>(areg::LogMessageType::MessageText));
        stmt.bind_uint32( 4, static_cast<uint32_t>(areg::LogPriority::PrioIgnore));
        stmt.bind_uint64( 5, static_cast<uint64_t>(proc.id()));
        stmt.bind_uint64( 6, static_cast<uint64_t>(threadId));
        stmt.bind_text(   7, String("Starting database logging..."));
        stmt.bind_text(   8, thread);
        stmt.bind_text(   9, module);
        stmt.bind_uint64(10, static_cast<uint64_t>(now.time()));
        stmt.bind_uint64(11, static_cast<uint64_t>(now.time()));
        stmt.bind_uint32(12, static_cast<uint32_t>(0u));
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

    // msg_len holds the length before the cut. A database written before the column
    // existed reads back as zero, so fall back to the stored text length.
    uint32_t storedLen  = static_cast<uint32_t>(stmt.as_uint32(13));
    log->logMessageLen  = storedLen != 0u ? storedLen : static_cast<uint32_t>(msg.length());
    log->logThreadLen   = static_cast<uint32_t>(thread.length());
    log->logModuleLen   = static_cast<uint32_t>(module.length());

    areg::copy_string_fast(log->logMessage, msg.as_string(), msg.length());
    areg::copy_string_fast(log->logThread, thread.as_string(), thread.length());
    areg::copy_string_fast(log->logModule, module.as_string(), module.length());
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

bool LogSqliteDatabase::connect(const String& dbPath, bool readOnly)
{
    Lock lock(mLock);
    if (mDbLogEnabled && mDatabase.is_operable() == false)
    {
        bool exists = File::has_file(dbPath);
        ASSERT(mIsInitialized == false);
        if (_open(dbPath, readOnly))
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
            if (readOnly == false)
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
    if ((mDatabase.is_operable() == false) || (mIsInitialized == false))
        return;

    if (mStmtLogs.is_valid())
        mStmtLogs.finalize();

    drop_table("filter_rules");
    mDatabase.commit(true);
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

    bool result{ mStmtLogs.next() == SqliteStatement::QueryResult::HasNoMore };
    mStmtLogs.reset();
    mStmtLogs.clear_bindings();
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
        && stmt.bind_uint64(1, static_cast<uint64_t>(after.timeCreated))
        && stmt.bind_uint64(2, after.rowId)
        && stmt.bind_int32 (3, limit);
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
        && stmt.bind_uint64(1, static_cast<uint64_t>(before.timeCreated))
        && stmt.bind_uint64(2, before.rowId)
        && stmt.bind_int32 (3, limit);
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

        ++result;
        if ((maxEntries > 0) && (result >= maxEntries))
            break;
    }

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
