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

//////////////////////////////////////////////////////////////////////////
// FileBuffer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Memory Buffer File object. All read and write  operations are performed
 *          on the buffer located in memory. The class contains Shared Buffer object
 *          to minimize data copy operations. The class supports all main
 *          operations for file access, using access modes. For more details about 
 *          file access modes, see description in class FileBase. For read-only operations,
 *          class has special attach mode, where the byte buffer is attached and only 
 *          reading operation is allowed. This class also used a base class for data streaming 
 *          between threads.
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
     * \brief	Default constructor with default parameters
     * \param	mode	    The file open mode. By default file object is opened with write permission and in binary mode
     * \param	blockSize	The block size to increase. By default the block size is 4 x areg::BLOCK_SIZE (default 64 bytes)
     **/
    explicit FileBuffer( uint32_t mode      = (static_cast<uint32_t>(FileBase::OpenMode::Write) | static_cast<uint32_t>(FileBase::OpenMode::Binary))
                       , const char*  name      = nullptr
                       , uint32_t blockSize = FileBuffer::BLOCK_SIZE);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	sharedBuffer    The referenced object to shared buffer to attach. 
     * \param	name	        The name of file object. Can be nullptr.
     **/
    FileBuffer(SharedBuffer & sharedBuffer, const char * name = nullptr);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	sharedBuffer    The referenced object to shared buffer to attach.
     * \param	name	        The name of file object. Can be nullptr.
     **/
    FileBuffer(const SharedBuffer & sharedBuffer, const char* name = nullptr);

    /**
     * \brief   Destructor
     **/
    virtual ~FileBuffer() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Friend global operators to stream Shared Buffer
