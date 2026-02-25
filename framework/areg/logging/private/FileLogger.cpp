/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/FileLogger.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       File Logger object to log message into the file
 ************************************************************************/
#include "areg/logging/private/FileLogger.hpp"

#include "areg/logging/LogConfiguration.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"

#if AREG_LOGS

FileLogger::FileLogger( LogConfiguration & logConfig)
    : LoggerBase(logConfig)
    , mLogFile  ( )
{
}

bool FileLogger::openLogger()
{
    if ( (mLogFile.isOpened() == false) && mLogConfiguration.isFileLoggingEnabled())
    {
        areg::String fileName(mLogConfiguration.getLogFile() );
        if ( fileName.isEmpty() == false )
        {
            bool newFile      = static_cast<bool>(mLogConfiguration.getAppendData()) == false;
            uint32_t mode = static_cast<uint32_t>(areg::File::OpenMode::Write) | 
                                static_cast<uint32_t>(areg::File::OpenMode::Read) |
                                static_cast<uint32_t>(areg::File::OpenMode::ShareRead) |
                                static_cast<uint32_t>(areg::File::OpenMode::ShareWrite) |
                                static_cast<uint32_t>(areg::File::OpenMode::Text);

            if (areg::File::existFile(fileName))
            {
                mode |= newFile ? static_cast<uint32_t>(areg::File::OpenMode::Truncate) : static_cast<uint32_t>(areg::File::OpenMode::Exist);
            }
            else
            {
                mode |= static_cast<uint32_t>(areg::File::OpenMode::Create);
            }

            if ( mLogFile.open( fileName, mode) && createLayouts() )
            {
                    
                areg::Process & curProcess = areg::Process::getInstance();
                areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
                areg::String::formatString( logMsgHello.logMessage
                                    , areg::LOG_MESSAGE_IZE
                                    , LoggerBase::FOMAT_MESSAGE_HELLO.data()
                                    , areg::Process::getString(curProcess.getEnvironment())
                                    , curProcess.getFullPath().getString()
                                    , logMsgHello.logModuleId);

                logMessage(logMsgHello);
            }
        }
    }

    return mLogFile.isOpened();
}

void FileLogger::closeLogger()
{
    if ( mLogFile.isOpened() )
    {
        areg::Process & curProcess = areg::Process::getInstance();
        areg::LogEntry logMsgGoodbye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
        areg::String::formatString(logMsgGoodbye.logMessage
                            , areg::LOG_MESSAGE_IZE
                            , LoggerBase::FORMAT_MESSAGE_BYE.data()
                            , areg::Process::getString(curProcess.getEnvironment())
                            , curProcess.getFullPath().getString()
                            , logMsgGoodbye.logModuleId);

        logMessage(logMsgGoodbye);
    }

    releaseLayouts();
    mLogFile.close();
}

void FileLogger::logMessage( const areg::LogEntry & logMessage )
{
    if (mLogFile.isOpened())
    {
        switch (logMessage.logMsgType)
        {
        case areg::LogMessageType::MessageText:
            getLayoutMessage().logMessage(logMessage, static_cast<areg::OutStream&>(mLogFile));
            break;

        case areg::LogMessageType::ScopeEnter:
            getLayoutEnterScope().logMessage( logMessage, static_cast<areg::OutStream &>(mLogFile) );
            break;

        case areg::LogMessageType::ScopeExit:
            getLayoutExitScope().logMessage( logMessage, static_cast<areg::OutStream &>(mLogFile) );
            break;

        case areg::LogMessageType::Undefined: // fall through
        default:
            ASSERT(false);  // unexpected message to log
            break;
        }
    }
}

bool FileLogger::isLoggerOpened() const
{
    return mLogFile.isOpened();
}

void FileLogger::flushLogs()
{
    mLogFile.flush();
}

#endif // AREG_LOGS
