/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/FileWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File object 
 *              Windows OS specific implementation
 *
 ************************************************************************/

#include "areg/base/File.hpp"

#ifdef	_WIN32

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>
#include <ShlObj.h>

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

FILEHANDLE File::_osGetInvalidHandle( void )
{
    return static_cast<FILEHANDLE>(INVALID_HANDLE_VALUE);
}

void File::_osCloseFile( void )
{
    if ( isOpened( ) )
    {
        ::CloseHandle(static_cast<HANDLE>(mFileHandle));
    }

    mFileHandle = File::_osGetInvalidHandle();
}

bool File::_osOpenFile( void )
{
    bool result{ isOpened( ) };
    if ( result == false)
    {
        if (mFileName.isEmpty() == false )
        {
            mFileMode = normalizeMode(mFileMode);
            unsigned long access    = 0;
            unsigned long shared    = 0;
            unsigned long creation  = 0;
            unsigned long attributes= FILE_ATTRIBUTE_NORMAL;

            if ((mFileMode & FileBase::FOB_READ) != 0)
                access |= GENERIC_READ;

            if ((mFileMode & FileBase::FOB_WRITE) != 0)
                access |= GENERIC_WRITE;
            
            if ((mFileMode & FileBase::FOB_SHARE_READ) != 0)
                shared |= FILE_SHARE_READ;
            
            if ((mFileMode & FileBase::FOB_SHARE_WRITE) != 0)
                shared |= FILE_SHARE_WRITE;
            
            if ((mFileMode & FileBase::FOB_CREATE) != 0)
                creation |= CREATE_ALWAYS;
            
            if ((mFileMode & FileBase::FOB_EXIST) != 0)
                creation |= OPEN_EXISTING;
            
            if ((mFileMode & FileBase::FOB_TRUNCATE) != 0)
                creation |= TRUNCATE_EXISTING;
            
            if ((mFileMode & FileBase::FOB_TEMP_FILE) != 0)
                attributes = FILE_ATTRIBUTE_TEMPORARY;
            
            if ((mFileMode & FileBase::FOB_WRITE_DIRECT) != 0)
                attributes |= FILE_FLAG_WRITE_THROUGH;
            
            if ((mFileMode & FileBase::FOB_CREATE ) != 0)
            {
                File::createDirCascaded( File::getFileDirectory(mFileName) );
            }

            mFileHandle = static_cast<FILEHANDLE>(::CreateFileA(mFileName.getString(), access, shared, nullptr, creation, attributes, nullptr));
            result = isOpened();
        }
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

    return result;
}

unsigned int File::_osWriteFile(const unsigned char* buffer, unsigned int size)
{
    ASSERT(mFileHandle != nullptr);
    ASSERT((buffer != nullptr) && (size != 0));

    DWORD sizeWrite{ 0 };
    ::WriteFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeWrite, nullptr);

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

bool File::_osTruncateFile( void )
{
    bool result{ false };
    if (SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_BEGIN) != IECursorPosition::INVALID_CURSOR_POSITION)
    {
        result = SetEndOfFile(static_cast<HANDLE>(mFileHandle)) ? true : false;
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

unsigned int File::_osCreateTempFileName(char* buffer, const char* folder, const char* prefix, unsigned int unique)
{
    ASSERT(buffer != nullptr);
    ASSERT(folder != nullptr);
    ASSERT(prefix != nullptr);

    return static_cast<unsigned int>(::GetTempFileNameA(folder, prefix, unique, buffer) != 0 ? strlen(buffer) : 0);
}

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

#endif // _WIN32
