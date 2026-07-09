/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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

#if AREG_LOGGING

namespace areg {

DatabaseLogger::DatabaseLogger(LogConfiguration& logConfig)
    : LoggerBase    (logConfig)
    , mDatabase     (nullptr)
{
}

DatabaseLogger::~DatabaseLogger()
{
    if (mDatabase != nullptr)
    {
        mDatabase->disconnect();
        mDatabase = nullptr;
    }
}

bool DatabaseLogger::open_logger()
{

    if (!is_valid())
        return false;
    
    if (!mLogConfiguration.is_db_logging_enabled())
        return false;
    
    String fileName(mLogConfiguration.database_full_path());
    String dbFile = File::normalize_path(fileName.as_string());
    return mDatabase->connect(dbFile, false);
}

void DatabaseLogger::close_logger()
{
    if (is_valid())
    {
        mDatabase->disconnect();
    }
}

void DatabaseLogger::log_message(const areg::LogEntry& logMessage)
{
    if (is_valid())
    {
        mDatabase->log_message(logMessage);
    }
}

bool DatabaseLogger::is_logger_opened() const noexcept
{
    return false;
}

bool DatabaseLogger::create_layouts()
{
    // no need to crate layouts for log DB
    return true;
}

void DatabaseLogger::release_layouts()
{
}

void DatabaseLogger::flush_logs()
{
    if (is_valid())
    {
        mDatabase->commit(true);
    }
}

} // namespace areg

#endif // AREG_LOGGING
