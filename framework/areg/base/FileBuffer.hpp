#ifndef AREG_BASE_FILEBUFFER_HPP
#define AREG_BASE_FILEBUFFER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/FileBuffer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Memory Shared Buffer File object.
 *              This class behaves as a File object. All write and read
 *              operations are performed on the buffer in memory.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/FileBase.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/MemoryDefs.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// FileBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Memory-based file buffer that performs all read and write operations on in-memory
 *          buffers. Supports shared buffer ownership and attach/detach modes for efficient data
 *          handling without unnecessary copying.
 **/
class AREG_API FileBuffer   : public    FileBase      // derive all basic functionalities of file operations
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   FileBuffer::BLOCK_SIZE
     *          Default block size in bytes. Used to increase buffer size
     *          during writing operation. The buffer allocation is aligned
     *          to the size of areg::BLOCK_SIZE. In case of read-only mode
     *          no buffer will be allocated and passed shared buffer will be used.
     **/
    static constexpr uint32_t   BLOCK_SIZE  { 4 * areg::BLOCK_SIZE };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief   Constructs a FileBuffer with specified open mode and block size. Default mode is
     *          write in binary; blockSize defaults to 4 × BLOCK_SIZE (256 bytes).
     *
     * \param   mode            File open mode flags (default: Write | Binary).
     * \param   blockSize       Block size for allocation increments (default: 256 bytes).
     **/
    explicit FileBuffer( uint32_t mode      = (static_cast<uint32_t>(FileBase::OpenMode::Write) | static_cast<uint32_t>(FileBase::OpenMode::Binary))
                       , const char*  name      = nullptr
                       , uint32_t blockSize = FileBuffer::BLOCK_SIZE);

    /**
     * \brief   Constructs a FileBuffer in attached mode from a non-const SharedBuffer reference.
     *
     * \param   sharedBuffer    The SharedBuffer to attach to.
     * \param   name            Optional name for the file object.
     **/
    FileBuffer(SharedBuffer & sharedBuffer, const char * name = nullptr);

    /**
     * \brief   Constructs a FileBuffer in attached read-only mode from a const SharedBuffer
     *          reference.
     *
     * \param   sharedBuffer    The SharedBuffer to attach to (read-only).
     * \param   name            Optional name for the file object.
     **/
    FileBuffer(const SharedBuffer & sharedBuffer, const char* name = nullptr);

    virtual ~FileBuffer() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief   Deserializes data from the input stream into the file buffer.
     *
     * \param   stream      The input stream to read from.
     * \param[in,out] input       The FileBuffer to receive deserialized data.
     * \return  Reference to the input stream.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, FileBuffer & input );
    /**
     * \brief   Serializes data from the file buffer to the output stream.
     *
     * \param[in,out] stream      The output stream to write to.
     * \param   output      The FileBuffer to read from.
     * \return  Reference to the output stream.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const FileBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    [[nodiscard]]
    inline bool is_empty() const noexcept;

    [[nodiscard]]
    inline bool is_valid() const noexcept;

    /**
     * \brief   Returns a pointer to the buffer data if the file is opened and non-empty; otherwise
     *          returns nullptr.
     *
     * \return  Pointer to buffer data, or nullptr.
     **/
    inline const uint8_t * data_buffer() const;

    /**
     * \brief   Returns a const reference to the underlying shared buffer.
     *
     * \return  Const reference to the SharedBuffer.
     **/
    inline const SharedBuffer & shared_buffer() const;

    /**
     * \brief   Inserts data into the file buffer at the specified position. Existing data is
     *          shifted.
     *
     * \param   buffer      The data buffer to insert.
     * \param   size        The size in bytes of the data.
     * \param   atPos       The position to insert at.
     * \return  The number of bytes written.
     **/
    uint32_t insert_at( const uint8_t* buffer, uint32_t size, uint32_t atPos );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// FileBase class virtual function overrides
