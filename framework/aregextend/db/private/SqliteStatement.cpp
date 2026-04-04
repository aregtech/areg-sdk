/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteStatement.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite statement object.
 ************************************************************************/
#include "aregextend/db/SqliteStatement.hpp"
#include "aregextend/db/SqliteDatabase.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

namespace {
    inline sqlite3* _sqlite(void* dbObject)
    {
        return reinterpret_cast<sqlite3*>(dbObject);
    }

    inline sqlite3_stmt* _sqlite_stmt(void* stmtObject)
    {
        return reinterpret_cast<sqlite3_stmt*>(stmtObject);
    }

    inline sqlite3_stmt** _sqlite_stmt(void** stmtObject)
    {
        return reinterpret_cast<sqlite3_stmt**>(stmtObject);
    }
} // namespace

namespace areg::ext {

SqliteStatement::SqliteStatement(SqliteDatabase& db, const String& sql)
    : mDatabase (db)
    , mStatement(nullptr)
    , mRowPos   (0)
{
    if (db.is_operable())
    {
        prepare(sql);
    }
}

SqliteStatement::SqliteStatement(SqliteDatabase& db) noexcept
    : mDatabase (db)
    , mStatement(nullptr)
    , mRowPos   (0)
{
}

SqliteStatement::~SqliteStatement() noexcept
{
    finalize();
}

bool SqliteStatement::prepare(const String& sql)
{
    mRowPos = 0; // Reset row position
    if ((mDatabase.is_operable() == false) || sql.is_empty())
        return false;

    return (SQLITE_OK == ::sqlite3_prepare_v2(_sqlite(mDatabase.mDbObject), sql.as_string(), -1, _sqlite_stmt(&mStatement), nullptr));
}

bool SqliteStatement::execute() noexcept
{
    return (is_valid() && (SQLITE_DONE == sqlite3_step(_sqlite_stmt(mStatement))));
}

SqliteStatement::QueryResult SqliteStatement::next() noexcept
{
    int32_t result = is_valid() ? sqlite3_step(_sqlite_stmt(mStatement)) : SQLITE_ERROR;
    if (result == SQLITE_DONE)
    {
        return SqliteStatement::QueryResult::HasNoMore;
    }
    else if (result == SQLITE_ROW)
    {
        mRowPos += 1;
        return SqliteStatement::QueryResult::HasMore;
    }
    else
    {
        return SqliteStatement::QueryResult::Failed;
    }
}

void SqliteStatement::reset() noexcept
{
    mRowPos = 0; // Reset row position
    sqlite3_reset(_sqlite_stmt(mStatement));
}

void SqliteStatement::finalize() noexcept
{
    if (mStatement != nullptr)
    {
        sqlite3_finalize(_sqlite_stmt(mStatement));
        mStatement = nullptr;
        mRowPos = 0; // Reset row position
    }
}

bool SqliteStatement::bind_int32(int32_t index, int32_t value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_int(_sqlite_stmt(mStatement), index + 1, value) == SQLITE_OK));
}

bool SqliteStatement::bind_uint32(int32_t index, uint32_t value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_int64(_sqlite_stmt(mStatement), index + 1, static_cast<sqlite_int64>(value)) == SQLITE_OK));
}

bool SqliteStatement::bind_int64(int32_t index, int64_t value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_int64(_sqlite_stmt(mStatement), index + 1, static_cast<sqlite_int64>(value)) == SQLITE_OK));
}

bool SqliteStatement::bind_uint64(int32_t index, uint64_t value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_int64(_sqlite_stmt(mStatement), index + 1, static_cast<sqlite_int64>(value)) == SQLITE_OK));
}

bool SqliteStatement::bind_double(int32_t index, double value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_double(_sqlite_stmt(mStatement), index + 1, value) == SQLITE_OK));
}

bool SqliteStatement::bind_float(int32_t index, float value) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_double(_sqlite_stmt(mStatement), index + 1, static_cast<double>(value)) == SQLITE_OK));
}

