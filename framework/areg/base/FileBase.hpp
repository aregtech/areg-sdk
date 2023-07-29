#ifndef AREG_BASE_FILEBASE_HPP
#define AREG_BASE_FILEBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/FileBase.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Base class of file objects.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IEIOStream.hpp"
#include "areg/base/IECursorPosition.hpp"

#include "areg/base/String.hpp"
#include "areg/base/private/ReadConverter.hpp"
#include "areg/base/private/WriteConverter.hpp"

#include <string_view>

class String;
class WideString;
class IEByteBuffer;

//////////////////////////////////////////////////////////////////////////
// FileBase class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Pure virtual class to perform file basic and common functionalities.
 *          The classes derived from this class should support data streaming, i.e.
 *          should support read / write, as well as should contain read / write position.
 *          objects to move pointer.
 *          Before performing any read and write operations, the file object should be opened.
 *          Otherwise the operation will fail. For details see function comments.
 * 
 *          Some use of File Open Mode:
 *
 *          1. FO_MODE_INVALID      - Invalid mode. No operation should be performed. Prevent caller setting this mode;
 *
 *          2. FO_MODE_READ         - File opened for reading. By default, all files should be opened with this flag.
 *
 *          3. FO_MODE_WRITE        - File opened for writing and reading. If flag is missed, the write functionality will be disabled.
 *
 *          4. FO_MODE_BINARY       - File opened in binary mode. All data will be read and written as binary data.
 *                                    For strings, the null-terminate symbol will be also written / read.
 *
 *          5. FO_MODE_TEXT         - File opened int text mode. All data will be written as plain text, 
 *                                    until does not match null-terminate symbol.
 *              
 *          6. FO_MODE_SHARE_READ   - File is opened in share read mode. The other File object is able to read.
 *                                    This flag makes sense for FS File object. For memory file it does not make sense,
 *                                    since buffer pointer may change address and File object is not able to control 
 *                                    memory access by other functionality.
 *
 *          7. FO_MODE_SHARE_WRITE  - File is opened in share write and read modes. The other File object is able to write and read data.
 *                                    This flag makes sense for FS File object. For memory file it does not make sense,
 *                                    since buffer pointer may change address and File object is not able to control 
 *                                    memory access by other functionality.
 *    
 *          8. FO_MODE_CREATE       - If file does not exist, the new file will be created. The size will set zero
 *                                    If FO_MODE_EXIST flag is set, will try to open existing file, otherwise creates new file.
 *
 *          9. FO_MODE_EXIST        - Opens only existing file. Opening might fail if file does not exist and 'FO_MODE_CREATE' flag is not set.
 *                                    If FO_MODE_CREATE flag is set and file does not exist, it will create new file.
 *                                    If FO_MODE_TRUNCATE flag is set, opens file for read/write and sets initial size of file to zero.
 *
 *         10. FO_MODE_TRUNCATE     - Opens file in read and write mode, sets initial size of file to zero. 
 *                                    Ignored in mode FO_MODE_ATTACH.
 *                                    Can be used with combination FO_MODE_CREATE and FO_MODE_EXIST.
 *
 *         11. FO_MODE_ATTACH       - Will use existing file handle / memory buffer pointer only in read mode. 
 *                                    On close neither handle, nor buffer pointer will be closed / deleted.
 *
 *         12. FO_MODE_DETACH       - Will open file / memory buffer for read and write access.
 *                                    On close neither handle, nor buffer pointer will be closed / deleted.
 *
 *         13. FO_MODE_FOR_DELETE - Will force to delete on close even if buffer is attached or marked as detached.
 *
 **/
class AREG_API FileBase : public IEIOStream
                        , public IECursorPosition
{
//////////////////////////////////////////////////////////////////////////
// Defined types
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   File open bits specifying opening mode, access rights and attributes
     **/
    typedef enum E_FileOpenBits : unsigned int
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
        , FOB_FOR_DELETE    = 2048  //!< 0000100000000000 <= delete on close bit
        , FOB_WRITE_DIRECT  = 4096  //!< 0001000000000000 <= write direct on disk bit
        , FOB_TEMP_FILE     = 8192  //!< 0010000000000000 <= create temporary file bit, will be deleted on close.

    } eFileOpenBits;

