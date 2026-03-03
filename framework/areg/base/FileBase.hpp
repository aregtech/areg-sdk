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
#include "areg/base/areg_global.h"
#include "areg/base/IOStream.hpp"
#include "areg/base/Cursor.hpp"

#include "areg/base/String.hpp"
#include "areg/base/private/ReadConverter.hpp"
#include "areg/base/private/WriteConverter.hpp"

#include <string_view>


namespace areg {

    class String;
    class WideString;
    class ByteBuffer;
} // namespace areg

namespace areg {

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
 *         13.Delete        - Deletes the file when the last handle is closed (delete-on-close).
 *                            On Win32 the OS guarantees deletion; on POSIX the file is unlinked at
 *                            close time. For memory FileBuffer, only the sole owner (non-shared
 *                            reference holder) may delete the backing buffer.
 *
 *         14.WriteDirect   - Will open file for writing access without buffering or caching.
 *                            Read and write flags are set automatically.
 *                            Write operations will not go through any intermediate cache,
 *                            they will go directly to disk. 
 *
 *         15.CreateTemp    - Will create and open file for temporary storage.
 *                            File systems avoid writing data back to mass storage if sufficient cache memory is available,
 *                            because an application deletes a temporary file after a handle is closed.
 *                            In that case, the system can entirely avoid writing the data.
 **/
/**
 * \brief   Pure virtual class providing file I/O operations with data streaming support, supporting
 *          read/write positioning, text/binary modes, and multiple file types (file system and
 *          memory buffer).
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
        , BitCreateNew  = 64u   //!< 0000000001000000 <= create bit
        , BitExist      = 128u  //!< 0000000010000000 <= must exist bit
        , BitTruncate   = 256u  //!< 0000000100000000 <= truncate bit
        , BitDelete     = 2048u //!< 0000100000000000 <= delete on close bit
        , BitDirect     = 4096u //!< 0001000000000000 <= write direct on disk bit
        , BitTemp       = 8192u //!< 0010000000000000 <= create temporary file bit, will be deleted on close.
        , BitOpenAlways = 16384u//!< 0100000000000000 <= open existing or create new.
    };

private:
    static constexpr uint32_t   BIT_INVALID     {static_cast<uint32_t>(OpenFlag::BitNone)};
    static constexpr uint32_t   BIT_READ        { static_cast<uint32_t>(OpenFlag::BitRead) };
    static constexpr uint32_t   BIT_WRITE       { static_cast<uint32_t>(OpenFlag::BitWrite) };
    static constexpr uint32_t   BIT_BINARY      { static_cast<uint32_t>(OpenFlag::BitBinary) };
    static constexpr uint32_t   BIT_TEXT        { static_cast<uint32_t>(OpenFlag::BitText) };
    static constexpr uint32_t   BIT_SHARE_READ  { static_cast<uint32_t>(OpenFlag::BitShareRead) };
    static constexpr uint32_t   BIT_SHARE_WRITE { static_cast<uint32_t>(OpenFlag::BitShareWrite) };
    static constexpr uint32_t   BIT_CREATE      { static_cast<uint32_t>(OpenFlag::BitCreateNew) };
    static constexpr uint32_t   BIT_EXIST       { static_cast<uint32_t>(OpenFlag::BitExist) };
    static constexpr uint32_t   BIT_TRUNCATE    { static_cast<uint32_t>(OpenFlag::BitTruncate) };
    static constexpr uint32_t   BIT_DELETE      { static_cast<uint32_t>(OpenFlag::BitDelete) };
    static constexpr uint32_t   BIT_DIRECT      { static_cast<uint32_t>(OpenFlag::BitDirect) };
    static constexpr uint32_t   BIT_FILE        { static_cast<uint32_t>(OpenFlag::BitTemp) };
    static constexpr uint32_t   BIT_OPEN_ALWAYS { static_cast<uint32_t>(OpenFlag::BitOpenAlways) };

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
        , Delete        = (BIT_DELETE)                                                      //!< 0000100000000000 <= delete on close. The file is deleted when the last handle is closed. For filesystem files the OS handles deletion; for memory buffers only the owner (sole-reference holder) may delete.
        , WriteDirect   = (BIT_DIRECT | BIT_WRITE | BIT_READ)                               //!< 0001000000000011 <= write operations will not go through any intermediate cache, they will go directly to disk. read and write flags are set automatically.
        , CreateTemp    = (BIT_FILE | BIT_WRITE | BIT_READ)                                 //!< 0010000000000011 <= The file is being used for temporary storage. File systems avoid writing data back to mass storage if sufficient cache memory is available, because an application deletes a temporary file after a handle is closed. In that case, the system can entirely avoid writing the data.
        , OpenAlways    = (BIT_OPEN_ALWAYS)                                                 //!< 0100000000000000 <= open existing or create new. If file does not exist, it will be created. If file exists, it will be opened.
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
     * \brief   Writes ASCII string to the file, handling null-termination based on file mode.
     *
     * \param[in,out] stream      The file stream to write to.
     * \param   ascii       The ASCII string to write.
     * \return  Reference to the file stream.
     **/
    friend inline FileBase & operator << (FileBase & stream, const char * ascii );

