#ifndef AREG_BASE_FILE_HPP
#define AREG_BASE_FILE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/File.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File class to work with files on File System.7
 *              Can be also used as an object for data streaming.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/FileBase.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Containers.hpp"

#include <string_view>
#include <filesystem>


/**
 * \brief   File class to work with files on File System. Supports data streaming
 *          functionalities.
 **/
//////////////////////////////////////////////////////////////////////////
// File class declaration
//////////////////////////////////////////////////////////////////////////
class AREG_API File : public FileBase
{
//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Special folder definitions
     **/
    typedef enum class E_SpecialFolder : uint8_t
    {
          SpecialUserHome       //!< Current user home folder path.
        , SpecialPersonal       //!< The user's document folder path.
        , SpecialAppData        //!< Application data folder path.
        , SpecialTemp           //!< USER or System temp folder path.

    } eSpecialFolder;

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   File::SPEACIAL_MASKS
     *          The list of masked names to replace with special folders.
     */
    static constexpr std::string_view   SPEACIAL_MASKS[]    = { "%home%", "%personal", "%appdata%", "%temp%" };

    /**
     * \brief   File::LEN_SPECIAL_MASKS
     *          The special masked names array length.
     */
    static constexpr int                LEN_SPECIAL_MASKS   { MACRO_ARRAYLEN(SPEACIAL_MASKS) };

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
    static constexpr int                MAXIMUM_PATH        {1024};

    /**
     * \brief   File::PATH_SEPARATOR
     *          The OS dependent path separator. On POSIX it is '/' and on Windows it is '\\'.
     **/
    static constexpr char               PATH_SEPARATOR      { std::filesystem::path::preferred_separator };

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(default: 4251)
#endif  // _MSC_VER

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor
     **/
    File( void );

    /**
     * \brief	Constructor to set file name / path and file open mode
     * \param	fileName	relative or absolute file path
     * \param	mode	    file open mode. 
     *                      For modes, see description in FileBase class 
     **/
    explicit File(const String& fileName, unsigned int mode = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY));

    /**
     * \brief   Destructor
     **/
    virtual ~File( void );


