/************************************************************************
 * \file        areg/base/private/win32/CEFileWin32.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object 
 *              Windows OS specific implementation
 *
 ************************************************************************/

#include "areg/base/CEFile.hpp"

#ifdef	_WINDOWS

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <shlobj.h>

#include "areg/base/CESharedBuffer.hpp"
#include "areg/base/CEProcess.hpp"
#include "areg/base/CEDateTime.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// CEFile class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
void * const        CEFile::INVALID_HANDLE      = static_cast<void *>(INVALID_HANDLE_VALUE);
const char          CEFile::PATH_SEPARATOR      = '\\';
const int           CEFile::MAXIMUM_PATH        = MAX_PATH;

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
static CEString _searchFile( const char* fileName, const char* fileExtension, const char* searchInDirectory )
{
    CEString result;
    if ( NEString::isEmpty<char>(fileName) == false )
    {
        fileExtension != NULL && *fileExtension == '.' ? fileExtension : NULL;
        CEString searchPath = CEFile::NameHasCurrentFolder(searchInDirectory) ? CEFile::GetCurrentDir() : searchInDirectory;
        char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
        unsigned int length = ::SearchPathA(searchPath, fileName, fileExtension, CEFile::MAXIMUM_PATH, buffer, &fileName);
        result = length != 0 && length <= CEFile::MAXIMUM_PATH ? buffer : "";

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
CEString CEFile::GetSpecialFolder(eSpecialFolder specialFolder)
{
    CEString result;
    char * buffer = DEBUG_NEW char [CEFile::MAXIMUM_PATH + 1];
    int csidl = -1;

    if (buffer != NULL)
    {
        buffer[0] = NEString::EndOfString;

        switch ( specialFolder )
        {
        case CEFile::SpecialUserHome:
            csidl = CSIDL_PROFILE;
            break;

        case CEFile::SpecialPersonal:
            csidl = CSIDL_PERSONAL;
            break;

        case CEFile::SpecialAppData:
            csidl = CSIDL_APPDATA;
            break;

        case CEFile::SpecialTemp:
            GetTempPathA(CEFile::MAXIMUM_PATH, buffer);
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

void CEFile::_closeFile( void )
{
    if ( IsOpened( ) )
    {
        ::CloseHandle(static_cast<HANDLE>(mFileHandle));
    }

    mFileHandle = CEFile::INVALID_HANDLE;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool CEFile::Open( void )
{
    bool result = false;
    if (IsOpened() == false)
    {
        if (mFileName.IsEmpty() == false )
        {
            mFileMode = NormalizeMode(mFileMode);
            unsigned long access    = 0;
            unsigned long shared    = 0;
            unsigned long creation  = 0;
            unsigned long attributes= FILE_ATTRIBUTE_NORMAL;

            if (mFileMode & CEFileBase::FOB_READ)
                access |= GENERIC_READ;

            if (mFileMode & CEFileBase::FOB_WRITE)
                access |= GENERIC_WRITE;
            
            if (mFileMode & CEFileBase::FOB_SHARE_READ)
                shared |= FILE_SHARE_READ;
            
            if (mFileMode & CEFileBase::FOB_SHARE_WRITE)
                shared |= FILE_SHARE_WRITE;
            
            if (mFileMode & CEFileBase::FOB_CREATE)
                creation |= CREATE_ALWAYS;
            
            if (mFileMode & CEFileBase::FOB_EXIST)
                creation |= OPEN_EXISTING;
            
            if (mFileMode & CEFileBase::FOB_TRUNCATE)
                creation |= TRUNCATE_EXISTING;
            
            if (mFileMode & CEFileBase::FOB_TEMP_FILE)
                attributes = FILE_ATTRIBUTE_TEMPORARY;
            
            if (mFileMode & CEFileBase::FOB_WRITE_DIRECT)
                attributes |= FILE_FLAG_WRITE_THROUGH;
            
            if ( mFileMode & CEFileBase::FOB_CREATE )
            {
                CEFile::CreateFoldersCascaded( CEFile::GetFileDirectory( static_cast<const char *>(mFileName) ).GetBuffer() );
            }

            mFileHandle = static_cast<void *>(::CreateFileA(mFileName.GetBuffer(), access, shared, NULL, creation, attributes, NULL));
            result = IsOpened();
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

void CEFile::Close( void )
{
    _closeFile();
}

unsigned int CEFile::Read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if ( IsOpened() && CanRead() )
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
                OUTPUT_ERR("Failed to read file [ %s ], error code [ %p ].", mFileName.GetBuffer(), GetLastError());
            }
        }
        else
        {
            OUTPUT_WARN("The length is zero, do not copy data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or reading mode is not set (mode = [ %d ]).", mFileName.GetBuffer(), IsOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int CEFile::Read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    buffer.InvalidateBuffer();

    if ( IsOpened() && CanRead() )
    {
        unsigned long sizeRead = 0;
        unsigned int  sizeReserve = 0;
        if (::ReadFile(static_cast<HANDLE>(mFileHandle), reinterpret_cast<LPVOID>(&sizeReserve), sizeof(unsigned int), &sizeRead, NULL))
        {
            sizeRead = buffer.ResizeBuffer(sizeReserve, false);
            unsigned char * data = sizeRead != 0 ? buffer.GetBuffer() : NULL;
            if ((data != NULL) && ReadFile(static_cast<HANDLE>(mFileHandle), reinterpret_cast<LPVOID>(data), sizeRead, &sizeRead, NULL))
            {
                result = static_cast<unsigned int>(sizeRead);
            }
            else
            {
                OUTPUT_ERR("Either was not able to reserve [ %d ] bytes of space, or failed read file [ %s ], error code [ %p ].", sizeReserve, mFileName.GetBuffer(), GetLastError());
            }
        }
        else
        {
            OUTPUT_WARN("Either failed to read file size or the length is zero, ignoring copying data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or reading mode is not set (mode = [ %d ]).", mFileName.GetBuffer(), IsOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int CEFile::Write( const IEByteBuffer & buffer )
{
    unsigned int result = 0;

    if ( IsOpened() && CanWrite() )
    {
        const unsigned char * data  = buffer.GetBuffer();
        unsigned int sizeUsed       = buffer.GetUsedSize();
        unsigned long sizeWrite     = 0;

        if ( ::WriteFile( static_cast<HANDLE>(mFileHandle), &sizeUsed, sizeof(unsigned int), &sizeWrite, NULL ) &&
             ::WriteFile( static_cast<HANDLE>(mFileHandle), data, sizeUsed, &sizeWrite, NULL ) )
        {
            result = sizeUsed + sizeof(unsigned int);
        }
        else
        {
            OUTPUT_ERR("Failed to write [ %d ] bytes of data in file [ %s ], error code [ %p ]", sizeUsed + sizeof(unsigned int), mFileName.GetBuffer(), GetLastError());
        }
    }
    else
    {
        OUTPUT_ERR("Either files [ %s ] is not opened or it is not opened to write file, error code [ %p ]", mFileName.GetBuffer(), GetLastError());
    }

    return result;
}

unsigned int CEFile::Write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;
    if ( IsOpened() && CanWrite() )
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
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.GetBuffer(), GetLastError());
            }
        }
        else
        {
            OUTPUT_WARN("The size of data is zero, ignoring to write data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or writting mode is not set (mode = [ %d ]).", mFileName.GetBuffer(), IsOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int CEFile::SetPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    unsigned int result = static_cast<unsigned int>(INVALID_SET_FILE_POINTER);

    if ( IsOpened() )
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
            OUTPUT_ERR("Unexpected CEFileBase::eCursorPosition type.");
            moveMethod  = FILE_CURRENT;
            moveOffset  = 0;
        }
        result = static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), moveOffset, NULL, moveMethod));
    }
    
    return result;
}

unsigned int CEFile::GetPosition( void ) const
{
    return (IsOpened() ? static_cast<unsigned int>(SetFilePointer(static_cast<HANDLE>(mFileHandle), 0, NULL, FILE_CURRENT)) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int CEFile::GetLength( void ) const
{
    return (IsOpened() ? static_cast<unsigned int>(GetFileSize(static_cast<HANDLE>(mFileHandle), NULL)) : NEMemory::INVALID_SIZE);
}

unsigned int CEFile::Reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName.GetBuffer());

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (IsOpened() && CanWrite())
    {
        unsigned int curPos = newSize <= 0 ? 0 : GetPosition();
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

bool CEFile::PurgeData( void )
{
    bool result = false;
    if (IsOpened() && CanWrite())
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

void CEFile::Flush( void )
{
    if ( mFileHandle != NULL )
    {
        ::FlushFileBuffers(static_cast<HANDLE>(mFileHandle));
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool CEFile::FileDelete( const char* filePath )
{
    return ( NEString::isEmpty<char>(filePath) == false ? ::DeleteFileA(filePath) == TRUE : false );
}

bool CEFile::DirectoryCreate( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::CreateDirectoryA(dirPath, NULL) == TRUE : false );
}

bool CEFile::DirectoryDelete( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::RemoveDirectoryA(dirPath) == TRUE : false );
}

bool CEFile::FileMove( const char* oldPath, const char* newPath )
{
    return ( NEString::isEmpty<char>(oldPath) == false && NEString::isEmpty<char>(newPath) == false ? ::MoveFileA(oldPath, newPath) == TRUE : false );
}

CEString CEFile::GetCurrentDir( void )
{
    CEString result;
    char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];

    if (buffer != NULL)
    {
        result = ::GetCurrentDirectoryA(CEFile::MAXIMUM_PATH, buffer) <= CEFile::MAXIMUM_PATH ? buffer : "";
        delete [] buffer;
    }

    return result;
}

bool CEFile::SetCurrentDir( const char* dirPath )
{
    return ( NEString::isEmpty<char>(dirPath) == false ? ::SetCurrentDirectoryA(dirPath) == TRUE : false );
}

bool CEFile::FileCopy( const char* originPath, const char* newPath, bool copyForce )
{
    return ( NEString::isEmpty<char>(originPath) == false && NEString::isEmpty<char>(newPath) == false  ? 
            ::CopyFileA(originPath, newPath, copyForce ? FALSE : TRUE) == TRUE                                              :
            false );
}

CEString CEFile::GetTempFolder( void )
{
    CEString result;
    char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        result = ::GetTempPathA(CEFile::MAXIMUM_PATH, buffer) <= CEFile::MAXIMUM_PATH ? buffer : "";
        delete [] buffer;
    }

    return result;
}

CEString CEFile::CreateTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    CEString result;
    char * buffer = DEBUG_NEW char [CEFile::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        unsigned int tmpUnique = unique ? 0 : static_cast<unsigned int>( CEDateTime::GetTickCount() );
        prefix = prefix == NULL ? CEFile::TEMP_FILE_PREFIX : prefix;
        CEString folder = inTempFolder ? CEFile::GetTempFolder() : CEFile::GetCurrentDir();
        result = ::GetTempFileNameA(folder, prefix, tmpUnique, buffer) != 0 ? buffer : "";

        delete [] buffer;
    }

    return result;
}

bool CEFile::IsDirectoryExist( const char* dirPath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        unsigned long attr = ::GetFileAttributesA(dirPath);
        result = (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0);
    }
    return result;
}

bool CEFile::IsFileExist( const char* filePath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        unsigned long attr = ::GetFileAttributesA(filePath);
        result = (attr != INVALID_FILE_ATTRIBUTES) && ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
    }
    return result;
}

CEString CEFile::GetFileFullPath( const char* filePath )
{
    CEString result  = filePath;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
        if ( (buffer != NULL) && (::GetFullPathNameA(result, CEFile::MAXIMUM_PATH, buffer, NULL) <= CEFile::MAXIMUM_PATH) )
            result = buffer;

        if (buffer != NULL)
            delete [] buffer;
    }

    return result;
}

bool CEFile::_createFolder( const char * dirPath )
{
    // no need to validate string. Put in assertion.
    ASSERT( NEString::isEmpty<char>(dirPath) == false );
    return (::CreateDirectoryA(dirPath, NULL) == TRUE);
}

#if 0
CEString CEFile::NormalizeFileName( const char * fileName, bool forceTime/*=false*/ )
{
    CEString result;

    SYSTEMTIME st;
    char fmt[32];
    memset(&st, 0, sizeof(SYSTEMTIME));
    memset(fmt, 0, 32);

    GetLocalTime(&st);
    CEString::PrintString(fmt, 32, CEFile::TIMESTAMP_FORMAT, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    if ( NEString::isEmpty<char>(fileName) == false )
    {
        result = fileName;
        NEString::CharPos index = result.FindFirstOf(CEFile::FILE_MASK_TIMESTAMP);
        if ( index != NEString::InvalidPos )
        {
            result.Replace(fmt, index, NEString::getStringLength<char>(CEFile::FILE_MASK_TIMESTAMP) );
        }
        else if (forceTime)
        {
            index = result.FindLastOf('.');
            if (index != NEString::InvalidPos)
                result.Insert(fmt, index);
            else
                result += fmt;
        }

        index = result.FindFirstOf( CEFile::FILE_MASK_APP_DATA );
        if ( index != NEString::InvalidPos )
        {
            CEString appFolder = CEFile::GetSpecialFolder(CEFile::SpecialAppData);
            result.Replace(appFolder, index, NEString::getStringLength<char>(CEFile::FILE_MASK_APP_DATA) );
        }
    }
    else if (forceTime)
    {
        result = fmt;
    }
    return CEFile::NormalizeFilePath(result);
}
#endif // 0

#endif // _WINDOWS
