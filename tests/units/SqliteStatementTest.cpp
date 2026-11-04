/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SqliteStatementTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the SQLite wrappers: database, statement and row.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "aregextend/db/SqliteDatabase.hpp"
#include "aregextend/db/SqliteRow.hpp"
#include "aregextend/db/SqliteStatement.hpp"
#include "areg/base/CommonDefs.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"

#include <cstdint>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <utility>

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
    using Db    = areg::ext::SqliteDatabase;
    using Stmt  = areg::ext::SqliteStatement;
    using Row   = areg::ext::SqliteRow;
    using Res   = areg::ext::SqliteStatement::QueryResult;
    using Col   = areg::ext::SqliteStatement::ColumnType;

    //! Returns the directory of this process for the database files of the tests.
    fs::path _test_dir()
    {
        return fs::path(testing::TempDir()) / ("areg-sqlite-" + std::to_string(areg::Process::instance().id()));
    }

    //! Returns the path of a database file in the directory of this process, with no file of an earlier run left.
    std::string _db_path(const char* name)
    {
        std::error_code err;
        fs::create_directories(_test_dir(), err);
        const std::string path{ (_test_dir() / name).string() };
        for (const char* suffix : { "", "-journal", "-wal", "-shm" })
        {
            fs::remove(path + suffix, err);
        }

        return path;
    }

    std::string _text(const areg::String& value)
    {
        return std::string(value.as_string());
    }

    //! Returns the first column of the first row of the query as an integer, or -1 if there is no row.
    int64_t _scalar(Db& db, const char* sql)
    {
        Stmt stmt(db, areg::String(sql));
        return (stmt.next() == Res::HasMore ? stmt.as_int64(0) : -1);
    }

    //! A database file that is open for the test and removed after it.
    class SqliteWrapperTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            mPath = _db_path("fixture.sqlite");
            ASSERT_TRUE(mDb.connect(areg::String(mPath.c_str()), areg::DatabaseRole::Recorder));
        }

        void TearDown() override
        {
            mDb.disconnect();
            std::error_code err;
            fs::remove_all(_test_dir(), err);
        }

        //! Creates the table of all column kinds and fills one row.
        void make_values()
        {
            ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE vals (i INTEGER, big INTEGER, d REAL, s TEXT, n TEXT, b BLOB);")));
            ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO vals VALUES (-7, 5000000000, 2.5, 'hello', NULL, x'0102');")));
        }

        Db          mDb;
        std::string mPath;
    };
}

//////////////////////////////////////////////////////////////////////////
// SqliteDatabase
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A database object that is not connected is not operable, keeps an empty path and refuses every command.
 **/
TEST(SqliteDatabaseTest, test_notConnectedRefusesEverything)
{
    Db db;
    EXPECT_FALSE(db.is_operable());
    EXPECT_TRUE(db.path().is_empty());
    EXPECT_EQ(db.last_inserted_id(), 0u);
    EXPECT_FALSE(db.execute(areg::String("SELECT 1;")));
    EXPECT_FALSE(db.begin());
    EXPECT_FALSE(db.commit(true));
    EXPECT_FALSE(db.commit(false));
    EXPECT_FALSE(db.rollback());
    db.disconnect();
    EXPECT_FALSE(db.is_operable());
}

/**
 * \brief   Connecting without a path and without an earlier path fails and leaves the object closed.
 **/
