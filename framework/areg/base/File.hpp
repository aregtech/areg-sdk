#ifndef AREG_BASE_FILE_HPP
#define AREG_BASE_FILE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/File.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, File class to work with files on File System.7
 *              Can be also used as an object for data streaming.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/FileBase.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Containers.hpp"

#include <string_view>
#include <filesystem>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// File class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   File class to work with files on File System. Supports data streaming functionalities.
 *          Represents a file object supporting sequential reading and writing operations, with
 *          streaming interfaces. Handles both file system files and memory-buffered file objects.
 **/
class AREG_API File : public FileBase
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Special folder definitions
     **/
    enum class SpecialFolder    : uint8_t
    {
          UserHome  = 0 //!< Current user home folder path.
        , Personal      //!< The user's document folder path.
        , AppData       //!< Application data folder path.
        , Temp          //!< USER or System temp folder path.
    };

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   File::SPEACIAL_MASKS
     *          The list of masked names to replace with special folders.
     */
    static constexpr std::string_view   SPEACIAL_MASKS[]    =   { "%home%", "%personal", "%appdata%", "%temp%" };

    /**
     * \brief   The length of the special file name masks array.
     **/
    static constexpr int32_t            LEN_SPECIAL_MASKS       { std::size(SPEACIAL_MASKS) };

    /**
     * \brief   File::TEMP_FILE_PREFIX
     *          The default prefix for temporary file. By default it is "_areg"
     **/
    static constexpr std::string_view   TEMP_FILE_PREFIX    { "_areg" };

    /**
     * \brief   Current directory
     **/
    static constexpr std::string_view   DIR_CURRENT         { "." };

    /**
     * \brief   Parent directory
     **/
    static constexpr std::string_view   DIR_PARENT          { ".." };

    /**
     * \brief   File::EXTENSION_SEPARATOR
     *          File extension separator.
     **/
    static constexpr char               EXTENSION_SEPARATOR { '.' };

    /**
     * \brief   File::UNIX_SEPARATOR
     *          Unix style path separator.
     **/
    static constexpr char               UNIX_SEPARATOR      { '/' };

    /**
     * \brief   File::DOS_SEPARATOR
     *          DOS style path separator.
     **/
    static constexpr char               DOS_SEPARATOR       { '\\' };

    /**
     * \brief   File::MAXIMUM_PATH
     *          The maximum path of file.
     **/
    static constexpr int32_t            MAXIMUM_PATH        {1024};

    /**
     * \brief   File::PATH_SEPARATOR
     *          The OS dependent path separator. On POSIX it is '/' and on Windows it is '\\'.
     **/
    static constexpr char               PATH_SEPARATOR      { std::filesystem::path::preferred_separator };

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    File();

    /**
     * \brief   Initializes a file object with the given path and mode; does not open the file.
     *
     * \param   fileName    Relative or absolute file path.
     * \param   mode        Bitwise OR combination of OpenMode and OpenFlag values.
     **/
    explicit File(const String& fileName, uint32_t mode = (static_cast<uint32_t>(OpenMode::OpenAlways) | static_cast<uint32_t>(OpenMode::Binary)));

    virtual ~File();


//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// FileBase class overrides
/************************************************************************/
    /**
     * \brief   Opens the file using previously set name and mode. Returns false if already open or
     *          parameters are invalid.
     **/
    bool open() override;

    /**
     * \brief   Opens a file or directory with the specified path and mode. Returns false if already
     *          open or path is invalid.
     *
     * \param   fileName    File path (name only, relative, or absolute) or directory path.
     * \param   mode        Bitwise OR combination of OpenMode and OpenFlag values; conflicting bits
     *                      are resolved.
     **/
    bool open(const String& fileName, uint32_t mode) override;

    /**
     * \brief   Closes the file. Behavior depends on open mode flags
     *          (Attach/Detach/CreateTemp/FO_FOR_DELETE).
     **/
    void close() override;

    /**
     * \brief   Deletes the file from the file system, even if opened in Attach mode.
     **/
    bool remove() override;

    /**
     * \brief   Moves the file pointer by offset bytes from the given origin and returns the new
     *          position, or INVALID_CURSOR_POSITION on failure.
     *
     * \param   offset      Byte offset to move. Positive moves forward; negative moves backward.
     * \param   startAt     Origin for seek: Begin (file start), Current (current position), or End
     *                      (file end).
     **/
    uint32_t set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept override;

    /**
     * \brief   Returns the current file pointer position in bytes, or INVALID_CURSOR_POSITION if
     *          the file is not open.
     **/
    [[nodiscard]]
    uint32_t position() const noexcept override;

    /**
     * \brief   Returns the current size of file data in bytes, or INVALID_SIZE if not open.
     **/
    [[nodiscard]]
    uint32_t length() const noexcept override;

    /**
     * \brief   Returns true if the file is currently open.
     **/
    [[nodiscard]]
    bool is_opened() const noexcept override;

    /**
     * \brief   Reserves or sets file size and returns the current pointer position, or
     *          INVALID_CURSOR_POSITION on failure.
     *
     * \param   newSize     New size in bytes; zero sets size to zero. Larger sizes preserve data;
     *                      smaller sizes truncate.
     **/
    uint32_t reserve(uint32_t newSize) override;

    /**
     * \brief   Clears all data and sets file size to zero.
     **/
    bool truncate() override;

