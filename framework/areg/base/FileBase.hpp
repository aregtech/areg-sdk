#ifndef AREG_BASE_FILEBASE_HPP
#define AREG_BASE_FILEBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/FileBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Base class of file objects.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/Cursor.hpp"

#include "areg/base/String.hpp"
#include "areg/base/private/ReadConverter.hpp"
#include "areg/base/private/WriteConverter.hpp"

#include <string_view>

class String;
class WideString;
class ByteBuffer;

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
 *          1. Invalid      - Invalid mode. No operation should be performed. Prevent caller setting this mode;
 *
 *          2. OnlyRead     - File opened exclusively for reading.
 *
 *          3. OnlyWrite    - File opened exclusively for writing.
 *
 *          4. Read         - File opened for reading. By default, all files should be opened with this flag.
 *
 *          5. Write        - File opened for writing and reading. If flag is missed, the write functionality will be disabled.
 *
 *          6. Binary       - File opened in binary mode. All data will be read and written as binary data.
 *                            For strings, the null-terminate symbol will be also written / read.
 *
 *          7. Text         - File opened int32_t text mode. All data will be written as plain text, 
 *                            until does not match null-terminate symbol.
 *              
 *          8. ShareRead    - File is opened in share read mode. The other File object is able to read.
 *                            This flag makes sense for FS File object. For memory file it does not make sense,
 *                            since buffer pointer may change address and File object is not able to control 
 *                            memory access by other functionality.
 *
 *          9. ShareWrite   - File is opened in share write and read modes. The other File object is able to write and read data.
 *                            This flag makes sense for FS File object. For memory file it does not make sense,
 *                            since buffer pointer may change address and File object is not able to control 
 *                            memory access by other functionality.
 *    
 *          10.Create       - If file does not exist, the new file will be created. The size will set zero
 *                            If Exist flag is set, will try to open existing file, otherwise creates new file.
 *
 *          11.Exist        - Opens only existing file. Opening might fail if file does not exist and 'Create' flag is not set.
 *                            If Create flag is set and file does not exist, it will create new file.
 *                            If Truncate flag is set, opens file for read/write and sets initial size of file to zero.
 *
 *         12.Truncate      - Opens file in read and write mode, sets initial size of file to zero. 
 *                            Ignored in mode Attach.
 *                            Can be used with combination Create and Exist.
 *
 *         13.Attach        - Will use existing file handle / memory buffer pointer only in read mode. 
 *                            On close neither handle, nor buffer pointer will be closed / deleted.
 *
 *         14.Detach        - Will open file / memory buffer for read and write access.
 *                            On close neither handle, nor buffer pointer will be closed / deleted.
 *
 *         15.Delete        - Will force to delete on close even if buffer is attached or marked as detached.
 *
 *         16.WriteDirect   - Will open file for writing access without buffering or caching.
 *                            Read and write flags are set automatically.
 *                            Write operations will not go through any intermediate cache,
 *                            they will go directly to disk. 
 *
 *         15.CreateTemp    - Will create and open file for temporary storage.
 *                            File systems avoid writing data back to mass storage if sufficient cache memory is available,
 *                            because an application deletes a temporary file after a handle is closed.
 *                            In that case, the system can entirely avoid writing the data.
 **/
