/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/db/SqliteStatement.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite statement object.
 ************************************************************************/
#include "aregextend/db/SqliteStatement.hpp"
#include "aregextend/db/SqliteDatabase.hpp"

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)

namespace
{
    inline sqlite3* _sqlite(void* dbObject)
    {
        return reinterpret_cast<sqlite3*>(dbObject);
    }

    inline sqlite3** _sqlite(void** dbObject)
    {
        return reinterpret_cast<sqlite3**>(dbObject);
    }

    inline sqlite3_stmt* _sqlite_stmt(void* stmtObject)
    {
        return reinterpret_cast<sqlite3_stmt*>(stmtObject);
    }

    inline sqlite3_stmt** _sqlite_stmt(void** stmtObject)
    {
        return reinterpret_cast<sqlite3_stmt**>(stmtObject);
    }
}

SqliteStatement::SqliteStatement(SqliteDatabase& db, const String& sql)
    : mDatabase (db)
    , mStatement(nullptr)
    , mRowPos   (0)
{
    if (db.isOperable())
    {
        prepare(sql);
    }
}

SqliteStatement::SqliteStatement(SqliteDatabase& db)
    : mDatabase (db)
    , mStatement(nullptr)
    , mRowPos   (0)
{
}

SqliteStatement::~SqliteStatement(void)
{
    finalize();
}

bool SqliteStatement::prepare(const String& sql)
{
    bool result{ (mDatabase.isOperable() && !sql.isEmpty() && (SQLITE_OK == ::sqlite3_prepare_v2(_sqlite(mDatabase.mDbObject), sql.getString(), -1, _sqlite_stmt(&mStatement), nullptr))) };
    mRowPos = result ? 1 : 0; // Reset row position
    return result;
}

bool SqliteStatement::execute()
{
    return isValid() && (sqlite3_step(_sqlite_stmt(mStatement)) == SQLITE_DONE);
}

bool SqliteStatement::next()
{
    bool result{ isValid() && (sqlite3_step(_sqlite_stmt(mStatement)) == SQLITE_ROW) };
    mRowPos += result ? 1 : 0; // Increment row position if a new row is available
    return result;
}

void SqliteStatement::reset()
{
    mRowPos = 0; // Reset row position
    sqlite3_reset(_sqlite_stmt(mStatement));
}

void SqliteStatement::finalize()
{
    if (mStatement != nullptr)
    {
        sqlite3_finalize(_sqlite_stmt(mStatement));
        mStatement = nullptr;
        mRowPos = 0; // Reset row position
    }
}

bool SqliteStatement::bindInt(int index, int value)
{
    return isValid() && (sqlite3_bind_int(_sqlite_stmt(mStatement), index, value) == SQLITE_OK);
}

bool SqliteStatement::bindInt64(int index, int64_t value)
{
    return isValid() && (sqlite3_bind_int64(_sqlite_stmt(mStatement), index, value) == SQLITE_OK);
}

bool SqliteStatement::bindDouble(int index, double value)
{
    return isValid() && (sqlite3_bind_double(_sqlite_stmt(mStatement), index, value) == SQLITE_OK);
}

bool SqliteStatement::bindText(int index, const String& value)
{
    return isValid() && (sqlite3_bind_text(_sqlite_stmt(mStatement), index, value.getString(), -1, SQLITE_TRANSIENT) == SQLITE_OK);
}

bool SqliteStatement::bindNull(int index)
{
    return isValid() && (sqlite3_bind_null(_sqlite_stmt(mStatement), index) == SQLITE_OK);
}

void SqliteStatement::clearBindings(void)
{
    ASSERT(isValid());
    sqlite3_clear_bindings(_sqlite_stmt(mStatement));
}

int SqliteStatement::getInt(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_int(_sqlite_stmt(mStatement), column);
}

int64_t SqliteStatement::getInt64(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_int64(_sqlite_stmt(mStatement), column);
}

double SqliteStatement::getDouble(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return sqlite3_column_double(_sqlite_stmt(mStatement), column);
}

String SqliteStatement::getText(int column) const
{
    return String(reinterpret_cast<const char*>(sqlite3_column_text(_sqlite_stmt(mStatement), column)));
}

bool SqliteStatement::isNull(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_NULL);
}

bool SqliteStatement::isColumnValid(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) != SQLITE_NULL);
}

bool SqliteStatement::isString(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_TEXT);
}

bool SqliteStatement::isInteger(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteStatement::isInteger64(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_INTEGER);
}

bool SqliteStatement::isDouble(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    return (sqlite3_column_type(_sqlite_stmt(mStatement), column) == SQLITE_FLOAT);
}

int SqliteStatement::getColumnCount(void) const
{
    ASSERT(isValid());
    return sqlite3_column_count(_sqlite_stmt(mStatement));
}

String SqliteStatement::getColumnName(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    const char* columnName = sqlite3_column_name(_sqlite_stmt(mStatement), column);
    return String((columnName != nullptr) ? columnName : String::EmptyString);
}

int SqliteStatement::getColumnIndex(const String& columnName) const
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

SqliteStatement::eColumnType SqliteStatement::getColumnType(int column) const
{
    ASSERT(isValid());
    ASSERT(column >= 0);
    int type = sqlite3_column_type(_sqlite_stmt(mStatement), column);
    switch (type)
    {
        case SQLITE_INTEGER:
            return eColumnType::ColumnInteger;
        case SQLITE_FLOAT:
            return eColumnType::ColumnDouble;
        case SQLITE_TEXT:
            return eColumnType::ColumnText;
        case SQLITE_BLOB:
            return eColumnType::ColumnBlob;
        case SQLITE_NULL:
            return eColumnType::ColumnNull;
        default:
            return eColumnType::ColumnUnknown;
    }
}
