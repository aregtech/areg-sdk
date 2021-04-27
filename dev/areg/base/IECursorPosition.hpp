#ifndef AREG_BASE_IECURSORPOSITION_HPP
#define AREG_BASE_IECURSORPOSITION_HPP
/************************************************************************
 * \file        areg/base/IECursorPosition.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Cursor Position interface.
 *              This is pure virtual class interface. It contains
 *              main Read / Write cursor position functionalities.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

/**
 * \brief       Pure virtual class. This is bases class to support
 *              cursor positioning during read / write operations.
 * 
 * \details     The class contains defined constants as cursor position.
 *              Also has defined functions such as getting and setting
 *              cursor position. Shared and Raw Buffer, File object
 *              and Ring Buffers have different logics of cursor 
 *              positioning. All of these classes either should
 *              provide individual logic or contain instance of object
 *              supporting read / write cursor positioning functions.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// IECursorPosition class declaration
//////////////////////////////////////////////////////////////////////////
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
    typedef enum E_CursorPosition
    {
          POSITION_BEGIN    = 0 //!< Set cursor position relative to begging of streaming data
        , POSITION_CURRENT      //!< Set cursor position relative to current position in streaming data
        , POSITION_END          //!< Set cursor position relative to end of streaming data

    } eCursorPosition;

    /**
     * \brief   IECursorPosition::INVALID_CURSOR_POSITION
     *          Indicator of invalid position of cursor
     **/
    static const unsigned int INVALID_CURSOR_POSITION      /*= static_cast<unsigned int>(INVALID_SET_FILE_POINTER)*/;

    /**
     * \brief   IECursorPosition::START_CURSOR_POSITION
     *          Indicator of cursor start position
     **/
    static const unsigned int START_CURSOR_POSITION         /*= static_cast<unsigned int>(0)*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected constructor
     **/
    IECursorPosition( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~IECursorPosition( void );


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
    virtual unsigned int GetPosition( void ) const = 0;

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
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int SetPosition( int offset, IECursorPosition::eCursorPosition startAt ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Move cursor position to the begin and returns 'true' if succeeded.
     *          Operation fails if moving position returns INVALID_CURSOR_POSITION
     **/
    inline bool MoveToBegin( void ) const;

    /**
     * \brief   Move cursor position to the end and returns 'true' if succeeded.
     *          Operation fails if moving position returns INVALID_CURSOR_POSITION
     **/
    inline bool MoveToEnd( void ) const;

protected:
    /**
     * \brief	Checks and returns whether current position of pointer is valid or not.
     *          The valid position must not be equal to INVALID_CURSOR_POSITION
     **/
    inline bool IsCurrentPositionValid( void ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden / Disabled methods
//////////////////////////////////////////////////////////////////////////
private:
    IECursorPosition( const IECursorPosition & /*src*/ );
    const IECursorPosition & operator = ( const IECursorPosition & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IECursorPosition pure virtual class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool IECursorPosition::IsCurrentPositionValid( void ) const
{   return (GetPosition() != IECursorPosition::INVALID_CURSOR_POSITION); }

inline bool IECursorPosition::MoveToBegin( void ) const
{   return (SetPosition(0, IECursorPosition::POSITION_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION); }

inline bool IECursorPosition::MoveToEnd( void ) const
{   return (SetPosition(0, IECursorPosition::POSITION_END)   != IECursorPosition::INVALID_CURSOR_POSITION); }

#endif  // AREG_BASE_IECURSORPOSITION_HPP