public:
    /**
     * \brief   File opening modes.
     **/
    typedef enum E_FileOpenMode : unsigned int
    {
          FO_MODE_INVALID       = (FOB_INVALID)                                                     //!< 0000000000000000 <= invalid mode

        , FO_MODE_ONLY_READ     = (FOB_READ)                                                        //!< 0000000000000001 <= exclusive read, contains only read bit
        , FO_MODE_ONLY_WRITE    = (FOB_WRITE | FOB_READ)                                            //!< 0000000000000011 <= exclusive write, should contain "read" bit
        , FO_MODE_READ          = (FOB_READ  | FOB_SHARE_READ)                                      //!< 0000000000010001 <= read mode, can share read
        , FO_MODE_WRITE         = (FOB_WRITE | FOB_READ | FOB_SHARE_READ)                           //!< 0000000000010011 <= write mode, should contain "read" bit and can share read
        , FO_MODE_BINARY        = (FOB_BINARY)                                                      //!< 0000000000000100 <= binary bit. strings in binary mode will write EOS char, all data will be dumped.
        , FO_MODE_TEXT          = (FOB_TEXT)                                                        //!< 0000000000001000 <= text bit. EOS char of string will not be written, can write and read line of string
        , FO_MODE_SHARE_READ    = (FOB_SHARE_READ  | FOB_READ)                                      //!< 0000000000010001 <= share read mode, should contain "read" bit
        , FO_MODE_SHARE_WRITE   = (FOB_SHARE_WRITE | FOB_SHARE_READ | FOB_WRITE | FOB_READ)         //!< 0000000000110011 <= share write mode, should contain "read", "write" and "share read" bits
        , FO_MODE_CREATE        = (FOB_CREATE)                                                      //!< 0000000001000000 <= always create file
        , FO_MODE_EXIST         = (FOB_EXIST)                                                       //!< 0000000010000000 <= file should exist, otherwise it fails
        , FO_MODE_TRUNCATE      = (FOB_TRUNCATE | FOB_WRITE | FOB_READ)                             //!< 0000000100000011 <= truncate file, i.e. set initial size zero, "read" and "write" bits should be set
        , FO_MODE_ATTACH        = (FOB_ATTACH | FOB_EXIST | FOB_SHARE_READ | FOB_READ)              //!< 0000001010010001 <= attached handle (buffer). Can only read memory data, cannot change the size or write data. Bits "read", "share read", "exist" are set automatically. Not able to control share mode, the pointer should be passed. Should not free buffer in destructor.
        , FO_MODE_DETACH        = (FOB_DETACH | FOB_CREATE | FOB_SHARE_READ | FOB_WRITE | FOB_READ) //!< 0000010001010011 <= mode detach buffer. Can read and write memory data. Bits "read", "write", "share read" and "create" are set automatically. Not able to control share modes. Should not free buffer in destructor, it is up to caller to free buffer.
        , FO_MODE_DELETE        = (FOB_FOR_DELETE)                                                  //!< 0000100000000000 <= mode to delete on close. Can be combined with any mode. The file / buffer will be deleted even if mode attached / detach are set.
        , FO_MODE_WRITE_DIRECT  = (FOB_WRITE_DIRECT | FOB_WRITE | FOB_READ)                         //!< 0001000000000011 <= write operations will not go through any intermediate cache, they will go directly to disk. read and write flags are set automatically.
        , FO_MODE_CREATE_TEMP   = (FOB_TEMP_FILE | FOB_WRITE | FOB_READ)                            //!< 0010000000000011 <= The file is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because an application deletes a temporary file after a handle is closed. In that case, the system can entirely avoid writing the data.

    } eFileOpenMode;

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   File::TIMESTAMP_FILE_MASK
     *          Mask used in file name to set time-stamp
     **/
    static constexpr std::string_view   FILE_MASK_TIMESTAMP { "%time%" };

    /**
     * \brief   File::TIMESTAMP_FORMAT
     *          Default time-stamp format, used as yyyy_mm_dd_hh_mm_ss_ms
     **/
    static constexpr std::string_view   TIMESTAMP_FORMAT    { "%04d_%02d_%02d_%02d_%02d_%02d_%03d" };

    /**
     * \brief   The name of application to add in file name or path
     **/
    static constexpr std::string_view   FILE_MASK_APPNAME   { "%appname%" };

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default protected constructor.
     **/
    FileBase( void );

    /**
     * \brief   Destructor.
     **/
    virtual ~FileBase( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Inputs ASCII string to the file.
     *          If file is opened in text mode, it does not write the string null-terminated character at the end.
     *          If file is opened in binary mode, it writes the string null-terminated character at the end.
     * \param   stream  The instance of file to write string.
     * \param   ascii   The ASCII string to write.
     * \return  The instance of file.
     **/
    friend inline FileBase & operator << (FileBase & stream, const char * ascii );

    /**
     * \brief   Inputs ASCII string to the file.
     *          If file is opened in text mode, it does not write the string null-terminated character at the end.
     *          If file is opened in binary mode, it writes the string null-terminated character at the end.
     * \param   stream  The instance of file to write string.
     * \param   ascii   The ASCII string to write.
     * \return  The instance of file.
     **/
    friend inline FileBase & operator << (FileBase & stream, const String & ascii );

    /**
     * \brief   Inputs wide-char string to the file.
     *          If file is opened in text mode, it does not write the string null-terminated character at the end.
     *          If file is opened in binary mode, it writes the string null-terminated character at the end.
     * \param   stream  The instance of file to write string.
     * \param   wide    The wide-char string to write.
     * \return  The instance of file.
     **/
    friend inline FileBase & operator << (FileBase & stream, const wchar_t * wide );

    /**
     * \brief   Inputs wide-char string to the file.
     *          If file is opened in text mode, it does not write the string null-terminated character at the end.
     *          If file is opened in binary mode, it writes the string null-terminated character at the end.
     * \param   stream  The instance of file to write string.
     * \param   wide    The wide-char string to write.
     * \return  The instance of file.
     **/
    friend inline FileBase & operator << (FileBase & stream, const WideString & wide );

    /**
     * \brief   Writes files data as ASCII string. The file outputs string until reaches end of file,
     *          or meets first unprintable symbol. On output, the 'ascii'
     * \param   stream  The instance of file data contains the data.
     * \param   ascii   On output, this contains the ASCII string data.
     **/
    friend inline const FileBase & operator >> ( const FileBase & stream, String & OUT ascii );

    /**
     * \brief   Writes files data as wide-char string. The file outputs string until reaches end of file,
     *          or meets first unprintable symbol. On output, the 'ascii'
     * \param   stream  The instance of file data contains the data.
     * \param   ascii   On output, this contains the wide-char string data.
     **/
    friend inline const FileBase & operator >> ( const FileBase & stream, WideString & OUT wide );

    //////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns instance of streaming object to read data.
     **/
    inline const IEInStream & getReadStream( void ) const;

    /**
     * \brief   Returns instance of streaming object to write data.
     **/
    inline IEOutStream & getWriteStream( void );

    /**
     * \brief   Returns the name of file object set by user. This can be either short name
     *          or normalized full path. Can be empty string for buffered file.
     * \return  Returns the given name of file.
     **/
    inline const String & getName( void ) const;

    /**
     * \brief   Returns the file open mode (bits)
     **/
    inline unsigned int getMode( void ) const;

/************************************************************************/
// State functions
/************************************************************************/
    /**
     * \brief   Returns true, if file is valid. The file is valid if it is opened for read and/or write.
     */
    inline bool isValid( void ) const;

    /**
     * \brief	Returns true if file is opened in mode to force to delete.
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isForceDelete( void ) const;

    /**
     * \brief   Returns true if file is opened in temporary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isTemporary( void ) const;

    /**
     * \brief	Returns true if buffer of memory file is attached
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isAttachMode( void ) const;

    /**
     * \brief   Returns true if buffer of memory file should be detached (on close)
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isDetachMode( void ) const;

    /**
     * \brief   Returns true if file pointer position is at the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isEndOfFile( void ) const;

    /**
     * \brief   Returns true if file is opened in text mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isTextMode( void ) const;

    /**
     * \brief	Returns true if file is opened in binary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isBinaryMode( void ) const;

    /**
     * \brief   Returns true if read sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isShareForRead( void ) const;

    /**
     * \brief   Returns true if write sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isSharedForWrite( void ) const;

    /**
     * \brief   Returns true if write operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool canWrite( void ) const;

    /**
     * \brief   Returns true if read operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool canRead( void ) const;

/************************************************************************/
// Cursor position operation functions
/************************************************************************/
    /**
     * \brief   Move file pointer to the beginning of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool moveToBegin( void );

    /**
     * \brief   Move file pointer to the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool moveToEnd( void );

    /**
     * \brief   Returns the start position value.
     **/
    inline static unsigned int getStartPosition( void );

    /**
     * \brief   Return the invalid position value.
     **/
    inline static unsigned int getInvalidPosition( void );

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
    int append(const unsigned char* buffer, unsigned int length);

    /**
     * \brief	Read data in inverted order.
     * \param	buffer	The buffer to read out data
     * \param	length	The length of buffer in bytes
     * \return	Returns the size of data read out.
     **/
    int readInvert(unsigned char* buffer, unsigned int length) const;

    /**
     * \brief	Write data in inverted order
     * \param	buffer	The buffer of data
     * \param	length	The length of data to write
     * \return	Returns the size of written data in bytes.
     **/
    int writeInvert( const unsigned char * buffer, int unsigned length );

    /**
     * \brief	Call to set new size of file object and returns the current position of pointer.
     *          The resized space will be filled by specified 1-byte value. If new size is more than 
     *          the current size, the size will be increased, the existing data and pointer position 
     *          will not be changed, the rest of data will be filled with specified 1-byte value.
     *          If new size is less than the current size of file object, data will be truncated until 
     *          the new size, and if needed the pointer position will be moved to the end of file.
     * 
     * \param	newSize     New Size is bytes to reserve or set.
     * \param   fillValue   The value to fill reserved space
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value IECursorPosition::INVALID_CURSOR_POSITION.
     **/
    unsigned int resizeAndFill(unsigned int newSize, unsigned char fillValue);

/************************************************************************/
// Read / Write simple types
/************************************************************************/
    /**
     * \brief   Reads boolean value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readBool(bool & OUT outValue) const;

    /**
     * \brief   Reads 1 byte of data, covert to char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readChar(char & OUT outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to wide char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readChar(wchar_t & OUT outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to short integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readShort(short & OUT outValue) const;

    /**
     * \brief   Reads 4 bytes of data, covert to integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readInt(int & OUT outValue) const;

    /**
     * \brief   Reads 8 bytes of data, covert to large integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readLarge(int64_t & OUT outValue) const;

    /**
     * \brief   Reads 2 bytes of data, covert to floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readFloat(float & OUT outValue) const;

    /**
     * \brief   Reads 4 bytes of data, covert to double floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   outValue[out]   On output contains value that could read.
     **/
    inline bool readDouble(double & OUT outValue) const;

    /**
     * \brief   Write 1 byte of char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeBool(bool inValue);

    /**
     * \brief   Write 1 byte of char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeChar(char inValue);

    /**
     * \brief   Write 2 bytes of wide char data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeChar(wchar_t inValue);

    /**
     * \brief   Write 2 bytes of short integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeShort(short inValue);

    /**
     * \brief   Write 4 bytes of integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeInt(int inValue);

    /**
     * \brief   Write 8 bytes of large integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeLarge(int64_t inValue);

    /**
     * \brief   Write 2 bytes of floating value data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeFloat(float inValue);

    /**
     * \brief   Write 4 bytes of double floating value data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeDouble(double inValue);

/************************************************************************/
// Read / Write strings
/************************************************************************/
    /**
     * \brief	Reads string, automatically sets null-terminate symbol at
     *          the end and returns number of characters, which could read.
     * 
     * \param	buffer      The buffer where the string should be written. 
     * \param	charCount	Number of characters to write in the string buffer.
     *                      The string buffer should be long enough to set
     *                      end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int readString( char * IN OUT buffer, int IN charCount ) const;
    int readString( wchar_t * IN OUT buffer, int IN charCount ) const;

    /**
     * \brief	Read string until end of file or null-terminated character.
     *          It does not validate string data.
     * 
     * \param	outBuffer	The string containing data
     * \return	Returns number of characters, which could read.
     **/
    int readString(String & OUT outBuffer) const;
    int readString(WideString & OUT outBuffer) const;

    /**
     * \brief	Reads a line of string, automatically sets null-terminate char at
     *          the end and returns number of characters, which could read.
     * 
     * \param	buffer      The buffer where the string should be written. 
     * \param	charCount	Number of characters to write in the string buffer.
     *                      The string buffer should be long enough to set
     *                      end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int readLine( char * IN OUT buffer, int IN charCount) const;
    int readLine( wchar_t * IN OUT buffer, int IN charCount ) const;

    /**
     * \brief	Reads a line of string until end of file, or first match of 
     *          null-terminated character or new line character. 
     *          It does not validate string data.
     * 
     * \param	outBuffer	On output, this contains a string of a line.
     * \return	Returns number of characters, which could read.
     **/
    int readLine(String & OUT outBuffer) const;
    int readLine(WideString & OUT outBuffer) const;

    /**
     * \brief	Writes string in file and returns true if succeeded.
     *          If file was opened in FO_MODE_TEXT, the null-terminate char of string will be skipped.
     *          If file was opened in FO_MODE_BINARY, the null-terminate char of string will be also written.
     *
     * \param	buffer  The buffer of string to write.
     * \return	Returns true if operation succeeded.
     **/
    bool writeString(const char* buffer);
    bool writeString(const wchar_t* buffer);
    bool writeString(const String& buffer);
    bool writeString(const WideString& buffer);

    /**
     * \brief	Writes line of string in file and returns true if succeeded.
     *          The new line character after every call (in UNIX format) will be set automatically.
     *          Different to WriteString, the null-terminate char of string will be skipped and new line will be written instead.
     *
     * \param	inValue	The buffer containing string.
     *
     * \return	Returns true if operation succeeded.
     **/
    bool writeLine(const char* inValue);
    bool writeLine(const wchar_t* inValue);
    bool writeLine(const String& buffer);
    bool writeLine(const WideString& buffer);

    /**
     * \brief   Writes string into the file.
     **/
    bool write(const char * asciiString);
    bool write(const wchar_t * wideString);

    /**
     * \brief   Searches the given binary data in the file and returns the position where the data starts.
     * \param   startPos    The position in the file to start to search.
     * \param   buffer      The binary data buffer to search.
     * \param   length      The length of the data to search.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (IECursorPosition::INVALID_CURSOR_POSITION).
     **/
    unsigned int searchData( unsigned int startPos, const unsigned char * buffer, int length ) const;
    /**
     * \brief   Searches the given binary data in the file and returns the position where the data starts.
     * \param   startPos    The position in the file to start to search.
     * \param   buffer      The buffer of byte-binary data to search.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (IECursorPosition::INVALID_CURSOR_POSITION).
     **/
    unsigned int searchData( unsigned int startPos, const IEByteBuffer & buffer ) const;

    /**
     * \brief   Searches the given null-terminated text in the file and returns the position where the data starts.
     * \param   startPos    The position in the file to start to search.
     * \param   text        The null-terminated text to search.
     * \param   length      The length of the data to search.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (IECursorPosition::INVALID_CURSOR_POSITION).
     **/
    unsigned int searchText( unsigned int startPos, const char * text, bool caseSensitive ) const;
    unsigned int searchText( unsigned int startPos, const wchar_t * text, bool caseSensitive ) const;
    unsigned int searchText( unsigned int startPos, const String & text, bool caseSensitive ) const;
    unsigned int searchText( unsigned int startPos, const WideString & text, bool caseSensitive ) const;

//////////////////////////////////////////////////////////////////////////
// Override methods
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// FileBase class overrides
/************************************************************************/
    /**
     * \brief   Opens the file. No operation can be performed before file is opened.
     *          For memory buffered file at least the file open mode should be set.
     *          For file system file object additionally the file name (or full / relative path) should be set.
     *          If mentioned parameters are not set, the open operation will fail and return false.
     *          If file object was already opened, second call to open file will fail.
     *
     * \return  Returns true if file object was opened with success.
     * \see     close()
     **/
    virtual bool open( void ) = 0;

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
     *                      For example, mode cannot contain (FO_MODE_ATTACH | FO_MODE_DETACH) at once.
     *                      One of bits will be ignored.
     *                      For more details see description of eFileOpenMode and eFileOpenBits.
     *
     * \return	Returns true if file was opened with success.
     * \see     close()
     **/
    virtual bool open(const String& fileName, unsigned int mode) = 0;

    /**
     * \brief   Call to close file object.
     *          If file was opened in FO_MODE_ATTACH or FO_MODE_DETACH modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FOR_DELETE or FO_MODE_CREATE_TEMP. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If FO_MODE_CREATE_TEMP is set, file object is always deleted on close.
     *          If FO_FOR_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     *
     * \see     open()
     **/
    virtual void close( void ) = 0;

    /**
     * \brief	Removes opened file. This will force to remove file object even if it is attached memory buffered file.
     * \return	Returns true if succeeded.
     **/
    virtual bool remove( void ) = 0;

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual unsigned int getLength( void ) const = 0;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    virtual bool isOpened() const = 0;

    /**
     * \brief	Call to set new size of file object and returns the current position of pointer.
     *          If new size is more than the current size, the size will be increased, the existing 
     *          data and pointer position will not be changed. If new size is less than the current 
     *          size of file object, data will be truncated until the new size, and if needed 
     *          the pointer position will be moved to the end of file.
     *
     * \param	newSize     New Size is bytes to set.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value IECursorPosition::INVALID_CURSOR_POSITION.
     **/
    virtual unsigned int reserve(unsigned int newSize) = 0;

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, return true
     **/
    virtual bool truncate( void ) = 0;

/************************************************************************/
// IEIOStream pure virtual
/************************************************************************/

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( unsigned char * buffer, unsigned int size ) const override = 0;

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. In this class 
     *          writes data into opened file.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char* buffer, unsigned int size ) override = 0;

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
    virtual unsigned int read( IEByteBuffer & buffer ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given ASCII String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & asciiString ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wideString ) const override;

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
    virtual unsigned int write( const IEByteBuffer & buffer ) override;

    /**
     * \brief   Writes string data from given ASCII String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   asciiString     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & asciiString ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wideString  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wideString ) override;

    /**
     * \brief   Clears the buffers for the file and causes all buffered data 
     *          to be written to the file.
     **/
    virtual void flush( void ) override;

