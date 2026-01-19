#ifndef AREG_BASE_SHAREDBUFFER_HPP
#define AREG_BASE_SHAREDBUFFER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SharedBuffer.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
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
#include "areg/base/GEGlobal.h"
#include "areg/base/BufferStreamBase.hpp"
#include "areg/base/private/BufferPosition.hpp"

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Shared Buffer uses instance of Byte Buffer and keeps track
 *          of reference counter. Every time the same Byte Buffer is used
 *          by other instance of Shared Buffer object, the reference
 *          count will be increased by one. When Shared Buffer
 *          release Byte Buffer, the reference count decreased by one.
 *          The Byte Buffer will be freed when Reference Count is zero.
 *          This mechanism prevents having multiple copy operation when
 *          binary data is passed to multiple objects.
 *          Any write operation preformed by one of Shared Buffer will have
 *          impact on data shared between different instances.
 *          The difference with Raw Buffer is that Raw Buffer object frees
 *          Byte Buffer data in destructor without checking reference count.
 *          By this, the user should prevent passing Byte Buffer object 
 *          instantiated by Raw Buffer object. Passing Byte Buffer to
 *          Raw Buffer object is allowed, since Raw Buffer will make a
 *          copy of data and any write operation will not have impact
 *          on other instances of Shared Buffer.
 *
 * \note    Shared Buffer is not thread safe. There is no data access
 *          synchronization mechanism in this class. The SharedBuffer is
 *          designed to avoid multiple copies of same data, rather than
 *          to share data between different instances of thread.
 *          The instance of Shared Buffer can be used for data streaming.
 **/
