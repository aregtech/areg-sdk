/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/DebugOutputLogger.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/
#include "areg/trace/private/DebugOutputLogger.hpp"

#include "areg/trace/private/LogConfiguration.hpp"
#include "areg/trace/private/TraceProperty.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/IEByteBuffer.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

#include "areg/base/private/NEDebug.hpp"

DebugOutputLogger::DebugOutputLogger( LogConfiguration & tracerConfig )
    : LoggerBase        ( tracerConfig )
    , IEOutStream       ( )

    , mIsOpened         ( false )
    , mOutputMessageA   ( )
{
}

bool DebugOutputLogger::openLogger(void)
{
#if defined(OUTPUT_DEBUG)
    if ( mIsOpened == false )
    {
        const LogConfiguration & traceConfig = getTraceConfiguration();
        ASSERT( static_cast<bool>(traceConfig.getStatus().getValue()) );
        
        const TraceProperty & prop = traceConfig.getDebugOutput();
        if ( prop.isValid() && static_cast<bool>(prop.getValue()) )
        {

            mIsOpened = createLayouts( );

            if ( mIsOpened )
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
            ; // no property was set
        }
    }
#endif  // !defined(OUTPUT_DEBUG)

    return mIsOpened;
}

void DebugOutputLogger::closeLogger(void)
{
#if defined(OUTPUT_DEBUG)
    if ( mIsOpened )
    {
        Process & curProcess = Process::getInstance();
        NETrace::sLogMessage logMsgGoodbye(NETrace::eMessageType::MsgText, 0, NETrace::eLogPriority::PrioIgnoreLayout, nullptr, 0);
        String::formatString( logMsgGoodbye.lmTrace.dataMessage
                            , NETrace::LOG_MESSAGE_BUFFER_SIZE
                            , LoggerBase::FORMAT_MESSAGE_BYE.data()
                            , Process::getString(curProcess.getEnvironment())
                            , curProcess.getFullPath().getString()
                            , logMsgGoodbye.lmTrace.dataModuleId);
        logMessage(logMsgGoodbye);
    }
#endif  // !defined(OUTPUT_DEBUG)

    releaseLayouts();
    mIsOpened = false;
}

#if defined(OUTPUT_DEBUG)

void DebugOutputLogger::logMessage(const NETrace::sLogMessage & logMessage)
{
    if ( mIsOpened && NETrace::isLogMessage(logMessage.lmHeader) )
    {
        switch (logMessage.lmHeader.hdrLogType)
        {
        case NETrace::eMessageType::LogMessage:
            getLayoutMessage().logMessage(logMessage, static_cast<IEOutStream &>(*this));
            break;

        case NETrace::eMessageType::LogScopeEnter:
            getLayoutEnterScope().logMessage( logMessage, static_cast<IEOutStream &>(*this) );
            break;

        case NETrace::eMessageType::LogScopeExit:
            getLayoutExitScope().logMessage( logMessage, static_cast<IEOutStream &>(*this) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }

        flush();
    }
}

#else // !defined(OUTPUT_DEBUG)

void DebugOutputLogger::logMessage(const NETrace::sLogMessage & /*logMessage*/)
{
}

#endif // !defined(OUTPUT_DEBUG)


bool DebugOutputLogger::isLoggerOpened(void) const
{
    return mIsOpened;
}

unsigned int DebugOutputLogger::write(const unsigned char * /*buffer*/, unsigned int size)
{
    return size;
}

unsigned int DebugOutputLogger::write(const IEByteBuffer & buffer)
{
    return buffer.getSizeUsed();
}

unsigned int DebugOutputLogger::write( const String & asciiString )
{
#if defined(OUTPUT_DEBUG)
    mOutputMessageA += asciiString;
#endif  // defined(OUTPUT_DEBUG)
    return asciiString.getSpace();
}

unsigned int DebugOutputLogger::write( const WideString & wideString )
{
#if defined(OUTPUT_DEBUG)
    mOutputMessageA += wideString;
#endif  // !defined(OUTPUT_DEBUG)
    return wideString.getSpace();
}

void DebugOutputLogger::flush(void)
{
#if defined(OUTPUT_DEBUG)
    NEDebug::outputMessageOS(mOutputMessageA.getString());
#endif // !defined(OUTPUT_DEBUG)

    mOutputMessageA.clear();
}

unsigned int DebugOutputLogger::getSizeWritable(void) const
{
    return static_cast<unsigned int>(0xFFFF);
}

void DebugOutputLogger::flushLogs(void)
{
    flush( );
}