class AREG_API FileBase : public IOStream
                        , public Cursor
{
//////////////////////////////////////////////////////////////////////////
// Defined types
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   File open bits specifying opening mode, access rights and attributes
     **/
    enum class OpenFlag : uint32_t
    {
          BitNone       = 0u    //!< 0000000000000000 <= invalid
        , BitRead       = 1u    //!< 0000000000000001 <= read bit
        , BitWrite      = 2u    //!< 0000000000000010 <= write bit
        , BitBinary     = 4u    //!< 0000000000000100 <= binary bit
        , BitText       = 8u    //!< 0000000000001000 <= text bit
        , BitShareRead  = 16u   //!< 0000000000010000 <= shared read bit
        , BitShareWrite = 32u   //!< 0000000000100000 <= shared write bit
        , BitCreate     = 64u   //!< 0000000001000000 <= create bit
        , BitExist      = 128u  //!< 0000000010000000 <= must exist bit
        , BitTruncate   = 256u  //!< 0000000100000000 <= truncate bit
        , BitAttach     = 512u  //!< 0000001000000000 <= attached bit
        , BitDetach     = 1024u //!< 0000010000000000 <= detach bit
        , BitDelete     = 2048u //!< 0000100000000000 <= delete on close bit
        , BitDirect     = 4096u //!< 0001000000000000 <= write direct on disk bit
        , BitTemp       = 8192u //!< 0010000000000000 <= create temporary file bit, will be deleted on close.
    };

private:
    static constexpr uint32_t   BIT_INVALID     {static_cast<uint32_t>(OpenFlag::BitNone)};
    static constexpr uint32_t   BIT_READ        { static_cast<uint32_t>(OpenFlag::BitRead) };
    static constexpr uint32_t   BIT_WRITE       { static_cast<uint32_t>(OpenFlag::BitWrite) };
    static constexpr uint32_t   BIT_BINARY      { static_cast<uint32_t>(OpenFlag::BitBinary) };
    static constexpr uint32_t   BIT_TEXT        { static_cast<uint32_t>(OpenFlag::BitText) };
    static constexpr uint32_t   BIT_SHARE_READ  { static_cast<uint32_t>(OpenFlag::BitShareRead) };
    static constexpr uint32_t   BIT_SHARE_WRITE { static_cast<uint32_t>(OpenFlag::BitShareWrite) };
    static constexpr uint32_t   BIT_CREATE      { static_cast<uint32_t>(OpenFlag::BitCreate) };
    static constexpr uint32_t   BIT_EXIST       { static_cast<uint32_t>(OpenFlag::BitExist) };
    static constexpr uint32_t   BIT_TRUNCATE    { static_cast<uint32_t>(OpenFlag::BitTruncate) };
    static constexpr uint32_t   BIT_ATTACH      { static_cast<uint32_t>(OpenFlag::BitAttach) };
    static constexpr uint32_t   BIT_DETACH      { static_cast<uint32_t>(OpenFlag::BitDetach) };
    static constexpr uint32_t   BIT_DELETE      { static_cast<uint32_t>(OpenFlag::BitDelete) };
    static constexpr uint32_t   BIT_DIRECT      { static_cast<uint32_t>(OpenFlag::BitDirect) };
    static constexpr uint32_t   BIT_FILE        { static_cast<uint32_t>(OpenFlag::BitTemp) };

public:
    /**
     * \brief   File opening modes.
     **/
    enum class OpenMode : uint32_t
    {
          Invalid       = (BIT_INVALID)                                                     //!< 0000000000000000 <= invalid mode

        , OnlyRead      = (BIT_READ)                                                        //!< 0000000000000001 <= exclusive read, contains only read bit
        , OnlyWrite     = (BIT_WRITE | BIT_READ)                                            //!< 0000000000000011 <= exclusive write, should contain "read" bit
        , Read          = (BIT_READ | BIT_SHARE_READ)                                       //!< 0000000000010001 <= read mode, can share read
        , Write         = (BIT_WRITE | BIT_READ | BIT_SHARE_READ)                           //!< 0000000000010011 <= write mode, should contain "read" bit and can share read
        , Binary        = (BIT_BINARY)                                                      //!< 0000000000000100 <= binary bit. strings in binary mode will write EOS char, all data will be dumped.
        , Text          = (BIT_TEXT)                                                        //!< 0000000000001000 <= text bit. EOS char of string will not be written, can write and read line of string
        , ShareRead     = (BIT_SHARE_READ  | BIT_READ)                                      //!< 0000000000010001 <= share read mode, should contain "read" bit
        , ShareWrite    = (BIT_SHARE_WRITE | BIT_SHARE_READ | BIT_WRITE | BIT_READ)         //!< 0000000000110011 <= share write mode, should contain "read", "write" and "share read" bits
        , Create        = (BIT_CREATE)                                                      //!< 0000000001000000 <= always create file
        , Exist         = (BIT_EXIST)                                                       //!< 0000000010000000 <= file should exist, otherwise it fails
        , Truncate      = (BIT_TRUNCATE | BIT_WRITE | BIT_READ)                             //!< 0000000100000011 <= truncate file, i.e. set initial size zero, "read" and "write" bits should be set
        , Attach        = (BIT_ATTACH | BIT_EXIST | BIT_SHARE_READ | BIT_READ)              //!< 0000001010010001 <= attached handle (buffer). Can only read memory data, cannot change the size or write data. Bits "read", "share read", "exist" are set automatically. Not able to control share mode, the pointer should be passed. Should not free buffer in destructor.
        , Detach        = (BIT_DETACH | BIT_CREATE | BIT_SHARE_READ | BIT_WRITE | BIT_READ) //!< 0000010001010011 <= mode detach buffer. Can read and write memory data. Bits "read", "write", "share read" and "create" are set automatically. Not able to control share modes. Should not free buffer in destructor, it is up to caller to free buffer.
        , Delete        = (BIT_DELETE)                                                      //!< 0000100000000000 <= mode to delete on close. Can be combined with any mode. The file / buffer will be deleted even if mode attached / detach are set.
        , WriteDirect   = (BIT_DIRECT | BIT_WRITE | BIT_READ)                               //!< 0001000000000011 <= write operations will not go through any intermediate cache, they will go directly to disk. read and write flags are set automatically.
        , CreateTemp    = (BIT_FILE | BIT_WRITE | BIT_READ)                                 //!< 0010000000000011 <= The file is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because an application deletes a temporary file after a handle is closed. In that case, the system can entirely avoid writing the data.
    };

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
    FileBase();

    /**
     * \brief   Destructor.
     **/
    virtual ~FileBase() = default;

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
    friend inline const FileBase & operator >> ( const FileBase & stream, String & ascii );

    /**
     * \brief   Writes files data as wide-char string. The file outputs string until reaches end of file,
     *          or meets first unprintable symbol. On output, the 'ascii'
     * \param   stream  The instance of file data contains the data.
     * \param   wide    On output, this contains the wide-char string data.
     **/
    friend inline const FileBase & operator >> ( const FileBase & stream, WideString & wide );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns instance of streaming object to read data.
     **/
    inline const InStream & getReadStream() const;

    /**
     * \brief   Returns instance of streaming object to write data.
     **/
    inline OutStream & getWriteStream();

    /**
     * \brief   Returns the name of file object set by user. This can be either int16_t name
     *          or normalized full path. Can be empty string for buffered file.
     * \return  Returns the given name of file.
     **/
    inline const String & getName() const;

    /**
     * \brief   Returns the file open mode (bits)
     **/
    inline uint32_t getMode() const;

/************************************************************************/
// State functions
/************************************************************************/
    /**
     * \brief   Returns true, if file is valid. The file is valid if it is opened for read and/or write.
     */
    inline bool isValid() const;

    /**
     * \brief	Returns true if file is opened in mode to force to delete.
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isForceDelete() const;

    /**
     * \brief   Returns true if file is opened in temporary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isTemporary() const;

    /**
     * \brief	Returns true if buffer of memory file is attached
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isAttachMode() const;

    /**
     * \brief   Returns true if buffer of memory file should be detached (on close)
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isDetachMode() const;

    /**
     * \brief   Returns true if file pointer position is at the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isEndOfFile() const;

    /**
     * \brief   Returns true if file is opened in text mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isTextMode() const;

    /**
     * \brief	Returns true if file is opened in binary mode
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isBinaryMode() const;

    /**
     * \brief   Returns true if read sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isShareForRead() const;

    /**
     * \brief   Returns true if write sharing of opened file is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool isSharedForWrite() const;

    /**
     * \brief   Returns true if write operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool canWrite() const;

    /**
     * \brief   Returns true if read operation is permitted
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool canRead() const;

/************************************************************************/
// Cursor position operation functions
/************************************************************************/
    /**
     * \brief   Move file pointer to the beginning of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool moveToBegin() const;

    /**
     * \brief   Move file pointer to the end of file
     *          File object must be opened before calling, otherwise will fail with assertion.
     **/
    inline bool moveToEnd() const;

    /**
     * \brief   Returns the start position value.
     **/
    inline static uint32_t getStartPosition();

    /**
     * \brief   Return the invalid position value.
     **/
    inline static uint32_t getInvalidPosition();

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
    int32_t append(const uint8_t* buffer, uint32_t length);

    /**
     * \brief	Read data in inverted order.
     * \param	buffer	The buffer to read out data
     * \param	length	The length of buffer in bytes
     * \return	Returns the size of data read out.
     **/
    int32_t readInvert(uint8_t* buffer, uint32_t length) const;

    /**
     * \brief	Write data in inverted order
     * \param	buffer	The buffer of data
     * \param	length	The length of data to write
     * \return	Returns the size of written data in bytes.
     **/
    int32_t writeInvert( const uint8_t * buffer, uint32_t length );

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
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value Cursor::INVALID_CURSOR_POSITION.
     **/
    uint32_t resizeAndFill(uint32_t newSize, uint8_t fillValue);

/************************************************************************/
// Read / Write simple types
/************************************************************************/
    /**
     * \brief   Reads boolean value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readBool(bool & Value) const;

    /**
     * \brief   Reads 1 byte of data, covert to char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readChar(char & Value) const;

    /**
     * \brief   Reads 2 bytes of data, covert to wide char and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readChar(wchar_t & Value) const;

    /**
     * \brief   Reads 2 bytes of data, covert to int16_t integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readShort(int16_t & Value) const;

    /**
     * \brief   Reads 4 bytes of data, covert to integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readInt(int32_t & Value) const;

    /**
     * \brief   Reads 8 bytes of data, covert to large integer and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readLarge(int64_t & Value) const;

    /**
     * \brief   Reads 2 bytes of data, covert to floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readFloat(float & Value) const;

    /**
     * \brief   Reads 4 bytes of data, covert to double floating value and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param[out]  Value   On output contains value that could read.
     **/
    inline bool readDouble(double & Value) const;

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
     * \brief   Write 2 bytes of int16_t integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeShort(int16_t inValue);

    /**
     * \brief   Write 4 bytes of integer data and if succeeds, returns true.
     *          If fails, this will not change the current file pointer position
     * \param   inValue [in]    Value to write.
     **/
    inline bool writeInt(int32_t inValue);

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
     * \param[in,out]   buffer      The buffer where the string should be written. 
     * \param[int32_t]      charCount   Number of characters to write in the string buffer.
     *                              The string buffer should be long enough to set
     *                              end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int32_t readString( char * buffer, int32_t charCount ) const;
    int32_t readString( wchar_t * buffer, int32_t charCount ) const;

    /**
     * \brief	Read string until end of file or null-terminated character.
     *          It does not validate string data.
     * 
     * \param[out]  buffer  The string containing data
     * \return	Returns number of characters, which could read.
     **/
    int32_t readString(String & buffer) const;
    int32_t readString(WideString & buffer) const;

    /**
     * \brief	Reads a line of string, automatically sets null-terminate char at
     *          the end and returns number of characters, which could read.
     * 
     * \param[in,out]   buffer      The buffer where the string should be written. 
     * \param[int32_t]      charCount   Number of characters to write in the string buffer.
     *                              The string buffer should be long enough to set
     *                              end-of-string character at the end.
     * \return	Returns number of characters, which could read.
     **/
    int32_t readLine( char * buffer, int32_t charCount) const;
    int32_t readLine( wchar_t * buffer, int32_t charCount ) const;

    /**
     * \brief	Reads a line of string until end of file, or first match of 
     *          null-terminated character or new line character. 
     *          It does not validate string data.
     * 
     * \param[out]  buffer  On output, this contains a string of a line.
     * \return	Returns number of characters, which could read.
     **/
    int32_t readLine(String & buffer) const;
    int32_t readLine(WideString & buffer) const;

    /**
     * \brief	Writes string in file and returns true if succeeded.
     *          If file was opened in Text, the null-terminate char of string will be skipped.
     *          If file was opened in Binary, the null-terminate char of string will be also written.
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
    bool write(const char * ascii);
    bool write(const wchar_t * wide);

    /**
     * \brief   Searches the given binary data in the file and returns the position where the data starts.
     * \param   startPos    The position in the file to start to search.
     * \param   buffer      The binary data buffer to search.
     * \param   length      The length of the data to search.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (Cursor::INVALID_CURSOR_POSITION).
     **/
    uint32_t searchData( uint32_t startPos, const uint8_t * buffer, uint32_t length ) const;
    /**
     * \brief   Searches the given binary data in the file and returns the position where the data starts.
     * \param   startPos    The position in the file to start to search.
     * \param   buffer      The buffer of byte-binary data to search.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (Cursor::INVALID_CURSOR_POSITION).
     **/
    uint32_t searchData( uint32_t startPos, const ByteBuffer & buffer ) const;

    /**
     * \brief   Searches the given null-terminated text in the file and returns the position where the data starts.
     * \param   startPos        The position in the file to start to search.
     * \param   text            The null-terminated text to search.
     * \param   caseSensitive   If true, the searching of text is case sensitive.
     * \return  If found, returns the valid position in the file where the binary data starts.
     *          Otherwise, returns invalid position (Cursor::INVALID_CURSOR_POSITION).
     **/
    uint32_t searchText( uint32_t startPos, const char * text, bool caseSensitive ) const;
    uint32_t searchText( uint32_t startPos, const wchar_t * text, bool caseSensitive ) const;
    uint32_t searchText( uint32_t startPos, const String & text, bool caseSensitive ) const;
    uint32_t searchText( uint32_t startPos, const WideString & text, bool caseSensitive ) const;

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
    virtual bool open() = 0;

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
     * \see     close()
     **/
    virtual bool open(const String& fileName, uint32_t mode) = 0;

    /**
     * \brief   Call to close file object.
     *          If file was opened in Attach or Detach modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FOR_DELETE or CreateTemp. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If CreateTemp is set, file object is always deleted on close.
     *          If FO_FOR_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     *
     * \see     open()
     **/
    virtual void close() = 0;

    /**
     * \brief	Removes opened file. This will force to remove file object even if it is attached memory buffered file.
     * \return	Returns true if succeeded.
     **/
    virtual bool remove() = 0;

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual uint32_t getLength() const = 0;

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
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value Cursor::INVALID_CURSOR_POSITION.
     **/
    virtual uint32_t reserve(uint32_t newSize) = 0;

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, return true
     **/
    virtual bool truncate() = 0;

/************************************************************************/
// IOStream pure virtual
/************************************************************************/

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual uint32_t read( uint8_t * buffer, uint32_t size ) const override = 0;

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. In this class 
     *          writes data into opened file.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual uint32_t write( const uint8_t* buffer, uint32_t size ) override = 0;

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
     * \brief   Clears the buffers for the file and causes all buffered data 
     *          to be written to the file.
     **/
    void flush() override;

protected:

    /**
     * \brief	Validates and normalize bits for file open mode.
     * \param	mode	Integer value of bitwise OR operation of OpenMode values
     * \return	Returns normalized value.
     **/
    virtual uint32_t normalizeMode(uint32_t mode) const;

/************************************************************************/
// OutStream overrides
/************************************************************************/
    /**
     * \brief   Resets cursor pointer and moves to the begin of data.
     *          Implement the function if stream has pointer reset mechanism
     **/
    void resetCursor() const override;

    /**
     * \brief   Normalizes the name, replace special masks such as time-stamp or process name in the give name.
     * \param[in,out]   name    On output, contains normalized name of file.
     **/
    static void normalizeName( String & name );

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
    uint32_t    mFileMode;

private:
    /**
     * \brief   Object of converter class.
     *          Used to retrieve and convert primitive types
     *          such as bool, char, int16_t, double, int32_t, etc.
     *          For more details see description of ReadConverter class
     **/
    ReadConverter   mReadConvert;
    /**
     * \brief   Object of converter class.
     *          Used to convert and write primitive types
     *          such as bool, char, int16_t, double, int32_t, etc.
     *          For more details see description of WriteConverter class
     **/
    WriteConverter    mWriteConvert;

//////////////////////////////////////////////////////////////////////////
// Private function class.
//////////////////////////////////////////////////////////////////////////
private:
    inline FileBase & self();
    inline const FileBase & self() const;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( FileBase );
};

