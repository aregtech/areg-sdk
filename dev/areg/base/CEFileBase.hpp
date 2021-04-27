#ifndef AREG_BASE_CEFILEBASE_HPP
#define AREG_BASE_CEFILEBASE_HPP
/************************************************************************
 * \file        areg/base/CEFileBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Pure virtual base class of File object. File open, close read, write and other functions
 *              should be overwritten. Used by memory file, normal file and memory mapped file classes.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/IECursorPosition.hpp"

#include "areg/base/private/CEReadConverter.hpp"
#include "areg/base/private/CEWriteConverter.hpp"

class CEString;
class CEWideString;

/**
 * \brief       Pure virtual base class of File object. File open, close read, write and other functions
 *              should be overwritten. Used by memory file, normal file and memory mapped file classes.
 *
 * \details     This is pure virtual class to perform main file functionalities.
 *              The classes derived from this class should also support data streaming, i.e.
 *              should support read / write, as well as should contain read / write position.
 *              objects to move pointer.
 *              Before performing any read and write operations, the file object should be opened.
 *              Otherwise the operation will fail. For details see function comments.
 *              
 *              Some use of File Open Mode:
 *
 *              1. FO_MODE_INVALID      - Invalid mode. No operation should be performed. Prevent caller setting this mode;
 *              
 *              2. FO_MODE_READ         - File opened for reading. By default, all files should be opened with this flag.
 *
 *              3. FO_MODE_WRITE        - File opened for writing and reading. If flag is missed, the write functionality will be disabled.
 *
 *              4. FO_MODE_BINARY       - File opened in binary mode. All data will be read and written as binary data.
 *                                        For strings, the null-terminate symbol will be also written / read.
 *
 *              5. FO_MODE_TEXT         - File opened int text mode. All data will be written as plain text, 
 *                                        until does not match null-terminate symbol.
 *              
 *              6. FO_MODE_SHARE_READ   - File is opened in share read mode. The other File object is able to read.
 *                                        This flag makes sense for FS File object. For memory file it does not make sense,
 *                                        since buffer pointer may change address and File object is not able to control 
 *                                        memory access by other functionality.
 *
 *              7. FO_MODE_SHARE_WRITE  - File is opened in share write and read modes. The other File object is able to write and read data.
 *                                        This flag makes sense for FS File object. For memory file it does not make sense,
 *                                        since buffer pointer may change address and File object is not able to control 
 *                                        memory access by other functionality.
 *              
 *              8. FO_MODE_CREATE       - If file does not exist, the new file will be created. The size will set zero
 *                                        If FO_MODE_EXIST flag is set, will try to open existing file, otherwise creates new file.
 *              
 *              9. FO_MODE_EXIST        - Opens only existing file. Opening might fail if file does not exist and 'FO_MODE_CREATE' flag is not set.
 *                                        If FO_MODE_CREATE flag is set and file does not exist, it will create new file.
 *                                        If FO_MODE_TRUNCATE flag is set, opens file for read/write and sets initial size of file to zero.
 *              
 *             10. FO_MODE_TRUNCATE     - Opens file in read and write mode, sets initial size of file to zero. 
 *                                        Ignored in mode FO_MODE_ATTACH.
 *                                        Can be used with combination FO_MODE_CREATE and FO_MODE_EXIST.
 *
 *             11. FO_MODE_ATTACH       - Will use existing file handle / memory buffer pointer only in read mode. 
 *                                        On close neither handle, nor buffer pointer will be closed / deleted.
 *             
 *             12. FO_MODE_DETACH       - Will open file / memory buffer for read and write access.
 *                                        On close neither handle, nor buffer pointer will be closed / deleted.
 *
 *             12. FO_MODE_FORCE_DELETE - Will force to delete on close even if buffer is attached or marked as detached.
 *
 **/
