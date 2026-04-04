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
 * \note        SharedBuffer is NOT thread-safe. Use external synchronization when
 *              a single buffer is accessed concurrently.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Cursor.hpp"
#include "areg/base/IOStream.hpp"       // InStream, OutStream, IOStream

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <memory>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Reference-counted, streaming, in-memory byte buffer.
 *
 *          Copying a SharedBuffer shares the underlying heap block (no data
 *          copy); the block is freed when the last owner is destroyed.
 *          Any write operation enlarges the block with an amortised doubling strategy.
 *
 *          The Cursor interface exposes the independent READ position.
 *          The WRITE position is always the end of the currently valid data
 *          (biUsed), so writes always append. Call reset() / move_to_begin()
 *          to rewind the read cursor after a write sequence.
 *
 * \note    Writing to a buffer that is shared (use_count > 1) is silently
 *          rejected when it would require reallocation, matching the semantics
 *          of reserve(). Callers should clone() before writing to a shared buffer.
 **/
class AREG_API SharedBuffer : public  IOStream
                            , public  Cursor
{
    friend class FileBuffer;

//////////////////////////////////////////////////////////////////////////
// Defined static constants and types
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Shared pointer allocator / deleter.
     **/
    using ByteBufferDeleter     = areg::BufferDeleter<areg::RawBuffer>;

    /**
     * \brief   Maximum length of byte buffer. It is defined as 64 Mb.
     **/
    static constexpr uint32_t   MAX_BUF_LENGTH  { 0x04000000u };

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
    SharedBuffer( uint32_t       reserveSize
                , const uint8_t* buffer
                , uint32_t       size
                , uint32_t       blockSize = areg::BLOCK_SIZE);

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
    SharedBuffer(const SharedBuffer& src) noexcept;

    /**
     * \brief   Move-constructs from src.  src is left invalid.
     **/
    SharedBuffer(SharedBuffer&& src) noexcept;

    virtual ~SharedBuffer() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline bool operator == (const SharedBuffer& other) const noexcept;
    inline bool operator != (const SharedBuffer& other) const noexcept;

    SharedBuffer& operator = (const SharedBuffer& src) noexcept;
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
    inline uint32_t position() const noexcept final;

    /**
     * \brief   Repositions the read cursor and returns the new position.
     *
     * \param   offset      Signed byte offset.
     * \param   startAt     Origin: Begin, Current, or End.
     * \return  New position, or INVALID_CURSOR_POSITION on failure.
     **/
    uint32_t set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept final;

//////////////////////////////////////////////////////////////////////////
// OutStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /** Serialises a SharedBuffer (length prefix + data) into this buffer. **/
    uint32_t write(const SharedBuffer& buf) final;

    /** Appends raw bytes at the end of written data. **/
    inline uint32_t write(const uint8_t* buf, uint32_t size) final;

    /** Appends a null-terminated ASCII string (includes NUL terminator). **/
    inline uint32_t write(const String& ascii) final;

    /** Appends a null-terminated wide string (includes NUL terminator). **/
    inline uint32_t write(const WideString& wide) final;

    /** No-op for in-memory buffers. **/
    inline void flush() noexcept final;

//////////////////////////////////////////////////////////////////////////
// InStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /** Deserialises a length-prefixed byte sequence into a SharedBuffer. **/
    uint32_t read(SharedBuffer& buf) const noexcept final;

    /** Reads a null-terminated ASCII string from the current read position. **/
    uint32_t read(String& ascii) const final;

    /** Reads a null-terminated wide string from the current read position. **/
    uint32_t read(WideString& wide) const final;

    /** Copies bytes from the current read position into buf. **/
    inline uint32_t read(uint8_t* buf, uint32_t size) const noexcept final;

    /** Resets the read cursor to the beginning of the buffer. **/
    inline void reset() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// SharedBuffer API
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Ensures capacity for at least \a size bytes beyond the current
     *          write position, optionally preserving existing content.
     *
     * \param   size    Total bytes required (not an increment).
     * \param   copy    If true, existing written bytes are copied into the new block.
     * \return  Available writable bytes after the operation; 0 on failure.
     **/
    uint32_t reserve(uint32_t size, bool copy);

    /**
     * \brief   Releases the shared block reference and resets the read cursor.
     **/
    inline void invalidate() noexcept;

    /**
     * \brief   Returns true if the buffer is empty or invalid.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns the number of bytes currently used in the buffer.
     **/
    [[nodiscard]]
    inline uint32_t size_used() const noexcept;

    /**
     * \brief   Sets the number of bytes marked as used in the buffer.
     *
     * \param   newSize     The new size in bytes. Must not exceed the buffer length.
     **/
    inline void set_size_used(uint32_t newSize);

    /**
     * \brief   Returns a read-only pointer to the data buffer.
     **/
    [[nodiscard]]
    inline const uint8_t* buffer() const;

    /**
     * \brief   Returns a pointer to the data buffer.
     **/
    [[nodiscard]]
    inline uint8_t* buffer();

    /**
     * \brief   Returns a read-only pointer to the underlying raw buffer.
     **/
    [[nodiscard]]
    inline const areg::RawBuffer* byte_buffer()  const noexcept;

    /**
     * \brief   Returns true if the buffer is valid.
     **/
    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns the total size of the buffer in bytes.
     **/
    [[nodiscard]]
    inline uint32_t size_available() const;

    /**
     * \brief   Returns the type of buffer (internal or external RPC buffer).
     **/
    [[nodiscard]]
    inline areg::BufferType type() const noexcept;

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
    bool is_equal(const SharedBuffer& other) const noexcept;

    /**
     * \brief   Returns true when the underlying block is shared by more than one owner.
     **/
    [[nodiscard]]
    inline bool is_shared() const noexcept;

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
    const uint8_t* current_ptr() const noexcept;

    /** Returns the block-growth increment set at construction. **/
    [[nodiscard]]
    inline constexpr uint32_t block_size() const noexcept;

    /**
     * \brief   Returns a fully independent copy (deep copy) of this buffer.
     *          The returned buffer's read cursor is at position 0.
     **/
    [[nodiscard]]
    SharedBuffer clone() const;

//////////////////////////////////////////////////////////////////////////
// Zero-copy and direct-write fast-path methods
//
//  These bypass the virtual IOStream dispatch and avoid per-field memcpy
//  on the read path.  Use them when the caller knows the concrete type is
//  SharedBuffer (or a subclass) and maximum throughput is required.
//
//  read_ptr<T>  / read_array<T> : return a typed pointer DIRECTLY INTO
//  the buffer memory — no copy, O(1) regardless of sizeof(T).  The caller
//  must not use the pointer after any write that causes buffer reallocation.
//
//  write_pod<T> / write_array<T>: append bytes without going through the
//  virtual write(uint8_t*,uint32_t) dispatch.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Zero-copy scalar read: returns a const pointer into the buffer at the current
     *          read position and advances the cursor by sizeof(T). Returns nullptr if fewer
     *          than sizeof(T) bytes remain. No allocation, no copy.
     *
     * \tparam  T   Trivially-copyable type (POD, struct, enum, etc.).
     * \return  Pointer into the buffer, or nullptr on underflow.
     **/
    template <typename T>
    [[nodiscard]]
    inline const T* read_ptr() noexcept;

    /**
     * \brief   Zero-copy array read: returns a typed pointer to an array of \a count elements
     *          directly in the buffer and advances the cursor by count * sizeof(T). Returns
     *          nullptr if insufficient data remains. No copy.
     *
     * \tparam  T       Trivially-copyable element type.
     * \param   count   Number of elements to expose.
     * \return  Pointer into the buffer, or nullptr on underflow.
     **/
    template <typename T>
    [[nodiscard]]
    inline const T* read_array(uint32_t count) noexcept;

    /**
     * \brief   Direct write of a trivially-copyable value, bypassing virtual IOStream dispatch.
     *          Equivalent to write(reinterpret_cast<const uint8_t*>(&value), sizeof(T)) but
     *          avoids the virtual call.
     *
     * \tparam  T       Trivially-copyable type.
     * \param   value   Value to write.
     * \return  Bytes written (sizeof(T) on success, 0 on failure).
     **/
    template <typename T>
    inline uint32_t write_pod(const T& value) noexcept;

    /**
     * \brief   Direct write of an array of trivially-copyable values as a single memcpy,
     *          bypassing virtual IOStream dispatch.
     *
     * \tparam  T       Trivially-copyable element type.
     * \param   data    Pointer to array data.
     * \param   count   Number of elements.
     * \return  Number of bytes written.
     **/
    template <typename T>
    inline uint32_t write_array(const T* data, uint32_t count) noexcept;

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /** Returns bytes available to read from the current read position. **/
    [[nodiscard]]
    inline uint32_t size_readable() const noexcept final;

    /** Returns bytes available to write (capacity minus written data). **/
    [[nodiscard]]
    inline uint32_t size_writable() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// SharedBuffer protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Initializes a buffer and optionally copies existing data.
     *
     * \param[out]  newBuffer   The buffer to initialize; if nullptr, the internal buffer is used.
     * \param       bufLength   The total length of the buffer.
     * \param       makeCopy    If true, copies existing data to the new buffer.
     * \return  Returns the current writing position in the initialized buffer; returns
     *          INVALID_CURSOR_POSITION if the buffer is invalid.
     **/
    virtual uint32_t init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept;

    /**
     * \brief   Returns sizeof(areg::RawBuffer): the total header allocation footprint.
     **/
    [[nodiscard]]
    virtual uint32_t header_size() const noexcept;

    /**
     * \brief   Returns sizeof(areg::BufferHeader): the data payload begins at this offset.
     **/
    [[nodiscard]]
    virtual uint32_t data_offset() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Hot-path helpers — protected so derived classes (e.g. RemoteMessage) can
// call them directly without going through the virtual OutStream/InStream
// dispatch.  Non-virtual; the compiler can inline all three within a TU.
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns the default block size from application configuration,
     *          falling back to areg::BLOCK_SIZE.  Result is cached atomically.
     **/
    [[nodiscard]]
    static uint32_t default_block_size() noexcept;

    /**
     * \brief   Core write: appends exactly \a size bytes at the current write
     *          position (= biUsed).  Grows the buffer if needed using the
     *          doubling strategy in SharedBuffer::reserve().
     *
     * \return  Bytes actually written.
     **/
    uint32_t write_data(const uint8_t* buf, uint32_t size) noexcept;

    /**
     * \brief   Core read: copies up to \a size bytes starting from mReadPos
     *          into buf and advances mReadPos.
     *
     * \return  Bytes actually copied.
     **/
    uint32_t read_data(uint8_t* buf, uint32_t size) const noexcept;

    /**
     * \brief   Returns a pointer to the readable data at mReadPos, or nullptr
     *          when the cursor is at or past the end or the buffer is invalid.
     **/
    [[nodiscard]]
    const uint8_t* buffer_to_read() const noexcept;

    /**
     * \brief   Returns mBlockSize: the alignment / growth step for this instance.
     **/
    [[nodiscard]]
    inline uint32_t aligned_size() const noexcept;

    /**
     * \brief   Returns a pointer to the underlying raw buffer.
     **/
    [[nodiscard]]
    inline areg::RawBuffer* byte_buffer() noexcept;

    /**
     * \brief   Returns a read-only pointer to the end of used space in the buffer.
     **/
    [[nodiscard]]
    inline const uint8_t* end_of_buffer() const noexcept;

    /**
     * \brief   Returns a pointer to the end of used space in the buffer.
     **/
    [[nodiscard]]
    inline uint8_t* end_of_buffer() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Allocation growth step in bytes.  Constant after construction.
     **/
    const uint32_t  mBlockSize;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Pointer to Byte Buffer structure.
     **/
    mutable std::shared_ptr<areg::RawBuffer> mByteBuffer;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   Byte offset (relative to the data payload start) at which this
     *          buffer's readable window begins.  Always 0 for normal buffers;
     *          non-zero for view buffers created by SharedBuffer::read(SharedBuffer&).
     **/
    mutable uint32_t    mBaseOffset;

    /**
     * \brief   Number of readable bytes in the view window.  When non-zero this
     *          buffer is a zero-copy view: reads are confined to the range
     *          [mBaseOffset, mBaseOffset + mViewSize) in the shared raw block.
     *          Zero means "use the full mByteBuffer->bufHeader.biUsed extent".
     **/
    mutable uint32_t    mViewSize;

    /**
     * \brief   Read cursor in bytes relative to the start of the view window
     *          (i.e. relative to mBaseOffset).  INVALID_CURSOR_POSITION is
     *          treated as 0 when the buffer is valid.
     *          The write cursor is NOT stored here: it is always equal to
     *          mByteBuffer->bufHeader.biUsed.
     **/
    mutable uint32_t    mReadPos;
};

