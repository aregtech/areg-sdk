#ifndef AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
#define AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/BufferPosition.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, buffer cursor position interface.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IECursorPosition.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEByteBuffer;

//////////////////////////////////////////////////////////////////////////
// BufferPosition class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is defining Buffer cursor position and contains
 *          implementation of simple cursor move functionalities.
 *          The object is used in buffer classes.
 **/
class AREG_API BufferPosition  : public IECursorPosition
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Sets the instance of byte buffer object
     * \param	buffer	Instance of Byte Buffer object
     **/
    BufferPosition( IEByteBuffer & buffer );

    /**
     * \brief   Destructor
     **/
    virtual ~BufferPosition( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Invalidates current position, i.e. sets current position to IECursorPosition::INVALID_CURSOR_POSITION
     **/
    inline void invalidate( void );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IECursorPosition interface overrides
/************************************************************************/

    /**
     * \brief	Returns the current position of pointer relative to begin in streaming data.
     *          The valid position should not be equal to INVALID_CURSOR_POSITION.
     *          Check current position validation before accessing data in streaming object.
     * \return	Returns the current position of pointer relative to begin in streaming data.
     **/
    virtual unsigned int getPosition( void ) const override;

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
     * \return	If succeeds, returns the current position of pointer in bytes or value IECursorPosition::INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Reference to the Byte Buffer object
     **/
    IEByteBuffer &         mBuffer;

    /**
     * \brief   Current position of Byte Buffer cursor.
     *          Value IECursorPosition::INVALID_CURSOR_POSITION means invalid position.
     **/
    mutable unsigned int    mPosition;

//////////////////////////////////////////////////////////////////////////
// Hidden / Disabled methods
//////////////////////////////////////////////////////////////////////////
private:
    BufferPosition( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( BufferPosition );
};

//////////////////////////////////////////////////////////////////////////
// BufferPosition class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void BufferPosition::invalidate( void )
{
    mPosition   = IECursorPosition::INVALID_CURSOR_POSITION;
}

#endif  // AREG_BASE_PRIVATE_BUFFERPOSITION_HPP