/************************************************************************/

    /**
     * \brief	Friend global operator declaration to make File Buffer streamable.
     *          Writes data from streaming object to file buffer
     * \param	stream	The data streaming object to read data
     * \param	input	The File Buffer object to write data
     * \return	The instance of streaming object.
     **/
    friend inline const InStream & operator >> ( const InStream & stream, FileBuffer & input );
    /**
     * \brief   Friend global operator declaration to make File Buffer streamable.
     *          Writes the from file buffer to streaming object
     * \param	stream	The data streaming object to write data
     * \param	output	The File Buffer object to read data
     * \return	The instance of streaming object.
     **/
    friend inline OutStream & operator << ( OutStream & stream, const FileBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    inline bool isEmpty() const;

    /**
     * \brief   Returns true if file is valid.
     **/
    inline bool isValid() const;

    /**
     * \brief	If file object was opened and the size is not zero (i.e. data was written / read) 
     *          it returns the pointer of buffer. Otherwise it will return nullptr.
     **/
    inline const uint8_t * getDataBuffer() const;

    /**
     * \brief   Returns reference to shared buffer object (for read only purpose)
     **/
    inline const SharedBuffer & getSharedBuffer() const;

    /**
     * \brief	Inserts data to file buffer and returns the size of written data.
     *          All data within file buffer will be shifted.
     * \param	buffer	The pointer to data buffer
     * \param	size	The size in bytes of data buffer
     * \param   atPos   The position to insert data.
     * \return	Returns the size in bytes of written data.
     **/
    uint32_t insertAt( const uint8_t* buffer, uint32_t size, uint32_t atPos );

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// FileBase class virtual function overrides
/************************************************************************/
    /**
     * \brief   Opens the file. No operation can be performed before file is opened.
     *          For memory buffered file at least the file open mode should be set.
     *          For file system file object additionally the file name (or full / relative path) should be set.
     *          If mentioned parameters are not set, the open operation will fail and return false.
     *          If file object was already opened, second call to open file will fail.
     *
     * \return  Returns true if file object was opened with success.
     **/
    bool open() override;

    /**
     * \brief	Opens the file object. For memory buffered file the file name can be nullptr.
     *          Any other name for memory buffered file object will have only symbolic meaning
     *          and will be ignored during open operation.
     *          For file system file object the file name should contain one of paths:
     *              a. only file name   -- means the file is located in current directory;
     *              b. relative path    -- mens relative to current directory file path;
     *              c. full path (including drive letter or network path) -- means full path to file object.
     *          For file system file object this also can be directory name.
     *
     * \param	fileName	The name or path (relative or full path) of file object. 
     *                      Can be either file object or directory.
     *
     * \param	mode	    The opening modes. The value should be combined with bitwise OR operation.
     *                      Before opening, the conflicting bits are removed.
     *                      For example, mode cannot contain (Attach | Detach) at once.
     *                      One of bits will be ignored.
     *                      For more details see description of OpenMode and OpenFlag.
     *
     * \return	Returns true if file was opened with success.
     **/
    bool open(const String& fileName, uint32_t mode) override;

    /**
     * \brief   Call to close file object.
     *          If file was opened in Attach or Detach modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FOR_DELETE or CreateTemp. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If CreateTemp is set, file object is always deleted on close.
     *          If FO_FOR_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     **/
    void close() override;

    /**
     * \brief	Delete opened file. This will force to delete file object even if it is attached memory buffered file
     * \return	Returns true if succeeded.
     **/
    bool remove() override;

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    uint32_t getLength() const override;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    bool isOpened() const override;

    /**
     * \brief	Call to set new size of file object and returns the current position of pointer.
     *          If new size is more than the current size, the size will be increased, the existing 
     *          data and pointer position will not be changed. If new size is less than the current 
     *          size of file object, data will be truncated until the new size, and if needed 
     *          the pointer position will be moved to the end of file.
     *
     * \param	newSize     New Size is bytes to set.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value Cursor::INVALID_CURSOR_POSITION.
     **/
    uint32_t reserve(uint32_t newSize) override;

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, returns true.
     **/
    bool truncate() override;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    uint32_t read( ByteBuffer & buffer ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   ascii     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wide      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t read( WideString & wide ) const override;

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    uint32_t read( uint8_t * buffer, uint32_t size ) const override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    uint32_t write( const ByteBuffer & buffer ) override;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   ascii     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wide  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    uint32_t write( const WideString & wide ) override;

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. In this class 
     *          writes data into opened file.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    uint32_t write( const uint8_t * buffer, uint32_t size ) override;

/************************************************************************/
// Cursor interface overrides
/************************************************************************/
    /**
     * \brief	Sets the file pointer position and returns current position. 
     *          The positive value of offset means move pointer forward.
     *          The negative value of offset means move pointer back.
     *          For memory buffered file the pointer cannot move more than Cursor::SeekOrigin::End.
     *
     * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
     * \param	startAt	Specifies the starting position of pointer and should have one of values:
     *                  Cursor::SeekOrigin::Begin   -- position from beginning of file
     *                  Cursor::SeekOrigin::Current -- from current pointer position
     *                  Cursor::SeekOrigin::End     -- from end of file
     *
     * \return	If succeeds, returns the current position of pointer in bytes or value Cursor::INVALID_CURSOR_POSITION if fails.
     **/
    uint32_t setPosition(int32_t offset, Cursor::SeekOrigin startAt) const override;

    /**
     * \brief	If succeeds, returns the current position of pointer in bytes or value Cursor::INVALID_CURSOR_POSITION if fails.
     *          Before calling function, the file object should be opened.
     * \return	If succeeds, returns the current position of pointer in bytes or value Cursor::INVALID_CURSOR_POSITION if fails.
     **/
    uint32_t getPosition() const override;

protected:
/************************************************************************/
// IOStream interface overrides
// includes InStream and OutStream interfaces
/************************************************************************/

    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    uint32_t getSizeReadable() const override;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    uint32_t getSizeWritable() const override;

/************************************************************************/
// Other overrides
/************************************************************************/
    /**
     * \brief	Validates and normalize bits for file open mode.
     * \param	mode	Integer value of bitwise OR operation of OpenMode values
     * \return	Returns normalized value.
     **/
    uint32_t normalizeMode(uint32_t mode) const override;

//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets the file object name. For internal call.
     **/
    inline void _setName(const char* name);

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

inline const uint8_t * FileBuffer::getDataBuffer() const
{
    return (isOpened() ? mSharedBuffer.getBuffer() : nullptr);
}

inline const SharedBuffer & FileBuffer::getSharedBuffer() const
{
    return mSharedBuffer;
}

inline bool FileBuffer::isValid() const
{
    return mSharedBuffer.isValid();
}

inline bool FileBuffer::isEmpty() const
{
    return ((isOpened() == false) || mSharedBuffer.isEmpty());
}

inline const InStream & operator >> ( const InStream & stream, FileBuffer & input )
{
    return (stream >> static_cast<SharedBuffer &>(input.mSharedBuffer));
}

inline OutStream & operator << ( OutStream & stream, const FileBuffer & output )
{
    return (stream << static_cast<const SharedBuffer &>(output.mSharedBuffer));
}

#endif  // AREG_BASE_FILEBUFFER_HPP
