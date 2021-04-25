#ifndef AREG_BASE_CERAWBUFFER_HPP
#define AREG_BASE_CERAWBUFFER_HPP
/************************************************************************
 * \file        areg/src/base/CERawBuffer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform Raw Byte Buffer class.
 *              This class is storing raw binary data. Used to store
 *              single object of fixed size.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEBufferStreamBase.hpp"
#include "areg/src/base/private/CEBufferPosition.hpp"

/**
 * \brief       This class is storing raw binary data and used to store
 *              single binary object of fixed size. This raw buffer
 *              is not used for sharing between objects. 
 *              The buffer will be freed when destructor is called.
 * 
 * \details     Every write operation, if needed, will change the buffer size
 *              and overwrite the existing operation. Use this class for
 *              single binary data object. The byte buffer is not sharable
 *              and it will be freed in destructor. Do not pass reference
 *              to Byte Buffer object to sharing objects. Instead, pass
 *              data buffer to copy buffer. On read and write access the
 *              object should be still valid.
 *
 **/

//////////////////////////////////////////////////////////////////////////
// CERawBuffer class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CERawBuffer  : public    CEBufferStreamBase  // basic streaming class 
                            , protected CEBufferPosition    // positioning class
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    CERawBuffer( void );

    /**
     * \brief   Constructor with data initialization
     * \param   buffer      The buffer of data to initialize
     * \param   dataSize    The size of data in buffer.
     **/
    CERawBuffer(const unsigned char* buffer, unsigned int dataSize);

    /**
     * \brief   Constructor to reserve space in buffer
     **/
    CERawBuffer(int reservedSize);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    CERawBuffer( const CERawBuffer & src );

    /**
     * \brief   Destructor
     **/
    virtual ~CERawBuffer( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Assigning operator. It will copy binary data
     *          from given source.
     * \param	src	    The raw binary data source to get data
     * \return	Reference to object.
     **/
    const CERawBuffer & operator = ( const CERawBuffer & src );

    /**
     * \brief	Comparing operator. Compares 2 raw buffer objects
     *          and returns true if 2 buffer objects are equal.
     *          It compares 2 buffer byte-by-byte.
     * \param	other	Reference to raw buffer object to compare
     * \return	Returns true if 2 buffer objects are equal.
     **/
    inline bool operator == ( const CERawBuffer &other ) const;

    /**
     * \brief	Comparing operator. Compares 2 raw buffer objects
     *          and returns true if 2 buffer objects are not equal.
     *          It compares 2 buffer byte-by-byte.
     * \param	other	Reference to raw buffer object to compare
     * \return	Returns true if 2 buffer objects are not equal.
     **/
    inline bool operator != ( const CERawBuffer &other ) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Returns the current position of pointer relative to begin in streaming data.
     *          The valid position should not be equal to INVALID_CURSOR_POSITION.
     *          Check current position validation before accessing data in streaming object.
     * \return	Returns the current position of pointer relative to begin in streaming data.
     **/
    inline unsigned int GetCursorPosition( void ) const;

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
    inline unsigned int SetCursorPosition( int offset, IECursorPosition::eCursorPosition startAt ) const;

    /**
     * \brief   Moves cursor to the begin of buffer and returns true if operation succeeded.
     * \return  Returns trues, if operation succeeded
     **/
    inline bool MoveToBeginOfData( void );

    /**
     * \brief   Moves cursor to the end of buffer and returns true if operation succeeded.
     * \return  Returns trues, if operation succeeded
     **/
    inline bool MoveToEndOfData( void );

protected:
/************************************************************************/
// IEByteBuffer protected overrides
/************************************************************************/
    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual int SizeDataOffset( void ) const;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    virtual int SizeBufferStruct( void ) const;

private:
/************************************************************************/
// IEByteBuffer interface overrides, hidden in raw buffer and not implemented in CEBufferStreamBase
/************************************************************************/

    /**
     * \brief   Invalid operation in case of Raw buffer. The reference counter
     *          of buffer should remain always 1
     **/
    virtual void AddReference( void );

    /**
     * \brief   Remove reference in the byte buffer reference counter.
     *          This is valid only in case of shared buffer.
     *          In case of raw buffer, the reference counter should not be changed.
     *          If reference counter reaches zero, the buffer is not valid anymore 
     *          and should be invalidated / deleted. The buffer of Invalid Buffer object
     *          should not be deleted / freed at all. To check buffer validation,
     *          call function IsValid().
     **/
    virtual void RemoveReference( void );

    /**
     * \brief   Returns true if buffer is shared between several byte buffer instances.
     **/
    virtual bool IsShared( void ) const;

    /**
     * \brief   Returns true if buffer can be shared.
     *          The Raw Buffer object should return false.
     *          The Shared Buffer object should return true.
     **/
    virtual bool IsSharable( void ) const;

    /**
     * \brief   Invalidates the buffer. Removes reference, assigns to invalid buffer,
     *          invalidates writing and reading positions.
     **/
    virtual void InvalidateBuffer( void );

//////////////////////////////////////////////////////////////////////////
// Local function member
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Delete buffer data, resets member variables
     **/
    void _deleteBuffer( void );
    /**
     * \brief   Returns reference to raw buffer object.
     **/
    inline CERawBuffer & self( void );
};

//////////////////////////////////////////////////////////////////////////
// CERawBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool CERawBuffer::operator == ( const CERawBuffer &other ) const
{   return IsEqual(other);                  }

inline bool CERawBuffer::operator != ( const CERawBuffer &other ) const
{   return (IsEqual(other) == false);       }

inline unsigned int CERawBuffer::GetCursorPosition( void ) const
{   return CEBufferPosition::GetPosition(); }

inline unsigned int CERawBuffer::SetCursorPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{   return  CEBufferPosition::SetPosition(offset, startAt); }

inline bool CERawBuffer::MoveToBeginOfData( void )
{   return CEBufferPosition::MoveToBegin(); }

inline bool CERawBuffer::MoveToEndOfData( void )
{   return CEBufferPosition::MoveToEnd();   }

inline CERawBuffer& CERawBuffer::self( void )
{   return (*this); }

#endif  // AREG_BASE_CERAWBUFFER_HPP
