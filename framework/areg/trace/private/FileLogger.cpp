/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/FileLogger.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       File Logger object to log message into the file
 ************************************************************************/
#include "areg/trace/private/FileLogger.hpp"

#include "areg/trace/private/LogConfiguration.hpp"
#include "areg/trace/private/TraceProperty.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"

FileLogger::FileLogger( LogConfiguration & tracerConfig )
    : LoggerBase( tracerConfig )

    , mLogFile          ( )
{
}

bool FileLogger::openLogger( void )
{
    if ( mLogFile.isOpened() == false )
    {
        const LogConfiguration & traceConfig = getTraceConfiguration();
        ASSERT( static_cast<bool>(traceConfig.getStatus().getValue()) );
        const TraceProperty & prop = traceConfig.getLogFile();
        if ( prop.isValid() )
        {
            String fileName(File::normalizePath(static_cast<const char *>(prop.getValue())) );
            if ( fileName.isEmpty() == false )
            {
                bool newFile      = static_cast<bool>(traceConfig.getAppendData()) == false;
                unsigned int mode = File::FO_MODE_WRITE | File::FO_MODE_READ | File::FO_MODE_SHARE_READ | File::FO_MODE_SHARE_WRITE | File::FO_MODE_TEXT;

                if (File::existFile(fileName))
                {
                    mode |= newFile ? File::FO_MODE_TRUNCATE : File::FO_MODE_EXIST;
                }
                else
                {
                    mode |= FileBase::FO_MODE_CREATE;
                }

                if ( mLogFile.open( fileName, mode) && createLayouts() )
                {
                    
                    Process & curProcess = Process::getInstance();
                    NETrace::sLogMessage logMsgHello(NETrace::eMessageType::MsgText, 0, NETrace::eLogPriority::PrioIgnoreLayout, nullptr, 0);
                    String::formatString( logMsgHello.lmTrace.dataMessage
                                        , NETrace::LOG_MESSAGE_BUFFER_SIZE
                                        , LoggerBase::FOMAT_MESSAGE_HELLO.data()
                                        , Process::getString(curProcess.getEnvironment())
                                        , curProcess.getFullPath().getString()
                                        , logMsgHello.lmTrace.dataModuleId);

                    logMessage(logMsgHello);
                }
            }
            else
            {
                ; // no file specified
            }
        }
        else
        {
            ; // no property was set
        }
    }

    return mLogFile.isOpened();
}

void FileLogger::closeLogger(void)
{
    if ( mLogFile.isOpened() )
    {
        Process & curProcess = Process::getInstance();
        NETrace::sLogMessage logMsgGoodbye(NETrace::eMessageType::MsgText, 0, NETrace::eLogPriority::PrioIgnoreLayout, nullptr, 0);
        String::formatString(logMsgGoodbye.lmTrace.dataMessage
                            , NETrace::LOG_MESSAGE_BUFFER_SIZE
                            , LoggerBase::FORMAT_MESSAGE_BYE.data()
                            , Process::getString(curProcess.getEnvironment())
                            , curProcess.getFullPath().getString()
                            , logMsgGoodbye.lmTrace.dataModuleId);

        logMessage(logMsgGoodbye);
    }

    releaseLayouts();
    mLogFile.close();
}

void FileLogger::logMessage( const NETrace::sLogMessage & logMessage )
{
    if (mLogFile.isOpened() && NETrace::isLogMessage(logMessage.lmHeader))
    {
        switch (logMessage.lmTrace.dataMsgType)
        {
        case NETrace::eMessageType::MsgText:
            getLayoutMessage().logMessage(logMessage, static_cast<IEOutStream&>(mLogFile));
            break;

        case NETrace::eMessageType::MsgScopeEnter:
            getLayoutEnterScope().logMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        case NETrace::eMessageType::MsgScopeExit:
            getLayoutExitScope().logMessage( logMessage, static_cast<IEOutStream &>(mLogFile) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }
    }
}

bool FileLogger::isLoggerOpened(void) const
{
    return mLogFile.isOpened();
}

void FileLogger::flushLogs(void)
{
    mLogFile.flush();
}
