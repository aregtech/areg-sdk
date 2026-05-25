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
 *              - A single unified cursor (mPosition) serves as BOTH the read and
 *                write position.  Writes land at mPosition and advance it by the
 *                number of bytes written.  When writing within already-written data
 *                (mPosition < biUsed) bytes are overwritten in-place.  When writing
 *                beyond the current end (mPosition >= biUsed) biUsed is extended.
 *                The Cursor interface (position / set_position) moves mPosition for
 *                both reads and writes.
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
 *          A single cursor (mPosition) is shared by both reads and writes.
 *          Writes land at the current cursor position: data inside biUsed is
 *          overwritten in-place; data beyond biUsed extends the buffer.
 *          To append after existing content call move_to_end() first; to
 *          rewind for reading call move_to_begin() / reset().
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
     *          heap block. The visible slice is preserved and the read cursor
     *          of the new object is reset to the beginning of that slice.
     **/
    SharedBuffer(const SharedBuffer& src) noexcept;

    /**
     * \brief   Move-constructs from src. The visible slice is preserved and
     *          the read cursor of the new object is reset to the beginning
     *          of that slice. The `src` is left invalid.
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

    /**
     * \brief   Reserve and ensure additional size to the existing. If the free space of the stream to write
     *          is enough, no changes should be done. If the size of the stream is not enough, it should allocate
     *          additional space and should not loose existing data.
     * \param   addSize     The size to add if required.
     * \return  Returns true if the stream has enough space to write the data.
     **/
    inline bool ensure_size(uint32_t addSize) final;

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
     * \brief   Ensures capacity for at least \a size bytes, optionally preserving
     *          existing content.
     *
     * \param   size    Total bytes required (not an increment).
     * \param   copy    If true, existing written bytes are copied into the new block
     *                  and mPosition is left after them (at biUsed).
     *                  If false, mPosition and biUsed are both reset to 0 -- any
     *                  in-progress write position is silently discarded.
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
     * \brief   Returns true when this instance owns the full backing allocation
     *          (i.e. is not a zero-copy view).  Equivalent to mViewEnd == 0.
     *          Only owner buffers can grow, resize, or write freely.
     **/
    [[nodiscard]]
    inline bool is_owner() const noexcept;

    /**
     * \brief   Returns true when this instance is a zero-copy view window into
     *          another buffer's allocation (created by operator>>(SharedBuffer&)).
     *          View buffers are read-only; call detach() to obtain a writable copy.
     **/
    [[nodiscard]]
    inline bool is_view() const noexcept;

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

    /** Returns the block-growth increment set at construction. **/
    [[nodiscard]]
    inline constexpr uint32_t block_size() const noexcept;

    /**
     * \brief   Returns a fully independent copy (deep copy) of this buffer.
     *          The returned buffer's read cursor is at position 0.
     **/
    [[nodiscard]]
    SharedBuffer clone() const;

    /**
     * \brief   Returns a fully independent owner copy of this buffer's data.
     *          For view buffers, copies [mViewStart, mViewEnd) into a fresh allocation
     *          and returns it as an owner buffer with cursor at 0.
     *          For owner buffers, equivalent to clone().
     *          Use this to obtain a writable buffer from a read-only view.
     **/
    [[nodiscard]]
    SharedBuffer detach() const;

//////////////////////////////////////////////////////////////////////////
// Zero-copy and direct-write fast-path methods
//
//  These bypass the virtual IOStream dispatch and avoid per-field memcpy
//  on the read path.  Use them when the caller knows the concrete type is
//  SharedBuffer (or a subclass) and maximum throughput is required.
//
//  read_ptr<T>  / read_array<T> : return a typed pointer DIRECTLY INTO
//  the buffer memory -- no copy, O(1) regardless of sizeof(T).  The caller
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