protected:

    /**
     * \brief	Validates and normalize bits for file open mode.
     * \param	mode	Integer value of bitwise OR operation of eFileOpenMode values
     * \return	Returns normalized value.
     **/
    virtual unsigned int normalizeMode(unsigned int mode) const;

/************************************************************************/
// IEOutStream overrides
/************************************************************************/
    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    virtual void resetCursor( void ) const override;

    /**
     * \brief   Normalizes the name, replace special masks such as time-stamp or process name in the give name.
     * \param[out]  name    On output, contains normalized name of file.
     **/
    static void normalizeName( String & IN OUT name );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////

protected:
    /**
     * \brief   Absolute or relative file path
     **/
    String          mFileName;

    /**
     * \brief   File open mode
     **/
    unsigned int    mFileMode;

private:
    /**
     * \brief   Object of converter class.
     *          Used to retrieve and convert primitive types
     *          such as bool, char, short, double, int, etc.
     *          For more details see description of ReadConverter class
     **/
    ReadConverter   mReadConvert;
    /**
     * \brief   Object of converter class.
     *          Used to convert and write primitive types
     *          such as bool, char, short, double, int, etc.
     *          For more details see description of WriteConverter class
     **/
    WriteConverter    mWriteConvert;

//////////////////////////////////////////////////////////////////////////
// Private function class.
//////////////////////////////////////////////////////////////////////////
private:
    inline FileBase & self( void );
    inline const FileBase & self( void ) const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( FileBase );
};

