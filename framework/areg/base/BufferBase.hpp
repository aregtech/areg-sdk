#ifndef AREG_BASE_BUFFERBASE_HPP
#define AREG_BASE_BUFFERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/BufferBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Shared Buffer with integrated streaming.
 *
 * \note        BufferBase is NOT thread-safe. Use external synchronization when
 *              a single buffer is accessed concurrently.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Cursor.hpp"
#include "areg/base/IOStream.hpp"

#include "areg/base/MemoryDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"

#include <memory>

namespace areg {

class SharedBuffer;

//////////////////////////////////////////////////////////////////////////
// BufferBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Reference-counted, streaming, in-memory byte buffer.
 *          Copying a BufferBase shares the underlying heap block (no data copy);
 *          the block is freed when the last owner is destroyed.
 *          Any write operation enlarges the block with an amortised doubling strategy.
 *          A single cursor (mPosition) is shared by both reads and writes.
 *
 * \note    BufferBase is NOT thread-safe. Use external synchronization when
 *          a single buffer is accessed concurrently.
 **/
class AREG_API BufferBase   : public  IOStream
                            , public  Cursor
{
    friend class FileBuffer;

//////////////////////////////////////////////////////////////////////////
// Defined static constants and types
//////////////////////////////////////////////////////////////////////////
protected:

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
    explicit BufferBase(uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs and pre-allocates space without writing any data.
     *
     * \param   reserveSize     Bytes to reserve upfront.
     * \param   blockSize       Allocation increment, rounded to areg::BLOCK_SIZE.
     **/
    BufferBase(uint32_t reserveSize, uint32_t blockSize);

    /**
     * \brief   Constructs and writes the supplied raw bytes into the buffer.
     *
     * \param   buffer      Data to copy into the buffer.
     * \param   size        Number of bytes.
     * \param   blockSize   Allocation increment (default: areg::BLOCK_SIZE).
     **/
    BufferBase(const uint8_t* buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs, pre-allocates, then writes the supplied raw bytes.
     *          The reservation is enlarged to at least \a size if needed.
     *
     * \param   reserveSize     Bytes to reserve upfront.
     * \param   buffer          Data to copy into the buffer.
     * \param   size            Number of bytes in buffer.
     * \param   blockSize       Allocation increment (default: areg::BLOCK_SIZE).
     **/
    BufferBase( uint32_t       reserveSize
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
    explicit BufferBase(const char* textString, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Constructs and writes a null-terminated wide string (including
     *          the terminating NUL character).
     *
     * \param   textString  The source string; nullptr is treated as L"".
     * \param   blockSize   Allocation increment (default: areg::BLOCK_SIZE).
     **/
    explicit BufferBase(const wchar_t* textString, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Share-constructs from src: the two objects reference the same
     *          heap block. The visible slice is preserved and the read cursor
     *          of the new object is reset to the beginning of that slice.
     **/
    BufferBase(const BufferBase& src) noexcept;

    /**
     * \brief   Move-constructs from src. The visible slice is preserved and
     *          the read cursor of the new object is reset to the beginning
     *          of that slice. The `src` is left invalid.
     **/
    BufferBase(BufferBase&& src) noexcept;

    virtual ~BufferBase() noexcept = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline bool operator == (const BufferBase& other) const noexcept;
    inline bool operator != (const BufferBase& other) const noexcept;

    BufferBase& operator = (const BufferBase& src) noexcept;
    BufferBase& operator = (BufferBase&& src) noexcept;

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
    inline uint32_t position() const noexcept override;

    /**
     * \brief   Repositions the read cursor and returns the new position.
     *
     * \param   offset      Signed byte offset.
     * \param   startAt     Origin: Begin, Current, or End.
     * \return  New position, or INVALID_CURSOR_POSITION on failure.
     **/
    uint32_t set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept override;

//////////////////////////////////////////////////////////////////////////
// OutStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Serialises a BufferBase (length prefix + data) into this buffer.
     **/
    uint32_t write(const SharedBuffer& buf) override;

    /**
     * \brief   Appends raw bytes at the end of written data.
     **/
    inline uint32_t write(const uint8_t* buf, uint32_t size) override;

    /**
     * \brief   Appends a null-terminated ASCII string (includes NUL terminator).
     **/
    inline uint32_t write(const String& ascii) override;

    /**
     * \brief   Appends a null-terminated wide string (includes NUL terminator).
     **/
    inline uint32_t write(const WideString& wide) override;

    /** No-op for in-memory buffers. **/
    inline void flush() noexcept override;

    /**
     * \brief   Reserve and ensure additional size to the existing. If the free space of the stream to write
     *          is enough, no changes should be done. If the size of the stream is not enough, it should allocate
     *          additional space and should not loose existing data.
     * \param   addSize     The size to add if required.
     * \return  Returns true if the stream has enough space to write the data.
     **/
    inline bool ensure_size(uint32_t addSize) override;

//////////////////////////////////////////////////////////////////////////
// InStream interface overrides
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Deserialises a length-prefixed byte sequence into a BufferBase.
     **/
    uint32_t read(SharedBuffer& buf) const noexcept override;

    /**
     * \brief   Reads a null-terminated ASCII string from the current read position.
     **/
    uint32_t read(String& ascii) const override;

    /**
     * \brief   Reads a null-terminated wide string from the current read position.
     **/
    uint32_t read(WideString& wide) const override;

    /**
     * \brief   Copies bytes from the current read position into buf.
     **/
    inline uint32_t read(uint8_t* buf, uint32_t size) const noexcept override;

    /**
     * \brief   Resets the read cursor to the beginning of the buffer.
     **/
    inline void reset() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// BufferBase API
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Ensures capacity for at least \a size bytes, optionally preserving existing content.
     *
     * \param   size    Total bytes required (not an increment).
     * \param   copy    If true, existing written bytes are copied into the new block
     *                  and mPosition is left after them (at biUsed).
     *                  If false, size and position are reset to 0
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
    inline const uint8_t* buffer() const noexcept;

    /**
     * \brief   Returns a pointer to the data buffer.
     **/
    [[nodiscard]]
    inline uint8_t* buffer() noexcept;

    /**
     * \brief   Returns a read-only pointer to the underlying raw buffer.
     **/
    [[nodiscard]]
    inline const areg::RawBuffer* byte_buffer()  const noexcept;

    /**
     * \brief   Returns the shared owner of the underlying heap block. Copying it (cheap: one atomic
     *          refcount increment) keeps the allocation alive without constructing a full BufferBase/
     *          Event. Used by batch send paths to retain drained message buffers until the writev
     *          completes, instead of moving heavyweight Event objects into a holding array.
     **/
    [[nodiscard]]
    inline const areg::RawBufferPtr& share_buffer() const noexcept;

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
     * \brief   Returns true when both buffers contain identical byte sequences.
     **/
    [[nodiscard]]
    bool is_equal(const BufferBase& other) const noexcept;

    /**
     * \brief   Returns true when the underlying block is shared by more than one owner.
     **/
    [[nodiscard]]
    inline bool is_shared() const noexcept;

    /**
     * \brief   Returns true when this instance is the sole owner of the underlying block
     *          Use to guard placement-new'd payload destructors.
     **/
    [[nodiscard]]
    inline bool is_unique() const noexcept;

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
    BufferBase clone() const;

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

//////////////////////////////////////////////////////////////////////////
// Zero-copy and direct-write fast-path methods
//
//  These bypass the virtual IOStream dispatch and avoid per-field memcpy
//  on the read path.  Use them when the caller knows the concrete type is
//  BufferBase (or a subclass) and maximum throughput is required.
//
//  read_ptr<T>  / read_array<T> : return a typed pointer DIRECTLY INTO
//  the buffer memory -- no copy.  The caller must not use the pointer 
//  after any write that causes buffer reallocation.
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
     *          Equivalent to write(reinterpret_cast<const uint8_t *>(&value), sizeof(T)) but
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

    /**
     * \brief   Write char string: uint32_t(byte_count) + raw bytes (no NUL on wire).
     **/
    inline uint32_t write_string_bin(const char* data, uint32_t len) noexcept;

    /**
     * \brief   Write wide-char string: uint32_t(byte_count) + raw bytes (no NUL on wire).
     **/
    inline uint32_t write_string_bin(const wchar_t* data, uint32_t len) noexcept;

    /**
     * \brief   Write areg::String in binary format: uint32_t(byte_count) + raw bytes (no NUL).
     **/
    inline uint32_t write_string_bin(const String& str) noexcept;

    /**
     * \brief   Write areg::WideString in binary format: uint32_t(byte_count) + raw bytes (no NUL).
     **/
    inline uint32_t write_string_bin(const WideString& str) noexcept;

    /**
     * \brief   Read binary-format char string. Returns total bytes consumed (header + payload), 0 on failure.
     **/
    uint32_t read_string_bin(String& str) const noexcept;

    /**
     * \brief   Read binary-format wide-char string. Returns total bytes consumed (header + payload), 0 on failure.
     **/
    uint32_t read_string_bin(WideString& str) const noexcept;

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Returns bytes available to read from the current read position.
     **/
    [[nodiscard]]
    inline uint32_t size_readable() const noexcept override;

    /**
     * \brief   Returns bytes available to write (capacity minus written data).
     **/
    [[nodiscard]]
    inline uint32_t size_writable() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// BufferBase protected overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Initializes a buffer and optionally copies existing data.
     *
     * \param[out]  newBuffer   The buffer to initialize
     * \param       bufLength   The total length of the buffer.
     * \param       makeCopy    If true, copies existing data to the new buffer.
     * \return  Returns the current writing position in the initialized buffer;
     *          returns INVALID_CURSOR_POSITION if the buffer is invalid.
     **/
    virtual uint32_t init_buffer(uint8_t* newBuffer, uint32_t bufLength, bool makeCopy) const noexcept;

    /**
     * \brief   Returns sizeof(areg::RawBuffer): the total header allocation footprint.
     **/
    [[nodiscard]]
    virtual uint32_t header_size() const noexcept;

    /**
     * \brief   Returned value is the data payload begins at this offset.
     **/
    [[nodiscard]]
    virtual uint32_t data_offset() const noexcept;

protected:

    /**
     * \brief   Returns true if the buffer accepts writes. BufferBase is always writable;
     *          SharedBuffer overrides this to return false while it is a read-only view.
     **/
    [[nodiscard]]
    virtual bool can_write() const noexcept;

    /**
     * \brief   Returns the default block size from application configuration,
     *          falling back to areg::BLOCK_SIZE.  Result is cached atomically.
     **/
    [[nodiscard]]
    static uint32_t default_block_size() noexcept;

    /**
     * \brief   Core write: writes exactly \a size bytes at the current cursor position.
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
     * \brief   Intrusive reference-counted owner of the Byte Buffer structure.
     *          One heap allocation per buffer; the refcount lives in BufferHeader::biRefCount.
     **/
    mutable areg::RawBufferPtr mByteBuffer;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   Unified read/write cursor, absolute byte offset from the start of the data.
     **/
    mutable uint32_t    mPosition;
};

//////////////////////////////////////////////////////////////////////////
// BufferBase inline implementation
//////////////////////////////////////////////////////////////////////////

inline bool BufferBase::operator == (const BufferBase& other) const noexcept
{
    return is_equal(other);
}

inline bool BufferBase::operator != (const BufferBase& other) const noexcept
{
    return !is_equal(other);
}

//////////////////////////////////////////////////////////////////////////
// Cursor overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t BufferBase::position() const noexcept
{
    return is_valid() ? mPosition : areg::Cursor::INVALID_CURSOR_POSITION;
}

//////////////////////////////////////////////////////////////////////////
// OutStream overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t BufferBase::write(const uint8_t* buf, uint32_t size)
{
    return write_data(buf, size);
}

inline uint32_t BufferBase::write(const String& ascii)
{
    return write_data(reinterpret_cast<const uint8_t*>(ascii.as_string()), ascii.space());
}

inline uint32_t BufferBase::write(const WideString& wide)
{
    return write_data(reinterpret_cast<const uint8_t*>(wide.as_string()), wide.space());
}

inline void BufferBase::flush() noexcept
{
    // No-op: in-memory buffer, nothing to flush.
}

inline bool BufferBase::ensure_size(uint32_t addSize)
{
    uint32_t newSize = size_used() + addSize;
    return (reserve(newSize, true) == newSize);
}

//////////////////////////////////////////////////////////////////////////
// InStream overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t BufferBase::read(uint8_t* buf, uint32_t size) const noexcept
{
    return read_data(buf, size);
}

inline void BufferBase::reset() const noexcept
{
    mPosition = 0u;
}

inline void BufferBase::invalidate() noexcept
{
    mPosition  = 0u;
    mByteBuffer.reset();
}

inline bool BufferBase::is_shared() const noexcept
{
    return is_valid() && (mByteBuffer.use_count() > 1);
}

inline bool BufferBase::is_unique() const noexcept
{
    return is_valid() && (mByteBuffer.use_count() == 1);
}

inline bool BufferBase::is_empty() const noexcept
{
    return !is_valid() || (mByteBuffer->bufHeader.biUsed == 0);
}

inline uint32_t BufferBase::size_used() const noexcept
{
    if (!is_valid()) return 0u;
    return mByteBuffer->bufHeader.biUsed;
}

inline const uint8_t* BufferBase::buffer() const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_read(raw) : nullptr);
}

inline uint8_t* BufferBase::buffer() noexcept
{
    areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_write(raw) : nullptr);
}

inline bool BufferBase::is_valid() const noexcept
{
    return (mByteBuffer.get() != nullptr);
}

inline uint32_t BufferBase::size_available() const
{
    return (is_valid() ? mByteBuffer->bufHeader.biLength : 0);
}

inline areg::BufferType BufferBase::type() const noexcept
{
    return (is_valid() ? mByteBuffer->bufHeader.biBufType : areg::BufferType::Unknown);
}

inline void BufferBase::set_size_used(uint32_t newSize)
{
    if (!is_valid() || (newSize > size_available()))
        return;

    mByteBuffer->bufHeader.biUsed = newSize;
    if (mPosition > newSize)
    {
        mPosition = newSize;
    }
}

inline const uint8_t* BufferBase::end_of_buffer() const noexcept
{
    return (is_valid() ? buffer() + size_used() : nullptr);
}

inline uint8_t* BufferBase::end_of_buffer() noexcept
{
    return (is_valid() ? buffer() + size_used() : nullptr);
}

inline areg::BufferHeader* BufferBase::header() noexcept
{
    return (is_valid() ? &(mByteBuffer.get()->bufHeader) : nullptr);
}

inline const areg::BufferHeader* BufferBase::header() const noexcept
{
    return (is_valid() ? &(mByteBuffer.get()->bufHeader) : nullptr);
}

inline bool BufferBase::is_begin() const noexcept
{
    return !is_valid() || (mPosition == 0u);
}

inline bool BufferBase::is_end() const noexcept
{
    if (!is_valid())
        return true;

    const uint32_t limit = mByteBuffer->bufHeader.biUsed;
    return (mPosition >= limit);
}

inline constexpr uint32_t BufferBase::block_size() const noexcept
{
    return mBlockSize;
}

inline const areg::RawBuffer* BufferBase::byte_buffer() const noexcept
{
    return mByteBuffer.get();
}

inline const areg::RawBufferPtr& BufferBase::share_buffer() const noexcept
{
    return mByteBuffer;
}

inline areg::RawBuffer* BufferBase::byte_buffer() noexcept
{
    return mByteBuffer.get();
}

inline const uint8_t* BufferBase::buffer_to_read() const noexcept
{
    const areg::RawBuffer* const raw = mByteBuffer.get();
    return (raw != nullptr ? areg::buffer_data_read(raw) + mPosition : nullptr);
}

inline uint8_t* BufferBase::buffer_to_write() noexcept
{
    areg::RawBuffer* raw = mByteBuffer.get();
    const uint8_t* buf   = areg::buffer_data_read(raw);
    return (raw != nullptr ? const_cast<uint8_t *>(buf) + mPosition : nullptr);
}

//////////////////////////////////////////////////////////////////////////
// IOStream protected overrides
//////////////////////////////////////////////////////////////////////////

inline uint32_t BufferBase::size_readable() const noexcept
{
    if (!is_valid())
        return 0u;
    const uint32_t limit = mByteBuffer->bufHeader.biUsed;
    return (mPosition < limit ? limit - mPosition : 0u);
}

inline uint32_t BufferBase::size_writable() const noexcept
{
    if (!is_valid())
        return 0u;
    return mByteBuffer->bufHeader.biLength - mByteBuffer->bufHeader.biUsed;
}


//////////////////////////////////////////////////////////////////////////
// BufferBase zero-copy and direct-write template implementations
//////////////////////////////////////////////////////////////////////////

template <typename T>
inline const T* BufferBase::read_ptr() noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "read_ptr<T>() requires trivially-copyable T");
    return read_array<T>(1);
}

template <typename T>
inline const T* BufferBase::read_array(uint32_t count) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "read_array<T>() requires trivially-copyable T");

    if (count == 0u)
        return nullptr;

    const areg::RawBuffer* const raw = mByteBuffer.get();
    if (raw == nullptr)
        return nullptr;

    const uint32_t sz{ static_cast<uint32_t>(sizeof(T)) * count };
    const uint32_t limit = raw->bufHeader.biUsed;
    if (mPosition + sz > limit)
        return nullptr;

    const T* result = reinterpret_cast<const T*>(buffer_to_read());
    mPosition += sz;
    return result;
}