TEST(SqliteDatabaseTest, test_connectWithEmptyPathFails)
{
    Db db;
    EXPECT_FALSE(db.connect(areg::String(), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
}

/**
 * \brief   Connecting a new file creates the file and the folders of its path.
 **/
TEST(SqliteDatabaseTest, test_connectNewFileCreatesFoldersAndFile)
{
    const std::string path{ (fs::path(_db_path("probe.sqlite")).parent_path() / "a" / "b" / "new.sqlite").string() };
    {
        Db db;
        ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
        EXPECT_TRUE(db.is_operable());
        EXPECT_FALSE(db.path().is_empty());
        EXPECT_TRUE(db.execute(areg::String("CREATE TABLE t (a INTEGER);")));
    }

    std::error_code err;
    EXPECT_TRUE(fs::exists(path, err));
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   An existing file is opened with the data it holds, also after a close and a second connect.
 **/
TEST(SqliteDatabaseTest, test_reopenExistingFileKeepsData)
{
    const std::string path{ _db_path("reopen.sqlite") };
    Db db;
    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Recorder));
    ASSERT_TRUE(db.execute(areg::String("CREATE TABLE t (a INTEGER);")));
    ASSERT_TRUE(db.execute(areg::String("INSERT INTO t VALUES (41), (42);")));
    db.disconnect();
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.execute(areg::String("INSERT INTO t VALUES (43);")));

    ASSERT_TRUE(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
    EXPECT_EQ(_scalar(db, "SELECT COUNT(*) FROM t;"), 2);
    EXPECT_EQ(_scalar(db, "SELECT MAX(a) FROM t;"), 42);

    std::error_code err;
    db.disconnect();
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   A path that cannot be a file, because a part of it is a file, is refused and the object stays closed.
 **/
TEST(SqliteDatabaseTest, test_connectInvalidPathFails)
{
    const std::string blocker{ _db_path("blocker.txt") };
    {
        std::ofstream out(blocker, std::ios::binary);
        out << "not a folder";
    }

    Db db;
    const std::string invalid{ (fs::path(blocker) / "sub" / "db.sqlite").string() };
    EXPECT_FALSE(db.connect(areg::String(invalid.c_str()), areg::DatabaseRole::Recorder));
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.execute(areg::String("SELECT 1;")));

    std::error_code err;
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   A file that is not a database opens, but the first command on it fails, and nothing is written over it.
 **/
TEST(SqliteDatabaseTest, test_corruptedFileRefusesCommands)
{
    const std::string path{ _db_path("corrupt.sqlite") };
    const std::string garbage(4096, 'x');
    {
        std::ofstream out(path, std::ios::binary);
        out << garbage;
    }

    Db db;
    if (db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer))
    {
        EXPECT_FALSE(db.execute(areg::String("SELECT COUNT(*) FROM sqlite_master;")));
        Stmt stmt(db, areg::String("SELECT COUNT(*) FROM sqlite_master;"));
        EXPECT_NE(stmt.next(), Res::HasMore);
    }

    db.disconnect();
    std::string content;
    {
        std::ifstream in(path, std::ios::binary);
        content.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }

    EXPECT_EQ(content, garbage);
    std::error_code err;
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   The constructor with open set to false keeps the path and does not open; connect without a path then opens it.
 **/
TEST(SqliteDatabaseTest, test_constructorWithoutOpenKeepsPath)
{
    const std::string path{ _db_path("lazy.sqlite") };
    Db db(areg::String(path.c_str()), false);
    EXPECT_FALSE(db.is_operable());
    EXPECT_FALSE(db.path().is_empty());
    EXPECT_EQ(_text(db.path()), _text(areg::File::normalize_path(areg::String(path.c_str()))));

    ASSERT_TRUE(db.connect(areg::String(), areg::DatabaseRole::Recorder));
    EXPECT_TRUE(db.is_operable());
    EXPECT_EQ(_text(db.path()), _text(areg::File::normalize_path(areg::String(path.c_str()))));

    std::error_code err;
    db.disconnect();
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   The constructor with an empty path and open set to false keeps an empty path.
 **/
TEST(SqliteDatabaseTest, test_constructorEmptyPathStaysEmpty)
{
    Db closed(areg::String(), false);
    EXPECT_TRUE(closed.path().is_empty());
    EXPECT_FALSE(closed.is_operable());

    Db opened(areg::String(), true);
    EXPECT_FALSE(opened.is_operable());
}

/**
 * \brief   The constructor with open set to true opens the file.
 **/
TEST(SqliteDatabaseTest, test_constructorWithOpenOpensFile)
{
    const std::string path{ _db_path("eager.sqlite") };
    Db db(areg::String(path.c_str()), true);
    EXPECT_TRUE(db.is_operable());
    EXPECT_TRUE(db.execute(areg::String("CREATE TABLE t (a INTEGER);")));

    std::error_code err;
    db.disconnect();
    fs::remove_all(_test_dir(), err);
}

/**
 * \brief   Invalid SQL is reported as a failure and does not close the database.
 **/
TEST_F(SqliteWrapperTest, test_executeInvalidSqlFails)
{
    EXPECT_FALSE(mDb.execute(areg::String("THIS IS NOT SQL;")));
    EXPECT_FALSE(mDb.execute(areg::String("SELECT * FROM missing_table;")));
    EXPECT_TRUE(mDb.is_operable());
    EXPECT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER);")));
}

/**
 * \brief   The id of the last insert is reported and is zero before any insert.
 **/
TEST_F(SqliteWrapperTest, test_lastInsertedId)
{
    EXPECT_EQ(mDb.last_inserted_id(), 0u);
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (id INTEGER PRIMARY KEY, a INTEGER);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t (a) VALUES (1);")));
    EXPECT_EQ(mDb.last_inserted_id(), 1u);
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t (a) VALUES (2);")));
    EXPECT_EQ(mDb.last_inserted_id(), 2u);
}

/**
 * \brief   Commit makes the rows of a transaction stay, rollback and commit(false) remove them.
 **/
TEST_F(SqliteWrapperTest, test_transactionCommitAndRollback)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER);")));

    ASSERT_TRUE(mDb.begin());
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (1);")));
    ASSERT_TRUE(mDb.commit(true));
    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t;"), 1);

    ASSERT_TRUE(mDb.begin());
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (2);")));
    ASSERT_TRUE(mDb.rollback());
    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t;"), 1);

    ASSERT_TRUE(mDb.begin());
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (3);")));
    ASSERT_TRUE(mDb.commit(false));
    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t;"), 1);
}

