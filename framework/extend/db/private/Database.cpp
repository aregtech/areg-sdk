/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/db/private/Database.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, extended library, SQLite database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "extend/db/Database.hpp"

#include "areg/base/File.hpp"

#include <string_view>

Database::Database(void)
    : IEDatabaseEngine  ( )
    , mDbPath           ( )
    , mDbObject         ( nullptr )
{
}

Database::Database(const String& dbPath, bool open)
    : IEDatabaseEngine  ( )
    , mDbPath           (dbPath.isEmpty() ? String::getEmptyString() : File::normalizePath(dbPath))
    , mDbObject         ( nullptr )
{
    if (open && (mDbPath.isEmpty() == false))
    {
        if (sqlite3_open(mDbPath.getString(), &mDbObject) != SQLITE_OK)
        {
            _close();
        }
    }
}

Database::~Database(void)
{
    _close();
}

inline bool Database::_open(const String& dbPath)
{
    bool result{ true };
    _close();
    mDbPath = dbPath.isEmpty() == false ? File::normalizePath(dbPath) : mDbPath;
    if (SQLITE_OK != sqlite3_open(mDbPath.getString(), &mDbObject))
    {
        _close();
        result = false;
    }

    return result;
}

inline void Database::_close(void)
{
    if (mDbObject != nullptr)
    {
        sqlite3_close(mDbObject);
        mDbObject = nullptr;
    }
}

bool Database::isOperable(void) const
{
    return (mDbObject != nullptr);
}

bool Database::connect(const String& dbPath)
{
    return _open(dbPath);
}

void Database::disconnect(void)
{
    _close();
}

bool Database::execute(const String& sql)
{
    bool result{ false };
    if (mDbObject != nullptr)
    {
        result = SQLITE_OK == sqlite3_exec(mDbObject, sql.getString(), nullptr, nullptr, nullptr);
    }

    return result;
}

bool Database::begin(void)
{
    constexpr std::string_view  sqlBegin{ "BEGIN TRANSACTION;" };

    return (mDbObject != nullptr ? SQLITE_OK == sqlite3_exec(mDbObject, sqlBegin.data(), nullptr, nullptr, nullptr) : false);
}

bool Database::commit(bool doCommit)
{
    constexpr std::string_view sqlCommit{ "COMMIT;" };
    constexpr std::string_view sqlRoolback{ "ROLLBACK;" };

    return (mDbObject != nullptr ? SQLITE_OK == sqlite3_exec(mDbObject, doCommit ? sqlCommit.data() : sqlRoolback.data(), nullptr, nullptr, nullptr) : false);
}

