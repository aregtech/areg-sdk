/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/FileWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
const char          File::PATH_SEPARATOR      = '\\';
const int           File::MAXIMUM_PATH        = MAX_PATH;
void * const        File::INVALID_HANDLE      = static_cast<void *>(INVALID_HANDLE_VALUE);

#if 0


/**
 * \brief	Search given file in directory and returns full path. 
 *          If searchInDirectory is nullptr, it will search file name
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
 *                              this parameter can be nullptr.
 * 
 * \param	searchInDirectory	Set folder name to search. If nullptr, it will search file name
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
        fileExtension = fileExtension != nullptr && *fileExtension == '.' ? fileExtension : nullptr;
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
String File::getSpecialDir(File::eSpecialFolder specialFolder)
{
    String result;
    char * buffer = DEBUG_NEW char [File::MAXIMUM_PATH + 1];
    int csidl = -1;

    if (buffer != nullptr)
    {
        buffer[0] = NEString::EndOfString;

        switch ( specialFolder )
        {
        case File::eSpecialFolder::SpecialUserHome:
            csidl = CSIDL_PROFILE;
            break;

        case File::eSpecialFolder::SpecialPersonal:
            csidl = CSIDL_PERSONAL;
            break;

        case File::eSpecialFolder::SpecialAppData:
            csidl = CSIDL_APPDATA;
            break;

        case File::eSpecialFolder::SpecialTemp:
            GetTempPathA(File::MAXIMUM_PATH, buffer);
            break;

        default:
            break;
        }

        if (csidl != -1)
        {
            SHGetFolderPathA( nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, buffer);
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
                File::createDirCascaded( File::getFileDirectory(mFileName) );
            }

            mFileHandle = static_cast<FILEHANDLE>(::CreateFileA(mFileName.getString(), access, shared, nullptr, creation, attributes, nullptr ));
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
        if ((buffer != nullptr) && (size > 0))
        {
            unsigned long sizeRead = 0;
            if (::ReadFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeRead, nullptr ))
            {
                result = static_cast<unsigned int>(sizeRead);
            }
            else
            {
                OUTPUT_ERR("Failed to read file [ %s ], error code [ %p ].", mFileName.getString(), static_cast<id_type>(GetLastError()));
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
        if ((buffer != nullptr) && (size > 0))
        {
            unsigned long sizeWrite = 0;
            if ( ::WriteFile( static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeWrite, nullptr ) )
            {
                result = static_cast<unsigned int>(sizeWrite);
            }
            else
            {
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.getString(), static_cast<id_type>(GetLastError()));
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
        case    IECursorPosition::eCursorPosition::PositionBegin:
            moveMethod  = FILE_BEGIN;
            break;

        case    IECursorPosition::eCursorPosition::PositionCurrent:
            moveMethod  = FILE_CURRENT;
            break;

        case    IECursorPosition::eCursorPosition::PositionEnd:
            moveMethod = FILE_END;
            break;

        default:
            OUTPUT_ERR("Unexpected FileBase::eCursorPosition type.");
            moveMethod  = FILE_CURRENT;
            moveOffset  = 0;
        }

        result = static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(moveOffset), nullptr, static_cast<DWORD>(moveMethod)));
    }
    
    return result;
}

unsigned int File::getPosition( void ) const
{
    return (isOpened() ? static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_CURRENT)) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int File::getLength( void ) const
{
    return (isOpened() ? static_cast<unsigned int>(GetFileSize(static_cast<HANDLE>(mFileHandle), nullptr )) : NEMemory::INVALID_SIZE);
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
            if (::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(newSize), nullptr, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
            {
                ::SetEndOfFile(static_cast<HANDLE>(mFileHandle));
                result = static_cast<unsigned int>(::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(curPos), nullptr, FILE_BEGIN));
            }
        }
        else if (::SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
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
        if (SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
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
    if ( mFileHandle != nullptr )
    {
        ::FlushFileBuffers(static_cast<HANDLE>(mFileHandle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::deleteFile( const char* filePath )
{
    return ((NEString::isEmpty<char>(filePath) == false) && (::DeleteFileA(filePath) == TRUE));
}

bool File::createDir( const char* dirPath )
{
    return ((NEString::isEmpty<char>(dirPath) == false) && (::CreateDirectoryA(dirPath, nullptr) == TRUE));
}

bool File::deleteDir( const char* dirPath )
{
    return ((NEString::isEmpty<char>(dirPath) == false) && (::RemoveDirectoryA(dirPath) == TRUE));
}

bool File::moveFile( const char* oldPath, const char* newPath )
{
    return ((NEString::isEmpty<char>(oldPath) == false) && (NEString::isEmpty<char>(newPath) == false) && (::MoveFileA(oldPath, newPath) == TRUE));
}

String File::getCurrentDir( void )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];

    if (buffer != nullptr)
    {
        result = ::GetCurrentDirectoryA(File::MAXIMUM_PATH, buffer) <= File::MAXIMUM_PATH ? buffer : "";
        delete [] buffer;
    }

    return result;
}

bool File::setCurrentDir( const char* dirPath )
{
    return ((NEString::isEmpty<char>(dirPath) == false) && (::SetCurrentDirectoryA(dirPath) == TRUE));
}

bool File::copyFile( const char* originPath, const char* newPath, bool copyForce )
{
    return ((NEString::isEmpty<char>(originPath) == false) && 
            (NEString::isEmpty<char>(newPath) == false) &&
            (::CopyFileA(originPath, newPath, copyForce ? FALSE : TRUE) == TRUE));
}

String File::getTempDir( void )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
    if (buffer != nullptr)
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
    if (buffer != nullptr)
    {
        unsigned int tmpUnique = unique ? 0 : static_cast<unsigned int>( DateTime::getSystemTickCount() );
        prefix = prefix == nullptr ? File::TEMP_FILE_PREFIX.data() : prefix;
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
    bool result{ false };
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        unsigned long attr = ::GetFileAttributesA(filePath);
        result = (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
    }
    return result;
}

String File::getFileFullPath( const char* filePath )
{
    String result(filePath);
    if (NEString::isEmpty<char>(filePath) == false)
    {
        char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
        if ( buffer != nullptr )
        {
            if ( ::GetFullPathNameA( filePath, File::MAXIMUM_PATH, buffer, nullptr) <= File::MAXIMUM_PATH)
            {
                result = buffer;
            }

            delete [] buffer;
        }
    }

    return result;
}

bool File::_createFolder( const char * dirPath )
{
    // no need to validate string. Put in assertion.
    ASSERT( NEString::isEmpty<char>(dirPath) == false );
    return (::CreateDirectoryA(dirPath, nullptr ) == TRUE);
}

#endif // _WINDOWS