//////////////////////////////////////////////////////////////////////////
// CEFileBase class declaration.
//////////////////////////////////////////////////////////////////////////
class AREG_API CEFileBase   : public    IEIOStream
                            , protected IECursorPosition
{
//////////////////////////////////////////////////////////////////////////
// Defined types
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   File open bits specifying opening mode, access rights and attributes
     **/
    typedef enum E_FileOpenBits
    {
          FOB_INVALID       = 0     //!< 0000000000000000 <= invalid
        , FOB_READ          = 1     //!< 0000000000000001 <= read bit
        , FOB_WRITE         = 2     //!< 0000000000000010 <= write bit
        , FOB_BINARY        = 4     //!< 0000000000000100 <= binary bit
        , FOB_TEXT          = 8     //!< 0000000000001000 <= text bit
        , FOB_SHARE_READ    = 16    //!< 0000000000010000 <= shared read bit
        , FOB_SHARE_WRITE   = 32    //!< 0000000000100000 <= shared write bit
        , FOB_CREATE        = 64    //!< 0000000001000000 <= create bit
        , FOB_EXIST         = 128   //!< 0000000010000000 <= must exist bit
        , FOB_TRUNCATE      = 256   //!< 0000000100000000 <= truncate bit
        , FOB_ATTACH        = 512   //!< 0000001000000000 <= attached bit
        , FOB_DETACH        = 1024  //!< 0000010000000000 <= detach bit
        , FOB_FORCE_DELETE  = 2048  //!< 0000100000000000 <= delete on close bit
        , FOB_WRITE_DIRECT  = 4096  //!< 0001000000000000 <= write direct on disk bit
        , FOB_TEMP_FILE     = 8192  //!< 0010000000000000 <= create temporary file bit, will be deleted on close.

    } eFileOpenBits;

public:
    /**
     * \brief   File opening modes.
     **/
    typedef enum E_FileOpenMode
    {
          FO_MODE_INVALID       = (FOB_INVALID)                                                     //!< 0000000000000000 <= invalid mode

        , FO_MODE_READ          = (FOB_READ)                                                        //!< 0000000000000001 <= read bit
        , FO_MODE_WRITE         = (FOB_WRITE | FOB_READ)                                            //!< 0000000000000011 <= write mode, should contain "read" bit
        , FO_MODE_BINARY        = (FOB_BINARY)                                                      //!< 0000000000000100 <= binary bit. strings in binary mode will write EOS char, all data will be dumpped.
        , FO_MODE_TEXT          = (FOB_TEXT)                                                        //!< 0000000000001000 <= text bit. EOS char of string will not be written, can write and read line of string
        , FO_MODE_SHARE_READ    = (FOB_SHARE_READ | FOB_READ)                                       //!< 0000000000010001 <= share read mode, should contain "read" bit
        , FO_MODE_SHARE_WRITE   = (FOB_SHARE_WRITE | FOB_SHARE_READ | FOB_WRITE | FOB_READ)         //!< 0000000000110011 <= share write mode, should contain "read", "write" and "share read" bits
        , FO_MODE_CREATE        = (FOB_CREATE)                                                      //!< 0000000001000000 <= always create file
        , FO_MODE_EXIST         = (FOB_EXIST)                                                       //!< 0000000010000000 <= file should exist, otherwise it failes
        , FO_MODE_TRUNCATE      = (FOB_TRUNCATE | FOB_WRITE | FOB_READ)                             //!< 0000000100000011 <= truncate file, i.e. set initial size zero, "read" and "write" bits should be set
        , FO_MODE_ATTACH        = (FOB_ATTACH | FOB_EXIST | FOB_SHARE_READ | FOB_READ)              //!< 0000001010010001 <= attached handle (buffer). Can only read memory data, cannot change the size or write data. Bits "read", "share read", "exist" are set automaticaly. Not able to control share mode, the pointer should be passed. Should not free buffer in destructor.
        , FO_MODE_DETACH        = (FOB_DETACH | FOB_CREATE | FOB_SHARE_READ | FOB_WRITE | FOB_READ) //!< 0000010001010011 <= mode detach buffer. Can read and write memory data. Bits "read", "write", "share read" and "create" are set automaticaly. Not able to control share modes. Should not free buffer in destructor, it is up to caller to free buffer.
        , FO_FORCE_DELETE       = (FOB_FORCE_DELETE)                                                //!< 0000100000000000 <= mode to delete on close. Can be combined with any mode. The file / buffer will be deleted even if mode attached / detach are set.
        , FO_MODE_WRITE_DIRECT  = (FOB_WRITE_DIRECT | FOB_WRITE | FOB_READ)                         //!< 0001000000000011 <= write operations will not go through any intermediate cache, they will go directly to disk. read and write flags are set automatically.
        , FO_MODE_CREATE_TEMP   = (FOB_TEMP_FILE | FOB_WRITE | FOB_READ)                            //!< 0010000000000011 <= The file is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because an application deletes a temporary file after a handle is closed. In that case, the system can entirely avoid writing the data.

    } eFileOpenMode;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   CEFileBase::AUTO_ALLOC
     *          Constant. It is a flag, indicating that buffer size
     *          should be allocated automatically by class 
     **/
    static const int          AUTO_ALLOC    /*= static_cast<int>(-1)*/;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default protected constructor.
     **/
    CEFileBase( void );

public:
    /**
     * \brief   Destructor
     **/
    virtual ~CEFileBase( void );


//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns reference to the streaming object to read data
     **/
    const IEInStream & GetReadStream( void ) const;

    /**
     * \brief   Returns reference to the streaming object to write data
     **/
    IEOutStream & GetWriteStream( void );

//////////////////////////////////////////////////////////////////////////
// Override methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the name of file object set by user. 
     *          This can be either short name or full path. 
     *          Can be NULL for buffered file.
     * \return  Returns the given name of file.
     **/
    virtual const char* GetName( void ) const = 0;

    /**
     * \brief   Returns the file open mode (bits)
     **/
    virtual int GetMode( void ) const = 0;

    /**
     * \brief   Opens the file. No operation can be performed before file is opened.
     *          For memory buffered file at least the file open mode should be set.
     *          For file system file object additionally the file name (or full / relative path) should be set.
     *          If mentioned parameters are not set, the open operation will fail and return false.
     *          If file object was already opened, second call to open file will fail.
     *
     * \return  Returns true if file object was opened with success.
     * \see     Close()
     **/
    virtual bool Open( void ) = 0;

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
     * \see     Close()
     **/
    virtual bool Open(const char* fileName, int mode) = 0;

    /**
     * \brief   Call to close file object.
     *          If file was opened in FO_MODE_ATTACH or FO_MODE_DETACH modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FORCE_DELETE or FO_MODE_CREATE_TEMP. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If FO_MODE_CREATE_TEMP is set, file object is always deleted on close.
     *          If FO_FORCE_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     *
     * \see     Open()
     **/
    virtual void Close( void ) = 0;

    /**
     * \brief	Removes opened file. This will force to remove file object even if it is attached memory buffered file
     * \return	Returns true if succeeded.
     **/
    virtual bool Remove( void ) = 0;

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual unsigned int GetLength( void ) const = 0;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    virtual bool IsOpened() const = 0;

    /**
     * \brief	Call to reserve space or set new size of file object and returns the current position of pointer.
     *          If new size is positive value, the size will be increased. Otherwise the reserved size will set size zero.
     *          If new size is more than the current size of file object, no data will be lost, no pointer position will be changed.
     *          If new size is less than the current size of file object, data will be truncated until the new size, 
     *          and if the new size is less than the current pointer position, the pointer will be move at the end of file.
     * 
     * \param	newSize New Size is bytes to reserve or set. Positive value will set the size. Negative value will set size zero.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value INVALID_POINTER_POSITION.
     **/
    virtual unsigned int Reserve(int newSize) = 0;

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, return true
     **/
    virtual bool PurgeData( void ) = 0;

public:
/************************************************************************/
// IEInStream interface overrides
/************************************************************************/
    /**
     * \brief	Read data from input stream object, copies into given buffer and
     *          returns the size of copied data. Implement the function to provide
     *          reading functionality of input streaming object.
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int Read( unsigned char* buffer, unsigned int size ) const = 0;

    /**
     * \brief   Reads data from input stream object, copies into give Byte Buffer object
     *          and returns the size of copied data. Overwrite this method if copy behavior
     *          changed for certain buffer. For other buffers it should have simple behavior
     *          as copying to raw buffer
     * \param   buffer  The instance of Byte Buffer object to stream data from Input Stream object
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int Read( IEByteBuffer & buffer ) const = 0;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEString & asciiString ) const = 0;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Read( CEWideString & wideString ) const = 0;

/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. 
     *          Implement function to provide writing functionality 
     *          of output stream object.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const unsigned char* buffer, unsigned int size ) = 0;

    /**
     * \brief	Writes Binary data from Byte Buffer object to Output Stream object
     *          and returns the size of written data. Overwrite this method if need 
     *          to change behavior of streaming buffer.
     * \param	buffer	The instance of Byte Buffer object containing data to stream to Output Stream.
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int Write( const IEByteBuffer & buffer ) = 0;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEString & asciiString ) = 0;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int Write( const CEWideString & wideString ) = 0;

protected:
/************************************************************************/
// IEInStream interface overrides
/************************************************************************/
    /**
     * \brief	Returns size in bytes of available data that can be read, 
     *          i.e. remaining readable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already read from stream, the available readable size is 'n - x'.
     **/
    virtual unsigned int GetReadableSize( void ) const = 0;

/************************************************************************/
// IEOutStream interface overrides
/************************************************************************/
    /**
     * \brief	Returns size in bytes of available space that can be written, 
     *          i.e. remaining writable size.
     *          No necessarily that this size is equal to size of streamable buffer.
     *          For example, if the size of buffer is 'n' and 'x' bytes of data was
     *          already written to stream, the available writable size is 'n - x'.
     **/
    virtual unsigned int GetWritableSize( void ) const = 0;

protected:
    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void ResetCursor( void ) const;

    /**
     * \brief	Flushes cached data to output stream object.
     *          Implement the function if device has caching mechanism
     **/
    virtual void Flush( void );

protected:
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
/************************************************************************/
// State functions
/************************************************************************/
    /**
     * \brief	Returns true if file is opened in mode to force delete
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsDeleteForced( void ) const;

    /**
     * \brief   Returns true if file is opened in temporary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsTemporary( void ) const;

    /**
     * \brief	Returns true if buffer of memory file is attached
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsAttachMode( void ) const;

    /**
     * \brief   Returns true if buffer of memory file should be detached (on close)
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsDetachMode( void ) const;

    /**
     * \brief   Returns true if file pointer position is at the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsEndOfFile( void ) const;

    /**
     * \brief   Returns true if file is opened in text mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsTextMode( void ) const;

    /**
     * \brief	Returns true if file is opened in binary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsBinaryMode( void ) const;

    /**
     * \brief   Returns true if write operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool CanWrite( void ) const;

    /**
     * \brief   Returns true if read operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool CanRead( void ) const;

    /**
     * \brief   Returns true if read sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsShareForRead( void ) const;

    /**
     * \brief   Returns true if write sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool IsSharedForWrite( void ) const;

/************************************************************************/
// Cursor position operation functions
/************************************************************************/
    /**
     * \brief   Move file pointer to the beginning of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool MoveCursorToBegin( void );

    /**
     * \brief   Move file pointer to the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool MoveCursorToEnd( void );

/************************************************************************/
// Read / Write operation functions
/************************************************************************/
    /**
     * \brief	Moves pointer to the end of file, increase size of file, appends data and returns size of append data bytes.
     *
     * \param	buffer	The buffer to be appended.
     * \param	length	The size in bytes of data to be copied from buffer
     *
     * \return	Returns size of copied data in bytes 
     **/
    int Append(const unsigned char* buffer, int length);

    /**
     * \brief	Read data in inverted order.
     * \param	buffer	The buffer to read out data
     * \param	length	The length of buffer in bytes
     * \return	Returns the size of data read out.
     **/
    int ReadInvert(unsigned char* buffer, int length) const;

    /**
     * \brief	Write data in inverted order
     * \param	buffer	The buffer of data
     * \param	length	The length of data to write
     * \return	Returns the size of written data in bytes.
     **/
    int WriteInvert(const unsigned char* buffer, int length);

    /**
     * \brief	Call to reserve space or set new size of file object and returns the current position of pointer.
     *          The reserved space will be filled by specified 1-byte value.
     *          If new size is positive value, the size will be increased. Otherwise the reserved size will set size zero.
     *          If new size is more than the current size of file object, no data will be lost, no pointer position will be changed.
     *          If new size is less than the current size of file object, data will be truncated until the new size, 
     *          and if the new size is less than the current pointer position, the pointer will be move at the end of file.
     * 
     * \param	newSize     New Size is bytes to reserve or set. Positive value will set the size. Negative value will set size zero.
     * \param   fillValue   The value to fill reserved space
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value INVALID_POINTER_POSITION.
     **/
    virtual unsigned int ReserveAndFill(int newSize, unsigned char fillValue);

/************************************************************************/
// Read / Write simple types
/************************************************************************/
    /**
     * \brief   Reads boolean value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadBool(bool & outValue) const;

    /**
     * \brief   Reads 1 byte of data, covert to char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadChar(char & outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to wide char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadWideChar(wchar_t & outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to short integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadShort(short & outValue) const;

    /**
     * \brief   Reads 4 bytes of data, covert to integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadInt(int & outValue) const;

    /**
     * \brief   Reads 4 bytes of data, covert to long integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadLong(long & outValue) const;

    /**
     * \brief   Reads 8 bytes of data, covert to large integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadLarge(int64_t & outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadFloat(float & outValue) const;

    /**
     * \brief   Reads 4 bytes of data, covert to double floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool ReadDouble(double & outValue) const;

    /**
     * \brief   Write 1 byte of char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteBool(bool inValue);

    /**
     * \brief   Write 1 byte of char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteChar(char inValue);

    /**
     * \brief   Write 2 bytes of wide char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteWideChar(wchar_t inValue);

    /**
     * \brief   Write 2 bytes of short integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteShort(short inValue);

    /**
     * \brief   Write 4 bytes of integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteInt(int inValue);

    /**
     * \brief   Write 4 bytes of long integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteLong(long inValue);

    /**
     * \brief   Write 8 bytes of large integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteLarge(int64_t inValue);

    /**
     * \brief   Write 2 bytes of floating value data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteFloat(float inValue);

    /**
     * \brief   Write 4 bytes of double floating value data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     **/
    inline bool WriteDouble(double inValue);

/************************************************************************/
// Read / Write strings
/************************************************************************/
    /**
     * \brief	Reads string of 8-bit characters, automatically sets null-terminate char at the end and returns number of characters, which could 
     * \param	buffer      The buffer where the string should be written. 
     * \param	elemCount	Number of characters to write. The buffer should be long enough to set end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int ReadString( char * buffer, int elemCount ) const;
    /**
     * \brief	Reads string of 16-bit characters, automatically sets null-terminate char at the end and returns number of characters, which could 
     * \param	buffer      The buffer where the string should be written. 
     * \param	elemCount	Number of characters to write. The buffer should be long enough to set end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int ReadString( wchar_t * buffer, int elemCount ) const;

    /**
     * \brief	Read string until end of file or null-terminated character. It does not validate string data
     * \param	outBuffer	The string containing data
     * \return	Returns number of characters, which could read.
     **/
    int ReadString(CEString & outBuffer) const;
    /**
     * \brief	Read string until end of file or null-terminated character. It does not validate string data
     * \param	outBuffer	The string containing data
     * \return	Returns number of characters, which could read.
     **/
    int ReadString(CEWideString & outBuffer) const;

    /**
     * \brief	Reads line of string of 8-bit characters, automatically sets null-terminate char at the end and returns number of characters, which could 
     * \param	buffer      The buffer where the string should be written. 
     * \param	elemCount	Number of characters to write. The buffer should be long enough to set end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int ReadLine( char * buffer, int elemCount) const;
    /**
     * \brief	Reads line of string of 16-bit characters, automatically sets null-terminate char at the end and returns number of characters, which could 
     * \param	buffer      The buffer where the string should be written. 
     * \param	elemCount	Number of characters to write. The buffer should be long enough to set end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int ReadLine( wchar_t * buffer, int elemCount ) const;

    /**
     * \brief	Reads 1 line of string until end of file or null-terminated character or new line character. 
     *          It does not validate string data
     * \param	outValue	The string containing data
     * \return	If succeeds, returns true.
     **/
    int ReadLine(CEString & outValue) const;
    /**
     * \brief	Reads 1 line of string until end of file or null-terminated character or new line character. 
     *          It does not validate string data
     * \param	outValue	The string containing data
     * \return	If succeeds, returns true.
     **/
    int ReadLine(CEWideString & outValue) const;

    /**
     * \brief	Writes string in file and returns true if succeeded.
     *          If file was opened in FO_MODE_TEXT, the null-terminate char of string will be skipped.
     *          If file was opened in FO_MODE_BINARY, the null-terminate char of string will be also written.
     *
     * \param	inValue	The buffer containing string.
     *
     * \return	Returns true if operation succeeded.
     **/
    bool WriteString(const char* inValue);
    bool WriteString(const wchar_t* inValue);

    /**
     * \brief	Writes line of string in file and returns true if succeeded.
     *          The new line character after every call (in UNIX format) will be set automatically.
     *          Different to WriteString, the null-terminate char of string will be skipped and new line will be written instead.
     *
     * \param	inValue	The buffer containing string.
     *
     * \return	Returns true if operation succeeded.
     **/
    bool WriteLine(const char* inValue);
    bool WriteLine(const wchar_t* inValue);

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Object of converter class.
     *          Used to retrieve and convert primitive types
     *          such as bool, char, short, double, int, etc.
     *          For more details see description of CEReadConverter class
     **/
    CEReadConverter     mReadConvert;
    /**
     * \brief   Object of converter class.
     *          Used to convert and write primitive types
     *          such as bool, char, short, double, int, etc.
     *          For more details see description of CEWriteConverter class
     **/
    CEWriteConverter    mWriteConvert;

//////////////////////////////////////////////////////////////////////////
// Private function class.
//////////////////////////////////////////////////////////////////////////
private:
    inline CEFileBase & self( void );

//////////////////////////////////////////////////////////////////////////
// Forbidden functions / methods call
//////////////////////////////////////////////////////////////////////////
private:
    CEFileBase(const CEFileBase & /*src*/ );
    const CEFileBase & operator = (const CEFileBase & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CEFileBase class inline functions
//////////////////////////////////////////////////////////////////////////
inline CEFileBase& CEFileBase::self( void )
{   return (*this); }

inline bool CEFileBase::IsDeleteForced( void ) const
{   return (GetMode() & static_cast<int>(FOB_FORCE_DELETE)) != 0;       }

inline bool CEFileBase::IsTemporary( void ) const
{   return (GetMode() & static_cast<int>(FO_MODE_CREATE_TEMP)) != 0;    }

inline bool CEFileBase::IsAttachMode( void ) const
{   return (GetMode() & static_cast<int>(FOB_ATTACH)) != 0;             }

inline bool CEFileBase::IsDetachMode( void ) const
{   return (GetMode() & static_cast<int>(FOB_DETACH)) != 0;             }

inline bool CEFileBase::IsTextMode( void ) const
{   return ( (GetMode() & static_cast<int>(FOB_TEXT)) != 0 );           }

inline bool CEFileBase::IsBinaryMode( void ) const
{   return ( (GetMode() & static_cast<int>(FOB_BINARY)) != 0 );         }

inline bool CEFileBase::IsShareForRead( void ) const
{   return ( (GetMode() & static_cast<int>(FOB_SHARE_READ)) != 0 );     }

inline bool CEFileBase::IsSharedForWrite( void ) const
{   return ( (GetMode() & static_cast<int>(FOB_SHARE_WRITE)) != 0 );    }

inline bool CEFileBase::IsEndOfFile( void ) const
{   ASSERT(IsOpened()); return ( GetPosition() < GetLength() );                             }

inline bool CEFileBase::CanWrite( void ) const
{   ASSERT(IsOpened()); return ( (GetMode() & static_cast<int>(FOB_WRITE)) != 0 );          }

inline bool CEFileBase::CanRead( void ) const
{   ASSERT(IsOpened()); return ( (GetMode() & static_cast<int>(FOB_READ)) != 0 );           }

inline bool CEFileBase::MoveCursorToBegin( void )
{   ASSERT(IsOpened()); return IECursorPosition::MoveToBegin();                            }

inline bool CEFileBase::MoveCursorToEnd( void )
{   ASSERT(IsOpened()); return IECursorPosition::MoveToEnd();                              }

inline const IEInStream& CEFileBase::GetReadStream( void ) const
{   ASSERT(IsOpened()); return static_cast<const IEInStream &>(*this); }

inline IEOutStream& CEFileBase::GetWriteStream( void )
{   ASSERT(IsOpened()); return static_cast<IEOutStream &>(*this);      }

inline bool CEFileBase::ReadBool(bool &outValue) const
{   return mReadConvert.GetBool(outValue);      }

inline bool CEFileBase::ReadChar( char &outValue ) const
{   return mReadConvert.GetChar(outValue);      }

inline bool CEFileBase::ReadWideChar( wchar_t &outValue ) const
{   return mReadConvert.GetChar(outValue);      }

inline bool CEFileBase::ReadShort( short &outValue ) const
{   return mReadConvert.GetShort(outValue);     }

inline bool CEFileBase::ReadInt( int &outValue ) const
{   return mReadConvert.GetInt(outValue);       }

inline bool CEFileBase::ReadLong( long &outValue ) const
{   return mReadConvert.GetLong(outValue);      }

inline bool CEFileBase::ReadLarge( int64_t &outValue ) const
{   return mReadConvert.GetInt64(outValue);     }

inline bool CEFileBase::ReadFloat(float &outValue) const
{   return mReadConvert.GetFloat(outValue);     }

inline bool CEFileBase::ReadDouble(double &outValue) const
{   return mReadConvert.GetDouble(outValue);    }

inline bool CEFileBase::WriteBool( bool inValue )
{   return mWriteConvert.SetBool(inValue);      }

inline bool CEFileBase::WriteChar( char inValue )
{   return mWriteConvert.SetChar(inValue);      }

inline bool CEFileBase::WriteWideChar( wchar_t inValue )
{   return mWriteConvert.SetChar(inValue);      }

inline bool CEFileBase::WriteShort( short inValue )
{   return mWriteConvert.SetShort(inValue);     }

inline bool CEFileBase::WriteInt( int inValue )
{   return mWriteConvert.SetInt(inValue);       }

inline bool CEFileBase::WriteLong( long inValue )
{   return mWriteConvert.SetLong(inValue);      }

inline bool CEFileBase::WriteLarge( int64_t inValue )
{   return mWriteConvert.SetInt64(inValue);     }

inline bool CEFileBase::WriteFloat( float inValue )
{   return mWriteConvert.SetFloat(inValue);     }

inline bool CEFileBase::WriteDouble( double inValue )
{   return mWriteConvert.SetDouble(inValue);    }

#endif  // AREG_BASE_CEFILEBASE_HPP
