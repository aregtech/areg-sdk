/************************************************************************
 * \file        areg/component/private/EventData.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Request and Response events,
 *              Event Data object containing minimal generic data to
 *              support request and response communication.
 *
 ************************************************************************/
#include "areg/component/EventData.hpp"
//////////////////////////////////////////////////////////////////////////
// EventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventData class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventData::EventData( unsigned int msgId, EventDataStream::eEventData dataType, const char* name /*= NULL*/ )
    : mDataType (NEService::getMessageDataType(msgId))
    , mData     (dataType, name)
{
    ; // do nothing
}

EventData::EventData( unsigned int msgId, const EventDataStream & args, const char* name /*= NULL*/ )
    : mDataType (NEService::getMessageDataType(msgId))
    , mData     (args, name)
{
    mData.resetCursor();
}

EventData::EventData( const EventData& src )
    : mDataType (src.mDataType)
    , mData     (src.mData)
{
    mData.resetCursor();
}

EventData::EventData(const IEInStream & stream)
    : mDataType ( NEService::UNDEFINED_DATA_TYPE )
    , mData     ( EventDataStream::EventDataExternal )
{
    stream >> mDataType;
    stream >> mData;
    mData.resetCursor();
}

EventData::~EventData( void )
{
    ; // do nothing
}

const EventData & EventData::operator = (const EventData & src)
{
    if (this != &src)
    {
        mDataType   = src.mDataType;
        mData       = src.mData;
    }
    return (*this);
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, EventData & input )
{
    stream >> input.mDataType;
    stream >> input.mData;
    input.mData.resetCursor();
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const EventData & output )
{
    stream << output.mDataType;
    stream << output.mData;
    output.mData.resetCursor();
    return stream;
}
