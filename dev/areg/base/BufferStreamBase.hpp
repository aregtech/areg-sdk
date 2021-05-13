#ifndef AREG_BASE_BUFFERSTREAMBASE_HPP
#define AREG_BASE_BUFFERSTREAMBASE_HPP

/************************************************************************
 * \file        areg/base/BufferStreamBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform base class of streaming buffer.
 *              This is pure virtual class and cannot be instantiated directly.
 *              Use one of child classes to instantiate object.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEByteBuffer.hpp"
#include "areg/base/IEIOStream.hpp"

#include "areg/base/private/ReadConverter.hpp"
#include "areg/base/private/WriteConverter.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IECursorPosition;

//////////////////////////////////////////////////////////////////////////
// BufferStreamBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Base class of streaming buffer. It is pure virtual.
 * 
 * \details     This is base class for data streaming into the binary buffer.
 *              This class is pure virtual and cannot be instantiated directly.
 *              Instead use one of child classes.
 *              The streaming buffer is used to transfer data between threads
 *              and processes. Depending on child class behavior, it will
 *              automatically either append data starting from current
 *              writing position, or it will write data from beginning
 *              of buffer. For more details see child classes.
 *
 **/
class AREG_API BufferStreamBase : public    IEByteBuffer
                                , public    IEIOStream
{
//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor. Do not call directly to instantiate object
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief	Initialization constructor. Requires instantiated write and
     *          read position objects.
     * \param	readPosition	Read position object
     * \param	writePosition	Write position object
     **/
    BufferStreamBase( NEMemory::sByteBuffer & byteBuffer, IECursorPosition & readPosition, IECursorPosition & writePosition );
    /**
     * \brief	Destructor
     **/
    virtual ~BufferStreamBase( void );

//////////////////////////////////////////////////////////////////////////
// operators
//////////////////////////////////////////////////////////////////////////
public:

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/
    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. 
     *          Implement function to provide writing functionality 
     *          of output stream object.
     * \param	buffer	The pointer to buffer to read data and 
     *                  copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char* buffer, unsigned int size );

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer );

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & asciiString );

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wideString );

/************************************************************************/
// IEInStream interface overrides
/************************************************************************/

    /**
     * \brief	Read data from input stream object, copies into given buffer and
     *          returns the size of copied data. Implement the function to provide
     *          reading functionality of input streaming object.
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( unsigned char* buffer, unsigned int size ) const;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const;

    /**
     * \brief   Inserts buffer of data at the given position and returns
     *          size of inserted data
     * \param   buffer      The pointer to data buffer to insert
     * \param   size        The size of data buffer
     * \param   insertAt    The position to insert data. If position is
     *                      not valid, it will add at the end
     * \return  Returns size in bytes of written data.
     **/
    virtual unsigned int insertAt(const unsigned char* buffer, unsigned int size, unsigned int insertAt);

/************************************************************************/
// IEByteBuffer overrides
/************************************************************************/
    /**
     * \brief	Reserves space for byte buffer structure, if needed, 
     *          copies existing data and the size of buffer available to write.
     *          1.  If requested reserved space is more than the length of data buffer, 
     *              the size of data buffer is increased.
     *          2.  If total data buffer spaces is more than IEByteBuffer::MAX_BUF_LENGTH,
     *              reserves the maximum IEByteBuffer::MAX_BUF_LENGTH.
     *          3.  If size is zero, calls removeReference() to free all space.
     *          4.  If buffer is shared (i.e. reference count is more than 1)
     *              the operation is ignored and function returns total remaining space
     *              to write data. Because if data is shared between different instances
     *              of byte-buffer, the size of buffer should not be changed.
     *
     * \param	size	Size in bytes to reserve
     * \param	copy    If true and the existing buffer is valid, it will copy data
     * \return	Returns the size available to use (i.e. remaining space).
     **/
    virtual unsigned int resize(unsigned int size, bool copy);

protected:
/************************************************************************/
// IEIOStream interface overrides
/************************************************************************/
    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int getSizeReadable( void ) const;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const;

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function if device has caching mechanism
     **/
    virtual void flush( void );

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void resetCursor( void ) const;

/************************************************************************/
// BufferStreamBase interface
/************************************************************************/
    /**
     * \brief	Returns true if binary data of 2 buffers are equal
     * \param	other	The streaming buffer object to compare data.
     * \return	Returns true if binary values of data in 2 buffers are equal.
     **/
    virtual bool isEqual(const BufferStreamBase &other) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief	Writes data to buffer and returns the size of written data. 
     * \param	buffer	The pointer to data source buffer
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    unsigned int writeData(const unsigned char* buffer, unsigned int size);

    /**
     * \brief	Reads data from buffer and returns the size of copied data.
     * \param	buffer	The pointer of destination buffer to copy data
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    unsigned int readData(unsigned char* buffer, unsigned int size) const;

    /**
     * \brief   Returns pointer to the buffer at current read position.
     *          Returns NULL if buffer is not valid or read position is at the end of buffer.
     **/
    const unsigned char * getBufferToRead( void ) const;

    /**
     * \brief   Returns pointer to the buffer at current write position.
     *          Returns NULL if buffer is not valid or read position is at the end of buffer.
     **/
    unsigned char * getBufferToWrite( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Read position object
     **/
    IECursorPosition &  mReadPosition;
    /**
     * \brief   Write position object
     **/
    IECursorPosition &  mWritePosition;

//////////////////////////////////////////////////////////////////////////
// Local member functions
//////////////////////////////////////////////////////////////////////////
private:
    inline BufferStreamBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    BufferStreamBase( void );
    BufferStreamBase( const BufferStreamBase & /*src*/ );
    const BufferStreamBase & operator = ( const BufferStreamBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// inline functions of BufferStreamBase class.
//////////////////////////////////////////////////////////////////////////
inline BufferStreamBase & BufferStreamBase::self( void )
{   return (*this); }

#endif  // AREG_BASE_BUFFERSTREAMBASE_HPP
