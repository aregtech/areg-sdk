#ifndef AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
#define AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/LogSqliteDatabase.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "aregextend/db/SqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <vector>

namespace aregext
{
    //////////////////////////////////////////////////////////////////////////
    // LogSqliteDatabase class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The logging database engine, responsible to log messages in the database.
     **/
    class LogSqliteDatabase : public areg::LogDatabaseEngine
    {
    //////////////////////////////////////////////////////////////////////////
    // Internal types
    //////////////////////////////////////////////////////////////////////////
    public:
        //!< The structure of a filter by scopes and priorities
        struct ScopeFilter
        {
            uint32_t    scopeId     { 0u }; //!< Scope ID
            uint32_t    scopePrio   { 0u }; //!< Scope log prio
        };

    //////////////////////////////////////////////////////////////////////////
    // Static methods
    //////////////////////////////////////////////////////////////////////////
    public:

        /**
         * \brief   Returns the SQL query to read instance dependent log scopes from the database.
         *          The ID of the instance can be specified (bound) to read scopes.
         **/
        static areg::String getReadScopesQuery();

        /**
         * \brief   Returns the SQL query to read all instances from the database.
         **/
        static areg::String getReadInstancesQuery();

        /**
         * \brief   Returns the SQL query to read all log messages from the database.
         **/
        static areg::String getReadAllLogMessagesQuery();

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        LogSqliteDatabase();
        virtual ~LogSqliteDatabase();

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns true if logging in the database is enabled.
         *          If logging is disabled, no database operation is performed.
         **/
        inline bool isDabataseLoggingEnabled() const;

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
        inline const areg::String& getDatabasePath() const;

        /**
         * \brief   Returns the initial database file path. The initial file path may contain mask like timestamp,
         *          so that each time database is disconnected and connected again with the empty file path,
         *          it creates new file.
         **/
        inline const areg::String& getInitialDatabasePath() const;

        /**
         * \brief   Return SQLite database object
         **/
        inline aregext::SqliteDatabase& getDatabase();
        inline const aregext::SqliteDatabase& getDatabase() const;

        inline aregext::SqliteStatement& getStatement();
        inline const aregext::SqliteStatement& getStatement() const;

        /**
         * \brief   Checks whether the specified table exists or not.
         * \param   table   The name of the table to check
         * \param   master  The name of the schema to check the table. If empty or nullptr, will check the "sqlite_master" schema.
         * \return  Returns true if specified table exists in the specified schema.
         **/
        bool tableExists(const char* table, const char* master = nullptr);

        /**
         * \brief   Drops specified table.
         **/
        bool dropTable(const char* table);

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:

    /************************************************************************/
    // DatabaseEngine class overrides.
    /************************************************************************/

        /**
         * \brief   Returns true if SqliteDatabase engine is opened and operable.
         *          Otherwise, returns false.
         **/
        bool isOperable() const override;

        /**
         * \brief   Connects to the specified database.
         * \param   dbPath      The path to the database. If needed, the path may contain
         *                      file path or URL, user name and password. It is up to
         *                      Database engine to parse the path and initialize the connection.
         *                      If the parameter is empty, it should take the data from the
         *                      'areg.init' configuration file.
         * \param   readOnly    If true, the database engine should connect in read-only mode.
         * \return  Returns true if succeeded to connect. Otherwise, returns false.
         **/
        bool connect(const areg::String& dbPath, bool readOnly) override;

        /**
         * \brief   Disconnects connected SqliteDatabase.
         **/
        void disconnect() override;

        /**
         * \brief   Execute the SQL script.
         * \param   sql     The SQL script to execute.
         * \return  Returns true if succeeds to execute the SQL script.
         **/
        bool execute(const areg::String & sql) override;

        /**
         * \brief   Call if need to make multiple operation. This call starts the transaction,
         *          that is required either commit or rollback call to complete the transaction.
         **/
        bool begin() override;