/************************************************************************/
// InStream interface overrides
/************************************************************************/

    /**
     * \brief   Reads data from the file into the buffer and returns the number of bytes read.
     *
     * \param[in,out] buffer      The buffer to receive file data.
     **/
    uint32_t read( SharedBuffer & buffer ) const override;

    /**
     * \brief   Reads text data from the file into an ASCII string and returns the number of bytes read.
     *
     * \param[in,out] ascii       The ASCII string to receive file data.
     **/
    uint32_t read( String & ascii ) const override;

    /**
     * \brief   Reads text data from the file into a wide-character string and returns the number of bytes read.
     *
     * \param[in,out] wide      The wide-character string to receive file data.
     **/
    uint32_t read( WideString & wide ) const override;

    /**
     * \brief   Reads up to size bytes from the file into the buffer and returns the number of bytes read.
     *
     * \param[in,out] buffer    The byte buffer to receive data.
     * \param   size            The capacity of the buffer in bytes.
     **/
    uint32_t read( uint8_t * buffer, uint32_t size ) const noexcept override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief   Writes binary data from the buffer to the file and returns the number of bytes written.
     *
     * \param   buffer      The buffer containing data to write.
     **/
    uint32_t write( const SharedBuffer& buffer ) override;

    /**
     * \brief   Writes ASCII string data to the file and returns the number of bytes written.
     *
     * \param   ascii       The ASCII string to write.
     **/
    uint32_t write( const String & ascii ) override;

    /**
     * \brief   Writes wide-character string data to the file and returns the number of bytes written.
     *
     * \param   wide    The wide-character string to write.
     **/
    uint32_t write( const WideString & wide ) override;

    /**
     * \brief   Writes size bytes from the buffer to the file and returns the number of bytes written.
     *
     * \param   buffer      The byte buffer containing data to write.
     * \param   size        The number of bytes to write.
     **/
    uint32_t write( const uint8_t* buffer, uint32_t size ) noexcept override;

    /**
     * \brief   Flushes buffered file data to the file system.
     **/
    void flush() noexcept override;

    /**
     * \brief   Reserve and ensure additional size to the existing. If the free space of the stream to write
     *          is enough, no changes should be done. If the size of the stream is not enough, it should allocate
     *          additional space and should not loose existing data.
     * \param   addSize     The size to add if required.
     * \return  Returns true if the stream has enough space to write the data.
     **/
    bool ensure_size(uint32_t addSize) override;

protected:
/************************************************************************/
// InStream interface overrides
/************************************************************************/
    /**
     * \brief   Returns the number of unread bytes remaining from the current position to the end.
     **/
    [[nodiscard]]
    uint32_t size_readable() const noexcept override;

