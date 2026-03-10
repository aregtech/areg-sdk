#ifndef AREG_BASE_SHAREDBUFFER_HPP
#define AREG_BASE_SHAREDBUFFER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/SharedBuffer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Shared Buffer with integrated streaming.
 *
 * \details     Flat, high-performance in-memory streaming buffer that shares its
 *              underlying heap allocation across copies via std::shared_ptr reference
 *              counting. The buffer is zero-virtual-dispatch on the hot path:
 *
 *              - Write position is always mByteBuffer->bufHeader.biUsed (no separate
 *                cursor object). Writes always append at the end of valid data.
 *              - Read position is tracked by the plain uint32_t mReadPos field.
 *                The Cursor interface (position / set_position) operates on mReadPos.
 *              - write_data() and read_data() access buffer fields directly; no
 *                virtual calls occur inside either function.
 *
 *              Compared with the previous BufferStreamBase design, this eliminates:
 *                - The BufferPosition object and its virtual Cursor dispatch chain.
 *                - The two Cursor& references stored in BufferStreamBase.
 *                - The BufferStreamBase middle layer entirely.
 *
 *              BufferStreamBase is retained for other potential streaming buffer
 *              implementations but is no longer a base of SharedBuffer.
 *
 * \note        SharedBuffer is NOT thread-safe. Use external synchronization when
 *              a single buffer is accessed concurrently.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ByteBuffer.hpp"    // includes Cursor.hpp and MemoryDefs.hpp
#include "areg/base/IOStream.hpp"      // InStream, OutStream, IOStream

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Reference-counted, streaming, in-memory byte buffer.
 *
 *          Copying a SharedBuffer shares the underlying heap block (no data
 *          copy); the block is freed when the last owner is destroyed.
 *          Any write operation enlarges the block with an amortised doubling
 *          strategy.
 *
 *          The Cursor interface exposes the independent READ position.
 *          The WRITE position is always the end of the currently valid data
 *          (biUsed), so writes always append. Call reset() / move_to_begin()
 *          to rewind the read cursor after a write sequence.
 *
 * \note    Writing to a buffer that is shared (use_count > 1) is silently
 *          rejected when it would require reallocation, matching the semantics
 *          of ByteBuffer::reserve(). Callers should clone() before writing to
 *          a shared buffer.
 **/