class AREG_API SharedBuffer : public  BufferStreamBase  // This is data streaming object
                            , public  IECursorPosition  // To control read and write operations
{
    friend class FileBuffer;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    explicit SharedBuffer(unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief   Reserves the space in the byte buffer to write data and sets block size.
     * \param   reserveSize Size in bytes to reserve
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer( unsigned int reserveSize, unsigned int blockSize );

    /**
     * \brief	Reserves space and writes given data into byte buffer.
     * \param	buffer      The data to initialize byte buffer
     * \param	size        The length in bytes of data
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer( const unsigned char * buffer, unsigned int size, unsigned int blockSize = NEMemory::BLOCK_SIZE );

    /**
     * \brief	Reserves requested space in bytes and writes given data into byte buffer.
     *          The reserved space should at least the size of the buffer to write.
     *          If the requested space to reserve is smaller than the size of the buffer,
     *          it will be increased to the size of the buffer.
     * \param   reserveSize Number of bytes to reserve in the shared buffer.
     * \param	buffer      The data to initialize byte buffer
     * \param	size        The length in bytes of data
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer(unsigned int reserveSize, const unsigned char* buffer, unsigned int size, unsigned int blockSize = NEMemory::BLOCK_SIZE);

    /**
     * \brief	Initialization constructor, writes given null-terminated string into byte buffer.
     *          It will write including EOS character ('\0').
     *          If given string is null, it will write only end-of-string ('\0') character.
     * \param	textString  The byte buffer as a null-terminated string
     * \param   blockSize   The size of minimum block size to increase on resize.
     *                      It is aligned to NEMemory::BLOCK_SIZE (minimum size)
     **/
    explicit SharedBuffer( const char * textString, unsigned int blockSize = NEMemory::BLOCK_SIZE );
    explicit SharedBuffer( const wchar_t * textString, unsigned int blockSize = NEMemory::BLOCK_SIZE );

    /**
     * \brief	Copy constructor. It does not copy data from src, it will refer to the same shared
     *          byte buffer object and will increase reference counter by one to prevent any other
     *          shared buffer object deleting used data
     * \param	src	    The source of shared buffer object instance.
     **/
    SharedBuffer(const SharedBuffer& src);

    /**
     * \brief	Moves data from given source.
     * \param	src	    The source of shared buffer to move data.
     **/
    SharedBuffer( SharedBuffer && src ) noexcept;

    /**
     * \brief   Destructor
     **/
    virtual ~SharedBuffer( void ) = default;

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
    inline bool operator == (const SharedBuffer & other) const;

    /**
     * \brief	Compares 2 instances of shared buffer and returns true if they are not equal
     * \param	other	Reference to another shared buffer object instance to compare
     * \return	Returns true if 2 objects are having different data or they are invalid. 
     **/
    inline bool operator != (const SharedBuffer & other) const;

    /**
     * \brief	Assigning operator, it does not copy source data, it increases
     *          byte buffer reference counter by one to prevent other shared
     *          object instance to delete buffer
     * \param	src	    Reference to source object
     **/
    SharedBuffer & operator = (const SharedBuffer & src);

    /**
     * \brief	Moves shared buffer data from given source.
     * \param	src	    The source to move data.
     **/
    SharedBuffer & operator = ( SharedBuffer && src ) noexcept;

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, SharedBuffer & input );

    /**
     * \brief	Friend global operator declaration to make Shared Buffer streamable.
     *          Writes the data from shared buffer to streaming
     * \param	stream	The data streaming object to write data
     * \param	output	The Shared Buffer object to write data
     * \return	Reference to Streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const SharedBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if either buffer is invalid, 
     *          or current position is equal to zero,
     *          or current position is equal to IECursorPosition::INVALID_CURSOR_POSITION.
     *          Otherwise, it returns false.
     **/
    inline bool isBeginOfBuffer( void ) const;

    /**
     * \brief   Returns true the buffer is valid and the current position
     *          is equal to used size of buffer.
     *          Otherwise, it returns false.
     **/
    inline bool isEndOfBuffer( void ) const;

    /**
     * \brief   Returns the pointer to the data buffer at current position.
     *          If position is invalid (IECursorPosition::INVALID_CURSOR_POSITION), it will return nullptr
     *          If it is a start position (IECursorPosition::START_CURSOR_POSITION), it will return pointer to complete buffer
     *          If it is an end of position, it will return nullptr
     *          In other cases it will return (GetBuffer() + GetCursorPosition())
     **/
    const unsigned char* getBufferAtCurrentPosition( void ) const;

    /**
     * \brief   Returns the size of block set in the constructor.
     **/
    inline unsigned int getBlockSize( void ) const;

    /**
     * \brief   Clones and returns the cloned shared buffer. The existing buffer has no change.
     **/
    SharedBuffer clone(void) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// IECursorPosition overrides
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
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int setPosition( int offset, IECursorPosition::eCursorPosition startAt ) const override;

/************************************************************************/
// IEByteBuffer interface overrides, not implemented in BufferStreamBase
/************************************************************************/

    /**
     * \brief   Returns true if buffer is shared between several byte buffer instances.
     **/
    virtual bool isShared( void ) const final;

    /**
     * \brief   Returns true if buffer can be shared.
     *          The Raw Buffer object should return false.
     *          The Shared Buffer object should return true.
     **/
    virtual bool canShare( void ) const final;

    /**
     * \brief   Invalidates the buffer. Removes reference, assigns to invalid buffer,
     *          invalidates writing and reading positions.
     **/
    virtual void invalidate( void ) override;

protected:
/************************************************************************/
// IEByteBuffer protected overrides
/************************************************************************/
    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual unsigned int getDataOffset( void ) const override;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    virtual unsigned int getHeaderSize( void ) const override;

    /**
     * \brief   Returns the size to align the buffer. By default it is sizeof(NEMemory::uAlign)
     **/
    virtual unsigned int getAlignedSize( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the default block size used by Shared Buffer.
     **/
    static uint32_t getDefaultBlockSize(void);

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
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    BufferPosition              mBufferPosition;

//////////////////////////////////////////////////////////////////////////
// Local function member
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to Shared Buffer.
     **/
    inline SharedBuffer & self( void );
};

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool SharedBuffer::operator == ( const SharedBuffer &other ) const
{
    return isEqual(other);
}

inline bool SharedBuffer::operator != ( const SharedBuffer &other ) const
{
    return (isEqual(other) == false);
}

inline bool SharedBuffer::isBeginOfBuffer( void ) const
{
    unsigned int curPos = getPosition();
    return ((isValid() == false) || (curPos == 0) || (curPos == IECursorPosition::INVALID_CURSOR_POSITION));
}

inline bool SharedBuffer::isEndOfBuffer( void ) const
{
    return ( isValid() && (getPosition() == getSizeUsed()) );
}

inline unsigned int SharedBuffer::getBlockSize(void) const
{
    return mBlockSize;
}

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const IEInStream & operator >> (const IEInStream & stream, SharedBuffer & input)
{
    if ( static_cast<const IEInStream *>(&stream) != static_cast<const IEInStream *>(&input) )
    {
        stream.read(input);
        input.moveToBegin();
    }

    return stream;
}

inline IEOutStream & operator << (IEOutStream & stream, const SharedBuffer & output)
{
    if ( static_cast<const IEOutStream *>(&stream) != static_cast<const IEOutStream *>(&output) )
    {
        stream.write( output );
        output.moveToBegin();
    }

    return stream;
}

#endif  // AREG_BASE_SHAREDBUFFER_HPP