/**
 * \brief   A transaction cannot be opened inside a transaction, and a commit without a transaction fails.
 **/
TEST_F(SqliteWrapperTest, test_transactionNestingAndStrayCommitFail)
{
    EXPECT_FALSE(mDb.commit(true));
    EXPECT_FALSE(mDb.rollback());
    ASSERT_TRUE(mDb.begin());
    EXPECT_FALSE(mDb.begin());
    EXPECT_TRUE(mDb.commit(true));
}

//////////////////////////////////////////////////////////////////////////
// SqliteStatement: preparation and execution
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A statement on a database that is not open is not valid and cannot be prepared.
 **/
TEST(SqliteStatementTest, test_statementOnClosedDatabaseIsInvalid)
{
    Db db;
    Stmt withSql(db, areg::String("SELECT 1;"));
    EXPECT_FALSE(withSql.is_valid());

    Stmt plain(db);
    EXPECT_FALSE(plain.is_valid());
    EXPECT_FALSE(plain.prepare(areg::String("SELECT 1;")));
    EXPECT_FALSE(plain.execute());
    EXPECT_EQ(plain.next(), Res::Failed);
    EXPECT_EQ(plain.row_pos(), 0u);
}

/**
 * \brief   Empty SQL and invalid SQL are refused and leave the statement not valid.
 **/
TEST_F(SqliteWrapperTest, test_prepareInvalidSqlFails)
{
    Stmt stmt(mDb);
    EXPECT_FALSE(stmt.prepare(areg::String()));
    EXPECT_FALSE(stmt.is_valid());
    EXPECT_FALSE(stmt.prepare(areg::String("SELEKT 1;")));
    EXPECT_FALSE(stmt.is_valid());
    EXPECT_FALSE(stmt.prepare(areg::String("SELECT * FROM missing_table;")));
    EXPECT_FALSE(stmt.is_valid());
    EXPECT_TRUE(stmt.prepare(areg::String("SELECT 1;")));
    EXPECT_TRUE(stmt.is_valid());
}

/**
 * \brief   Preparing again replaces the previous statement and a failed prepare drops it.
 **/
TEST_F(SqliteWrapperTest, test_prepareAgainReplacesStatement)
{
    Stmt stmt(mDb, areg::String("SELECT 11;"));
    ASSERT_TRUE(stmt.is_valid());
    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(stmt.as_int32(0), 11);

    ASSERT_TRUE(stmt.prepare(areg::String("SELECT 22;")));
    EXPECT_EQ(stmt.row_pos(), 0u);
    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(stmt.as_int32(0), 22);

    EXPECT_FALSE(stmt.prepare(areg::String("SELEKT 1;")));
    EXPECT_FALSE(stmt.is_valid());
}

/**
 * \brief   A command that returns no row ends with execute() true, a query that returns a row does not.
 **/
TEST_F(SqliteWrapperTest, test_executeCommandAndQuery)
{
    Stmt create(mDb, areg::String("CREATE TABLE t (a INTEGER);"));
    EXPECT_TRUE(create.execute());

    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (5);"));
    EXPECT_TRUE(insert.execute());

    Stmt select(mDb, areg::String("SELECT a FROM t;"));
    EXPECT_FALSE(select.execute());
}