    /**
     * \brief   Writes ASCII string to the file, handling null-termination based on file mode.
     *
     * \param[in,out] stream      The file stream to write to.
     * \param   ascii       The ASCII string to write.
     * \return  Reference to the file stream.
     * \note    String overload.
     **/
    friend inline FileBase & operator << (FileBase & stream, const String & ascii );

    /**
     * \brief   Writes wide-char string to the file, handling null-termination based on file mode.
     *
     * \param[in,out] stream      The file stream to write to.
     * \param   wide        The wide-char string to write.
     * \return  Reference to the file stream.
     **/
    friend inline FileBase & operator << (FileBase & stream, const wchar_t * wide );

    /**
     * \brief   Writes wide-char string to the file, handling null-termination based on file mode.
     *
     * \param[in,out] stream      The file stream to write to.
     * \param   wide        The wide-char string to write.
     * \return  Reference to the file stream.
     * \note    WideString overload.
     **/
    friend inline FileBase & operator << (FileBase & stream, const WideString & wide );

    /**
     * \brief   Reads file data as ASCII string, stopping at end of file or first unprintable
     *          character.
     *
     * \param   stream      The file stream to read from.
     * \param[out] ascii       On output, contains the ASCII string data read.
     * \return  Reference to the file stream.
     **/
    friend inline const FileBase & operator >> ( const FileBase & stream, String & ascii );

    /**
     * \brief   Reads file data as wide-char string, stopping at end of file or first unprintable
     *          character.
     *
     * \param   stream      The file stream to read from.
     * \param[out] wide        On output, contains the wide-char string data read.
     * \return  Reference to the file stream.
     * \note    WideString overload.
     **/
    friend inline const FileBase & operator >> ( const FileBase & stream, WideString & wide );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the input stream object.
     **/
    inline const InStream & read_stream() const;

    /**
     * \brief   Returns the output stream object.
     **/
    inline OutStream & write_stream();

    /**
     * \brief   Returns the name of the file object set by user. This can be either a short name or
     *          normalized full path. Can be empty for buffered files.
     *
     * \return  Returns the given name of file.
     **/
    inline const String & name() const;

    /**
     * \brief   Returns the file open mode as a bitwise combination of flags.
     **/
    inline uint32_t mode() const;

/************************************************************************/
// State functions
/************************************************************************/
    /**
     * \brief   Returns true if the file is valid and opened for read and/or write.
     **/
    inline bool is_valid() const;

    /**
     * \brief   Returns true if the file is opened with force-delete mode.
     **/
    inline bool is_force_delete() const;

    /**
     * \brief   Returns true if the file is opened in temporary mode.
     **/
    inline bool is_temporary() const;

    /**
     * \brief   Returns true if the file pointer is at the end of file.
     **/
    inline bool is_eof() const;

    /**
     * \brief   Returns true if the file is opened in text mode.
     **/
    inline bool is_text_mode() const;

    /**
     * \brief   Returns true if the file is opened in binary mode.
     **/
    inline bool is_binary_mode() const;

    /**
     * \brief   Returns true if read sharing of the opened file is permitted.
     **/
    inline bool is_shared_read() const;

    /**
     * \brief   Returns true if write sharing of the opened file is permitted.
     **/
    inline bool is_shared_write() const;

    /**
     * \brief   Returns true if write operations are permitted.
     **/
    inline bool can_write() const;

