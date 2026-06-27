/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/FileWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, File object 
 *              Windows OS specific implementation
 *
 ************************************************************************/


#ifdef	_WIN32

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/File.hpp"
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif // !WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <ShlObj.h>

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/UtilityDefs.hpp"

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
static areg::String _searchFile( const char* fileName, const char* fileExtension, const char* searchInDirectory )
{
    String result;
    if (areg::is_empty<char>(fileName) == false )
    {
        fileExtension = fileExtension != nullptr && *fileExtension == '.' ? fileExtension : nullptr;
        String searchPath = File::NameHasCurrentFolder(searchInDirectory) ? File::GetCurrentDir() : searchInDirectory;
        char * buffer = new char[File::MAXIMUM_PATH + 1];
        uint32_t length = ::SearchPathA(searchPath, fileName, fileExtension, File::MAXIMUM_PATH, buffer, &fileName);
        result = length != 0 && length <= File::MAXIMUM_PATH ? buffer : "";

        delete [] buffer;
    }

    return result;
}

#endif

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

FILEHANDLE File::_os_invalid_handle() noexcept
{
    return static_cast<FILEHANDLE>(INVALID_HANDLE_VALUE);
}

void File::_os_close_file() noexcept
{
    if ( is_opened( ) )
    {
        ::CloseHandle(static_cast<HANDLE>(mFileHandle));
    }

    mFileHandle = File::_os_invalid_handle();
}

bool File::_os_open_file() noexcept
{
    bool result{ is_opened( ) };
    if ( result == false)
    {
        if (mFileName.is_empty() == false )
        {
            mFileMode = normalize_mode(mFileMode);
            unsigned long access    = 0;
            unsigned long shared    = 0;
            unsigned long creation  = 0;
            unsigned long attributes= FILE_ATTRIBUTE_NORMAL;

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitRead)) != 0)
                access |= GENERIC_READ;

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitWrite)) != 0)
                access |= GENERIC_WRITE;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareRead)) != 0)
                shared |= FILE_SHARE_READ;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareWrite)) != 0)
                shared |= FILE_SHARE_WRITE;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitCreateNew)) != 0)
                creation |= CREATE_ALWAYS;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitOpenAlways)) != 0)
                creation |= OPEN_ALWAYS;

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitExist)) != 0)
                creation |= OPEN_EXISTING;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTruncate)) != 0)
                creation |= TRUNCATE_EXISTING;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTemp)) != 0)
                attributes = FILE_ATTRIBUTE_TEMPORARY;
            
            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitDirect)) != 0)
                attributes |= FILE_FLAG_WRITE_THROUGH;

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitDelete)) != 0)
            {
                access     |= DELETE;                   // required for delete-on-close
                attributes |= FILE_FLAG_DELETE_ON_CLOSE;// OS deletes file when last handle is closed
            }

            if (((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitCreateNew)) != 0) ||
                ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitOpenAlways)) != 0))
            {
                File::create_dir_cascaded( File::file_directory(mFileName) );
            }

            mFileHandle = static_cast<FILEHANDLE>(::CreateFileA(mFileName.as_string(), access, shared, nullptr, creation, attributes, nullptr));
            result = is_opened();
        }
    }

    return result;
}

uint32_t File::_os_read_file(uint8_t* buffer, uint32_t size) const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    ASSERT((buffer != nullptr) && (size > 0));

    uint32_t result{ 0 };
    DWORD sizeRead{ 0 };

    if (::ReadFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeRead, nullptr))
    {
        result = static_cast<uint32_t>(sizeRead);
    }

    return result;
}

uint32_t File::_os_write_file(const uint8_t* buffer, uint32_t size) noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    ASSERT((buffer != nullptr) && (size != 0));

    DWORD sizeWrite{ 0 };
    ::WriteFile(static_cast<HANDLE>(mFileHandle), buffer, static_cast<unsigned long>(size), &sizeWrite, nullptr);

    return static_cast<uint32_t>(sizeWrite);
}

