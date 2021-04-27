#ifndef AREG_BASE_PRIVATE_CEBUFFERPOSITION_HPP
#define AREG_BASE_PRIVATE_CEBUFFERPOSITION_HPP
/************************************************************************
 * \file        areg/base/private/CEBufferPosition.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, buffer cursor position interface
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
// CEBufferPosition class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is defining Buffer cursor position and contains
 *          implementation of simple cursor move functionalities.
 *          The object is used in buffer classes.
 **/
class AREG_API CEBufferPosition  : public IECursorPosition
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Available constructor
     * \param	buffer	Reference to Byte Buffer object
     **/
    CEBufferPosition( IEByteBuffer & buffer );

    /**
     * \brief   Destructor
     **/
    virtual ~CEBufferPosition( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Invalidates current position, 
     *          i.e. sets current position to IECursorPosition::INVALID_CURSOR_POSITION
     **/
    inline void InvalidatePosition( void );

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
    virtual unsigned int GetPosition( void ) const;

    /**
     * \brief	Sets the pointer position and returns current position in streaming data
     *          The positive value of offset means move pointer forward.
     *          The negative value of offset means move pointer back.
     *
     * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
     * \param	startAt	Specifies the starting position of pointer and should have one of values:
     *                  IECursorPosition::POSITION_BEGIN   -- position from the beginning of data
     *                  IECursorPosition::POSITION_CURRENT -- position from current pointer position
     *                  IECursorPosition::POSITION_END     -- position from the end of file
     *
     * \return	If succeeds, returns the current position of pointer in bytes or value IECursorPosition::INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int SetPosition( int offset, IECursorPosition::eCursorPosition startAt ) const;

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
    CEBufferPosition( void );
    CEBufferPosition(const CEBufferPosition & /*src*/ );
    const CEBufferPosition& operator = (const CEBufferPosition & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEBufferPosition class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline void CEBufferPosition::InvalidatePosition( void )
{   mPosition   = IECursorPosition::INVALID_CURSOR_POSITION;   }

#endif  // AREG_BASE_PRIVATE_CEBUFFERPOSITION_HPP