    /**
     * \brief   Returns true if read operations are permitted.
     **/
    inline bool can_read() const;

/************************************************************************/
// Cursor position operation functions
/************************************************************************/
    /**
     * \brief   Moves the file pointer to the beginning of the file.
     **/
    inline bool move_to_begin() const;

    /**
     * \brief   Moves the file pointer to the end of the file.
     **/
    inline bool move_to_end() const;

    /**
     * \brief   Returns the start position value (zero).
     **/
    inline static uint32_t start_position();

    /**
     * \brief   Returns the invalid position value.
     **/
    inline static uint32_t invalid_position();

/************************************************************************/
// Read / Write operation functions
/************************************************************************/
    /**
     * \brief   Moves the pointer to the end of the file, increases file size, appends data, and
     *          returns the number of bytes appended.
     *
     * \param   buffer      The buffer containing data to append.
     * \param   length      The size in bytes of data to copy from buffer.
     * \return  Returns the size of data appended in bytes.
     **/
    int32_t append(const uint8_t* buffer, uint32_t length);

    /**
     * \brief   Reads data in inverted (reversed) order.
     *
     * \param[out] buffer      On output, contains the data read in inverted order.
     * \param   length      The size in bytes of the buffer.
     * \return  Returns the size of data read in bytes.
     **/
    int32_t read_invert(uint8_t* buffer, uint32_t length) const;

    /**
     * \brief   Writes data in inverted (reversed) order.
     *
     * \param   buffer      The buffer containing data to write.
     * \param   length      The size in bytes of data to write.
     * \return  Returns the size of data written in bytes.
     **/
    int32_t write_invert( const uint8_t * buffer, uint32_t length );

    /**
     * \brief   Sets the new file size and fills resized space with a specified byte value. Returns
     *          the current pointer position.
     *
     * \param   newSize         The new size in bytes to set or reserve.
     * \param   fillValue       The byte value to fill any newly reserved space.
     * \return  Returns the current file pointer position on success; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     **/
    uint32_t resize_and_fill(uint32_t newSize, uint8_t fillValue);

/************************************************************************/
// Read / Write simple types
/************************************************************************/
    /**
     * \brief   Reads a boolean value and returns true on success; file pointer position unchanged
     *          on failure.
     *
     * \param[out] Value       On output, contains the boolean value read.
     **/
    inline bool read_bool(bool & Value) const;

    /**
     * \brief   Reads one byte and converts to char; returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param[out] Value       On output, contains the char value read.
     **/
    inline bool read_char(char & Value) const;

    /**
     * \brief   Reads two bytes and converts to wide char; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the wide char value read.
     * \note    Wide char overload.
     **/
    inline bool read_char(wchar_t & Value) const;

    /**
     * \brief   Reads two bytes and converts to int16_t; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the int16_t value read.
     **/
    inline bool read_short(int16_t & Value) const;

    /**
     * \brief   Reads four bytes and converts to int32_t; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the int32_t value read.
     **/
    inline bool read_int(int32_t & Value) const;

    /**
     * \brief   Reads eight bytes and converts to int64_t; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the int64_t value read.
     **/
    inline bool read_large(int64_t & Value) const;

    /**
     * \brief   Reads four bytes and converts to float; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the float value read.
     **/
    inline bool read_float(float & Value) const;

    /**
     * \brief   Reads eight bytes and converts to double; returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param[out] Value       On output, contains the double value read.
     **/
    inline bool read_double(double & Value) const;

    /**
     * \brief   Writes a boolean as one byte and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The boolean value to write.
     **/
    inline bool write_bool(bool inValue);

    /**
     * \brief   Writes one byte of char data and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The char value to write.
     **/
    inline bool write_char(char inValue);

    /**
     * \brief   Writes two bytes of wide char data and returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param   inValue     The wide char value to write.
     * \note    Wide char overload.
     **/
    inline bool write_char(wchar_t inValue);

    /**
     * \brief   Writes two bytes of int16_t data and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The int16_t value to write.
     **/
    inline bool write_short(int16_t inValue);

    /**
     * \brief   Writes four bytes of int32_t data and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The int32_t value to write.
     **/
    inline bool write_int(int32_t inValue);

    /**
     * \brief   Writes eight bytes of int64_t data and returns true on success; file pointer
     *          position unchanged on failure.
     *
     * \param   inValue     The int64_t value to write.
     **/
    inline bool write_large(int64_t inValue);