/**
 * \brief   A step that violates a constraint is reported as Failed by next() and as false by execute().
 **/
TEST_F(SqliteWrapperTest, test_stepConstraintViolationFails)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE u (k INTEGER PRIMARY KEY);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO u VALUES (1);")));

    Stmt viaNext(mDb, areg::String("INSERT INTO u VALUES (1);"));
    EXPECT_EQ(viaNext.next(), Res::Failed);

    Stmt viaExecute(mDb, areg::String("INSERT INTO u VALUES (1);"));
    EXPECT_FALSE(viaExecute.execute());
    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM u;"), 1);
}

/**
 * \brief   The rows are read one by one, the row position counts them, and the end of the rows is reported once reached.
 **/
TEST_F(SqliteWrapperTest, test_nextCountsRowsUntilNoMore)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (10), (20), (30);")));

    Stmt stmt(mDb, areg::String("SELECT a FROM t ORDER BY a;"));
    ASSERT_TRUE(stmt.is_valid());
    EXPECT_EQ(stmt.row_pos(), 0u);
    for (uint32_t i = 1; i <= 3u; ++i)
    {
        ASSERT_EQ(stmt.next(), Res::HasMore);
        EXPECT_EQ(stmt.row_pos(), i);
        EXPECT_EQ(stmt.as_int32(0), static_cast<int32_t>(i * 10u));
    }

    EXPECT_EQ(stmt.next(), Res::HasNoMore);
    EXPECT_EQ(stmt.row_pos(), 3u);
}

/**
 * \brief   Reset rewinds the statement to the first row and the row position to zero; the bindings stay.
 **/
TEST_F(SqliteWrapperTest, test_resetRewindsAndKeepsBindings)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (1), (2), (3), (4);")));

    Stmt stmt(mDb, areg::String("SELECT a FROM t WHERE a > ? ORDER BY a;"));
    ASSERT_TRUE(stmt.bind_int32(0, 1));
    ASSERT_EQ(stmt.next(), Res::HasMore);
    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(stmt.row_pos(), 2u);

    stmt.reset();
    EXPECT_EQ(stmt.row_pos(), 0u);
    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(stmt.as_int32(0), 2);
}

/**
 * \brief   Reset and finalize of a statement that is not valid are harmless, and finalize can be repeated.
 **/
TEST_F(SqliteWrapperTest, test_resetAndFinalizeOfInvalidStatement)
{
    Stmt stmt(mDb);
    stmt.reset();
    stmt.finalize();
    EXPECT_FALSE(stmt.is_valid());

    ASSERT_TRUE(stmt.prepare(areg::String("SELECT 1;")));
    ASSERT_EQ(stmt.next(), Res::HasMore);
    stmt.finalize();
    EXPECT_FALSE(stmt.is_valid());
    EXPECT_EQ(stmt.row_pos(), 0u);
    stmt.finalize();
    EXPECT_FALSE(stmt.is_valid());
    EXPECT_EQ(stmt.next(), Res::Failed);
}

//////////////////////////////////////////////////////////////////////////
// SqliteStatement: binding and reading values
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Every type bound to a statement is read back unchanged, with its column type.
 **/
