/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
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
const int           File::MAXIMUM_PATH        = 1024;
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
unsigned int File::_osGetSpecialDir(char* buffer, unsigned int length, const eSpecialFolder specialFolder)
{
    ASSERT(buffer != nullptr);
    buffer[0] = NEString::EndOfString;

    int csidl = -1;
    switch (specialFolder)
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
        GetTempPathA(length, buffer);
        break;

    default:
        break;
    }

    if (csidl != -1)
    {
        SHGetFolderPathA(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, buffer);
    }

    return static_cast<unsigned int>(strlen(buffer));
}

void File::_osCloseFile( void )
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

bool File::_osOpenFile( void )
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

unsigned int File::_osReadFile(unsigned char* buffer, unsigned int size) const
{
    ASSERT(mFileHandle != nullptr);
    ASSERT((buffer != nullptr) && (size > 0));

    unsigned int result{ 0 };
    DWORD sizeRead{ 0 };

    if (::ReadFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeRead, nullptr))
    {
        result = static_cast<unsigned int>(sizeRead);
    }
    else
    {
        OUTPUT_ERR("Failed to read file [ %s ], error code [ %p ].", mFileName.getString(), static_cast<id_type>(GetLastError()));
    }

    return result;
}

unsigned int File::_osWriteFile(const unsigned char* buffer, unsigned int size)
{
    ASSERT(mFileHandle != nullptr);
    ASSERT((buffer != nullptr) && (size != 0));

    DWORD sizeWrite{ 0 };
    if (::WriteFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeWrite, nullptr) == FALSE)
    {
        OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.getString(), static_cast<id_type>(GetLastError()));
    }

    return static_cast<unsigned int>(sizeWrite);
}

unsigned int File::_osSetPositionFile(int offset, IECursorPosition::eCursorPosition startAt) const
{
    ASSERT(mFileHandle != nullptr);

    unsigned long moveMethod = FILE_BEGIN;
    unsigned long moveOffset = static_cast<unsigned long>(offset);
    switch (startAt)
    {
    case    IECursorPosition::eCursorPosition::PositionBegin:
        moveMethod = FILE_BEGIN;
        break;

    case    IECursorPosition::eCursorPosition::PositionCurrent:
        moveMethod = FILE_CURRENT;
        break;

    case    IECursorPosition::eCursorPosition::PositionEnd:
        moveMethod = FILE_END;
        break;

    default:
        OUTPUT_ERR("Unexpected FileBase::eCursorPosition type.");
        moveMethod = FILE_CURRENT;
        moveOffset = 0;
    }

    return static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(moveOffset), nullptr, static_cast<DWORD>(moveMethod)));
}

unsigned int File::_osGetPositionFile( void ) const
{
    ASSERT(mFileHandle != nullptr);
    return static_cast<unsigned int>( SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_CURRENT) );
}

unsigned int File::_osGetLengthFile( void ) const
{
    return static_cast<unsigned int>( GetFileSize(static_cast<HANDLE>(mFileHandle), nullptr ) );
}

unsigned int File::_osReserveFile(unsigned int newSize)
{
    ASSERT(mFileHandle != nullptr);

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    unsigned int curPos = newSize <= 0 ? 0 : _osGetPositionFile();
    curPos = newSize > static_cast<int>(curPos) ? curPos : newSize;
    if (newSize != 0)
    {
        LARGE_INTEGER li;
        li.QuadPart = static_cast<int64_t>(newSize);
        if (::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(li.LowPart), static_cast<PLONG>(&li.HighPart), FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
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

    return result;
}

bool File::_osTruncateFile( void )
{
    bool result{ false };
    if (SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
    {
        result = SetEndOfFile(static_cast<HANDLE>(mFileHandle)) ? true : false;
    }
    else
    {
        OUTPUT_ERR("Failed to set file pointer new position.");
    }

    return result;
}

void File::_osFlushFile( void )
{
    ASSERT(mFileHandle != nullptr);
    ::FlushFileBuffers(static_cast<HANDLE>(mFileHandle));
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::_osDeleteFile( const char* filePath )
{
    ASSERT(filePath != nullptr);
    return (::DeleteFileA(filePath) == TRUE);
}

bool File::_osCreateDir( const char* dirPath )
{
    ASSERT(dirPath != nullptr);
    return (::CreateDirectoryA(dirPath, nullptr) == TRUE);
}

bool File::_osDeleteDir( const char* dirPath )
{
    ASSERT(dirPath != nullptr);
    return (::RemoveDirectoryA(dirPath) == TRUE);
}

bool File::_osMoveFile( const char* oldPath, const char* newPath )
{
    ASSERT(oldPath != nullptr);
    ASSERT(newPath != nullptr);
    return (::MoveFileA(oldPath, newPath) == TRUE);
}

unsigned int File::_osGetCurrentDir(char* buffer, unsigned int length)
{
    ASSERT(buffer != nullptr);
    return static_cast<unsigned int>(::GetCurrentDirectoryA(length, buffer));
}

bool File::_osSetCurrentDir( const char* dirPath )
{
    ASSERT(dirPath != nullptr);
    return (::SetCurrentDirectoryA(dirPath) == TRUE);
}

bool File::_osCopyFile( const char* originPath, const char* newPath, bool copyForce )
{
    ASSERT(originPath != nullptr);
    ASSERT(newPath != nullptr);
    return (::CopyFileA(originPath, newPath, copyForce ? FALSE : TRUE) == TRUE);
}

unsigned int File::_osGetTempDir(char* buffer, unsigned int length)
{
    ASSERT(buffer);
    return static_cast<unsigned int>(::GetTempPathA(length, buffer));
}

unsigned int File::_osCreateTempFile(char* buffer, const char* folder, const char* prefix, unsigned int unique)
{
    ASSERT(buffer != nullptr);
    ASSERT(folder != nullptr);
    ASSERT(prefix != nullptr);

    return static_cast<unsigned int>(::GetTempFileNameA(folder, prefix, unique, buffer) != 0 ? strlen(buffer) : 0);
}

bool File::_osExistDir(const char* dirPath)
{
    ASSERT(dirPath != nullptr);
    unsigned long attr = ::GetFileAttributesA(dirPath);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool File::_osExistFile(const char* filePath)
{
    ASSERT(filePath != nullptr);
    unsigned long attr = ::GetFileAttributesA(filePath);
    return (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

#endif // _WINDOWS