        /**
         * \brief   Commits or rolls back the SqliteDatabase changes and returns true if succeeded.
         * \param   doCommit    If true, the SqliteDatabase engine should commit the changes.
         *                      Otherwise, the SqliteDatabase engine should rollback the changes.
         * \return  Returns true if operation succeeded. Otherwise, returns false.
         **/
        bool commit(bool doCommit) override;

    /************************************************************************/
    // LogDatabaseEngine class overrides.
    /************************************************************************/

        /**
         * \brief   Returns true if the database and the log tables are initialized,
         *          and ready to log messages.
         **/
        bool areTablesInitialized() const override;

        /**
         * \brief   Called when logging message should be saved in the database.
         * \param   message     The structure of the message to log.
         * \return  Returns true if succeeded to save the log in the database.
         **/
        bool logMessage(const areg::LogEntry & message) override;

        /**
         * \brief   Called when need to log information about log source instance.
         * \param   instance    The structure of the logging message source to save in database.
         * \param   timestamp   The timestamp to register when the instance is logged.
         * \return  Returns true if succeeded to save the log instance in the database.
         **/
        bool logInstanceConnected(const areg::ConnectedInstance & instance, const areg::DateTime & timestamp) override;

        /**
         * \brief   Called when an instance of log source is disconnected.
         *          This call should as well automatically deactivate the log scopes.
         * \param   cookie      The cookie ID of the instance to mark as disconnected.
         * \param   timestamp   The deactivation timestamp to set.
         * \return  Returns true if operation succeeded.
         **/
        bool logInstanceDisconnected(const ITEM_ID & cookie, const areg::DateTime & timestamp) override;

        /**
         * \brief   Called when need to log the information of the scope in the database.
         * \param   scope       The log scope information to save in the database.
         * \param   cookie      The cookie of the log scope owner instance.
         * \param   timestamp   The timestamp to register when the scope is logged.
         * \return  Returns true if succeeded to save the log scope in the database.
         **/
        bool logScopeActivate(const areg::ScopeEntry & scope, const ITEM_ID & cookie, const areg::DateTime & timestamp) override;

        /**
         * \brief   Called when need to log the information of the scope in the database.
         * \param   scopeName   The name of the scope.
         * \param   scopeId     The ID of the scope.
         * \param   scopePrio   The log priority of the scope
         * \param   cookie      The cookie of the log scope owner instance.
         * \param   timestamp   The timestamp to register when the scope is logged.
         * \return  Returns true if succeeded to save the log scope in the database.
         **/
        bool logScopeActivate(const areg::String & scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID & cookie, const areg::DateTime & timestamp) override;

        /**
         * \brief   Called when need to log the information of the list of scopes in the database.
         * \param   scopes      The information of the list of the log scope to save in the database.
         * \param   cookie      The cookie of the log scope owner instance.
         * \param   timestamp   The timestamp to register when the scope is logged.
         * \return  Returns the number of scope entries saved in the database.
         **/
        uint32_t logScopesActivate(const areg::ScopeNames& scopes, const ITEM_ID& cookie, const areg::DateTime& timestamp) override;

        /**
         * \brief   Call to deactivate all scopes related with the specified cookie ID.
         * \param   cookie      The cookie ID, which scopes should be marked as deactivated.
         * \param   timestamp   The deactivation timestamp to set.
         * \return  Returns true if operation succeeded.
         **/
        bool logScopesDeactivate(const ITEM_ID & cookie, const areg::DateTime & timestamp) override;

        /**
         * \brief   Call to deactivate a single scope related with the specified cookie ID.
         * \param   cookie      The cookie ID, which scope should be marked as deactivated.
         * \param   scopeId     The scope ID to mark as deactivated.
         * \param   timestamp   The deactivation timestamp to set.
         * \return  Returns true if operation succeeded.
         **/
        bool logScopeDeactivate(const ITEM_ID & cookie, uint32_t scopeId, const areg::DateTime & timestamp) override;

        /**
         * \brief   Rolls back the database changes and returns true if succeeded.
         **/
        bool rollback() override;

    //////////////////////////////////////////////////////////////////////////
    // Attributes and operations
    //////////////////////////////////////////////////////////////////////////