public:

    /** Write char string: uint32_t(byte_count) + raw bytes (no NUL on wire). **/
    inline uint32_t write_string_bin(const char* data, uint32_t len) noexcept;

    /** Write wide-char string: uint32_t(byte_count) + raw bytes (no NUL on wire). **/
    inline uint32_t write_string_bin(const wchar_t* data, uint32_t len) noexcept;

    /** Write areg::String in binary format: uint32_t(byte_count) + raw bytes (no NUL). **/
    inline uint32_t write_string_bin(const String& str) noexcept;

    /** Write areg::WideString in binary format: uint32_t(byte_count) + raw bytes (no NUL). **/
    inline uint32_t write_string_bin(const WideString& str) noexcept;

    /** Read binary-format char string. Returns total bytes consumed (header + payload), 0 on failure. **/
    uint32_t read_string_bin(String& str) const noexcept;

    /** Read binary-format wide-char string. Returns total bytes consumed (header + payload), 0 on failure. **/
    uint32_t read_string_bin(WideString& str) const noexcept;

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
// Hot-path helpers -- protected so derived classes (e.g. RemoteMessage) can
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
     * \brief   Core write: writes exactly \a size bytes at the current cursor
     *          position (mPosition), then advances mPosition by \a size.  Extends
     *          biUsed when writing beyond the existing end.  Grows the buffer if
     *          needed using the
     *          doubling strategy in SharedBuffer::reserve().
     *
     * \return  Bytes actually written.
     **/
    uint32_t write_data(const uint8_t* buf, uint32_t size) noexcept;

    /**
     * \brief   Core read: copies up to \a size bytes starting from mPosition
     *          into buf and advances mPosition.
     *
     * \return  Bytes actually copied.
     **/
    uint32_t read_data(uint8_t* buf, uint32_t size) const noexcept;

    /**
     * \brief   Returns a pointer to the readable data at mPosition, or nullptr
     *          when the cursor is at or past the end or the buffer is invalid.
     **/
    [[nodiscard]]
    inline const uint8_t* buffer_to_read() const noexcept;

    /**
     * \brief   Returns a pointer to the writable data at mPosition, or nullptr
     *          when the cursor is at or past the end or the buffer is invalid.
     **/
    [[nodiscard]]
    inline uint8_t* buffer_to_write() noexcept;

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

    /**
     * \brief   Returns the header of the buffer object.
     **/
    [[nodiscard]]
    inline areg::BufferHeader* header() noexcept;

    /**
     * \brief   Returns the header of the buffer object.
     **/
    [[nodiscard]]
    inline const areg::BufferHeader* header() const noexcept;

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
     * \brief   Unified read/write cursor: absolute byte offset from the start of
     *          the data area (buffer_data_read base).  Both read_data() and
     *          write_data() operate at this position and advance it.
     *          Writing within biUsed overwrites in-place; beyond biUsed extends it.
     **/
    mutable uint32_t    mPosition;

    /**
     * \brief   Absolute start of the active view window (inclusive).
     *          Set to 0 for non-view (full-buffer) instances.
     *          For zero-copy sub-buffer views created by read(SharedBuffer&),
     *          this is the absolute offset of the first byte of the view.
     **/
    mutable uint32_t    mViewStart;

    /**
     * \brief   Absolute end of the active view window (exclusive).
     *          0 means no view: use biUsed as the readable limit.
     *          For zero-copy views, mViewEnd = mViewStart + view_length.
     **/
    mutable uint32_t    mViewEnd;
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
    return is_valid() ? (mPosition - mViewStart) : areg::Cursor::INVALID_CURSOR_POSITION;
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

