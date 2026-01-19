/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteRow.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite row object to get results.
 ************************************************************************/

#include "aregextend/db/SqliteRow.hpp"
#include "aregextend/db/SqliteStatement.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

namespace
{
    inline sqlite3_stmt* _sqlite_stmt(void* stmtObject)
    {
        return reinterpret_cast<sqlite3_stmt*>(stmtObject);
    }
}

SqliteRow::SqliteRow(void)
    : mStatement(nullptr)
{
}

SqliteRow::SqliteRow(SqliteStatement& statement)
    : mStatement(statement.mStatement)
{
}

SqliteRow::SqliteRow(void* statement)
    : mStatement(statement)
{
}

SqliteRow::SqliteRow(const SqliteRow& src)
    : mStatement(src.mStatement)
{
}

SqliteRow::SqliteRow(SqliteRow&& src) noexcept
    : mStatement(src.mStatement)
{
    src.mStatement = nullptr; // Transfer ownership
}

SqliteRow& SqliteRow::operator = (const SqliteRow& src)
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

int SqliteRow::getInt(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_int(_sqlite_stmt(mStatement), column);
}

int64_t SqliteRow::getInt64(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_int64(_sqlite_stmt(mStatement), column);
}

double SqliteRow::getDouble(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_double(_sqlite_stmt(mStatement), column);
}

String SqliteRow::getText(int column) const
{
    return String(reinterpret_cast<const char*>(sqlite3_column_text(_sqlite_stmt(mStatement), column)));
}

bool SqliteRow::isNull(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_NULL);
}

bool SqliteRow::isColumnValid(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) != SQLITE_NULL);
}

bool SqliteRow::isString(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_TEXT);
}

bool SqliteRow::isInteger(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteRow::isInteger64(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteRow::isDouble(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_FLOAT);
}

int SqliteRow::getColumnCount(void) const
{
    ASSERT(isValid());
    return sqlite3_column_count(_sqlite_stmt(mStatement));
}

String SqliteRow::getColumnName(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    const char* columnName = sqlite3_column_name(_sqlite_stmt(mStatement), column);
    return String((columnName != nullptr) ? columnName : String::EmptyString);
}

int SqliteRow::getColumnIndex(const String& columnName) const
{
    ASSERT(isValid());
    ASSERT(!columnName.isEmpty());
    int columnCount = getColumnCount();
    for (int index = 0; index < columnCount; ++index)
    {
        if (getColumnName(index) == columnName)
        {
            return index;
        }
    }

    return NECommon::INVALID_INDEX; // Column not found
}
