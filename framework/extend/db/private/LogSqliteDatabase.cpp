/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/db/private/LogSqliteDatabase.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "extend/db/LogSqliteDatabase.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/NEMath.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/NEService.hpp"
#include "areg/trace/NETrace.hpp"
#include "areg/trace/private/NELogging.hpp"

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

    //! A string format to generate UPDATE state to change entry in the version table.
    //! Used when application stops collecting logs.
    constexpr std::string_view  _fmtUpdVersion
    {
        "UPDATE version SET time_closed = %llu;"
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

    //! A string format to generate UPDATE statement and mark all instances as disconnected.
    //! It is closed before closing database to ensure that all entries are updated.
    constexpr std::string_view _fmCloseInstances
    {
        "UPDATE instances SET inst_connect = 0, time_disconnected = %llu, time_updated = %llu WHERE time_disconnected IS NULL;"
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

    //! A string format to generate UPDATE statement to mark all scopes of all instances as inactive.
    constexpr std::string_view _fmtCloseScopes
    {
        "UPDATE scopes SET time_inactivated = %llu, scope_is_active = 0 WHERE scope_is_active = 1;"
    };

    //! Create a table with logs that contain information of application cookie ID,
    //! scope ID, log priority, log message, and information like thread.
    constexpr std::string_view  _sqlCreateTbLogs
    {
        "CREATE TABLE \"logs\" ("
            "\"id\"	                INTEGER NOT NULL UNIQUE,"
            "\"cookie_id\"	        INTEGER,"
            "\"scope_id\"	        INTEGER,"
            "\"msg_type\"	        INTEGER,"
            "\"msg_prio\"	        INTEGER,"
            "\"msg_module_id\"	    INTEGER,"
            "\"msg_thread_id\"	    INTEGER,"
            "\"msg_log\"	        TEXT,"
            "\"msg_thread\"	        TEXT,"
            "\"msg_module\"	        TEXT,"
            "\"time_created\"	    NUMERIC,"
            "\"time_received\"      NUMERIC,"
            "CONSTRAINT \"pk_msg_id\" PRIMARY KEY(\"id\" AUTOINCREMENT)"
            ");"
    };

    //! A string format to create INSERT statement to insert new log message in the logs table.
    constexpr std::string_view _fmtLog
    {
        "INSERT INTO logs "
        "(cookie_id, scope_id, msg_type, msg_prio, msg_module_id, msg_thread_id, msg_log, msg_thread, msg_module, time_created, time_received)"
        "VALUES "
        "(%llu, %u, %u, %u, %llu, %llu, \'%s\', \'%s\', \'%s\', %llu, %llu);"
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

    //! The size of the string buffer to generate a message.
    constexpr uint32_t  MSG_LEN     { 512 };

    //! The size of the string buffer to format SQL scripts
    constexpr uint32_t  SQL_LEN     { 768 };

    //! The size of the string buffer to format SQL script to insert a log.
    constexpr uint32_t  SQL_LEN_MAX { 1024 };
}

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class implementation.
//////////////////////////////////////////////////////////////////////////

LogSqliteDatabase::LogSqliteDatabase(void)
    : IELogDatabaseEngine   ( )

    , mDbPath               ( )
    , mDbObject             ( nullptr )
    , mIsInitialized        ( false )
    , mDbLogEnabled         ( true )
{
}

LogSqliteDatabase::~LogSqliteDatabase(void)
{
    _close();
}

inline bool LogSqliteDatabase::_open(const String& dbPath)
{
    if (mDbLogEnabled == false)
        return false;

    bool result{ true };
    _close();
    mDbPath = dbPath.isEmpty() == false ? File::normalizePath(dbPath) : mDbPath;
    String folder = File::getFileDirectory(mDbPath);
    if ((folder.isEmpty() == false) && (File::existDir(folder) == false))
    {
        File::createDirCascaded(folder);
    }

    if (SQLITE_OK != ::sqlite3_open(mDbPath.getString(), reinterpret_cast<sqlite3 **>(&mDbObject)))
    {
        _close();
        result = false;
    }

    return result;
}

inline void LogSqliteDatabase::_close(void)
{
    if (mDbObject != nullptr)
    {
        ::sqlite3_close(reinterpret_cast<sqlite3*>(mDbObject));
        mDbObject = nullptr;
        mIsInitialized = false;
    }
}

inline void LogSqliteDatabase::_createTables(void)
{
    ASSERT(mDbObject != nullptr);

    VERIFY(_execute(_sqlCreateTbVersion.data()));
    VERIFY(_execute(_sqlCreateTbInstances.data()));
    VERIFY(_execute(_sqlCreateTbScopes.data()));
    VERIFY(_execute(_sqlCreateTbLogs.data()));
}

inline void LogSqliteDatabase::_createIndexes(void)
{
    VERIFY(_execute(_sqlCraeteIdxCookie.data()));
    VERIFY(_execute(_sqlCreateIdxScopes.data()));
    VERIFY(_execute(_sqlCreateIdxLogs.data()));
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
                        , NETrace::LOG_VERSION.data()
                        , "AREG SDK database logging module. Visit https://aregtech.com for more information."
                        , "Created by AREG log observer API module."
                        , mDbPath.getString()
                        , static_cast<uint64_t>(now.getTime())
                        );
    VERIFY(_execute(sql));

    String::formatString(sql, SQL_LEN, _fmtLog.data()
                        , static_cast<uint64_t>(NEService::COOKIE_LOCAL)
                        , static_cast<uint32_t>(NEMath::CHECKSUM_IGNORE)
                        , static_cast<uint32_t>(NETrace::eLogMessageType::LogMessageText)
                        , static_cast<uint32_t>(NETrace::eLogPriority::PrioIgnore)
                        , static_cast<uint64_t>(proc.getId())
                        , static_cast<uint64_t>(threadId)
                        , "Starting database logging..."
                        , thread.getString()
                        , module.getString()
                        , static_cast<uint64_t>(now.getTime())
                        , static_cast<uint64_t>(now.getTime())
                        );
    VERIFY(_execute(sql));
}

inline bool LogSqliteDatabase::_execute(const char* sql)
{
    if (mDbLogEnabled)
    {
        ASSERT(NEString::isEmpty<char>(sql) == false);
        return (mDbObject != nullptr ? SQLITE_OK == ::sqlite3_exec(reinterpret_cast<sqlite3*>(mDbObject), sql, nullptr, nullptr, nullptr) : false);
    }

    return false;
}

bool LogSqliteDatabase::isOperable(void) const
{
    return (mDbObject != nullptr);
}

bool LogSqliteDatabase::connect(const String& dbPath /*= String::EmptyString*/)
{
    if (mDbObject == nullptr)
    {
        ASSERT(mIsInitialized == false);
        if (_open(dbPath))
        {
            _createTables();
            _createIndexes();
            _initialize();
            commit(true);
            mIsInitialized = true;
        }
    }

    return mIsInitialized;
}

void LogSqliteDatabase::disconnect(void)
{
    Process& proc{ Process::getInstance() };
    DateTime now{ DateTime::getNow() };
    String module{ proc.getAppName() };
    id_type threadId{ Thread::getCurrentThreadId() };
    String thread{ Thread::getThreadName(threadId) };

    char sql[SQL_LEN]{};

    String::formatString( sql, SQL_LEN, _fmtUpdVersion.data()
                        , static_cast<uint64_t>(now.getTime()));
    _execute(sql);

    String::formatString( sql, SQL_LEN, _fmtLog.data()
                        , NEService::COOKIE_LOCAL
                        , NEMath::CHECKSUM_IGNORE
                        , static_cast<uint32_t>(NETrace::eLogMessageType::LogMessageText)
                        , static_cast<uint32_t>(NETrace::eLogPriority::PrioIgnore)
                        , static_cast<uint64_t>(proc.getId())
                        , static_cast<uint64_t>(threadId)
                        , "Closing database logging..."
                        , thread.getString()
                        , module.getString()
                        , static_cast<uint64_t>(now.getTime())
                        , static_cast<uint64_t>(now.getTime())
                        );
    _execute(sql);

    String::formatString( sql, SQL_LEN, _fmtCloseScopes.data()
                        , static_cast<uint64_t>(now.getTime()));
    _execute(sql);

    String::formatString( sql, SQL_LEN, _fmCloseInstances.data()
                        , static_cast<uint64_t>(now.getTime())
                        , static_cast<uint64_t>(now.getTime()));
    _execute(sql);

    commit(true);
    _close();
}

bool LogSqliteDatabase::execute(const String& sql)
{
    return sql.isEmpty() ? false : _execute(sql.getString());
}

bool LogSqliteDatabase::begin(void)
{
    return _execute("BEGIN TRANSACTION;");
}

bool LogSqliteDatabase::commit(bool doCommit)
{
    return _execute(doCommit ? "COMMIT;" : "ROLLBACK;");
}

bool LogSqliteDatabase::tablesInitialized(void) const
{
    return mIsInitialized;
}

bool LogSqliteDatabase::logMessage(const NETrace::sLogMessage& message, const DateTime& timestamp)
{
    char sql[SQL_LEN_MAX];
    String::formatString(sql, SQL_LEN_MAX, _fmtLog.data()
                        , static_cast<uint64_t>(message.logCookie)
                        , static_cast<uint32_t>(message.logScopeId)
                        , static_cast<uint32_t>(message.logMsgType)
                        , static_cast<uint32_t>(message.logMessagePrio)
                        , static_cast<uint64_t>(message.logModuleId)
                        , static_cast<uint64_t>(message.logThreadId)
                        , message.logMessage
                        , message.logThreadLen != 0 ? message.logThread : String::EmptyString
                        , message.logModuleId  != 0 ? message.logModule : String::EmptyString
                        , static_cast<uint64_t>(message.logTimestamp)
                        , static_cast<uint64_t>(timestamp.getTime())
                        );

    return _execute(sql);
}

bool LogSqliteDatabase::logInstanceConnected(const NEService::sServiceConnectedInstance& instance, const DateTime& timestamp)
{
    Process& proc{ Process::getInstance() };
    String module{ proc.getAppName() };
    id_type threadId{ Thread::getCurrentThreadId() };
    String thread{ Thread::getThreadName(threadId) };

    char msg[MSG_LEN];
    String::formatString( msg, MSG_LEN, "The %u-bit instance [ %s ] with cookie [ %llu ] is connected at time [ %s ]"
                        , static_cast<uint32_t>(instance.ciBitness)
                        , instance.ciInstance.getString()
                        , static_cast<uint64_t>(instance.ciCookie)
                        , timestamp.formatTime().getString());

    char sqlLog[SQL_LEN_MAX];
    String::formatString( sqlLog, SQL_LEN_MAX, _fmtLog.data()
                        , static_cast<uint64_t>(NEService::COOKIE_LOCAL)
                        , static_cast<uint32_t>(NEMath::CHECKSUM_IGNORE)
                        , static_cast<uint32_t>(NETrace::eLogMessageType::LogMessageText)
                        , static_cast<uint32_t>(NETrace::eLogPriority::PrioIgnore)
                        , static_cast<uint64_t>(proc.getId())
                        , static_cast<uint64_t>(threadId)
                        , msg
                        , thread.getString()
                        , module.getString()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(timestamp.getTime())
                        );

    char sqlInst[SQL_LEN];
    String::formatString( sqlInst, SQL_LEN, _fmtInstance.data()
                        , static_cast<uint64_t>(instance.ciCookie)
                        , static_cast<uint32_t>(instance.ciSource)
                        , static_cast<uint32_t>(instance.ciBitness)
                        , instance.ciInstance.getString()
                        , instance.ciLocation.getString()
                        , static_cast<uint64_t>(instance.ciTimestamp)
                        , static_cast<uint64_t>(timestamp.getTime())
                        );

    return (_execute(sqlLog) && _execute(sqlInst));
}

bool LogSqliteDatabase::logInstanceDisconnected(const ITEM_ID& cookie, const DateTime& timestamp)
{
    logScopesDeactivate(cookie, timestamp);

    Process& proc{ Process::getInstance() };
    String module{ proc.getAppName() };
    id_type threadId{ Thread::getCurrentThreadId() };
    String thread{ Thread::getThreadName(threadId) };

    char msg[MSG_LEN];
    String::formatString( msg, MSG_LEN, "The instance with cookie [ %llu ] is disconnected at time [ %s ]"
                        , static_cast<uint64_t>(cookie)
                        , timestamp.formatTime().getString());

    char sqlLog[SQL_LEN_MAX];
    String::formatString( sqlLog, SQL_LEN_MAX, _fmtLog.data()
                        , static_cast<uint64_t>(NEService::COOKIE_LOCAL)
                        , static_cast<uint32_t>(NEMath::CHECKSUM_IGNORE)
                        , static_cast<uint32_t>(NETrace::eLogMessageType::LogMessageText)
                        , static_cast<uint32_t>(NETrace::eLogPriority::PrioIgnore)
                        , static_cast<uint64_t>(proc.getId())
                        , static_cast<uint64_t>(threadId)
                        , msg
                        , thread.getString()
                        , module.getString()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(timestamp.getTime())
                        );

    char sqlInst[SQL_LEN];
    String::formatString( sqlInst, SQL_LEN, _fmtUpdInstance.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(DateTime::getNow().getTime())
                        , static_cast<uint64_t>(cookie));

    return (_execute(sqlLog) && _execute(sqlInst));
}

bool LogSqliteDatabase::logScopeActivate(const NETrace::sScopeInfo & scope, const ITEM_ID& cookie, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtScopes.data()
                        , static_cast<uint32_t>(scope.scopeId)
                        , static_cast<uint64_t>(cookie)
                        , static_cast<uint32_t>(scope.scopePrio)
                        , scope.scopeName.getString()
                        , static_cast<uint64_t>(timestamp.getTime()));
    return _execute(sql);
}