inline bool SharedBuffer::ensure_size(uint32_t addSize)
{
    uint32_t newSize = size_used() + addSize;
    return (reserve(newSize, true) == newSize);
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::read(uint8_t* buf, uint32_t size) const noexcept
{
    return read_data(buf, size);
}

inline void SharedBuffer::reset() const noexcept
{
    mPosition = mViewStart;
}

inline void SharedBuffer::invalidate() noexcept
{
    mPosition  = 0u;
    mViewStart = 0u;
    mViewEnd   = 0u;
    mByteBuffer.reset();
}

inline bool SharedBuffer::is_shared() const noexcept
{
    return is_valid() && (mByteBuffer.use_count() > 1);
}

inline bool SharedBuffer::is_owner() const noexcept
{
    return (mViewEnd == 0u);
}

inline bool SharedBuffer::is_view() const noexcept
{
    return (mViewEnd != 0u);
}

inline bool SharedBuffer::is_empty() const noexcept
{
    return !is_valid() || (mByteBuffer->bufHeader.biUsed == 0);
}

inline uint32_t SharedBuffer::size_used() const noexcept
{
    if (!is_valid()) return 0u;
    return (mViewEnd > 0u ? mViewEnd - mViewStart : mByteBuffer->bufHeader.biUsed);
}

inline const uint8_t* SharedBuffer::buffer() const
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_read(raw) + mViewStart : nullptr);
}

inline uint8_t* SharedBuffer::buffer()
{
    areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_write(raw) + mViewStart : nullptr);
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
    if (!is_valid() || (newSize > size_available()))
        return;

    if (mViewEnd != 0u)
    {
        ASSERT(false);
        return;
    }

    mByteBuffer->bufHeader.biUsed = newSize;
    if (mPosition > newSize)
    {
        mPosition = newSize;
    }
}

inline const uint8_t* SharedBuffer::end_of_buffer() const noexcept
{
    return (is_valid() ? buffer() + size_used() : nullptr);
}

inline uint8_t* SharedBuffer::end_of_buffer() noexcept
{
    return (is_valid() ? buffer() + size_used() : nullptr);
}

inline areg::BufferHeader* SharedBuffer::header() noexcept
{
    return (is_valid() ? &(mByteBuffer.get()->bufHeader) : nullptr);
}

inline const areg::BufferHeader* SharedBuffer::header() const noexcept
{
    return (is_valid() ? &(mByteBuffer.get()->bufHeader) : nullptr);
}

inline bool SharedBuffer::is_begin() const noexcept
{
    return !is_valid() || (mPosition == mViewStart);
}

inline bool SharedBuffer::is_end() const noexcept
{
    if (!is_valid())
        return true;

    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : mByteBuffer->bufHeader.biUsed);
    return (mPosition >= limit);
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

inline const uint8_t* SharedBuffer::buffer_to_read() const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_read(raw) + mPosition : nullptr);
}

inline uint8_t* SharedBuffer::buffer_to_write() noexcept
{
    areg::RawBuffer* raw = mByteBuffer.get();
    const uint8_t* buf   = areg::buffer_data_read(raw);
    return (raw != nullptr ? const_cast<uint8_t *>(buf) + mPosition : nullptr);
}

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t SharedBuffer::size_readable() const noexcept
{
    if (!is_valid()) return 0u;
    const uint32_t limit = (mViewEnd > 0u ? mViewEnd : mByteBuffer->bufHeader.biUsed);
    return (mPosition < limit ? limit - mPosition : 0u);
}

inline uint32_t SharedBuffer::size_writable() const noexcept
{
    if (!is_valid() || is_view())
        return 0u;
    return mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed;
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
    return read_array<T>(1);
}

template <typename T>
inline const T* SharedBuffer::read_array(uint32_t count) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "read_array<T>() requires trivially-copyable T");

    if (count == 0u)
        return nullptr;

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return nullptr;

    const uint32_t sz{ static_cast<uint32_t>(sizeof(T)) * count };
    const uint32_t limit = (mViewEnd != 0u ? mViewEnd : raw->bufHeader.biUsed);
    if (mPosition + sz > limit)
        return nullptr;

    const T* result = reinterpret_cast<const T*>(buffer_to_read());
    mPosition += sz;
    return result;
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

