#ifndef AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
#define AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/db/LogSqliteDatabase.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "aregextend/db/SqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/component/NEService.hpp"
#include "areg/logging/NELogging.hpp"
#include "areg/logging/IELogDatabaseEngine.hpp"
#include "areg/base/String.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging database engine, responsible to log messages in the database.
 **/
class LogSqliteDatabase : public IELogDatabaseEngine
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LogSqliteDatabase(void);
    virtual ~LogSqliteDatabase(void);

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if logging in the database is enabled.
     *          If logging is disabled, no database operation is performed.
     **/
    inline bool isDabataseLoggingEnabled(void) const;

    /**
     * \brief   Enables or disables the database logging.
     *          This flag should be set before initializing database, where the
     *          database file and data tables are created and initialized.
     * \param   enable  Flag, indicating whether the logging in the database is enabled or not.
     **/
    inline void setDatabaseLoggingEnabled(bool enable);

    /**
     * \brief   Returns database file path.
     **/
    inline const String& getDatabasePath(void) const;

    /**
     * \brief   Returns the initial database file path. The initial file path may contain mask like timestamp,
     *          so that each time database is disconnected and connected again with the empty file path,
     *          it creates new file.
     **/
    inline const String& getInitialDatabasePath(void) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// IEDatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if SqliteDatabase engine is opened and operable.
     *          Otherwise, returns false.
     **/
    virtual bool isOperable(void) const override;

    /**
     * \brief   Connects to the specified SqliteDatabase.
     * \param   dbPath  The path to the SqliteDatabase. If needed, the path may contain
     *                  file path or URL, user name and password. It is up to
     *                  SqliteDatabase engine to parse the path and initialize the connection.
     *                  If the parameter is empty, it should take the data from the
     *                  'areg.init' configuration file.
     * \return  Returns true if succeeded to connect. Otherwise, returns false.
     **/
    virtual bool connect(const String & dbPath) override;

    /**
     * \brief   Disconnects connected SqliteDatabase.
     **/
    virtual void disconnect(void) override;

    /**
     * \brief   Execute the SQL script.
     * \param   sql     The SQL script to execute.
     * \return  Returns true if succeeds to execute the SQL script.
     **/
    virtual bool execute(const String & sql) override;

    /**
     * \brief   Call if need to make multiple operation. This call starts the transaction,
     *          that is required either commit or rollback call to complete the transaction.
     **/
    virtual bool begin(void) override;

    /**
     * \brief   Commits or rolls back the SqliteDatabase changes and returns true if succeeded.
     * \param   doCommit    If true, the SqliteDatabase engine should commit the changes.
     *                      Otherwise, the SqliteDatabase engine should rollback the changes.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    virtual bool commit(bool doCommit) override;

/************************************************************************/
// IELogDatabaseEngine interface overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the database and the log tables are initialized,
     *          and ready to log messages.
     **/
    virtual bool areTablesInitialized(void) const override;

    /**
     * \brief   Called when logging message should be saved in the database.
     * \param   message     The structure of the message to log.
     * \param   timestamp   The timestamp to register when the message is logged.
     * \return  Returns true if succeeded to save the log in the database.
     **/
    virtual bool logMessage(const NELogging::sLogMessage & message, const DateTime & timestamp) override;

    /**
     * \brief   Called when need to log information about log source instance.
     * \param   instance    The structure of the logging message source to save in database.
     * \param   timestamp   The timestamp to register when the instance is logged.
     * \return  Returns true if succeeded to save the log instance in the database.
     **/
    virtual bool logInstanceConnected(const NEService::sServiceConnectedInstance & instance, const DateTime & timestamp) override;

    /**
     * \brief   Called when an instance of log source is disconnected.
     *          This call should as well automatically deactivate the log scopes.
     * \param   cookie      The cookie ID of the instance to mark as disconnected.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logInstanceDisconnected(const ITEM_ID & cookie, const DateTime & timestamp) override;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     * \param   scope       The log scope information to save in the database.
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    virtual bool logScopeActivate(const NELogging::sScopeInfo & scope, const ITEM_ID & cookie, const DateTime & timestamp) override;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     * \param   scopeName   The name of the scope.
     * \param   scopeId     The ID of the scope.
     * \param   scopePrio   The log priority of the scope
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    virtual bool logScopeActivate(const String & scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID & cookie, const DateTime & timestamp) override;

    /**
     * \brief   Called when need to log the information of the list of scopes in the database.
     * \param   scopes      The information of the list of the log scope to save in the database.
     * \param   cookie      The cookie of the log scope owner instance.
     * \param   timestamp   The timestamp to register when the scope is logged.
     * \return  Returns the number of scope entries saved in the database.
     **/
    virtual uint32_t logScopesActivate(const NELogging::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp) override;

    /**
     * \brief   Call to deactivate all scopes related with the specified cookie ID.
     * \param   cookie      The cookie ID, which scopes should be marked as deactivated.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logScopesDeactivate(const ITEM_ID & cookie, const DateTime & timestamp) override;

    /**
     * \brief   Call to deactivate a single scope related with the specified cookie ID.
     * \param   cookie      The cookie ID, which scope should be marked as deactivated.
     * \param   scopeId     The scope ID to mark as deactivated.
     * \param   timestamp   The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    virtual bool logScopeDeactivate(const ITEM_ID & cookie, unsigned int scopeId, const DateTime & timestamp) override;

    /**
     * \brief   Rolls back the database changes and returns true if succeeded.
     **/
    virtual bool rollback(void) override;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Call to query and get list of names of connected instances from log database.
     **/
    std::vector<String> getLogInstanceNames(void);
    void getLogInstanceNames(std::vector<String>& names);

    /**
     * \brief   Call to query and get list of IDs of connected instances from log database.
     **/
    std::vector<ITEM_ID> getLogInstances(void);
    void getLogInstances(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to query and get list of names of threads of the connected instances from log database.
     **/
    std::vector<String> getLogThreadNames(void);
    void getLogThreadNames(std::vector<String>& names);

    /**
     * \brief   Call to query and get list of IDs of threads of the connected instances from log database.
     **/
    std::vector<ITEM_ID> getLogThreads(void);
    void getLogThreads(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to get the list of log priorities.
     **/
    std::vector<String> getPriorityNames(void);
    void getPriorityNames(std::vector<String>& names);

    /**
     * \brief   Call to query and get information of connected instances from log database.
     *          This query will receive list of all registered instances.
     **/
    std::vector< NEService::sServiceConnectedInstance> getLogInstanceInfos(void);
    void getLogInstanceInfos(std::vector< NEService::sServiceConnectedInstance>& infos);

    /**
     * \brief   Call to query and get information of log scopes of specified instance from log database.
     *          This query will receive list of all registered scopes.
     * \param   instID  The ID of the instance.
     **/
    std::vector<NELogging::sScopeInfo> getLogInstScopes(ITEM_ID instId);
    void getLogInstScopes(std::vector<NELogging::sScopeInfo>& scopes, ITEM_ID instId);

    /**
     * \brief   Call to get all log messages from log database.
     **/
    std::vector<SharedBuffer> getLogMessages(void);
    void getLogMessages(std::vector<SharedBuffer>& messages);

    /**
     * \brief   Call to get log messages of the specified instance from log database.
     *          If `instId` is `NEService::COOKIE_ANY` it receives the list of all instances
     *          similar to the call to `getLogMessages()`.
     * \param   instId  The ID of the instance to get log messages.
     *                  If `NEService::COOKIE_ANY` it receives log messages of all instances.
     **/
    std::vector<SharedBuffer> getLogInstMessages(ITEM_ID instId = NEService::COOKIE_ANY);
    void getLogInstMessages(std::vector<SharedBuffer>& messages, ITEM_ID instId = NEService::COOKIE_ANY);

    /**
     * \brief   Call to get log messages of the specified scope from log database.
     *          If `scopeId` is `0` it receives the list of all scopes
     *          similar to the call to `getLogMessages()`.
     * \param   scopeId     The ID of the scope to get log messages.
     *                      If `0` it receives log messages of all scopes.
     **/
    std::vector<SharedBuffer> getLogScopeMessages(uint32_t scopeId = 0);
    void getLogScopeMessages(std::vector<SharedBuffer>& messages, uint32_t scopeId = 0);

    /**
     * \brief   Call to get log messages of the specified instance and log scope ID from log database.
     *          If `instId` is `NEService::COOKIE_ANY` and `scopeId` is `0`, it receives the list of all logs
     *          similar to the call to `getLogMessages()`.
     * \param   instId      The ID of the instance to get log messages.
     *                      If `NEService::COOKIE_ANY` it receives log messages of all instances.
     * \param   scopeId     The ID of the scope to get log messages.
     *                      If `0` it receives log messages of all scopes.
     **/
    std::vector<SharedBuffer> getLogMessages(ITEM_ID instId, uint32_t scopeId);
    void getLogMessages(std::vector<SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Opens or creates the specified database file.
     *          The path can be relative or absolute, it may as contain the mask.
     **/
    inline bool _open(const String& dbPath);

    /**
     * \brief   In the opened database file, creates the tables required to save logs.
     **/
    inline void _createTables(void);

    /**
     * \brief   In the opened database file, creates the indexes required by optimize operations.
     **/
    inline void _createIndexes(void);

    /**
     * \brief   Logs the initial information in the database like logging version and application name.
     **/
    inline void _initialize(void);

    inline void _copyLogMessage(SqliteStatement& stmt, SharedBuffer & buf);

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The path to the SQLite database file.
    SqliteDatabase  mDatabase;

    //!< The statement to log messages in the database.
    SqliteStatement mStmtLogs;

    //!< The initial path to the SQLIte database file. The path may contain mask like timestamp.
    String          mDbInitPath;

    //!< Flag, indicating whether the database and data tables are initialized or not.
    bool            mIsInitialized;

    //!< Flag, indicating whether the database logging is enabled or not.
    bool            mDbLogEnabled;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(LogSqliteDatabase);
};

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class inline methods.
//////////////////////////////////////////////////////////////////////////

bool LogSqliteDatabase::isDabataseLoggingEnabled(void) const
{
    return mDbLogEnabled;
}

inline void LogSqliteDatabase::setDatabaseLoggingEnabled(bool enable)
{
    mDbLogEnabled = enable;
}

inline const String& LogSqliteDatabase::getDatabasePath(void) const
{
    return mDatabase.getPath();
}

inline const String& LogSqliteDatabase::getInitialDatabasePath(void) const
{
    return mDbInitPath;
}

#endif  // AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