    /**
     * \brief   Writes four bytes of float data and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The float value to write.
     **/
    inline bool write_float(float inValue);

    /**
     * \brief   Writes eight bytes of double data and returns true on success; file pointer position
     *          unchanged on failure.
     *
     * \param   inValue     The double value to write.
     **/
    inline bool write_double(double inValue);

/************************************************************************/
// Read / Write strings
/************************************************************************/
    /**
     * \brief   Reads a string into the buffer, automatically null-terminates it, and returns the
     *          number of characters read.
     *
     * \param[out] buffer          On output, contains the string read.
     * \param   charCount       The maximum number of characters the buffer can hold.
     * \return  Returns the number of characters read.
     **/
    int32_t read_string( char * buffer, int32_t charCount ) const;
    /**
     * \brief   Reads a wide-char string into the buffer, automatically null-terminates it, and
     *          returns the number of characters read.
     *
     * \param[out] buffer          On output, contains the wide-char string read.
     * \param   charCount       The maximum number of characters the buffer can hold.
     * \return  Returns the number of characters read.
     * \note    Wide char overload.
     **/
    int32_t read_string( wchar_t * buffer, int32_t charCount ) const;

    /**
     * \brief   Reads a string until end of file or null-terminated character without validation.
     *
     * \param[out] buffer      On output, contains the string read.
     * \return  Returns the number of characters read.
     **/
    int32_t read_string(String & buffer) const;
    /**
     * \brief   Reads a wide-char string until end of file or null-terminated character without
     *          validation.
     *
     * \param[out] buffer      On output, contains the wide-char string read.
     * \return  Returns the number of characters read.
     * \note    WideString overload.
     **/
    int32_t read_string(WideString & buffer) const;

    /**
     * \brief   Reads a line into the buffer, automatically null-terminates it, and returns the
     *          number of characters read.
     *
     * \param[out] buffer          On output, contains the line read.
     * \param   charCount       The maximum number of characters the buffer can hold.
     * \return  Returns the number of characters read.
     **/
    int32_t read_line( char * buffer, int32_t charCount) const;
    /**
     * \brief   Reads a wide-char line into the buffer, automatically null-terminates it, and
     *          returns the number of characters read.
     *
     * \param[out] buffer          On output, contains the wide-char line read.
     * \param   charCount       The maximum number of characters the buffer can hold.
     * \return  Returns the number of characters read.
     * \note    Wide char overload.
     **/
    int32_t read_line( wchar_t * buffer, int32_t charCount ) const;

    /**
     * \brief   Reads a line until end of file, null-terminator, or newline character without
     *          validation.
     *
     * \param[out] buffer      On output, contains the line read.
     * \return  Returns the number of characters read.
     **/
    int32_t read_line(String & buffer) const;
    /**
     * \brief   Reads a wide-char line until end of file, null-terminator, or newline character
     *          without validation.
     *
     * \param[out] buffer      On output, contains the wide-char line read.
     * \return  Returns the number of characters read.
     * \note    WideString overload.
     **/
    int32_t read_line(WideString & buffer) const;

    /**
     * \brief   Writes a string to the file, handling null-termination based on file mode.
     *
     * \param   buffer      The string to write.
     * \return  Returns true if operation succeeded.
     **/
    bool write_string(const char* buffer);
    /**
     * \brief   Writes a wide-char string to the file, handling null-termination based on file mode.
     *
     * \param   buffer      The wide-char string to write.
     * \return  Returns true if operation succeeded.
     * \note    Wide char overload.
     **/
    bool write_string(const wchar_t* buffer);
    /**
     * \brief   Writes a string to the file, handling null-termination based on file mode.
     *
     * \param   buffer      The string to write.
     * \return  Returns true if operation succeeded.
     * \note    String overload.
     **/
    bool write_string(const String& buffer);
    /**
     * \brief   Writes a wide-char string to the file, handling null-termination based on file mode.
     *
     * \param   buffer      The wide-char string to write.
     * \return  Returns true if operation succeeded.
     * \note    WideString overload.
     **/
    bool write_string(const WideString& buffer);