inline uint32_t SharedBuffer::write_string_bin(const char* data, uint32_t len) noexcept
{
    const uint32_t byte_count = len;
    if (write_data(reinterpret_cast<const uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;
    if ((byte_count == 0u) || (data == nullptr))
        return sizeof(uint32_t);
    const uint32_t written = write_data(reinterpret_cast<const uint8_t*>(data), byte_count);
    return sizeof(uint32_t) + written;
}

inline uint32_t SharedBuffer::write_string_bin(const wchar_t* data, uint32_t len) noexcept
{
    const uint32_t byte_count = len * static_cast<uint32_t>(sizeof(wchar_t));
    if (write_data(reinterpret_cast<const uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;
    if ((byte_count == 0u) || (data == nullptr))
        return sizeof(uint32_t);
    const uint32_t written = write_data(reinterpret_cast<const uint8_t*>(data), byte_count);
    return sizeof(uint32_t) + written;
}

inline uint32_t SharedBuffer::write_string_bin(const String& str) noexcept
{
    const uint32_t len = (str.length() > 0) ? static_cast<uint32_t>(str.length()) : 0u;
    return write_string_bin(str.as_string(), len);
}

inline uint32_t SharedBuffer::write_string_bin(const WideString& str) noexcept
{
    const uint32_t len = (str.length() > 0) ? static_cast<uint32_t>(str.length()) : 0u;
    return write_string_bin(str.as_string(), len);
}

template<typename T>
inline std::enable_if_t<std::is_arithmetic_v<T>, SharedBuffer&>
operator << (SharedBuffer& stream, const T& value)
{
    stream.write_pod(value);
    return stream;
}

template<typename T>
inline std::enable_if_t<std::is_arithmetic_v<T>, const SharedBuffer&>
operator >> (const SharedBuffer& stream, T& value)
{
    stream.read(reinterpret_cast<uint8_t*>(&value), sizeof(T));
    return stream;
}

template<typename CharType>
inline const SharedBuffer& operator >> (const SharedBuffer& stream, std::basic_string<CharType>& input)
{
    input.clear();

    const uint32_t pos  = stream.position();
    const uint32_t used = stream.size_used();
    if (pos >= used)
        return stream;

    const uint32_t avail     = used - pos;
    const uint32_t max_chars = avail / static_cast<uint32_t>(sizeof(CharType));
    if (max_chars == 0u)
        return stream;

    const CharType* const src = reinterpret_cast<const CharType*>(stream.buffer() + pos);

    uint32_t len = 0u;
    if constexpr (std::is_same_v<CharType, char>)
    {
        const void* nul = ::memchr(src, '\0', max_chars);
        len = (nul != nullptr)
            ? static_cast<uint32_t>(static_cast<const char*>(nul) - src)
            : max_chars;
    }
    else if constexpr (std::is_same_v<CharType, wchar_t>)
    {
        const wchar_t* nul = static_cast<const wchar_t*>(std::wmemchr(src, L'\0', max_chars));
        len = (nul != nullptr) ? static_cast<uint32_t>(nul - src) : max_chars;
    }
    else
    {
        while ((len < max_chars) && (src[len] != static_cast<CharType>('\0')))
            ++len;
    }

    if (len > 0u)
        input.assign(src, len);

    // Advance past the characters read + the NUL terminator (if present).
    const bool has_nul    = (len < max_chars);
    const uint32_t advance = (has_nul ? (len + 1u) : len) * static_cast<uint32_t>(sizeof(CharType));
    stream.set_position(static_cast<int32_t>(advance), Cursor::SeekOrigin::Current);

    return stream;
}

template<>
struct required_size<areg::SharedBuffer>
{
    [[nodiscard]]
    inline uint32_t operator()(const areg::SharedBuffer& buf) const noexcept
    {
        return static_cast<uint32_t>(sizeof(uint32_t)) + buf.size_used();
    }
};
} // namespace areg
#endif  // AREG_BASE_SHAREDBUFFER_HPP