template <typename T>
inline uint32_t BufferBase::write_pod(const T& value) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "write_pod<T>() requires trivially-copyable T");
    return write_data(reinterpret_cast<const uint8_t*>(&value), static_cast<uint32_t>(sizeof(T)));
}

template <typename T>
inline uint32_t BufferBase::write_array(const T* data, uint32_t count) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>, "write_array<T>() requires trivially-copyable T");
    if ((data == nullptr) || (count == 0u))
        return 0u;

    return write_data(reinterpret_cast<const uint8_t*>(data), static_cast<uint32_t>(sizeof(T)) * count);
}

inline uint32_t BufferBase::write_string_bin(const char* data, uint32_t len) noexcept
{
    const uint32_t byte_count = len;
    if (write_data(reinterpret_cast<const uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;
    if ((byte_count == 0u) || (data == nullptr))
        return sizeof(uint32_t);
    const uint32_t written = write_data(reinterpret_cast<const uint8_t*>(data), byte_count);
    return sizeof(uint32_t) + written;
}

inline uint32_t BufferBase::write_string_bin(const wchar_t* data, uint32_t len) noexcept
{
    const uint32_t byte_count = len * static_cast<uint32_t>(sizeof(wchar_t));
    if (write_data(reinterpret_cast<const uint8_t*>(&byte_count), sizeof(uint32_t)) != sizeof(uint32_t))
        return 0u;
    if ((byte_count == 0u) || (data == nullptr))
        return sizeof(uint32_t);
    const uint32_t written = write_data(reinterpret_cast<const uint8_t*>(data), byte_count);
    return sizeof(uint32_t) + written;
}

inline uint32_t BufferBase::write_string_bin(const String& str) noexcept
{
    const uint32_t len = (str.length() > 0) ? static_cast<uint32_t>(str.length()) : 0u;
    return write_string_bin(str.as_string(), len);
}

inline uint32_t BufferBase::write_string_bin(const WideString& str) noexcept
{
    const uint32_t len = (str.length() > 0) ? static_cast<uint32_t>(str.length()) : 0u;
    return write_string_bin(str.as_string(), len);
}

} // namespace areg

#endif  // AREG_BASE_BUFFERBASE_HPP
