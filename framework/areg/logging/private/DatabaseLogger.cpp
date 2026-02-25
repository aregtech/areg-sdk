/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/DatabaseLogger.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/logging/private/DatabaseLogger.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/logging/LogConfiguration.hpp"

#include <string_view>

#if AREG_LOGS

DatabaseLogger::DatabaseLogger(LogConfiguration& logConfig)
    : LoggerBase    (logConfig)
    , mDatabase     (nullptr)
    , mLock         (false)
{
}

DatabaseLogger::~DatabaseLogger()
{
    Lock lock(mLock);
    if (mDatabase != nullptr)
    {
        mDatabase->disconnect();
        mDatabase = nullptr;
    }
}

bool DatabaseLogger::open_logger()
{
    Lock lock(mLock);
    bool result{ false };

    String dbFile;
    if (is_valid())
    {
        if (mLogConfiguration.is_db_logging_enabled())
        {
            String fileName(mLogConfiguration.database_full_path());
            dbFile = File::normalize_path(fileName.as_string());

            if (mDatabase->connect(dbFile, false))
            {
                result = true;
            }
        }
    }

    return result;
}

void DatabaseLogger::close_logger()
{
    Lock lock(mLock);
    if (is_valid())
    {
        mDatabase->disconnect();
    }
}

void DatabaseLogger::log_message(const NELogging::LogEntry& log_message)
{
    Lock lock(mLock);
    if (is_valid())
    {
        mDatabase->log_message(log_message);
    }
}

bool DatabaseLogger::is_logger_opened() const
{
    return false;
}

void DatabaseLogger::flush_logs()
{
    Lock lock(mLock);

    if (is_valid())
    {
        mDatabase->commit(true);
    }
}

bool DatabaseLogger::create_layouts()
{
    // no need to crate layouts for log DB
    return true;
}

void DatabaseLogger::release_layouts()
{
}

#endif // AREG_LOGS