//////////////////////////////////////////////////////////////////////////
// FileBase class inline functions
//////////////////////////////////////////////////////////////////////////
inline FileBase & FileBase::self()
{
    return (*this);
}

inline const FileBase & FileBase::self() const
{
    return (*this);
}

inline const String & FileBase::getName() const
{
    return mFileName;
}

inline uint32_t FileBase::getMode() const
{
    return mFileMode;
}

inline bool FileBase::isValid() const
{
    return isOpened();
}

inline bool FileBase::isForceDelete() const
{
    return (getMode() & static_cast<uint32_t>(OpenFlag::BitDelete)) != 0;
}

inline bool FileBase::isTemporary() const
{
    return (getMode() & static_cast<uint32_t>(OpenFlag::BitTemp)) != 0;
}

inline bool FileBase::isAttachMode() const
{
    return (getMode() & static_cast<uint32_t>(OpenFlag::BitAttach)) != 0;
}

inline bool FileBase::isDetachMode() const
{
    return (getMode() & static_cast<uint32_t>(OpenFlag::BitDetach)) != 0;
}

inline bool FileBase::isTextMode() const
{
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitText)) != 0 );
}

inline bool FileBase::isBinaryMode() const
{
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitBinary)) != 0 );
}

inline bool FileBase::isShareForRead() const
{
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitShareRead)) != 0 );
}

