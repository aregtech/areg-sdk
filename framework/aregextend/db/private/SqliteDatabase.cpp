/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/private/SqliteDatabase.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/db/SqliteDatabase.hpp"

#include "areg/base/File.hpp"

#include <string_view>

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

    inline sqlite3** _sqlite(void** dbObject)
    {
        return reinterpret_cast<sqlite3 **>(dbObject);
    }
} // namespace

namespace areg::ext {

SqliteDatabase::SqliteDatabase()
    : DatabaseEngine( )
    , mDbPath       ( )
    , mDbObject     ( nullptr )
{
}

SqliteDatabase::SqliteDatabase(const String& dbPath, bool open)
    : DatabaseEngine( )
    , mDbPath       ( )
    , mDbObject     ( nullptr )
{
    if (open)
    {
        _open(dbPath);
    }
    else
    {
        mDbPath = dbPath.is_empty() ? String::empty_string() : File::normalize_path(dbPath);
    }
}

SqliteDatabase::~SqliteDatabase()
{
    _close();
}

inline bool SqliteDatabase::_open(const String& dbPath)
{
    bool result{ true };
    _close();
    mDbPath = dbPath.is_empty() == false ? File::normalize_path(dbPath) : mDbPath;
    if (mDbPath.is_empty())
    {
        return false;
    }

    String folder = File::file_directory(mDbPath);
    if ((folder.is_empty() == false) && (File::has_dir(folder) == false))
    {
        File::create_dir_cascaded(folder);
    }

    if (SQLITE_OK != ::sqlite3_open(mDbPath.as_string(), _sqlite(&mDbObject)))
    {
        _close();
        result = false;
    }

    return result;
}

inline void SqliteDatabase::_close() noexcept
{
    if (mDbObject != nullptr)
    {
        ::sqlite3_close(_sqlite(mDbObject));
        mDbObject = nullptr;
    }
}

bool SqliteDatabase::is_operable() const noexcept
{
    return (mDbObject != nullptr);
}

bool SqliteDatabase::connect(const String& dbPath, bool /*readOnly*/)
{
    return _open(dbPath);
}

void SqliteDatabase::disconnect()
{
    _close();
}

bool SqliteDatabase::execute(const String& sql)
{
    bool result{ false };
    if (mDbObject != nullptr)
    {
        result = SQLITE_OK == ::sqlite3_exec(_sqlite(mDbObject), sql.as_string(), nullptr, nullptr, nullptr);
    }

    return result;
}

bool SqliteDatabase::begin()
{
    constexpr std::string_view  sqlBegin{ "BEGIN TRANSACTION;" };

    return (mDbObject != nullptr ? SQLITE_OK == ::sqlite3_exec(_sqlite(mDbObject), sqlBegin.data(), nullptr, nullptr, nullptr) : false);
}

bool SqliteDatabase::commit(bool doCommit)
{
    constexpr std::string_view sqlCommit{ "COMMIT;" };
    constexpr std::string_view sqlRollback{ "ROLLBACK;" };

    return (mDbObject != nullptr ? SQLITE_OK == ::sqlite3_exec(_sqlite(mDbObject), doCommit ? sqlCommit.data() : sqlRollback.data(), nullptr, nullptr, nullptr) : false);
}

bool SqliteDatabase::rollback()
{
    return commit(false);
}

} // namespace areg::ext