class AREG_API SharedBuffer  : public  ByteBuffer
                              , public  IOStream
                              , public  Cursor
{
    friend class FileBuffer;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Constructs an empty buffer with the given block-growth step.
     *
     * \param   blockSize   Allocation increment in bytes, rounded up to
     *                      areg::BLOCK_SIZE (default: areg::BLOCK_SIZE).
     **/
    explicit SharedBuffer(uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs and pre-allocates space without writing any data.
     *
     * \param   reserveSize     Bytes to reserve upfront.
     * \param   blockSize       Allocation increment, rounded to areg::BLOCK_SIZE.
     **/
    SharedBuffer(uint32_t reserveSize, uint32_t blockSize);

    /**
     * \brief   Constructs and writes the supplied raw bytes into the buffer.
     *
     * \param   buffer      Data to copy into the buffer.
     * \param   size        Number of bytes.
     * \param   blockSize   Allocation increment (default: areg::BLOCK_SIZE).
     **/
    SharedBuffer(const uint8_t* buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs, pre-allocates, then writes the supplied raw bytes.
     *          The reservation is enlarged to at least \a size if needed.
     *
     * \param   reserveSize     Bytes to reserve upfront.
     * \param   buffer          Data to copy into the buffer.
     * \param   size            Number of bytes in buffer.
     * \param   blockSize       Allocation increment (default: areg::BLOCK_SIZE).
     **/
    SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size
               , uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs and writes a null-terminated ASCII string (including
     *          the terminating NUL character).
     *
     * \param   textString  The source string; nullptr is treated as "".
     * \param   blockSize   Allocation increment (default: areg::BLOCK_SIZE).
     **/
    explicit SharedBuffer(const char* textString, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs and writes a null-terminated wide string (including
     *          the terminating NUL character).
     *
     * \param   textString  The source string; nullptr is treated as L"".
     * \param   blockSize   Allocation increment (default: areg::BLOCK_SIZE).
     **/
    explicit SharedBuffer(const wchar_t* textString, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Share-constructs from src: the two objects reference the same
     *          heap block.  The read cursor of the new object is reset to 0.
     **/
    SharedBuffer(const SharedBuffer& src);

    /**
     * \brief   Move-constructs from src.  src is left invalid.
     **/
    SharedBuffer(SharedBuffer&& src) noexcept;

    virtual ~SharedBuffer() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline bool operator == (const SharedBuffer& other) const;
    inline bool operator != (const SharedBuffer& other) const;

    SharedBuffer& operator = (const SharedBuffer& src);
    SharedBuffer& operator = (SharedBuffer&& src) noexcept;

/************************************************************************/
// Friend global streaming operators
/************************************************************************/

    /**
     * \brief   Deserialises data from stream into this buffer and rewinds read cursor.
     **/
    friend inline const InStream& operator >> (const InStream& stream, SharedBuffer& input);

    /**
     * \brief   Serialises this buffer's data into stream and rewinds the output read cursor.
     **/
    friend inline OutStream& operator << (OutStream& stream, const SharedBuffer& output);

//////////////////////////////////////////////////////////////////////////
// Cursor interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns the current read-cursor position in bytes relative to the
     *          beginning of the buffer.  Returns INVALID_CURSOR_POSITION if the
     *          buffer is not valid.
     **/
    [[nodiscard]]
    uint32_t position() const noexcept override;

    /**
     * \brief   Repositions the read cursor and returns the new position.
     *
     * \param   offset      Signed byte offset.
     * \param   startAt     Origin: Begin, Current, or End.
     * \return  New position, or INVALID_CURSOR_POSITION on failure.
     **/
    uint32_t set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept override;

//////////////////////////////////////////////////////////////////////////
// ByteBuffer interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /** Returns true when the underlying block is shared by more than one owner. **/
    [[nodiscard]]
    bool is_shared() const noexcept override final;

    /** Always returns true: SharedBuffer supports reference-counted sharing. **/
    [[nodiscard]]
    bool can_share() const noexcept override final;

    /**
     * \brief   Releases the shared block reference and resets the read cursor.
     **/
    void invalidate() override;

    /**
     * \brief   Ensures capacity for at least \a size bytes beyond the current
     *          write position, optionally preserving existing content.
     *
     * \param   size    Total bytes required (not an increment).
     * \param   copy    If true, existing written bytes are copied into the new block.
     * \return  Available writable bytes after the operation; 0 on failure.
     **/
    uint32_t reserve(uint32_t size, bool copy) override;

//////////////////////////////////////////////////////////////////////////
// OutStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /** Appends raw bytes at the end of written data. **/
    uint32_t write(const uint8_t* buf, uint32_t size) override;

    /** Serialises a ByteBuffer (length prefix + data) into this buffer. **/
    uint32_t write(const ByteBuffer& buf) override;

    /** Appends a null-terminated ASCII string (includes NUL terminator). **/
    uint32_t write(const String& ascii) override;

    /** Appends a null-terminated wide string (includes NUL terminator). **/
    uint32_t write(const WideString& wide) override;

    /** No-op for in-memory buffers. **/
    void flush() override;

//////////////////////////////////////////////////////////////////////////
// InStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /** Copies bytes from the current read position into buf. **/
    uint32_t read(uint8_t* buf, uint32_t size) const override;

    /** Deserialises a length-prefixed byte sequence into a ByteBuffer. **/
    uint32_t read(ByteBuffer& buf) const override;

    /** Reads a null-terminated ASCII string from the current read position. **/
    uint32_t read(String& ascii) const override;

    /** Reads a null-terminated wide string from the current read position. **/
    uint32_t read(WideString& wide) const override;

    /** Resets the read cursor to the beginning of the buffer. **/
    void reset() const override;

//////////////////////////////////////////////////////////////////////////
// Additional streaming operations (formerly in BufferStreamBase)
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Inserts bytes at an arbitrary position, shifting later bytes right.
     *          Appends if \a atPos is at or past the current write position.
     *
     * \param   buf     Source data.
     * \param   size    Bytes to insert.
     * \param   atPos   Byte offset at which to insert.
     * \return  Bytes inserted.
     **/
    uint32_t insert_at(const uint8_t* buf, uint32_t size, uint32_t atPos);

    /**
     * \brief   Returns true when both buffers contain identical byte sequences.
     **/
    [[nodiscard]]
    bool is_equal(const SharedBuffer& other) const;

    /**
     * \brief   Returns true when the read cursor is at (or before) the beginning.
     **/
    [[nodiscard]]
    inline bool is_begin() const noexcept;

    /**
     * \brief   Returns true when the read cursor equals the end of written data.
     **/
    [[nodiscard]]
    inline bool is_end() const noexcept;

    /**
     * \brief   Returns a pointer to the buffer at the current read position, or
     *          nullptr if the cursor is at the end or the buffer is invalid.
     **/
    [[nodiscard]]
    const uint8_t* current_ptr() const;

    /** Returns the block-growth increment set at construction. **/
    [[nodiscard]]
    inline uint32_t block_size() const noexcept;

    /**
     * \brief   Returns a fully independent copy (deep copy) of this buffer.
     *          The returned buffer's read cursor is at position 0.
     **/
    [[nodiscard]]
    SharedBuffer clone() const;

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /** Returns bytes available to read from the current read position. **/
    [[nodiscard]]
    uint32_t size_readable() const override;

    /** Returns bytes available to write (capacity minus written data). **/
    [[nodiscard]]
    uint32_t size_writable() const override;

//////////////////////////////////////////////////////////////////////////
// ByteBuffer protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /** Returns sizeof(areg::BufferHeader): the data payload begins at this offset. **/
    [[nodiscard]]
    uint32_t data_offset() const noexcept override;

    /** Returns sizeof(areg::RawBuffer): the total header allocation footprint. **/
    [[nodiscard]]
    uint32_t header_size() const noexcept override;

    /** Returns mBlockSize: the alignment / growth step for this instance. **/
    [[nodiscard]]
    uint32_t aligned_size() const noexcept override;

    /**
     * \brief   Returns the default block size from application configuration,
     *          falling back to areg::BLOCK_SIZE.  Result is cached atomically.
     **/
    [[nodiscard]]
    static uint32_t default_block_size() noexcept;

//////////////////////////////////////////////////////////////////////////
// Hot-path helpers — protected so derived classes (e.g. RemoteMessage) can
// call them directly without going through the virtual OutStream/InStream
// dispatch.  Non-virtual; the compiler can inline all three within a TU.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Core write: appends exactly \a size bytes at the current write
     *          position (= biUsed).  Grows the buffer if needed using the
     *          doubling strategy in ByteBuffer::reserve().
     *
     * \return  Bytes actually written.
     **/
    uint32_t write_data(const uint8_t* buf, uint32_t size);

    /**
     * \brief   Core read: copies up to \a size bytes starting from mReadPos
     *          into buf and advances mReadPos.
     *
     * \return  Bytes actually copied.
     **/
    uint32_t read_data(uint8_t* buf, uint32_t size) const;

    /**
     * \brief   Returns a pointer to the readable data at mReadPos, or nullptr
     *          when the cursor is at or past the end or the buffer is invalid.
     **/
    [[nodiscard]]
    const uint8_t* buffer_to_read() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Allocation growth step in bytes.  Constant after construction.
     **/
    const uint32_t  mBlockSize;

private:

    /**
     * \brief   Read cursor in bytes relative to the start of the data payload.
     *          INVALID_CURSOR_POSITION is treated as 0 when the buffer is valid.
     *          The write cursor is NOT stored here: it is always equal to
     *          mByteBuffer->bufHeader.biUsed.
     **/
    mutable uint32_t    mReadPos;
};

//////////////////////////////////////////////////////////////////////////
// SharedBuffer inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool SharedBuffer::operator == (const SharedBuffer& other) const
{
    return is_equal(other);
}

inline bool SharedBuffer::operator != (const SharedBuffer& other) const
{
    return (is_equal(other) == false);
}

inline bool SharedBuffer::is_begin() const noexcept
{
    const uint32_t cur = position();
    return (is_valid() == false) || (cur == 0u) || (cur == Cursor::INVALID_CURSOR_POSITION);
}

inline bool SharedBuffer::is_end() const noexcept
{
    return is_valid() && (position() == size_used());
}

inline uint32_t SharedBuffer::block_size() const noexcept
{
    return mBlockSize;
}

/************************************************************************/
// Friend global streaming operators
/************************************************************************/

inline const InStream& operator >> (const InStream& stream, SharedBuffer& input)
{
    if (static_cast<const InStream*>(&stream) != static_cast<const InStream*>(&input))
    {
        stream.read(input);
        input.move_to_begin();
    }

    return stream;
}

inline OutStream& operator << (OutStream& stream, const SharedBuffer& output)
{
    if (static_cast<const OutStream*>(&stream) != static_cast<const OutStream*>(&output))
    {
        stream.write(output);
        output.move_to_begin();
    }

    return stream;
}

} // namespace areg
#endif  // AREG_BASE_SHAREDBUFFER_HPP