/************************************************************************/
// OutStream interface overrides
/************************************************************************/
    /**
     * \brief   Returns the number of bytes that can be written from the current position.
     **/
    [[nodiscard]]
    uint32_t size_writable() const noexcept override;

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the absolute path of the given file or directory; if nullptr, returns the
     *          current directory.
     *
     * \param   filePath    Absolute or relative file/directory path, or nullptr for current
     *                      directory.
     **/
    [[nodiscard]]
    static String file_full_path(const String& filePath);

    /**
     * \brief   Returns the directory portion of the path including the trailing separator.
     *
     * \param   filePath    Absolute or relative file path.
     **/
    [[nodiscard]]
    static String file_directory(const String& filePath);

    /**
     * \brief   Returns the file name with extension; empty for a directory path.
     *
     * \param   filePath    Absolute or relative file path.
     **/
    [[nodiscard]]
    static String name_with_extension(const String& filePath);

    /**
     * \brief   Returns only the file extension; empty for a directory path.
     *
     * \param   filePath    Absolute or relative file path.
     **/
    [[nodiscard]]
    static String file_extension(const String& filePath);

    /**
     * \brief   Returns the file name without extension; empty for a directory path.
     *
     * \param   filePath    Absolute or relative file path.
     **/
    [[nodiscard]]
    static String file_name(const String& filePath);

    /**
     * \brief   Deletes the file at the given path; returns true if successful.
     *
     * \param   filePath    Absolute or relative path to the file to delete.
     **/
    static bool delete_file(const String& filePath);

    /**
     * \brief   Moves a file or directory from oldPath to newPath; returns true if successful.
     *
     * \param   oldPath     Original absolute or relative path.
     * \param   newPath     New absolute or relative path.
     **/
    static bool move_file(const String& oldPath, const String& newPath);

    /**
     * \brief   Copies a file from srcPath to newPath; returns true if successful.
     *
     * \param   srcPath         Relative or absolute path of the source file.
     * \param   newPath         Relative or absolute destination path.
     * \param   copyForce       If true, overwrites existing file at destination; if false, fails if
     *                          destination exists.
     **/
    static bool copy_file(const String& srcPath, const String& newPath, bool copyForce);

    /**
     * \brief   Returns true if a file exists at the given path.
     *
     * \param   filePath    Relative or absolute path to check.
     **/
    [[nodiscard]]
    static bool has_file(const String& filePath);

    /**
     * \brief   Generates a temporary file name with the given prefix; returns absolute path if
     *          inTempFolder is true, otherwise relative.
     *
     * \param   prefix          File name prefix; if nullptr uses TEMP_FILE_PREFIX ("_cz").
     * \param   unique          If true, generates a system-guaranteed unique name; if false, uses
     *                          tick count (non-unique).
     * \param   inTempFolder    If true, places file in system temp folder; if false, places in
     *                          current folder (relative path).
     **/
    [[nodiscard]]
    static String temp_name(const String& prefix, bool unique, bool inTempFolder);
    /**
     * \brief   Generates a unique temporary file name in the system temp folder with default prefix.
     **/
    [[nodiscard]]
    static String temp_name();

    /**
     * \brief   Returns the absolute path of the current working directory.
     **/
    [[nodiscard]]
    static String current_dir();

    /**
     * \brief   Sets the current working directory; returns true if successful.
     *
     * \param   dirPath     Absolute or relative path to the new current directory.
     **/
    static bool set_current_dir(const String& dirPath);

    /**
     * \brief   Creates a directory at the given path; returns true if successful.
     *
     * \param   dirPath     Absolute or relative path of the directory to create.
     **/
    static bool create_dir(const String& dirPath);

    /**
     * \brief   Deletes the directory at the given path; returns true if successful.
     *
     * \param   dirPath     Absolute or relative path of the directory to delete.
     **/
    static bool delete_dir(const String& dirPath);

    /**
     * \brief   Returns the absolute path of the system temporary directory.
     **/
    [[nodiscard]]
    static String temp_dir();

    /**
     * \brief   Returns true if a directory exists at the given path.
     *
     * \param   dirPath     Relative or absolute path to check.
     **/
    [[nodiscard]]
    static bool has_dir(const String& dirPath);

    /**
     * \brief   Normalizes path by resolving . and .. symbols, expanding %time% placeholders, and
     *          resolving special folder masks.
     *
     * \param   fileName    File path to normalize, may include . .. and special masks.
     **/
    [[nodiscard]]
    static String normalize_path( const String& fileName );

    /**
     * \brief   Creates nested directories as needed; returns true if successful or directories already exist.
     *
     * \param   dirPath     Absolute or relative path of nested directories to create.
     **/
    static bool create_dir_cascaded(const String& dirPath);

    /**
     * \brief   Returns the directory containing the currently running executable.
     **/
    [[nodiscard]]
    static const String & executable_dir() noexcept;

    /**
     * \brief   Returns the path of a system special directory; returns empty string if not available.
     *
     * \param   specialFolder       Enumeration specifying the type of special folder to retrieve.
     **/
    [[nodiscard]]
    static String special_dir(const File::SpecialFolder specialFolder);

    /**
     * \brief   Returns the parent directory path of the given file or directory; returns empty
     *          string if not found.
     *
     * \param   filePath    Path of file or directory whose parent to retrieve.
     **/
    [[nodiscard]]
    static String parent_dir( const String& filePath );

    /**
     * \brief   Finds the parent directory name within the path; returns true if found.
     *
     * \param   filePath    Path to search for parent directory name.
     * \param[out] nextPos  On return, if not nullptr, points to the end of the parent  name in the string.
     * \param   lastPos     If not nullptr, search begins at this position; if nullptr, searches from the end.
     **/
    static bool find_parent( const char * filePath, const char ** nextPos, const char * lastPos = nullptr ) noexcept;

    /**
     * \brief   Splits the file path into components and appends them to the list; returns the
     *          number of components added.
     *
     * \param   filePath            File path to split (full, relative, or with special names).
     * \param[in,out] in_out_List   List to receive path components (without separators);
     *                              components are appended, not replaced.
     **/
    static int32_t split_path(const String& filePath, StringList & in_out_List);

    /**
     * \brief   Constructs a full file path from directory and file name components.
     *
     * \param   dirName     Full or relative directory path (may contain masks).
     * \param   fileName    File name (may contain masks).
     **/
    [[nodiscard]]
    static String make_full_path(const String& dirName, const String& fileName);