//////////////////////////////////////////////////////////////////////////
// FileBase class inline functions
//////////////////////////////////////////////////////////////////////////
inline FileBase & FileBase::self( void )
{
    return (*this);
}

inline const FileBase & FileBase::self( void ) const
{
    return (*this);
}

inline const String & FileBase::getName( void ) const
{
    return mFileName;
}

inline unsigned int FileBase::getMode( void ) const
{
    return mFileMode;
}

inline bool FileBase::isValid( void ) const
{
    return isOpened();
}

inline bool FileBase::isForceDelete( void ) const
{
    return (getMode() & static_cast<int>(FOB_FOR_DELETE)) != 0;
}

inline bool FileBase::isTemporary( void ) const
{
    return (getMode() & static_cast<int>(FO_MODE_CREATE_TEMP)) != 0;
}

inline bool FileBase::isAttachMode( void ) const
{
    return (getMode() & static_cast<int>(FOB_ATTACH)) != 0;
}

inline bool FileBase::isDetachMode( void ) const
{
    return (getMode() & static_cast<int>(FOB_DETACH)) != 0;
}

inline bool FileBase::isTextMode( void ) const
{
    return ( (getMode() & static_cast<int>(FOB_TEXT)) != 0 );
}

inline bool FileBase::isBinaryMode( void ) const
{
    return ( (getMode() & static_cast<int>(FOB_BINARY)) != 0 );
}

