#ifndef AREG_BASE_BYTEBUFFER_HPP
#define AREG_BASE_BYTEBUFFER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/ByteBuffer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Byte Buffer interface.
 *              This is base class for classes supporting functionalities
 *              to control binary (Byte) buffer. Following classes are
 *              derived from this class: SharedBuffer, FileBuffer, etc.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Cursor.hpp"

#include <memory>

//////////////////////////////////////////////////////////////////////////
// ByteBuffer pure virtual class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for binary (byte) buffer containers.
 **/
class AREG_API ByteBuffer
{
    friend class BufferStreamBase;

//////////////////////////////////////////////////////////////////////////
// Defined static constants and types
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Shared pointer allocator / deleter.
     **/
    using ByteBufferDeleter     = NEMemory::BufferDeleter<NEMemory::RawBuffer>;

    /**
     * \brief   ByteBuffer::MAX_BUF_LENGTH
     *          Maximum length of byte buffer. It is defined as 64 Mb.
     **/
    static constexpr uint32_t   MAX_BUF_LENGTH  { 0x04000000u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Protected default constructor.
     **/
    ByteBuffer();

    /**
     * \brief   Initializes the byte buffer from a source raw buffer.
     *
     * \param   byteBuffer      The source raw buffer to initialize from.
     **/
    ByteBuffer( NEMemory::RawBuffer & byteBuffer );

    /**
     * \brief   Moves a byte buffer from the source.
     *
     * \param   src     The source byte buffer to move from.
     **/
    ByteBuffer( ByteBuffer && src ) noexcept;

public:
    /**
     * \brief   Destructor 
     **/
    virtual ~ByteBuffer() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ByteBuffer overrides
/************************************************************************/
    /**
     * \brief   Returns true if this buffer is shared between multiple instances.
     **/
    virtual bool is_shared() const = 0;

    /**
     * \brief   Returns true if this buffer supports sharing between instances.
     **/
    virtual bool can_share() const = 0;

    /**
     * \brief   Invalidates the buffer and resets all positions.
     **/
    virtual void invalidate();

    /**
     * \brief   Reserves space in the byte buffer, optionally copying existing data.
     *
     * \param   size    The size in bytes to reserve. If zero, frees all space. If the buffer is
     *                  shared, the operation is ignored.
     * \param   copy    If true and the buffer is valid, existing data is copied to the new buffer.
     * \return  Returns the total remaining space available for writing.
     **/
    virtual uint32_t reserve(uint32_t size, bool copy);

/************************************************************************/
// ByteBuffer Attributes and operations
/************************************************************************/

    /**
     * \brief   Returns a read-only pointer to the underlying raw buffer.
     **/
    inline const NEMemory::RawBuffer * byte_buffer()  const;

    /**
     * \brief   Returns true if the buffer is empty or invalid.
     **/
    inline bool is_empty() const;

    /**
     * \brief   Returns the number of bytes currently used in the buffer.
     **/
    inline uint32_t size_used() const;
    
    /**
     * \brief   Sets the number of bytes marked as used in the buffer.
     *
     * \param   newSize     The new size in bytes. Must not exceed the buffer length.
     **/
    inline void set_size_used(uint32_t newSize);

    /**
     * \brief   Returns a read-only pointer to the data buffer.
     **/
    inline const uint8_t * buffer() const;

    /**
     * \brief   Returns a pointer to the data buffer.
     **/
    inline uint8_t * buffer();

    /**
     * \brief   Returns true if the buffer is valid.
     **/
    inline bool is_valid() const;
    
    /**
     * \brief   Returns the total size of the buffer in bytes.
     **/
    inline uint32_t size_available() const;

    /**
     * \brief   Returns the type of buffer (internal or external RPC buffer).
     **/
    inline NEMemory::BufferType type() const;

//////////////////////////////////////////////////////////////////////////
// Protected internal members
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns a pointer to the underlying raw buffer.
     **/
    inline NEMemory::RawBuffer * byte_buffer();

    /**
     * \brief   Returns a read-only pointer to the end of used space in the buffer.
     **/
    inline const uint8_t * end_of_buffer() const;

    /**
     * \brief   Returns a pointer to the end of used space in the buffer.
     **/
    inline uint8_t * end_of_buffer();

/************************************************************************/
// ByteBuffer protected overrides
/************************************************************************/

    /**
     * \brief   Initializes a buffer and optionally copies existing data.
     *
     * \param[out] newBuffer       The buffer to initialize; if nullptr, the internal buffer is
     *                             used.
     * \param   bufLength       The total length of the buffer.
     * \param   makeCopy        If true, copies existing data to the new buffer.
     * \return  Returns the current writing position in the initialized buffer; returns
     *          INVALID_CURSOR_POSITION if the buffer is invalid.
     **/
    virtual uint32_t init_buffer(uint8_t * newBuffer, uint32_t bufLength, bool makeCopy) const;

    /**
     * \brief   Returns the aligned size for buffer allocation.
     **/
    virtual uint32_t aligned_size() const;

    /**
     * \brief   Returns the offset of the data from the beginning of the buffer.
     **/
    virtual uint32_t data_offset() const = 0;

    /**
     * \brief   Returns the size of the buffer header structure.
     **/
    virtual uint32_t header_size() const = 0;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Pointer to Byte Buffer structure.
     **/
    mutable std::shared_ptr<NEMemory::RawBuffer> mByteBuffer;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY( ByteBuffer );
};

//////////////////////////////////////////////////////////////////////////
// ByteBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const NEMemory::RawBuffer * ByteBuffer::byte_buffer() const
{
    return mByteBuffer.get();
}

inline NEMemory::RawBuffer * ByteBuffer::byte_buffer()
{
    return mByteBuffer.get( );
}

inline bool ByteBuffer::is_empty() const
{
    return (is_valid() == false) || (mByteBuffer->bufHeader.biUsed == 0);
}

inline uint32_t ByteBuffer::size_used() const
{
    return (is_valid() ? mByteBuffer->bufHeader.biUsed : 0);
}

inline const uint8_t* ByteBuffer::buffer() const
{
    return NEMemory::buffer_data_read(mByteBuffer.get());
}

inline uint8_t* ByteBuffer::buffer()
{
    return NEMemory::buffer_data_write(mByteBuffer.get());
}

inline bool ByteBuffer::is_valid() const
{
    return (mByteBuffer.get() != nullptr);
}

inline uint32_t ByteBuffer::size_available() const
{
    return (is_valid() ? mByteBuffer->bufHeader.biLength : 0);
}

inline NEMemory::BufferType ByteBuffer::type() const
{
    return (is_valid() ? mByteBuffer->bufHeader.biBufType : NEMemory::BufferType::Unknown);
}

inline void ByteBuffer::set_size_used(uint32_t newSize)
{
    if (is_valid() && newSize <= size_available())
    {
        mByteBuffer->bufHeader.biUsed = newSize;
    }
}

inline const uint8_t * ByteBuffer::end_of_buffer() const
{
    return (is_valid() ? NEMemory::buffer_data_read(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);    
}

inline uint8_t * ByteBuffer::end_of_buffer()
{
    return (is_valid() ? NEMemory::buffer_data_write(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);
}

#endif  // AREG_BASE_BYTEBUFFER_HPP
