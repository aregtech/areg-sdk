/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/DatabaseLogger.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, File Logger object to log message into the file
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/trace/private/DatabaseLogger.hpp"

#include "areg/base/File.hpp"
#include "areg/persist/IEDatabaseEngine.hpp"
#include "areg/trace/private/LogConfiguration.hpp"

DatabaseLogger::DatabaseLogger(LogConfiguration& tracerConfig)
    : LoggerBase    (tracerConfig)
    , mDatabase     (nullptr)
    , mLock         (false)
{
}

DatabaseLogger::~DatabaseLogger(void)
{
    Lock lock(mLock);
    if (mDatabase != nullptr)
    {
        mDatabase->disconnect();
        mDatabase = nullptr;
    }
}

bool DatabaseLogger::openLogger(void)
{
    Lock lock(mLock);
    bool result{ false };

    String dbFile;
    if (isValid())
    {
        if (mLogConfiguration.isDatabaseLoggingEnabled())
        {
            String fileName(mLogConfiguration.getDatabaseLocation());
            dbFile = File::normalizePath(fileName.getString());

            if (mDatabase->connect(dbFile))
            {
                result = true;
            }
        }
    }

    return result;
}

void DatabaseLogger::closeLogger(void)
{
    Lock lock(mLock);
    if (isValid())
    {
        mDatabase->disconnect();
    }
}

void DatabaseLogger::logMessage(const NETrace::sLogMessage& logMessage)
{
}

bool DatabaseLogger::isLoggerOpened(void) const
{
    return false;
}

void DatabaseLogger::flushLogs(void)
{
    Lock lock(mLock);

    if (isValid())
    {
        mDatabase->commit(true);
    }
}

bool DatabaseLogger::createLayouts(void)
{
    // no need to crate layouts for log DB
    return true;
}

void DatabaseLogger::releaseLayouts(void)
{
}

