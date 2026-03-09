#ifndef AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
#define AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/BufferPosition.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, buffer cursor position interface.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Cursor.hpp"
namespace areg {

/************************************************************************
 * Dependencies
 ************************************************************************/
class ByteBuffer;

//////////////////////////////////////////////////////////////////////////
// BufferPosition class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Defines and implements buffer cursor positioning and movement functionality.
 **/
class AREG_API BufferPosition
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the cursor for the specified ByteBuffer.
     *
     * \param   buffer      The ByteBuffer instance to track.
     **/
    BufferPosition( ByteBuffer & buffer );

    virtual ~BufferPosition() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Invalidates the current position by setting it to INVALID_CURSOR_POSITION.
     **/
    inline void invalidate() noexcept;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Cursor interface overrides
/************************************************************************/

    /**
     * \brief   Returns the current position relative to the beginning of the buffer. Use
     *          is_position_valid() to check validity.
     **/
    [[nodiscard]]
    uint32_t position() const noexcept;

    /**
     * \brief   Moves the cursor by the specified offset relative to the starting origin.
     *
     * \param   offset      The offset in bytes to move. Positive moves forward; negative moves
     *                      backward.
     * \param   startAt     The starting position: Begin, Current, or End.
     * \return  Returns the new cursor position in bytes; INVALID_CURSOR_POSITION if the operation
     *          fails.
     **/
    uint32_t set_position( int32_t offset, Cursor::SeekOrigin startAt ) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ByteBuffer &        mBuffer;    //!< Reference to the tracked ByteBuffer.
    mutable uint32_t    mPosition;  //!< Current cursor position (INVALID_CURSOR_POSITION if invalid).

//////////////////////////////////////////////////////////////////////////
// Hidden / Disabled methods
//////////////////////////////////////////////////////////////////////////
private:
    BufferPosition() = delete;
    AREG_NOCOPY_NOMOVE( BufferPosition );
};

//////////////////////////////////////////////////////////////////////////
// BufferPosition class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void BufferPosition::invalidate()  noexcept
{
    mPosition   = Cursor::INVALID_CURSOR_POSITION;
}

} // namespace areg
#endif  // AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
