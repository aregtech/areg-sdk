/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/db/private/LogSqliteDatabase.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/db/LogSqliteDatabase.hpp"

#include "aregextend/db/SqliteStatement.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/NEService.hpp"
#include "areg/logging/NELogging.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)


namespace
{
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

    //! A string format to generate INSERT statement to insert a new entry in the version.
    //! Normally, this is the only entry in the version table.
    constexpr std::string_view  _fmtVersion
    {
        "INSERT INTO version (name, version, describe, created_by, db_name, time_created) VALUES (\'%s\', \'%s\', \'%s\', \'%s\', \'%s\', %llu);"
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

    //! A string format to generate INSERT statement to insert a new entry in the instances table.
    //! It is called when a new log source instance is connecting to the log collector service.
    //! Each entry is a logging source process with unique cookie ID and the field indicating
    //! when the instance was connected or disconnected.
    constexpr std::string_view _fmtInstance
    {
        "INSERT INTO instances "
        "(cookie_id, inst_connect, inst_type, inst_bits, inst_name, inst_location, time_connected, time_updated) "
        "VALUES "
        "(%llu, 1, %u, %u, \'%s\', \'%s\', %llu, %llu);"
    };

    //! A string format to generate UPDATE statement to update instance entry.
    //! It is called when a log source instance disconnects from log collector service
    //! of when the log observer disconnects / stops running.
    //! It updates the disconnect time for the instance with specified cookie ID.
    constexpr std::string_view _fmtUpdInstance
    {
        "UPDATE instances SET inst_connect = 0, time_disconnected = %llu, time_updated = %llu WHERE cookie_id = %llu AND time_disconnected IS NULL;"
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

    //! A string format to generate INSERT statement to insert an information about scope.
    //! It is called when registering or updating scope list of the connected application.
    constexpr std::string_view _fmtScopes
    {
        "INSERT INTO scopes (scope_id, cookie_id, scope_is_active, scope_prio, scope_name, time_received)  VALUES (%u, %llu, 1, %u, \'%s\', %llu);"
    };

    //! A string to generate INSERT statement to insert a new scope in the scopes table.
    constexpr std::string_view _sqlInsertScope
    {
        "INSERT INTO scopes (scope_id, cookie_id, scope_is_active, scope_prio, scope_name, time_received)  VALUES (?, ?, 1, ?, ?, ?);"
    };

    //! A string format to generate UPDATE statement to update the scope state of a connected instance.
    //! The script will mark all scopes of specified cookie ID as inactive.
    constexpr std::string_view _fmtUpdScopes
    {
        "UPDATE scopes SET time_inactivated = %llu, scope_is_active = 0 WHERE cookie_id = %llu AND scope_is_active = 1;"
    };

    //! A string format to generate UPDATE statement to update the activation state of a single scope.
    //! The script will mark the specified scope of the specified cookie as inactive.
    constexpr std::string_view _fmtUpdScope
    {
        "UPDATE scopes SET time_inactivated = %llu, scope_is_active = 0 WHERE cookie_id = %llu AND scope_id = %u AND scope_is_active = 1;"
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
            "\"msg_thread\"	        TEXT,"
            "\"msg_module\"	        TEXT,"
            "\"time_created\"	    NUMERIC,"
            "\"time_received\"      NUMERIC,"
            "\"time_duration\"      NUMERIC,"
            "CONSTRAINT \"pk_msg_id\" PRIMARY KEY(\"id\" AUTOINCREMENT)"
            ");"
    };

    //! A string format to create INSERT statement to insert new log message in the logs table.
    constexpr std::string_view _fmtLog
    {
        "INSERT INTO logs "
        "(cookie_id, scope_id, session_id, msg_type, msg_prio, msg_module_id, msg_thread_id, msg_log, msg_thread, msg_module, time_created, time_received, time_duration)"
        "VALUES "
        "(%llu, %u, %u, %u, %u, %llu, %llu, \'%s\', \'%s\', \'%s\', %llu, %llu, %llu);"
    };

    //! A string format to create INSERT statement to insert new log message in the logs table.
    constexpr std::string_view _sqlInsertLog
    {
        "INSERT INTO logs "
        "(cookie_id, scope_id, session_id, msg_type, msg_prio, msg_module_id, msg_thread_id, msg_log, msg_thread, msg_module, time_created, time_received, time_duration)"
        "VALUES "
        "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
    };

    //! A script to create index of the instances table. 
    constexpr std::string_view  _sqlCraeteIdxCookie
    {
        "CREATE UNIQUE INDEX \"idx_inst_cookies\" ON \"instances\" (\"cookie_id\", \"time_connected\", \"time_disconnected\");"
    };

    //! A script to create index of the scopes table.
    constexpr std::string_view _sqlCreateIdxScopes
    {
        "CREATE UNIQUE INDEX \"idx_scope_id\" ON \"scopes\" (\"scope_id\", \"cookie_id\", \"time_received\", \"time_inactivated\");"
    };

    //! A script to create index of the logs table
    constexpr std::string_view  _sqlCreateIdxLogs
    {
        "CREATE INDEX \"idx_logs\" ON \"logs\" (\"cookie_id\", \"scope_id\", \"msg_thread_id\"); "
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
    constexpr std::string_view _sqlGetAllLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module FROM logs ORDER BY time_created;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetInstLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module FROM logs WHERE cookie_id = ? ORDER BY time_created;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetScopeLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module FROM logs WHERE scope_id = ? ORDER BY time_created;"
    };

    //! A script to extract logged messages of the certain instance source
    constexpr std::string_view _sqlGetInstScopeLogMessages
    {
        "SELECT msg_type, msg_prio, cookie_id, msg_module_id, msg_thread_id, time_created, time_received, time_duration, scope_id, session_id, msg_log, msg_thread, msg_module FROM logs WHERE cookie_id = ? AND scope_id = ? ORDER BY time_created;"
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

    constexpr std::string_view _sqlCheckTable
    {
        "SELECT name FROM %s WHERE type = \'table\' AND name = \'%s\';"
    };

    constexpr std::string_view _sqlCheckMasterTable
    {
        "SELECT name FROM sqlite_master WHERE type = \'table\' AND name = \'scopes\';"
    };

    constexpr std::string_view _sqlCheckTempTable
    {
        "SELECT name FROM sqlite_temp_master WHERE type=\'table\' AND name=\'filter_rules\';"
    };

    constexpr std::string_view _sqlCreateTempScopes
    {
        "CREATE TEMP TABLE filter_rules"
        "   scope_id      INTEGER NOT NULL DEFAULT 0,"
        "   target_id     INTEGER NOT NULL DEFAULT 0,"
        "   log_mask      INTEGER NOT NULL DEFAULT 1008"
        ");"
        "INSERT INTO filter_rules(scope_id, target_id, log_mask) SELECT scope_id, cookie_id, 1008 FROM scopes;"
    };

    constexpr std::string_view _sqlCreateTempFilter
    {
        "CREATE TEMP TABLE filter_masks"
        "   scope_id      INTEGER NOT NULL DEFAULT 0,"
        "   log_mask      INTEGER NOT NULL DEFAULT 1008"
        ");"
    };


    constexpr std::string_view _sqlInsertTempFilter
    {
        "INSERT INTO filter_masks(scope_id, log_mask) VALUES(?, ?);"
    };

    constexpr std::string_view _sqlFilterScopeLogsCountAll
    {
        "SELECT COUNT(l.id)"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE ((l.msg_prio & r.log_mask) != 0)"
    };

    constexpr std::string_view _sqlFilterScopeLogsCount
    {
        "SELECT COUNT(l.id)"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE (l.cookie_id = ?) AND ((l.msg_prio & r.log_mask) != 0)"
    };

    constexpr std::string_view _sqlFilterScopeLogsAll
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, l.msg_log, l.msg_thread, l.msg_module"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE ((l.msg_prio & r.log_mask) != 0)"
        "   ORDER BY time_created;"
    };

    constexpr std::string_view _sqlFilterScopeLogsInst
    {
        "SELECT l.msg_type, l.msg_prio, l.cookie_id, l.msg_module_id, l.msg_thread_id, l.time_created, l.time_received, l.time_duration, l.scope_id, l.session_id, l.msg_log, l.msg_thread, l.msg_module"
        "   FROM logs AS l"
        "   JOIN filter_rules AS r"
        "       ON l.scope_id = r.scope_id AND l.cookie_id = r.target_id"
        "   WHERE (l.cookie_id = ?) AND ((l.msg_prio & r.log_mask) != 0)"
        "   ORDER BY time_created;"
    };

    constexpr std::string_view _sqlUpdateFilterScopes
    {
        "UPDATE filter_rules SET log_mask = ("
        "       SELECT t.log_mask"
        "       FROM filter_masks m"
        "WHERE m.scope_id = filter_rules.scope_id)"
        "WHERE target_id = ?"
        "AND scope_id IN(SELECT scope_id FROM filter_masks);"
    };

    constexpr std::string_view _sqlDropTable
    {
        "DROP TABLE IF EXISTS %s;"
    };

    //! The size of the string buffer to format SQL scripts
    constexpr uint32_t  SQL_LEN     { 768 };
}

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class implementation.
//////////////////////////////////////////////////////////////////////////

String LogSqliteDatabase::getReadScopesQuery(void)
{
    return String(_sqlGetScopeLogMessages);
}

String LogSqliteDatabase::getReadInstancesQuery(void)
{
    return String(_sqlGetInstScopeLogMessages);
}

String LogSqliteDatabase::getReadAllLogMessagesQuery(void)
{
    return String(_sqlGetAllLogMessages);
}

LogSqliteDatabase::LogSqliteDatabase(void)
    : IELogDatabaseEngine   ( )

    , mDatabase             ( )
    , mStmtLogs             (mDatabase)
    , mDbInitPath           ( )
    , mIsInitialized        ( false )
    , mDbLogEnabled         ( true )
    , mLock                 ( false )
{
}

LogSqliteDatabase::~LogSqliteDatabase(void)
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
    if (dbPath.isEmpty() == false)
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

inline void LogSqliteDatabase::_createTables(void)
{
    VERIFY(mDatabase.execute(_sqlCreateTbVersion));
    VERIFY(mDatabase.execute(_sqlCreateTbInstances));
    VERIFY(mDatabase.execute(_sqlCreateTbScopes));
    VERIFY(mDatabase.execute(_sqlCreateTbLogs));
}

inline void LogSqliteDatabase::_createIndexes(void)
{
    VERIFY(mDatabase.execute(_sqlCraeteIdxCookie));
    VERIFY(mDatabase.execute(_sqlCreateIdxScopes));
    VERIFY(mDatabase.execute(_sqlCreateIdxLogs));
}

inline void LogSqliteDatabase::_initialize(void)
{
    Process& proc{ Process::getInstance() };
    DateTime now{ DateTime::getNow() };
    String module{ proc.getAppName() };
    id_type threadId{ Thread::getCurrentThreadId() };
    String thread{ Thread::getThreadName(threadId) };

    char sql[SQL_LEN]{};
    String::formatString( sql, SQL_LEN, _fmtVersion.data()
                        , Process::getInstance().getName().getString()
                        , NELogging::LOG_VERSION.data()
                        , "AREG SDK database logging module. Visit https://areg.tech for more information."
                        , "Created by AREG log observer API module."
                        , mDatabase.getPath().getString()
                        , static_cast<uint64_t>(now.getTime())
                        );
    VERIFY(mDatabase.execute(sql));

    String::formatString(sql, SQL_LEN, _fmtLog.data()
                        , static_cast<uint64_t>(NEService::COOKIE_LOCAL)
                        , static_cast<uint32_t>(NEMath::CHECKSUM_IGNORE)
                        , static_cast<uint32_t>(0u)
                        , static_cast<uint32_t>(NELogging::eLogMessageType::LogMessageText)
                        , static_cast<uint32_t>(NELogging::eLogPriority::PrioIgnore)
                        , static_cast<uint64_t>(proc.getId())
                        , static_cast<uint64_t>(threadId)
                        , "Starting database logging..."
                        , thread.getString()
                        , module.getString()
                        , static_cast<uint64_t>(now.getTime())
                        , static_cast<uint64_t>(now.getTime())
                        , static_cast<uint32_t>(0u)
                        );
    VERIFY(mDatabase.execute(sql));
}

inline void LogSqliteDatabase::_copyLogMessage(SqliteStatement& stmt, SharedBuffer& buf)
{
    constexpr uint32_t _logSize{ static_cast<uint32_t>(sizeof(NELogging::sLogMessage)) };
    buf.reserve(_logSize, false);
    buf.setSizeUsed(_logSize);
    buf.moveToBegin();
    NELogging::sLogMessage* log = reinterpret_cast<NELogging::sLogMessage*>(buf.getBuffer());

    ASSERT(log != nullptr);
    log->logDataType = NELogging::eLogDataType::LogDataRemote;
    log->logSource = NEService::COOKIE_ANY;
    log->logTarget = NEService::COOKIE_ANY;

    log->logMsgType     = static_cast<NELogging::eLogMessageType>(stmt.getUint32(0));
    log->logMessagePrio = static_cast<NELogging::eLogPriority>(stmt.getUint32(1));
    log->logCookie      = static_cast<ITEM_ID>( stmt.getInt64(2));
    log->logModuleId    = static_cast<ITEM_ID>( stmt.getInt64(3));
    log->logThreadId    = static_cast<ITEM_ID>( stmt.getInt64(4));
    log->logTimestamp   = static_cast<TIME64>(  stmt.getUint64(5));
    log->logReceived    = static_cast<TIME64>(  stmt.getUint64(6));
    log->logDuration    = static_cast<uint32_t>(stmt.getUint32(7));
    log->logScopeId     = static_cast<uint32_t>(stmt.getUint32(8));
    log->logSessionId   = static_cast<uint32_t>(stmt.getUint32(9));
    String msg          = stmt.getText(10);
    String thread       = stmt.getText(11);
    String module       = stmt.getText(12);

    log->logMessageLen  = msg.getLength();
    log->logThreadLen   = thread.getLength();
    log->logModuleLen   = module.getLength();

    NEString::copyStringFast(log->logMessage, msg.getString(), msg.getLength());
    NEString::copyStringFast(log->logThread, thread.getString(), thread.getLength());
    NEString::copyStringFast(log->logModule, module.getString(), module.getLength());
}

inline void LogSqliteDatabase::_copyLogInstances(SqliteStatement& stmt, NEService::sServiceConnectedInstance& inst)
{
    inst.ciSource   = static_cast<NEService::eMessageSource>(  stmt.getUint32(0));
    inst.ciBitness  = static_cast<NEService::eInstanceBitness>(stmt.getUint32(1));
    inst.ciCookie   = static_cast<ITEM_ID>(stmt.getInt64(2));
    inst.ciTimestamp= static_cast<TIME64>( stmt.getInt64(3));
    inst.ciInstance = stmt.getText(4);
    inst.ciLocation = stmt.getText(5);
}

inline void LogSqliteDatabase::_copyLogScopes(SqliteStatement& stmt, NELogging::sScopeInfo& scope)
{
    scope.scopeName = stmt.getText(0);
    scope.scopeId   = static_cast<uint32_t>(stmt.getUint32(1));
    scope.scopePrio = static_cast<uint32_t>(stmt.getUint32(2));
}

inline bool LogSqliteDatabase::_tableExists(const char* master, const char* table)
{
    bool result{ false };
    SqliteStatement stmt(mDatabase);
    if ((NEString::isEmpty<char>(master) == false) && (NEString::isEmpty<char>(table) == false))
    {
        String sql;
        sql.format(_sqlCheckTable.data(), master, table);
        result = (stmt.prepare(sql) && (SqliteStatement::eQueryResult::HasMore == stmt.next()));
    }

    return result;
}

inline void LogSqliteDatabase::_dropTable(const char* table)
{
    if (NEString::isEmpty<char>(table))
        return;

    String sql;
    sql.format(_sqlDropTable.data(), table);
    SqliteStatement stmt(mDatabase, sql);
    stmt.execute();
    stmt.finalize();
}

bool LogSqliteDatabase::isOperable(void) const
{
    return mDatabase.isOperable();
}

bool LogSqliteDatabase::connect(const String& dbPath, bool readOnly)
{
    Lock lock(mLock);
    if (mDbLogEnabled && mDatabase.isOperable() == false)
    {
        bool exists = File::existFile(dbPath);
        ASSERT(mIsInitialized == false);
        if (_open(dbPath, readOnly))
        {
            if (exists == false)
            {
                _createTables();
                _createIndexes();
                _initialize();
                commit(true);
            }

            mIsInitialized = true;
            if (readOnly == false)
            {
                mStmtLogs.prepare(_sqlInsertLog);
            }
        }
    }

    return mIsInitialized;
}

void LogSqliteDatabase::disconnect(void)
{
    Lock lock(mLock);
    if ((mDatabase.isOperable() == false) || (mIsInitialized == false))
        return;

    if (mStmtLogs.isValid())
        mStmtLogs.finalize();

    _dropTable("filter_rules");
    mDatabase.commit(true);
    mDatabase.disconnect();
    mIsInitialized = false;
}

bool LogSqliteDatabase::execute(const String& sql)
{
    Lock lock(mLock);
    return mDatabase.execute(sql);
}

bool LogSqliteDatabase::begin(void)
{
    Lock lock(mLock);
    return mDatabase.begin();
}

bool LogSqliteDatabase::commit(bool doCommit)
{
    Lock lock(mLock);
    return mDatabase.commit(doCommit);
}

bool LogSqliteDatabase::areTablesInitialized(void) const
{
    return mIsInitialized;
}

bool LogSqliteDatabase::logMessage(const NELogging::sLogMessage& message)
{
    Lock lock(mLock);
    if (mStmtLogs.isValid() == false)
    {
        return false;
    }

    mStmtLogs.bindUint64( 0, static_cast<uint64_t>(message.logCookie));
    mStmtLogs.bindUint32( 1, static_cast<uint32_t>(message.logScopeId));
    mStmtLogs.bindUint32( 2, static_cast<uint32_t>(message.logSessionId));
    mStmtLogs.bindUint32( 3, static_cast<uint32_t>(message.logMsgType));
    mStmtLogs.bindUint32( 4, static_cast<uint32_t>(message.logMessagePrio));
    mStmtLogs.bindUint64( 5, static_cast<uint64_t>(message.logModuleId));
    mStmtLogs.bindUint64( 6, static_cast<uint64_t>(message.logThreadId));
    mStmtLogs.bindText(   7, message.logMessage);
    mStmtLogs.bindText(   8, message.logThread);
    mStmtLogs.bindText(   9, message.logModule);
    mStmtLogs.bindUint64(10, static_cast<uint64_t>(message.logTimestamp));
    mStmtLogs.bindUint64(11, static_cast<uint64_t>(message.logReceived));
    mStmtLogs.bindUint32(12, static_cast<uint64_t>(message.logDuration));

    bool result{ mStmtLogs.next() == SqliteStatement::eQueryResult::HasNoMore };
    mStmtLogs.reset();
    mStmtLogs.clearBindings();
    return result;
}

bool LogSqliteDatabase::logInstanceConnected(const NEService::sServiceConnectedInstance& instance, const DateTime& timestamp)
{
    Lock lock(mLock);
    Process& proc    { Process::getInstance() };
    String   module  { proc.getAppName() };
    id_type  threadId{ Thread::getCurrentThreadId() };
    String   thread  { Thread::getThreadName(threadId) };

    char sqlInst[SQL_LEN];
    String::formatString( sqlInst, SQL_LEN, _fmtInstance.data()
                        , static_cast<uint64_t>(instance.ciCookie)
                        , static_cast<uint32_t>(instance.ciSource)
                        , static_cast<uint32_t>(instance.ciBitness)
                        , instance.ciInstance.c_str()
                        , instance.ciLocation.c_str()
                        , static_cast<uint64_t>(instance.ciTimestamp)
                        , static_cast<uint64_t>(timestamp.getTime())
                        );
    return mDatabase.execute(sqlInst);
}

bool LogSqliteDatabase::logInstanceDisconnected(const ITEM_ID& cookie, const DateTime& timestamp)
{
    Lock lock(mLock);
    logScopesDeactivate(cookie, timestamp);

    Process& proc{ Process::getInstance() };
    String module{ proc.getAppName() };
    id_type threadId{ Thread::getCurrentThreadId() };
    String thread{ Thread::getThreadName(threadId) };

    char sqlInst[SQL_LEN];
    String::formatString( sqlInst, SQL_LEN, _fmtUpdInstance.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(DateTime::getNow().getTime())
                        , static_cast<uint64_t>(cookie)
                        );
    return mDatabase.execute(sqlInst);
}

bool LogSqliteDatabase::logScopeActivate(const NELogging::sScopeInfo & scope, const ITEM_ID& cookie, const DateTime& timestamp)
{
    return logScopeActivate(scope.scopeName, scope.scopeId, scope.scopePrio, cookie, timestamp);
}

uint32_t LogSqliteDatabase::logScopesActivate(const NELogging::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp)
{
    Lock lock(mLock);
    uint32_t result{ 0 };
    SqliteStatement stmt(mDatabase, _sqlInsertScope);
    for (const auto& scope : scopes.getData())
    {
        stmt.bindUint32(0, static_cast<uint32_t>(scope.scopeId));
        stmt.bindUint64(1, static_cast<uint64_t>(cookie));
        stmt.bindUint32(2, static_cast<uint32_t>(scope.scopePrio));
        stmt.bindText(  3, scope.scopeName.getString());
        stmt.bindUint64(4, static_cast<uint64_t>(timestamp.getTime()));
        result += stmt.next() == SqliteStatement::eQueryResult::HasMore ? 1 : 0;
        stmt.reset();
        stmt.clearBindings();
    }

    return result;
}

bool LogSqliteDatabase::logScopeActivate(const String& scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID& cookie, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtScopes.data()
                        , static_cast<uint32_t>(scopeId)
                        , static_cast<uint64_t>(cookie)
                        , static_cast<uint32_t>(scopePrio)
                        , scopeName.getString()
                        , static_cast<uint64_t>(timestamp.getTime())
                        );
    return execute(sql);
}

bool LogSqliteDatabase::logScopesDeactivate(const ITEM_ID& cookie, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtUpdScopes.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(cookie)
                        );
    return execute(sql);
}

