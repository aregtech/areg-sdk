/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventData.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Generic Request and Response events,
 *              Event Data object containing minimal generic data to
 *              support request and response communication.
 *
 ************************************************************************/
#include "areg/component/EventData.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// EventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventData class Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventData::EventData( unsigned int msgId, EventDataStream::eEventData dataType, const char* name /*= nullptr*/ )
    : mDataType (NEService::getMessageDataType(msgId))
    , mData     (dataType, name)
{
}

EventData::EventData( unsigned int msgId, const EventDataStream & args, const char* name /*= nullptr*/ )
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

EventData::EventData( EventData && src ) noexcept
    : mDataType ( std::move(src.mDataType) )
    , mData     ( std::move(src.mData) )
{
    mData.resetCursor( );
}

EventData::EventData(const IEInStream & stream)
    : mDataType ( NEService::eMessageDataType::UndefinedDataType )
    , mData     ( EventDataStream::eEventData::EventDataExternal )
{
    stream >> mDataType;
    stream >> mData;
    mData.resetCursor();
}

EventData & EventData::operator = (const EventData & src)
{
    mDataType   = src.mDataType;
    mData       = src.mData;
    return (*this);
}

EventData & EventData::operator = ( EventData && src ) noexcept
{
    if (this != &src)
    {
        mDataType   = std::move(src.mDataType);
        mData       = std::move(src.mData);
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