//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
public:
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
     **/
    virtual bool open( void ) override;

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
     **/
    virtual bool open(const String& fileName, unsigned int mode) override;

    /**
     * \brief   Call to close file object.
     *          If file was opened in FO_MODE_ATTACH or FO_MODE_DETACH modes, on close the file object will not be deleted
     *          except if mode is combined with values FO_FOR_DELETE or FO_MODE_CREATE_TEMP. Attach and Detach modes are
     *          valid and meaningful only for memory buffered file object. It has no meaning for File System file object.
     *          If FO_MODE_CREATE_TEMP is set, file object is always deleted on close.
     *          If FO_FOR_DELETE is set, file object is deleted only for memory buffered file even if file was opened with attach mode.
     **/
    virtual void close( void ) override;

    /**
     * \brief	Delete opened file. This will force to delete file object even if it is attached memory buffered file
     * \return	Returns true if succeeded.
     **/
    virtual bool remove( void ) override;

    /**
     * \brief	Sets the file pointer position and returns current position. 
     *          The positive value of offset means move pointer forward.
     *          The negative value of offset means move pointer back.
     *          For memory buffered file the pointer cannot move more than IECursorPosition::eCursorPosition::PositionEnd.
     *
     * \param	offset	The offset in bytes to move. Positive value means moving forward. Negative value means moving back.
     * \param	startAt	Specifies the starting position of pointer and should have one of values:
     *                  IECursorPosition::eCursorPosition::PositionBegin   -- position from beginning of file
     *                  IECursorPosition::eCursorPosition::PositionCurrent -- from current pointer position
     *                  IECursorPosition::eCursorPosition::PositionEnd     -- from end of file
     *
     * \return	If succeeds, returns the current position of pointer in bytes or value IECursorPosition::INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int setPosition(int offset, IECursorPosition::eCursorPosition startAt) const override;

    /**
     * \brief	If succeeds, returns the current position of pointer in bytes or value IECursorPosition::INVALID_CURSOR_POSITION if fails.
     *          Before calling function, the file object should be opened.
     * \return	If succeeds, returns the current position of pointer in bytes or value IECursorPosition::INVALID_CURSOR_POSITION if fails.
     **/
    virtual unsigned int getPosition( void ) const override;

    /**
     * \brief	If succeeds, returns the current valid length of file data. otherwise returns INVALID_SIZE value.
     **/
    virtual unsigned int getLength( void ) const override;

    /**
     * \brief   Returns the current open status of file object. If file is opened, returns true
     **/
    virtual bool isOpened() const override;

    /**
     * \brief	Call to reserve space or set new size of file object and returns the current position of pointer.
     *          If new size is positive value, the size will be increased. Otherwise the reserved size will set size zero.
     *          If new size is more than the current size of file object, no data will be lost, no pointer position will be changed.
     *          If new size is less than the current size of file object, data will be truncated until the new size, 
     *          and if the new size is less than the current pointer position, the pointer will be move at the end of file.
     * 
     * \param	newSize	New Size is bytes to reserve or set.
     *
     * \return  If succeeds, returns the current position of file pointer. Otherwise it returns value IECursorPosition::INVALID_CURSOR_POSITION.
     **/
    virtual unsigned int reserve(unsigned int newSize) override;

    /**
     * \brief   Purge file object data, sets the size zero and if succeeds, return true
     **/
    virtual bool truncate( void ) override;

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
     * \param   ascii     The buffer of ASCII String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( String & ascii ) const override;

    /**
     * \brief   Reads string data from Input Stream object and copies into given Wide String.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wide      The buffer of Wide String to stream data from Input Stream object.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int read( WideString & wide ) const override;

    /**
     * \brief	Reads data from input stream object, copies into given buffer and
     *          returns the size of copied data
     * \param	buffer	The pointer to buffer to copy data from input object
     * \param	size	The size in bytes of available buffer
     * \return	Returns the size in bytes of copied data
     **/
    virtual unsigned int read( unsigned char * buffer, unsigned int size ) const override;

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
     * \param   ascii     The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const String & ascii ) override;

    /**
     * \brief   Writes string data from given wide-char String object to output stream object.
     *          Overwrite method if need to change behavior of streaming string.
     * \param   wide  The buffer of String containing data to stream to Output Stream.
     * \return  Returns the size in bytes of copied string data.
     **/
    virtual unsigned int write( const WideString & wide ) override;

    /**
     * \brief	Write data to output stream object from given buffer
     *          and returns the size of written data. In this class 
     *          writes data into opened file.
     * \param	buffer	The pointer to buffer to read data and 
     *          copy to output stream object
     * \param	size	The size in bytes of data buffer
     * \return	Returns the size in bytes of written data
     **/
    virtual unsigned int write( const unsigned char* buffer, unsigned int size ) override;

    /**
     * \brief   Clears the buffers for the file and causes all buffered data 
     *          to be written to the file.
     **/
    virtual void flush( void ) override;

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
    virtual unsigned int getSizeReadable( void ) const override;

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
    virtual unsigned int getSizeWritable( void ) const override;