bool LogSqliteDatabase::logScopeDeactivate(const ITEM_ID& cookie, unsigned int scopeId, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtUpdScope.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(cookie)
                        , static_cast<uint32_t>(scopeId)
                        );
    return execute(sql);
}

bool LogSqliteDatabase::rollback(void)
{
    Lock lock(mLock);
    return mDatabase.rollback();
}

std::vector<String> LogSqliteDatabase::getLogInstanceNames(void)
{
    std::vector<String> result;
    getLogInstanceNames(result);
    return result;
}

void LogSqliteDatabase::getLogInstanceNames(std::vector<String>& OUT names)
{
    Lock lock(mLock);
    names.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstanceName);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            String instName{ stmt.getText(0) };
            if (instName.isEmpty() == false)
            {
                names.push_back(instName);
            }
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(names.size()));
}

std::vector<ITEM_ID> LogSqliteDatabase::getLogInstances(void)
{
    std::vector<ITEM_ID> result;
    getLogInstances(result);
    return result;
}

void LogSqliteDatabase::getLogInstances(std::vector<ITEM_ID>& OUT ids)
{
    Lock lock(mLock);
    ids.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstanceIds);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            ITEM_ID instId{ static_cast<ITEM_ID>(stmt.getInt64(0)) };
            ids.push_back(instId);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(ids.size()));
}

