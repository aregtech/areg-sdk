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
 * \brief       Areg Platform, Shared Buffer class
 *              This Buffer is used for Read and Write of data and
 *              it might be shared between different threads and objects.
 *              The Data in buffer remains valid until the number of references
 *              to the buffer is more than zero.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/BufferStreamBase.hpp"
#include "areg/base/private/BufferPosition.hpp"
namespace areg {

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
                            , public  Cursor  // To control read and write operations
{
    friend class FileBuffer;

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the object with the specified or default block size.
     *
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    explicit SharedBuffer(uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Reserves the space in the byte buffer to write data and sets block size.
     *
     * \param   reserveSize     Size in bytes to reserve
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer( uint32_t reserveSize, uint32_t blockSize );

    /**
     * \brief   Reserves space and writes given data into byte buffer.
     *
     * \param   buffer          The data to initialize byte buffer
     * \param   size            The length in bytes of data
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer( const uint8_t * buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE );

    /**
     * \brief   Reserves requested space in bytes and writes given data into byte buffer. The
     *          reserved space should at least the size of the buffer to write. If the requested
     *          space to reserve is smaller than the size of the buffer, it will be increased to the
     *          size of the buffer.
     *
     * \param   reserveSize     Number of bytes to reserve in the shared buffer.
     * \param   buffer          The data to initialize byte buffer
     * \param   size            The length in bytes of data
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    SharedBuffer(uint32_t reserveSize, const uint8_t* buffer, uint32_t size, uint32_t blockSize = areg::BLOCK_SIZE);

    /**
     * \brief   Initializes the object and writes given null-terminated string into byte buffer,
     *          including the EOS character.
     *
     * \param   textString      The byte buffer as a null-terminated string
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    explicit SharedBuffer( const char * textString, uint32_t blockSize = areg::BLOCK_SIZE );
    /**
     * \brief   Initializes the object and writes given null-terminated wide string into byte
     *          buffer, including the EOS character.
     *
     * \param   textString      The byte buffer as a null-terminated string
     * \param   blockSize       The size of minimum block size to increase on resize. It is aligned
     *                          to areg::BLOCK_SIZE (minimum size)
     **/
    explicit SharedBuffer( const wchar_t * textString, uint32_t blockSize = areg::BLOCK_SIZE );

    /**
     * \brief   Does not copy data from source; instead refers to the same shared
     *          byte buffer object and increases the reference counter by one.
     **/
    SharedBuffer(const SharedBuffer& src);

    /**
     * \brief   Moves data from given source.
     **/
    SharedBuffer( SharedBuffer && src ) noexcept;

    virtual ~SharedBuffer() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    inline bool operator == (const SharedBuffer & other) const;

    inline bool operator != (const SharedBuffer & other) const;

    SharedBuffer & operator = (const SharedBuffer & src);

    SharedBuffer & operator = ( SharedBuffer && src ) noexcept;

/************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief   Extracts data from input stream and writes it to the shared buffer.
     *
     * \param   stream      The data streaming object to read data
     * \param[in,out] input       The Shared Buffer object to write data
     * \return  Reference to streaming object.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, SharedBuffer & input );

    /**
     * \brief   Writes data from shared buffer to output stream.
     *
     * \param[in,out] stream      The data streaming object to write data
     * \param   output      The Shared Buffer object to read data
     * \return  Reference to streaming object.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const SharedBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if buffer is invalid, current position is zero, or position is
     *          Cursor::INVALID_CURSOR_POSITION.
     **/
    [[nodiscard]]
    inline bool is_begin() const noexcept;

    /**
     * \brief   Returns true if the buffer is valid and the current position is equal to used size
     *          of buffer.
     **/
    [[nodiscard]]
    inline bool is_end() const noexcept;

    /**
     * \brief   Returns the pointer to the data buffer at current position, or nullptr if position
     *          is invalid or at end.
     **/
    [[nodiscard]]
    const uint8_t* current_ptr() const;

    /**
     * \brief   Returns the size of block set in the constructor.
     **/
    [[nodiscard]]
    inline uint32_t block_size() const noexcept;

    /**
     * \brief   Creates and returns a copy of the shared buffer with independent data.
     **/
    [[nodiscard]]
    SharedBuffer clone() const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// Cursor overrides
/************************************************************************/
    /**
     * \brief   Returns the current position of pointer relative to begin in streaming data.
     *
     * \return  Returns the current position of pointer relative to begin in streaming data.
     **/
    [[nodiscard]]
    uint32_t position() const override;

    /**
     * \brief   Sets the pointer position and returns current position in streaming data. Positive
     *          offset moves forward; negative moves back.
     *
     * \param   offset      The offset in bytes to move. Positive value means moving forward.
     *                      Negative value means moving back.
     * \param   startAt     Specifies the starting position of pointer and should have one of
     *                      values: Cursor::SeekOrigin::Begin -- position from the beginning of data
     *                      Cursor::SeekOrigin::Current -- position from current pointer position
     *                      Cursor::SeekOrigin::End -- position from the end of file
     * \return  If succeeds, returns the current position of pointer in bytes or value
     *          INVALID_CURSOR_POSITION if fails.
     **/
    uint32_t set_position( int32_t offset, Cursor::SeekOrigin startAt ) const override;

/************************************************************************/
// ByteBuffer interface overrides, not implemented in BufferStreamBase
/************************************************************************/

    /**
     * \brief   Returns true if buffer is shared between several byte buffer instances.
     **/
    bool is_shared() const noexcept final;

    /**
     * \brief   Returns true if buffer can be shared (always true for SharedBuffer).
     **/
    bool can_share() const noexcept final;

    /**
     * \brief   Invalidates the buffer, removes reference, and resets reading and writing positions.
     **/
    void invalidate() override;

protected:
/************************************************************************/
// ByteBuffer protected overrides
/************************************************************************/
    /**
     * \brief   Returns the offset value from the beginning of byte buffer.
     **/
    [[nodiscard]]
    uint32_t data_offset() const noexcept override;

    /**
     * \brief   Returns the size of data byte structure to allocate.
     **/
    [[nodiscard]]
    uint32_t header_size() const noexcept override;

    /**
     * \brief   Returns the size to align the buffer.
     **/
    [[nodiscard]]
    uint32_t aligned_size() const noexcept final;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns the default block size used by Shared Buffer.
     **/
    [[nodiscard]]
    static uint32_t default_block_size() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The size of block to increase at once every time when need to resize.
     *          This value is a constant and cannot be changed. Set during initialization.
     **/
    const uint32_t  mBlockSize;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    BufferPosition  mBufferPosition;

//////////////////////////////////////////////////////////////////////////
// Local function member
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns reference to this Shared Buffer.
     **/
    inline SharedBuffer & self();
};

//////////////////////////////////////////////////////////////////////////
// SharedBuffer class inline function implementation
//////////////////////////////////////////////////////////////////////////

inline bool SharedBuffer::operator == ( const SharedBuffer &other ) const
{
    return is_equal(other);
}

inline bool SharedBuffer::operator != ( const SharedBuffer &other ) const
{
    return (is_equal(other) == false);
}

inline bool SharedBuffer::is_begin() const noexcept
{
    uint32_t curPos = position();
    return ((is_valid() == false) || (curPos == 0) || (curPos == Cursor::INVALID_CURSOR_POSITION));
}

inline bool SharedBuffer::is_end() const noexcept
{
    return ( is_valid() && (position() == size_used()) );
}

inline uint32_t SharedBuffer::block_size() const noexcept
{
    return mBlockSize;
}

/************************************************************************/
// Friend streamable operators
/************************************************************************/

inline const InStream & operator >> (const InStream & stream, SharedBuffer & input)
{
    if ( static_cast<const InStream *>(&stream) != static_cast<const InStream *>(&input) )
    {
        stream.read(input);
        input.move_to_begin();
    }

    return stream;
}

inline OutStream & operator << (OutStream & stream, const SharedBuffer & output)
{
    if ( static_cast<const OutStream *>(&stream) != static_cast<const OutStream *>(&output) )
    {
        stream.write( output );
        output.move_to_begin();
    }

    return stream;
}

} // namespace areg
#endif  // AREG_BASE_SHAREDBUFFER_HPP