    /**
     * \brief   Writes a string as a line with automatic newline appended; skips null-terminator and
     *          writes newline instead.
     *
     * \param   inValue     The string to write.
     * \return  Returns true if operation succeeded.
     **/
    bool write_line(const char* inValue);
    /**
     * \brief   Writes a wide-char string as a line with automatic newline appended; skips
     *          null-terminator and writes newline instead.
     *
     * \param   inValue     The wide-char string to write.
     * \return  Returns true if operation succeeded.
     * \note    Wide char overload.
     **/
    bool write_line(const wchar_t* inValue);
    /**
     * \brief   Writes a string as a line with automatic newline appended; skips null-terminator and
     *          writes newline instead.
     *
     * \param   buffer      The string to write.
     * \return  Returns true if operation succeeded.
     * \note    String overload.
     **/
    bool write_line(const String& buffer);
    /**
     * \brief   Writes a wide-char string as a line with automatic newline appended; skips
     *          null-terminator and writes newline instead.
     *
     * \param   buffer      The wide-char string to write.
     * \return  Returns true if operation succeeded.
     * \note    WideString overload.
     **/
    bool write_line(const WideString& buffer);

    /**
     * \brief   Writes a string to the file.
     *
     * \param   ascii       The ASCII string to write.
     * \return  Returns true if operation succeeded.
     **/
    bool write(const char * ascii);
    /**
     * \brief   Writes a wide-char string to the file.
     *
     * \param   wide    The wide-char string to write.
     * \return  Returns true if operation succeeded.
     * \note    Wide char overload.
     **/
    bool write(const wchar_t * wide);

    /**
     * \brief   Searches for binary data in the file starting from the specified position.
     *
     * \param   startPos    The position in the file to start searching.
     * \param   buffer      The binary data to search for.
     * \param   length      The length of the data to search.
     * \return  Returns the position where the data starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     **/
    uint32_t search_data( uint32_t startPos, const uint8_t * buffer, uint32_t length ) const;
    /**
     * \brief   Searches for binary data in the file starting from the specified position.
     *
     * \param   startPos    The position in the file to start searching.
     * \param   buffer      The ByteBuffer containing data to search for.
     * \return  Returns the position where the data starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     * \note    ByteBuffer overload.
     **/
    uint32_t search_data( uint32_t startPos, const ByteBuffer & buffer ) const;

    /**
     * \brief   Searches for null-terminated text in the file starting from the specified position.
     *
     * \param   startPos            The position in the file to start searching.
     * \param   text                The null-terminated text to search for.
     * \param   caseSensitive       If true, the search is case-sensitive.
     * \return  Returns the position where the text starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     **/
    uint32_t search_text( uint32_t startPos, const char * text, bool caseSensitive ) const;
    /**
     * \brief   Searches for null-terminated wide-char text in the file starting from the specified
     *          position.
     *
     * \param   startPos            The position in the file to start searching.
     * \param   text                The null-terminated wide-char text to search for.
     * \param   caseSensitive       If true, the search is case-sensitive.
     * \return  Returns the position where the text starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     * \note    Wide char overload.
     **/
    uint32_t search_text( uint32_t startPos, const wchar_t * text, bool caseSensitive ) const;
    /**
     * \brief   Searches for text in the file starting from the specified position.
     *
     * \param   startPos            The position in the file to start searching.
     * \param   text                The text to search for.
     * \param   caseSensitive       If true, the search is case-sensitive.
     * \return  Returns the position where the text starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     * \note    String overload.
     **/
    uint32_t search_text( uint32_t startPos, const String & text, bool caseSensitive ) const;
    /**
     * \brief   Searches for wide-char text in the file starting from the specified position.
     *
     * \param   startPos            The position in the file to start searching.
     * \param   text                The wide-char text to search for.
     * \param   caseSensitive       If true, the search is case-sensitive.
     * \return  Returns the position where the text starts if found; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     * \note    WideString overload.
     **/
    uint32_t search_text( uint32_t startPos, const WideString & text, bool caseSensitive ) const;

//////////////////////////////////////////////////////////////////////////
// Override methods
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// FileBase class overrides
/************************************************************************/
    /**
     * \brief   Opens the file with previously configured name and mode.
     *
     * \return  Returns true if the file was opened successfully.
     **/
    virtual bool open() = 0;