        /**
         * \brief   Call to query and get list of names of connected instances from log database.
         * \param[out]  names   On output, the vector contains names of connected instances.
         **/
        void getLogInstanceNames(std::vector<areg::String>& names);
        std::vector<areg::String> getLogInstanceNames();

        /**
         * \brief   Call to query and get list of IDs of connected instances from log database.
         * \param[out]  ids     On output, the vector contains IDs of connected instances.
         **/
        void getLogInstances(std::vector<ITEM_ID>& ids);
        std::vector<ITEM_ID> getLogInstances();

        /**
         * \brief   Call to query and get list of names of threads of the connected instances from log database.
         * \param[out]  names   On output, the vector contains names of threads of connected instances.
         **/
        void getLogThreadNames(std::vector<areg::String>& names);
        std::vector<areg::String> getLogThreadNames();

        /**
         * \brief   Call to query and get list of IDs of threads of the connected instances from log database.
         * \param[out]  ids     On output, the vector contains IDs of threads of connected instances.
         **/
        void getLogThreads(std::vector<ITEM_ID>& ids);
        std::vector<ITEM_ID> getLogThreads();

        /**
         * \brief   Call to get the list of log priorities.
         * \param   names   On output, the vector contains names of log priorities.
         **/
        void getPriorityNames(std::vector<areg::String>& names);
        std::vector<areg::String> getPriorityNames();

        /**
         * \brief   Call to query and get information of connected instances from log database.
         *          This query will receive list of all registered instances.
         * \param[out]  infos   On output, the vector contains information of connected instances.
         **/
        void getLogInstanceInfos(std::vector< areg::ConnectedInstance>& infos);
        std::vector< areg::ConnectedInstance> getLogInstanceInfos();

        /**
         * \brief   Call to query and get information of log scopes of specified instance from log database.
         *          This query will receive list of all registered scopes.
         * \param[out]  scopes  On output, the vector contains information of log scopes.
         * \param[in]   instID  The ID of the instance.
         **/
        void getLogInstScopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId);
        std::vector<areg::ScopeEntry> getLogInstScopes(ITEM_ID instId);

        /**
         * \brief   Call to get all log messages from log database.
         * \param[out]  messages    On output, the vector contains all log messages.
         **/
        void getLogMessages(std::vector<areg::SharedBuffer>& messages);
        std::vector<areg::SharedBuffer> getLogMessages();

