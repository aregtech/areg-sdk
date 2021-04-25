#ifndef AREG_BASE_CESHAREDBUFFER_HPP
#define AREG_BASE_CESHAREDBUFFER_HPP
/************************************************************************
 * \file        areg/src/base/CESharedBuffer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Shared Buffer class
 *              This Buffer is used for Read and Write of data and
 *              it might be shared between different threads and objects.
 *              The Data in buffer remains valid until the number of references
 *              to the buffer is more than zero.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/GEGlobal.h"
#include "areg/src/base/CEBufferStreamBase.hpp"
#include "areg/src/base/private/CEBufferPosition.hpp"

/**
 * \brief       Shared Buffer class is used to transfer data without
 *              making multiple copy operations. The data in buffer 
 *              remains valid until the number of references are 
 *              more than zero.
 *              Shared Buffer is not thread safe. There is data access
 *              synchronization mechanism in this class.
 *              The instance of Shared Buffer can be used for data streaming.
 * 
 * \details     Shared Buffer uses instance of Byte Buffer and keeps track
 *              of reference counter. Every time the same Byte Buffer is used
 *              by other instance of Shared Buffer object, the reference
 *              count will be increased by one, and then Shared Buffer
 *              release Byte Buffer, the reference count decreased by one.
 *              The Byte Buffer will be freed when Reference Count reaches zero.
 *              This mechanism prevents having multiple copy operation when
 *              binary data is passed to multiple objects.
 *              Any write operation preformed by one of Shared Buffer will have
 *              impact on data shared between different instances.
 *              The difference with Raw Buffer is that Raw Buffer object frees
 *              Byte Buffer data in destructor without checking reference count.
 *              By this, the user should prevent passing Byte Buffer object 
 *              instantiated by Raw Buffer object. Passing Byte Buffer to
 *              Raw Buffer object is allowed, since Raw Buffer will make a
 *              copy of data and any write operation will not have impact
 *              on other instances of Shared Buffer.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// CESharedBuffer class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CESharedBuffer   : public    CEBufferStreamBase  // This is data streaming object
                                , protected CEBufferPosition    // To control read and write operations
{
//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CESharedBuffer(unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Constructor to reserve space for byte buffer object
     * \param   reserveSize Size in bytes to reserve
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CESharedBuffer( unsigned int reserveSize, 
                    unsigned int blockSize);

    /**
     * \brief	Initialization constructor, writes given data into byte buffer
     * \param	buffer      The data to initialize byte buffer
     * \param	size        The length in bytes of data
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CESharedBuffer( const unsigned char*    buffer, 
                          unsigned int      size, 
                          unsigned int      blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief	Initialization constructor, writes given null-terminated string into byte buffer.
     *          It will write including EOS character ('\0').
     *          If given string is null, it will write only end-of-string ('\0') character.
     * \param	textString  The byte buffer as a null-terminated string
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    CESharedBuffer( const char *      textString, 
                    unsigned int      blockSize = NEMemory::BLOCK_SIZE);
    CESharedBuffer( const wchar_t *   textString, 
                    unsigned int      blockSize = NEMemory::BLOCK_SIZE);


    /**
     * \brief	Initialization constructor. 
     *          It will not copy data, it will increase the reference counter by 1
     *          to prevent any other object deleting shared buffer object.
     *          The shared buffer should not be invalid. Otherwise it will be ignored.
     * \param	buffer	The reference to byte buffer object to share
     **/
    CESharedBuffer(NEMemory::sByteBuffer& buffer);

    /**
     * \brief	Copy constructor. It does not copy data from src, it will refer to the same shared
     *          byte buffer object and will increase reference counter by one to prevent any other
     *          shared buffer object deleting used data
     * \param	src	    The source of shared buffer object instance.
     **/
    CESharedBuffer(const CESharedBuffer& src);

    /**
     * \brief   Destructor
     **/
    virtual ~CESharedBuffer( void );

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Compares 2 instances of shared buffer and returns true if they are equal
     * \param	other	Reference to another shared buffer object instance to compare
     * \return	Returns true if 2 objects are same or have similar data. 
     *          If 2 objects are invalid, it will return false.
     **/
    inline bool operator == (const CESharedBuffer & other) const;

    /**
     * \brief	Compares 2 instances of shared buffer and returns true if they are not equal
     * \param	other	Reference to another shared buffer object instance to compare
     * \return	Returns true if 2 objects are having different data or they are invalid. 
     **/
    inline bool operator != (const CESharedBuffer & other) const;

    /**
     * \brief	Assigning operator, it does not copy source data, it increases
     *          byte buffer reference counter by one to prevent other shared
     *          object instance to delete buffer
     * \param	src	    Reference to source object
     **/
    const CESharedBuffer & operator = (const CESharedBuffer & src);

