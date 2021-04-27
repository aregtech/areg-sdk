/************************************************************************
 * \file        areg/base/private/CEBufferPosition.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, cursor position interface
 *
 ************************************************************************/
#include "areg/base/private/CEBufferPosition.hpp"
#include "areg/base/IEByteBuffer.hpp"

//////////////////////////////////////////////////////////////////////////
// CEBufferPosition class implementation
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEBufferPosition::CEBufferPosition( IEByteBuffer & buffer )
    : IECursorPosition ( )
    , mBuffer           ( buffer )
    , mPosition         ( IECursorPosition::INVALID_CURSOR_POSITION )
{
    ; // do nothing
}

CEBufferPosition::~CEBufferPosition( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Returns current position of cursor.
 **/
unsigned int CEBufferPosition::GetPosition( void ) const
{
    if ( mBuffer.IsValid() )
        return ( mPosition == IECursorPosition::INVALID_CURSOR_POSITION ? 0 : mPosition );
    else
        return IECursorPosition::INVALID_CURSOR_POSITION;
}

/**
 * \brief   Sets the current position of cursor
 **/
unsigned int CEBufferPosition::SetPosition( int offset, IECursorPosition::eCursorPosition startAt ) const 
{
    int size   = static_cast<int>( mBuffer.GetUsedSize() );
    int curPos = static_cast<int>( GetPosition() );
    if ( curPos != static_cast<int>(IECursorPosition::INVALID_CURSOR_POSITION) )
    {
        ASSERT( mBuffer.IsValid() );

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