TEST_F(SqliteWrapperTest, test_bindEveryTypeAndReadBack)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE vals (c0, c1, c2, c3, c4, c5, c6, c7, c8);")));
    Stmt insert(mDb, areg::String("INSERT INTO vals VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"));
    ASSERT_TRUE(insert.is_valid());
    EXPECT_TRUE(insert.bind_int32 (0, std::numeric_limits<int32_t>::min()));
    EXPECT_TRUE(insert.bind_uint32(1, std::numeric_limits<uint32_t>::max()));
    EXPECT_TRUE(insert.bind_int64 (2, std::numeric_limits<int64_t>::min()));
    EXPECT_TRUE(insert.bind_uint64(3, 0x0123456789ABCDEFull));
    EXPECT_TRUE(insert.bind_double(4, 3.25));
    EXPECT_TRUE(insert.bind_float (5, 1.5f));
    EXPECT_TRUE(insert.bind_text  (6, areg::String("dynamic text")));
    EXPECT_TRUE(insert.bind_text_static(7, "static text"));
    EXPECT_TRUE(insert.bind_null  (8));
    EXPECT_TRUE(insert.execute());

    Stmt select(mDb, areg::String("SELECT * FROM vals;"));
    ASSERT_EQ(select.next(), Res::HasMore);
    EXPECT_EQ(select.as_int32(0), std::numeric_limits<int32_t>::min());
    EXPECT_EQ(select.as_uint32(1), std::numeric_limits<uint32_t>::max());
    EXPECT_EQ(select.as_int64(2), std::numeric_limits<int64_t>::min());
    EXPECT_EQ(select.as_uint64(3), 0x0123456789ABCDEFull);
    EXPECT_DOUBLE_EQ(select.as_double(4), 3.25);
    EXPECT_FLOAT_EQ(select.as_float(5), 1.5f);
    EXPECT_EQ(_text(select.as_text(6)), "dynamic text");
    EXPECT_EQ(_text(select.as_text(7)), "static text");
    EXPECT_TRUE(select.is_null(8));
    EXPECT_EQ(_text(select.as_text(8)), "");

    EXPECT_EQ(select.column_type(0), Col::Integer);
    EXPECT_EQ(select.column_type(2), Col::Integer);
    EXPECT_EQ(select.column_type(4), Col::Double);
    EXPECT_EQ(select.column_type(5), Col::Double);
    EXPECT_EQ(select.column_type(6), Col::Text);
    EXPECT_EQ(select.column_type(7), Col::Text);
    EXPECT_EQ(select.column_type(8), Col::Null);
}

/**
 * \brief   A 64-bit unsigned value with the high bit set keeps its bits through the database.
 **/
TEST_F(SqliteWrapperTest, test_unsigned64KeepsHighBit)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a);")));
    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (?);"));
    ASSERT_TRUE(insert.bind_uint64(0, std::numeric_limits<uint64_t>::max()));
    ASSERT_TRUE(insert.execute());

    Stmt select(mDb, areg::String("SELECT a FROM t;"));
    ASSERT_EQ(select.next(), Res::HasMore);
    EXPECT_EQ(select.as_uint64(0), std::numeric_limits<uint64_t>::max());
    EXPECT_EQ(select.as_int64(0), -1);
}

/**
 * \brief   A binding with a negative index, an index past the last parameter, or on a statement that is not valid is refused.
 **/
TEST_F(SqliteWrapperTest, test_bindInvalidIndexOrStatementFails)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a, b);")));
    Stmt stmt(mDb, areg::String("INSERT INTO t VALUES (?, ?);"));
    ASSERT_TRUE(stmt.is_valid());

    EXPECT_FALSE(stmt.bind_int32 (-1, 1));
    EXPECT_FALSE(stmt.bind_uint32(-1, 1u));
    EXPECT_FALSE(stmt.bind_int64 (-1, 1));
    EXPECT_FALSE(stmt.bind_uint64(-1, 1u));
    EXPECT_FALSE(stmt.bind_double(-1, 1.0));
    EXPECT_FALSE(stmt.bind_float (-1, 1.0f));
    EXPECT_FALSE(stmt.bind_text  (-1, areg::String("x")));
    EXPECT_FALSE(stmt.bind_text_static(-1, "x"));
    EXPECT_FALSE(stmt.bind_null  (-1));

    EXPECT_FALSE(stmt.bind_int32 (2, 1));
    EXPECT_FALSE(stmt.bind_uint32(2, 1u));
    EXPECT_FALSE(stmt.bind_int64 (2, 1));
    EXPECT_FALSE(stmt.bind_uint64(2, 1u));
    EXPECT_FALSE(stmt.bind_double(2, 1.0));
    EXPECT_FALSE(stmt.bind_float (2, 1.0f));
    EXPECT_FALSE(stmt.bind_text  (2, areg::String("x")));
    EXPECT_FALSE(stmt.bind_text_static(2, "x"));
    EXPECT_FALSE(stmt.bind_null  (2));

    Stmt closed(mDb);
    EXPECT_FALSE(closed.bind_int32 (0, 1));
    EXPECT_FALSE(closed.bind_uint32(0, 1u));
    EXPECT_FALSE(closed.bind_int64 (0, 1));
    EXPECT_FALSE(closed.bind_uint64(0, 1u));
    EXPECT_FALSE(closed.bind_double(0, 1.0));
    EXPECT_FALSE(closed.bind_float (0, 1.0f));
    EXPECT_FALSE(closed.bind_text  (0, areg::String("x")));
    EXPECT_FALSE(closed.bind_text_static(0, "x"));
    EXPECT_FALSE(closed.bind_null  (0));
}