//////////////////////////////////////////////////////////////////////////
// Static operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Returns the absolute path to file / folder.
     * \param	filePath	Absolute or relative file / folder name.
     **/
    static String getFileFullPath(const char * filePath = nullptr);

    /**
     * \brief   Returns the full path of directory part of file path, including backslash
     * \param   filePath    Absolute or relative file path
     **/
    static String getFileDirectory(const char * filePath);

    /**
     * \brief	Returns file name with extension of the given file path.
     *          If path is a folder, the file name is empty.
     * \param	filePath	Absolute or relative file path
     **/
    static String getFileNameWithExtension(const char * filePath);

    /**
     * \brief   Returns only file extension of the given file path.
     *          If path is a folder, the file name is empty.
     * \param   filePath    Absolute or relative file path
     **/
    static String getFileExtension(const char * filePath);

    /**
     * \brief   Returns only file name without extension of the given file path.
     *          If path is a folder, the file name is empty.
     * \param   filePath    Absolute or relative file path
     **/
    static String getFileName(const char * filePath);

    /**
     * \brief	Delete given file. Returns true if succeeds.
     * \param	filePath	Absolute or relative file path to delete.
     **/
    static bool deleteFile(const char * filePath);

    /**
     * \brief	Move file or folder from old location to new location
     * \param	oldPath	    Old absolute or relative path of file or folder
     * \param	newPath	    New absolute or relative path of file or folder
     **/
    static bool moveFile(const char * oldPath, const char * newPath);

    /**
     * \brief	Copies file from old location to new location. File in original location will remain unchanged
     * \param	srcPath	    Relative or absolute path of source file.
     * \param	newPath	    Relative or absolute path to new location to copy.
     * \param	copyForce	If new location already contains file:
     *                          - if true, file will be overwritten
     *                          - if false, operation will fail
     * \return	Returns true if operation succeeds.
     **/
    static bool copyFile(const char * srcPath, const char * newPath, bool copyForce);

    /**
     * \brief	Checks whether the given path is an existing file or not.
     * \param	filePath    The relative or absolute path of file to check
     * \return	If file exists, returns true. Otherwise returns false
     **/
    static bool existFile(const char * filePath);

    /**
     * \brief	Generates temp file name in current or in temporary folder
     * \param	prefix	        The prefix that should contain temp file. If nullptr TEMP_FILE_PREFIX (i.e. "_cz") will be used
     * \param	unique	        If true, the system will generate own unique name. 
     *                          Otherwise it will use tick count and will not guaranty uniqueness.
     * \param	inTempFolder	If true, generated file path will be in system defined temp folder, otherwise in current folder. 
     *                          File path in current folder will have relative path
     * \return	Absolute or relative path to temp file.
     **/
    static String genTempFileName(const char * prefix, bool unique, bool inTempFolder);
    /**
     * \brief   Generate unique temp file name in temporary folder with prefix TEMP_FILE_PREFIX ("_cz")
     **/
    static String genTempFileName( void );

    /**
     * \brief	Returns absolute path of current directory
     **/
    static String getCurrentDir( void );

    /**
     * \brief	Sets current directory. Returns true if succeeds.
     * \param	dirPath	    Absolute or relative path to directory to set as current
     **/
    static bool setCurrentDir(const char * dirPath);

    /**
     * \brief	Creates folder. Returns true if succeeds.
     * \param	dirPath	    Absolute or relative path of directory
     **/
    static bool createDir(const char * dirPath);

    /**
    * \brief	Deletes folder. Returns true if succeeds.
    * \param	dirPath	    Absolute or relative path of directory
     **/
    static bool deleteDir(const char * dirPath);

    /**
     * \brief	Returns full path of system defined temporary folder
     **/
    static String getTempDir( void );

    /**
     * \brief	Checks whether the given path is an existing directory or not.
     * \param	dirPath     The relative or absolute path of directory to check
     * \return	If directory exists, returns true. Otherwise returns false
     **/
    static bool existDir(const char * dirPath);

    /**
     * \brief   1.  Normalizes file path, replaces current and parent folder symbols like "." or "..".
     *          2.  Generates file name setting time-stamp on the name of file 
     *              time-stamp in format yyyy_mm_dd_hh_mm_ss_ms keeping the file extension.
     *              If passed file name has keyword "%time%" it will replace with mentioned
     *              time-stamp format.
     *          3.  Replaces special folder masks such as user home or temp.
     *
     *          Examples:
     *              a. "./dir1/dir2/..\\dir4\\file.dat" ==> converted to "<current dir>/dir1/dir4/file.dat",
     *                  where <current dir> is current working directory.
     *
     *              2.  ".logs/logs_%time-stamp%.dat" ==> converted to "<current dir>/logs_yyyy_mm_dd_hh_mm_ss_ms.dat",
     *                  where <current dir> is current working directory and "yyyy_mm_dd_hh_mm_ss_ms" is a data-time format.
     *
     *              3.  "%personal%/my_file.dat" ==> converted to "<user document dir>/my_file.dat", where
     *                  <user document dir> is a special folder in user profile folder.
     *
     * \param   fileName    The file path and name to normalize.
     * \return  Returns generated unique and normalized file name.
     **/
    static String normalizePath( const char * fileName );

    /**
     * \brief	Create Directories cascaded (nested) and returns true if succeeded or directories were already existing
     *
     * \param	dirPath	    Absolute or relative path to directory (cascaded) that should be created.
     *                      For example: if dirPath is "C:\\dir1\\dir2\\dir3\\" and "C:\\" does not contain such folders,
     *                      It will create all 3 folders.
     *                      Note, if dirPath will be "C:\\folder\\file.dat", it will assume that "file.dat" is a folder name 
     *                      and will try to create directory inside of "C:\\folder\\"
     *
     * \return	Returns true if succeeds.
     **/
    static bool createDirCascaded(const char * dirPath);

    /**
     * \brief   Returns folder location of current executable.
     **/
    static const String & getExecutableDir( void );

    /**
     * \brief   Returns special folder path. The type of required folder is defined in
     *          specialFolder parameter.
     * \param   specialFolder   The type of special folder in the system.
     * \return  If function succeeds, the return value is full path of special folder.
     *          Otherwise, it returns empty string.
     **/
    static String getSpecialDir(const eSpecialFolder specialFolder);

    /**
     * \brief   Returns the parent directory of given path, which can be either file or directory.
     * \param   filePath    Path of file or directory to get the parent directory.
     * \return  If succeeded, the return string contains full path to parent directory.
     **/
    static String getParentDir( const char * filePath );

    /**
     * \brief   Finds the parent directory name in the given file path. On output, if parameter 'nextPos' is not nullptr, 
     *          it will indicate the end of parent folder path in the given string. If parameter 'lastPos' is not nullptr,
     *          the searching in the file path string will start from that position.
     * \param   filePath    The path of file or directory to search parent directory name.
     * \param   nextPos     On output, if it is not nullptr, it indicates the last position of parent directory name.
     *                      Otherwise, the function did not find parent directory.
     * \param   lastPos     If not nullptr, it should indicate the position in the filePath to start to search parent
     *                      directory. If nullptr, the function searches parent directory at the end of string.
     * \return  Returns true if the given path contains the name of parent directory.
     *
     * \example     Extract Parents:
     * 
     *      // This example recursively searches parent directory until reaches the end:
     *      const char * filePath   = "/home/auser/some/path/file.name";
     *      const char * nextPos    = nullptr;
     *      const char * lastPos    = nullptr;
     *      printf("File  : %s\n", filePath);
     *      while( File::findParent(filePath, &nextPos, lastPos) )
     *      {
     *          String str(filePath, MACRO_ELEM_COUNT(filePath, nextPos));
     *          printf("Parent: %s .\n", str.getString());
     *          lastPos = nextPos;
     *      }
     *
     *      Outputs:
     *          File  : /home/auser/some/path/file.name
     *          Parent: /home/auser/some/path/
     *          Parent: /home/auser/some/
     *          Parent: /home/auser/
     *          Parent: /home/
     *          Parent: /
     **/
    static bool findParent( const char * filePath, const char ** nextPos, const char * lastPos = nullptr );

    /**
     * \brief   Splits given file path. Each node is filled in StringList.
     * \param   filePath    The file path to split. It can be full or relative path. 
     *                          It may contain special names.
     * \param   in_out_List     On output, it will be added nodes and they will not contain path separator.
     *                          Note, the function does not clean outList before adding nodes. This means:
     *                              -   make sure to remove all if don't need them
     *                              -   it may contain parent nodes and then split relative path, which nodes 
     *                                  will be added to the list.
     * \return  Returns number of new nodes added to the list.
     **/
    static int splitPath(const char * filePath, StringList & in_out_List);