inline bool FileBase::isShareForRead( void ) const
{
    return ( (getMode() & static_cast<int>(FOB_SHARE_READ)) != 0 );
}

inline bool FileBase::isSharedForWrite( void ) const
{
    return ( (getMode() & static_cast<int>(FOB_SHARE_WRITE)) != 0 );
}

inline bool FileBase::isEndOfFile( void ) const
{
    ASSERT(isOpened());
    return ( getPosition() < getLength() );
}

inline bool FileBase::canWrite( void ) const
{
    ASSERT(isOpened());
    return ( (getMode() & static_cast<int>(FOB_WRITE)) != 0 );
}

inline bool FileBase::canRead( void ) const
{
    ASSERT(isOpened());
    return ( (getMode() & static_cast<int>(FOB_READ)) != 0 );
}

inline bool FileBase::moveToBegin( void )
{
    ASSERT(isOpened());
    return IECursorPosition::moveToBegin();
}

inline bool FileBase::moveToEnd( void )
{
    ASSERT(isOpened());
    return IECursorPosition::moveToEnd();
}

inline unsigned int FileBase::getStartPosition( void )
{
    return IECursorPosition::START_CURSOR_POSITION;
}

inline unsigned int FileBase::getInvalidPosition( void )
{
    return IECursorPosition::INVALID_CURSOR_POSITION;
}

