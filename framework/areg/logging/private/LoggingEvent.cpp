/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/logging/private/LoggingEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The logging thread, which is receiving log events
 *              and performs logging operations. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/logging/private/LoggingEvent.hpp"
#include "areg/logging/private/LogMessage.hpp"

#include <utility>

#if AREG_LOGS

namespace
{
    constexpr const uint32_t _logMessageSize{ static_cast<uint32_t>(sizeof(NELogging::sLogMessage)) };
    constexpr const uint32_t _logNamesSize{ static_cast<uint32_t>(NELogging::LOG_NAMES_SIZE * 2) };
    constexpr const uint32_t _logLocalMessage{ _logMessageSize - _logNamesSize };
}

LoggingEventData::LoggingEventData( void )
    : mAction       ( LoggingEventData::eLoggingAction::LoggingUndefined )
    , mDataBuffer   ( )
{
}

LoggingEventData::LoggingEventData( LoggingEventData::eLoggingAction action )
    : mAction       ( action )
    , mDataBuffer   ( )
{
}

LoggingEventData::LoggingEventData( LoggingEventData::eLoggingAction action, const SharedBuffer & dataBuffer )
    : mAction       ( action )
    , mDataBuffer   ( dataBuffer )
{
}

LoggingEventData::LoggingEventData( LoggingEventData::eLoggingAction action, const NELogging::sLogMessage & logData )
    : mAction       ( action )
    , mDataBuffer   (_logMessageSize, reinterpret_cast<const unsigned char *>(&logData), _logLocalMessage)
{
}

LoggingEventData::LoggingEventData( const LoggingEventData & src )
    : mAction       ( src.mAction )
    , mDataBuffer   ( src.mDataBuffer )
{
}

LoggingEventData::LoggingEventData( LoggingEventData && src ) noexcept
    : mAction       ( src.mAction )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
{
}

LoggingEventData & LoggingEventData::operator = (const LoggingEventData & src)
{
    mAction = src.mAction;
    mDataBuffer = src.mDataBuffer;
    return (*this);
}

LoggingEventData & LoggingEventData::operator = ( LoggingEventData && src ) noexcept
{
    mAction = src.mAction;
    mDataBuffer = std::move(src.mDataBuffer);
    return (*this);
}

#endif  // AREG_LOGS
