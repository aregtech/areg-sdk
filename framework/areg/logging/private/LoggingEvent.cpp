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

#include <utility>

#if AREG_LOGGING

namespace areg {

LoggingEventData::LoggingEventData()
    : mAction   ( LoggingEventData::LogAction::Undefined )
    , mEntry    ( )
{
}

LoggingEventData::LoggingEventData( LoggingEventData::LogAction action )
    : mAction   ( action )
    , mEntry    ( )
{
}

LoggingEventData::LoggingEventData( const LoggingEventData & src )
    : mAction   ( src.mAction )
    , mEntry    ( src.mEntry )
{
}

LoggingEventData::LoggingEventData( LoggingEventData && src ) noexcept
    : mAction   ( src.mAction )
    , mEntry    ( std::move(src.mEntry) )
{
}

LoggingEventData & LoggingEventData::operator = (const LoggingEventData & src)
{
    if ( this != &src )
    {
        mAction = src.mAction;
        mEntry  = src.mEntry;
    }

    return (*this);
}

LoggingEventData & LoggingEventData::operator = ( LoggingEventData && src ) noexcept
{
    if ( this != &src )
    {
        mAction = src.mAction;
        mEntry  = std::move(src.mEntry);
    }

    return (*this);
}

} // namespace areg

#endif  // AREG_LOGGING
