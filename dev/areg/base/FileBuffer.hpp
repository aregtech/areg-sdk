#ifndef AREG_BASE_FILEBUFFER_HPP
#define AREG_BASE_FILEBUFFER_HPP
/************************************************************************
 * \file        areg/base/FileBuffer.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Memory Shared Buffer File object.
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
#include "areg/base/NEMemory.hpp"

/**
 * \brief   Memory Buffer File object. All read and write  operations are performed
 *          on the buffer located in memory. The class contains Shared Buffer object
 *          to minimize data copy operations when passesbe increasing reference count
 *              of byte buffer structure. The class supports all main
 *              operations for file access, using access modes. 
 *              For more details about file access modes, see description
 *              in class FileBase. For read-only operations, class
 *              has special attach mode, where the byte buffer is attached
 *              and only reading operation is allowed.
 *              This class also used a base class for data streaming 
 *              between threads.
 *
 **/

//////////////////////////////////////////////////////////////////////////
// FileBuffer class declaration
//////////////////////////////////////////////////////////////////////////
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
     *          to the size of NEMemory::BLOCK_SIZE. In case of read-only mode
     *          no buffer will be allocated and passed shared buffer will be used.
     **/
    static const unsigned int   BLOCK_SIZE  /*= 4 * NEMemory::BLOCK_SIZE*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    
    /**
     * \brief	Default constructor with default parameters
     * \param	mode	    The file open mode. By default file object is opened with write permission and in binary mode
     * \param	blockSize	The block size to increase. By default the block size is BLOCK_SIZE (64 bytes)
     **/
    FileBuffer(   int          mode      = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY)
                , const char*  name      = NULL
                , unsigned int blockSize = BLOCK_SIZE);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    FileBuffer(NEMemory::sByteBuffer & buffer, const char * name = NULL);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    FileBuffer(SharedBuffer & sharedBuffer, const char * name = NULL);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    FileBuffer(const SharedBuffer & sharedBuffer, const char* name = NULL);

    /**
     * \brief   Destructor
     **/
    virtual ~FileBuffer( void );

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, FileBuffer & input );
    /**
     * \brief   Friend global operator declaration to make File Buffer streamable.
     *          Writes the from file buffer to streaming object
     * \param	stream	The data streaming object to write data
     * \param	output	The File Buffer object to read data
     * \return	The instance of streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const FileBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns true if file is valid.
     **/
    inline bool isValid( void ) const;

    /**
     * \brief	If file object was opened and the the size is not zero (i.e. data was written / read) 
     *          it returns the pointer of buffer. Otherwise it will return NULL.
     **/
    inline const unsigned char* getDataBuffer( void ) const;

    /**
     * \brief   Returns reference to shared buffer object (for read only purpose)
     **/
    inline const SharedBuffer& getSharedBuffer( void ) const;

    /**
     * \brief	Inserts data to file buffer and returns the size of written data.
     *          All data within file buffer will be shifted.
     * \param	buffer	The pointer to data buffer
     * \param	size	The size in bytes of data buffer
     * \param   atPos   The position to insert data.
     * \return	Returns the size in bytes of written data.
     **/
    unsigned int insertAt( const unsigned char* buffer, unsigned int size, unsigned int atPos );

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
    virtual bool open( void );

    /**
     * \brief	Opens the file object. For memory buffered file the file name can be NULL.
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
     *                      For example, mode cannot contain (FO_MODE_ATTACH | FO_MODE_DETACH) at once.
     *                      One of bits will be ignored.
     *                      For more details see description of eFileOpenMode and eFileOpenBits.
     *
     * \return	Returns true if file was opened with success.
     **/
    virtual bool open(const char* fileName, unsigned int mode);

    /**
     * \brief   Call to close file object.
     *          If file was opened in FO_MODE_ATTACH or FO_MODE_DETACH modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FOR_DELETE or FO_MODE_CREATE_TEMP. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If FO_MODE_CREATE_TEMP is set, file object is always deleted on close.
     *          If FO_FOR_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     **/
    virtual void close( void );

    /**
     * \brief	Delete opened file. This will force to delete file object even if it is attached memory buffered file
     * \return	Returns true if succeeded.
     **/
    virtual bool remove( void );

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual unsigned int getLength( void ) const;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    virtual bool isOpened( void ) const;

    /**
     * \brief	Call to set new size of file object and returns the current position of pointer.
     *          If new size is more than the current size, the size will be increased, the existing 
     *          data and pointer position will not be changed. If new size is less than the current 
     *          size of file object, data will be truncated until the new size, and if needed 
     *          the pointer position will be moved to the end of file.
     *
     * \param	newSize     New Size is bytes to set. Positive value will set the size. Negative value will set size zero.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value INVALID_POINTER_POSITION.
     **/
    virtual unsigned int reserve(int newSize);

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, returns true.
     **/
    virtual bool truncate( void );

