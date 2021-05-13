/************************************************************************
 * \file        areg/base/private/BufferPosition.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, cursor position interface
 *
 ************************************************************************/
#include "areg/base/private/BufferPosition.hpp"
#include "areg/base/IEByteBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// BufferPosition class implementation
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
BufferPosition::BufferPosition( IEByteBuffer & buffer )
    : IECursorPosition ( )
    , mBuffer           ( buffer )
    , mPosition         ( IECursorPosition::INVALID_CURSOR_POSITION )
{
    ; // do nothing
}

BufferPosition::~BufferPosition( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Returns current position of cursor.
 **/
unsigned int BufferPosition::getPosition( void ) const
{
    if ( mBuffer.isValid() )
        return ( mPosition == IECursorPosition::INVALID_CURSOR_POSITION ? 0 : mPosition );
    else
        return IECursorPosition::INVALID_CURSOR_POSITION;
}

/**
 * \brief   Sets the current position of cursor
 **/
unsigned int BufferPosition::setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const 
{
    int size   = static_cast<int>( mBuffer.getSizeUsed() );
    int curPos = static_cast<int>( getPosition() );
    if ( curPos != static_cast<int>(IECursorPosition::INVALID_CURSOR_POSITION) )
    {
        ASSERT( mBuffer.isValid() );

        switch (startAt)
        {
        case IECursorPosition::POSITION_BEGIN:
            curPos = 0;
            offset = offset < 0 ? 0 : MACRO_MIN(offset, size);
            break;

        case IECursorPosition::POSITION_CURRENT:
            offset = offset < 0 ? MACRO_MAX(offset, -1 * curPos) : MACRO_MIN(offset, size - curPos);
            break;

        case IECursorPosition::POSITION_END:
            curPos = size;
            offset = offset < 0 ? MACRO_MAX(offset, -1 * size) : 0;
            break;

        default:
            ASSERT(false);
        }
    }
    else
    {
        offset = 0;
    }
    mPosition = static_cast<unsigned int>(curPos + offset);
    return mPosition;
}
