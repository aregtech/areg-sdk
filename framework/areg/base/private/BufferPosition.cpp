/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/BufferPosition.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, cursor position interface
 *
 ************************************************************************/
#include "areg/base/private/BufferPosition.hpp"
#include "areg/base/IEByteBuffer.hpp"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////
// BufferPosition class implementation
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
BufferPosition::BufferPosition( IEByteBuffer & buffer )
    : mBuffer           ( buffer )
    , mPosition         ( IECursorPosition::INVALID_CURSOR_POSITION )
{
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
    {
        return (mPosition == IECursorPosition::INVALID_CURSOR_POSITION ? 0 : mPosition);
    }
    else
    {
        return IECursorPosition::INVALID_CURSOR_POSITION;
    }
}

/**
 * \brief   Sets the current position of cursor
 **/
unsigned int BufferPosition::setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{
    if (mBuffer.isValid() == false)
    {
        return IECursorPosition::INVALID_CURSOR_POSITION;
    }

    int size{ static_cast<int>(mBuffer.getSizeUsed()) };
    int curPos{ static_cast<int>(mPosition == IECursorPosition::INVALID_CURSOR_POSITION ? 0 : mPosition) };

    switch (startAt)
    {
    case IECursorPosition::eCursorPosition::PositionBegin:
        curPos = 0;
        offset = offset < 0 ? 0 : std::min(offset, size);
        break;

    case IECursorPosition::eCursorPosition::PositionCurrent:
        offset = offset < 0 ? std::max(offset, -1 * curPos) : std::min(offset, size - curPos);
        break;

    case IECursorPosition::eCursorPosition::PositionEnd:
        curPos = size;
        offset = offset < 0 ? std::max(offset, -1 * size) : 0;
        break;

    default:
        ASSERT(false);
    }

    mPosition = static_cast<unsigned int>(curPos + offset);
    return mPosition;
}