private:

    /**
     * \brief   Returns true if specified file path starts with current folder,
     *          i.e. starts with "./" or ".\". The path of name will be
     *          considered as relative to the path of executable file location.
     * \param   filePath    File path to check.
     * \param   skipSep     If true, it ignores checking path separator at the end of
     *                      "current directory". Otherwise, the filePath should contain
     *                      the path separator symbol.
     * \return  Returns true if specified file path starts with current folder
     **/
    static inline bool _nameHasCurrentFolder( const char * filePath, bool skipSep );

    /**
     * \brief   Returns true if specified file path starts with current folder,
     *          i.e. starts with "../" or "..\". The path of name will be
     *          considered as relative to the parent folder.
     * \param   filePath    File path to check.
     * \param   skipSep     If true, it ignores checking path separator at the end of
     *                      "parent directory". Otherwise, the filePath should contain
     *                      the path separator symbol.
     * \return  Returns true if specified file path starts with current folder
     **/
    static inline bool _nameHasParentFolder( const char * filePath, bool skipSep );

//////////////////////////////////////////////////////////////////////////
// OS specific methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   OS specific method to close file and free resources.
     */
    void _osCloseFile( void );
    
    /**
     * \brief   OS specific method to open file for reading and/or writing.
     * \return  Returns true if succeeded to open the file.
     */
    bool _osOpenFile( void );

    /**
     * \brief   OS specific method to read data from opened file.
     * 
     * \param   buffer  The buffer to read data.
     * \param   size    The size in bytes of the buffer.
     * \return  Returns the size of data that could read from the file.
     *          Returns zero if failed to read or no more data to read.
     */
    unsigned int _osReadFile(unsigned char* buffer, unsigned int size) const;

    /**
     * \brief   OS specific method to write data into the file. The file should
     *          be opened for writing.
     * 
     * \param   buffer  The buffer to write data.
     * \param   size    The size in bytes of data in the buffer to write.
     * \return  Returns the size of data that could write into the file.
     *          Returns zero if failed to write or no data to write.
     */
    unsigned int _osWriteFile( const unsigned char* buffer, unsigned int size );

    /**
     * \brief   OS specific method to move cursor position in the file.
     * 
     * \param   offset  The offset in bytes to move cursor. Positive value means move
     *                  cursor forward. The negative value means move cursor back.
     * \param   startAt The position to start moving cursor.
     * \return  If succeeded, returns the new position of the cursor. Otherwise, returns
     *          invalid position (IECursorPosition::INVALID_CURSOR_POSITION).
     */
    unsigned int _osSetPositionFile(int offset, IECursorPosition::eCursorPosition startAt) const;

    /**
     * \brief   If file is opened, return the current cursor position in the file.
     *          Otherwise, returns invalid position (IECursorPosition::INVALID_CURSOR_POSITION).
     */
    unsigned int _osGetPositionFile(void) const;

    /**
     * \brief   OS specific method to truncate the opened file until the current position of the cursor.
     *          The file should be opened for the writing.
     * \return  Returns true if operation succeeded. Otherwise, returns false.
     */
    bool _osTruncateFile(void);

    /**
     * \brief   OS specific method to flash file buffer into the file system.
     */
    void _osFlushFile(void);

    /**
     * \brief   OS specific method to generate temporary file name.
     *          On output, the 'buffer' contains the name of the file.
     *          The buffer size is File::MAXIMUM_PATH characters, including
     *          null-terminated character.
     * 
     * \param   buffer  The buffer that will contain the temporary file name.
     *                  Initially, the size of buffer is File::MAXIMUM_PATH.
     * \param   folder  The name of the folder to create temporary file.
     * \param   prefix  The prefix to add in front of the file name.
     * \param   unique  If true, the name of the file is unique.
     * \return  Returns the length of data in the buffer.
     */
    static unsigned int _osCreateTempFileName(char* buffer, const char* folder, const char * prefix, unsigned int unique);

    /**
     * \brief   OS specific method to retrieve the OS specific methods.
     * 
     * \param   buffer          The buffer to write generated file name.
     * \param   length          The length of the buffer to write the name of special file name.
     * \param   specialFolder   THe flag indicating the uniqueness and specialty of the file.
     * \return  Return the length of the path in the 'buffer'.
     */
    static unsigned int _osGetSpecialDir(char* buffer, unsigned int length, const eSpecialFolder specialFolder);

    /**
     * \brief   Returns OS specific invalid file handle.
     **/
    static FILEHANDLE _osGetInvalidHandle( void );

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
    DECLARE_NOCOPY_NOMOVE( File );
};

#endif  // AREG_BASE_FILE_HPP
