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
#include "areg/base/GEGlobal.h"
#include "areg/base/Cursor.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class ByteBuffer; }

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // BufferPosition class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This class is defining Buffer cursor position and contains
     *          implementation of simple cursor move functionalities.
     *          The object is used in buffer classes.
     **/
    class AREG_API BufferPosition
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief	Sets the instance of byte buffer object
         * \param	buffer	Instance of Byte Buffer object
         **/
        BufferPosition( areg::ByteBuffer & buffer );

        /**
         * \brief   Destructor
         **/
        virtual ~BufferPosition() = default;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Invalidates current position, i.e. sets current position to Cursor::INVALID_CURSOR_POSITION
         **/
        inline void invalidate();

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Cursor interface overrides
    /************************************************************************/

        /**
         * \brief	Returns the current position of pointer relative to begin in streaming data.
         *          The valid position should not be equal to INVALID_CURSOR_POSITION.
         *          Check current position validation before accessing data in streaming object.
         * \return	Returns the current position of pointer relative to begin in streaming data.
         **/
        uint32_t getPosition() const;

        /**
         * \brief	Sets the pointer position and returns current position in streaming data
         *          The positive value of offset means move pointer forward.
         *          The negative value of offset means move pointer back.
         *
         * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
         * \param	startAt	Specifies the starting position of pointer and should have one of values:
         *                  Cursor::SeekOrigin::Begin   -- position from the beginning of data
         *                  Cursor::SeekOrigin::Current -- position from current pointer position
         *                  Cursor::SeekOrigin::End     -- position from the end of file
         *
         * \return	If succeeds, returns the current position of pointer in bytes or value Cursor::INVALID_CURSOR_POSITION if fails.
         **/
        uint32_t setPosition( int32_t offset, areg::Cursor::SeekOrigin startAt ) const;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Reference to the Byte Buffer object
         **/
        areg::ByteBuffer &         mBuffer;

        /**
         * \brief   Current position of Byte Buffer cursor.
         *          Value Cursor::INVALID_CURSOR_POSITION means invalid position.
         **/
        mutable uint32_t    mPosition;

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

    inline void BufferPosition::invalidate()
    {
        mPosition   = areg::Cursor::INVALID_CURSOR_POSITION;
    }

} // namespace areg
#endif  // AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