inline const IEInStream& FileBase::getReadStream( void ) const
{
    ASSERT(isOpened());
    return static_cast<const IEInStream &>(*this);
}

inline IEOutStream& FileBase::getWriteStream( void )
{
    ASSERT(isOpened());
    return static_cast<IEOutStream &>(*this);
}

inline bool FileBase::readBool(bool & OUT outValue) const
{
    return mReadConvert.getBool(outValue);
}

inline bool FileBase::readChar( char & OUT outValue ) const
{
    return mReadConvert.getChar(outValue);
}

inline bool FileBase::readChar( wchar_t & OUT outValue ) const
{
    return mReadConvert.getChar(outValue);
}

inline bool FileBase::readShort( short & OUT outValue ) const
{
    return mReadConvert.getShort(outValue);
}

inline bool FileBase::readInt( int & OUT outValue ) const
{
    return mReadConvert.getInt(outValue);
}

inline bool FileBase::readLarge( int64_t & OUT outValue ) const
{
    return mReadConvert.getInt64(outValue);
}

inline bool FileBase::readFloat(float & OUT outValue) const
{
    return mReadConvert.getFloat(outValue);
}

inline bool FileBase::readDouble(double & OUT outValue) const
{
    return mReadConvert.getDouble(outValue);
}

inline bool FileBase::writeBool( bool inValue )
{
    return mWriteConvert.setBool(inValue);
}

