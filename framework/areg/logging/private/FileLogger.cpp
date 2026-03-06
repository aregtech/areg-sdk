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

#if AREG_LOGGING

namespace areg {

FileLogger::FileLogger( LogConfiguration & logConfig)
    : LoggerBase(logConfig)
    , mLogFile  ( )
{
}

bool FileLogger::open_logger()
{
    if ( (mLogFile.is_opened() == false) && mLogConfiguration.is_file_logging_enabled())
    {
        String fileName(mLogConfiguration.log_file() );
        if ( fileName.is_empty() == false )
        {
            bool newFile      = static_cast<bool>(mLogConfiguration.append_data()) == false;
            uint32_t mode = static_cast<uint32_t>(File::OpenMode::Write) | 
                                static_cast<uint32_t>(File::OpenMode::Read) |
                                static_cast<uint32_t>(File::OpenMode::ShareRead) |
                                static_cast<uint32_t>(File::OpenMode::ShareWrite) |
                                static_cast<uint32_t>(File::OpenMode::Text);

            if (File::has_file(fileName))
            {
                mode |= newFile ? static_cast<uint32_t>(File::OpenMode::Truncate) : static_cast<uint32_t>(File::OpenMode::Exist);
            }
            else
            {
                mode |= static_cast<uint32_t>(File::OpenMode::Create);
            }

            if ( mLogFile.open( fileName, mode) && create_layouts() )
            {
                    
                Process & curProcess = Process::instance();
                areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
                String::format_string( logMsgHello.logMessage
                                    , areg::LOG_MESSAGE_SIZE
                                    , LoggerBase::FORMAT_MESSAGE_HELLO.data()
                                    , Process::as_string(curProcess.environment())
                                    , curProcess.full_path().as_string()
                                    , logMsgHello.logModuleId);

                log_message(logMsgHello);
            }
        }
    }

    return mLogFile.is_opened();
}

void FileLogger::close_logger()
{
    if ( mLogFile.is_opened() )
    {
        Process & curProcess = Process::instance();
        areg::LogEntry logMsgGoodbye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
        String::format_string(logMsgGoodbye.logMessage
                            , areg::LOG_MESSAGE_SIZE
                            , LoggerBase::FORMAT_MESSAGE_BYE.data()
                            , Process::as_string(curProcess.environment())
                            , curProcess.full_path().as_string()
                            , logMsgGoodbye.logModuleId);

        log_message(logMsgGoodbye);
    }

    release_layouts();
    mLogFile.close();
}

void FileLogger::log_message( const areg::LogEntry & logMessage)
{
    if (mLogFile.is_opened())
    {
        switch (logMessage.logMsgType)
        {
        case areg::LogMessageType::MessageText:
            layout_message().log_message(logMessage, static_cast<OutStream&>(mLogFile));
            break;

        case areg::LogMessageType::ScopeEnter:
            layout_enter_scope().log_message(logMessage, static_cast<OutStream &>(mLogFile) );
            break;

        case areg::LogMessageType::ScopeExit:
            layout_exit_scope().log_message(logMessage, static_cast<OutStream &>(mLogFile) );
            break;

        case areg::LogMessageType::Undefined: // fall through
        default:
            ASSERT(false);  // unexpected message to log
            break;
        }
    }
}

bool FileLogger::is_logger_opened() const
{
    return mLogFile.is_opened();
}

void FileLogger::flush_logs()
{
    mLogFile.flush();
}

} // namespace areg

#endif // AREG_LOGGING