/************************************************************************/
// IEInStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( IEByteBuffer & buffer ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const;

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( unsigned char * buffer, unsigned int size ) const;

/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/
    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const IEByteBuffer & buffer );

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & asciiString );

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wideString );

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. In this class 
     *          writes data into opened file.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char* buffer, unsigned int size );

/************************************************************************/
// IECursorPosition interface overrides
/************************************************************************/
    /**
     * \brief	Sets the file pointer position and returns current position. 
     *          The positive value of offset means move pointer forward.
     *          The negative value of offset means move pointer back.
     *          For memory buffered file the pointer cannot move more than IECursorPosition::POSITION_END.
     *
     * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
     * \param	startAt	Specifies the starting position of pointer and should have one of values:
     *                  IECursorPosition::POSITION_BEGIN   -- position from beginning of file
     *                  IECursorPosition::POSITION_CURRENT -- from current pointer position
     *                  IECursorPosition::POSITION_END     -- from end of file
     *
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     **/
    virtual unsigned int setPosition(int offset, IECursorPosition::eCursorPosition startAt) const;

    /**
     * \brief	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     *          Before calling function, the file object should be opened.
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     **/
    virtual unsigned int getPosition( void ) const;

protected:
/************************************************************************/
// IEIOStream interface overrides
// includes IEInStream and IEOutStream interfaces
/************************************************************************/

    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int getSizeReadable( void ) const;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int getSizeWritable( void ) const;

/************************************************************************/
// Other overrides
/************************************************************************/
    /**
     * \brief	Validates and normalize bits for file open mode.
     * \param	mode	Integer value of bitwise OR operation of eFileOpenMode values
     * \return	Returns normalized value.
     **/
    virtual unsigned int normalizeMode(unsigned int mode) const;

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
// Forbidden functions / methods call
//////////////////////////////////////////////////////////////////////////
private:
    FileBuffer(const FileBuffer &);
    const FileBuffer& operator = (const FileBuffer &);
};

//////////////////////////////////////////////////////////////////////////
// FileBuffer class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const unsigned char* FileBuffer::getDataBuffer( void ) const
{
    return (isOpened() ? mSharedBuffer.getBuffer() : NULL);
}

inline const SharedBuffer& FileBuffer::getSharedBuffer( void ) const
{
    return mSharedBuffer;
}

inline bool FileBuffer::isValid( void ) const
{
    return mSharedBuffer.isValid();
}

inline bool FileBuffer::isEmpty( void ) const
{
    return ((isOpened() == false) || mSharedBuffer.isEmpty());
}

inline const IEInStream & operator >> ( const IEInStream & stream, FileBuffer & input )
{
    return (stream >> static_cast<SharedBuffer &>(input.mSharedBuffer));
}

inline IEOutStream & operator << ( IEOutStream & stream, const FileBuffer & output )
{
    return (stream << static_cast<const SharedBuffer &>(output.mSharedBuffer));
}

#endif  // AREG_BASE_FILEBUFFER_HPP
