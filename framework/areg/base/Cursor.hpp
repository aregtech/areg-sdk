#ifndef AREG_BASE_CURSOR_HPP
#define AREG_BASE_CURSOR_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/Cursor.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Cursor Position interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <limits>

namespace areg {

 //////////////////////////////////////////////////////////////////////////
 // Cursor class declaration
 //////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for cursor positioning during read/write operations. Supports different
 *          cursor positioning logic for shared buffers, raw buffers, files, and ring buffers.
 **/
class AREG_API Cursor
{
//////////////////////////////////////////////////////////////////////////
// Defined constants and types
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Cursor::SeekOrigin
     *          Cursor positions, required during moving cursor position back or forward
     **/
    enum class SeekOrigin   : int8_t
    {
          Begin     = 0 //!< Set cursor position relative to begging of streaming data
        , Current       //!< Set cursor position relative to current position in streaming data
        , End           //!< Set cursor position relative to end of streaming data

    };

    /**
     * \brief   Indicator values for invalid and start cursor positions.
     **/
    enum : uint32_t
    {
          INVALID_CURSOR_POSITION   = std::numeric_limits<uint32_t>::max()
        , START_CURSOR_POSITION     = 0u
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    Cursor() = default;
    virtual ~Cursor() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Cursor overrides
/************************************************************************/
    /**
     * \brief   Returns the current position relative to the beginning of streaming data. Use
     *          is_position_valid() to check validity before accessing data.
     *
     * \return  Returns the current cursor position in bytes.
     **/
    [[nodiscard]]
    virtual uint32_t position() const noexcept = 0;

    /**
     * \brief   Moves the cursor position by the specified offset relative to the starting origin.
     *
     * \param   offset      The offset in bytes to move. Positive moves forward; negative moves
     *                      backward.
     * \param   startAt     The starting position: Begin (start of data), Current (current
     *                      position), or End (end of data).
     * \return  Returns the new cursor position in bytes; INVALID_CURSOR_POSITION if the operation
     *          fails.
     **/
    virtual uint32_t set_position( int32_t offset, Cursor::SeekOrigin startAt ) const noexcept = 0;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Moves cursor to the beginning of data.
     **/
    inline bool move_to_begin() const noexcept;

    /**
     * \brief   Moves cursor to the end of data.
     **/
    inline bool move_to_end() const noexcept;

protected:
    /**
     * \brief   Returns true if the current cursor position is valid (not INVALID_CURSOR_POSITION).
     **/
    [[nodiscard]]
    inline bool is_position_valid() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hidden / Disabled methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( Cursor );
};

//////////////////////////////////////////////////////////////////////////
// Cursor pure virtual class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool Cursor::is_position_valid() const noexcept
{
    return (position() != Cursor::INVALID_CURSOR_POSITION);
}

inline bool Cursor::move_to_begin() const noexcept
{
    return (set_position(0, Cursor::SeekOrigin::Begin) != Cursor::INVALID_CURSOR_POSITION);
}

inline bool Cursor::move_to_end() const noexcept
{
    return (set_position(0, Cursor::SeekOrigin::End)   != Cursor::INVALID_CURSOR_POSITION);
}

} // namespace areg
#endif  // AREG_BASE_CURSOR_HPP