std::vector<String> LogSqliteDatabase::getLogThreadNames(void)
{
    std::vector<String> result;
    getLogThreadNames(result);
    return result;
}

void LogSqliteDatabase::getLogThreadNames(std::vector<String>& OUT names)
{
    Lock lock(mLock);
    names.clear();
    SqliteStatement stmt(mDatabase, _sqlGetThreadNames);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            String instName{ stmt.getText(0) };
            names.push_back(instName);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(names.size()));
}

std::vector<ITEM_ID> LogSqliteDatabase::getLogThreads(void)
{
    std::vector<ITEM_ID> result;
    getLogThreads(result);
    return result;
}

void LogSqliteDatabase::getLogThreads(std::vector<ITEM_ID>& OUT ids)
{
    Lock lock(mLock);
    ids.clear();
    SqliteStatement stmt(mDatabase, _sqlGetThreadIds);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            ITEM_ID instId{ static_cast<ITEM_ID>(stmt.getInt64(0)) };
            ids.push_back(instId);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(ids.size()));
}

std::vector<String> LogSqliteDatabase::getPriorityNames(void)
{
    std::vector<String> result{
        { 
          NELogging::logPrioToString(NELogging::eLogPriority::PrioAny)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioScope)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioDebug)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioInfo)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioWarning)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioError)
        , NELogging::logPrioToString(NELogging::eLogPriority::PrioFatal)
        }
    };
    return result;
}

