#ifndef AREG_BASE_BUFFERSTREAMBASE_HPP
#define AREG_BASE_BUFFERSTREAMBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/BufferStreamBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform base class of streaming buffer.
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
 * \brief       Base class of streaming buffer for data streaming into the binary buffer.
 *              This class is pure virtual and cannot be instantiated directly.
 *              The streaming buffer is used to transfer data between threads
 *              and processes. Depending on child class behavior, it either appends
 *              data starting from current writing position, or writes data from the 
 *              beginning of buffer.
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
     * \brief	Requires instances of byte-buffer, write and read position objects.
     * \param	readPosition	Read position object.
     * \param	writePosition	Write position object.
     **/
    BufferStreamBase( IECursorPosition & readPosition, IECursorPosition & writePosition );
    /**
     * \brief	Destructor
     **/
    virtual ~BufferStreamBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:

/************************************************************************/
// BufferStreamBase interface
/************************************************************************/
    /**
     * \brief   Inserts buffer of data at the given position and returns
     *          size of inserted data
     * \param   buffer  The pointer to data buffer to insert
     * \param   size    The size of data buffer
     * \param   atPos   The position to insert data. If position is
     *                      not valid, it will add at the end
     * \return  Returns size in bytes of written data.
     **/
    virtual unsigned int insertAt(const unsigned char* buffer, unsigned int size, unsigned int atPos);

    /**
     * \brief	Returns true if binary data of 2 buffers are equal
     * \param	other	The streaming buffer object to compare data.
     * \return	Returns true if binary values of data in 2 buffers are equal.
     **/
    virtual bool isEqual(const BufferStreamBase &other) const;

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
    virtual unsigned int write( const unsigned char* buffer, unsigned int size ) override;

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer ) override;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & asciiString ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wideString ) override;

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
    virtual unsigned int read( unsigned char* buffer, unsigned int size ) const override;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const override;

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
     * \param	size	The size in bytes to reserve
     * \param	copy    If true and the existing buffer is valid, it will copy data
     * \return	Returns the size available to use (i.e. remaining space).
     **/
    virtual unsigned int reserve(unsigned int size, bool copy) override;

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
    virtual unsigned int getSizeReadable( void ) const override;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const override;

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function if device has caching mechanism
     **/
    virtual void flush( void ) override;

    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void resetCursor( void ) const override;

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
     *          Returns nullptr if buffer is not valid or read position is at the end of buffer.
     **/
    const unsigned char * getBufferToRead( void ) const;

    /**
     * \brief   Returns pointer to the buffer at current write position.
     *          Returns nullptr if buffer is not valid or read position is at the end of buffer.
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
// Hidden / forbidden function calls
//////////////////////////////////////////////////////////////////////////
private:
    BufferStreamBase( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( BufferStreamBase );
};

#endif  // AREG_BASE_BUFFERSTREAMBASE_HPP