    /**
     * \brief   Opens the file with the specified name (or path) and mode. Supports file names,
     *          relative paths, full paths, and directory names.
     *
     * \param   fileName    The name or path (relative or full) of the file or directory. Can be
     *                      empty for memory-buffered files.
     * \param   mode        The bitwise combination of open mode flags. Conflicting bits are removed
     *                      automatically.
     * \return  Returns true if the file was opened successfully.
     **/
    virtual bool open(const String& fileName, uint32_t mode) = 0;

    /**
     * \brief   Closes the file. File handles and buffers are handled based on open mode (Attach,
     *          Detach, etc.).
     **/
    virtual void close() = 0;

    /**
     * \brief   Removes the opened file. Forces removal even if the buffer is attached.
     *
     * \return  Returns true if the operation succeeded.
     **/
    virtual bool remove() = 0;

    /**
     * \brief   Returns the current valid length of file data; INVALID_SIZE on failure.
     **/
    virtual uint32_t length() const = 0;

    /**
     * \brief   Returns true if the file is currently open.
     **/
    virtual bool is_opened() const = 0;

    /**
     * \brief   Reserves or resizes the file to the specified size and returns the current pointer
     *          position.
     *
     * \param   newSize     The new size in bytes.
     * \return  Returns the current pointer position on success; otherwise
     *          Cursor::INVALID_CURSOR_POSITION.
     **/
    virtual uint32_t reserve(uint32_t newSize) = 0;

    /**
     * \brief   Purges all file data, sets size to zero, and returns true on success.
     **/
    virtual bool truncate() = 0;

/************************************************************************/
// IOStream pure virtual
/************************************************************************/

    /**
     * \brief   Reads data from the file into the buffer and returns the number of bytes read.
     *
     * \param[out] buffer      On output, contains the data read from the file.
     * \param   size        The size in bytes of the available buffer.
     * \return  Returns the number of bytes read.
     **/
    virtual uint32_t read( uint8_t * buffer, uint32_t size ) const override = 0;

    /**
     * \brief   Writes data from the buffer to the file and returns the number of bytes written.
     *
     * \param   buffer      The buffer containing data to write.
     * \param   size        The size in bytes of the data to write.
     * \return  Returns the number of bytes written.
     **/
    virtual uint32_t write( const uint8_t* buffer, uint32_t size ) override = 0;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from the file into the ByteBuffer and returns the number of bytes read.
     *
     * \param[out] buffer      On output, contains the data read from the file.
     * \return  Returns the number of bytes read.
     * \note    ByteBuffer overload.
     **/
    uint32_t read( ByteBuffer & buffer ) const override;

    /**
     * \brief   Reads string data from the file into the ASCII String and returns the number of
     *          bytes read.
     *
     * \param[out] ascii       On output, contains the string data read from the file.
     * \return  Returns the number of bytes read.
     * \note    String overload.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads string data from the file into the WideString and returns the number of bytes
     *          read.
     *
     * \param[out] wide    On output, contains the wide-char string data read from the file.
     * \return  Returns the number of bytes read.
     * \note    WideString overload.
     **/
    uint32_t read( WideString & wide ) const override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief   Writes binary data from ByteBuffer to the file and returns the number of bytes
     *          written.
     *
     * \param   buffer      The ByteBuffer containing data to write.
     * \return  Returns the number of bytes written.
     * \note    ByteBuffer overload.
     **/
    uint32_t write( const ByteBuffer & buffer ) override;

    /**
     * \brief   Writes string data from the String to the file and returns the number of bytes
     *          written.
     *
     * \param   ascii       The String containing data to write.
     * \return  Returns the number of bytes written.
     * \note    String overload.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes wide-char string data from the WideString to the file and returns the number
     *          of bytes written.
     *
     * \param   wide    The WideString containing data to write.
     * \return  Returns the number of bytes written.
     * \note    WideString overload.
     **/
    uint32_t write( const WideString & wide ) override;

    /**
     * \brief   Flushes all buffered data to the file.
     **/
    void flush() override;

protected:

    /**
     * \brief   Validates and normalizes file open mode flags, resolving conflicting bits.
     *
     * \param   mode    The bitwise combination of OpenMode flags.
     * \return  Returns the normalized mode value.
     **/
    virtual uint32_t normalize_mode(uint32_t mode) const;

/************************************************************************/
// OutStream overrides
/************************************************************************/
    /**
     * \brief   Resets the cursor pointer to the beginning of file data.
     **/
    void reset() const override;