bool SqliteStatement::bind_text(int32_t index, const String& value) noexcept
{
    const char* txt = value.as_string();
    return (txt != nullptr ? (index >= 0) && is_valid() && (sqlite3_bind_text(_sqlite_stmt(mStatement), index + 1, txt, value.length(), SQLITE_TRANSIENT) == SQLITE_OK) : bind_null(index));
}

bool SqliteStatement::bind_null(int32_t index) noexcept
{
    return ((index >= 0) && is_valid() && (sqlite3_bind_null(_sqlite_stmt(mStatement), index + 1) == SQLITE_OK));
}

void SqliteStatement::clear_bindings() noexcept
{
    ASSERT(is_valid());
    sqlite3_clear_bindings(_sqlite_stmt(mStatement));
}

int32_t SqliteStatement::as_int32(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<int32_t>(sqlite3_column_int(_sqlite_stmt(mStatement), index));
}

uint32_t SqliteStatement::as_uint32(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<uint32_t>(sqlite3_column_int64(_sqlite_stmt(mStatement), index));
}

int64_t SqliteStatement::as_int64(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<int64_t>(sqlite3_column_int64(_sqlite_stmt(mStatement), index));
}

uint64_t SqliteStatement::as_uint64(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<uint64_t>(sqlite3_column_int64(_sqlite_stmt(mStatement), index));
}

double SqliteStatement::as_double(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<double>(sqlite3_column_double(_sqlite_stmt(mStatement), index));
}

float SqliteStatement::as_float(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return static_cast<float>(sqlite3_column_double(_sqlite_stmt(mStatement), index));
}

String SqliteStatement::as_text(int32_t index) const
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    const char* txt = reinterpret_cast<const char*>(sqlite3_column_text(_sqlite_stmt(mStatement), index));
    return String(txt != nullptr ? txt : String::EmptyString);
}

bool SqliteStatement::is_null(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) == SQLITE_NULL);
}

bool SqliteStatement::is_column_valid(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) != SQLITE_NULL);
}

bool SqliteStatement::is_string(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) == SQLITE_TEXT);
}

bool SqliteStatement::is_int(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) == SQLITE_INTEGER);
}

bool SqliteStatement::is_int64(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) == SQLITE_INTEGER);
}

bool SqliteStatement::is_double(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), index) == SQLITE_FLOAT);
}

int32_t SqliteStatement::column_count() const noexcept
{
    ASSERT(is_valid());
    return sqlite3_column_count(_sqlite_stmt(mStatement));
}

String SqliteStatement::column_name(int32_t index) const
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    const char* columnName = sqlite3_column_name(_sqlite_stmt(mStatement), index);
    return String((columnName != nullptr) ? columnName : String::EmptyString);
}

int32_t SqliteStatement::column_index(const String& columnName) const noexcept
{
    ASSERT(is_valid());
    ASSERT(!columnName.is_empty());
    int32_t columnCount = column_count();
    for (int index = 0; index < columnCount; ++index)
    {
        if (column_name(index) == columnName)
        {
            return index;
        }
    }

    return areg::INVALID_INDEX; // Column not found
}

SqliteStatement::ColumnType SqliteStatement::column_type(int32_t index) const noexcept
{
    ASSERT(is_valid());
    ASSERT(index >= 0);
    int32_t type = sqlite3_column_type(_sqlite_stmt(mStatement), index);
    switch (type)
    {
        case SQLITE_INTEGER:
            return ColumnType::Integer;
        case SQLITE_FLOAT:
            return ColumnType::Double;
        case SQLITE_TEXT:
            return ColumnType::Text;
        case SQLITE_BLOB:
            return ColumnType::Blob;
        case SQLITE_NULL:
            return ColumnType::Null;
        default:
            return ColumnType::Unknown;
    }
}

} // namespace areg::ext
