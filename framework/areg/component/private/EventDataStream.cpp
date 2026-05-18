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

namespace areg {

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, static members
//////////////////////////////////////////////////////////////////////////

const EventDataStream& EventDataStream::empty_data() noexcept
{
    static const EventDataStream _data(EventDataStream::EventDataKind::Empty);
    return _data;
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
EventDataStream::EventDataStream( EventDataStream::EventDataKind evetDataType )
    : IOStream      ( )

    , mEventDataType(evetDataType)
    , mDataBuffer   ( )
    , mSharedList   ( )
{
}

EventDataStream::EventDataStream( const EventDataStream & src )
    : IOStream    ( )

    , mEventDataType(src.mEventDataType)
    , mDataBuffer   (src.mDataBuffer)
    , mSharedList   (src.mSharedList)
{
}

EventDataStream::EventDataStream( EventDataStream && src ) noexcept
    : IOStream    ( )

    , mEventDataType( src.mEventDataType )
    , mDataBuffer   ( std::move(src.mDataBuffer) )
    , mSharedList   ( std::move(src.mSharedList) )
{
}

EventDataStream::EventDataStream(const InStream & stream)
    : IOStream    ( )

    , mEventDataType( EventDataStream::EventDataKind::External)
    , mDataBuffer   ( )
    , mSharedList   ( )
{
    stream >> mEventDataType >> mDataBuffer;
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
        mDataBuffer.move_to_begin();
    }

    return (*this);
}

EventDataStream & EventDataStream::operator = ( EventDataStream && src ) noexcept
{
    if ( static_cast<EventDataStream *>(this) != &src )
    {
        mSharedList = std::move(src.mSharedList);
        mDataBuffer = std::move(src.mDataBuffer);
        mDataBuffer.move_to_begin( );
    }

    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// EventDataStream class, Methods
//////////////////////////////////////////////////////////////////////////
uint32_t EventDataStream::read( uint8_t* buffer, uint32_t size ) const noexcept
{
    return mDataBuffer.read(buffer, size);
}

uint32_t EventDataStream::read(SharedBuffer& buffer ) const
{
    uint32_t result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal && mSharedList.is_empty() == false)
    {
        static_cast<SharedBuffer &>(buffer) = mSharedList.pop_first();
        result = buffer.size_used();
    }
    else
    {
        result = mDataBuffer.read(buffer);
    }

    return result;
}

uint32_t EventDataStream::read( String & ascii ) const
{
    return mDataBuffer.read(ascii);
}

uint32_t EventDataStream::read( WideString & wide ) const
{
    return mDataBuffer.read(wide);
}

void EventDataStream::reset() const noexcept
{
    mDataBuffer.move_to_begin();
}

uint32_t EventDataStream::write( const uint8_t* buffer, uint32_t size )
{
    return mDataBuffer.write(buffer, size);
}

uint32_t EventDataStream::write( const SharedBuffer& buffer )
{
    uint32_t result = 0;
    if (mEventDataType == EventDataStream::EventDataKind::Internal)
    {
        mSharedList.push_last( static_cast<const SharedBuffer &>(buffer) );
        result = buffer.size_used();
    }
    else
    {
        result = mDataBuffer.write(buffer);
    }

    return result;
}

uint32_t EventDataStream::write( const String & ascii )
{
    return mDataBuffer.write(ascii);
}

uint32_t EventDataStream::write( const WideString & wide )
{
    return mDataBuffer.write(wide);
}

void EventDataStream::flush() noexcept
{
}

uint32_t EventDataStream::size_readable() const noexcept
{
    ASSERT(false);
    return 0;
}

uint32_t EventDataStream::size_writable() const noexcept
{
    ASSERT(false);
    return 0;
}

} // namespace areg