    /**
     * \brief   Normalizes the file name by replacing special masks (e.g., timestamp, process name).
     *
     * \param[in,out] name    On output, contains the normalized file name.
     **/
    static void normalize_name( String & name );

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
    /**
     * \brief   Returns a reference to this object for use with stream write converters.
     **/
    inline FileBase & self();
    /**
     * \brief   Returns a const reference to this object for use with stream write converters.
     * \note    Const overload.
     **/
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

inline const String & FileBase::name() const
{
    return mFileName;
}

inline uint32_t FileBase::mode() const
{
    return mFileMode;
}

inline bool FileBase::is_valid() const
{
    return is_opened();
}

inline bool FileBase::is_force_delete() const
{
    return (mode() & static_cast<uint32_t>(OpenFlag::BitDelete)) != 0;
}

inline bool FileBase::is_temporary() const
{
    return (mode() & static_cast<uint32_t>(OpenFlag::BitTemp)) != 0;
}

inline bool FileBase::is_text_mode() const
{
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitText)) != 0 );
}

inline bool FileBase::is_binary_mode() const
{
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitBinary)) != 0 );
}

inline bool FileBase::is_shared_read() const
{
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitShareRead)) != 0 );
}

inline bool FileBase::is_shared_write() const
{
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitShareWrite)) != 0 );
}

inline bool FileBase::is_eof() const
{
    ASSERT(is_opened());
    return ( position() < length() );
}

inline bool FileBase::can_write() const
{
    ASSERT(is_opened());
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitWrite)) != 0 );
}

inline bool FileBase::can_read() const
{
    ASSERT(is_opened());
    return ( (mode() & static_cast<uint32_t>(OpenFlag::BitRead)) != 0 );
}

inline bool FileBase::move_to_begin() const
{
    ASSERT(is_opened());
    return Cursor::move_to_begin();
}

inline bool FileBase::move_to_end() const
{
    ASSERT(is_opened());
    return Cursor::move_to_end();
}

inline uint32_t FileBase::start_position()
{
    return Cursor::START_CURSOR_POSITION;
}

inline uint32_t FileBase::invalid_position()
{
    return Cursor::INVALID_CURSOR_POSITION;
}

inline const InStream& FileBase::read_stream() const
{
    ASSERT(is_opened());
    return static_cast<const InStream &>(*this);
}

inline OutStream& FileBase::write_stream()
{
    ASSERT(is_opened());
    return static_cast<OutStream &>(*this);
}

inline bool FileBase::read_bool(bool & Value) const
{
    return mReadConvert.as_bool(Value);
}

inline bool FileBase::read_char( char & Value ) const
{
    return mReadConvert.as_char(Value);
}

inline bool FileBase::read_char( wchar_t & Value ) const
{
    return mReadConvert.as_char(Value);
}

inline bool FileBase::read_short( int16_t & Value ) const
{
    return mReadConvert.as_short(Value);
}

inline bool FileBase::read_int( int32_t & Value ) const
{
    return mReadConvert.as_int(Value);
}

inline bool FileBase::read_large( int64_t & Value ) const
{
    return mReadConvert.int64(Value);
}

inline bool FileBase::read_float(float & Value) const
{
    return mReadConvert.as_float(Value);
}

inline bool FileBase::read_double(double & Value) const
{
    return mReadConvert.as_double(Value);
}

inline bool FileBase::write_bool( bool inValue )
{
    return mWriteConvert.set_bool(inValue);
}

inline bool FileBase::write_char( char inValue )
{
    return mWriteConvert.set_char(inValue);
}

inline bool FileBase::write_char( wchar_t inValue )
{
    return mWriteConvert.set_char(inValue);
}

inline bool FileBase::write_short( int16_t inValue )
{
    return mWriteConvert.set_short(inValue);
}

inline bool FileBase::write_int( int32_t inValue )
{
    return mWriteConvert.set_int(inValue);
}

inline bool FileBase::write_large( int64_t inValue )
{
    return mWriteConvert.set_int64(inValue);
}

inline bool FileBase::write_float( float inValue )
{
    return mWriteConvert.set_float(inValue);
}

inline bool FileBase::write_double( double inValue )
{
    return mWriteConvert.set_double(inValue);
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

} // namespace areg
#endif  // AREG_BASE_FILEBASE_HPP