/**
 * \brief   A null text pointer is bound as a null value.
 **/
TEST_F(SqliteWrapperTest, test_bindNullTextPointerBindsNull)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a TEXT);")));
    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (?);"));
    ASSERT_TRUE(insert.bind_text_static(0, nullptr));
    ASSERT_TRUE(insert.execute());

    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t WHERE a IS NULL;"), 1);
}

/**
 * \brief   A text with quotes and SQL commands is stored as data and does not change the statement.
 **/
TEST_F(SqliteWrapperTest, test_boundTextIsNeverExecuted)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a TEXT);")));
    const areg::String hostile("'); DROP TABLE t; --");
    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (?);"));
    ASSERT_TRUE(insert.bind_text(0, hostile));
    ASSERT_TRUE(insert.execute());

    Stmt select(mDb, areg::String("SELECT a FROM t;"));
    ASSERT_EQ(select.next(), Res::HasMore);
    EXPECT_EQ(_text(select.as_text(0)), "'); DROP TABLE t; --");
}

/**
 * \brief   Text with non-ASCII characters keeps every byte.
 **/
TEST_F(SqliteWrapperTest, test_utf8TextKeepsBytes)
{
    const std::string utf8{ "caf\xC3\xA9 \xE2\x82\xAC" };
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a TEXT);")));
    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (?);"));
    ASSERT_TRUE(insert.bind_text(0, areg::String(utf8)));
    ASSERT_TRUE(insert.execute());

    Stmt select(mDb, areg::String("SELECT a FROM t;"));
    ASSERT_EQ(select.next(), Res::HasMore);
    EXPECT_EQ(_text(select.as_text(0)), utf8);
}

/**
 * \brief   Cleared bindings are null again on the next execution of the statement.
 **/
TEST_F(SqliteWrapperTest, test_clearBindingsMakesNextRowNull)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER, s TEXT);")));
    Stmt insert(mDb, areg::String("INSERT INTO t VALUES (?, ?);"));
    ASSERT_TRUE(insert.bind_int32(0, 9));
    ASSERT_TRUE(insert.bind_text(1, areg::String("kept")));
    ASSERT_TRUE(insert.execute());

    insert.reset();
    insert.clear_bindings();
    ASSERT_TRUE(insert.execute());

    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t WHERE a IS NULL AND s IS NULL;"), 1);
    EXPECT_EQ(_scalar(mDb, "SELECT COUNT(*) FROM t WHERE a = 9 AND s = 'kept';"), 1);
}

/**
 * \brief   The column kind checks and the column description of a row agree with the stored values.
 **/
TEST_F(SqliteWrapperTest, test_columnKindsAndDescription)
{
    make_values();
    Stmt stmt(mDb, areg::String("SELECT i, big, d, s, n, b FROM vals;"));
    ASSERT_EQ(stmt.next(), Res::HasMore);

    EXPECT_EQ(stmt.column_count(), 6);
    EXPECT_EQ(_text(stmt.column_name(0)), "i");
    EXPECT_EQ(_text(stmt.column_name(5)), "b");
    EXPECT_EQ(_text(stmt.column_name(6)), "");
    EXPECT_EQ(stmt.column_index(areg::String("s")), 3);
    EXPECT_EQ(stmt.column_index(areg::String("missing")), areg::INVALID_INDEX);

    EXPECT_TRUE(stmt.is_int(0));
    EXPECT_TRUE(stmt.is_int64(1));
    EXPECT_FALSE(stmt.is_int(2));
    EXPECT_TRUE(stmt.is_double(2));
    EXPECT_FALSE(stmt.is_double(0));
    EXPECT_TRUE(stmt.is_string(3));
    EXPECT_FALSE(stmt.is_string(0));
    EXPECT_TRUE(stmt.is_null(4));
    EXPECT_FALSE(stmt.is_null(3));
    EXPECT_TRUE(stmt.is_column_valid(3));
    EXPECT_FALSE(stmt.is_column_valid(4));

    EXPECT_EQ(stmt.column_type(0), Col::Integer);
    EXPECT_EQ(stmt.column_type(1), Col::Integer);
    EXPECT_EQ(stmt.column_type(2), Col::Double);
    EXPECT_EQ(stmt.column_type(3), Col::Text);
    EXPECT_EQ(stmt.column_type(4), Col::Null);
    EXPECT_EQ(stmt.column_type(5), Col::Blob);

    EXPECT_EQ(stmt.as_int32(0), -7);
    EXPECT_EQ(stmt.as_int64(1), 5000000000ll);
    EXPECT_DOUBLE_EQ(stmt.as_double(2), 2.5);
}

