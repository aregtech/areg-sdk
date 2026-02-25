/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDataStream.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, event data stream implementation
 *
 ************************************************************************/
#include "areg/component/EventDataStream.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// EventDataStream class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
    //! The default name of the event stream.
    static constexpr std::string_view DefaultStreamName{ "EventDataStream" };

}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, static members
//////////////////////////////////////////////////////////////////////////

const EventDataStream& EventDataStream::empty_data()
{
    static const EventDataStream _data(EventDataStream::EventDataKind::Empty, areg::String("EmptyData"));
    return _data;
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventDataStream::EventDataStream( EventDataStream::EventDataKind evetDataType, const areg::String & name /*= areg::String::getEmptyString()*/ )
    : areg::IOStream      ( )

    , mEventDataType(evetDataType)
    , mBufferName   (name.isEmpty() == false ? name : DefaultStreamName)
    , mDataBuffer   ( )
    , mSharedList   ( )
{
}

EventDataStream::EventDataStream( const EventDataStream & buffer, const areg::String & name )
    : areg::IOStream    ( )

    , mEventDataType(buffer.mEventDataType)
    , mBufferName   (name.isEmpty() == false ? name : DefaultStreamName)
    , mDataBuffer   (buffer.mDataBuffer)
    , mSharedList   (buffer.mSharedList)
{
    mDataBuffer.moveToBegin();
}

EventDataStream::EventDataStream( const EventDataStream & src )
    : areg::IOStream    ( )

    , mEventDataType(src.mEventDataType)
    , mBufferName   (src.mBufferName)
    , mDataBuffer   (src.mDataBuffer)
    , mSharedList   (src.mSharedList)
{
}

EventDataStream::EventDataStream( EventDataStream && src ) noexcept
    : areg::IOStream    ( )

    , mEventDataType( src.mEventDataType )
    , mBufferName   ( std::move(src.mBufferName) )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
    , mSharedList   ( std::move(src.mSharedList) )
{
}

EventDataStream::EventDataStream(const areg::InStream & stream)
    : areg::IOStream    ( )

    , mEventDataType( EventDataStream::EventDataKind::External)
    , mBufferName   ( DefaultStreamName)
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    stream >> mEventDataType >> mBufferName >> mDataBuffer;
}

EventDataStream::~EventDataStream()
{
    mSharedList.clear();
    mDataBuffer.invalidate();
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, operators
//////////////////////////////////////////////////////////////////////////
EventDataStream & EventDataStream::operator = ( const EventDataStream & src )
{
    if (static_cast<const EventDataStream *>(this) != &src)
    {
        mSharedList = src.mSharedList;
        mDataBuffer = src.mDataBuffer;
        mDataBuffer.moveToBegin();
    }

    return (*this);
}

EventDataStream & EventDataStream::operator = ( EventDataStream && src ) noexcept
{
    if ( static_cast<EventDataStream *>(this) != &src )
    {
        mSharedList = std::move(src.mSharedList);
        mDataBuffer = std::move(src.mDataBuffer);
        mDataBuffer.moveToBegin( );
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Methods
//////////////////////////////////////////////////////////////////////////
uint32_t EventDataStream::read( uint8_t* buffer, uint32_t size ) const
{
    return mDataBuffer.read(buffer, size);
}

uint32_t EventDataStream::read( areg::ByteBuffer & buffer ) const
{
    uint32_t result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal && mSharedList.isEmpty() == false)
    {
        static_cast<areg::SharedBuffer &>(buffer) = mSharedList.popFirst();
        result = buffer.getSizeUsed();
    }
    else
    {
        result = mDataBuffer.read(buffer);
    }

    return result;
}

uint32_t EventDataStream::read( areg::String & ascii ) const
{
    return mDataBuffer.read(ascii);
}

uint32_t EventDataStream::read( areg::WideString & wide ) const
{
    return mDataBuffer.read(wide);
}

void EventDataStream::resetCursor() const
{
    mDataBuffer.moveToBegin();
}

uint32_t EventDataStream::write( const uint8_t* buffer, uint32_t size )
{
    return mDataBuffer.write(buffer, size);
}

uint32_t EventDataStream::write( const areg::ByteBuffer & buffer )
{
    uint32_t result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal)
    {
        mSharedList.pushLast( static_cast<const areg::SharedBuffer &>(buffer) );
        result = buffer.getSizeUsed();
    }
    else
    {
        result = mDataBuffer.write(buffer);
    }

    return result;
}

uint32_t EventDataStream::write( const areg::String & ascii )
{
    return mDataBuffer.write(ascii);
}

uint32_t EventDataStream::write( const areg::WideString & wide )
{
    return mDataBuffer.write(wide);
}

void EventDataStream::flush()
{
}

uint32_t EventDataStream::getSizeReadable() const
{
    ASSERT(false);
    return 0;
}

uint32_t EventDataStream::getSizeWritable() const
{
    ASSERT(false);
    return 0;
}
