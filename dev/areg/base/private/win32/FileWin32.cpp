/************************************************************************
 * \file        areg/base/private/win32/FileWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object 
 *              Windows OS specific implementation
 *
 ************************************************************************/

#include "areg/base/File.hpp"

#ifdef	_WINDOWS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <shlobj.h>

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
void * const        File::INVALID_HANDLE      = static_cast<void *>(INVALID_HANDLE_VALUE);
const char          File::PATH_SEPARATOR      = '\\';
const int           File::MAXIMUM_PATH        = MAX_PATH;

#if 0


/**
 * \brief	Search given file in directory and returns full path. 
 *          If searchInDirectory is NULL, it will search file name
 *              - in current directory
 *              - in system directory
 *              - in windows directory
 *              - in directories that are listed in the PATH environment variable.
 *
 * \param	fileName	        The file name to search. File can contain no extension 
 *                              if it is specified in fileExtension variable
 *
 * \param   fileExtension       The extension to be added to the file name when searching for the file. 
 *                              The first character of the file name extension must be a period (.) (example, ".cfg"). 
 *                              The extension is added only if the specified file name does not end with an extension. 
 *                              If a file name extension is not required or if the file name contains an extension, 
 *                              this parameter can be NULL.
 * 
 * \param	searchInDirectory	Set folder name to search. If NULL, it will search file name
 *                                  - in current directory
 *                                  - in system directory
 *                                  - in windows directory
 *                                  - in directories that are listed in the PATH environment variable.
 *
 * \return	If succeeded, returns the full path to file (including file name). Otherwise returned string is empty.
 **/
static String _searchFile( const char* fileName, const char* fileExtension, const char* searchInDirectory )
{
    String result;
    if ( NEString::isEmpty<char>(fileName) == false )
    {
        fileExtension != NULL && *fileExtension == '.' ? fileExtension : NULL;
        String searchPath = File::NameHasCurrentFolder(searchInDirectory) ? File::GetCurrentDir() : searchInDirectory;
        char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
        unsigned int length = ::SearchPathA(searchPath, fileName, fileExtension, File::MAXIMUM_PATH, buffer, &fileName);
        result = length != 0 && length <= File::MAXIMUM_PATH ? buffer : "";

        delete [] buffer;
    }

    return result;
}

#endif

/**
 * \brief   Returns special folder path. The type of required folder is defined in
 *          specialFolder parameter.
 * \param   specialFolder   The type of special folder in the system.
 * \return  If function succeeds, the return value is full path of special folder.
 *          Otherwise, it returns empty string.
 **/
String File::getSpecialDir(eSpecialFolder specialFolder)
{
    String result;
    char * buffer = DEBUG_NEW char [File::MAXIMUM_PATH + 1];
    int csidl = -1;

    if (buffer != NULL)
    {
        buffer[0] = NEString::EndOfString;

        switch ( specialFolder )
        {
        case File::SpecialUserHome:
            csidl = CSIDL_PROFILE;
            break;

        case File::SpecialPersonal:
            csidl = CSIDL_PERSONAL;
            break;

        case File::SpecialAppData:
            csidl = CSIDL_APPDATA;
            break;

        case File::SpecialTemp:
            GetTempPathA(File::MAXIMUM_PATH, buffer);
            break;

        default:
            break;
        }

        if (csidl != -1)
        {
            SHGetFolderPathA(NULL, csidl, NULL, SHGFP_TYPE_CURRENT, buffer);
        }

        result = static_cast<const char *>(buffer);
        delete [] buffer;
    }

    return result;
}