/**
 * \brief   A column index past the last column reads as null, zero and an empty text and does not fail.
 **/
TEST_F(SqliteWrapperTest, test_columnIndexOutOfRangeReadsEmpty)
{
    make_values();
    Stmt stmt(mDb, areg::String("SELECT i, s FROM vals;"));
    ASSERT_EQ(stmt.next(), Res::HasMore);

    EXPECT_TRUE(stmt.is_null(2));
    EXPECT_TRUE(stmt.is_null(100));
    EXPECT_FALSE(stmt.is_column_valid(100));
    EXPECT_EQ(stmt.column_type(100), Col::Null);
    EXPECT_EQ(stmt.as_int32(100), 0);
    EXPECT_EQ(stmt.as_uint32(100), 0u);
    EXPECT_EQ(stmt.as_int64(100), 0);
    EXPECT_EQ(stmt.as_uint64(100), 0u);
    EXPECT_DOUBLE_EQ(stmt.as_double(100), 0.0);
    EXPECT_FLOAT_EQ(stmt.as_float(100), 0.0f);
    EXPECT_EQ(_text(stmt.as_text(100)), "");
    EXPECT_FALSE(stmt.is_string(100));
    EXPECT_FALSE(stmt.is_int(100));
    EXPECT_FALSE(stmt.is_int64(100));
    EXPECT_FALSE(stmt.is_double(100));
}

//////////////////////////////////////////////////////////////////////////
// SqliteRow
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A default row is not valid; a row made of a statement is valid.
 **/
TEST_F(SqliteWrapperTest, test_rowValidity)
{
    Row empty;
    EXPECT_FALSE(empty.is_valid());

    Stmt stmt(mDb, areg::String("SELECT 1;"));
    Row fromStatement(stmt);
    EXPECT_TRUE(fromStatement.is_valid());

    Stmt closed(mDb);
    Row fromClosed(closed);
    EXPECT_FALSE(fromClosed.is_valid());

    Row fromPointer(static_cast<void*>(nullptr));
    EXPECT_FALSE(fromPointer.is_valid());
}

/**
 * \brief   Reproduces the defect: row() and next_row() were declared and never defined, so a caller did not link.
 **/
TEST_F(SqliteWrapperTest, test_statementGivesItsRows)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE nums (v INTEGER);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO nums VALUES (1), (2);")));
    Stmt stmt(mDb, areg::String("SELECT v FROM nums ORDER BY v;"));
    EXPECT_FALSE(stmt.row().is_valid());

    Row first{ stmt.next_row() };
    ASSERT_TRUE(first.is_valid());
    EXPECT_EQ(first.as_int(0), 1);
    EXPECT_EQ(stmt.row_pos(), 1u);
    ASSERT_TRUE(stmt.row().is_valid());
    EXPECT_EQ(stmt.row().as_int(0), 1);

    Row second{ stmt.next_row() };
    ASSERT_TRUE(second.is_valid());
    EXPECT_EQ(second.as_int(0), 2);
    EXPECT_EQ(first.as_int(0), 2);

    EXPECT_FALSE(stmt.next_row().is_valid());
    EXPECT_FALSE(stmt.row().is_valid());
    EXPECT_EQ(stmt.row_pos(), 2u);
}

/**
 * \brief   A statement that is not prepared gives rows that are not valid.
 **/
TEST_F(SqliteWrapperTest, test_statementWithoutQueryGivesNoRow)
{
    Stmt stmt(mDb);
    EXPECT_FALSE(stmt.row().is_valid());
    EXPECT_FALSE(stmt.next_row().is_valid());
}

/**
 * \brief   A row reads the values of the current row of its statement, with the type checks and the column names.
 **/