uint32_t LogSqliteDatabase::logScopesActivate(const NETrace::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp)
{
    uint32_t result{ 0 };
    char sql[SQL_LEN];
    for (const auto& scope : scopes.getData())
    {
        String::formatString( sql, SQL_LEN, _fmtScopes.data()
                            , static_cast<uint32_t>(scope.scopeId)
                            , static_cast<uint64_t>(cookie)
                            , static_cast<uint32_t>(scope.scopePrio)
                            , scope.scopeName.getString()
                            , static_cast<uint64_t>(timestamp.getTime()));
        result += _execute(sql) ? 1 : 0;
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
                        , static_cast<uint64_t>(timestamp.getTime()));
    return _execute(sql);
}

bool LogSqliteDatabase::logScopesDeactivate(const ITEM_ID& cookie, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtUpdScopes.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(cookie));
    return _execute(sql);
}

bool LogSqliteDatabase::logScopeDeactivate(const ITEM_ID& cookie, unsigned int scopeId, const DateTime& timestamp)
{
    char sql[SQL_LEN];
    String::formatString( sql, SQL_LEN, _fmtUpdScope.data()
                        , static_cast<uint64_t>(timestamp.getTime())
                        , static_cast<uint64_t>(cookie)
                        , static_cast<uint32_t>(scopeId)
                        );
    return _execute(sql);
}
