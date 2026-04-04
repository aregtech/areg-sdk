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
#include "areg/base/areg_global.h"

#include "aregextend/db/SqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <vector>

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The logging database engine, responsible to log messages in the database.
 **/
class LogSqliteDatabase final : public LogDatabaseEngine
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
     * \brief   Returns the SQL query to read instance dependent log scopes from the database. The
     *          ID of the instance can be specified (bound) to read scopes.
     **/
    static String read_scopes_query();

    /**
     * \brief   Returns the SQL query to read all instances from the database.
     **/
    static String read_instances_query();

    /**
     * \brief   Returns the SQL query to read all log messages from the database.
     **/
    static String read_all_log_messages_query();

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
     * \brief   Returns true if logging in the database is enabled. If logging is disabled, no
     *          database operation is performed.
     **/
    [[nodiscard]]
    inline bool is_database_logging_enabled() const noexcept;

    /**
     * \brief   Enables or disables the database logging. This flag should be set before
     *          initializing database, where the database file and data tables are created and
     *          initialized.
     *
     * \param   enable      Flag, indicating whether the logging in the database is enabled or not.
     **/
    inline void set_database_logging_enabled(bool enable) noexcept;

    /**
     * \brief   Returns database file path.
     **/
    [[nodiscard]]
    inline const String& database_path() const noexcept;

    /**
     * \brief   Returns the initial database file path. The initial file path may contain mask like
     *          timestamp, so that each time database is disconnected and connected again with the
     *          empty file path, it creates new file.
     **/
    [[nodiscard]]
    inline const String& initial_database_path() const noexcept;

    /**
     * \brief   Returns SQLite database object.
     **/
    [[nodiscard]]
    inline SqliteDatabase& database() noexcept;

    /**
     * \brief   Returns SQLite database object.
     **/
    [[nodiscard]]
    inline const SqliteDatabase& database() const noexcept;

    /**
     * \brief   Returns SQLite database object.
     **/
    [[nodiscard]]
    inline SqliteStatement& statement() noexcept;

    /**
     * \brief   Returns SQLite database object.
     **/
    [[nodiscard]]
    inline const SqliteStatement& statement() const noexcept;

    /**
     * \brief   Returns true if the specified table exists or not.
     *
     * \param   table       The name of the table to check.
     * \param   master      The name of the schema to check. If nullptr, the default schema is used.
     * \return  Returns true if specified table exists in the specified schema.
     **/
    [[nodiscard]]
    bool table_exists(const char* table, const char* master = nullptr);

    /**
     * \brief   Drops specified table.
     * \param   table   The name of the table to drop.
     **/
    bool drop_table(const char* table);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// DatabaseEngine class overrides.
/************************************************************************/

    /**
     * \brief   Returns true if SqliteDatabase engine is opened and operable. Otherwise, returns
     *          false.
     **/
    bool is_operable() const noexcept final;

    /**
     * \brief   Connects to the specified database.
     *
     * \param   dbPath      The path to the database. If needed, the path may contain file path or
     *                      URL, user name and password. It is up to Database engine to parse the
     *                      path and initialize the connection. If the parameter is empty, it should
     *                      take the data from the 'areg.init' configuration file.
     * \param   readOnly    If true, the database engine should connect in read-only mode.
     * \return  Returns true if succeeded to connect. Otherwise, returns false.
     **/
    bool connect(const String& dbPath, bool readOnly) final;

    /**
     * \brief   Disconnects connected SqliteDatabase.
     **/
    void disconnect() final;

    /**
     * \brief   Execute the SQL script.
     *
     * \param   sql     The SQL script to execute.
     * \return  Returns true if succeeds to execute the SQL script.
     **/
    bool execute(const String & sql) final;

    /**
     * \brief   Call to start a transaction for multiple operations. Requires a matching commit or
     *          rollback call.
     **/
    bool begin() final;

    /**
     * \brief   Commits or rolls back the SqliteDatabase changes and returns true if succeeded.
     *
     * \param   doCommit    If true, the SqliteDatabase engine should commit the changes. Otherwise,
     *                      the SqliteDatabase engine should rollback the changes.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     **/
    bool commit(bool doCommit) final;