TEST_F(SqliteWrapperTest, test_rowReadsCurrentRow)
{
    make_values();
    Stmt stmt(mDb, areg::String("SELECT i, big, d, s, n FROM vals;"));
    Row row(stmt);
    ASSERT_EQ(stmt.next(), Res::HasMore);

    EXPECT_EQ(row.column_count(), 5);
    EXPECT_EQ(row.as_int(0), -7);
    EXPECT_EQ(row.as_int64(1), 5000000000ll);
    EXPECT_DOUBLE_EQ(row.as_double(2), 2.5);
    EXPECT_EQ(_text(row.as_text(3)), "hello");
    EXPECT_EQ(_text(row.as_text(4)), "");

    EXPECT_TRUE(row.is_integer(0));
    EXPECT_TRUE(row.is_integer64(1));
    EXPECT_TRUE(row.is_double(2));
    EXPECT_FALSE(row.is_double(0));
    EXPECT_TRUE(row.is_string(3));
    EXPECT_FALSE(row.is_string(0));
    EXPECT_TRUE(row.is_null(4));
    EXPECT_FALSE(row.is_null(3));
    EXPECT_TRUE(row.is_column_valid(3));
    EXPECT_FALSE(row.is_column_valid(4));

    EXPECT_EQ(_text(row.column_name(0)), "i");
    EXPECT_EQ(_text(row.column_name(4)), "n");
    EXPECT_EQ(_text(row.column_name(5)), "");
    EXPECT_EQ(row.column_index(areg::String("d")), 2);
    EXPECT_EQ(row.column_index(areg::String("absent")), areg::INVALID_INDEX);
}

/**
 * \brief   A row follows the statement to the next row, since it only points at the statement.
 **/
TEST_F(SqliteWrapperTest, test_rowFollowsStatement)
{
    ASSERT_TRUE(mDb.execute(areg::String("CREATE TABLE t (a INTEGER);")));
    ASSERT_TRUE(mDb.execute(areg::String("INSERT INTO t VALUES (1), (2);")));
    Stmt stmt(mDb, areg::String("SELECT a FROM t ORDER BY a;"));
    Row row(stmt);

    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(row.as_int(0), 1);
    ASSERT_EQ(stmt.next(), Res::HasMore);
    EXPECT_EQ(row.as_int(0), 2);
}

/**
 * \brief   A column index past the last column of a row reads as null, zero and an empty text.
 **/
TEST_F(SqliteWrapperTest, test_rowColumnOutOfRangeReadsEmpty)
{
    make_values();
    Stmt stmt(mDb, areg::String("SELECT i FROM vals;"));
    Row row(stmt);
    ASSERT_EQ(stmt.next(), Res::HasMore);

    EXPECT_TRUE(row.is_null(50));
    EXPECT_FALSE(row.is_column_valid(50));
    EXPECT_FALSE(row.is_string(50));
    EXPECT_FALSE(row.is_integer(50));
    EXPECT_FALSE(row.is_integer64(50));
    EXPECT_FALSE(row.is_double(50));
    EXPECT_EQ(row.as_int(50), 0);
    EXPECT_EQ(row.as_int64(50), 0);
    EXPECT_DOUBLE_EQ(row.as_double(50), 0.0);
    EXPECT_EQ(_text(row.as_text(50)), "");
    EXPECT_EQ(_text(row.column_name(50)), "");
}

/**
 * \brief   The text of an invalid row is empty and does not fail.
 **/
TEST(SqliteRowTest, test_invalidRowTextIsEmpty)
{
    Row row;
    EXPECT_EQ(_text(row.as_text(0)), "");
}

/**
 * \brief   A copy of a row points at the same statement; a move leaves the source invalid.
 **/
TEST_F(SqliteWrapperTest, test_rowCopyAndMove)
{
    make_values();
    Stmt stmt(mDb, areg::String("SELECT i FROM vals;"));
    Row source(stmt);
    ASSERT_EQ(stmt.next(), Res::HasMore);

    Row copy(source);
    EXPECT_TRUE(copy.is_valid());
    EXPECT_TRUE(source.is_valid());
    EXPECT_EQ(copy.as_int(0), -7);

    Row assigned;
    assigned = source;
    EXPECT_TRUE(assigned.is_valid());
    EXPECT_EQ(assigned.as_int(0), -7);

    Row moved(std::move(copy));
    EXPECT_TRUE(moved.is_valid());
    EXPECT_FALSE(copy.is_valid());
    EXPECT_EQ(moved.as_int(0), -7);

    Row moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_TRUE(moveAssigned.is_valid());
    EXPECT_FALSE(assigned.is_valid());
    EXPECT_EQ(moveAssigned.as_int(0), -7);
}