//////////////////////////////////////////////////////////////////////////
// OS specific methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS-specific implementation to close the file and free resources.
     **/
    void _os_close_file() noexcept;
    
    /**
     * \brief   OS-specific implementation to open the file for reading and/or writing; returns true if successful.
     **/
    bool _os_open_file() noexcept;

    /**
     * \brief   OS-specific implementation to read bytes from the file; returns the number of bytes read.
     *
     * \param[out] buffer   Buffer to receive data.
     * \param   size        Capacity of the buffer in bytes.
     **/
    uint32_t _os_read_file(uint8_t* buffer, uint32_t size) const noexcept;

    /**
     * \brief   OS-specific implementation to write bytes to the file; returns the number of bytes written.
     *
     * \param   buffer      Buffer containing data to write.
     * \param   size        Number of bytes to write.
     **/
    uint32_t _os_write_file( const uint8_t* buffer, uint32_t size ) noexcept;

    /**
     * \brief   OS-specific implementation to move the file pointer; returns new position or
     *          INVALID_CURSOR_POSITION on failure.
     *
     * \param   offset      Byte offset; positive moves forward, negative moves backward.
     * \param   startAt     Origin for seek: Begin, Current, or End.
     **/
    uint32_t _os_set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept;

    /**
     * \brief   OS-specific implementation returning the current file pointer position, or
     *          INVALID_CURSOR_POSITION if not open.
     **/
    uint32_t _os_file_position() const noexcept;

    /**
     * \brief   OS-specific implementation to truncate the file at the current pointer position;
     *          returns true if successful.
     **/
    bool _os_truncate_file() noexcept;

    /**
     * \brief   OS-specific implementation to resize the file to the specified size using a
     *          descriptor-based call, without closing or reopening the file.
     *          Extends the file with zero bytes when growing; truncates data when shrinking.
     *          Does not modify the file pointer position.
     *
     * \param   newSize     The desired file size in bytes.
     * \return  Returns true if the file was successfully resized.
     **/
    bool _os_reserve(uint32_t newSize) noexcept;

    /**
     * \brief   OS-specific implementation to flush buffered file data to the file system.
     **/
    void _os_flush_file() noexcept;

    /**
     * \brief   OS-specific implementation to return the number of bytes that can be read from the
     *          current position to the end of file; returns 0 if not open.
     **/
    [[nodiscard]]
    uint32_t _os_file_length() const noexcept;

    /**
     * \brief   OS-specific implementation to generate a temporary file name; returns the length of
     *          the name written to buffer.
     *
     * \param[out] buffer   Buffer to receive the generated temp file name (max
     *                      File::MAXIMUM_PATH chars including null terminator).
     * \param   folder      Directory path where temp file should be located.
     * \param   prefix      Prefix string to prepend to the generated file name.
     * \param   unique      If non-zero, generates a system-guaranteed unique name.
     **/
    static uint32_t _os_temp_name(char* buffer, const char* folder, const char * prefix, uint32_t unique) noexcept;

    /**
     * \brief   OS-specific implementation to retrieve the path of a special system directory;
     *          returns the path length.
     *
     * \param[out]  buffer          Buffer to receive the directory path.
     * \param       length          Capacity of the buffer in bytes.
     * \param       specialFolder   Enumeration specifying the type of special directory.
     **/
    static uint32_t _os_special_dir(char* buffer, uint32_t length, const File::SpecialFolder specialFolder) noexcept;

    /**
     * \brief   Returns the OS-specific invalid file handle value.
     **/
    [[nodiscard]]
    static FILEHANDLE _os_invalid_handle() noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   File System object handle.
     **/
    mutable FILEHANDLE      mFileHandle;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden methods
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( File );
};

} // namespace areg
#endif  // AREG_BASE_FILE_HPP
