/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/trace/private/TraceEvent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       The tracer thread, which is receiving log events
 *              and performs log operations. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/trace/private/TraceEvent.hpp"
#include "areg/trace/private/LogMessage.hpp"

#include <utility>

namespace
{
    constexpr const uint32_t _logMessageSize{ static_cast<uint32_t>(sizeof(NETrace::sLogMessage)) };
    constexpr const uint32_t _logNamesSize{ static_cast<uint32_t>(NETrace::LOG_NAMES_SIZE * 2) };
    constexpr const uint32_t _logLocalMessage{ _logMessageSize - _logNamesSize };
}

TraceEventData::TraceEventData( void )
    : mAction       ( TraceEventData::eTraceAction::TraceUndefined )
    , mDataBuffer   ( )
{
}

TraceEventData::TraceEventData( TraceEventData::eTraceAction action )
    : mAction       ( action )
    , mDataBuffer   ( )
{
}

TraceEventData::TraceEventData( TraceEventData::eTraceAction action, const SharedBuffer & dataBuffer )
    : mAction       ( action )
    , mDataBuffer   ( dataBuffer )
{
}

TraceEventData::TraceEventData( TraceEventData::eTraceAction action, const NETrace::sLogMessage & logData )
    : mAction       ( action )
    , mDataBuffer   (_logMessageSize, reinterpret_cast<const unsigned char *>(&logData), _logLocalMessage)
{
}

TraceEventData::TraceEventData( const TraceEventData & src )
    : mAction       ( src.mAction )
    , mDataBuffer   ( src.mDataBuffer )
{
}

TraceEventData::TraceEventData( TraceEventData && src ) noexcept
    : mAction       ( src.mAction )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
{
}

TraceEventData & TraceEventData::operator = (const TraceEventData & src)
{
    mAction = src.mAction;
    mDataBuffer = src.mDataBuffer;
    return (*this);
}

TraceEventData & TraceEventData::operator = ( TraceEventData && src ) noexcept
{
    mAction = src.mAction;
    mDataBuffer = std::move(src.mDataBuffer);
    return (*this);
}
