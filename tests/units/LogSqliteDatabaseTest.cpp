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
#include "aregextend/db/LogDbWriter.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/base/File.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
    #include <share.h>
#endif  // defined(_WIN32)

// Use these options if compile for Windows with MSVC
#ifdef _MSC_VER
#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "sqlite3")
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "aregsqlite3")
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
#endif  // _MSC_VER

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
        sqlite3_auto_extension(reinterpret_cast<void (*)(void)>(reinterpret_cast<void *>(&_capture_db)));
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

    //! Returns the checkpoint sequence in the header of an open write-ahead log, which grows each time
    //! the log starts over from its beginning. Returns 0 if the header cannot be read.
    //! Opens a file for binary reading, shared with writers.
    std::FILE* _open_shared(const std::string& path)
    {
#if defined(_WIN32)
        return ::_fsopen(path.c_str(), "rb", _SH_DENYNO);
#else   // defined(_WIN32)
        return std::fopen(path.c_str(), "rb");
#endif  // defined(_WIN32)
    }

    uint32_t _log_sequence(std::FILE* wal)
    {
        unsigned char header[16]{ };
        if ((wal == nullptr) || (std::fseek(wal, 0, SEEK_SET) != 0) || (std::fread(header, 1u, sizeof(header), wal) != sizeof(header)))
            return 0u;

        return (static_cast<uint32_t>(header[12]) << 24) | (static_cast<uint32_t>(header[13]) << 16) | (static_cast<uint32_t>(header[14]) << 8) | static_cast<uint32_t>(header[15]);
    }

    areg::LogEntry _entry(TIME64 timeCreated, uint32_t scopeId, ITEM_ID cookie, areg::LogPriority prio)
    {
        areg::LogEntry entry{ areg::LogMessageType::MessageText };
        entry.logMessagePrio = prio;
        entry.logCookie      = cookie;
        entry.logScopeId     = scopeId;
        entry.logTimestamp   = timeCreated;
        entry.logReceived    = timeCreated;
        entry.logMessageLen  = static_cast<uint32_t>(areg::copy_string(entry.logMessage, areg::LOG_MSG_SIZE, "log database test"));
        entry.logThreadLen   = static_cast<uint32_t>(areg::copy_string(entry.logThread, areg::LOG_NAME_SIZE, "TestThread"));
        entry.logModuleLen   = static_cast<uint32_t>(areg::copy_string(entry.logModule, areg::LOG_NAME_SIZE, "areg-unit-tests"));
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

    //! Returns true if the plan reads the logs through the given time ordered index without sorting them again.
    bool _uses_time_index(const std::string& plan, bool seek, const std::string& index = "idx_logs_time")
    {
        const bool viaIndex{ plan.find("USING INDEX " + index) != std::string::npos ||
                             plan.find("USING COVERING INDEX " + index) != std::string::npos };
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
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

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
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
        ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
 *          the log starts over again and again instead of growing with every batch. When the log
 *          has not started over for longer than usual, the writer pauses after each batch.
 **/
TEST(LogSqliteDatabaseTest, test_continuousWriterKeepsTheLogShort)
{
    const std::string path{ _db_path("t2continuous.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    constexpr uint32_t restarts{ 3u };
    constexpr uint32_t rows{ 1000u };
    constexpr uint32_t longCycle{ 150u };
    constexpr std::chrono::milliseconds pause{ 50 };
    const auto until{ std::chrono::steady_clock::now() + std::chrono::seconds(120) };
    uint32_t batches{ 0u };
    uint32_t seen{ 0u };
    uint32_t cycle{ 0u };
    std::FILE* wal{ _open_shared(path + "-wal") };
    ASSERT_NE(wal, nullptr);
    std::setvbuf(wal, nullptr, _IONBF, 0u);
    uint32_t lastSeq{ _log_sequence(wal) };
    for (; (seen < restarts) && (std::chrono::steady_clock::now() < until); ++batches)
    {
        ASSERT_TRUE(_write_rows(writer, rows, static_cast<TIME64>(1000u + batches * rows)));
        const uint32_t seq{ _log_sequence(wal) };
        cycle = (seq != lastSeq) ? 0u : cycle + 1u;
        seen += (seq != lastSeq) ? 1u : 0u;
        lastSeq = seq;
        if (cycle > longCycle)
        {
            std::this_thread::sleep_for(pause);
        }
    }

    // Closes the log before the writer deletes it.
    std::fclose(wal);
    EXPECT_EQ(seen, restarts) << "batches written: " << batches;
    writer.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs;"), std::to_string(1u + batches * rows));
    EXPECT_FALSE(_exists(path + "-wal"));
}

/**
 * \brief   The rows of a writer that goes idle reach the database file within seconds, also after the
 *          write-ahead log passed the size at which the writer copies the rest itself. A reader holds
 *          the log while the writer passes that size.
 **/
TEST(LogSqliteDatabaseTest, test_idleWriterAfterLongLogReachesTheFile)
{
    const std::string path{ _db_path("t2idlelong.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    sqlite3* reader{ nullptr };
    sqlite3_stmt* stmt{ nullptr };
    ASSERT_EQ(sqlite3_open_v2(path.c_str(), &reader, SQLITE_OPEN_READONLY, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_prepare_v2(reader, "SELECT id FROM logs;", -1, &stmt, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_step(stmt), SQLITE_ROW);

    // 4200 frames of 4 KB pages: past the size at which the writer copies the rest itself.
    constexpr std::uintmax_t longLog{ 4200u * (4096u + 24u) };
    std::error_code err;
    for (uint32_t b = 0; (b < 400u) && (fs::file_size(path + "-wal", err) < longLog); ++b)
    {
        ASSERT_TRUE(_write_rows(writer, 1000u, static_cast<TIME64>(1000u + b * 1000u)));
    }

    ASSERT_GE(fs::file_size(path + "-wal", err), longLog);
    sqlite3_finalize(stmt);
    sqlite3_close(reader);

    const std::uintmax_t pageSize{ std::stoull(_query_text(path, "PRAGMA page_size;")) };
    const std::uintmax_t expected{ pageSize * std::stoull(_query_text(path, "PRAGMA page_count;")) };
    std::uintmax_t size{ fs::file_size(path, err) };
    const auto until{ std::chrono::steady_clock::now() + std::chrono::seconds(60) };
    while ((size != expected) && (std::chrono::steady_clock::now() < until))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        size = fs::file_size(path, err);
    }

    EXPECT_EQ(size, expected);
    writer.disconnect();
}

/**
 * \brief   A few rows committed by a writer reach the database file within seconds, while the writer
 *          stays open and writes nothing more.
 **/
TEST(LogSqliteDatabaseTest, test_idleWriterRowsReachTheFile)
{
    const std::string path{ _db_path("t2idle.sqlog") };
    LogDb writer;
    ASSERT_TRUE(writer.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    std::error_code err;
    const std::uintmax_t emptySize{ fs::file_size(path, err) };
    ASSERT_FALSE(err);

    ASSERT_TRUE(_write_rows(writer, 500u, 1000));
    std::uintmax_t size{ emptySize };
    const auto until{ std::chrono::steady_clock::now() + std::chrono::seconds(60) };
    while ((size == emptySize) && (std::chrono::steady_clock::now() < until))
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
        ASSERT_TRUE(warmup.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        warmup.disconnect();
    }

    const sqlite3_int64 before{ sqlite3_memory_used() };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
        ASSERT_TRUE(warmup.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        warmup.disconnect();
    }

    const sqlite3_int64 before{ sqlite3_memory_used() };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
        const std::string index{ instId == areg::TARGET_ALL ? "idx_logs_time" : "idx_logs_cookie" };

        areg::ext::SqliteStatement stmt(db.database());
        std::vector<uint32_t> forward;
        LogDb::LogPosition pos{};
        for (int32_t read = 1, rounds = 0; read > 0; ++rounds)
        {
            ASSERT_LT(rounds, 1000);
            ASSERT_TRUE(db.setup_statement_read_filter_window(stmt, instId, pos, 100));
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), pos.timeCreated != 0, index)) << _newest_plan(conn);
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
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), pos.timeCreated != 0, index)) << _newest_plan(conn);
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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
            EXPECT_TRUE(_uses_time_index(_newest_plan(conn), false, filtered && (instId != areg::TARGET_ALL) ? "idx_logs_cookie" : "idx_logs_time")) << _newest_plan(conn);

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
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        ASSERT_TRUE(_write_rows(db, 3u, 1000));
        ASSERT_TRUE(db.execute("DROP TABLE IF EXISTS markers;"));
        db.disconnect();
    }

    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
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

//////////////////////////////////////////////////////////////////////////
// Connection, schema, instances, scopes, readers, counters, filters and the API on a closed log.
//////////////////////////////////////////////////////////////////////////

namespace
{
    using Stmt  = areg::ext::SqliteStatement;
    using Res   = areg::ext::SqliteStatement::QueryResult;

    constexpr uint32_t _prioAll{ static_cast<uint32_t>(areg::LogPriority::PrioScopeLogs) };

    //! Creates the file with the text in it, an empty text gives an empty file.
    void _write_file(const std::string& path, const char* text)
    {
        constexpr uint32_t MODE{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Binary) | static_cast<uint32_t>(areg::File::OpenMode::Create) };
        areg::File file;
        ASSERT_TRUE(file.open(path.c_str(), MODE));
        const uint32_t size{ static_cast<uint32_t>(areg::string_length<char>(text)) };
        if (size != 0u)
        {
            ASSERT_EQ(file.write(reinterpret_cast<const uint8_t*>(text), size), size);
        }

        file.close();
    }

    //! Returns the size of the file in bytes.
    uint32_t _file_size(const std::string& path)
    {
        constexpr uint32_t MODE{ static_cast<uint32_t>(areg::File::OpenMode::Read) | static_cast<uint32_t>(areg::File::OpenMode::Binary) | static_cast<uint32_t>(areg::File::OpenMode::Exist) };
        areg::File file;
        return file.open(path.c_str(), MODE) ? static_cast<uint32_t>(file.length()) : 0u;
    }

    std::string _txt(const areg::String& value)
    {
        return std::string(value.as_string());
    }

    areg::ConnectedInstance _instance(ITEM_ID cookie, const char* name, const char* location, TIME64 connected)
    {
        areg::ConnectedInstance inst;
        inst.ciSource    = areg::MessageSource::SourceClient;
        inst.ciBitness   = areg::InstanceBitness::Bitness64;
        inst.ciCookie    = cookie;
        inst.ciTimestamp = connected;
        inst.ciInstance  = name;
        inst.ciLocation  = location;
        return inst;
    }

    areg::DateTime _at(TIME64 moment)
    {
        return areg::DateTime(moment);
    }

    const areg::LogEntry* _log_of(const areg::SharedBuffer& buffer)
    {
        return reinterpret_cast<const areg::LogEntry*>(buffer.buffer());
    }

    //! Writes 12 rows i = 0..11 created at 100 + 10 * i, of cookie 31 (even i) or 32 (odd i) and scope i % 3.
    void _write_set(LogDb& db)
    {
        ASSERT_TRUE(db.begin());
        for (uint32_t i = 0; i < 12u; ++i)
        {
            ASSERT_TRUE(db.log_message(_entry(static_cast<TIME64>(100u + (10u * i)), i % 3u, 31u + (i % 2u), areg::LogPriority::PrioInfo)));
        }

        ASSERT_TRUE(db.commit(true));
    }

    //! Registers the scopes 1 and 2 of the cookies 31 and 32.
    void _write_set_scopes(LogDb& db)
    {
        for (const ITEM_ID cookie : { static_cast<ITEM_ID>(31u), static_cast<ITEM_ID>(32u) })
        {
            ASSERT_TRUE(db.log_scope_activate(areg::String("one"), 1u, _prioAll, cookie, _at(10)));
            ASSERT_TRUE(db.log_scope_activate(areg::String("two"), 2u, _prioAll, cookie, _at(10)));
        }
    }

    std::vector<ITEM_ID> _sorted(std::vector<ITEM_ID> ids)
    {
        std::sort(ids.begin(), ids.end());
        return ids;
    }
}

/**
 * \brief   The text of the three static queries prepares on a log file and takes the parameters it names.
 **/
TEST(LogSqliteDatabaseTest, test_staticQueriesPrepareOnLogFile)
{
    const std::string path{ _db_path("s1queries.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    Stmt scopes(db.database(), LogDb::read_scopes_query());
    ASSERT_TRUE(scopes.is_valid());
    EXPECT_TRUE(scopes.bind_uint32(0, 1u));
    EXPECT_FALSE(scopes.bind_uint32(1, 1u));

    Stmt instances(db.database(), LogDb::read_instances_query());
    ASSERT_TRUE(instances.is_valid());
    EXPECT_TRUE(instances.bind_uint32(0, 1u));
    EXPECT_TRUE(instances.bind_uint32(1, 1u));
    EXPECT_FALSE(instances.bind_uint32(2, 1u));

    Stmt all(db.database(), LogDb::read_all_log_messages_query());
    ASSERT_TRUE(all.is_valid());
    EXPECT_TRUE(all.bind_int32(0, -1));
    EXPECT_TRUE(all.bind_uint32(1, 0u));
    EXPECT_FALSE(all.bind_uint32(2, 0u));
    EXPECT_EQ(all.next(), Res::HasMore);

    all.finalize();
    db.disconnect();
}

/**
 * \brief   With database logging switched off the log refuses to connect; switched on it connects, and the
 *          paths and the objects of the log are reachable also through a constant reference.
 **/
TEST(LogSqliteDatabaseTest, test_loggingSwitchPathsAndAccessors)
{
    const std::string path{ _db_path("s2switch.sqlog") };
    LogDb db;
    EXPECT_TRUE(db.is_database_logging_enabled());
    db.set_database_logging_enabled(false);
    EXPECT_FALSE(db.is_database_logging_enabled());
    EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.tables_initialized());
    EXPECT_FALSE(_exists(path));

    db.set_database_logging_enabled(true);
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_TRUE(db.is_operable());
    EXPECT_TRUE(db.tables_initialized());
    EXPECT_TRUE(_exists(path));
    EXPECT_EQ(_txt(db.initial_database_path()), path);
    EXPECT_EQ(_txt(db.database_path()), _txt(db.database().path()));
    EXPECT_FALSE(db.database_path().is_empty());

    const LogDb& view{ db };
    EXPECT_EQ(&view.database(), &db.database());
    EXPECT_EQ(&view.statement(), &db.statement());
    EXPECT_TRUE(view.statement().is_valid());
    EXPECT_EQ(_txt(view.database_path()), _txt(db.database_path()));

    db.disconnect();
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.tables_initialized());
}

/**
 * \brief   A new file gets the schema and the header row; connecting an existing file keeps its rows; a viewer has
 *          no insert statement and cannot add rows.
 **/
TEST(LogSqliteDatabaseTest, test_connectCreatesSchemaAndViewerKeepsRows)
{
    const std::string path{ _db_path("s3schema.sqlog") };
    {
        LogDb db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        for (const char* table : { "version", "instances", "scopes", "logs", "markers" })
        {
            EXPECT_TRUE(db.table_exists(table)) << table;
        }

        EXPECT_FALSE(db.table_exists("absent"));
        EXPECT_FALSE(db.table_exists(""));
        EXPECT_FALSE(db.table_exists(nullptr));
        EXPECT_FALSE(db.table_exists("filter_rules", "sqlite_temp_master"));
        EXPECT_TRUE(db.table_exists("refused_spans", "sqlite_temp_master"));

        EXPECT_EQ(db.count_log_entries(), 1u);
        EXPECT_EQ(db.max_log_id(), 1u);
        EXPECT_EQ(db.max_marker_id(), 0u);
        EXPECT_TRUE(db.log_message(_entry(100, 0u, 31u, areg::LogPriority::PrioInfo)));
        EXPECT_EQ(db.last_log_id(), 2u);
        EXPECT_EQ(db.max_log_id(), 2u);

        // a second connect on a connected log changes nothing
        EXPECT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        EXPECT_EQ(db.count_log_entries(), 2u);
        db.disconnect();
    }

    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM version;"), "1");
    EXPECT_EQ(_query_text(path, "SELECT msg_log FROM logs WHERE id = 1;"), "Starting database logging...");

    LogDb viewer;
    ASSERT_TRUE(viewer.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
    EXPECT_TRUE(viewer.tables_initialized());
    EXPECT_FALSE(viewer.statement().is_valid());
    EXPECT_EQ(viewer.count_log_entries(), 2u);
    EXPECT_FALSE(viewer.log_message(_entry(200, 0u, 31u, areg::LogPriority::PrioInfo)));
    EXPECT_EQ(viewer.count_log_entries(), 2u);
    viewer.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM version;"), "1");
}

/**
 * \brief   Reproduces the defect: connecting without a path tested the empty argument, so the existing file was treated as a new one.
 **/
TEST(LogSqliteDatabaseTest, test_reconnectWithoutPathKeepsExistingFile)
{
    const std::string path{ _db_path("s3reconnect.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    db.disconnect();

    ASSERT_TRUE(db.connect(areg::String(), areg::DatabaseRole::Recorder));
    EXPECT_EQ(db.count_log_entries(), 1u);
    db.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM version;"), "1");
}

/**
 * \brief   Reproduces the defect: a path with a mask was tested as written, so the existing file it names was treated as a new one.
 **/
TEST(LogSqliteDatabaseTest, test_reconnectWithMaskedPathKeepsExistingFile)
{
    const std::string masked{ _db_path("s3mask_%appname%.sqlog") };
    const std::string path{ areg::File::normalize_path(masked.c_str()).as_string() };
    ASSERT_NE(path, masked);
    for (const char* suffix : { "", "-journal", "-wal", "-shm" })
    {
        static_cast<void>(areg::File::delete_file((path + suffix).c_str()));
    }

    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(masked.c_str()), areg::DatabaseRole::Recorder));
    db.disconnect();

    ASSERT_TRUE(db.connect(areg::String(masked.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_EQ(db.count_log_entries(), 1u);
    EXPECT_EQ(_txt(db.initial_database_path()), masked);
    db.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM version;"), "1");
}

/**
 * \brief   Reproduces the defect: an empty file counted as an existing database, so connect succeeded and every write failed.
 **/
TEST(LogSqliteDatabaseTest, test_recorderInitializesAnEmptyFile)
{
    const std::string path{ _db_path("s3zero.sqlog") };
    _write_file(path, "");

    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_TRUE(db.log_message(_entry(1000, 0u, 41u, areg::LogPriority::PrioInfo)));
    db.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM version;"), "1");
}

/**
 * \brief   Reproduces the defect: a database left with no tables by a crash stayed unusable, connect succeeded and every write failed.
 **/
TEST(LogSqliteDatabaseTest, test_recorderInitializesADatabaseWithoutTables)
{
    const std::string path{ _db_path("s3notables.sqlog") };
    sqlite3* raw{ nullptr };
    ASSERT_EQ(sqlite3_open(path.c_str(), &raw), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(raw, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_close(raw), SQLITE_OK);

    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_TRUE(db.log_message(_entry(1000, 0u, 41u, areg::LogPriority::PrioInfo)));
    db.disconnect();
}

/**
 * \brief   Reproduces the defect: a recorder connected to a database of another application, changed its journal mode and could write nothing.
 **/
TEST(LogSqliteDatabaseTest, test_recorderRefusesAForeignDatabase)
{
    const std::string path{ _db_path("s3foreign.sqlog") };
    sqlite3* raw{ nullptr };
    ASSERT_EQ(sqlite3_open(path.c_str(), &raw), SQLITE_OK);
    ASSERT_EQ(sqlite3_exec(raw, "CREATE TABLE mine (a INTEGER);", nullptr, nullptr, nullptr), SQLITE_OK);
    ASSERT_EQ(sqlite3_close(raw), SQLITE_OK);

    LogDb db;
    EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_EQ(_query_text(path, "PRAGMA journal_mode;"), "delete");
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM sqlite_master;"), "1");
}

/**
 * \brief   Reproduces the defect: a file that is no database connected and could write nothing.
 **/
TEST(LogSqliteDatabaseTest, test_connectRefusesAFileThatIsNoDatabase)
{
    const std::string path{ _db_path("s3garbage.sqlog") };
    constexpr char TEXT[]{ "this file holds plain text and is no database of any kind" };
    _write_file(path, TEXT);

    LogDb db;
    EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_EQ(_file_size(path), static_cast<uint32_t>(sizeof(TEXT) - 1u));
}

/**
 * \brief   Reproduces the defect: a viewer created a missing file and wrote the tables into it.
 **/
TEST(LogSqliteDatabaseTest, test_viewerWritesNoFile)
{
    const std::string path{ _db_path("s3viewer.sqlog") };
    {
        LogDb db;
        EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
    }
    EXPECT_FALSE(areg::File::has_file(path.c_str()));

    _write_file(path, "");
    {
        LogDb db;
        EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
    }
    EXPECT_EQ(_file_size(path), 0u);
}

/**
 * \brief   Dropping a table quotes its name, so a name with quotes or SQL in it drops nothing else.
 **/
TEST(LogSqliteDatabaseTest, test_dropTableQuotesTheName)
{
    const std::string path{ _db_path("s4drop.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    EXPECT_FALSE(db.drop_table(nullptr));
    EXPECT_FALSE(db.drop_table(""));
    EXPECT_TRUE(db.drop_table("absent"));

    ASSERT_TRUE(db.execute(areg::String("CREATE TABLE \"odd\"\"name\" (a INTEGER);")));
    EXPECT_TRUE(db.table_exists("odd\"name"));
    EXPECT_TRUE(db.drop_table("odd\"name"));
    EXPECT_FALSE(db.table_exists("odd\"name"));

    EXPECT_TRUE(db.drop_table("logs\"; DROP TABLE instances; --"));
    EXPECT_TRUE(db.table_exists("instances"));
    EXPECT_TRUE(db.table_exists("logs"));

    EXPECT_TRUE(db.drop_table("version"));
    EXPECT_FALSE(db.table_exists("version"));
    db.disconnect();
}

/**
 * \brief   Connected instances are saved with every field and read back as ids, names and infos; a disconnect sets the time once.
 **/
TEST(LogSqliteDatabaseTest, test_instancesRoundTrip)
{
    const std::string path{ _db_path("s5inst.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_EQ(db.count_log_instances(), 0u);
    EXPECT_TRUE(db.log_instances().empty());
    EXPECT_TRUE(db.log_instance_names().empty());
    EXPECT_TRUE(db.log_instance_infos().empty());

    ASSERT_TRUE(db.log_instance_connected(_instance(11u, "alpha", "/opt/alpha", 5000), _at(7000)));
    ASSERT_TRUE(db.log_instance_connected(_instance(12u, "beta", "", 6000), _at(7001)));
    EXPECT_EQ(db.count_log_instances(), 2u);

    EXPECT_EQ(_sorted(db.log_instances()), (std::vector<ITEM_ID>{ 11u, 12u }));
    std::vector<ITEM_ID> ids{ 99u };
    db.log_instances(ids);
    EXPECT_EQ(_sorted(ids), (std::vector<ITEM_ID>{ 11u, 12u }));

    std::vector<std::string> names;
    for (const areg::String& name : db.log_instance_names())
    {
        names.push_back(_txt(name));
    }

    std::sort(names.begin(), names.end());
    EXPECT_EQ(names, (std::vector<std::string>{ "alpha", "beta" }));
    std::vector<areg::String> junkNames{ areg::String("junk") };
    db.log_instance_names(junkNames);
    EXPECT_EQ(junkNames.size(), 2u);

    const std::vector<areg::ConnectedInstance> infos{ db.log_instance_infos() };
    ASSERT_EQ(infos.size(), 2u);
    EXPECT_EQ(infos[0].ciCookie, 11u);
    EXPECT_EQ(infos[0].ciSource, areg::MessageSource::SourceClient);
    EXPECT_EQ(infos[0].ciBitness, areg::InstanceBitness::Bitness64);
    EXPECT_EQ(infos[0].ciTimestamp, static_cast<TIME64>(5000));
    EXPECT_EQ(infos[0].ciInstance, "alpha");
    EXPECT_EQ(infos[0].ciLocation, "/opt/alpha");
    EXPECT_EQ(infos[1].ciCookie, 12u);
    EXPECT_EQ(infos[1].ciInstance, "beta");
    EXPECT_EQ(infos[1].ciLocation, "");
    std::vector<areg::ConnectedInstance> junkInfos(5);
    db.log_instance_infos(junkInfos);
    EXPECT_EQ(junkInfos.size(), 2u);

    EXPECT_EQ(_query_text(path, "SELECT inst_connect FROM instances WHERE cookie_id = 11;"), "1");
    EXPECT_TRUE(db.log_instance_disconnected(11u, _at(9000)));
    EXPECT_EQ(_query_text(path, "SELECT inst_connect FROM instances WHERE cookie_id = 11;"), "0");
    EXPECT_EQ(_query_text(path, "SELECT time_disconnected FROM instances WHERE cookie_id = 11;"), "9000");
    EXPECT_TRUE(db.log_instance_disconnected(11u, _at(9500)));
    EXPECT_EQ(_query_text(path, "SELECT time_disconnected FROM instances WHERE cookie_id = 11;"), "9000");
    EXPECT_EQ(_query_text(path, "SELECT inst_connect FROM instances WHERE cookie_id = 12;"), "1");
    db.disconnect();
}

/**
 * \brief   Reproduces the defect: the check counted the row of an instance with an empty name, so a Debug build aborted.
 **/
TEST(LogSqliteDatabaseTest, test_emptyInstanceNameIsLeftOutOfNames)
{
    const std::string path{ _db_path("s5empty.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    ASSERT_TRUE(db.log_instance_connected(_instance(11u, "alpha", "", 5000), _at(7000)));
    ASSERT_TRUE(db.log_instance_connected(_instance(13u, "", "", 6500), _at(7002)));
    const std::vector<areg::String> names{ db.log_instance_names() };
    ASSERT_EQ(names.size(), 1u);
    EXPECT_EQ(_txt(names[0]), "alpha");
    db.disconnect();
}

/**
 * \brief   An instance name with quotes and SQL in it is saved as text and changes no table.
 **/
TEST(LogSqliteDatabaseTest, test_instanceNameWithSqlIsData)
{
    const std::string path{ _db_path("s6inject.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    ASSERT_TRUE(db.log_instance_connected(_instance(15u, "x'); DROP TABLE logs; --", "y\"); DROP TABLE scopes; --", 1), _at(2)));

    EXPECT_TRUE(db.table_exists("logs"));
    EXPECT_TRUE(db.table_exists("scopes"));
    const std::vector<areg::ConnectedInstance> infos{ db.log_instance_infos() };
    ASSERT_EQ(infos.size(), 1u);
    EXPECT_EQ(infos[0].ciInstance, "x'); DROP TABLE logs; --");
    EXPECT_EQ(infos[0].ciLocation, "y\"); DROP TABLE scopes; --");
    db.disconnect();
}

/**
 * \brief   Scopes are saved one by one or as a list, a duplicate in a list is not counted, and the scopes of an
 *          instance are read back; deactivation touches the scope, the instance or all, as asked.
 **/
TEST(LogSqliteDatabaseTest, test_scopesRoundTripAndDeactivate)
{
    const std::string path{ _db_path("s7scopes.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_EQ(db.count_scope_entries(), 0u);

    EXPECT_TRUE(db.log_scope_activate(areg::ScopeEntry("scope_a", 1u, _prioAll), 21u, _at(10)));
    EXPECT_TRUE(db.log_scope_activate(areg::String("scope_b"), 2u, 0x100u, 21u, _at(10)));

    areg::ScopeNames list;
    list.add(areg::ScopeEntry("scope_c", 3u, _prioAll));
    list.add(areg::ScopeEntry("scope_d", 4u, _prioAll));
    list.add(areg::ScopeEntry("scope_c_again", 3u, _prioAll));
    EXPECT_EQ(db.log_scopes_activate(list, 22u, _at(10)), 2u);

    EXPECT_EQ(db.count_scope_entries(), 4u);
    EXPECT_EQ(db.count_scope_entries(21u), 2u);
    EXPECT_EQ(db.count_scope_entries(22u), 2u);
    EXPECT_EQ(db.count_scope_entries(99u), 0u);

    std::vector<areg::ScopeEntry> scopes{ db.log_inst_scopes(21u) };
    ASSERT_EQ(scopes.size(), 2u);
    std::sort(scopes.begin(), scopes.end(), [](const areg::ScopeEntry& l, const areg::ScopeEntry& r) { return l.scopeId < r.scopeId; });
    EXPECT_EQ(_txt(scopes[0].scopeName), "scope_a");
    EXPECT_EQ(scopes[0].scopeId, 1u);
    EXPECT_EQ(scopes[0].scopePrio, _prioAll);
    EXPECT_EQ(_txt(scopes[1].scopeName), "scope_b");
    EXPECT_EQ(scopes[1].scopeId, 2u);
    EXPECT_EQ(scopes[1].scopePrio, 0x100u);
    std::vector<areg::ScopeEntry> junk(7);
    db.log_inst_scopes(junk, 22u);
    EXPECT_EQ(junk.size(), 2u);
    db.log_inst_scopes(junk, 99u);
    EXPECT_TRUE(junk.empty());

    EXPECT_TRUE(db.log_scope_deactivate(21u, 1u, _at(500)));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM scopes WHERE cookie_id = 21 AND scope_is_active = 1;"), "1");
    EXPECT_EQ(_query_text(path, "SELECT time_inactivated FROM scopes WHERE cookie_id = 21 AND scope_id = 1;"), "500");

    EXPECT_TRUE(db.log_scopes_deactivate(22u, _at(600)));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM scopes WHERE cookie_id = 22 AND scope_is_active = 1;"), "0");
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM scopes WHERE cookie_id = 21 AND scope_is_active = 1;"), "1");

    EXPECT_TRUE(db.log_instance_disconnected(21u, _at(700)));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM scopes WHERE scope_is_active = 1;"), "0");
    EXPECT_EQ(_query_text(path, "SELECT time_inactivated FROM scopes WHERE cookie_id = 21 AND scope_id = 2;"), "700");
    db.disconnect();
}

/**
 * \brief   The priority names are the seven priorities a viewer offers, in the order of their value.
 **/
TEST(LogSqliteDatabaseTest, test_priorityNames)
{
    LogDb db;
    const std::vector<areg::String> names{ db.log_priority_names() };
    ASSERT_EQ(names.size(), 7u);
    EXPECT_EQ(_txt(names[0]), _txt(areg::priority_to_string(areg::LogPriority::PrioAny)));
    EXPECT_EQ(_txt(names[3]), _txt(areg::priority_to_string(areg::LogPriority::PrioInfo)));
    EXPECT_EQ(_txt(names[6]), _txt(areg::priority_to_string(areg::LogPriority::PrioFatal)));

    std::vector<areg::String> filled{ areg::String("x") };
    db.log_priority_names(filled);
    EXPECT_EQ(filled.size(), 7u);
}

/**
 * \brief   The threads of the rows are read as names and as ids, each once.
 **/
TEST(LogSqliteDatabaseTest, test_threadNamesAndIds)
{
    const std::string path{ _db_path("s8threads.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    const std::vector<areg::String> names{ db.log_thread_names() };
    int testThread{ 0 };
    for (const areg::String& name : names)
    {
        testThread += (_txt(name) == "TestThread") ? 1 : 0;
    }

    EXPECT_EQ(testThread, 1);
    std::vector<areg::String> junk{ areg::String("x"), areg::String("y"), areg::String("z"), areg::String("w") };
    db.log_thread_names(junk);
    EXPECT_EQ(junk.size(), names.size());

    const std::vector<ITEM_ID> threads{ db.log_threads() };
    EXPECT_FALSE(threads.empty());
    std::vector<ITEM_ID> again{ 12345u };
    db.log_threads(again);
    EXPECT_EQ(again, threads);
    db.disconnect();
}

/**
 * \brief   Every combination of instance and scope selects the rows it names, in time order, with the fields saved.
 **/
TEST(LogSqliteDatabaseTest, test_messageReadersSelectByInstanceAndScope)
{
    const std::string path{ _db_path("s9readers.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    EXPECT_EQ(db.log_messages().size(), 13u);
    EXPECT_EQ(db.log_inst_messages(31u).size(), 6u);
    EXPECT_EQ(db.log_inst_messages(32u).size(), 6u);
    EXPECT_EQ(db.log_inst_messages().size(), 13u);
    EXPECT_EQ(db.log_inst_messages(areg::COOKIE_ANY).size(), 13u);
    EXPECT_EQ(db.log_inst_messages(777u).size(), 0u);
    EXPECT_EQ(db.log_scope_messages(1u).size(), 4u);
    EXPECT_EQ(db.log_scope_messages(0u).size(), 13u);
    EXPECT_EQ(db.log_scope_messages().size(), 13u);
    EXPECT_EQ(db.log_messages(31u, 1u).size(), 2u);
    EXPECT_EQ(db.log_messages(32u, 2u).size(), 2u);
    EXPECT_EQ(db.log_messages(areg::COOKIE_ANY, 0u).size(), 13u);
    EXPECT_EQ(db.log_messages(areg::COOKIE_ANY, 1u).size(), 4u);
    EXPECT_EQ(db.log_messages(31u, 0u).size(), 6u);
    EXPECT_EQ(db.log_messages(31u, 9u).size(), 0u);

    std::vector<areg::SharedBuffer> junk(3);
    db.log_messages(junk);
    EXPECT_EQ(junk.size(), 13u);
    db.log_inst_messages(junk, 31u);
    EXPECT_EQ(junk.size(), 6u);
    db.log_inst_messages(junk);
    EXPECT_EQ(junk.size(), 13u);
    db.log_scope_messages(junk, 2u);
    EXPECT_EQ(junk.size(), 4u);
    db.log_scope_messages(junk);
    EXPECT_EQ(junk.size(), 13u);
    db.log_messages(junk, 32u, 1u);
    EXPECT_EQ(junk.size(), 2u);
    db.log_messages(junk, areg::COOKIE_ANY, 0u);
    EXPECT_EQ(junk.size(), 13u);
    db.log_messages(junk, areg::COOKIE_ANY, 2u);
    EXPECT_EQ(junk.size(), 4u);
    db.log_messages(junk, 31u, 0u);
    EXPECT_EQ(junk.size(), 6u);
    db.log_messages(junk, 31u, 2u);
    EXPECT_EQ(junk.size(), 2u);

    const std::vector<areg::SharedBuffer> inst31{ db.log_inst_messages(31u) };
    ASSERT_EQ(inst31.size(), 6u);
    TIME64 previous{ 0 };
    for (const areg::SharedBuffer& buffer : inst31)
    {
        const areg::LogEntry* entry{ _log_of(buffer) };
        ASSERT_NE(entry, nullptr);
        EXPECT_EQ(entry->logCookie, 31u);
        EXPECT_EQ(entry->logMessagePrio, areg::LogPriority::PrioInfo);
        EXPECT_EQ(entry->logDataType, areg::LogDataType::Remote);
        EXPECT_GT(entry->logTimestamp, previous);
        EXPECT_STREQ(entry->logMessage, "log database test");
        EXPECT_STREQ(entry->logThread, "TestThread");
        EXPECT_STREQ(entry->logModule, "areg-unit-tests");
        previous = entry->logTimestamp;
    }

    EXPECT_EQ(_log_of(inst31.front())->logTimestamp, static_cast<TIME64>(100));
    EXPECT_EQ(_log_of(inst31.front())->logScopeId, 0u);
    EXPECT_EQ(_log_of(inst31.back())->logTimestamp, static_cast<TIME64>(200));
    EXPECT_EQ(_log_of(inst31.back())->logScopeId, 1u);
    db.disconnect();
}

/**
 * \brief   Reproduces the defect: the paged readers did not read the length before the cut, so a cut text looked complete.
 **/
TEST(LogSqliteDatabaseTest, test_cutMessageKeepsLengthBeforeTheCut)
{
    const std::string path{ _db_path("s10cut.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    areg::LogEntry cut = _entry(1000, 0u, 41u, areg::LogPriority::PrioInfo);
    std::memset(cut.logMessage, 'a', areg::LOG_MSG_SIZE - 1u);
    cut.logMessage[areg::LOG_MSG_SIZE - 1u] = '\0';
    cut.logMessageLen = 5000u;

    areg::LogEntry full = _entry(1001, 0u, 42u, areg::LogPriority::PrioInfo);
    std::memset(full.logMessage, 'b', areg::LOG_MSG_SIZE - 1u);
    full.logMessage[areg::LOG_MSG_SIZE - 1u] = '\0';
    full.logMessageLen = areg::LOG_MSG_SIZE - 1u;

    areg::LogEntry shortText = _entry(1002, 0u, 43u, areg::LogPriority::PrioInfo);
    shortText.logMessageLen = 3u;

    ASSERT_TRUE(db.log_message(cut));
    ASSERT_TRUE(db.log_message(full));
    ASSERT_TRUE(db.log_message(shortText));

    const std::vector<areg::SharedBuffer> cutRows{ db.log_inst_messages(41u) };
    ASSERT_EQ(cutRows.size(), 1u);
    EXPECT_EQ(_log_of(cutRows[0])->logMessageLen, 5000u);
    EXPECT_EQ(std::strlen(_log_of(cutRows[0])->logMessage), static_cast<size_t>(areg::LOG_MSG_SIZE - 1u));

    const std::vector<areg::SharedBuffer> fullRows{ db.log_inst_messages(42u) };
    ASSERT_EQ(fullRows.size(), 1u);
    EXPECT_EQ(_log_of(fullRows[0])->logMessageLen, areg::LOG_MSG_SIZE - 1u);

    const std::vector<areg::SharedBuffer> shortRows{ db.log_inst_messages(43u) };
    ASSERT_EQ(shortRows.size(), 1u);
    EXPECT_EQ(_log_of(shortRows[0])->logMessageLen, static_cast<uint32_t>(std::strlen("log database test")));

    const std::vector<areg::SharedBuffer> allRows{ db.log_messages() };
    ASSERT_EQ(allRows.size(), 4u);
    EXPECT_EQ(_log_of(allRows[0])->logMessageLen, 5000u);
    EXPECT_EQ(_log_of(allRows[1])->logMessageLen, areg::LOG_MSG_SIZE - 1u);

    areg::ext::SqliteStatement stmt(db.database());
    ASSERT_EQ(db.setup_statement_read_logs(stmt, static_cast<ITEM_ID>(41u), -1, 0u), 1u);
    std::vector<areg::SharedBuffer> stmtRows;
    ASSERT_EQ(LogDb::log_messages(stmtRows, stmt), 1);
    EXPECT_EQ(_log_of(stmtRows[0])->logMessageLen, 5000u);
    db.disconnect();
}

TEST(LogSqliteDatabaseTest, test_cutMessageKeepsLengthInWindowRead)
{
    const std::string path{ _db_path("s10cutw.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));

    areg::LogEntry cut = _entry(1000, 0u, 41u, areg::LogPriority::PrioInfo);
    std::memset(cut.logMessage, 'a', areg::LOG_MSG_SIZE - 1u);
    cut.logMessage[areg::LOG_MSG_SIZE - 1u] = '\0';
    cut.logMessageLen = 5000u;

    areg::LogEntry full = _entry(1001, 0u, 42u, areg::LogPriority::PrioInfo);
    std::memset(full.logMessage, 'b', areg::LOG_MSG_SIZE - 1u);
    full.logMessage[areg::LOG_MSG_SIZE - 1u] = '\0';
    full.logMessageLen = areg::LOG_MSG_SIZE - 1u;

    areg::LogEntry shortText = _entry(1002, 0u, 43u, areg::LogPriority::PrioInfo);
    shortText.logMessageLen = 3u;

    ASSERT_TRUE(db.log_message(cut));
    ASSERT_TRUE(db.log_message(full));
    ASSERT_TRUE(db.log_message(shortText));

    areg::ext::SqliteStatement stmt(db.database());
    ASSERT_TRUE(db.setup_statement_read_window(stmt, LogDb::LogPosition{ }, 100));
    std::vector<areg::SharedBuffer> rows;
    LogDb::LogPosition last{ };
    ASSERT_EQ(LogDb::fill_log_window(rows, stmt, 100, last), 4);
    stmt.finalize();
    for (const areg::SharedBuffer& row : rows)
    {
        const areg::LogEntry* entry{ _log_of(row) };
        if (entry->logCookie == 41u)
        {
            EXPECT_EQ(entry->logMessageLen, 5000u);
            EXPECT_EQ(std::strlen(entry->logMessage), static_cast<size_t>(areg::LOG_MSG_SIZE - 1u));
        }
        else if (entry->logCookie == 42u)
        {
            EXPECT_EQ(entry->logMessageLen, areg::LOG_MSG_SIZE - 1u);
        }
        else if (entry->logCookie == 43u)
        {
            EXPECT_EQ(entry->logMessageLen, static_cast<uint32_t>(std::strlen("log database test")));
        }
    }

    db.disconnect();
}

/**
 * \brief   A statement set up for the scopes reads the scopes of all instances or of one, and the limit stops the read.
 **/
TEST(LogSqliteDatabaseTest, test_scopeStatementReadsAllOrOneInstance)
{
    const std::string path{ _db_path("s11scopestmt.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set_scopes(db);

    Stmt stmt(db.database());
    std::vector<areg::ScopeEntry> scopes;
    EXPECT_EQ(db.setup_statement_read_scopes(stmt, areg::TARGET_ALL), 4u);
    EXPECT_EQ(LogDb::log_inst_scopes(scopes, stmt), 4);
    EXPECT_EQ(scopes.size(), 4u);

    scopes.clear();
    EXPECT_EQ(db.setup_statement_read_scopes(stmt, areg::TARGET_ALL), 4u);
    EXPECT_EQ(LogDb::log_inst_scopes(scopes, stmt, 3), 3);
    EXPECT_EQ(scopes.size(), 3u);

    scopes.clear();
    EXPECT_EQ(db.setup_statement_read_scopes(stmt, 32u), 2u);
    EXPECT_EQ(LogDb::log_inst_scopes(scopes, stmt), 2);
    ASSERT_EQ(scopes.size(), 2u);
    const std::string joined{ _txt(scopes[0].scopeName) + _txt(scopes[1].scopeName) };
    EXPECT_TRUE((joined == "onetwo") || (joined == "twoone")) << joined;
    EXPECT_EQ(scopes[0].scopeId + scopes[1].scopeId, 3u);

    std::vector<areg::ScopeEntry> slots(5);
    EXPECT_EQ(db.setup_statement_read_scopes(stmt, areg::TARGET_ALL), 4u);
    EXPECT_EQ(LogDb::fill_inst_scopes(slots, stmt, 1u, -1), 4);
    EXPECT_EQ(_txt(slots[0].scopeName), "");
    for (size_t i = 1u; i < slots.size(); ++i)
    {
        EXPECT_NE(_txt(slots[i].scopeName), "") << i;
        EXPECT_TRUE((slots[i].scopeId == 1u) || (slots[i].scopeId == 2u)) << i;
    }

    std::vector<areg::ScopeEntry> two(2);
    EXPECT_EQ(db.setup_statement_read_scopes(stmt, areg::TARGET_ALL), 4u);
    EXPECT_EQ(LogDb::fill_inst_scopes(two, stmt, 0u, 2), 2);
    EXPECT_EQ(LogDb::fill_inst_scopes(two, stmt, 2u, -1), 0);

    stmt.finalize();
    EXPECT_EQ(LogDb::log_inst_scopes(scopes, stmt), 0);
    EXPECT_EQ(LogDb::fill_inst_scopes(slots, stmt, 0u, -1), 0);
    db.disconnect();
}

/**
 * \brief   A statement set up for the logs reads the window it was given, while the returned count is the whole set.
 **/
TEST(LogSqliteDatabaseTest, test_logStatementReadsWindowOfTheSet)
{
    const std::string path{ _db_path("s12logstmt.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    const std::vector<areg::SharedBuffer> all{ db.log_messages() };
    ASSERT_EQ(all.size(), 13u);

    Stmt stmt(db.database());
    std::vector<areg::SharedBuffer> logs;
    EXPECT_EQ(db.setup_statement_read_logs(stmt, areg::TARGET_ALL, 5, 2u), 13u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt), 5);
    ASSERT_EQ(logs.size(), 5u);
    for (size_t i = 0; i < logs.size(); ++i)
    {
        EXPECT_EQ(_log_of(logs[i])->logTimestamp, _log_of(all[i + 2u])->logTimestamp);
    }

    logs.clear();
    EXPECT_EQ(db.setup_statement_read_logs(stmt, areg::TARGET_ALL, -1, 0u), 13u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt, 4), 4);
    EXPECT_EQ(logs.size(), 4u);

    logs.clear();
    EXPECT_EQ(db.setup_statement_read_logs(stmt, 31u, 3, 1u), 6u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt), 3);
    ASSERT_EQ(logs.size(), 3u);
    EXPECT_EQ(_log_of(logs[0])->logTimestamp, static_cast<TIME64>(120));
    EXPECT_EQ(_log_of(logs[2])->logTimestamp, static_cast<TIME64>(160));
    EXPECT_EQ(_log_of(logs[1])->logCookie, 31u);

    std::vector<areg::SharedBuffer> slots(4);
    EXPECT_EQ(db.setup_statement_read_logs(stmt, 32u, -1, 0u), 6u);
    EXPECT_EQ(LogDb::fill_log_messages(slots, stmt, 0u, 4), 4);
    for (const areg::SharedBuffer& slot : slots)
    {
        EXPECT_EQ(_log_of(slot)->logCookie, 32u);
    }

    std::vector<areg::SharedBuffer> none;
    EXPECT_EQ(db.setup_statement_read_logs(stmt, 32u, -1, 0u), 6u);
    EXPECT_EQ(LogDb::fill_log_messages(none, stmt, 0u, -1), 0);

    stmt.finalize();
    EXPECT_EQ(LogDb::log_messages(logs, stmt), 0);
    EXPECT_EQ(LogDb::fill_log_messages(slots, stmt, 0u, -1), 0);
    db.disconnect();
}

/**
 * \brief   The instances are filled into the vector the caller sized, and an empty vector or a closed statement fills nothing.
 **/
TEST(LogSqliteDatabaseTest, test_fillInstancesIntoSizedVector)
{
    const std::string path{ _db_path("s13fillinst.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    ASSERT_TRUE(db.log_instance_connected(_instance(11u, "alpha", "/a", 5000), _at(7000)));
    ASSERT_TRUE(db.log_instance_connected(_instance(12u, "beta", "/b", 6000), _at(7001)));

    Stmt stmt(db.database(), areg::String("SELECT inst_type, inst_bits, cookie_id, time_connected, inst_name, inst_location FROM instances ORDER BY time_connected;"));
    ASSERT_TRUE(stmt.is_valid());
    std::vector<areg::ConnectedInstance> infos(2);
    EXPECT_EQ(LogDb::fill_log_instances(infos, stmt), 2);
    EXPECT_EQ(infos[0].ciCookie, 11u);
    EXPECT_EQ(infos[0].ciInstance, "alpha");
    EXPECT_EQ(infos[1].ciCookie, 12u);
    EXPECT_EQ(infos[1].ciLocation, "/b");

    std::vector<areg::ConnectedInstance> empty;
    stmt.reset();
    EXPECT_EQ(LogDb::fill_log_instances(empty, stmt), 0);

    stmt.finalize();
    EXPECT_EQ(LogDb::fill_log_instances(infos, stmt), 0);
    db.disconnect();
}

/**
 * \brief   The filter counts and the filtered read follow the rules: all by default, a scope or an instance switched
 *          off drops its rows, reset brings them back, and the rows read equal the rows counted.
 **/
TEST(LogSqliteDatabaseTest, test_filterRulesCountAndRead)
{
    const std::string path{ _db_path("s14filter.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    _write_set_scopes(db);

    Stmt stmt(db.database());
    EXPECT_EQ(db.setup_statement_read_filter_logs(stmt, areg::TARGET_ALL), 0u);
    EXPECT_FALSE(db.reset());
    EXPECT_FALSE(db.disable_filter_mask());

    areg::ArrayList<LogDb::ScopeFilter> none;
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 13u);
    EXPECT_EQ(db.count_filter_logs(), 13u);

    std::vector<areg::SharedBuffer> logs;
    EXPECT_EQ(db.setup_statement_read_filter_logs(stmt, areg::TARGET_ALL), 13u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt), 13);

    logs.clear();
    const uint32_t counted{ db.setup_statement_read_filter_logs(stmt, 31u) };
    EXPECT_GT(counted, 0u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt), static_cast<int32_t>(counted));
    for (const areg::SharedBuffer& log : logs)
    {
        EXPECT_EQ(_log_of(log)->logCookie, 31u);
    }

    areg::ArrayList<LogDb::ScopeFilter> scopeOne;
    scopeOne.add(LogDb::ScopeFilter{ 1u, 0u });
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, scopeOne), 9u);
    EXPECT_EQ(db.count_filter_logs(), 9u);

    areg::ArrayList<LogDb::ScopeFilter> scopeTwoOfOne;
    scopeTwoOfOne.add(LogDb::ScopeFilter{ 2u, 0u });
    static_cast<void>(db.setup_filter_logs(32u, scopeTwoOfOne));
    EXPECT_EQ(db.count_filter_logs(), 7u);

    EXPECT_TRUE(db.disable_filter_mask(areg::TARGET_ALL));
    EXPECT_EQ(db.count_filter_logs(), 5u);
    EXPECT_TRUE(db.reset(areg::TARGET_ALL));
    EXPECT_EQ(db.count_filter_logs(), 13u);

    EXPECT_TRUE(db.disable_filter_mask(31u));
    EXPECT_EQ(db.count_filter_logs(), 9u);
    EXPECT_TRUE(db.reset(31u));
    EXPECT_EQ(db.count_filter_logs(), 13u);

    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   The readers of a window and of a filtered window refuse to prepare while no filter rule or no connection
 *          exists, and the index of the time order is created again when it is missing.
 **/
TEST(LogSqliteDatabaseTest, test_windowPreconditionsAndTimeIndex)
{
    const std::string path{ _db_path("s15precond.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    Stmt stmt(db.database());
    const LogDb::LogPosition start{ };
    const LogDb::LogPosition place{ 150, 3u };
    EXPECT_FALSE(db.setup_statement_read_filter_window(stmt, areg::TARGET_ALL, start, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window(stmt, 31u, place, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_back(stmt, areg::TARGET_ALL, start, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_back(stmt, 31u, place, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_at(stmt, areg::TARGET_ALL, 0u, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_at(stmt, 31u, 2u, 10));

    EXPECT_TRUE(db.setup_statement_read_window(stmt, start, 10));
    EXPECT_TRUE(db.setup_statement_read_window(stmt, place, 10));
    EXPECT_TRUE(db.setup_statement_read_window_back(stmt, start, 10));
    EXPECT_TRUE(db.setup_statement_read_window_back(stmt, place, 10));
    EXPECT_TRUE(db.setup_statement_read_window_at(stmt, 2u, 10));

    areg::ArrayList<LogDb::ScopeFilter> none;
    _write_set_scopes(db);
    ASSERT_GT(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
    EXPECT_TRUE(db.setup_statement_read_filter_window(stmt, 31u, place, 10));
    EXPECT_TRUE(db.setup_statement_read_filter_window_back(stmt, 31u, place, 10));
    EXPECT_TRUE(db.setup_statement_read_filter_window_at(stmt, 31u, 1u, 10));
    stmt.finalize();

    EXPECT_TRUE(db.execute(areg::String("DROP INDEX idx_logs_time;")));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM sqlite_master WHERE name = 'idx_logs_time';"), "0");
    EXPECT_TRUE(db.ensure_time_index());
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM sqlite_master WHERE name = 'idx_logs_time';"), "1");
    EXPECT_TRUE(db.ensure_time_index());

    EXPECT_TRUE(db.drop_table("logs"));
    EXPECT_FALSE(db.ensure_time_index());
    db.disconnect();
}

/**
 * \brief   Refused spans are replaced by each call and an empty list clears them; the rows of a refused span
 *          are not read by a window.
 **/
TEST(LogSqliteDatabaseTest, test_refusedSpansReplaceEachOther)
{
    const std::string path{ _db_path("s16refused.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    const auto window = [&](int32_t limit) -> size_t
    {
        Stmt stmt(db.database());
        std::vector<areg::SharedBuffer> logs;
        LogDb::LogPosition last{ };
        if (db.setup_statement_read_window(stmt, LogDb::LogPosition{ }, limit) == false)
            return 9999u;

        static_cast<void>(LogDb::fill_log_window(logs, stmt, limit, last));
        return logs.size();
    };

    EXPECT_EQ(window(100), 13u);

    areg::ArrayList<LogDb::RefusedSpan> spans;
    spans.add(LogDb::RefusedSpan{ 1u, 32u, 0, 0 });
    ASSERT_TRUE(db.set_refused_spans(spans));
    EXPECT_EQ(window(100), 11u);

    spans.add(LogDb::RefusedSpan{ 0u, 31u, 0, 0 });
    ASSERT_TRUE(db.set_refused_spans(spans));
    EXPECT_EQ(window(100), 9u);

    areg::ArrayList<LogDb::RefusedSpan> bounded;
    bounded.add(LogDb::RefusedSpan{ 1u, 32u, 100, 130 });
    ASSERT_TRUE(db.set_refused_spans(bounded));
    EXPECT_EQ(window(100), 12u);

    areg::ArrayList<LogDb::RefusedSpan> noSpans;
    ASSERT_TRUE(db.set_refused_spans(noSpans));
    EXPECT_EQ(window(100), 13u);
    db.disconnect();
}

/**
 * \brief   A log that is not connected refuses every command and every reader and returns empty results.
 **/
TEST(LogSqliteDatabaseTest, test_notConnectedLogRefusesEverything)
{
    LogDb db;
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.tables_initialized());
    EXPECT_FALSE(db.execute(areg::String("SELECT 1;")));
    EXPECT_FALSE(db.begin());
    EXPECT_FALSE(db.commit(true));
    EXPECT_FALSE(db.rollback());
    EXPECT_FALSE(db.table_exists("logs"));
    EXPECT_FALSE(db.log_message(_entry(1, 0u, 31u, areg::LogPriority::PrioInfo)));
    EXPECT_FALSE(db.log_message(_entry(1, 0u, 31u, areg::LogPriority::PrioInfo), 7u));
    EXPECT_FALSE(db.log_dropped_messages(3u, 8u));
    EXPECT_EQ(db.max_log_id(), 0u);
    EXPECT_EQ(db.max_marker_id(), 0u);
    EXPECT_EQ(db.last_log_id(), 0u);
    EXPECT_FALSE(db.remove_log(1u));
    EXPECT_FALSE(db.remove_marker(1u));
    EXPECT_EQ(db.add_marker(1u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);
    EXPECT_FALSE(db.insert_marker(5u, 1u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n"), 1));
    std::vector<LogDb::LogMarker> markers;
    EXPECT_EQ(db.read_markers(markers), 0u);

    EXPECT_EQ(db.count_log_entries(), 0u);
    EXPECT_EQ(db.count_scope_entries(), 0u);
    EXPECT_EQ(db.count_log_instances(), 0u);
    EXPECT_EQ(db.count_filter_logs(), 0u);
    areg::ArrayList<LogDb::ScopeFilter> none;
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
    EXPECT_FALSE(db.ensure_time_index());
    EXPECT_FALSE(db.reset());
    EXPECT_FALSE(db.disable_filter_mask());

    Stmt stmt(db.database());
    const LogDb::LogPosition place{ 150, 3u };
    EXPECT_EQ(db.setup_statement_read_filter_logs(stmt, areg::TARGET_ALL), 0u);
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, 0xFFFFu), 0u);
    EXPECT_FALSE(db.setup_statement_read_window(stmt, place, 5));
    EXPECT_FALSE(db.setup_statement_read_window_back(stmt, place, 5));
    EXPECT_FALSE(db.setup_statement_read_window_at(stmt, 1u, 5));
    EXPECT_FALSE(db.setup_statement_read_filter_window(stmt, areg::TARGET_ALL, place, 5));
    EXPECT_FALSE(db.setup_statement_read_filter_window_back(stmt, areg::TARGET_ALL, place, 5));
    EXPECT_FALSE(db.setup_statement_read_filter_window_at(stmt, areg::TARGET_ALL, 1u, 5));
    areg::ArrayList<LogDb::RefusedSpan> spans;
    EXPECT_FALSE(db.set_refused_spans(spans));

    EXPECT_TRUE(db.log_messages().empty());
    EXPECT_TRUE(db.log_instance_infos().empty());
    EXPECT_TRUE(db.log_instances().empty());
    db.disconnect();
}

/**
 * \brief   A dropped-messages row gets the identifier it is given, an error priority and the number it reports.
 **/
TEST(LogSqliteDatabaseTest, test_droppedMessagesRowKeepsItsId)
{
    const std::string path{ _db_path("s17dropped.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    ASSERT_TRUE(db.log_dropped_messages(7u, 500u));
    EXPECT_EQ(db.max_log_id(), 500u);
    EXPECT_EQ(_query_text(path, "SELECT msg_prio FROM logs WHERE id = 500;"), std::to_string(static_cast<uint32_t>(areg::LogPriority::PrioError)));
    EXPECT_EQ(_query_text(path, "SELECT msg_log FROM logs WHERE id = 500;").rfind("Dropped [ 7 ]", 0), 0u);
    db.disconnect();
}

/**
 * \brief   A marker with a given identifier is saved with it, a second one with the same identifier is refused, and
 *          a marker of a missing row or of a level that is no marker level is refused.
 **/
TEST(LogSqliteDatabaseTest, test_insertMarkerWithGivenId)
{
    const std::string path{ _db_path("s18marker.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    EXPECT_FALSE(db.insert_marker(0u, 2u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n"), 1));
    EXPECT_TRUE(db.insert_marker(50u, 2u, areg::LogPriority::PrioMarkerError, areg::String("author"), areg::String("note"), 1234));
    EXPECT_FALSE(db.insert_marker(50u, 3u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n"), 1));
    EXPECT_FALSE(db.insert_marker(51u, 9999u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n"), 1));
    EXPECT_FALSE(db.insert_marker(52u, 2u, areg::LogPriority::PrioInfo, areg::String("a"), areg::String("n"), 1));
    EXPECT_EQ(db.add_marker(9999u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);
    EXPECT_EQ(db.add_marker(0u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);

    std::vector<LogDb::LogMarker> markers;
    ASSERT_EQ(db.read_markers(markers), 1u);
    EXPECT_EQ(markers[0].markerId, 50u);
    EXPECT_EQ(markers[0].masterId, 2u);
    EXPECT_EQ(markers[0].level, areg::LogPriority::PrioMarkerError);
    EXPECT_EQ(markers[0].timeSet, static_cast<TIME64>(1234));
    EXPECT_EQ(_txt(markers[0].author), "author");
    EXPECT_EQ(_txt(markers[0].note), "note");
    EXPECT_EQ(db.max_marker_id(), 50u);

    EXPECT_FALSE(db.remove_marker(0u));
    EXPECT_TRUE(db.remove_marker(50u));
    EXPECT_EQ(db.read_markers(markers), 0u);
    EXPECT_FALSE(db.remove_log(0u));
    db.disconnect();
}

/**
 * \brief   An author and a note longer than the limit are cut, never inside a multibyte character.
 **/
TEST(LogSqliteDatabaseTest, test_markerTextCutKeepsCharactersWhole)
{
    const std::string path{ _db_path("s19cut.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    const std::string author{ std::string(62u, 'a') + "\xC3\xA9" + "tail" };
    const std::string note{ std::string(1022u, 'n') + "\xC3\xA9" + "tail" };
    const std::string longAscii(2000u, 'z');
    ASSERT_NE(db.add_marker(2u, areg::LogPriority::PrioMarker, areg::String(author), areg::String(note)), 0u);
    ASSERT_NE(db.add_marker(3u, areg::LogPriority::PrioMarker, areg::String(longAscii), areg::String(longAscii)), 0u);

    std::vector<LogDb::LogMarker> markers;
    ASSERT_EQ(db.read_markers(markers), 2u);
    EXPECT_EQ(_txt(markers[0].author), std::string(62u, 'a'));
    EXPECT_EQ(_txt(markers[0].note), std::string(1022u, 'n'));
    EXPECT_EQ(static_cast<uint32_t>(markers[1].author.length()), areg::LOG_NAME_SIZE - 1u);
    EXPECT_EQ(static_cast<uint32_t>(markers[1].note.length()), 1023u);
    db.disconnect();
}

/**
 * \brief   The marked-log reader counts and reads the rows of one instance only when asked for one.
 **/
TEST(LogSqliteDatabaseTest, test_markedLogsOfOneInstance)
{
    const std::string path{ _db_path("s20marked.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    ASSERT_NE(db.add_marker(2u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);
    ASSERT_NE(db.add_marker(3u, areg::LogPriority::PrioMarkerWarning, areg::String("a"), areg::String("n")), 0u);
    ASSERT_NE(db.add_marker(4u, areg::LogPriority::PrioMarkerError, areg::String("a"), areg::String("n")), 0u);

    Stmt stmt(db.database());
    std::vector<areg::SharedBuffer> logs;
    const uint16_t any{ static_cast<uint16_t>(areg::LogPriority::PrioMarker) | static_cast<uint16_t>(areg::LogPriority::PrioMarkerWarning) | static_cast<uint16_t>(areg::LogPriority::PrioMarkerError) | static_cast<uint16_t>(areg::LogPriority::PrioMarkerInfo) };
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, any), 3u);
    EXPECT_EQ(LogDb::log_messages(logs, stmt), 3);

    logs.clear();
    const uint32_t ofOne{ db.setup_statement_read_marked_logs(stmt, 31u, any) };
    EXPECT_EQ(LogDb::log_messages(logs, stmt), static_cast<int32_t>(ofOne));
    for (const areg::SharedBuffer& log : logs)
    {
        EXPECT_EQ(_log_of(log)->logCookie, 31u);
    }

    EXPECT_EQ(ofOne, 2u);
    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   Reproduces the defect: a counter asserted that its statement prepared, so a file without the table
 *          ended a Debug process. A missing table gives an empty result and the log stays usable.
 **/
TEST(LogSqliteDatabaseTest, test_missingTablesGiveEmptyResults)
{
    const std::string path{ _db_path("s21missing.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    _write_set_scopes(db);
    areg::ArrayList<LogDb::ScopeFilter> none;
    ASSERT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 13u);

    Stmt stmt(db.database());
    ASSERT_TRUE(db.drop_table("scopes"));
    EXPECT_EQ(db.count_scope_entries(), 0u);
    EXPECT_EQ(db.count_scope_entries(31u), 0u);
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
    EXPECT_EQ(db.setup_statement_read_filter_logs(stmt, areg::TARGET_ALL), 0u);

    ASSERT_NE(db.add_marker(2u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);
    ASSERT_TRUE(db.drop_table("logs"));
    EXPECT_EQ(db.count_log_entries(), 0u);
    EXPECT_EQ(db.count_log_entries(31u), 0u);
    EXPECT_EQ(db.count_filter_logs(), 0u);
    EXPECT_EQ(db.count_filter_logs(31u), 0u);
    EXPECT_EQ(db.setup_statement_read_marked_logs(stmt, areg::TARGET_ALL, 0xFFFFu), 0u);
    EXPECT_FALSE(stmt.is_valid());

    const LogDb::LogPosition start{ };
    const LogDb::LogPosition place{ 150, 3u };
    EXPECT_FALSE(db.setup_statement_read_filter_window(stmt, areg::TARGET_ALL, start, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_back(stmt, 31u, place, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_at(stmt, areg::TARGET_ALL, 0u, 10));
    EXPECT_FALSE(db.setup_statement_read_filter_window_at(stmt, 31u, 1u, 10));

    ASSERT_TRUE(db.drop_table("instances"));
    EXPECT_EQ(db.count_log_instances(), 0u);

    EXPECT_TRUE(db.execute(areg::String("CREATE TABLE probe (x INTEGER);")));
    EXPECT_TRUE(db.table_exists("probe"));
    stmt.finalize();
    db.disconnect();
}

/**
 * \brief   A filter whose temporary table, its rows or its index cannot be made is set up for nothing and
 *          leaves no rule behind.
 **/
TEST(LogSqliteDatabaseTest, test_filterSetupFailsAtEachStep)
{
    areg::ArrayList<LogDb::ScopeFilter> none;
    const auto open = [](LogDb& db, const char* name)
    {
        ASSERT_TRUE(db.connect(areg::String(_db_path(name).c_str()), areg::DatabaseRole::Recorder));
        _write_set(db);
        _write_set_scopes(db);
    };

    {
        LogDb db;
        open(db, "s22table.sqlog");
        ASSERT_TRUE(db.execute(areg::String("CREATE TEMP VIEW filter_rules AS SELECT 1 AS x;")));
        EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
        EXPECT_FALSE(db.table_exists("filter_rules", "sqlite_temp_master"));
        db.disconnect();
    }

    {
        LogDb db;
        open(db, "s22rows.sqlog");
        ASSERT_TRUE(db.drop_table("scopes"));
        ASSERT_TRUE(db.execute(areg::String("CREATE TABLE scopes (scope_id INTEGER);")));
        EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
        EXPECT_FALSE(db.table_exists("filter_rules", "sqlite_temp_master"));
        db.disconnect();
    }

    {
        LogDb db;
        open(db, "s22index.sqlog");
        ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TABLE idx_filter_rules (x INTEGER);")));
        EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
        EXPECT_FALSE(db.table_exists("filter_rules", "sqlite_temp_master"));
        db.disconnect();
    }
}

/**
 * \brief   A filter rule that cannot be written makes the setup count nothing, with and without scopes given.
 **/
TEST(LogSqliteDatabaseTest, test_filterUpdateRefusedCountsNothing)
{
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(_db_path("s23update.sqlog").c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    _write_set_scopes(db);
    areg::ArrayList<LogDb::ScopeFilter> none;
    ASSERT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 13u);
    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TRIGGER no_update BEFORE UPDATE ON filter_rules BEGIN SELECT RAISE(ABORT, 'refused'); END;")));

    areg::ArrayList<LogDb::ScopeFilter> scopeOne;
    scopeOne.add(LogDb::ScopeFilter{ 1u, 0u });
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, scopeOne), 0u);
    EXPECT_EQ(db.setup_filter_logs(31u, scopeOne), 0u);
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, none), 0u);
    EXPECT_EQ(db.count_filter_logs(), 13u);

    ASSERT_TRUE(db.execute(areg::String("DROP TRIGGER no_update;")));
    EXPECT_EQ(db.setup_filter_logs(areg::TARGET_ALL, scopeOne), 9u);
    db.disconnect();
}

/**
 * \brief   A filter table of another shape makes reset and disable fail, for all instances and for one.
 **/
TEST(LogSqliteDatabaseTest, test_foreignFilterTableRefusesResetAndDisable)
{
    // Reproduces the defect: the failed prepare aborted a Debug build.
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(_db_path("s29foreignfilter.sqlog").c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TABLE filter_rules (x INTEGER);")));

    EXPECT_FALSE(db.reset(areg::TARGET_ALL));
    EXPECT_FALSE(db.reset(31u));
    EXPECT_FALSE(db.disable_filter_mask(areg::TARGET_ALL));
    EXPECT_FALSE(db.disable_filter_mask(31u));
    EXPECT_EQ(db.count_log_entries(), 13u);
    db.disconnect();
}

/**
 * \brief   A row whose deletion is refused stays with its markers; a marker table that cannot be indexed takes
 *          no marker.
 **/
TEST(LogSqliteDatabaseTest, test_refusedRemovalKeepsRowAndMarkers)
{
    const std::string path{ _db_path("s24remove.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    ASSERT_NE(db.add_marker(2u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);

    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TRIGGER no_delete_log BEFORE DELETE ON logs BEGIN SELECT RAISE(ABORT, 'refused'); END;")));
    EXPECT_FALSE(db.remove_log(2u));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM markers WHERE master_id = 2;"), "1");
    EXPECT_EQ(db.count_log_entries(), 13u);
    ASSERT_TRUE(db.execute(areg::String("DROP TRIGGER no_delete_log;")));

    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TRIGGER no_delete_marker BEFORE DELETE ON markers BEGIN SELECT RAISE(ABORT, 'refused'); END;")));
    EXPECT_FALSE(db.remove_log(2u));
    EXPECT_EQ(db.count_log_entries(), 13u);
    ASSERT_TRUE(db.execute(areg::String("DROP TRIGGER no_delete_marker;")));
    EXPECT_TRUE(db.remove_log(2u));
    EXPECT_EQ(db.count_log_entries(), 12u);

    ASSERT_TRUE(db.drop_table("markers"));
    ASSERT_TRUE(db.execute(areg::String("CREATE VIEW markers AS SELECT 1 AS x;")));
    EXPECT_EQ(db.add_marker(3u, areg::LogPriority::PrioMarker, areg::String("a"), areg::String("n")), 0u);
    db.disconnect();
}

/**
 * \brief   A marker text made only of UTF-8 continuation bytes is cut to nothing, never inside a sequence.
 **/
TEST(LogSqliteDatabaseTest, test_markerTextOfContinuationBytesIsCutToNothing)
{
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(_db_path("s25tail.sqlog").c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);

    const std::string tail(2000u, '\x80');
    ASSERT_NE(db.add_marker(2u, areg::LogPriority::PrioMarker, areg::String(tail), areg::String(tail)), 0u);
    std::vector<LogDb::LogMarker> markers;
    ASSERT_EQ(db.read_markers(markers), 1u);
    EXPECT_TRUE(markers[0].author.is_empty());
    EXPECT_TRUE(markers[0].note.is_empty());
    db.disconnect();
}

/**
 * \brief   Refused spans that cannot be cleared, written or given a table are refused, and a later call with
 *          the table repaired succeeds.
 **/
TEST(LogSqliteDatabaseTest, test_refusedSpansWriteFailures)
{
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(_db_path("s26spans.sqlog").c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    areg::ArrayList<LogDb::RefusedSpan> spans;
    spans.add(LogDb::RefusedSpan{ 1u, 32u, 0, 0 });

    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TRIGGER no_insert BEFORE INSERT ON refused_spans BEGIN SELECT RAISE(ABORT, 'refused'); END;")));
    EXPECT_FALSE(db.set_refused_spans(spans));
    ASSERT_TRUE(db.execute(areg::String("DROP TRIGGER no_insert;")));
    ASSERT_TRUE(db.set_refused_spans(spans));

    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP TRIGGER no_clear BEFORE DELETE ON refused_spans BEGIN SELECT RAISE(ABORT, 'refused'); END;")));
    EXPECT_FALSE(db.set_refused_spans(spans));
    ASSERT_TRUE(db.execute(areg::String("DROP TRIGGER no_clear;")));

    ASSERT_TRUE(db.execute(areg::String("DROP TABLE temp.refused_spans;")));
    ASSERT_TRUE(db.execute(areg::String("CREATE TEMP VIEW refused_spans AS SELECT 1 AS x;")));
    EXPECT_FALSE(db.set_refused_spans(spans));
    EXPECT_FALSE(db.setup_statement_read_window(*std::make_unique<Stmt>(db.database()), LogDb::LogPosition{ }, 10));

    ASSERT_TRUE(db.execute(areg::String("DROP VIEW temp.refused_spans;")));
    EXPECT_TRUE(db.set_refused_spans(spans));
    db.disconnect();
}

/**
 * \brief   A recorder refuses a path that is a directory and leaves the directory as it is.
 **/
TEST(LogSqliteDatabaseTest, test_recorderRefusesADirectory)
{
    const std::string path{ _db_path("s27dir.sqlog") };
    std::error_code err;
    ASSERT_TRUE(fs::create_directories(path, err));
    LogDb db;
    EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.tables_initialized());
    EXPECT_TRUE(fs::is_directory(path, err));
    db.disconnect();
    fs::remove(path, err);
}

/**
 * \brief   A recorder refuses an empty file it cannot write, because the tables cannot be created.
 **/
TEST(LogSqliteDatabaseTest, test_recorderRefusesAnEmptyReadOnlyFile)
{
    // Reproduces the defect: the failed CREATE TABLE aborted a Debug build and connected a Release build.
    const std::string path{ _db_path("s28readonly.sqlog") };
    _write_file(path, "");
    std::error_code err;
    fs::permissions(path, fs::perms::owner_read | fs::perms::group_read | fs::perms::others_read, fs::perm_options::replace, err);
    constexpr uint32_t MODE{ static_cast<uint32_t>(areg::File::OpenMode::Write) | static_cast<uint32_t>(areg::File::OpenMode::Exist) };
    areg::File probe;
    if (probe.open(path.c_str(), MODE))
    {
        probe.close();
        fs::permissions(path, fs::perms::owner_write, fs::perm_options::add, err);
        fs::remove(path, err);
        GTEST_SKIP() << "the account writes read-only files";
    }

    LogDb db;
    EXPECT_FALSE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.tables_initialized());
    db.disconnect();
    EXPECT_EQ(_file_size(path), 0u);

    fs::permissions(path, fs::perms::owner_write, fs::perm_options::add, err);
    fs::remove(path, err);
}

//////////////////////////////////////////////////////////////////////////
// The writer thread: start and stop rules, tasks and the transaction limits.
//////////////////////////////////////////////////////////////////////////

#if AREG_LOGGING

//////////////////////////////////////////////////////////////////////////
// The writer thread: identifiers given when queued, drops, markers in order, stop saves all.
//////////////////////////////////////////////////////////////////////////

namespace
{
    using LogWriter = areg::ext::LogDbWriter;

    //! The cookie of the rows the writer tests queue, other than the one of the header row.
    constexpr ITEM_ID _writerCookie{ 77u };

    areg::MessageEnvelope _envelope(uint32_t i)
    {
        return areg::create_log_message(_entry(static_cast<TIME64>(1000u + i), 0u, _writerCookie, areg::LogPriority::PrioInfo), areg::LogDataType::Remote, _writerCookie);
    }

    //! Queues a task that holds the writer thread until the gate opens.
    void _hold_writer(LogWriter& writer, areg::SyncEvent& gate)
    {
        ASSERT_TRUE(writer.queue_task([&gate](LogDb& /*database*/) { gate.lock(areg::WAIT_INFINITE); }));
    }
}

/**
 * \brief   A queued row gets its identifier at once, and the file holds the rows with exactly those identifiers.
 **/
TEST(LogSqliteDatabaseTest, test_writerGivesIdsWhenQueued)
{
    const std::string path{ _db_path("w1ids.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    const uint32_t first{ db.max_log_id() + 1u };
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    std::vector<uint32_t> queued;
    for (uint32_t i = 0; i < 2500u; ++i)
    {
        queued.push_back(writer.queue_log(_envelope(i)));
    }

    writer.stop();
    db.disconnect();

    for (uint32_t i = 0; i < 2500u; ++i)
    {
        ASSERT_EQ(queued[i], first + i);
    }

    std::vector<uint32_t> saved;
    for (const Row& row : _rows_in_time_order(path))
    {
        if (row.cookie == static_cast<uint32_t>(_writerCookie))
        {
            saved.push_back(row.id);
        }
    }

    EXPECT_EQ(saved, queued);
}

/**
 * \brief   A full queue drops new rows and counts them; the file gets the accepted rows and one row
 *          that reports the drops, and no identifier is used twice.
 **/
TEST(LogSqliteDatabaseTest, test_writerDropsAndReportsWhenFull)
{
    const std::string path{ _db_path("w2drop.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(1u));

    areg::SyncEvent gate(true, false);
    _hold_writer(writer, gate);

    uint32_t accepted{ 0u };
    uint32_t dropped{ 0u };
    for (uint32_t i = 0; (i < 1'000'000u) && (dropped < 25u); ++i)
    {
        if (writer.queue_log(_envelope(i)) != 0u)
        {
            ++accepted;
        }
        else
        {
            ++dropped;
        }
    }

    const LogWriter::QueueState state{ writer.queue_state() };
    gate.set_signaled();
    writer.stop();
    db.disconnect();

    ASSERT_EQ(dropped, 25u);
    EXPECT_EQ(state.dropped, 25u);
    EXPECT_EQ(state.pending, accepted);
    EXPECT_LE(state.pendingBytes, state.limitBytes);
    EXPECT_EQ(state.limitBytes, 1024u * 1024u);
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;"), std::to_string(accepted));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(DISTINCT id) = COUNT(*) FROM logs;"), "1");
    EXPECT_EQ(_query_text(path, "SELECT msg_log FROM logs WHERE msg_log LIKE 'Dropped%';").rfind("Dropped [ 25 ]", 0), 0u);
}

/**
 * \brief   A marker of a row still in the queue is saved after the row and attached to it; a marker
 *          of a row never queued, or with a level that is no marker level, gets no identifier.
 **/
TEST(LogSqliteDatabaseTest, test_writerSavesMarkerOfQueuedRow)
{
    const std::string path{ _db_path("w3mark.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    areg::SyncEvent gate(true, false);
    _hold_writer(writer, gate);
    const uint32_t logId{ writer.queue_log(_envelope(1u)) };
    ASSERT_NE(logId, 0u);
    const uint32_t markerId{ writer.queue_marker(logId, areg::LogPriority::PrioMarkerWarning, "tester", "queued row") };
    EXPECT_NE(markerId, 0u);
    EXPECT_EQ(writer.queue_marker(logId + 5u, areg::LogPriority::PrioMarker, "", ""), 0u);
    EXPECT_EQ(writer.queue_marker(logId, areg::LogPriority::PrioInfo, "", ""), 0u);

    gate.set_signaled();
    writer.stop();

    std::vector<LogDb::LogMarker> markers;
    ASSERT_EQ(db.read_markers(markers), 1u);
    EXPECT_EQ(markers[0].markerId, markerId);
    EXPECT_EQ(markers[0].masterId, logId);
    EXPECT_EQ(markers[0].level, areg::LogPriority::PrioMarkerWarning);
    db.disconnect();
}

/**
 * \brief   Stopping the writer saves every row that was queued before the stop.
 **/
TEST(LogSqliteDatabaseTest, test_writerStopSavesTheQueue)
{
    const std::string path{ _db_path("w4stop.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    areg::SyncEvent gate(true, false);
    _hold_writer(writer, gate);
    for (uint32_t i = 0; i < 5000u; ++i)
    {
        ASSERT_NE(writer.queue_log(_envelope(i)), 0u);
    }

    gate.set_signaled();
    writer.stop();
    EXPECT_EQ(writer.queue_log(_envelope(0u)), 0u);
    EXPECT_EQ(writer.queue_state().pending, 0u);
    db.disconnect();

    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;"), "5000");
}

/**
 * \brief   A single row is committed without more rows coming, so another connection reads it soon.
 **/
TEST(LogSqliteDatabaseTest, test_writerCommitsALoneRow)
{
    const std::string path{ _db_path("w5lone.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));
    ASSERT_NE(writer.queue_log(_envelope(1u)), 0u);

    std::string count;
    const auto until{ std::chrono::steady_clock::now() + std::chrono::seconds(60) };
    while ((count != "1") && (std::chrono::steady_clock::now() < until))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        count = _query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;");
    }

    EXPECT_EQ(count, "1");
    writer.stop();
    db.disconnect();
}

/**
 * \brief   A writer on a log that is not open does not start; a started writer reports the default queue size and
 *          starting it again changes nothing.
 **/
TEST(LogSqliteDatabaseTest, test_writerStartRules)
{
    LogDb closed;
    LogWriter idle(closed);
    EXPECT_FALSE(idle.start(0u));
    EXPECT_FALSE(idle.is_running());
    EXPECT_EQ(idle.queue_log(_envelope(1u)), 0u);
    EXPECT_FALSE(idle.queue_task([](LogDb&) { }));
    EXPECT_EQ(idle.queue_marker(1u, areg::LogPriority::PrioMarker, "a", "n"), 0u);

    const std::string path{ _db_path("w6start.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));
    EXPECT_TRUE(writer.is_running());
    EXPECT_TRUE(writer.start(1u));
    EXPECT_EQ(writer.queue_state().limitBytes, static_cast<uint64_t>(LogWriter::DEFAULT_QUEUE_MB) * 1024u * 1024u);
    EXPECT_TRUE(writer.queue_state().running);

    writer.stop();
    EXPECT_FALSE(writer.is_running());
    EXPECT_FALSE(writer.queue_state().running);
    db.disconnect();
}

/**
 * \brief   A task that is empty, or that comes while the writer does not run, is refused; a marker of no row, or
 *          of a row the writer never gave out, is refused.
 **/
TEST(LogSqliteDatabaseTest, test_writerRefusesTasksAndMarkers)
{
    const std::string path{ _db_path("w7refuse.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    EXPECT_FALSE(writer.queue_task(LogWriter::Task{ }));
    EXPECT_EQ(writer.queue_marker(0u, areg::LogPriority::PrioMarker, "a", "n"), 0u);
    EXPECT_EQ(writer.queue_marker(1u + db.max_log_id() + 100u, areg::LogPriority::PrioMarker, "a", "n"), 0u);

    writer.stop();
    EXPECT_FALSE(writer.queue_task([](LogDb&) { }));
    EXPECT_EQ(writer.queue_marker(1u, areg::LogPriority::PrioMarker, "a", "n"), 0u);
    EXPECT_EQ(writer.queue_log(_envelope(1u)), 0u);
    db.disconnect();
}

/**
 * \brief   Tasks run on the writer thread in the order they were queued, and a task reaches the database.
 **/
TEST(LogSqliteDatabaseTest, test_writerRunsTasksInOrder)
{
    const std::string path{ _db_path("w8tasks.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    std::vector<int> order;
    ASSERT_TRUE(writer.queue_task([&order](LogDb&) { order.push_back(1); }));
    ASSERT_TRUE(writer.queue_task([&order](LogDb& database) { order.push_back(2); database.execute(areg::String("CREATE TABLE task_probe (a INTEGER);")); }));
    ASSERT_TRUE(writer.queue_task([&order](LogDb&) { order.push_back(3); }));
    writer.stop();

    EXPECT_EQ(order, (std::vector<int>{ 1, 2, 3 }));
    EXPECT_TRUE(db.table_exists("task_probe"));
    db.disconnect();
}

/**
 * \brief   A row written while the writer is stopped inside an open transaction is saved by the stop.
 **/
TEST(LogSqliteDatabaseTest, test_writerStopSavesRowOfOpenTransaction)
{
    const std::string path{ _db_path("w9open.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    areg::SyncEvent written(true, false);
    ASSERT_NE(writer.queue_log(_envelope(1u)), 0u);
    ASSERT_TRUE(writer.queue_task([&written](LogDb&) { written.set_signaled(); }));
    ASSERT_TRUE(written.lock(60'000u));

    writer.stop();
    db.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;"), "1");
}

/**
 * \brief   A transaction that is open longer than the limit is committed after its batch, without more rows coming
 *          and without a stop, so another connection reads the row.
 **/
TEST(LogSqliteDatabaseTest, test_writerCommitsOldTransactionAfterBatch)
{
    const std::string path{ _db_path("w10old.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    ASSERT_NE(writer.queue_log(_envelope(1u)), 0u);
    ASSERT_TRUE(writer.queue_task([](LogDb&)
        {
            const uint64_t begin{ areg::DateTime::system_tick_count() };
            while ((areg::DateTime::system_tick_count() - begin) < (LogWriter::BATCH_TIMEOUT_MS + 50u))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }));

    std::string count;
    const auto until{ std::chrono::steady_clock::now() + std::chrono::seconds(60) };
    while ((count != "1") && (std::chrono::steady_clock::now() < until))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        count = _query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;");
    }

    EXPECT_EQ(count, "1");
    writer.stop();
    db.disconnect();
}

/**
 * \brief   A queue of more than 65 000 entries is run completely and in order, and the writer keeps working after it.
 **/
TEST(LogSqliteDatabaseTest, test_writerRunsEveryTaskOfALargeQueue)
{
    const std::string path{ _db_path("w11large.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    areg::SyncEvent gate(true, false);
    _hold_writer(writer, gate);

    constexpr uint32_t tasks{ 70'000u };
    uint32_t executed{ 0u };
    uint32_t outOfOrder{ 0u };
    for (uint32_t i = 0; i < tasks; ++i)
    {
        ASSERT_TRUE(writer.queue_task([&executed, &outOfOrder, i](LogDb&)
            {
                outOfOrder += (executed == i) ? 0u : 1u;
                ++ executed;
            }));
    }

    gate.set_signaled();
    areg::SyncEvent after(true, false);
    ASSERT_TRUE(writer.queue_task([&after](LogDb&) { after.set_signaled(); }));
    ASSERT_TRUE(after.lock(60'000u));
    EXPECT_EQ(executed, tasks);
    EXPECT_EQ(outOfOrder, 0u);

    ASSERT_NE(writer.queue_log(_envelope(1u)), 0u);
    writer.stop();
    db.disconnect();
    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;"), "1");
}

/**
 * \brief   More rows than one transaction holds, queued before the writer runs, are all saved with distinct identifiers.
 **/
TEST(LogSqliteDatabaseTest, test_writerSavesMoreRowsThanOneTransactionHolds)
{
    const std::string path{ _db_path("w12batch.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    LogWriter writer(db);
    ASSERT_TRUE(writer.start(0u));

    areg::SyncEvent gate(true, false);
    _hold_writer(writer, gate);
    constexpr uint32_t rows{ LogWriter::BATCH_MESSAGES + 100u };
    const areg::MessageEnvelope envelope{ _envelope(1u) };
    for (uint32_t i = 0; i < rows; ++i)
    {
        ASSERT_NE(writer.queue_log(envelope), 0u);
    }

    gate.set_signaled();
    writer.stop();
    db.disconnect();

    EXPECT_EQ(_query_text(path, "SELECT COUNT(*) FROM logs WHERE cookie_id = 77;"), std::to_string(rows));
    EXPECT_EQ(_query_text(path, "SELECT COUNT(DISTINCT id) = COUNT(*) FROM logs;"), "1");
}

#endif  // AREG_LOGGING

/**
 * \brief   Every query on a file whose tables were dropped returns nothing; a missing marker is
 *          removed without an error, and no marker is removed from a file without the markers table.
 **/
TEST(LogSqliteDatabaseTest, test_queriesOfDroppedTablesReturnNothing)
{
    const std::string path{ _db_path("s22dropped.sqlog") };
    LogDb db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    _write_set(db);
    _write_set_scopes(db);
    EXPECT_TRUE(db.remove_marker(1u));
    EXPECT_FALSE(db.remove_marker(0u));

    ASSERT_TRUE(db.drop_table("logs"));
    ASSERT_TRUE(db.drop_table("scopes"));
    ASSERT_TRUE(db.drop_table("instances"));
    if (db.table_exists("markers"))
    {
        ASSERT_TRUE(db.drop_table("markers"));
    }

    EXPECT_FALSE(db.remove_marker(1u));

    std::vector<areg::String> names{ areg::String("stale") };
    db.log_instance_names(names);
    EXPECT_TRUE(names.empty());
    names.push_back(areg::String("stale"));
    db.log_thread_names(names);
    EXPECT_TRUE(names.empty());

    std::vector<ITEM_ID> ids{ 1u };
    db.log_threads(ids);
    EXPECT_TRUE(ids.empty());

    std::vector<areg::ScopeEntry> scopes;
    db.log_inst_scopes(scopes, 31u);
    EXPECT_TRUE(scopes.empty());

    std::vector<areg::SharedBuffer> messages;
    db.log_messages(messages);
    EXPECT_TRUE(messages.empty());
    db.log_inst_messages(messages, 31u);
    EXPECT_TRUE(messages.empty());
    db.log_scope_messages(messages, 1u);
    EXPECT_TRUE(messages.empty());
    db.log_messages(messages, 31u, 1u);
    EXPECT_TRUE(messages.empty());
    EXPECT_EQ(db.max_log_id(), 0u);

    db.disconnect();
    EXPECT_FALSE(db.remove_marker(1u));
}
