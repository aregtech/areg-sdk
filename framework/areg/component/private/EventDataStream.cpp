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
    static const EventDataStream _data(EventDataStream::EventDataKind::Empty, String("EmptyData"));
    return _data;
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventDataStream::EventDataStream( EventDataStream::EventDataKind evetDataType, const String & name /*= String::getEmptyString()*/ )
    : IOStream      ( )

    , mEventDataType(evetDataType)
    , mBufferName   (name.isEmpty() == false ? name : DefaultStreamName)
    , mDataBuffer   ( )
    , mSharedList   ( )
{
}

EventDataStream::EventDataStream( const EventDataStream & buffer, const String & name )
    : IOStream    ( )

    , mEventDataType(buffer.mEventDataType)
    , mBufferName   (name.isEmpty() == false ? name : DefaultStreamName)
    , mDataBuffer   (buffer.mDataBuffer)
    , mSharedList   (buffer.mSharedList)
{
    mDataBuffer.moveToBegin();
}

EventDataStream::EventDataStream( const EventDataStream & src )
    : IOStream    ( )

    , mEventDataType(src.mEventDataType)
    , mBufferName   (src.mBufferName)
    , mDataBuffer   (src.mDataBuffer)
    , mSharedList   (src.mSharedList)
{
}

EventDataStream::EventDataStream( EventDataStream && src ) noexcept
    : IOStream    ( )

    , mEventDataType( src.mEventDataType )
    , mBufferName   ( std::move(src.mBufferName) )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
    , mSharedList   ( std::move(src.mSharedList) )
{
}

EventDataStream::EventDataStream(const InStream & stream)
    : IOStream    ( )

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
unsigned int EventDataStream::read( unsigned char* buffer, unsigned int size ) const
{
    return mDataBuffer.read(buffer, size);
}

unsigned int EventDataStream::read( ByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal && mSharedList.isEmpty() == false)
    {
        static_cast<SharedBuffer &>(buffer) = mSharedList.popFirst();
        result = buffer.getSizeUsed();
    }
    else
    {
        result = mDataBuffer.read(buffer);
    }

    return result;
}

unsigned int EventDataStream::read( String & ascii ) const
{
    return mDataBuffer.read(ascii);
}

unsigned int EventDataStream::read( WideString & wide ) const
{
    return mDataBuffer.read(wide);
}

void EventDataStream::resetCursor() const
{
    mDataBuffer.moveToBegin();
}

unsigned int EventDataStream::write( const unsigned char* buffer, unsigned int size )
{
    return mDataBuffer.write(buffer, size);
}

unsigned int EventDataStream::write( const ByteBuffer & buffer )
{
    unsigned int result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal)
    {
        mSharedList.pushLast( static_cast<const SharedBuffer &>(buffer) );
        result = buffer.getSizeUsed();
    }
    else
    {
        result = mDataBuffer.write(buffer);
    }

    return result;
}

unsigned int EventDataStream::write( const String & ascii )
{
    return mDataBuffer.write(ascii);
}

unsigned int EventDataStream::write( const WideString & wide )
{
    return mDataBuffer.write(wide);
}

void EventDataStream::flush()
{
}

unsigned int EventDataStream::getSizeReadable() const
{
    ASSERT(false);
    return 0;
}

unsigned int EventDataStream::getSizeWritable() const
{
    ASSERT(false);
    return 0;
}