inline bool FileBase::isSharedForWrite() const
{
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitShareWrite)) != 0 );
}

inline bool FileBase::isEndOfFile() const
{
    ASSERT(isOpened());
    return ( getPosition() < getLength() );
}

inline bool FileBase::canWrite() const
{
    ASSERT(isOpened());
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitWrite)) != 0 );
}

inline bool FileBase::canRead() const
{
    ASSERT(isOpened());
    return ( (getMode() & static_cast<uint32_t>(OpenFlag::BitRead)) != 0 );
}

inline bool FileBase::moveToBegin() const
{
    ASSERT(isOpened());
    return Cursor::moveToBegin();
}

inline bool FileBase::moveToEnd() const
{
    ASSERT(isOpened());
    return Cursor::moveToEnd();
}

inline uint32_t FileBase::getStartPosition()
{
    return Cursor::START_CURSOR_POSITION;
}

inline uint32_t FileBase::getInvalidPosition()
{
    return Cursor::INVALID_CURSOR_POSITION;
}

inline const InStream& FileBase::getReadStream() const
{
    ASSERT(isOpened());
    return static_cast<const InStream &>(*this);
}

inline OutStream& FileBase::getWriteStream()
{
    ASSERT(isOpened());
    return static_cast<OutStream &>(*this);
}

