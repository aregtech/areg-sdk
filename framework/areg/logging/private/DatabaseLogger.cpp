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

namespace areg
{

    #if AREG_LOGS

    DatabaseLogger::DatabaseLogger(areg::LogConfiguration& logConfig)
        : areg::LoggerBase    (logConfig)
        , mDatabase     (nullptr)
        , mLock         (false)
    {
    }

    DatabaseLogger::~DatabaseLogger()
    {
        areg::Lock lock(mLock);
        if (mDatabase != nullptr)
        {
            mDatabase->disconnect();
            mDatabase = nullptr;
        }
    }

    bool DatabaseLogger::openLogger()
    {
        areg::Lock lock(mLock);
        bool result{ false };

        areg::String dbFile;
        if (isValid())
        {
            if (mLogConfiguration.isDatabaseLoggingEnabled())
            {
                areg::String fileName(mLogConfiguration.getDatabaseFullPath());
                dbFile = areg::File::normalizePath(fileName.getString());

                if (mDatabase->connect(dbFile, false))
                {
                    result = true;
                }
            }
        }

        return result;
    }

    void DatabaseLogger::closeLogger()
    {
        areg::Lock lock(mLock);
        if (isValid())
        {
            mDatabase->disconnect();
        }
    }

    void DatabaseLogger::logMessage(const areg::LogEntry& logMessage)
    {
        areg::Lock lock(mLock);
        if (isValid())
        {
            mDatabase->logMessage(logMessage);
        }
    }

    bool DatabaseLogger::isLoggerOpened() const
    {
        return false;
    }

    void DatabaseLogger::flushLogs()
    {
        areg::Lock lock(mLock);

        if (isValid())
        {
            mDatabase->commit(true);
        }
    }

    bool DatabaseLogger::createLayouts()
    {
        // no need to crate layouts for log DB
        return true;
    }

    void DatabaseLogger::releaseLayouts()
    {
    }

    #endif // AREG_LOGS
} // namespace areg
