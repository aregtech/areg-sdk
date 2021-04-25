#ifndef AREG_BASE_CEFILEBUFFER_HPP
#define AREG_BASE_CEFILEBUFFER_HPP
/************************************************************************
 * \file        areg/src/base/CEFileBuffer.hpp
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
#include "areg/src/base/CEFileBase.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/NEMemory.hpp"

/**
 * \brief       Memory Shared Buffer File object. All read and write
 *              operations are performed on the buffer located in memory.
 * 
 * \details     As a base class it is using Shared buffer to minimize
 *              data copy operations be increasing reference count
 *              of byte buffer structure. The class supports all main
 *              operations for file access, using access modes. 
 *              For more details about file access modes, see description
 *              in class CEFileBase. For read-only operations, class
 *              has special attach mode, where the byte buffer is attached
 *              and only reading operation is allowed.
 *              This class also used a base class for data streaming 
 *              between threads.
 *
 **/

//////////////////////////////////////////////////////////////////////////
// CEFileBuffer class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API CEFileBuffer : public    CEFileBase      // derive all basic functionalities of file operations
                            , protected CESharedBuffer  // protected derive shared buffer object to support
                                                        // data streaming operations.
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEFileBuffer::BLOCK_SIZE
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
    CEFileBuffer(  int         mode     = (CEFileBase::FO_MODE_WRITE | CEFileBase::FO_MODE_BINARY)
                 , const char* name     = NULL
                 , int        blockSize = BLOCK_SIZE);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    CEFileBuffer(NEMemory::sByteBuffer & buffer, const char * name = NULL);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    CEFileBuffer(CESharedBuffer & sharedBuffer, const char * name = NULL);

    /**
     * \brief	Constructor to set by default attached mode
     * \param	buffer	The reference to buffer to attach. 
     *                  If not invalid buffer (NEMemory::InvalidBuffer),
     *                  the default file open mode will be FO_MODE_ATTACH
     * \param	name	The name of file object. Can be NULL
     **/
    CEFileBuffer(const CESharedBuffer & sharedBuffer, const char* name = NULL);

    /**
     * \brief   Destructor
     **/
    virtual ~CEFileBuffer( void );

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
    friend inline const IEInStream & operator >> ( const IEInStream & stream, CEFileBuffer & input );
    /**
     * \brief   Friend global operator declaration to make File Buffer streamable.
     *          Writes the from file buffer to streaming object
     * \param	stream	The data streaming object to write data
     * \param	output	The File Buffer object to read data
     * \return	The instance of streaming object.
     **/
    friend inline IEOutStream & operator << ( IEOutStream & stream, const CEFileBuffer & output );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if buffer is either empty or is invalid.
     **/
    bool IsEmpty( void ) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// CEFileBase class virtual function overrides
/************************************************************************/
    /**
     * \brief   Returns the name of file object set by user. This can be either short name or full path. Can be NULL for buffered file.
     * \return  Returns the given name of file.
     **/
    virtual const char* GetName( void ) const;

    /**
     * \brief   Returns the file open mode (bits)
     **/
    virtual int GetMode( void ) const;

    /**
     * \brief   Opens the file. No operation can be performed before file is opened.
     *          For memory buffered file at least the file open mode should be set.
     *          For file system file object additionally the file name (or full / relative path) should be set.
     *          If mentioned parameters are not set, the open operation will fail and return false.
     *          If file object was already opened, second call to open file will fail.
     *
     * \return  Returns true if file object was opened with success.
     **/
    virtual bool Open( void );

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
    virtual bool Open(const char* fileName, int mode);

    /**
     * \brief   Call to close file object.
     *          If file was opened in FO_MODE_ATTACH or FO_MODE_DETACH modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FORCE_DELETE or FO_MODE_CREATE_TEMP. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If FO_MODE_CREATE_TEMP is set, file object is always deleted on close.
     *          If FO_FORCE_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     **/
    virtual void Close( void );

    /**
     * \brief	Delete opened file. This will force to delete file object even if it is attached memory buffered file
     * \return	Returns true if succeeded.
     **/
    virtual bool Remove( void );

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual unsigned int GetLength( void ) const;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    virtual bool IsOpened() const;

    /**
     * \brief	Call to reserve space or set new size of file object and returns the current position of pointer.
     *          If new size is positive value, the size will be increased. Otherwise the reserved size will set size zero.
     *          If new size is more than the current size of file object, no data will be lost, no pointer position will be changed.
     *          If new size is less than the current size of file object, data will be truncated until the new size, 
     *          and if the new size is less than the current pointer position, the pointer will be move at the end of file.
     * 
     * \param	size	New Size is bytes to reserve or set. Positive value will set the size. Negative value will set size zero.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value INVALID_POINTER_POSITION.
     **/
    virtual unsigned int Reserve(int newSize);

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, returns true.
     **/
    virtual bool PurgeData( void );