inline bool FileBase::readBool(bool & Value) const
{
    return mReadConvert.getBool(Value);
}

inline bool FileBase::readChar( char & Value ) const
{
    return mReadConvert.getChar(Value);
}

inline bool FileBase::readChar( wchar_t & Value ) const
{
    return mReadConvert.getChar(Value);
}

inline bool FileBase::readShort( int16_t & Value ) const
{
    return mReadConvert.getShort(Value);
}

inline bool FileBase::readInt( int32_t & Value ) const
{
    return mReadConvert.getInt(Value);
}

inline bool FileBase::readLarge( int64_t & Value ) const
{
    return mReadConvert.getInt64(Value);
}

inline bool FileBase::readFloat(float & Value) const
{
    return mReadConvert.getFloat(Value);
}

inline bool FileBase::readDouble(double & Value) const
{
    return mReadConvert.getDouble(Value);
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

inline bool FileBase::writeShort( int16_t inValue )
{
    return mWriteConvert.setShort(inValue);
}

inline bool FileBase::writeInt( int32_t inValue )
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

inline const FileBase & operator >> ( const FileBase & stream, String & ascii )
{
    stream.read(ascii);
    return stream;
}

inline const FileBase & operator >> ( const FileBase & stream, WideString & wide )
{
    stream.read(wide);
    return stream;
}

#endif  // AREG_BASE_FILEBASE_HPP