//////////////////////////////////////////////////////////////////////////
// SharedBuffer inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool SharedBuffer::operator == (const SharedBuffer& other) const noexcept
{
    return is_equal(other);
}

inline bool SharedBuffer::operator != (const SharedBuffer& other) const noexcept
{
    return !is_equal(other);
}

//////////////////////////////////////////////////////////////////////////
// Cursor overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::position() const noexcept
{
    return is_valid() ? mReadPos : Cursor::INVALID_CURSOR_POSITION;
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::write(const uint8_t* buf, uint32_t size)
{
    return write_data(buf, size);
}

inline uint32_t SharedBuffer::write(const String& ascii)
{
    return write_data(reinterpret_cast<const uint8_t*>(ascii.as_string()), ascii.space());
}

inline uint32_t SharedBuffer::write(const WideString& wide)
{
    return write_data(reinterpret_cast<const uint8_t*>(wide.as_string()), wide.space());
}

inline void SharedBuffer::flush() noexcept
{
    // No-op: in-memory buffer, nothing to flush.
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::read(uint8_t* buf, uint32_t size) const noexcept
{
    return read_data(buf, size);
}

void SharedBuffer::reset() const noexcept
{
    mReadPos = 0u;
}

inline void SharedBuffer::invalidate() noexcept
{
    mReadPos    = 0u;
    mBaseOffset = 0u;
    mViewSize   = 0u;
    mByteBuffer.reset();
}

inline bool SharedBuffer::is_shared() const noexcept
{
    return is_valid() && (mByteBuffer.use_count() > 1);
}

inline bool SharedBuffer::is_empty() const noexcept
{
    return (is_valid() == false) || (mByteBuffer->bufHeader.biUsed == 0);
}

inline uint32_t SharedBuffer::size_used() const noexcept
{
    if (!is_valid())
        return 0u;
    return (mViewSize > 0u) ? mViewSize : mByteBuffer->bufHeader.biUsed;
}

inline const uint8_t* SharedBuffer::buffer() const
{
    return areg::buffer_data_read(mByteBuffer.get());
}

inline uint8_t* SharedBuffer::buffer()
{
    return areg::buffer_data_write(mByteBuffer.get());
}

inline bool SharedBuffer::is_valid() const noexcept
{
    return (mByteBuffer.get() != nullptr);
}

inline uint32_t SharedBuffer::size_available() const
{
    return (is_valid() ? mByteBuffer->bufHeader.biLength : 0);
}

inline areg::BufferType SharedBuffer::type() const noexcept
{
    return (is_valid() ? mByteBuffer->bufHeader.biBufType : areg::BufferType::Unknown);
}

inline void SharedBuffer::set_size_used(uint32_t newSize)
{
    if (is_valid() && newSize <= size_available())
    {
        mByteBuffer->bufHeader.biUsed = newSize;
    }
}

inline const uint8_t* SharedBuffer::end_of_buffer() const noexcept
{
    return (is_valid() ? areg::buffer_data_read(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);
}

inline uint8_t* SharedBuffer::end_of_buffer() noexcept
{
    return (is_valid() ? areg::buffer_data_write(mByteBuffer.get()) + mByteBuffer->bufHeader.biUsed : nullptr);
}

inline bool SharedBuffer::is_begin() const noexcept
{
    return !is_valid() || (mReadPos == 0u);
}

inline bool SharedBuffer::is_end() const noexcept
{
    if (!is_valid())
        return true;
    const uint32_t limit = (mViewSize > 0u) ? mViewSize : mByteBuffer->bufHeader.biUsed;
    return mReadPos >= limit;
}

inline constexpr uint32_t SharedBuffer::block_size() const noexcept
{
    return mBlockSize;
}

inline const areg::RawBuffer* SharedBuffer::byte_buffer() const noexcept
{
    return mByteBuffer.get();
}

inline areg::RawBuffer* SharedBuffer::byte_buffer() noexcept
{
    return mByteBuffer.get();
}

inline uint32_t SharedBuffer::aligned_size() const noexcept
{
    return mBlockSize;
}

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::size_readable() const noexcept
{
    if (!is_valid())
        return 0u;

    const uint32_t used = (mViewSize > 0u) ? mViewSize : mByteBuffer->bufHeader.biUsed;
    ASSERT(mReadPos <= used);
    return used - mReadPos;
}

inline uint32_t SharedBuffer::size_writable() const noexcept
{
    if (!is_valid())
        return 0u;

    const uint32_t written  { mByteBuffer->bufHeader.biUsed   };
    const uint32_t available{ mByteBuffer->bufHeader.biLength };
    ASSERT(written <= available);
    return available - written;
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

//////////////////////////////////////////////////////////////////////////
// SharedBuffer zero-copy and direct-write template implementations
//////////////////////////////////////////////////////////////////////////

template <typename T>
inline const T* SharedBuffer::read_ptr() noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "read_ptr<T>() requires trivially-copyable T");
    constexpr uint32_t sz{ static_cast<uint32_t>(sizeof(T)) };
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return nullptr;

    const uint32_t limit = (mViewSize > 0u) ? mViewSize : raw->bufHeader.biUsed;
    if (mReadPos + sz > limit)
        return nullptr;

    const T* ptr{ reinterpret_cast<const T*>(areg::buffer_data_read(raw) + mBaseOffset + mReadPos) };
    mReadPos += sz;
    return ptr;
}

template <typename T>
inline const T* SharedBuffer::read_array(uint32_t count) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "read_array<T>() requires trivially-copyable T");
    if (count == 0u)
        return nullptr;

    const uint32_t sz{ static_cast<uint32_t>(sizeof(T)) * count };
    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return nullptr;

    const uint32_t limit = (mViewSize > 0u) ? mViewSize : raw->bufHeader.biUsed;
    if (mReadPos + sz > limit)
        return nullptr;

    const T* ptr{ reinterpret_cast<const T*>(areg::buffer_data_read(raw) + mBaseOffset + mReadPos) };
    mReadPos += sz;
    return ptr;
}

template <typename T>
inline uint32_t SharedBuffer::write_pod(const T& value) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "write_pod<T>() requires trivially-copyable T");
    return write_data(reinterpret_cast<const uint8_t*>(&value), static_cast<uint32_t>(sizeof(T)));
}

template <typename T>
inline uint32_t SharedBuffer::write_array(const T* data, uint32_t count) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "write_array<T>() requires trivially-copyable T");
    if ((data == nullptr) || (count == 0u))
        return 0u;

    return write_data(reinterpret_cast<const uint8_t*>(data), static_cast<uint32_t>(sizeof(T)) * count);
}

} // namespace areg
#endif  // AREG_BASE_SHAREDBUFFER_HPP