/************************************************************************/
// IEIOStream interface overrides
// includes IEInStream and IEOutStream interfaces
/************************************************************************/
    /**
     * \brief	Write data to file buffer and returns the size of written data
     * \param	buffer	The pointer to data buffer
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const unsigned char* buffer, unsigned int size );

    /**
     * \brief	Read data from file buffer, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int Read( unsigned char* buffer, unsigned int size ) const;

    /**
     * \brief	Inserts data to file buffer and returns the size of written data.
     *          All data within file buffer will be shifted.
     * \param	buffer	    The pointer to data buffer
     * \param	size	    The size in bytes of data buffer
     * \param   insertAt    The position to insert data.
     * \return	Returns the size in bytes of written data.
     **/
    unsigned int Insert( const unsigned char* buffer, unsigned int size, unsigned int insertAt );

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const IEByteBuffer & buffer );

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int Read( IEByteBuffer & buffer ) const;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEString & asciiString );

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEWideString & wideString );

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEString & asciiString ) const;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEWideString & wideString ) const;

public:
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
    virtual unsigned int SetCursorPosition(int offset, IECursorPosition::eCursorPosition startAt) const;

    /**
     * \brief	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     *          Before calling function, the file object should be opened.
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     **/
    virtual unsigned int GetCursorPosition( void ) const;

protected:
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
    virtual unsigned int SetPosition(int offset, IECursorPosition::eCursorPosition startAt) const;

    /**
     * \brief	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     *          Before calling function, the file object should be opened.
     * \return	If succeeds, returns the current position of pointer in bytes or value INVALID_POINTER_POSITION if fails.
     **/
    virtual unsigned int GetPosition( void ) const;

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
    virtual unsigned int GetReadableSize( void ) const;

    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int GetWritableSize( void ) const;

/************************************************************************/
// CEBufferStreamBase interface, overrides
/************************************************************************/
    /**
     * \brief   Inserts buffer of data at the given position and returns
     *          size of inserted data
     * \param   buffer      The pointer to data buffer to insert
     * \param   size        The size of data buffer
     * \param   insertAt    The position to insert data. If position is
     *                      not valid, it will add at the end
     * \return  Returns size in bytes of written data.
     **/
    virtual unsigned int InsertData(const unsigned char* buffer, unsigned int size, unsigned int insertAt);

protected:
/************************************************************************/
// Other overrides
/************************************************************************/
    /**
     * \brief	Validates and normalize bits for file open mode.
     * \param	mode	Integer value of bitwise OR operation of eFileOpenMode values
     * \return	Returns normalized value.
     **/
    virtual int NormalizeMode(int mode) const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	If file object was opened and the the size is not zero (i.e. data was written / read) 
     *          it returns the pointer of buffer. Otherwise it will return NULL.
     **/
    inline const unsigned char* GetDataBuffer( void ) const;

    /**
     * \brief   Returns reference to shared buffer object (for read only purpose)
     **/
    inline const CESharedBuffer& GetSharedBuffer( void ) const;

//////////////////////////////////////////////////////////////////////////
// Private functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets the file object name. For internal call.
     **/
    inline void SetName(const char* name);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   File Open mode
     **/
    int             mFileMode;

    /**
     * \brief   File object name, can be empty
     **/
    CEString        mFileName;

    /**
     * \brief   If true, file object was opened.
     **/
    bool            mIsOpened;

//////////////////////////////////////////////////////////////////////////
// Forbidden functions / methods call
//////////////////////////////////////////////////////////////////////////
private:
    CEFileBuffer(const CEFileBuffer &);
    const CEFileBuffer& operator = (const CEFileBuffer &);
};

//////////////////////////////////////////////////////////////////////////
// CEFileBuffer class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline const unsigned char* CEFileBuffer::GetDataBuffer( void ) const
{   return (IsOpened() ? CESharedBuffer::GetBuffer() : NULL);       }

inline const CESharedBuffer& CEFileBuffer::GetSharedBuffer( void ) const
{   return static_cast<const CESharedBuffer &>(*this);              }

inline const IEInStream & operator >> ( const IEInStream & stream, CEFileBuffer & input )
{   return (stream >> static_cast<CESharedBuffer &>(input));        }

inline IEOutStream & operator << ( IEOutStream & stream, const CEFileBuffer & output )
{   return (stream << static_cast<const CESharedBuffer &>(output)); }

#endif  // AREG_BASE_CEFILEBUFFER_HPP
