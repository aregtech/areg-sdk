/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDataStream.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, event data stream implementation
 *
 ************************************************************************/
#include "areg/component/EventDataStream.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// EventDataStream class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, static members
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined Empty Data object.
 **/
const EventDataStream EventDataStream::EmptyData(EventDataStream::eEventData::EventDataInternal, static_cast<const char *>("EventDataStream::EmptyData"));

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventDataStream::EventDataStream( EventDataStream::eEventData evetDataType, const char* name /*= nullptr*/ )
    : IEIOStream    ( )

    , mEventDataType(evetDataType)
    , mBufferName   (name != nullptr ? name : "EventDataStream")
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    ;
}

EventDataStream::EventDataStream( const EventDataStream & buffer, const char* name )
    : IEIOStream    ( )

    , mEventDataType(buffer.mEventDataType)
    , mBufferName   (name != nullptr ? name : "EventDataStream")
    , mDataBuffer   (buffer.mDataBuffer)
    , mSharedList   (buffer.mSharedList)
{
    mDataBuffer.moveToBegin();
}

EventDataStream::EventDataStream( const EventDataStream & src )
    : IEIOStream    ( )

    , mEventDataType(src.mEventDataType)
    , mBufferName   (src.mBufferName)
    , mDataBuffer   (src.mDataBuffer)
    , mSharedList   (src.mSharedList)
{
    mDataBuffer.moveToBegin();
}

EventDataStream::EventDataStream( EventDataStream && src ) noexcept
    : IEIOStream    ( )

    , mEventDataType( src.mEventDataType )
    , mBufferName   ( std::move(src.mBufferName) )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
    , mSharedList   ( std::move(src.mSharedList) )
{
    mDataBuffer.moveToBegin( );
}

EventDataStream::EventDataStream(const IEInStream & stream)
    : IEIOStream    ( )

    , mEventDataType( EventDataStream::eEventData::EventDataExternal)
    , mBufferName   ("EventDataStream")
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    stream >> mEventDataType >> mBufferName >> mDataBuffer;
}

EventDataStream::~EventDataStream( void )
{
    mSharedList.removeAll();
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
        mBufferName = std::move(src.mBufferName);
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

unsigned int EventDataStream::read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    if (mEventDataType == EventDataStream::eEventData::EventDataInternal && mSharedList.isEmpty() == false)
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

unsigned int EventDataStream::read( String & asciiString ) const
{
    return mDataBuffer.read(asciiString);
}

unsigned int EventDataStream::read( WideString & wideString ) const
{
    return mDataBuffer.read(wideString);
}

void EventDataStream::resetCursor( void ) const
{
    mDataBuffer.moveToBegin();
}

unsigned int EventDataStream::write( const unsigned char* buffer, unsigned int size )
{
    return mDataBuffer.write(buffer, size);
}

unsigned int EventDataStream::write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;
    if (mEventDataType == EventDataStream::eEventData::EventDataInternal)
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

unsigned int EventDataStream::write( const String & asciiString )
{
    return mDataBuffer.write(asciiString);
}

unsigned int EventDataStream::write( const WideString & wideString )
{
    return mDataBuffer.write(wideString);
}

void EventDataStream::flush( void )
{
    ; // do nothing
}

unsigned int EventDataStream::getSizeReadable( void ) const
{
    ASSERT(false);
    return 0;
}

unsigned int EventDataStream::getSizeWritable( void ) const
{
    ASSERT(false);
    return 0;
}

AREG_API const IEInStream & operator >> ( const IEInStream & stream, EventDataStream & input )
{
    stream >> input.mEventDataType;
    stream >> input.mBufferName;
    stream >> input.mDataBuffer;
    return stream;
}

AREG_API IEOutStream & operator << ( IEOutStream & stream, const EventDataStream & output )
{
    ASSERT(output.mEventDataType == EventDataStream::eEventData::EventDataExternal);
    stream << EventDataStream::eEventData::EventDataExternal;
    stream << output.mBufferName;
    stream << output.mDataBuffer;
    return stream;
}