void File::_closeFile( void )
{
    if ( isOpened( ) )
    {
        ::CloseHandle(static_cast<HANDLE>(mFileHandle));
    }

    mFileHandle = File::INVALID_HANDLE;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool File::open( void )
{
    bool result = false;
    if (isOpened() == false)
    {
        if (mFileName.isEmpty() == false )
        {
            mFileMode = normalizeMode(mFileMode);
            unsigned long access    = 0;
            unsigned long shared    = 0;
            unsigned long creation  = 0;
            unsigned long attributes= FILE_ATTRIBUTE_NORMAL;

            if (mFileMode & FileBase::FOB_READ)
                access |= GENERIC_READ;

            if (mFileMode & FileBase::FOB_WRITE)
                access |= GENERIC_WRITE;
            
            if (mFileMode & FileBase::FOB_SHARE_READ)
                shared |= FILE_SHARE_READ;
            
            if (mFileMode & FileBase::FOB_SHARE_WRITE)
                shared |= FILE_SHARE_WRITE;
            
            if (mFileMode & FileBase::FOB_CREATE)
                creation |= CREATE_ALWAYS;
            
            if (mFileMode & FileBase::FOB_EXIST)
                creation |= OPEN_EXISTING;
            
            if (mFileMode & FileBase::FOB_TRUNCATE)
                creation |= TRUNCATE_EXISTING;
            
            if (mFileMode & FileBase::FOB_TEMP_FILE)
                attributes = FILE_ATTRIBUTE_TEMPORARY;
            
            if (mFileMode & FileBase::FOB_WRITE_DIRECT)
                attributes |= FILE_FLAG_WRITE_THROUGH;
            
            if ( mFileMode & FileBase::FOB_CREATE )
            {
                File::createDirCascaded( File::getFileDirectory( static_cast<const char *>(mFileName) ).getString() );
            }

            mFileHandle = static_cast<void *>(::CreateFileA(mFileName.getString(), access, shared, NULL, creation, attributes, NULL));
            result = isOpened();
        }
        else
        {
            OUTPUT_ERR("Either file name or file open mode is not set.");
        }
    }
    else
    {
        OUTPUT_WARN("File is already opened. Close file.");
    }

    return result;
}

void File::close( void )
{
    _closeFile();
}

unsigned int File::read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if ( isOpened() && canRead() )
    {
        if ((buffer != NULL) && (size > 0))
        {
            unsigned long sizeRead = 0;
            if (::ReadFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeRead, NULL))
            {
                result = static_cast<unsigned int>(sizeRead);
            }
            else
            {
                OUTPUT_ERR("Failed to read file [ %s ], error code [ %p ].", mFileName.getString(), GetLastError());
            }
        }
        else
        {
            OUTPUT_WARN("The length is zero, do not copy data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or reading mode is not set (mode = [ %d ]).", mFileName.getString(), isOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int File::write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;
    if ( isOpened() && canWrite() )
    {
        if (buffer != NULL && size > 0)
        {
            unsigned long sizeWrite = 0;
            if ( ::WriteFile( static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeWrite, NULL ) )
            {
                result = static_cast<unsigned int>(sizeWrite);
            }
            else
            {
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.getString(), GetLastError());
            }
        }
        else
        {
            OUTPUT_WARN("The size of data is zero, ignoring to write data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or writting mode is not set (mode = [ %d ]).", mFileName.getString(), isOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int File::setPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    unsigned int result = static_cast<unsigned int>(INVALID_SET_FILE_POINTER);

    if ( isOpened() )
    {
        unsigned long moveMethod = FILE_BEGIN;
        unsigned long moveOffset = static_cast<unsigned long>(offset);
        switch (startAt)
        {
        case    IECursorPosition::POSITION_BEGIN:
            moveMethod  = FILE_BEGIN;
            break;

        case    IECursorPosition::POSITION_CURRENT:
            moveMethod  = FILE_CURRENT;
            break;

        case    IECursorPosition::POSITION_END:
            moveMethod = FILE_END;
            break;

        default:
            OUTPUT_ERR("Unexpected FileBase::eCursorPosition type.");
            moveMethod  = FILE_CURRENT;
            moveOffset  = 0;
        }
        result = static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), moveOffset, NULL, moveMethod));
    }
    
    return result;
}

unsigned int File::getPosition( void ) const
{
    return (isOpened() ? static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, NULL, FILE_CURRENT)) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int File::getLength( void ) const
{
    return (isOpened() ? static_cast<unsigned int>(GetFileSize(static_cast<HANDLE>(mFileHandle), NULL)) : NEMemory::INVALID_SIZE);
}

unsigned int File::reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName.getString());

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (isOpened() && canWrite())
    {
        unsigned int curPos = newSize <= 0 ? 0 : getPosition();
        curPos = newSize > static_cast<int>(curPos) ? curPos : newSize;
        if (newSize > 0)
        {
            if (::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<unsigned long>(newSize), NULL, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
            {
                ::SetEndOfFile(static_cast<HANDLE>(mFileHandle));
                result = static_cast<unsigned int>(::SetFilePointer(static_cast<HANDLE>(mFileHandle), curPos, NULL, FILE_BEGIN));
            }
        }
        else if (::SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, NULL, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
        {
            result = ::SetEndOfFile(static_cast<HANDLE>(mFileHandle)) ? 0 : IECursorPosition::INVALID_CURSOR_POSITION;
        }
        else
        {
            OUTPUT_ERR("Failed to set file pointer new position.");
        }
    }
    return result;
}

bool File::truncate( void )
{
    bool result = false;
    if (isOpened() && canWrite())
    {
        if (SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, NULL, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
        {
            result = SetEndOfFile(static_cast<HANDLE>(mFileHandle)) ? true : false;
        }
        else
        {
            OUTPUT_ERR("Failed to set file pointer new position.");
        }
    }
    return result;
}

void File::flush( void )
{
    if ( mFileHandle != NULL )
    {
        ::FlushFileBuffers(static_cast<HANDLE>(mFileHandle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::deleteFile( const char* filePath )
{
    return ( NEString::isEmpty<char>(filePath) == false ? ::DeleteFileA(filePath) == TRUE : false );
}

bool File::createDir( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::CreateDirectoryA(dirPath, NULL) == TRUE : false );
}

bool File::deleteDir( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::RemoveDirectoryA(dirPath) == TRUE : false );
}

bool File::moveFile( const char* oldPath, const char* newPath )
{
    return ( NEString::isEmpty<char>(oldPath) == false && NEString::isEmpty<char>(newPath) == false ? ::MoveFileA(oldPath, newPath) == TRUE : false );
}

String File::getCurrentDir( void )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];

    if (buffer != NULL)
    {
        result = ::GetCurrentDirectoryA(File::MAXIMUM_PATH, buffer) <= File::MAXIMUM_PATH ? buffer : "";
        delete [] buffer;
    }

    return result;
}

bool File::setCurrentDir( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::SetCurrentDirectoryA(dirPath) == TRUE : false );
}

bool File::copyFile( const char* originPath, const char* newPath, bool copyForce )
{
    return ( NEString::isEmpty<char>(originPath) == false && NEString::isEmpty<char>(newPath) == false  ? 
            ::CopyFileA(originPath, newPath, copyForce ? FALSE : TRUE) == TRUE                                              :
            false );
}

String File::getTempDir( void )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        result = ::GetTempPathA(File::MAXIMUM_PATH, buffer) <= File::MAXIMUM_PATH ? buffer : "";
        delete [] buffer;
    }

    return result;
}

