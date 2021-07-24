/************************************************************************
 * \file        areg/trace/private/TraceEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       The tracer thread, which is receiving log events
 *              and performs log operations. 
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/trace/private/TraceEvent.hpp"
#include "areg/trace/private/LogMessage.hpp"

TraceEventData::TraceEventData( void )
    : mAction       ( TraceEventData::TraceUndefined )
    , mDataBuffer   ( )
{
}

TraceEventData::TraceEventData( eTraceAction action )
    : mAction       ( action )
    , mDataBuffer   ( )
{
}

TraceEventData::TraceEventData( eTraceAction action, const SharedBuffer & dataBuffer )
    : mAction       ( action )
    , mDataBuffer   ( dataBuffer )
{
}

TraceEventData::TraceEventData( eTraceAction action, const LogMessage & logData )
    : mAction       ( action )
    , mDataBuffer   ( )
{
    mDataBuffer << logData;
}

TraceEventData::TraceEventData( const TraceEventData & src )
    : mAction       ( src.mAction )
    , mDataBuffer   ( src.mDataBuffer )
{
}

TraceEventData::~TraceEventData( void )
{
}

const TraceEventData & TraceEventData::operator = (const TraceEventData & src)
{
    mAction = src.mAction;
    mDataBuffer = src.mDataBuffer;
    return (*this);
}
