/************************************************************************
 * \file        areg/trace/private/CETraceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The tracer thread, which is receiving log events
 *              and performs log operations. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/trace/private/CETraceEvent.hpp"
#include "areg/trace/private/CELogMessage.hpp"

CETraceEventData::CETraceEventData( void )
    : mAction       ( CETraceEventData::TraceUndefined )
    , mDataBuffer   ( )
{
}

CETraceEventData::CETraceEventData( eTraceAction action )
    : mAction       ( action )
    , mDataBuffer   ( )
{
}

CETraceEventData::CETraceEventData( eTraceAction action, const CESharedBuffer & dataBuffer )
    : mAction       ( action )
    , mDataBuffer   ( dataBuffer )
{
}

CETraceEventData::CETraceEventData( eTraceAction action, const CELogMessage & logData )
    : mAction       ( action )
    , mDataBuffer   ( )
{
    mDataBuffer << logData;
}

CETraceEventData::CETraceEventData( const CETraceEventData & src )
    : mAction       ( src.mAction )
    , mDataBuffer   ( src.mDataBuffer )
{
}

CETraceEventData::~CETraceEventData( void )
{
}

const CETraceEventData & CETraceEventData::operator = (const CETraceEventData & src)
{
    mAction = src.mAction;
    mDataBuffer = src.mDataBuffer;
    return (*this);
}
