/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteRow.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite row object to get results.
 ************************************************************************/

#include "aregextend/db/SqliteRow.hpp"
#include "aregextend/db/SqliteStatement.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

namespace {
    inline sqlite3_stmt* _sqlite_stmt(void* stmtObject) noexcept
    {
        return reinterpret_cast<sqlite3_stmt*>(stmtObject);
    }
} // namespace

namespace areg::ext {

SqliteRow::SqliteRow() noexcept
    : mStatement(nullptr)
{
}

SqliteRow::SqliteRow(SqliteStatement& statement) noexcept
    : mStatement(statement.mStatement)
{
}

SqliteRow::SqliteRow(void* statement) noexcept
    : mStatement(statement)
{
}

SqliteRow::SqliteRow(const SqliteRow& src) noexcept
    : mStatement(src.mStatement)
{
}

SqliteRow::SqliteRow(SqliteRow&& src) noexcept
    : mStatement(src.mStatement)
{
    src.mStatement = nullptr; // Transfer ownership
}

SqliteRow& SqliteRow::operator = (const SqliteRow& src) noexcept
{
    mStatement = src.mStatement;
    return (*this);
}

SqliteRow& SqliteRow::operator = (SqliteRow&& src) noexcept
{
    mStatement = src.mStatement;
    src.mStatement = nullptr; // Transfer ownership
    return (*this);
}

int32_t SqliteRow::as_int(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return sqlite3_column_int(_sqlite_stmt(mStatement), column);
}

int64_t SqliteRow::as_int64(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return sqlite3_column_int64(_sqlite_stmt(mStatement), column);
}

double SqliteRow::as_double(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return sqlite3_column_double(_sqlite_stmt(mStatement), column);
}

String SqliteRow::as_text(int32_t column) const noexcept
{
    return String(reinterpret_cast<const char*>(sqlite3_column_text(_sqlite_stmt(mStatement), column)));
}

bool SqliteRow::is_null(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_NULL);
}

bool SqliteRow::is_column_valid(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) != SQLITE_NULL);
}

bool SqliteRow::is_string(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_TEXT);
}

bool SqliteRow::is_integer(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteRow::is_integer64(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteRow::is_double(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_FLOAT);
}

int32_t SqliteRow::column_count() const noexcept
{
    ASSERT(is_valid());
    return sqlite3_column_count(_sqlite_stmt(mStatement));
}

String SqliteRow::column_name(int32_t column) const noexcept
{
    ASSERT(is_valid());
    ASSERT(column >= 0);
    const char* columnName = sqlite3_column_name(_sqlite_stmt(mStatement), column);
    return String((columnName != nullptr) ? columnName : String::EmptyString);
}

int32_t SqliteRow::column_index(const String& columnName) const noexcept
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

} // namespace areg::ext
