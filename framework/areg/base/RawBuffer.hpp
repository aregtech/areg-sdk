#ifndef AREG_BASE_RAWBUFFER_HPP
#define AREG_BASE_RAWBUFFER_HPP
/************************************************************************
 * \file        areg/base/RawBuffer.hpp
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
#include "areg/base/GEGlobal.h"
#include "areg/base/BufferStreamBase.hpp"
#include "areg/base/private/BufferPosition.hpp"

//////////////////////////////////////////////////////////////////////////
// RawBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Use this class for single binary data object. The byte buffer is not sharable
 *          and it will be freed in destructor. Do not pass reference to Byte Buffer object 
 *          to sharing objects. Instead, pass data buffer to copy buffer. On read and write 
 *          access the object should be still valid. Every write operation, if needed, will 
 *          change the buffer size and overwrite the existing operation. 
 **/
class AREG_API RawBuffer    : public BufferStreamBase  // basic streaming class 
                            , public BufferPosition    // positioning class
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    RawBuffer( void );

    /**
     * \brief   Constructor with data initialization
     * \param   buffer      The buffer of data to initialize
     * \param   dataSize    The size of data in buffer.
     **/
    RawBuffer(const unsigned char* buffer, unsigned int dataSize);

    /**
     * \brief   Constructor to reserve space in buffer
     **/
    RawBuffer(int reservedSize);

    /**
     * \brief   Copy constructor.
     * \param   src     The source to copy data.
     **/
    RawBuffer( const RawBuffer & src );

    /**
     * \brief   Destructor
     **/
    virtual ~RawBuffer( void );

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
    const RawBuffer & operator = ( const RawBuffer & src );

    /**
     * \brief	Comparing operator. Compares 2 raw buffer objects
     *          and returns true if 2 buffer objects are equal.
     *          It compares 2 buffer byte-by-byte.
     * \param	other	Reference to raw buffer object to compare
     * \return	Returns true if 2 buffer objects are equal.
     **/
    inline bool operator == ( const RawBuffer &other ) const;

    /**
     * \brief	Comparing operator. Compares 2 raw buffer objects
     *          and returns true if 2 buffer objects are not equal.
     *          It compares 2 buffer byte-by-byte.
     * \param	other	Reference to raw buffer object to compare
     * \return	Returns true if 2 buffer objects are not equal.
     **/
    inline bool operator != ( const RawBuffer &other ) const;

protected:
/************************************************************************/
// IEByteBuffer protected overrides
/************************************************************************/
    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual unsigned int getDataOffset( void ) const;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    virtual unsigned int getHeaderSize( void ) const;

private:
/************************************************************************/
// IEByteBuffer interface overrides, hidden in raw buffer and not implemented in BufferStreamBase
/************************************************************************/

    /**
     * \brief   Invalid operation in case of Raw buffer. The reference counter
     *          of buffer should remain always 1
     **/
    virtual void addReference( void );

    /**
     * \brief   Remove reference in the byte buffer reference counter.
     *          This is valid only in case of shared buffer.
     *          In case of raw buffer, the reference counter should not be changed.
     *          If reference counter reaches zero, the buffer is not valid anymore 
     *          and should be invalidated / deleted. The buffer of Invalid Buffer object
     *          should not be deleted / freed at all. To check buffer validation,
     *          call function isValid().
     **/
    virtual void removeReference( void );

    /**
     * \brief   Returns true if buffer is shared between several byte buffer instances.
     **/
    virtual bool isShared( void ) const;

    /**
     * \brief   Returns true if buffer can be shared.
     *          The Raw Buffer object should return false.
     *          The Shared Buffer object should return true.
     **/
    virtual bool canShare( void ) const;

    /**
     * \brief   Invalidates the buffer. Removes reference, assigns to invalid buffer,
     *          invalidates writing and reading positions.
     **/
    virtual void invalidate( void );

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
    inline RawBuffer & self( void );
};

//////////////////////////////////////////////////////////////////////////
// RawBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool RawBuffer::operator == ( const RawBuffer &other ) const
{
    return isEqual(other);
}

inline bool RawBuffer::operator != ( const RawBuffer &other ) const
{
    return (isEqual(other) == false);
}

inline RawBuffer& RawBuffer::self( void )
{
    return (*this);
}

#endif  // AREG_BASE_RAWBUFFER_HPP
