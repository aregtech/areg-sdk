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


#if AREG_LOGGING

namespace areg {

DebugOutputLogger::DebugOutputLogger( LogConfiguration & logConfig)
    : LoggerBase        ( logConfig )
    , OutStream       ( )

    , mIsOpened         ( false )
    , mOutputMessageA   ( )
{
}

bool DebugOutputLogger::open_logger()
{
#if defined(OUTPUT_DEBUG)
    if ( mIsOpened == false )
    {
        if (mLogConfiguration.is_debug_logging_enabled())
        {
            mIsOpened = create_layouts();

            if (mIsOpened)
            {
                Process& curProcess = Process::instance();
                areg::LogEntry logMsgHello(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
                String::format_string( logMsgHello.logMessage
                                    , areg::LOG_MESSAGE_IZE
                                    , LoggerBase::FOMAT_MESSAGE_HELLO.data()
                                    , Process::as_string(curProcess.environment())
                                    , curProcess.full_path().as_string()
                                    , logMsgHello.logModuleId);

                log_message(logMsgHello);
            }
        }
    }
#endif  // !defined(OUTPUT_DEBUG)

    return mIsOpened;
}

void DebugOutputLogger::close_logger()
{
#if defined(OUTPUT_DEBUG)
    if ( mIsOpened )
    {
        Process & curProcess = Process::instance();
        areg::LogEntry logMsgGoodbye(areg::LogMessageType::MessageText, 0u, 0u, 0u, areg::LogPriority::PrioIgnoreLayout, nullptr, 0);
        String::format_string( logMsgGoodbye.logMessage
                            , areg::LOG_MESSAGE_IZE
                            , LoggerBase::FORMAT_MESSAGE_BYE.data()
                            , Process::as_string(curProcess.environment())
                            , curProcess.full_path().as_string()
                            , logMsgGoodbye.logModuleId);
        log_message(logMsgGoodbye);
    }
#endif  // !defined(OUTPUT_DEBUG)

    release_layouts();
    mIsOpened = false;
}

#if defined(OUTPUT_DEBUG)

void DebugOutputLogger::log_message(const areg::LogEntry & logMessage)
{
    if ( mIsOpened )
    {
        switch (logMessage.logMsgType)
        {
        case areg::LogMessageType::MessageText:
            layout_message().log_message(logMessage, static_cast<OutStream&>(*this));
            break;

        case areg::LogMessageType::ScopeEnter:
            layout_enter_scope().log_message(logMessage, static_cast<OutStream&>(*this));
            break;

        case areg::LogMessageType::ScopeExit:
            layout_exit_scope().log_message(logMessage, static_cast<OutStream &>(*this) );
            break;

        default:
            ASSERT(false);  // unexpected message to log
            break;
        }

        flush();
    }
}

#else // !defined(OUTPUT_DEBUG)

void DebugOutputLogger::log_message(const areg::LogEntry & /*logMessage*/)
{
}

#endif // !defined(OUTPUT_DEBUG)


bool DebugOutputLogger::is_logger_opened() const
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

uint32_t DebugOutputLogger::write(const ByteBuffer & buffer)
{
    return write(buffer.buffer(), buffer.size_used());
}

uint32_t DebugOutputLogger::write( const String & ascii )
{
#if defined(OUTPUT_DEBUG)
    mOutputMessageA += ascii;
#endif  // defined(OUTPUT_DEBUG)
    return ascii.space();
}

uint32_t DebugOutputLogger::write( const WideString & wide )
{
#if defined(OUTPUT_DEBUG)
    mOutputMessageA += wide;
#endif  // !defined(OUTPUT_DEBUG)
    return wide.space();
}

void DebugOutputLogger::flush()
{
#if defined(OUTPUT_DEBUG)
    areg::output_message_os(mOutputMessageA.as_string());
#endif // !defined(OUTPUT_DEBUG)

    mOutputMessageA.clear();
}

uint32_t DebugOutputLogger::size_writable() const
{
    return static_cast<uint32_t>(0xFFFF);
}

} // namespace areg

#endif // AREG_LOGGING
