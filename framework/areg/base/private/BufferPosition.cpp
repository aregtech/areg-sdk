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
#include "areg/base/ByteBuffer.hpp"

#include <algorithm>

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // BufferPosition class implementation
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    BufferPosition::BufferPosition( ByteBuffer & buffer )
        : mBuffer           ( buffer )
        , mPosition         ( Cursor::INVALID_CURSOR_POSITION )
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns current position of cursor.
     **/
    uint32_t BufferPosition::getPosition() const
    {
        if ( mBuffer.isValid() )
        {
            return (mPosition == Cursor::INVALID_CURSOR_POSITION ? 0 : mPosition);
        }
        else
        {
            return Cursor::INVALID_CURSOR_POSITION;
        }
    }

    /**
     * \brief   Sets the current position of cursor
     **/
    uint32_t BufferPosition::setPosition( int32_t offset, Cursor::SeekOrigin startAt ) const
    {
        if (mBuffer.isValid() == false)
        {
            return Cursor::INVALID_CURSOR_POSITION;
        }

        int32_t size{ static_cast<int32_t>(mBuffer.getSizeUsed()) };
        int32_t curPos{ static_cast<int32_t>(mPosition == Cursor::INVALID_CURSOR_POSITION ? 0 : mPosition) };

        switch (startAt)
        {
        case Cursor::SeekOrigin::Begin:
            curPos = 0;
            offset = offset < 0 ? 0 : std::min(offset, size);
            break;

        case Cursor::SeekOrigin::Current:
            offset = offset < 0 ? std::max(offset, -1 * curPos) : std::min(offset, size - curPos);
            break;

        case Cursor::SeekOrigin::End:
            curPos = size;
            offset = offset < 0 ? std::max(offset, -1 * size) : 0;
            break;

        default:
            ASSERT(false);
        }

        mPosition = static_cast<uint32_t>(curPos + offset);
        return mPosition;
    }
} // namespace areg