uint32_t File::_os_set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept
{
    ASSERT(mFileHandle != nullptr);

    unsigned long moveMethod = FILE_BEGIN;
    unsigned long moveOffset = static_cast<unsigned long>(offset);
    switch (startAt)
    {
    case    Cursor::SeekOrigin::Begin:
        moveMethod = FILE_BEGIN;
        break;

    case    Cursor::SeekOrigin::Current:
        moveMethod = FILE_CURRENT;
        break;

    case    Cursor::SeekOrigin::End:
        moveMethod = FILE_END;
        break;

    default:
        AREG_OUTPUT_ERR("Unexpected FileBase::SeekOrigin type.");
        moveMethod = FILE_CURRENT;
        moveOffset = 0;
    }

    DWORD pos = ::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(moveOffset), nullptr, static_cast<DWORD>(moveMethod));
    return static_cast<uint32_t>(pos != INVALID_SET_FILE_POINTER ? pos : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t File::_os_file_position() const noexcept
{
    ASSERT(mFileHandle != nullptr);
    DWORD pos = ::SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_CURRENT);
    return static_cast<uint32_t>(pos != INVALID_SET_FILE_POINTER ? pos : Cursor::INVALID_CURSOR_POSITION);
}

bool File::_os_truncate_file() noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    DWORD pos = ::SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, nullptr, FILE_BEGIN);
    return ((pos != INVALID_SET_FILE_POINTER) && static_cast<bool>(::SetEndOfFile(static_cast<HANDLE>(mFileHandle))));
}

bool File::_os_reserve(uint32_t newSize) noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    // Move the pointer to the target size then mark it as the new end-of-file.
    DWORD moved = ::SetFilePointer(static_cast<HANDLE>(mFileHandle), static_cast<LONG>(newSize), nullptr, FILE_BEGIN);
    return ((moved == INVALID_SET_FILE_POINTER) && static_cast<bool>(::SetEndOfFile(static_cast<HANDLE>(mFileHandle))));
}

void File::_os_flush_file() noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    ::FlushFileBuffers(static_cast<HANDLE>(mFileHandle));
}

uint32_t File::_os_file_length() const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    LARGE_INTEGER size{};
    return ::GetFileSizeEx(static_cast<HANDLE>(mFileHandle), &size) ? static_cast<uint32_t>(size.QuadPart) : 0u;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

uint32_t File::_os_temp_name(char* buffer, const char* folder, const char* prefix, uint32_t unique) noexcept
{
    ASSERT(buffer != nullptr);
    ASSERT(folder != nullptr);
    ASSERT(prefix != nullptr);

    return static_cast<uint32_t>(::GetTempFileNameA(folder, prefix, unique, buffer) != 0 ? strlen(buffer) : 0u);
}

/**
 * \brief   Returns special folder path. The type of required folder is defined in
 *          specialFolder parameter.
 * \param   specialFolder   The type of special folder in the system.
 * \return  If function succeeds, the return value is full path of special folder.
 *          Otherwise, it returns empty string.
 **/
uint32_t File::_os_special_dir(char* buffer, uint32_t length, const File::SpecialFolder specialFolder) noexcept
{
    ASSERT(buffer != nullptr);
    buffer[0] = areg::EndOfString;

    int32_t csidl = -1;
    switch (specialFolder)
    {
    case File::SpecialFolder::UserHome:
        csidl = CSIDL_PROFILE;
        break;

    case File::SpecialFolder::Personal:
        csidl = CSIDL_PERSONAL;
        break;

    case File::SpecialFolder::AppData:
        csidl = CSIDL_APPDATA;
        break;

    case File::SpecialFolder::Temp:
        GetTempPathA(length, buffer);
        break;

    default:
        break;
    }

    if (csidl != -1)
    {
        ::SHGetFolderPathA(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, buffer);
    }

    return static_cast<uint32_t>(strlen(buffer));
}

} // namespace areg
#endif // _WIN32