String File::genTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    String result;
    char * buffer = DEBUG_NEW char [File::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        unsigned int tmpUnique = unique ? 0 : static_cast<unsigned int>( DateTime::getSystemTickCount() );
        prefix = prefix == NULL ? File::TEMP_FILE_PREFIX : prefix;
        String folder = inTempFolder ? File::getTempDir() : File::getCurrentDir();
        result = ::GetTempFileNameA(folder, prefix, tmpUnique, buffer) != 0 ? buffer : "";

        delete [] buffer;
    }

    return result;
}

bool File::existDir( const char* dirPath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        unsigned long attr = ::GetFileAttributesA(dirPath);
        result = (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
    }
    return result;
}

bool File::existFile( const char* filePath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        unsigned long attr = ::GetFileAttributesA(filePath);
        result = (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
    }
    return result;
}

String File::getFileFullPath( const char* filePath )
{
    String result  = filePath;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
        if ( (buffer != NULL) && (::GetFullPathNameA(result, File::MAXIMUM_PATH, buffer, NULL) <= File::MAXIMUM_PATH) )
            result = buffer;

        if (buffer != NULL)
            delete [] buffer;
    }

    return result;
}

bool File::_createFolder( const char * dirPath )
{
    // no need to validate string. Put in assertion.
    ASSERT( NEString::isEmpty<char>(dirPath) == false );
    return (::CreateDirectoryA(dirPath, NULL) == TRUE);
}

#if 0
String File::NormalizeFileName( const char * fileName, bool forceTime/*=false*/ )
{
    String result;

    SYSTEMTIME st;
    char fmt[32];
    memset(&st, 0, sizeof(SYSTEMTIME));
    memset(fmt, 0, 32);

    GetLocalTime(&st);
    String::PrintString(fmt, 32, File::TIMESTAMP_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    if ( NEString::isEmpty<char>(fileName) == false )
    {
        result = fileName;
        NEString::CharPos index = result.FindFirstOf(File::FILE_MASK_TIMESTAMP);
        if ( index != NEString::InvalidPos )
        {
            result.Replace(fmt, index, NEString::getStringLength<char>(File::FILE_MASK_TIMESTAMP) );
        }
        else if (forceTime)
        {
            index = result.FindLastOf('.');
            if (index != NEString::InvalidPos)
                result.Insert(fmt, index);
            else
                result += fmt;
        }

        index = result.FindFirstOf( File::FILE_MASK_APP_DATA );
        if ( index != NEString::InvalidPos )
        {
            String appFolder = File::GetSpecialFolder(File::SpecialAppData);
            result.Replace(appFolder, index, NEString::getStringLength<char>(File::FILE_MASK_APP_DATA) );
        }
    }
    else if (forceTime)
    {
        result = fmt;
    }
    return File::NormalizeFilePath(result);
}
#endif // 0

#endif // _WINDOWS