/************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief	Friend operator declaration to make Shared Buffer streamable.
     *          Writes the data from streaming object to Shared Buffer
     * \param	stream	The data streaming object to read data
     * \param	input	The Shared Buffer object to write data
     * \return	Reference to Streaming object.
     **/
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CESharedBuffer & input );

    /**
     * \brief	Friend global operator declaration to make Shared Buffer streamable.
     *          Writes the data from shared buffer to streaming
     * \param	stream	The data streaming object to write data
     * \param	input	The Shared Buffer object to write data
     * \return	Reference to Streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CESharedBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the current position of cursor (for read and write operations)
     **/
    inline unsigned int GetCursorPosition( void ) const;

    /**
     * \brief	Sets new position of cursor for read and write operations and returns
     *          the current position of cursor.
     * \param	offset	Offset in bytes to move cursor. The positive value will move
     *                  cursor position forward and negative value will move position
     *                  backward. The offset position is relative to startAt flag.
     * \param	startAt	Flag to indicate starting point to move. Possible values:
     *                  1. IECursorPosition::POSITION_BEGIN    --  offset is relative to beginning of buffer.
     *                                                              If offset is negative, cursor will be moved
     *                                                              to beginning of buffer
     *                  2. IECursorPosition::POSITION_CURRENT  --  offset is relative to current position.
     *                                                              To move cursor forward, offset should have positive value.
     *                                                              To move cursor backward, offset should have negative value.
     *                  3. IECursorPosition::POSITION_END      --  offset is relative to the end of buffer.
     *                                                              If offset is positive, cursor will move to the end of buffer.
     * \return	Current position of read / write cursor. The invalid value is IECursorPosition::INVALID_CURSOR_POSITION
     **/
    inline unsigned int SetCursorPosition(int offset, IECursorPosition::eCursorPosition startAt) const;

    /**
     * \brief   Move read / write cursor position to the beginning of buffer.
     *          If succeeded to move, returns true.
     **/
    inline bool MoveToBeginOfData( void ) const;

    /**
     * \brief   Move read / write cursor position to the end of buffer.
     *          If succeeded to move, returns true.
     **/
    inline bool MoveToEndOfData( void ) const;

    /**
     * \brief   Returns true if either buffer is invalid, 
     *          or current position is equal to zero,
     *          or current position is equal to IECursorPosition::INVALID_CURSOR_POSITION.
     *          Otherwise, it returns false.
     **/
    inline bool IsBeginOfBuffer( void ) const;

    /**
     * \brief   Returns true the buffer is valid and the current position
     *          is equal to used size of buffer.
     *          Otherwise, it returns false.
     **/
    inline bool IsEndOfBuffer( void ) const;

    /**
     * \brief   Returns the pointer to the data buffer at current position.
     *          If position is invalid (IECursorPosition::INVALID_CURSOR_POSITION), it will return NULL
     *          If it is a start position (IECursorPosition::START_CURSOR_POSITION), it will return pointer to complete buffer
     *          If it is an end of position, it will return NULL
     *          In other cases it will return (GetBuffer() + GetCursorPosition())
     **/
    const unsigned char* GetBufferAtCurrentPosition( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// IEByteBuffer interface overrides, not implemented in CEBufferStreamBase
/************************************************************************/

    /**
     * \brief   Adding reference in the byte buffer reference counter.
     *          This is valid only in case of shared buffer.
     *          In case of raw buffer, the reference counter should not be changed.
     *          The reference counter prevents that the byte buffer is deleted / freed
     *          by any other object. For example, if buffer structure is attached to 
     *          byte buffer object instance, until reference counter is not reaching
     *          to zero, it should not be deleted, since it might be used by other class
     *          The reference counter of invalid buffer should not be changed at all.
     *          To check buffer validation, call function IsValid()
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

    /**
     * \brief   Returns the size to align the buffer. By default it is sizeof(NEMemory::uAlign)
     **/
    virtual unsigned int SizeAlignment( void ) const;

/************************************************************************/
// CEBufferStreamBase interface
/************************************************************************/

    /**
     * \brief	Returns true if binary data of 2 buffers are equal
     * \param	other	The streaming buffer object to compare data.
     * \return	Returns true if binary values of data in 2 buffers are equal.
     **/
    virtual bool IsEqual(const CEBufferStreamBase &other) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Increase reference counter of buffer lock.
     **/
    void IncreaseLock( void );
    /**
     * \brief   Decrease reference counter of buffer lock
     **/
    void DecreaseLock( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The size of block to increase at once every time when need to resize.
     *          This value is a constant and cannot be changed. Set during initialization.
     **/
    const unsigned int          mBlockSize;

//////////////////////////////////////////////////////////////////////////
// Local function member
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to Shared Buffer.
     **/
    inline CESharedBuffer & self( void );
};

//////////////////////////////////////////////////////////////////////////
// CESharedBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool CESharedBuffer::operator == ( const CESharedBuffer &other ) const
{   return IsEqual(other);                                  }

inline bool CESharedBuffer::operator != ( const CESharedBuffer &other ) const
{   return (IsEqual(other) == false);                       }

inline unsigned int CESharedBuffer::GetCursorPosition( void ) const
{   return CEBufferPosition::GetPosition();                 }

inline unsigned int CESharedBuffer::SetCursorPosition( int offset, IECursorPosition::eCursorPosition startAt ) const
{   return CEBufferPosition::SetPosition(offset, startAt);  }

inline bool CESharedBuffer::MoveToBeginOfData( void ) const
{   return CEBufferPosition::MoveToBegin();                 }

inline bool CESharedBuffer::MoveToEndOfData( void ) const
{   return CEBufferPosition::MoveToEnd();                   }

inline bool CESharedBuffer::IsBeginOfBuffer( void ) const
{   unsigned int curPos = GetCursorPosition();  return (IsValid() ? curPos == 0 || curPos == IECursorPosition::INVALID_CURSOR_POSITION : true);    }

inline bool CESharedBuffer::IsEndOfBuffer( void ) const
{   return (IsValid() ? GetCursorPosition() == GetUsedSize() : false);  }

inline CESharedBuffer& CESharedBuffer::self( void )
{   return (*this);                                         }

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const IEInStream & operator >> (const IEInStream & stream, CESharedBuffer & input)
{
    if ( static_cast<const IEInStream *>(&stream) != static_cast<const IEInStream *>(&input) )
    {
        stream.Read(input);
        input.MoveToBeginOfData();
    }
    return stream;
}

inline IEOutStream & operator << (IEOutStream & stream, const CESharedBuffer & output)
{
    if ( static_cast<const IEOutStream *>(&stream) != static_cast<const IEOutStream *>(&output) )
        stream.Write(output);
    return stream;
}

#endif  // AREG_BASE_CESHAREDBUFFER_HPP
