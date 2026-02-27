/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/DebugOutputLogger.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debug Output Logger object to log message into the
 *              system Output console object
 ************************************************************************/
#include "areg/logging/private/DebugOutputLogger.hpp"

#include "areg/logging/LogConfiguration.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/ByteBuffer.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"

#include "areg/base/private/DebugDefs.hpp"

namespace areg
{

    #if AREG_LOGS

    DebugOutputLogger::DebugOutputLogger( areg::LogConfiguration & logConfig)
        : areg::LoggerBase        ( logConfig )
        , areg::OutStream       ( )

        , mIsOpened         ( false )
        , mOutputMessageA   ( )
    {
    }

    bool DebugOutputLogger::openLogger()
    {
    #if defined(OUTPUT_DEBUG)
        if ( mIsOpened == false )
        {
            if (mLogConfiguration.isDebugOutputLoggingEnabled())
            {
                mIsOpened = createLayouts();

                if (mIsOpened)
                {
                    areg::Process& curProcess = areg::Process::getInstance();
                    areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
                    areg::String::formatString( logMsgHello.logMessage
                                        , areg::LOG_MESSAGE_IZE
                                        , areg::LoggerBase::FOMAT_MESSAGE_HELLO.data()
                                        , areg::Process::getString(curProcess.getEnvironment())
                                        , curProcess.getFullPath().getString()
                                        , logMsgHello.logModuleId);

                    logMessage(logMsgHello);
                }
            }
        }
    #endif  // !defined(OUTPUT_DEBUG)

        return mIsOpened;
    }

    void DebugOutputLogger::closeLogger()
    {
    #if defined(OUTPUT_DEBUG)
        if ( mIsOpened )
        {
            areg::Process & curProcess = areg::Process::getInstance();
            areg::LogEntry logMsgGoodbye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
            areg::String::formatString( logMsgGoodbye.logMessage
                                , areg::LOG_MESSAGE_IZE
                                , areg::LoggerBase::FORMAT_MESSAGE_BYE.data()
                                , areg::Process::getString(curProcess.getEnvironment())
                                , curProcess.getFullPath().getString()
                                , logMsgGoodbye.logModuleId);
            logMessage(logMsgGoodbye);
        }
    #endif  // !defined(OUTPUT_DEBUG)

        releaseLayouts();
        mIsOpened = false;
    }

    #if defined(OUTPUT_DEBUG)

    void DebugOutputLogger::logMessage(const areg::LogEntry & logMessage)
    {
        if ( mIsOpened )
        {
            switch (logMessage.logMsgType)
            {
            case areg::LogMessageType::MessageText:
                getLayoutMessage().logMessage(logMessage, static_cast<areg::OutStream&>(*this));
                break;

            case areg::LogMessageType::ScopeEnter:
                getLayoutEnterScope().logMessage(logMessage, static_cast<areg::OutStream&>(*this));
                break;

            case areg::LogMessageType::ScopeExit:
                getLayoutExitScope().logMessage( logMessage, static_cast<areg::OutStream &>(*this) );
                break;

            default:
                ASSERT(false);  // unexpected message to log
                break;
            }

            flush();
        }
    }

    #else // !defined(OUTPUT_DEBUG)

    void DebugOutputLogger::logMessage(const areg::LogEntry & /*logMessage*/)
    {
    }

    #endif // !defined(OUTPUT_DEBUG)


    bool DebugOutputLogger::isLoggerOpened() const
    {
        return mIsOpened;
    }

    #if defined(OUTPUT_DEBUG)
    uint32_t DebugOutputLogger::write(const uint8_t * buffer, uint32_t size)
    {
        mOutputMessageA.append(reinterpret_cast<const char *>(buffer), size);
        return size;
    }
    #else   // defined(OUTPUT_DEBUG)
    uint32_t DebugOutputLogger::write(const uint8_t * /* buffer */, uint32_t size)
    {
        return size;
    }
    #endif  // defined(OUTPUT_DEBUG)

    uint32_t DebugOutputLogger::write(const areg::ByteBuffer & buffer)
    {
        return write(buffer.getBuffer(), buffer.getSizeUsed());
    }

    uint32_t DebugOutputLogger::write( const areg::String & ascii )
    {
    #if defined(OUTPUT_DEBUG)
        mOutputMessageA += ascii;
    #endif  // defined(OUTPUT_DEBUG)
        return ascii.getSpace();
    }

    uint32_t DebugOutputLogger::write( const areg::WideString & wide )
    {
    #if defined(OUTPUT_DEBUG)
        mOutputMessageA += wide;
    #endif  // !defined(OUTPUT_DEBUG)
        return wide.getSpace();
    }

    void DebugOutputLogger::flush()
    {
    #if defined(OUTPUT_DEBUG)
        areg::outputMessageOS(mOutputMessageA.getString());
    #endif // !defined(OUTPUT_DEBUG)

        mOutputMessageA.clear();
    }

    uint32_t DebugOutputLogger::getSizeWritable() const
    {
        return static_cast<uint32_t>(0xFFFF);
    }

    #endif // AREG_LOGS
} // namespace areg
