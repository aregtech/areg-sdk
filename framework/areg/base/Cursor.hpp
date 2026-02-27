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
#include "areg/base/GEGlobal.h"

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // Cursor class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   This is bases class to support cursor positioning when read / write.
     *          The class contains defined constants as cursor position and basic operations
     *          Shared and Raw Buffer, File object and Ring Buffers have different logics 
     *          of cursor position, and they should provide on logic.
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
         * \brief   Cursor::INVALID_CURSOR_POSITION
         *          Indicator of invalid position of cursor
         **/
        static constexpr uint32_t INVALID_CURSOR_POSITION   { static_cast<uint32_t>(~0) };

        /**
         * \brief   Cursor::START_CURSOR_POSITION
         *          Indicator of cursor start position
         **/
        static constexpr uint32_t START_CURSOR_POSITION     { static_cast<uint32_t>(0) };

    //////////////////////////////////////////////////////////////////////////
    // Constructor / destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Protected constructor
         **/
        Cursor() = default;
        /**
         * \brief   Destructor
         **/
        virtual ~Cursor() = default;

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Cursor overrides
    /************************************************************************/
        /**
         * \brief	Returns the current position of pointer relative to begin in streaming data.
         *          The valid position should not be equal to INVALID_CURSOR_POSITION.
         *          Check current position validation before accessing data in streaming object.
         * \return	Returns the current position of pointer relative to begin in streaming data.
         **/
        virtual uint32_t getPosition() const = 0;

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
         * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_CURSOR_POSITION if fails.
         **/
        virtual uint32_t setPosition( int32_t offset, Cursor::SeekOrigin startAt ) const = 0;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Move cursor position to the begin and returns 'true' if succeeded.
         *          Operation fails if moving position returns INVALID_CURSOR_POSITION
         **/
        inline bool moveToBegin() const;

        /**
         * \brief   Move cursor position to the end and returns 'true' if succeeded.
         *          Operation fails if moving position returns INVALID_CURSOR_POSITION
         **/
        inline bool moveToEnd() const;

    protected:
        /**
         * \brief	Checks and returns whether current position of pointer is valid or not.
         *          The valid position must not be equal to INVALID_CURSOR_POSITION
         **/
        inline bool isPositionValid() const;

    //////////////////////////////////////////////////////////////////////////
    // Hidden / Disabled methods
    //////////////////////////////////////////////////////////////////////////
    private:
        AREG_NOCOPY_NOMOVE( Cursor );
    };

    //////////////////////////////////////////////////////////////////////////
    // Cursor pure virtual class inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline bool Cursor::isPositionValid() const
    {
        return (getPosition() != Cursor::INVALID_CURSOR_POSITION);
    }

    inline bool Cursor::moveToBegin() const
    {
        return (setPosition(0, Cursor::SeekOrigin::Begin) != Cursor::INVALID_CURSOR_POSITION);
    }

    inline bool Cursor::moveToEnd() const
    {
        return (setPosition(0, Cursor::SeekOrigin::End)   != Cursor::INVALID_CURSOR_POSITION);
    }

} // namespace areg
#endif  // AREG_BASE_CURSOR_HPP