/************************************************************************/
// LogDatabaseEngine class overrides.
/************************************************************************/

    /**
     * \brief   Returns true if the database and the log tables are initialized, and ready to log
     *          messages.
     **/
    bool tables_initialized() const noexcept final;

    /**
     * \brief   Called when logging message should be saved in the database.
     *
     * \param   message     The structure of the message to log.
     * \return  Returns true if succeeded to save the log in the database.
     **/
    bool log_message(const areg::LogEntry & message) final;

    /**
     * \brief   Called when need to log information about log source instance.
     *
     * \param   instance        The structure of the logging message source to save in database.
     * \param   timestamp       The timestamp to register when the instance is logged.
     * \return  Returns true if succeeded to save the log instance in the database.
     **/
    bool log_instance_connected(const areg::ConnectedInstance & instance, const DateTime & timestamp) final;

    /**
     * \brief   Called when an instance of log source is disconnected. This call should as well
     *          automatically deactivate the log scopes.
     *
     * \param   cookie          The cookie ID of the instance to mark as disconnected.
     * \param   timestamp       The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    bool log_instance_disconnected(const ITEM_ID & cookie, const DateTime & timestamp) final;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     *
     * \param   scope           The log scope information to save in the database.
     * \param   cookie          The cookie of the log scope owner instance.
     * \param   timestamp       The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    bool log_scope_activate(const areg::ScopeEntry & scope, const ITEM_ID & cookie, const DateTime & timestamp) final;

    /**
     * \brief   Called when need to log the information of the scope in the database.
     *
     * \param   scopeName       The name of the scope.
     * \param   scopeId         The ID of the scope.
     * \param   scopePrio       The log priority of the scope.
     * \param   cookie          The cookie of the log scope owner instance.
     * \param   timestamp       The timestamp to register when the scope is logged.
     * \return  Returns true if succeeded to save the log scope in the database.
     **/
    bool log_scope_activate(const String & scopeName, uint32_t scopeId, uint32_t scopePrio, const ITEM_ID & cookie, const DateTime & timestamp) final;

    /**
     * \brief   Called when need to log the information of the list of scopes in the database.
     *
     * \param   scopes          The information of the list of the log scope to save in the
     *                          database.
     * \param   cookie          The cookie of the log scope owner instance.
     * \param   timestamp       The timestamp to register when the scope is logged.
     * \return  Returns the number of scope entries saved in the database.
     **/
    uint32_t log_scopes_activate(const areg::ScopeNames& scopes, const ITEM_ID& cookie, const DateTime& timestamp) final;

    /**
     * \brief   Call to deactivate all scopes related with the specified cookie ID.
     *
     * \param   cookie          The cookie ID, which scopes should be marked as deactivated.
     * \param   timestamp       The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    bool log_scopes_deactivate(const ITEM_ID & cookie, const DateTime & timestamp) final;

    /**
     * \brief   Call to deactivate a single scope related with the specified cookie ID.
     *
     * \param   cookie          The cookie ID, which scope should be marked as deactivated.
     * \param   scopeId         The scope ID to mark as deactivated.
     * \param   timestamp       The deactivation timestamp to set.
     * \return  Returns true if operation succeeded.
     **/
    bool log_scope_deactivate(const ITEM_ID & cookie, uint32_t scopeId, const DateTime & timestamp) final;

    /**
     * \brief   Rolls back the database changes and returns true if succeeded.
     **/
    bool rollback() final;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Call to query and get list of names of connected instances from log database.
     * \param[out] names       On output, the vector contains names of connected instances.
     **/
    void log_instance_names(std::vector<areg::String>& names);

    /**
     * \brief   Call to query and get list of names of connected instances from log database.
     **/
    [[nodiscard]]
    std::vector<areg::String> log_instance_names();

    /**
     * \brief   Call to query and get list of IDs of connected instances from log database.
     * \param[out] ids     On output, the vector contains IDs of connected instances.
     **/
    void log_instances(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to query and get list of IDs of connected instances from log database.
     **/
    [[nodiscard]]
    std::vector<ITEM_ID> log_instances();

    /**
     * \brief   Call to query and get list of names of threads of the connected instances from log database.
     * \param[out] names       On output, the vector contains names of threads of connected instances.
     **/
    void log_thread_names(std::vector<areg::String>& names);

    /**
     * \brief   Call to query and get list of names of threads of the connected instances from log database.
     **/
    [[nodiscard]]
    std::vector<String> log_thread_names();

    /**
     * \brief   Call to query and get list of IDs of threads of the connected instances from log database.
     * \param[out] ids     On output, the vector contains IDs of threads of connected instances.
     **/
    void log_threads(std::vector<ITEM_ID>& ids);

    /**
     * \brief   Call to query and get list of IDs of threads of the connected instances from log database.
     **/
    [[nodiscard]]
    std::vector<ITEM_ID> log_threads();

    /**
     * \brief   Call to get the list of log priorities.
     * \param   names       On output, the vector contains names of log priorities.
     **/
    void priority_names(std::vector<areg::String>& names);

    /**
     * \brief   Call to get the list of log priorities.
     **/
    [[nodiscard]]
    std::vector<areg::String> priority_names();

    /**
     * \brief   Call to query and get information of connected instances from log database. This
     *          query will receive list of all registered instances.
     *
     * \param[out] infos       On output, the vector contains information of connected instances.
     **/
    void log_instance_infos(std::vector< areg::ConnectedInstance>& infos);

    /**
     * \brief   Call to query and get information of connected instances from log database. This
     *          query will receive list of all registered instances.
     **/
    [[nodiscard]]
    std::vector<areg::ConnectedInstance> log_instance_infos();

    /**
     * \brief   Call to query and get information of log scopes of specified instance from log
     *          database. This query will receive list of all registered scopes.
     *
     * \param[out]  scopes  On output, the vector contains information of log scopes.
     * \param       instId  The ID of the instance to get log scopes.
     **/
    void log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, ITEM_ID instId);

    /**
     * \brief   Call to query and get information of log scopes of specified instance from log
     *          database. This query will receive list of all registered scopes.
     *
     * \param   instId      The ID of the instance to get log scopes.
     **/
    [[nodiscard]]
    std::vector<areg::ScopeEntry> log_inst_scopes(ITEM_ID instId);

    /**
     * \brief   Call to get all log messages from log database.
     *
     * \param[out] messages    On output, the vector contains all log messages.
     **/
    void log_messages(std::vector<SharedBuffer>& messages);

    /**
     * \brief   Call to get all log messages from log database.
     **/
    [[nodiscard]]
    std::vector<areg::SharedBuffer> log_messages();

    /**
     * \brief   Call to get log messages of the specified instance from log database. If `instId` is
     *          `areg::COOKIE_ANY` it receives the list of all instances similar to the call to `log_messages()`.
     *
     * \param[out]  messages    On output, the vector contains log messages of the specified instance.
     * \param       instId      The ID of the instance to get log messages. If `areg::COOKIE_ANY`
     *                          it receives log messages of all instances.
     **/
    void log_inst_messages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId = areg::COOKIE_ANY);

    /**
     * \brief   Call to get log messages of the specified instance from log database. If `instId` is
     *          `areg::COOKIE_ANY` it receives the list of all instances similar to the call to `log_messages()`.
     *
     * \param   instId  The ID of the instance to get log messages. If `areg::COOKIE_ANY`
     *                  it receives log messages of all instances.
     **/
    [[nodiscard]]
    std::vector<areg::SharedBuffer> log_inst_messages(ITEM_ID instId = areg::COOKIE_ANY);

    /**
     * \brief   Call to get log messages of the specified scope from log database. If `scopeId` is
     *          `0` it receives the list of all scopes similar to the call to `log_messages()`.
     *
     * \param[out]  messages    On output, the vector contains log messages of the specified scope.
     * \param       scopeId     The ID of the scope to get log messages. If `0` it receives log messages of all scopes.
     **/
    void log_scope_messages(std::vector<areg::SharedBuffer>& messages, uint32_t scopeId = 0);

    /**
     * \brief   Call to get log messages of the specified scope from log database. If `scopeId` is
     *          `0` it receives the list of all scopes similar to the call to `log_messages()`.
     *
     * \param   scopeId     The ID of the scope to get log messages. If `0` it receives log messages of all scopes.
     **/
    [[nodiscard]]
    std::vector<areg::SharedBuffer> log_scope_messages(uint32_t scopeId = 0);

    /**
     * \brief   Call to get log messages of the specified instance and log scope ID from log
     *          database. If `instId` is `areg::COOKIE_ANY` and `scopeId` is `0`, it receives
     *          the list of all logs similar to the call to `log_messages()`.
     *
     * \param[out] messages    On output, the vector contains log messages of the specified instance
     *                         and scope.
     * \param   instId      The ID of the instance to get log messages. If `areg::COOKIE_ANY`
     *                      it receives log messages of all instances.
     * \param   scopeId     The ID of the scope to get log messages. If `0` it receives log messages of all scopes.
     **/
    void log_messages(std::vector<areg::SharedBuffer>& messages, ITEM_ID instId, uint32_t scopeId);

    /**
     * \brief   Call to get log messages of the specified instance and log scope ID from log
     *          database. If `instId` is `areg::COOKIE_ANY` and `scopeId` is `0`, it receives
     *          the list of all logs similar to the call to `log_messages()`.
     *
     * \param   instId      The ID of the instance to get log messages. If `areg::COOKIE_ANY`
     *                      it receives log messages of all instances.
     * \param   scopeId     The ID of the scope to get log messages. If `0` it receives log messages of all scopes.
     **/
    [[nodiscard]]
    std::vector<areg::SharedBuffer> log_messages(ITEM_ID instId, uint32_t scopeId);

    /**
     * \brief   Call to get log scopes using SQLite Statement object. The SQLite Statement should be
     *          already initialized and the parameters should be bound, if there is any. The method
     *          will extract the log scopes from the statement up to the specified maximum number of
     *          entries or all if the `maxEntries` is `-1`.
     *
     * \param[out] scopes   The vector to fill with log scopes. The extracted scopes will be
     *                      added to the existing vector.
     * \param   stmt        The SQLite Statement object to extract log scopes. The Statement should
     *                      be already initialized and the parameters should be bound, if there is any.
     * \param   maxEntries  The maximum number of log scopes to extract from the statement. If this number is negative,
     *                      it should extract all available log scopes.
     * \return  Returns number of entries added to the vector.
     **/
    static int32_t log_inst_scopes(std::vector<areg::ScopeEntry>& scopes, areg::ext::SqliteStatement& stmt, int32_t maxEntries = -1);

    /**
     * \brief   Call to get log messages using SQLite Statement object. The SQLite Statement should
     *          be already initialized and the parameters should be bound, if there is any. The
     *          method will extract the log messages from the statement up to the specified maximum
     *          number of entries or all if the `maxEntries` is `-1`.
     *
     * \param[out] logs     The vector to fill with log messages. The extracted messages will be
     *                      added to the existing vector.
     * \param   stmt        The SQLite Statement object to extract log messages. The Statement should be
     *                      already initialized and the parameters should be bound, if there is any.
     * \param   maxEntries  The maximum number of log messages to extract from the statement. If this number is negative,
     *                      it should extract all available log messages.
     * \return  Returns number of entries added to the vector.
     **/
    static int32_t log_messages(std::vector<areg::SharedBuffer>& logs, areg::ext::SqliteStatement& stmt, int32_t maxEntries = -1);

    /**
     * \brief   Fills log instances in the specified array. The array should be initialized and it
     *          should have enough space to set data. The method does not append instance to the
     *          array. Use `count_log_instances()` method to know the initialized size of array to
     *          re-size. On output, the `infos` array will not change the size and will contain
     *          instance information extracted from database.
     *
     * \param[in,out] infos     The array to set instance information from database. It should
     *                          have enough space to set data. On output, this array will not
     *                          change the size.
     * \param   stmt            The SQLite Statement object to extract instance information. The
     *                          Statement should be already initialized and prepared to extract instance information.
     * \return  Returns number of entries set in the array.
     **/
    static int32_t fill_log_instances(std::vector<areg::ConnectedInstance>& infos, areg::ext::SqliteStatement& stmt);

    /**
     * \brief   Fills scope data in the specified array. The array should be initialized and it
     *          should have enough space to set data. The method does not append scope to the array.
     *          Use `count_scope_entries()` method to know the initialized size of array to re-size.
     *          On output, the `scopes` array will not change the size and it will contain scope
     *          information starting at position `startAt` and maximum `maxEntries`, if this number
     *          is positive. This method can be called in the loop until the returns number is equal
     *          to `maxEntries`.
     *
     * \param[in,out] scopes    The array to set scope information from database. It should have
     *                          enough space to set data. On output, this array will not change
     *                          the size and will contain scope information starting from position
     *                          `startAt` with maximum `maxEntries` number of entries, if it is
     *                          positive number.
     * \param   stmt            The SQLite Statement object to extract scope information. The Statement
     *                          should be already initialized and prepared to extract scope information.
     * \param   startAt         The zero-based position to start to fill scope data. The first call in
     *                          the loop should have position 0.
     * \param   maxEntries      The maximum number of scope entries to set in the array. If this number is negative,
     *                          it should fetch all remaining scope entries.
     * \return  Returns number of entries set in the array.
     **/
    static int32_t fill_inst_scopes(std::vector<areg::ScopeEntry>& scopes, areg::ext::SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries = -1);

    /**
     * \brief   Fills log message data in the specified array. The array should be initialized and
     *          it should have enough space to set data. The method does not append log message to
     *          the array. Use `count_log_entries()` method to know the initialized size of array to
     *          re-size. On output, the `logs` array will not change the size and it will contain
     *          log messages starting at position `startAt` and maximum `maxEntries`, if this number
     *          is positive. This method can be called in the loop until the returns number is equal
     *          to `maxEntries`.
     *
     * \param[in,out] logs      The array to set log message information from database. It should
     *                          have enough space to set data. On output, this array will not
     *                          change the size and will contain log messages starting from
     *                          position `startAt` with maximum `maxEntries` number of entries, if
     *                          it is positive number.
     * \param   stmt            The SQLite Statement object to extract log messages. The Statement
     *                          should be already initialized and prepared to extract log messages.
     * \param   startAt         The zero-based position to start to fill log messages. The first call in
     *                          the loop should have position 0.
     * \param   maxEntries      The maximum number of log messages to set in the array. If this number is negative,
     *                          it should fetch all remaining log messages.
     * \return  Returns number of entries set in the array.
     **/
    static int32_t fill_log_messages(std::vector<areg::SharedBuffer>& logs, areg::ext::SqliteStatement& stmt, uint32_t startAt, int32_t maxEntries = -1);

    /**
     * \brief   Call to setup statement to read the list of logging scopes from log database. The
     *          statement will fetch all scopes if `instId` is `areg::TARGET_ALL`. The
     *          statement object should be already initialized and bind with the log database. The
     *          logging database should be opened for reading data.
     *
     * \param[out]  stmt    The SQLite statement object bound with SQLite logging database.
     * \param       instId  The ID of instance to filter. If `areg::TARGET_ALL` is specified, it applies filter for all instances.
     * \return  Returns number of scopes of specified instance.
     **/
    uint32_t setup_statement_read_scopes(areg::ext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Call to setup statement to read the list of logs from logging database. The
     *          statement will fetch all logs if `instId` is `areg::TARGET_ALL`. The statement
     *          object should be already initialized and bind with the log database. The logging
     *          database should be opened for reading data.
     *
     * \param[out]  stmt    The SQLite statement object bound with SQLite logging database.
     * \param       instId  The ID of instance to filter. If `areg::TARGET_ALL` is specified, it applies filter for all instances.
     * \return  Returns number of log messages of specified instance ID.
     **/
    uint32_t setup_statement_read_logs(areg::ext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Sets up the log filters.
     *
     * \param   instId      The ID of instance to filter.
     * \param   filter      The scope prio filters to setup.
     * \return  Returns number of log entries after applying filter.
     **/
    uint32_t setup_filter_logs(ITEM_ID instId, const areg::ArrayList<ScopeFilter>& filter);

    /**
     * \brief   Sets up the statement to extract filtered logs from database for the given instance
     *          and returns the number of filtered logs to extract.
     *
     * \param[out]  stmt    The statement object to use to extract logs from database.
     * \param       instId  The ID of instance to filter. If `areg::TARGET_ALL` is specified, it applies filter for all instances.
     * \return  Returns number of log entries after applying filter.
     **/
    uint32_t setup_statement_read_filter_logs(areg::ext::SqliteStatement& stmt, ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Returns number of log messages of specified instance ID. Returns number of all log
     *          messages if the instance ID is `areg::TARGET_ALL`.
     *
     * \param   instId      The ID of instance to filter. If `areg::TARGET_ALL` is specified, it returns number of all log messages.
     **/
    [[nodiscard]]
    uint32_t count_log_entries(ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Returns number of scopes of specified instance ID. Returns number of all scopes if
     *          the instance ID is `areg::TARGET_ALL`.
     *
     * \param   instId      The ID of instance to filter. If `areg::TARGET_ALL` is specified, it returns number of all scopes.
     **/
    [[nodiscard]]
    uint32_t count_scope_entries(ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Returns number of log instances.
     **/
    [[nodiscard]]
    uint32_t count_log_instances();

    /**
     * \brief   Returns number of filtered log messages of specified instance ID.
     * \param   instId      The ID of instance to filter. If `areg::TARGET_ALL` is specified, it returns number of all log messages.
     **/
    [[nodiscard]]
    uint32_t count_filter_logs(ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Resets the logging priority filter mask of the specified instance ID or for all
     *          instances. Returns true if operation succeeded.
     *
     * \param   instId  The ID of instance to reset filter. If `areg::TARGET_ALL` is specified, it resets filter for all
     **/
    bool reset(ITEM_ID instId = areg::TARGET_ALL);

    /**
     * \brief   Disables the logging priority filter mask of the specified instance ID or for all
     *          instances. Returns true if operation succeeded.
     **/
    bool disable_filter_mask(ITEM_ID instId = areg::TARGET_ALL);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Opens or creates the specified database file. The path can be relative or absolute,
     *          it may as contain the mask.
     *
     * \param   dbPath      The relative or absolute path the database file.
     * \param   readOnly    If true, the database engine should connect in read-only mode.
     * \return  Returns true if succeeded to open or create the database file.
     **/
    inline bool _open(const areg::String& dbPath, bool readOnly);

    /**
     * \brief   In the opened database file, creates the tables required to save logs.
     **/
    inline void _create_tables() noexcept;

    /**
     * \brief   In the opened database file, creates the indexes required by optimize operations.
     **/
    inline void _create_indexes() noexcept;

    /**
     * \brief   Logs the initial information in the database like logging version and application
     *          name.
     **/
    inline void _initialize() noexcept;

    /**
     * \brief   Extracts the log message from the SqliteStatement and copies it to the SharedBuffer.
     *
     * \param   stmt    The SqliteStatement to extract the log message.
     * \param   buf     The SharedBuffer to copy the log message.
     **/
    inline static void _copy_log_message(areg::ext::SqliteStatement& stmt, areg::SharedBuffer& buf);

    /**
     * \brief   Extracts the log instance from the SqliteStatement and copies it to the
     *          areg::ConnectedInstance.
     *
     * \param   stmt    The SqliteStatement to extract the log instance.
     * \param   inst    The areg::ConnectedInstance to copy the log instance.
     **/
    inline static void _copy_log_instances(areg::ext::SqliteStatement& stmt, areg::ConnectedInstance & inst);

    /**
     * \brief   Extracts the log scope from the SqliteStatement and copies it to the
     *          areg::ScopeEntry.
     *
     * \param   stmt        The SqliteStatement to extract the log scope.
     * \param   scope       The areg::ScopeEntry to copy the log scope.
     **/
    inline static void _copy_log_scopes(areg::ext::SqliteStatement& stmt, areg::ScopeEntry& scope);

    /**
     * \brief   Updates the filter information for the specified instance.
     *
     * \param   instId      The instance ID to apply filters.
     * \param   filter      The list of scopes and filter mask to apply.
     * \return  Returns true if operation succeeded.
     **/
    inline uint32_t _update_filter_log_scopes(ITEM_ID instId, const areg::ArrayList<ScopeFilter>& filter);

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
protected:
    //!< The path to the SQLite database file.
    SqliteDatabase  mDatabase;

    //!< The statement to log messages in the database.
    SqliteStatement mStmtLogs;

    //!< The initial path to the SQLIte database file. The path may contain mask like timestamp.
    areg::String    mDbInitPath;

    //!< Flag, indicating whether the database and data tables are initialized or not.
    bool            mIsInitialized;

    //!< Flag, indicating whether the database logging is enabled or not.
    bool            mDbLogEnabled;

    //!< Mutex to protect database operations.
    areg::Mutex     mLock;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LogSqliteDatabase);
};

//////////////////////////////////////////////////////////////////////////
// LogSqliteDatabase class inline methods.
//////////////////////////////////////////////////////////////////////////

bool LogSqliteDatabase::is_database_logging_enabled() const noexcept
{
    return mDbLogEnabled;
}

inline void LogSqliteDatabase::set_database_logging_enabled(bool enable) noexcept
{
    mDbLogEnabled = enable;
}

inline const String& LogSqliteDatabase::database_path() const noexcept
{
    return mDatabase.path();
}

inline const String& LogSqliteDatabase::initial_database_path() const noexcept
{
    return mDbInitPath;
}

inline SqliteDatabase& LogSqliteDatabase::database() noexcept
{
    return mDatabase;
}

inline const SqliteDatabase& LogSqliteDatabase::database() const noexcept
{
    return mDatabase;
}

inline SqliteStatement& LogSqliteDatabase::statement() noexcept
{
    return mStmtLogs;
}

inline const SqliteStatement& LogSqliteDatabase::statement() const noexcept
{
    return mStmtLogs;
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_DB_LOGSQLITEDATABASE_HPP