        /**
         * \brief   Call to get log messages of the specified instance from log database.
         *          If `instId` is `areg::COOKIE_ANY` it receives the list of all instances
         *          similar to the call to `getLogMessages()`.
         * \param[out]  messages    On output, the vector contains log messages of the specified instance.
         * \param[in]   instId      The ID of the instance to get log messages.
         *                          If `areg::COOKIE_ANY` it receives log messages of all instances.
         **/
        void getLogInstMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId = areg::COOKIE_ANY);
        std::vector<areg::SharedBuffer> getLogInstMessages(ITEM_ID instId = areg::COOKIE_ANY);

        /**
         * \brief   Call to get log messages of the specified scope from log database.
         *          If `scopeId` is `0` it receives the list of all scopes
         *          similar to the call to `getLogMessages()`.
         * \param[out]  messages    On output, the vector contains log messages of the specified scope.
         * \param[in]   scopeId     The ID of the scope to get log messages.
         *                          If `0` it receives log messages of all scopes.
         **/
        void getLogScopeMessages(std::vector<areg::SharedBuffer>& messages, uint32_t scopeId = 0);
        std::vector<areg::SharedBuffer> getLogScopeMessages(uint32_t scopeId = 0);

        /**
         * \brief   Call to get log messages of the specified instance and log scope ID from log database.
         *          If `instId` is `areg::COOKIE_ANY` and `scopeId` is `0`, it receives the list of all logs
         *          similar to the call to `getLogMessages()`.
         * \param[out]  messages    On output, the vector contains log messages of the specified instance and scope.
         * \param[in]   instId      The ID of the instance to get log messages.
         *                          If `areg::COOKIE_ANY` it receives log messages of all instances.
         * \param[in]   scopeId     The ID of the scope to get log messages.
         *                          If `0` it receives log messages of all scopes.
         **/
        void getLogMessages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);
        std::vector<areg::SharedBuffer> getLogMessages(ITEM_ID instId, uint32_t scopeId);

        /**
         * \brief   Call to get log scopes using SQLite Statement object. The SQLite Statement should be already initialized
         *          and the parameters should be bound, if there is any. The method will extract the log scopes from the statement
         *          up to the specified maximum number of entries or all if the `maxEntries` is `-1`.
         * \param[out]  scopes      The vector to fill with log scopes. The extracted scopes will be added to the existing vector.
         * \param[in]   stmt        The SQLite Statement object to extract log scopes. The Statement should be already initialized
         *                          and the parameters should be bound, if there is any.
         * \param[in]   maxEntries  The maximum number of entries to extract. If `-1`, it extracts all entries.
         * \return  Returns number of entries added to the vector.
         **/
        static int32_t getLogInstScopes(std::vector<areg::ScopeEntry>& scopes, aregext::SqliteStatement& stmt, int32_t maxEntries = -1);

        /**
         * \brief   Call to get log messages using SQLite Statement object. The SQLite Statement should be already initialized
         *          and the parameters should be bound, if there is any. The method will extract the log messages from the statement
         *          up to the specified maximum number of entries or all if the `maxEntries` is `-1`.
         * \param[out]   logs        The vector to fill with log messages. The extracted messages will be added to the existing vector.
         * \param[in]   stmt        The SQLite Statement object to extract log messages. The Statement should be already initialized
         *                          and the parameters should be bound, if there is any.
         * \param[in]   maxEntries  The maximum number of entries to extract. If `-1`, it extracts all entries.
         * \return  Returns number of entries added to the vector.
         **/
        static int32_t getLogMessages(std::vector<areg::SharedBuffer>& logs, aregext::SqliteStatement& stmt, int32_t maxEntries = -1);

        /**
         * \brief   Fills log instances in the specified array. The array should be initialized and it should have enough space to set data.
         *          The method does not append instance to the array. Use `countLogInstances()` method to know the initialized size of array to re-size.
         *          On output, the `infos` array will not change the size and will contain instance information extracted from database.
         * \param[in,out]   infos       The array to set instance information from database. It should have enough space to set data.
         *                              On output, this array will not change the size.
         * \param[in]       stmt        The SQLite Statement object to extract instance information. The Statement should be already initialized
         *                              and prepared to extract instance information.
         * \return  Returns number of entries set in the array.
         **/
        static int32_t fillLogInstances(std::vector< areg::ConnectedInstance>& infos, aregext::SqliteStatement& stmt);

        /**
         * \brief   Fills scope data in the specified array. The array should be initialized and it should have enough space to set data.
         *          The method does not append scope to the array. Use `countScopeEntries()` method to know the initialized size of array to re-size.
         *          On output, the `scopes` array will not change the size and it will contain scope information starting at position `startAt`
         *          and maximum `maxEntries`, if this number is positive.
         *          This method can be called in the loop until the returns number is equal to `maxEntries`.
         * \param[in,out]   scopes      The array to set scope information from database. It should have enough space to set data.
         *                              On output, this array will not change the size and will contain scope information starting
         *                              from position `startAt` with maximum `maxEntries` number of entries, if it is positive number.
         * \param[in]   stmt            The SQLite Statement object to extract scope information. The Statement should be already initialized
         *                              and prepared to extract scope information.
         * \param[in]   startAt         The zero-based position to start to fill scope data. The first call in the loop should have position 0.
         * \param[in]   maxEntries      The maximum number of entries to extract. If `-1`, it extracts all entries.
         * \return  Returns number of entries set in the array.
         **/
        static int32_t fillInstScopes(std::vector<areg::ScopeEntry>& scopes, aregext::SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries = -1);

        /**
         * \brief   Fills log message data in the specified array. The array should be initialized and it should have enough space to set data.
         *          The method does not append log message to the array. Use `countLogEntries()` method to know the initialized size of array to re-size.
         *          On output, the `logs` array will not change the size and it will contain log messages starting at position `startAt`
         *          and maximum `maxEntries`, if this number is positive.
         *          This method can be called in the loop until the returns number is equal to `maxEntries`.
         * \param[in,out]   logs        The array to set log message information from database. It should have enough space to set data.
         *                              On output, this array will not change the size and will contain log messages starting
         *                              from position `startAt` with maximum `maxEntries` number of entries, if it is positive number.
         * \param[in]   stmt            The SQLite Statement object to extract log messages. The Statement should be already initialized
         *                              and prepared to extract log messages.
         * \param[in]   startAt         The zero-based position to start to fill log messages. The first call in the loop should have position 0.
         * \param[in]   maxEntries      The maximum number of entries to extract. If `-1`, it extracts all entries.
         * \return  Returns number of entries set in the array.
         **/
        static int32_t fillLogMessages(std::vector<areg::SharedBuffer>& logs, aregext::SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries = -1);

        /**
         * \brief   Call to setup statement to read the list of logging scopes from log database.
         *          The statement will fetch all scopes if `instId` is `areg::TARGET_ALL`.
         *          The statement object should be already initialized and bind with the log database.
         *          The logging database should be opened for reading data.
         * \param[out]  stmt    The SQLite statement object bound with SQLite logging database.
         * \param[in]   instId  The ID of the instance to bind to fetch scopes. If fetches all scopes if equal to `areg::TARGET_ALL`.
         * @return  Returns number of scopes of specified instance.
         **/
        uint32_t setupStatementReadScopes(aregext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Call to setup statement to read the list of logs from logging database.
         *          The statement will fetch all logs if `instId` is `areg::TARGET_ALL`.
         *          The statement object should be already initialized and bind with the log database.
         *          The logging database should be opened for reading data.
         * \param[out]  stmt    The SQLite statement object bound with SQLite logging database.
         * \param[in]   instId  The ID of the instance to bind to fetch scopes. If fetches all scopes if equal to `areg::TARGET_ALL`.
         * @return  Returns number of log messages of specified instance ID.
         **/
        uint32_t setupStatementReadLogs(aregext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Sets up the log filters
         * \param   instId  The ID of instance to filter.
         * \param   filter  The scope prio filters to setup
         * \return  Returns number of log entries after applying filter.
         **/
        uint32_t setupFilterLogs(ITEM_ID instId, const areg::ArrayList<ScopeFilter>& filter);

        /**
         * \brief   Sets up the statement to extract filtered logs from database for the given instance and returns the number of filtered logs to extract.
         * \param[out]  stmt    The statement object to use to extract logs from database.
         * \param[in]   instId  The ID of the instance to apply the filter or areg::TARGET_ALL if the filter is applied to all instances.
         * \return  Returns number of log entries after applying filter.
         **/
        uint32_t setupStatementReadFilterLogs(aregext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Returns number of log messages of specified instance ID.
         *          Returns number of all log messages if the instance ID is `areg::TARGET_ALL`.
         * \param   instId  The ID of the instance to get log messages. If `areg::TARGET_ALL` extracts all log messages.
         **/
        uint32_t countLogEntries(ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Returns number of scopes of specified instance ID.
         *          Returns number of all scopes if the instance ID is `areg::TARGET_ALL`.
         * \param   instId  The ID of the instance to get scope information. If `areg::TARGET_ALL` extracts all scopes.
         **/
        uint32_t countScopeEntries(ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Returns number of log instances.
         **/
        uint32_t countLogInstances();

        /**
         * \brief   Returns number of filtered log messages of specified instance ID.
         **/
        uint32_t countFilterLogs(ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Resets the logging priority filter mask of the specified instance ID or for all instances.
         *          Returns true if operation succeeded.
         **/
        bool resetFilterMask(ITEM_ID instId = areg::TARGET_ALL);

        /**
         * \brief   Disables the logging priority filter mask of the specified instance ID or for all instances.
         *          Returns true if operation succeeded.
         **/
        bool disableFilterMask(ITEM_ID instId = areg::TARGET_ALL);

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:

        /**
         * \brief   Opens or creates the specified database file.
         *          The path can be relative or absolute, it may as contain the mask.
         * \param   dbPath      The relative or absolute path the database file.
         * \param   readOnly    If true, the database engine should connect in read-only mode.
         * \return  Returns true if succeeded to open or create the database file.
         **/
        inline bool _open(const areg::String& dbPath, bool readOnly);

        /**
         * \brief   In the opened database file, creates the tables required to save logs.
         **/
        inline void _createTables();

        /**
         * \brief   In the opened database file, creates the indexes required by optimize operations.
         **/
        inline void _createIndexes();

        /**
         * \brief   Logs the initial information in the database like logging version and application name.
         **/
        inline void _initialize();

        /**
         * \brief   Extracts the log message from the SqliteStatement and copies it to the SharedBuffer.
         * \param   stmt    The SqliteStatement to extract the log message.
         * \param   buf     The SharedBuffer to copy the log message.
         **/
        inline static void _copyLogMessage(aregext::SqliteStatement& stmt, areg::SharedBuffer & buf);

        /**
         * \brief   Extracts the log instance from the SqliteStatement and copies it to the areg::ConnectedInstance.
         * \param   stmt    The SqliteStatement to extract the log instance.
         * \param   inst    The areg::ConnectedInstance to copy the log instance.
         **/
        inline static void _copyLogInstances(aregext::SqliteStatement& stmt, areg::ConnectedInstance & inst);

        /**
         * \brief   Extracts the log scope from the SqliteStatement and copies it to the areg::ScopeEntry.
         * \param   stmt    The SqliteStatement to extract the log scope.
         * \param   scope   The areg::ScopeEntry to copy the log scope.
         **/
        inline static void _copyLogScopes(aregext::SqliteStatement& stmt, areg::ScopeEntry& scope);

        /**
         * \brief   Updates the filter information for the specified instance.
         * \param   instId  The instance ID to apply filters
         * \param   filter  The list of scopes and filter mask to apply
         * \return  Returns true if operation succeeded.
         **/
        inline uint32_t _updaeFilterLogScopes(ITEM_ID instId, const areg::ArrayList<ScopeFilter>& filter);

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    protected:
        //!< The path to the SQLite database file.
        aregext::SqliteDatabase  mDatabase;

        //!< The statement to log messages in the database.
        aregext::SqliteStatement mStmtLogs;

        //!< The initial path to the SQLIte database file. The path may contain mask like timestamp.
        areg::String          mDbInitPath;

        //!< Flag, indicating whether the database and data tables are initialized or not.
        bool            mIsInitialized;

        //!< Flag, indicating whether the database logging is enabled or not.
        bool            mDbLogEnabled;

        //!< Mutex to protect database operations.
        areg::Mutex           mLock;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE(LogSqliteDatabase);
    };

    //////////////////////////////////////////////////////////////////////////
    // LogSqliteDatabase class inline methods.
    //////////////////////////////////////////////////////////////////////////

    bool LogSqliteDatabase::isDabataseLoggingEnabled() const
    {
        return mDbLogEnabled;
    }

    inline void LogSqliteDatabase::setDatabaseLoggingEnabled(bool enable)
    {
        mDbLogEnabled = enable;
    }

    inline const areg::String& LogSqliteDatabase::getDatabasePath() const
    {
        return mDatabase.getPath();
    }

    inline const areg::String& LogSqliteDatabase::getInitialDatabasePath() const
    {
        return mDbInitPath;
    }

    inline aregext::SqliteDatabase& LogSqliteDatabase::getDatabase()
    {
        return mDatabase;
    }

    inline const aregext::SqliteDatabase& LogSqliteDatabase::getDatabase() const
    {
        return mDatabase;
    }

    inline aregext::SqliteStatement& LogSqliteDatabase::getStatement()
    {
        return mStmtLogs;
    }

    inline const aregext::SqliteStatement& LogSqliteDatabase::getStatement() const
    {
        return mStmtLogs;
    }

} // namespace aregext
#endif  // AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
