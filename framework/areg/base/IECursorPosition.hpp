#ifndef AREG_BASE_IECURSORPOSITION_HPP
#define AREG_BASE_IECURSORPOSITION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/IECursorPosition.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Cursor Position interface.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

 //////////////////////////////////////////////////////////////////////////
 // IECursorPosition class declaration
 //////////////////////////////////////////////////////////////////////////
 /**
  * \brief   This is bases class to support cursor positioning when read / write.
  *          The class contains defined constants as cursor position and basic operations
  *          Shared and Raw Buffer, File object and Ring Buffers have different logics 
  *          of cursor position, and they should provide on logic.
  **/
class AREG_API IECursorPosition
{
//////////////////////////////////////////////////////////////////////////
// Defined constants and types
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   IECursorPosition::eCursorPosition
     *          Cursor positions, required during moving cursor position back or forward
     **/
    typedef enum class E_CursorPosition : int8_t
    {
          PositionBegin     = 0 //!< Set cursor position relative to begging of streaming data
        , PositionCurrent       //!< Set cursor position relative to current position in streaming data
        , PositionEnd           //!< Set cursor position relative to end of streaming data

    } eCursorPosition;

    /**
     * \brief   IECursorPosition::INVALID_CURSOR_POSITION
     *          Indicator of invalid position of cursor
     **/
    static constexpr unsigned int INVALID_CURSOR_POSITION   { static_cast<unsigned int>(~0) };

    /**
     * \brief   IECursorPosition::START_CURSOR_POSITION
     *          Indicator of cursor start position
     **/
    static constexpr unsigned int START_CURSOR_POSITION     { static_cast<unsigned int>(0) };

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor
     **/
    IECursorPosition( void ) = default;
    /**
     * \brief   Destructor
     **/
    virtual ~IECursorPosition( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IECursorPosition overrides
/************************************************************************/
    /**
     * \brief	Returns the current position of pointer relative to begin in streaming data.
     *          The valid position should not be equal to INVALID_CURSOR_POSITION.
     *          Check current position validation before accessing data in streaming object.
     * \return	Returns the current position of pointer relative to begin in streaming data.
     **/
    virtual unsigned int getPosition( void ) const = 0;

    /**
     * \brief	Sets the pointer position and returns current position in streaming data
     *          The positive value of offset means move pointer forward.
     *          The negative value of offset means move pointer back.
     *
     * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
     * \param	startAt	Specifies the starting position of pointer and should have one of values:
     *                  IECursorPosition::eCursorPosition::PositionBegin   -- position from the beginning of data
     *                  IECursorPosition::eCursorPosition::PositionCurrent -- position from current pointer position
     *                  IECursorPosition::eCursorPosition::PositionEnd     -- position from the end of file
     *
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Move cursor position to the begin and returns 'true' if succeeded.
     *          Operation fails if moving position returns INVALID_CURSOR_POSITION
     **/
    inline bool moveToBegin( void ) const;

    /**
     * \brief   Move cursor position to the end and returns 'true' if succeeded.
     *          Operation fails if moving position returns INVALID_CURSOR_POSITION
     **/
    inline bool moveToEnd( void ) const;

protected:
    /**
     * \brief	Checks and returns whether current position of pointer is valid or not.
     *          The valid position must not be equal to INVALID_CURSOR_POSITION
     **/
    inline bool isPositionValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden / Disabled methods
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( IECursorPosition );
};

//////////////////////////////////////////////////////////////////////////
// IECursorPosition pure virtual class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool IECursorPosition::isPositionValid( void ) const
{
    return (getPosition() != IECursorPosition::INVALID_CURSOR_POSITION);
}

inline bool IECursorPosition::moveToBegin( void ) const
{
    return (setPosition(0, IECursorPosition::eCursorPosition::PositionBegin) != IECursorPosition::INVALID_CURSOR_POSITION);
}

inline bool IECursorPosition::moveToEnd( void ) const
{
    return (setPosition(0, IECursorPosition::eCursorPosition::PositionEnd)   != IECursorPosition::INVALID_CURSOR_POSITION);
}

#endif  // AREG_BASE_IECURSORPOSITION_HPP
