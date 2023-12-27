#ifndef AREG_BASE_IEBYTEBUFFER_HPP
#define AREG_BASE_IEBYTEBUFFER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/IEByteBuffer.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Byte Buffer interface.
 *              This is base class for classes supporting functionalities
 *              to control binary (Byte) buffer. Following classes are
 *              derived from this class: SharedBuffer, FileBuffer, etc.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEMemory.hpp"
#include "areg/base/IECursorPosition.hpp"

#include <memory>

//////////////////////////////////////////////////////////////////////////
// IEByteBuffer pure virtual class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Binary (Byte) buffer bases class as a container of binary data.
 **/
class AREG_API IEByteBuffer
{
    friend class BufferStreamBase;

//////////////////////////////////////////////////////////////////////////
// Defined static constants and types
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Shared pointer allocator / deleter.
     **/
    using ByteBufferDeleter     = NEMemory::BufferDeleter<NEMemory::sByteBuffer>;

    /**
     * \brief   IEByteBuffer::MAX_BUF_LENGTH
     *          Maximum length of byte buffer. It is defined as 64 Mb.
     **/
    static constexpr unsigned int   MAX_BUF_LENGTH  { 0x04000000u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor.
     **/
    IEByteBuffer( void );

    /**
     * \brief   Initializes byte-buffer from given source
     **/
    IEByteBuffer( NEMemory::sByteBuffer & byteBuffer );

    /**
     * \brief   Moves byte-buffer from given source
     **/
    IEByteBuffer( IEByteBuffer && src ) noexcept;

public:
    /**
     * \brief   Destructor 
     **/
    virtual ~IEByteBuffer( void ) = default;

    /**
     * \brief   Moves data from given source
     **/

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IEByteBuffer overrides
/************************************************************************/
    /**
     * \brief   Returns true if buffer is shared between several byte buffer instances.
     **/
    virtual bool isShared( void ) const = 0;

    /**
     * \brief   Returns true if buffer can be shared.
     *          The Raw Buffer object should return false.
     *          The Shared Buffer object should return true.
     **/
    virtual bool canShare( void ) const = 0;

    /**
     * \brief   Invalidates the buffer. Removes reference, assigns to invalid buffer,
     *          invalidates writing and reading positions.
     **/
    virtual void invalidate( void );

    /**
     * \brief	Reserves space for byte buffer structure, if needed, 
     *          copies existing data and the size of buffer available to write.
     *          1.  If requested reserved space is more than the length of data buffer, 
     *              the size of data buffer is increased.
     *          2.  If total data buffer spaces is more than IEByteBuffer::MAX_BUF_LENGTH,
     *              reserves the maximum IEByteBuffer::MAX_BUF_LENGTH.
     *          3.  If size is zero, calls RemoveReference() to free all space.
     *          4.  If buffer is shared (i.e. reference count is more than 1)
     *              the operation is ignored and function returns total remaining space
     *              to write data. Because if data is shared between different instances
     *              of byte-buffer, the size of buffer should not be changed.
     *
     * \param	size	The size in bytes to reserve
     * \param	copy    If true and the existing buffer is valid, it will copy data
     * \return	Returns the size available to use (i.e. remaining space).
     **/
    virtual unsigned int reserve(unsigned int size, bool copy);

/************************************************************************/
// IEByteBuffer Attributes and operations
/************************************************************************/

    /**
     * \brief   Returns pointer to the byte buffer.
     **/
    inline const NEMemory::sByteBuffer * getByteBuffer( void)  const;

    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns the use
     **/
    inline unsigned int getSizeUsed( void ) const;
    
    /**
     * \brief	Sets the used size value in byte buffer object.
     *          It should not be more than the length of buffer.
     * \param	newSize	The new size in bytes to set in byte buffer object.
     **/
    inline void setSizeUsed(unsigned int newSize);

    /**
     * \brief   Returns the constant pointer to the data buffer of byte buffer
     **/
    inline const unsigned char * getBuffer( void ) const;

    /**
     * \brief   Returns pointer to the data buffer of byte buffer
     **/
    inline unsigned char * getBuffer( void );

    /**
     * \brief   Checks whether the buffer is valid or not.
     *          No read / write should be performed on invalid buffer.
     **/
    inline bool isValid( void ) const;
    
    /**
     * \brief   Returns the size in byte of byte buffer
     *          If empty, returns zero
     **/
    inline unsigned int getSizeAvailable( void ) const;

    /**
     * \brief   Returns type of byte buffer. 
     *          If valid, it should be either internal or external (for RPC)
     **/
    inline NEMemory::eBufferType getType( void ) const;

//////////////////////////////////////////////////////////////////////////
// Protected internal members
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns pointer to the byte buffer.
     **/
    inline NEMemory::sByteBuffer * getByteBuffer( void );

    /**
     * \brief   Returns read-only end-of-buffer, i.e. end of used space. The end of buffer means 
     *          there is no data to read. Can be only used to manipulate with cursor.
     **/
    inline const unsigned char * getEndOfBuffer( void ) const;

    /**
     * \brief   Returns end-of-buffer, i.e. end of used space. So that, if length of buffer is
     *          bigger than used space, there can be still data written. 
     **/
    inline unsigned char * getEndOfBuffer( void );

/************************************************************************/
// IEByteBuffer protected overrides
/************************************************************************/

    /**
     * \brief	Initialize passed buffer making it as byte buffer and
     *          returns the current writing position in initialized byte buffer.
     *          If needed, it will copy existing binary data to passed buffer.
     *          The function is called from child classes when new byte buffer
     *          is created and the existing data should be passed to new buffer.
     * \param	newBuffer	The buffer to initialize. If nullptr, the buffer object. 
     * \param	bufLength	The length of entire buffer, i.e. length of complete byte buffer.
     * \param	makeCopy	If 'true' it will make copy of existing buffer
     * \return	Returns the current writing position in initialized buffer.
     *          If buffer is invalid, it will return INVALID_CURSOR_POSITION.
     *          If no data is copied, it will return position at the beginning of buffer.
     *          If data is copied, will return the position of written data.
     **/
    virtual unsigned int initBuffer(unsigned char * newBuffer, unsigned int bufLength, bool makeCopy) const;

    /**
     * \brief   Returns the size to align the buffer. By default it is sizeof(NEMemory::uAlign)
     **/
    virtual unsigned int getAlignedSize( void ) const;

    /**
     * \brief   Returns the offset value from the beginning of byte buffer, which should be set
     **/
    virtual unsigned int getDataOffset( void ) const = 0;

    /**
     * \brief   Returns the size of data byte structure (header and one byte).
     *          This is a minimum size of byte buffer to reserve when initializing buffer.
     *          The size can differ for shared and remote message.
     **/
    virtual unsigned int getHeaderSize( void ) const = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Pointer to Byte Buffer structure.
     **/
    mutable std::shared_ptr<NEMemory::sByteBuffer> mByteBuffer;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY( IEByteBuffer );
};

//////////////////////////////////////////////////////////////////////////
// IEByteBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const NEMemory::sByteBuffer * IEByteBuffer::getByteBuffer(void) const
{
    return mByteBuffer.get();
}

inline NEMemory::sByteBuffer * IEByteBuffer::getByteBuffer( void )
{
    return mByteBuffer.get( );
}

inline bool IEByteBuffer::isEmpty( void ) const
{
    return (isValid() == false) || (mByteBuffer->bufHeader.biUsed == 0);
}

inline unsigned int IEByteBuffer::getSizeUsed( void ) const
{
    return (isValid() ? mByteBuffer->bufHeader.biUsed : 0);
}

inline const unsigned char* IEByteBuffer::getBuffer( void ) const
{
    return NEMemory::getBufferDataRead(mByteBuffer.get());
}

inline unsigned char* IEByteBuffer::getBuffer( void )
{
    return NEMemory::getBufferDataWrite(mByteBuffer.get());
}

inline bool IEByteBuffer::isValid( void ) const
{
    return (mByteBuffer.get() != nullptr);
}

inline unsigned int IEByteBuffer::getSizeAvailable( void ) const
{
    return (isValid() ? mByteBuffer->bufHeader.biLength : 0);
}

inline NEMemory::eBufferType IEByteBuffer::getType( void ) const
{
    return (isValid() ? mByteBuffer->bufHeader.biBufType : NEMemory::eBufferType::BufferUnknown);
}

inline void IEByteBuffer::setSizeUsed(unsigned int newSize)
{
    if (isValid() && newSize <= getSizeAvailable())
    {
        mByteBuffer->bufHeader.biUsed = newSize;
    }
}

inline const unsigned char * IEByteBuffer::getEndOfBuffer(void) const
{
    return (isValid() ? NEMemory::getBufferDataRead(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);    
}

inline unsigned char * IEByteBuffer::getEndOfBuffer(void)
{
    return (isValid() ? NEMemory::getBufferDataWrite(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);
}

#endif  // AREG_BASE_IEBYTEBUFFER_HPP