void LogSqliteDatabase::getPriorityNames(std::vector<String>& OUT names)
{
    names = getPriorityNames();
}

std::vector<NEService::sServiceConnectedInstance> LogSqliteDatabase::getLogInstanceInfos(void)
{
    std::vector<NEService::sServiceConnectedInstance> result;
    getLogInstanceInfos(result);
    return result;
}

void LogSqliteDatabase::getLogInstanceInfos(std::vector<NEService::sServiceConnectedInstance>& OUT infos)
{
    Lock lock(mLock);
    infos.clear();
    SqliteStatement stmt(mDatabase, _sqlGetLogInstances);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            NEService::sServiceConnectedInstance inst;
            _copyLogInstances(stmt, inst);
            infos.push_back(inst);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(infos.size()));
}

std::vector<NELogging::sScopeInfo> LogSqliteDatabase::getLogInstScopes(ITEM_ID IN instId)
{
    std::vector<NELogging::sScopeInfo> result;
    getLogInstScopes(result, instId);
    return result;
}

void LogSqliteDatabase::getLogInstScopes(std::vector<NELogging::sScopeInfo>& OUT scopes, ITEM_ID IN instId)
{
    Lock lock(mLock);
    scopes.clear();
    SqliteStatement stmt(mDatabase, _sqlGetLogScopes);
    if (stmt.isValid())
    {
        stmt.bindUint64(0, static_cast<uint64_t>(instId));
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            NELogging::sScopeInfo scope;
            _copyLogScopes(stmt, scope);
            scopes.push_back(scope);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(scopes.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::getLogMessages(void)
{
    std::vector<SharedBuffer> result;
    getLogMessages(result);
    return result;
}

void LogSqliteDatabase::getLogMessages(std::vector<SharedBuffer>& OUT messages)
{
    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetAllLogMessages);
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer buf;
            _copyLogMessage(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::getLogInstMessages(ITEM_ID IN instId)
{
    std::vector<SharedBuffer> result;
    getLogInstMessages(result, instId);
    return result;
}

void LogSqliteDatabase::getLogInstMessages(std::vector<SharedBuffer>& OUT messages, ITEM_ID IN instId)
{
    if (instId == NEService::COOKIE_ANY)
    {
        getLogMessages(messages);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstLogMessages);
    if (stmt.isValid())
    {
        stmt.bindUint64(0, static_cast<uint64_t>(instId));
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer buf;
            _copyLogMessage(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::getLogScopeMessages(uint32_t IN scopeId)
{
    std::vector<SharedBuffer> result;
    getLogScopeMessages(result, scopeId);
    return result;
}

void LogSqliteDatabase::getLogScopeMessages(std::vector<SharedBuffer>& OUT messages, uint32_t IN scopeId)
{
    if (scopeId == 0)
    {
        getLogMessages(messages);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetScopeLogMessages);
    if (stmt.isValid())
    {
        stmt.bindUint32(0, static_cast<uint32_t>(scopeId));
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer buf;
            _copyLogMessage(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(messages.size()));
}

std::vector<SharedBuffer> LogSqliteDatabase::getLogMessages(ITEM_ID IN instId, uint32_t IN scopeId)
{
    if (instId == NEService::COOKIE_ANY)
    {
        return (scopeId == 0 ? getLogMessages() : getLogScopeMessages(scopeId));
    }
    else if (scopeId == 0)
    {
        return getLogInstMessages(instId);
    }

    Lock lock(mLock);
    std::vector<SharedBuffer> result;
    SqliteStatement stmt(mDatabase, _sqlGetInstScopeLogMessages);
    if (stmt.isValid())
    {
        stmt.bindUint64(0, static_cast<uint64_t>(instId));
        stmt.bindUint32(1, static_cast<uint32_t>(scopeId));
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer buf;
            _copyLogMessage(stmt, buf);
            result.push_back(buf);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(result.size()));
    return result;
}

void LogSqliteDatabase::getLogMessages(std::vector<SharedBuffer>& OUT messages, ITEM_ID IN instId, uint32_t IN scopeId)
{
    if (instId == NEService::COOKIE_ANY)
    {
        if (scopeId == 0)
            getLogMessages(messages);
        else
            getLogScopeMessages(messages, scopeId);
        return;
    }
    else if (scopeId == 0)
    {
        getLogInstMessages(messages, instId);
        return;
    }

    Lock lock(mLock);
    messages.clear();
    SqliteStatement stmt(mDatabase, _sqlGetInstScopeLogMessages);
    if (stmt.isValid())
    {
        stmt.bindUint64(0, static_cast<uint64_t>(instId));
        stmt.bindUint32(1, static_cast<uint32_t>(scopeId));
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer buf;
            _copyLogMessage(stmt, buf);
            messages.push_back(buf);
        }
    }

    ASSERT(stmt.getRowPos() == static_cast<uint32_t>(messages.size()));
}

int LogSqliteDatabase::getLogInstScopes(std::vector<NELogging::sScopeInfo>& OUT scopes, SqliteStatement& IN stmt, int IN maxEntries /*= -1*/)
{
    int result{ 0 };
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            NELogging::sScopeInfo scope;
            _copyLogScopes(stmt, scope);
            scopes.push_back(scope);
            ++ result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int LogSqliteDatabase::getLogMessages(std::vector<SharedBuffer>& OUT logs, SqliteStatement& IN stmt, int IN maxEntries /*= -1*/)
{
    int result{ 0 };
    if (stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            SharedBuffer log;
            _copyLogMessage(stmt, log);
            logs.push_back(log);
            ++ result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int LogSqliteDatabase::fillLogInstances(std::vector<NEService::sServiceConnectedInstance>& IN OUT infos, SqliteStatement& IN stmt)
{
    int result{ 0 };
    if ((static_cast<uint32_t>(infos.size()) != 0) && stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(infos.size()) > static_cast<uint32_t>(result));
            NEService::sServiceConnectedInstance& inst{ infos[result] };
            _copyLogInstances(stmt, inst);
            ++result;
        }
    }

    return result;
}

int LogSqliteDatabase::fillInstScopes(std::vector<NELogging::sScopeInfo>& IN OUT scopes, SqliteStatement& IN stmt, uint32_t IN startAt, int IN maxEntries)
{
    int result{ 0 };
    if ((static_cast<uint32_t>(scopes.size()) > startAt) && stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(scopes.size()) > (startAt + static_cast<uint32_t>(result)));
            NELogging::sScopeInfo& scope{ scopes[startAt + static_cast<uint32_t>(result)] };
            _copyLogScopes(stmt, scope);
            ++result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

int LogSqliteDatabase::fillLogMessages(std::vector<SharedBuffer>& IN OUT logs, SqliteStatement& IN stmt, uint32_t IN startAt, int IN maxEntries)
{
    int result{ 0 };
    if ((static_cast<uint32_t>(logs.size()) > startAt) && stmt.isValid())
    {
        while (stmt.next() == SqliteStatement::eQueryResult::HasMore)
        {
            ASSERT(static_cast<uint32_t>(logs.size()) > (startAt + static_cast<uint32_t>(result)));
            SharedBuffer& log{ logs[startAt + static_cast<uint32_t>(result)] };
            _copyLogMessage(stmt, log);
            ++result;
            if ((maxEntries > 0) && (result >= maxEntries))
                break;
        }
    }

    return result;
}

bool LogSqliteDatabase::setupStatementReadScopes(SqliteStatement& IN OUT stmt, ITEM_ID IN instId)
{
    stmt.reset();
    if (instId == NEService::TARGET_ALL)
    {
        return stmt.prepare(_sqlGetAllLogScopes);
    }
    else
    {
        return (stmt.prepare(_sqlGetLogScopes) && stmt.bindInt64(0, instId));
    }
}

bool LogSqliteDatabase::setupStatementReadLogs(SqliteStatement& IN OUT stmt, ITEM_ID IN instId)
{
    stmt.reset();
    if (instId == NEService::TARGET_ALL)
    {
        return stmt.prepare(_sqlGetAllLogMessages);
    }
    else
    {
        return (stmt.prepare(_sqlGetInstLogMessages) && stmt.bindInt64(0, instId));
    }
}

uint32_t LogSqliteDatabase::filterLogScopes(SqliteStatement& IN OUT stmt, ITEM_ID IN instId, const TEArrayList<LogSqliteDatabase::sScopeFilter>& IN filter)
{
    Lock lock(mLock);
    if (mDatabase.isOperable() == false)
        return 0u;

    if (_tableExists("sqlite_master", "scopes") == false)
        return 0u;

    if (_tableExists("sqlite_temp_master", "filter_rules") == false)
    {
        SqliteStatement stmtTemp(mDatabase, _sqlCreateTempScopes);
        if (stmtTemp.execute() == false)
        {
            commit(false);
            return 0u;
        }

        commit(true);
    }

    if (updaeFilterLogScopes(instId, filter) == false)
        return 0u;

    uint32_t result = countFilterLogs(instId);
    stmt.reset();
    if (result > 0)
    {
        if (instId == NEService::TARGET_ALL)
        {
            stmt.prepare(_sqlFilterScopeLogsAll);
        }
        else if (stmt.prepare(_sqlFilterScopeLogsInst))
        {
            stmt.bindInt64(0, instId);
        }
    }

    return result;
}

bool LogSqliteDatabase::updaeFilterLogScopes(ITEM_ID IN instId, const TEArrayList<sScopeFilter>& IN filter)
{
    Lock lock(mLock);
    if ((mDatabase.isOperable() == false) || filter.isEmpty())
        return false;

    SqliteStatement stmt(mDatabase);
    stmt.prepare(_sqlCreateTempFilter);
    stmt.execute();
    stmt.reset();

    stmt.prepare(_sqlInsertTempFilter);
    for (const auto& scope : filter.getData())
    {
        stmt.bindUint32(0, scope.scopeId);
        stmt.bindUint32(1, scope.scopePrio);
        stmt.execute();
        stmt.reset();
        stmt.clearBindings();
    }

    stmt.prepare(_sqlUpdateFilterScopes);
    stmt.bindInt64(0, instId);
    stmt.execute();
    stmt.finalize();
    _dropTable("filter_masks");
    return true;
}

uint32_t LogSqliteDatabase::countLogEntries(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.isOperable() == false)
        return 0u;

    SqliteStatement stmt(mDatabase);
    if (instId == NEService::TARGET_ALL)
    {
        stmt.prepare(_sqlCountAllLogs);
    }
    else if (stmt.prepare(_sqlCountInstanceLogs))
    {
        stmt.bindInt64(0, instId);
    }

    return (stmt.next() != SqliteStatement::eQueryResult::Failed ? stmt.getUint32(0) : 0);
}

uint32_t LogSqliteDatabase::countScopeEntries(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.isOperable() == false)
        return 0;

    SqliteStatement stmt(mDatabase);
    if (instId == NEService::TARGET_ALL)
    {
        stmt.prepare(_sqlCountAllScopes);
    }
    else if (stmt.prepare(_sqlCountInstanceScopes))
    {
        stmt.bindInt64(0, instId);
    }

    return (stmt.next() != SqliteStatement::eQueryResult::Failed ? stmt.getUint32(0) : 0);
}

uint32_t LogSqliteDatabase::countLogInstances(void)
{
    Lock lock(mLock);
    if (mDatabase.isOperable() == false)
        return 0;

    SqliteStatement stmt(mDatabase, _sqlCountInstances);
    return (stmt.next() != SqliteStatement::eQueryResult::Failed ? stmt.getUint32(0) : 0);
}

uint32_t LogSqliteDatabase::countFilterLogs(ITEM_ID instId)
{
    Lock lock(mLock);
    if (mDatabase.isOperable() == false)
        return 0;

    SqliteStatement stmt(mDatabase);
    if (instId == NEService::TARGET_ALL)
    {
        stmt.prepare(_sqlFilterScopeLogsCountAll);
    }
    else if (stmt.prepare(_sqlFilterScopeLogsCount))
    {
        stmt.bindInt64(0, instId);
    }

    return (stmt.next() != SqliteStatement::eQueryResult::Failed ? stmt.getUint32(0) : 0);
}
