/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogSqliteDatabaseTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the log database: file mode, windows, row ids and markers.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace
{
    namespace fs = std::filesystem;
    using LogDb = areg::ext::LogSqliteDatabase;

    //! The first connection SQLite opened since the last _capture_connections(). A writer opens its
    //! own connection first, then the one of its background checkpoint.
    sqlite3* _lastDb{ nullptr };

    int _capture_db(sqlite3* db, const char** /*errMsg*/, const sqlite3_api_routines* /*api*/)
    {
        if (_lastDb == nullptr)
        {
            _lastDb = db;
        }

        return SQLITE_OK;
    }

    //! Makes SQLite report the next connection it opens to _capture_db().
    void _capture_connections()
    {
        _lastDb = nullptr;
        sqlite3_auto_extension(reinterpret_cast<void (*)(void)>(&_capture_db));
    }

    //! Returns the path of a database file in a directory of this process, with no file of an earlier run left.
    std::string _db_path(const char* name)
    {
        const fs::path dir{ fs::temp_directory_path() / ("areg-logdb-" + std::to_string(areg::Process::instance().id())) };
        std::error_code err;
        fs::create_directories(dir, err);
        const std::string path{ (dir / name).string() };
        for (const char* suffix : { "", "-wal", "-shm", "-journal" })
        {
            fs::remove(path + suffix, err);
        }

        return path;
    }

    bool _exists(const std::string& path)
    {
        std::error_code err;
        return fs::exists(path, err);
    }

    areg::LogEntry _entry(TIME64 timeCreated, uint32_t scopeId, ITEM_ID cookie, areg::LogPriority prio)
    {
        areg::LogEntry entry{ areg::LogMessageType::MessageText };
        entry.logMessagePrio = prio;
        entry.logCookie      = cookie;
        entry.logScopeId     = scopeId;
        entry.logTimestamp   = timeCreated;
        entry.logReceived    = timeCreated;
        std::strcpy(entry.logMessage, "log database test");
        entry.logMessageLen  = 17u;
        std::strcpy(entry.logThread, "TestThread");
        entry.logThreadLen   = 10u;
        std::strcpy(entry.logModule, "areg-unit-tests");
        entry.logModuleLen   = 15u;
        return entry;
    }

    //! Writes `count` rows in one transaction, row i created at `firstTime + i`.
    bool _write_rows(LogDb& db, uint32_t count, TIME64 firstTime)
    {
        bool result{ db.begin() };
        for (uint32_t i = 0; result && (i < count); ++i)
        {
            result = db.log_message(_entry(firstTime + i, 0u, 1u, areg::LogPriority::PrioInfo));
        }

        return db.commit(result) && result;
    }

    //! One row of the logs table, as the tests compare it.
    struct Row
    {
        uint32_t    id      { 0u };
        TIME64      time    { 0 };
        uint32_t    scope   { 0u };
        uint32_t    cookie  { 0u };
        uint32_t    prio    { 0u };
    };

    //! Reads every row of the file in time order, on a read only connection of its own.
    std::vector<Row> _rows_in_time_order(const std::string& path)
    {
        std::vector<Row> rows;
        sqlite3* db{ nullptr };
        sqlite3_stmt* stmt{ nullptr };
        if ((sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK) &&
            (sqlite3_prepare_v2(db, "SELECT id, time_created, scope_id, cookie_id, msg_prio FROM logs ORDER BY time_created, id;", -1, &stmt, nullptr) == SQLITE_OK))
        {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                Row row;
                row.id      = static_cast<uint32_t>(sqlite3_column_int64(stmt, 0));
                row.time    = static_cast<TIME64>(sqlite3_column_int64(stmt, 1));
                row.scope   = static_cast<uint32_t>(sqlite3_column_int64(stmt, 2));
                row.cookie  = static_cast<uint32_t>(sqlite3_column_int64(stmt, 3));
                row.prio    = static_cast<uint32_t>(sqlite3_column_int64(stmt, 4));
                rows.push_back(row);
            }
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return rows;
    }

    std::vector<uint32_t> _ids(const std::vector<Row>& rows)
    {
        std::vector<uint32_t> ids;
        for (const Row& row : rows)
        {
            ids.push_back(row.id);
        }

        return ids;
    }

    //! Runs a query of one value on a read only connection of its own and returns the value as text.
    std::string _query_text(const std::string& path, const char* sql)
    {
        std::string result;
        sqlite3* db{ nullptr };
        sqlite3_stmt* stmt{ nullptr };
        if ((sqlite3_open_v2(path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) == SQLITE_OK) &&
            (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) &&
            (sqlite3_step(stmt) == SQLITE_ROW) &&
            (sqlite3_column_text(stmt, 0) != nullptr))
        {
            result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return result;
    }

    //! Runs a query of one value on the given connection and returns the value as text.
    std::string _query_text(sqlite3* db, const char* sql)
    {
        std::string result;
        sqlite3_stmt* stmt{ nullptr };
        if ((sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) &&
            (sqlite3_step(stmt) == SQLITE_ROW) &&
            (sqlite3_column_text(stmt, 0) != nullptr))
        {
            result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }

        sqlite3_finalize(stmt);
        return result;
    }

    //! Returns the query plan of the newest statement of the connection, one step per line, after its SQL.
    std::string _newest_plan(sqlite3* db)
    {
        sqlite3_stmt* newest{ sqlite3_next_stmt(db, nullptr) };
        const std::string sql{ (newest != nullptr) && (sqlite3_sql(newest) != nullptr) ? sqlite3_sql(newest) : "" };
        std::string plan{ sql + "\n" };
        sqlite3_stmt* explain{ nullptr };
        if (sql.empty() == false && (sqlite3_prepare_v2(db, ("EXPLAIN QUERY PLAN " + sql).c_str(), -1, &explain, nullptr) == SQLITE_OK))
        {
            while (sqlite3_step(explain) == SQLITE_ROW)
            {
                plan += reinterpret_cast<const char*>(sqlite3_column_text(explain, 3));
                plan += "\n";
            }
        }

        sqlite3_finalize(explain);
        return plan;
    }

    //! Returns true if the plan reads the logs through the time index without sorting them again.
    bool _uses_time_index(const std::string& plan, bool seek)
    {
        const bool viaIndex{ plan.find("USING INDEX idx_logs_time") != std::string::npos ||
                             plan.find("USING COVERING INDEX idx_logs_time") != std::string::npos };
        const bool seeks{ (plan.find("SEARCH logs USING") != std::string::npos) || (plan.find("SEARCH l USING") != std::string::npos) };
        const bool sorts{ plan.find("TEMP B-TREE") != std::string::npos };
        return viaIndex && (sorts == false) && ((seek == false) || seeks);
    }

    //! Returns the number of statements of the connection that are still stepping.
    int _active_statements(sqlite3* db)
    {
        int result{ 0 };
        for (sqlite3_stmt* stmt = sqlite3_next_stmt(db, nullptr); stmt != nullptr; stmt = sqlite3_next_stmt(db, stmt))
        {
            result += sqlite3_stmt_busy(stmt) != 0 ? 1 : 0;
        }

        return result;
    }

    constexpr areg::LogPriority _prios[]{ areg::LogPriority::PrioDebug, areg::LogPriority::PrioInfo, areg::LogPriority::PrioWarning, areg::LogPriority::PrioError };

    //! Writes 10 000 rows of scopes 0..4, cookies 1 and 2, whose times repeat about 7 times each and do not follow the row ids.
    void _write_mixed(LogDb& db)
    {
        ASSERT_TRUE(db.begin());
        for (uint32_t i = 0; i < 10000u; ++i)
        {
            ASSERT_TRUE(db.log_message(_entry(static_cast<TIME64>(1000u + ((i * 7919u) % 1429u)), i % 5u, 1u + (i % 2u), _prios[i % 4u])));
        }

        const areg::DateTime now{ areg::DateTime::now() };
        for (uint32_t cookie = 1u; cookie <= 2u; ++cookie)
        {
            for (uint32_t scope = 1u; scope <= 4u; ++scope)
            {
                ASSERT_TRUE(db.log_scope_activate(areg::String("scope_") + areg::String::make_string(scope), scope, static_cast<uint32_t>(areg::LogPriority::PrioScopeLogs), cookie, now));
            }
        }

        ASSERT_TRUE(db.commit(true));
    }
}

//////////////////////////////////////////////////////////////////////////
// File mode, plain windows and statements: the API that existed before markers.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A reader that holds a stepped statement on the file must not make the writer's commits fail.
 **/
TEST(LogSqliteDatabaseTest, test_commitWhileReaderHoldsStatement)
{
    const std::string path{ _db_path("t1.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), false));

    sqlite3* reader{ nullptr };
    sqlite3_stmt* stmt{ nullptr };
    ASSERT_EQ(sqlite3_open_v2(path.c_str(), &reader, SQLITE_OPEN_READONLY, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(reader, "SELECT id FROM logs;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);

    constexpr uint32_t batches{ 10u };
    constexpr uint32_t rows{ 1000u };
    uint32_t failedBegin{ 0u };
    uint32_t failedCommit{ 0u };
    for (uint32_t b = 0; b < batches; ++b)
    {
        failedBegin += writer.begin() ? 0u : 1u;
        for (uint32_t i = 0; i < rows; ++i)
        {
            writer.log_message(_entry(static_cast<TIME64>(1000u + b * rows + i), 0u, 1u, areg::LogPriority::PrioInfo));
        }

        failedCommit += writer.commit(true) ? 0u : 1u;
    }

    EXPECT_EQ(failedBegin, 0u);
    EXPECT_EQ(failedCommit, 0u);

    sqlite3_finalize(stmt);
    stmt = nullptr;
    ASSERT_EQ(sqlite3_prepare_v2(reader, "SELECT COUNT(*) FROM logs;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);
    EXPECT_EQ(sqlite3_column_int64(stmt, 0), static_cast<sqlite3_int64>(1u + batches * rows));
    sqlite3_finalize(stmt);
    sqlite3_close(reader);

    writer.disconnect();
}

/**
 * \brief   A file open to write is in WAL mode; after a clean close it is one plain file that opens read only.
 **/
TEST(LogSqliteDatabaseTest, test_closedFileIsOnePlainFile)
{
    _capture_connections();
    const std::string path{ _db_path("t2.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), false));
    sqlite3* writerDb{ _lastDb };
    ASSERT_TRUE(_write_rows(writer, 100u, 1000));

    EXPECT_EQ(_query_text(path, "PRAGMA journal_mode;"), "wal");
    EXPECT_EQ(_query_text(writerDb, "PRAGMA synchronous;"), "1");

    writer.disconnect();
    EXPECT_FALSE(_exists(path + "-wal"));
    EXPECT_FALSE(_exists(path + "-shm"));
    EXPECT_EQ(_query_text(path, "PRAGMA journal_mode;"), "delete");

    const fs::path copyDir{ fs::path(path).parent_path() / "t2-readonly" };
    const fs::path copyFile{ copyDir / "t2.sqlog" };
    std::error_code err;
    fs::permissions(copyDir, fs::perms::owner_all, fs::perm_options::add, err);
    fs::remove_all(copyDir, err);
    ASSERT_TRUE(fs::create_directories(copyDir, err));
    ASSERT_TRUE(fs::copy_file(path, copyFile, err));
    fs::permissions(copyFile, fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read, fs::perm_options::replace, err);
    fs::permissions(copyDir, fs::perms::owner_read | fs::perms::owner_exec, fs::perm_options::replace, err);

    EXPECT_EQ(_query_text(copyFile.string(), "SELECT COUNT(*) FROM logs;"), "101");

    fs::permissions(copyDir, fs::perms::owner_all, fs::perm_options::add, err);
    fs::permissions(copyFile, fs::perms::owner_write, fs::perm_options::add, err);
    fs::remove_all(copyDir, err);
}

/**
 * \brief   A writer destroyed without `disconnect()` leaves one plain file, as a disconnected one does.
 **/
TEST(LogSqliteDatabaseTest, test_destroyedWriterLeavesOnePlainFile)
{
    const std::string path{ _db_path("t2destroyed.sqlog") };
    {
        LogDb writer;
        ASSERT_TRUE(writer.connect(areg::String(path.c_str()), false));
        ASSERT_TRUE(_write_rows(writer, 100u, 1000));
        ASSERT_TRUE(writer.begin());
        ASSERT_TRUE(writer.log_message(_entry(5000, 0u, 1u, areg::LogPriority::PrioInfo)));
    }

    EXPECT_FALSE(_exists(path + "-wal"));
    EXPECT_FALSE(_exists(path + "-shm"));
    EXPECT_EQ(_query_text(path, "PRAGMA journal_mode;"), "delete");
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs;"), "102");
}

/**
 * \brief   A writer that commits batch after batch without a pause keeps its write-ahead log short:
 *          the log starts over instead of growing with every batch.
 **/
TEST(LogSqliteDatabaseTest, test_continuousWriterKeepsTheLogShort)
{
    const std::string path{ _db_path("t2continuous.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), false));

    constexpr uint32_t batches{ 200u };
    constexpr uint32_t rows{ 1000u };
    std::uintmax_t largestLog{ 0u };
    for (uint32_t b = 0; b < batches; ++b)
    {
        ASSERT_TRUE(_write_rows(writer, rows, static_cast<TIME64>(1000u + b * rows)));
        std::error_code err;
        const std::uintmax_t size{ fs::file_size(path + "-wal", err) };
        largestLog = err ? largestLog : std::max(largestLog, size);
    }

    EXPECT_GT(largestLog, 0u);
    EXPECT_LT(largestLog, static_cast<std::uintmax_t>(24u * 1024u * 1024u));
    writer.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs;"), std::to_string(1u + batches * rows));
    EXPECT_FALSE(_exists(path + "-wal"));
}

/**
 * \brief   A few rows committed by a writer reach the database file within seconds, while the writer
 *          stays open and writes nothing more.
 **/
TEST(LogSqliteDatabaseTest, test_idleWriterRowsReachTheFile)
{
    const std::string path{ _db_path("t2idle.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), false));
    std::error_code err;
    const std::uintmax_t emptySize{ fs::file_size(path, err) };
    ASSERT_FALSE(err);

    ASSERT_TRUE(_write_rows(writer, 500u, 1000));
    std::uintmax_t size{ emptySize };
    for (int i = 0; (i < 50) && (size == emptySize); ++i)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        size = fs::file_size(path, err);
    }

    EXPECT_GT(size, emptySize);
    writer.disconnect();
}

/**
 * \brief   The windows that continue from a place seek it in the time index; the first and the last
 *          window read the index in order. None sorts the rows again.
 **/
TEST(LogSqliteDatabaseTest, test_windowPlansUseTheTimeIndex)
{
    _capture_connections();
    const std::string path{ _db_path("t4plan.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    sqlite3* conn{ _lastDb };
    _write_mixed(db);

    const LogDb::LogPosition middle{ 1700, 5000u };
    areg::ext::SqliteStatement stmt(db.database());

    ASSERT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{}, 100));
    std::string plan{ _newest_plan(conn) };
    EXPECT_TRUE(_uses_time_index(plan, false)) << plan;

    ASSERT_TRUE(db.setup_statement_read_window(stmt, middle, 100));
    plan = _newest_plan(conn);
    EXPECT_TRUE(_uses_time_index(plan, true)) << plan;

    ASSERT_TRUE(db.setup_statement_read_window_back(stmt, LogDb::LogPosition{}, 100));
    plan = _newest_plan(conn);
    EXPECT_TRUE(_uses_time_index(plan, false)) << plan;

    ASSERT_TRUE(db.setup_statement_read_window_back(stmt, middle, 100));
    plan = _newest_plan(conn);
    EXPECT_TRUE(_uses_time_index(plan, true)) << plan;

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   A window read that stops before the end of the query leaves no statement stepping, so a
 *          writer on another connection commits right after.
 **/
TEST(LogSqliteDatabaseTest, test_windowEndsWithStatementReset)
{
    _capture_connections();
    const std::string path{ _db_path("t5reset.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    sqlite3* conn{ _lastDb };
    ASSERT_TRUE(_write_rows(db, 1000u, 1000));

    areg::ext::SqliteStatement stmt(db.database());
    ASSERT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{}, 100));
    std::vector<areg::SharedBuffer> logs;
    LogDb::LogPosition last{};
    EXPECT_EQ(LogDb::fill_log_window(logs, stmt, 50, last), 50);
    EXPECT_EQ(_active_statements(conn), 0);

    sqlite3* other{ nullptr };
    ASSERT_EQ(sqlite3_open_v2(path.c_str(), &other, SQLITE_OPEN_READWRITE, nullptr), SQLITE_OK);
    EXPECT_EQ(sqlite3_exec(other, "BEGIN; UPDATE version SET describe = describe; COMMIT;", nullptr, nullptr, nullptr), SQLITE_OK);
    sqlite3_exec(other, "ROLLBACK;", nullptr, nullptr, nullptr);
    sqlite3_close(other);

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   Preparing a statement again frees the query it held, so a closed database leaves no
 *          SQLite memory behind.
 **/
TEST(LogSqliteDatabaseTest, test_preparedAgainFreesThePrevious)
{
    const std::string path{ _db_path("leak.sqlog") };
    {
        LogDb warmup;
        ASSERT_TRUE(warmup.connect(areg::String(path.c_str()), false));
        warmup.disconnect();
    }

    const sqlite3_int64 before{ sqlite3_memory_used() };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
        areg::ext::SqliteStatement stmt(db.database());
        for (int i = 0; i < 3; ++i)
        {
            EXPECT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{}, 10));
        }

        stmt.finalize();
        db.disconnect();
    }

    EXPECT_EQ(sqlite3_memory_used(), before);
}

/**
 * \brief   A database closed while a statement is still prepared closes its connection once the
 *          statement is finalized, and leaves no SQLite memory behind.
 **/
TEST(LogSqliteDatabaseTest, test_closedWithPreparedStatementFreesTheConnection)
{
    const std::string path{ _db_path("leak2.sqlog") };
    {
        LogDb warmup;
        ASSERT_TRUE(warmup.connect(areg::String(path.c_str()), false));
        warmup.disconnect();
    }

    const sqlite3_int64 before{ sqlite3_memory_used() };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), true));
        areg::ext::SqliteStatement stmt(db.database());
        EXPECT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{}, 10));
        db.disconnect();
        EXPECT_FALSE(db.is_operable());
        stmt.finalize();
    }

    EXPECT_EQ(sqlite3_memory_used(), before);
}

//////////////////////////////////////////////////////////////////////////
// Row ids, filtered windows and markers.
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A window read reports the database id of every row it read.
 **/
TEST(LogSqliteDatabaseTest, test_windowReportsRowIds)
{
    _capture_connections();
    const std::string path{ _db_path("t5ids.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    sqlite3* conn{ _lastDb };
    _write_mixed(db);
    const std::vector<uint32_t> expected{ _ids(_rows_in_time_order(path)) };

    areg::ext::SqliteStatement stmt(db.database());
    ASSERT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{}, 100));
    std::vector<areg::SharedBuffer> logs;
    std::vector<uint32_t> ids;
    LogDb::LogPosition last{};
    ASSERT_EQ(LogDb::fill_log_window(logs, stmt, 50, last, ids), 50);
    ASSERT_EQ(ids.size(), 50u);
    EXPECT_TRUE(std::equal(ids.begin(), ids.end(), expected.begin()));
    EXPECT_EQ(last.rowId, static_cast<uint64_t>(ids.back()));
    EXPECT_EQ(_active_statements(conn), 0);

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   Forward and backward windows of 100 rows cover every row exactly once, in time order,
 *          when many rows share a timestamp.
 **/
TEST(LogSqliteDatabaseTest, test_keysetWindowsCoverEveryRowOnce)
{
    const std::string path{ _db_path("t4.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    _write_mixed(db);
    const std::vector<uint32_t> expected{ _ids(_rows_in_time_order(path)) };
    ASSERT_EQ(expected.size(), 10001u);

    areg::ext::SqliteStatement stmt(db.database());
    std::vector<uint32_t> forward;
    LogDb::LogPosition pos{};
    for (int32_t read = 1, rounds = 0; read > 0; ++rounds)
    {
        ASSERT_LT(rounds, 1000);
        ASSERT_TRUE(db.setup_statement_read_window(stmt, pos, 100));
        std::vector<areg::SharedBuffer> logs;
        read = LogDb::fill_log_window(logs, stmt, 100, pos, forward);
        ASSERT_EQ(logs.size(), static_cast<size_t>(read));
    }

    EXPECT_EQ(forward, expected);

    std::vector<uint32_t> backward;
    pos = LogDb::LogPosition{};
    for (int32_t read = 1, rounds = 0; read > 0; ++rounds)
    {
        ASSERT_LT(rounds, 1000);
        ASSERT_TRUE(db.setup_statement_read_window_back(stmt, pos, 100));
        std::vector<areg::SharedBuffer> logs;
        read = LogDb::fill_log_window(logs, stmt, 100, pos, backward);
    }

    std::reverse(backward.begin(), backward.end());
    EXPECT_EQ(backward, expected);

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   Filtered windows, forward and backward, of all instances and of one, equal the filtered
 *          rows in time order; each seeks the time index.
 **/
TEST(LogSqliteDatabaseTest, test_filteredWindowsEqualTheFilteredSet)
{
    _capture_connections();
    const std::string path{ _db_path("t4filter.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    sqlite3* conn{ _lastDb };
    _write_mixed(db);
    const std::vector<Row> all{ _rows_in_time_order(path) };

    constexpr uint32_t maskScope1{ static_cast<uint32_t>(areg::LogPriority::PrioError) | static_cast<uint32_t>(areg::LogPriority::PrioWarning) };
    constexpr uint32_t maskScope2{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    areg::ArrayList<LogDb::ScopeFilter> filter;
    filter.add(LogDb::ScopeFilter{ 1u, maskScope1 });
    filter.add(LogDb::ScopeFilter{ 2u, maskScope2 });
    filter.add(LogDb::ScopeFilter{ 3u, 0u });
    ASSERT_GT(db.setup_filter_logs(areg::TARGET_ALL, filter), 0u);

    const auto passes = [&](const Row& row, ITEM_ID instId) -> bool
    {
        if ((instId != areg::TARGET_ALL) && (row.cookie != static_cast<uint32_t>(instId)))
            return false;

        switch (row.scope)
        {
        case 0u: return true;
        case 1u: return (row.prio & maskScope1) != 0u;
        case 2u: return (row.prio & maskScope2) != 0u;
        case 3u: return false;
        case 4u: return (row.prio & static_cast<uint32_t>(areg::LogPriority::PrioScopeLogs)) != 0u;
        default: return false;
        }
    };

    for (const ITEM_ID instId : { static_cast<ITEM_ID>(areg::TARGET_ALL), static_cast<ITEM_ID>(1u) })
    {
        std::vector<uint32_t> expected;
        for (const Row& row : all)
        {
            if (passes(row, instId))
                expected.push_back(row.id);
        }

        ASSERT_GT(expected.size(), 1000u);
        ASSERT_LT(expected.size(), all.size());

        areg::ext::SqliteStatement stmt(db.database());
        std::vector<uint32_t> forward;
        LogDb::LogPosition pos{};
        for (int32_t read = 1, rounds = 0; read > 0; ++rounds)
        {
            ASSERT_LT(rounds, 1000);
            ASSERT_TRUE(db.setup_statement_read_filter_window(stmt, instId, pos, 100));
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), pos.timeCreated != 0)) << _newest_plan(conn);
            std::vector<areg::SharedBuffer> logs;
            read = LogDb::fill_log_window(logs, stmt, 100, pos, forward);
        }

        EXPECT_EQ(forward, expected) << "instance " << instId;

        std::vector<uint32_t> backward;
        pos = LogDb::LogPosition{};
        for (int32_t read = 1, rounds = 0; read > 0; ++rounds)
        {
            ASSERT_LT(rounds, 1000);
            ASSERT_TRUE(db.setup_statement_read_filter_window_back(stmt, instId, pos, 100));
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), pos.timeCreated != 0)) << _newest_plan(conn);
            std::vector<areg::SharedBuffer> logs;
            read = LogDb::fill_log_window(logs, stmt, 100, pos, backward);
        }

        std::reverse(backward.begin(), backward.end());
        EXPECT_EQ(backward, expected) << "instance " << instId;
        stmt.finalize();
    }

    db.disconnect();
}

/**
 * \brief   A window read at a row number holds the rows the keyset windows read at that number,
 *          plain and filtered, and reads the time index in order.
 **/
TEST(LogSqliteDatabaseTest, test_windowAtRowNumberEqualsTheKeysetSlice)
{
    _capture_connections();
    const std::string path{ _db_path("t4at.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    sqlite3* conn{ _lastDb };
    _write_mixed(db);

    const auto sweep = [&](ITEM_ID instId, bool filtered) -> std::vector<uint32_t>
    {
        areg::ext::SqliteStatement stmt(db.database());
        std::vector<uint32_t> ids;
        LogDb::LogPosition pos{};
        for (int32_t read = 1, rounds = 0; (read > 0) && (rounds < 1000); ++rounds)
        {
            const bool ready{ filtered ? db.setup_statement_read_filter_window(stmt, instId, pos, 100) : db.setup_statement_read_window(stmt, pos, 100) };
            EXPECT_TRUE(ready);
            std::vector<areg::SharedBuffer> logs;
            read = LogDb::fill_log_window(logs, stmt, 100, pos, ids);
        }

        return ids;
    };

    const auto check = [&](ITEM_ID instId, bool filtered)
    {
        const std::vector<uint32_t> all{ sweep(instId, filtered) };
        ASSERT_GT(all.size(), 1000u);
        areg::ext::SqliteStatement stmt(db.database());
        for (const uint32_t offset : { 0u, 1u, 99u, 4321u, static_cast<uint32_t>(all.size()) - 50u, static_cast<uint32_t>(all.size()) + 5u })
        {
            const bool ready{ filtered ? db.setup_statement_read_filter_window_at(stmt, instId, offset, 100) : db.setup_statement_read_window_at(stmt, offset, 100) };
            ASSERT_TRUE(ready);
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), false)) << _newest_plan(conn);

            std::vector<areg::SharedBuffer> logs;
            std::vector<uint32_t> ids;
            LogDb::LogPosition last{};
            const int32_t read{ LogDb::fill_log_window(logs, stmt, 100, last, ids) };
            const size_t from{ std::min<size_t>(offset, all.size()) };
            const size_t to{ std::min<size_t>(from + 100u, all.size()) };
            EXPECT_EQ(static_cast<size_t>(read), to - from) << "offset " << offset;
            EXPECT_TRUE(std::equal(ids.begin(), ids.end(), all.begin() + static_cast<std::ptrdiff_t>(from), all.begin() + static_cast<std::ptrdiff_t>(to))) << "offset " << offset;
            EXPECT_EQ(_active_statements(conn), 0);
        }

        stmt.finalize();
    };

    check(areg::TARGET_ALL, false);

    areg::ArrayList<LogDb::RefusedSpan> spans;
    spans.add(LogDb::RefusedSpan{ 2u, static_cast<ITEM_ID>(1u), 0, 0 });
    spans.add(LogDb::RefusedSpan{ 4u, static_cast<ITEM_ID>(2u), 1200, 1300 });
    ASSERT_TRUE(db.set_refused_spans(spans));
    check(areg::TARGET_ALL, false);
    ASSERT_TRUE(db.set_refused_spans(areg::ArrayList<LogDb::RefusedSpan>{ }));

    areg::ArrayList<LogDb::ScopeFilter> filter;
    filter.add(LogDb::ScopeFilter{ 1u, static_cast<uint32_t>(areg::LogPriority::PrioError) });
    filter.add(LogDb::ScopeFilter{ 3u, 0u });
    ASSERT_GT(db.setup_filter_logs(areg::TARGET_ALL, filter), 0u);
    check(areg::TARGET_ALL, true);
    check(static_cast<ITEM_ID>(2u), true);

    db.disconnect();
}

/**
 * \brief   Markers are set on an existing master row with a marker level, read back, removed one by
 *          one, and removed with their master row.
 **/
TEST(LogSqliteDatabaseTest, test_markersAttachedToTheirMaster)
{
    const std::string path{ _db_path("t3.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    ASSERT_TRUE(_write_rows(db, 5u, 1000));

    std::vector<LogDb::LogMarker> markers;
    EXPECT_EQ(db.read_markers(markers), 0u);

    const std::string longAuthor(100, 'a');
    const std::string longNote(2000, 'n');
    const uint32_t m1{ db.add_marker(3u, areg::LogPriority::PrioMarkerError, "owner", "look here") };
    const uint32_t m2{ db.add_marker(3u, areg::LogPriority::PrioMarkerInfo, longAuthor.c_str(), longNote.c_str()) };
    const uint32_t m3{ db.add_marker(5u, areg::LogPriority::PrioMarker, "", "") };
    EXPECT_NE(m1, 0u);
    EXPECT_NE(m2, 0u);
    EXPECT_NE(m3, 0u);
    EXPECT_EQ(db.add_marker(4u, areg::LogPriority::PrioError, "owner", "not a marker level"), 0u);
    EXPECT_EQ(db.add_marker(999u, areg::LogPriority::PrioMarker, "owner", "no such row"), 0u);
    EXPECT_EQ(db.add_marker(0u, areg::LogPriority::PrioMarker, "owner", "no row id"), 0u);

    ASSERT_EQ(db.read_markers(markers), 3u);
    ASSERT_EQ(markers.size(), 3u);
    EXPECT_EQ(markers[0].markerId, m1);
    EXPECT_EQ(markers[0].masterId, 3u);
    EXPECT_EQ(markers[0].level, areg::LogPriority::PrioMarkerError);
    EXPECT_EQ(markers[0].author, "owner");
    EXPECT_EQ(markers[0].note, "look here");
    EXPECT_NE(markers[0].timeSet, 0);
    EXPECT_EQ(markers[1].author.length(), areg::LOG_NAME_SIZE - 1u);
    EXPECT_EQ(markers[1].note.length(), 1023u);
    EXPECT_EQ(markers[2].masterId, 5u);
    EXPECT_EQ(markers[2].level, areg::LogPriority::PrioMarker);

    areg::ext::SqliteStatement stmt(db.database());
    std::vector<areg::SharedBuffer> logs;
    std::vector<uint32_t> ids;
    LogDb::LogPosition last{};
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, static_cast<uint16_t>(areg::LogPriority::PrioMarker)), 2u);
    EXPECT_EQ(LogDb::fill_log_window(logs, stmt, -1, last, ids), 2);
    EXPECT_EQ(ids, (std::vector<uint32_t>{ 3u, 5u }));

    ids.clear();
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, static_cast<uint16_t>(areg::LogPriority::PrioError)), 1u);
    EXPECT_EQ(LogDb::fill_log_window(logs, stmt, -1, last, ids), 1);
    EXPECT_EQ(ids, (std::vector<uint32_t>{ 3u }));

    EXPECT_TRUE(db.remove_marker(m2));
    ASSERT_EQ(db.read_markers(markers), 2u);
    EXPECT_EQ(markers[0].markerId, m1);
    EXPECT_EQ(markers[1].markerId, m3);

    EXPECT_TRUE(db.begin());
    EXPECT_TRUE(db.remove_log(3u));
    EXPECT_TRUE(db.commit(true));
    ASSERT_EQ(db.read_markers(markers), 1u);
    EXPECT_EQ(markers[0].markerId, m3);

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   A file made without the markers table has no markers; the first marker creates the table.
 **/
TEST(LogSqliteDatabaseTest, test_fileWithoutMarkersTable)
{
    const std::string path{ _db_path("t3old.sqlog") };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
        ASSERT_TRUE(_write_rows(db, 3u, 1000));
        ASSERT_TRUE(db.execute("DROP TABLE IF EXISTS markers;"));
        db.disconnect();
    }

    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), false));
    EXPECT_FALSE(db.table_exists("markers"));
    std::vector<LogDb::LogMarker> markers;
    EXPECT_EQ(db.read_markers(markers), 0u);

    areg::ext::SqliteStatement stmt(db.database());
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, 0xFFFFu), 0u);

    EXPECT_TRUE(db.remove_log(2u));
    EXPECT_NE(db.add_marker(3u, areg::LogPriority::PrioMarkerWarning, "owner", "first"), 0u);
    EXPECT_TRUE(db.table_exists("markers"));
    EXPECT_EQ(db.read_markers(markers), 1u);

    stmt.finalize();
    db.disconnect();
}