inline bool FileBase::writeChar( char inValue )
{
    return mWriteConvert.setChar(inValue);
}

inline bool FileBase::writeChar( wchar_t inValue )
{
    return mWriteConvert.setChar(inValue);
}

inline bool FileBase::writeShort( short inValue )
{
    return mWriteConvert.setShort(inValue);
}

inline bool FileBase::writeInt( int inValue )
{
    return mWriteConvert.setInt(inValue);
}

inline bool FileBase::writeLarge( int64_t inValue )
{
    return mWriteConvert.setInt64(inValue);
}

inline bool FileBase::writeFloat( float inValue )
{
    return mWriteConvert.setFloat(inValue);
}

inline bool FileBase::writeDouble( double inValue )
{
    return mWriteConvert.setDouble(inValue);
}

inline FileBase & operator << ( FileBase & stream, const char * ascii )
{
    stream.write(ascii);
    return stream;
}

inline FileBase & operator << ( FileBase & stream, const String & ascii )
{
    stream.write( ascii );
    return stream;
}

inline FileBase & operator << ( FileBase & stream, const wchar_t * wide )
{
    stream.write( wide );
    return stream;
}

inline FileBase & operator << ( FileBase & stream, const WideString & wide )
{
    stream.write( wide );
    return stream;
}

inline const FileBase & operator >> ( const FileBase & stream, String & OUT ascii )
{
    stream.read(ascii);
    return stream;
}

inline const FileBase & operator >> ( const FileBase & stream, WideString & OUT wide )
{
    stream.read(wide);
    return stream;
}

#endif  // AREG_BASE_FILEBASE_HPP