/************************************************************************/
    /**
     * \brief   Opens the file buffer. The file must be configured with open mode. For memory
     *          buffers, no file name is required. For file system buffers, the file name must be
     *          set.
     *
     * \return  True if opened successfully; false if already open or required parameters not set.
     **/
    bool open() override;

    /**
     * \brief   Opens the file buffer with the specified name and mode. For memory buffers, the file
     *          name is optional and symbolic. For file system buffers, name can be a file,
     *          directory, or relative/full path.
     *
     * \param   fileName    The file name, path (relative or full), or directory.
     * \param   mode        Open mode flags combined with bitwise OR. Conflicting flags
     *                      (Attach|Detach) are resolved by removing one.
     * \return  True if opened successfully; false otherwise.
     **/
    bool open(const String& fileName, uint32_t mode) override;

    /**
     * \brief   Closes the file buffer. In attach mode, the buffer is not deleted unless
     *          FO_FOR_DELETE or CreateTemp flag is set.
     **/
    void close() override;

    /**
     * \brief   Deletes the opened file. Forces deletion even if in attach mode.
     *
     * \return  True if successful; false otherwise.
     **/
    bool remove() override;

    /**
     * \brief   Returns the current valid length of file data in bytes, or INVALID_SIZE if no data.
     *
     * \return  Length in bytes, or INVALID_SIZE.
     **/
    uint32_t length() const noexcept override;

    /**
     * \brief   Returns true if the file is currently open.
     **/
    [[nodiscard]]
    bool is_opened() const noexcept override;

    /**
     * \brief   Resizes the file buffer. Enlarges buffer without truncating data or moving the
     *          pointer. Shrinking truncates data and may move the pointer to end.
     *
     * \param   newSize     The new size in bytes.
     * \return  The current file pointer position, or INVALID_CURSOR_POSITION if failed.
     **/
    uint32_t reserve(uint32_t newSize) override;

    /**
     * \brief   Clears all data from the buffer and sets size to zero.
     *
     * \return  True if successful; false otherwise.
     **/
    bool truncate() override;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from the buffer and copies into the provided SharedBuffer object.
     *
     * \param[out] buffer      The SharedBuffer to receive the data.
     * \return  The number of bytes copied.
     **/
    uint32_t read(SharedBuffer& buffer ) const override;

    /**
     * \brief   Reads string data from the buffer and copies into the provided ASCII String.
     *
     * \param[out] ascii       The ASCII String to receive the data.
     * \return  The number of bytes copied.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads string data from the buffer and copies into the provided WideString.
     *
     * \param[out] wide    The WideString to receive the data.
     * \return  The number of bytes copied.
     **/
    uint32_t read( WideString & wide ) const override;

    /**
     * \brief   Reads data from the buffer and copies into the provided byte array.
     *
     * \param[out] buffer      The byte array to receive the data.
     * \param   size        The size in bytes of the array.
     * \return  The number of bytes copied.
     **/
    uint32_t read( uint8_t * buffer, uint32_t size ) const noexcept override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief   Writes data from the SharedBuffer object to the file buffer.
     *
     * \param   buffer      The SharedBuffer to read from.
     * \return  The number of bytes written.
     **/
    uint32_t write( const SharedBuffer& buffer ) override;

    /**
     * \brief   Writes string data from the ASCII String object to the file buffer.
     *
     * \param   ascii       The ASCII String to read from.
     * \return  The number of bytes written.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes string data from the WideString object to the file buffer.
     *
     * \param   wide    The WideString to read from.
     * \return  The number of bytes written.
     **/
    uint32_t write( const WideString & wide ) override;

    /**
     * \brief   Writes data from the given byte array to the file buffer.
     *
     * \param   buffer      The byte array to read from.
     * \param   size        The size in bytes of the data.
     * \return  The number of bytes written.
     **/
    uint32_t write( const uint8_t * buffer, uint32_t size ) noexcept override;

/************************************************************************/
// Cursor interface overrides
/************************************************************************/
    /**
     * \brief   Moves the file pointer by the specified offset from the start position. For memory
     *          buffers, pointer cannot move beyond end.
     *
     * \param   offset      Offset in bytes. Positive moves forward; negative moves backward.
     * \param   startAt     Start position: Begin, Current, or End.
     * \return  Current pointer position, or INVALID_CURSOR_POSITION if failed.
     **/
    uint32_t set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept override;

    /**
     * \brief   Returns the current file pointer position in bytes. File must be opened.
     *
     * \return  Current position in bytes, or INVALID_CURSOR_POSITION if failed.
     **/
    uint32_t position() const noexcept override;

protected:
/************************************************************************/
// IOStream interface overrides
// includes InStream and OutStream interfaces
/************************************************************************/

    /**
     * \brief   Returns the number of bytes remaining to read from the current pointer position.
     *
     * \return  Number of bytes available to read.
     **/
    [[nodiscard]]
    uint32_t size_readable() const noexcept override;

    /**
     * \brief   Returns the number of bytes of available space remaining to write from the current
     *          pointer position.
     *
     * \return  Number of bytes available to write.
     **/
    [[nodiscard]]
    uint32_t size_writable() const noexcept override;

/************************************************************************/
// Other overrides
/************************************************************************/
    /**
     * \brief   Validates and normalizes file open mode flags.
     *
     * \param   mode    The open mode flags as a bitwise OR of OpenMode values.
     * \return  Normalized mode flags.
     **/
    uint32_t normalize_mode(uint32_t mode) const noexcept override;

//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets the file name. For internal use only.
     **/
    inline void _set_name(const char* name);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Shared Buffer object
     **/
    SharedBuffer    mSharedBuffer;
    /**
     * \brief   If true, file object was opened.
     **/
    bool            mIsOpened;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( FileBuffer );
};

//////////////////////////////////////////////////////////////////////////
// FileBuffer class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const uint8_t * FileBuffer::data_buffer() const
{
    return (is_opened() ? mSharedBuffer.buffer() : nullptr);
}

inline const SharedBuffer & FileBuffer::shared_buffer() const
{
    return mSharedBuffer;
}

inline bool FileBuffer::is_valid() const noexcept
{
    return mSharedBuffer.is_valid();
}

inline bool FileBuffer::is_empty() const noexcept
{
    return ((is_opened() == false) || mSharedBuffer.is_empty());
}

inline const InStream & operator >> ( const InStream & stream, FileBuffer & input )
{
    return (stream >> static_cast<SharedBuffer &>(input.mSharedBuffer));
}

inline OutStream & operator << ( OutStream & stream, const FileBuffer & output )
{
    return (stream << static_cast<const SharedBuffer &>(output.mSharedBuffer));
}

} // namespace areg
#endif  // AREG_BASE_FILEBUFFER_HPP
