/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/db/private/SqliteDatabase.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "extend/db/SqliteDatabase.hpp"

#include "areg/base/File.hpp"

#include <string_view>

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include <sqlite3.h>
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #include "sqlite3/amalgamation/sqlite3.h"
#endif  // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)



SqliteDatabase::SqliteDatabase(void)
    : IEDatabaseEngine  ( )
    , mDbPath           ( )
    , mDbObject         ( nullptr )
{
}

SqliteDatabase::SqliteDatabase(const String& dbPath, bool open)
    : IEDatabaseEngine  ( )
    , mDbPath           (dbPath.isEmpty() ? String::getEmptyString() : File::normalizePath(dbPath))
    , mDbObject         ( nullptr )
{
    if (open && (mDbPath.isEmpty() == false))
    {
        if (::sqlite3_open(mDbPath.getString(), reinterpret_cast<sqlite3 **>(&mDbObject)) != SQLITE_OK)
        {
            _close();
        }
    }
}

SqliteDatabase::~SqliteDatabase(void)
{
    _close();
}

inline bool SqliteDatabase::_open(const String& dbPath)
{
    bool result{ true };
    _close();
    mDbPath = dbPath.isEmpty() == false ? File::normalizePath(dbPath) : mDbPath;
    if (SQLITE_OK != ::sqlite3_open(mDbPath.getString(), reinterpret_cast<sqlite3 **>(&mDbObject)))
    {
        _close();
        result = false;
    }

    return result;
}

inline void SqliteDatabase::_close(void)
{
    if (mDbObject != nullptr)
    {
        ::sqlite3_close(reinterpret_cast<sqlite3 *>(mDbObject));
        mDbObject = nullptr;
    }
}

bool SqliteDatabase::isOperable(void) const
{
    return (mDbObject != nullptr);
}

bool SqliteDatabase::connect(const String& dbPath)
{
    return _open(dbPath);
}

void SqliteDatabase::disconnect(void)
{
    _close();
}

bool SqliteDatabase::execute(const String& sql)
{
    bool result{ false };
    if (mDbObject != nullptr)
    {
        result = SQLITE_OK == ::sqlite3_exec(reinterpret_cast<sqlite3*>(mDbObject), sql.getString(), nullptr, nullptr, nullptr);
    }

    return result;
}

bool SqliteDatabase::begin(void)
{
    constexpr std::string_view  sqlBegin{ "BEGIN TRANSACTION;" };

    return (mDbObject != nullptr ? SQLITE_OK == ::sqlite3_exec(reinterpret_cast<sqlite3*>(mDbObject), sqlBegin.data(), nullptr, nullptr, nullptr) : false);
}

bool SqliteDatabase::commit(bool doCommit)
{
    constexpr std::string_view sqlCommit{ "COMMIT;" };
    constexpr std::string_view sqlRoolback{ "ROLLBACK;" };

    return (mDbObject != nullptr ? SQLITE_OK == ::sqlite3_exec(reinterpret_cast<sqlite3*>(mDbObject), doCommit ? sqlCommit.data() : sqlRoolback.data(), nullptr, nullptr, nullptr) : false);
}
