/************************************************************************
 * \file        areg/src/base/private/posix/CEFilePosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object
 *              POSIX specific implementation
 *
 ************************************************************************/

#include "areg/src/base/CEFile.hpp"

#ifdef  _POSIX


#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CEDateTime.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/EContainers.hpp"

#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//////////////////////////////////////////////////////////////////////////
// CEFile class implementation
//////////////////////////////////////////////////////////////////////////

#define POSIX_INVALID_FD        -1

typedef struct S_PosixFile
{
    inline S_PosixFile( void )
        : fd (POSIX_INVALID_FD)
    {    }

    int     fd;     //<! The POSIX file description. Invalid or not used if -1 (POSIX_INVALID_FD)
} sPosixFile;

//////////////////////////////////////////////////////////////////////////
// local statics
//////////////////////////////////////////////////////////////////////////

static inline bool fileExist( const char * filePath )
{
    return ( (NEString::isEmpty<char>(filePath) == false) && (RETURNED_OK == access(filePath, R_OK)) );
}

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const char          CEFile::PATH_SEPARATOR      = '/';
const int           CEFile::MAXIMUM_PATH        = _POSIX_PATH_MAX;
void * const        CEFile::INVALID_HANDLE      = static_cast<void *>(NULL);

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void CEFile::_closeFile( void )
{
    if ( IsOpened( ) )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);
        if (file->fd != POSIX_INVALID_FD)
            close(file->fd);

        delete file;
    }

    mFileHandle = CEFile::INVALID_HANDLE;
}

bool CEFile::Open( void )
{
    sPosixFile * file = NULL;

    if (IsOpened() == false)
    {
        file = DEBUG_NEW sPosixFile;
        if ( (mFileName.IsEmpty() == false) && (file != NULL) )
        {
            mFileMode = NormalizeMode(mFileMode);
            int     options = 0;
            mode_t  modes   = 0;

            if (mFileMode & CEFileBase::FOB_READ)
                options |= O_RDONLY;

            if (mFileMode & CEFileBase::FOB_WRITE)
                options |= O_RDWR;

            if (mFileMode & CEFileBase::FOB_SHARE_READ) 
                modes |= (S_IRGRP | S_IROTH | S_IRUSR);

            if (mFileMode & CEFileBase::FOB_SHARE_WRITE)
                modes |= (S_IWGRP | S_IWOTH | S_IWUSR);

            if (mFileMode & CEFileBase::FOB_CREATE)
                options |= O_CREAT;

            if (mFileMode & CEFileBase::FOB_EXIST)
                options &= ~O_CREAT;

            if (mFileMode & CEFileBase::FOB_TRUNCATE)
                options |= O_TRUNC;

            // if (mFileMode & CEFileBase::FOB_TEMP_FILE)
            //     ; // do nothing
            // if (mFileMode & CEFileBase::FOB_WRITE_DIRECT)
            //     ; // do nothing

            CEString dirName = CEFile::GetFileDirectory( static_cast<const char *>(mFileName));

            if ( options & O_CREAT )
            {
                if (fileExist(mFileName.GetBuffer()))
                {
                    options &= ~O_CREAT;
                    if (dirName == mFileName)
                    {
                        options |= O_DIRECTORY; // set directory option
                        options &= ~O_TRUNC;    // remove truncate, since it is not applicable for directories
                    }
                }
                else
                {
                    CEFile::CreateFoldersCascaded(dirName);
                    if (dirName == mFileName)
                    {
                        options |= O_DIRECTORY; // set directory option
                        options &= ~O_TRUNC;    // remove truncate, since it is not applicable for directories
                        options &=~ O_CREAT;    // we don't need this, because it is automatically created cascaded
                    }
                }
            }

            file->fd = (mFileMode & CEFileBase::FOB_TEMP_FILE) ? mkstemp(mFileName.UnsafeBuffer()) : open(mFileName.GetBuffer(), options, modes);
            if (file->fd != POSIX_INVALID_FD)
            {
                mFileHandle = static_cast<void *>(file);
            }
            else
            {
                OUTPUT_ERR("Failed to open file [ %s ], errno = [ %p ]", mFileName.GetBuffer(), errno);
                delete file;
                file = NULL;
            }
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

    return (file != NULL);
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
            sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
            ASSERT(file != NULL);

            ssize_t sizeRead = read(file->fd, buffer, size);
            if (sizeRead > 0)
            {
                result = sizeRead;
            }
            else
            {
                OUTPUT_ERR("Failed read file [ %s ], error code [ %p ].", mFileName, errno);

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
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        int sizeReserve = 0;
        int sizeRead    = 0;
        if ( (read(file->fd, &sizeReserve, sizeof(int)) == sizeof(int)) && (sizeReserve > 0) )
        {
            sizeRead = buffer.ResizeBuffer(sizeReserve, false);
            unsigned char * data = sizeRead != 0 ? buffer.GetBuffer() : NULL;
            if ( (data != NULL) && (read(file->fd, data, sizeRead) == sizeRead))
            {
                result = sizeRead + sizeof(int);
            }
            else
            {
                OUTPUT_ERR("Either was not able to reserve [ %d ] bytes of space, or failed read file [ %s ], error code [ %p ].", sizeReserve, mFileName.GetBuffer(), errno);
            }
        }
        else
        {
            OUTPUT_ERR("Unable to read the size of byte-buffer, the file [ %s ], error code [ %p ]", mFileName.GetBuffer(), errno);
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
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        const unsigned char * data  = buffer.GetBuffer();
        int sizeUsed                = static_cast<int>(buffer.GetUsedSize());

        if ( (write(file->fd, &sizeUsed, sizeof(int)) == sizeof(int)) && (write(file->fd, data, sizeUsed) == sizeUsed) )
        {
            result = sizeUsed + sizeof(int);
        }
        else
        {
            OUTPUT_ERR("Failed to write [ %d ] bytes of data in file [ %s ], error code [ %p ]", sizeUsed + sizeof(int), mFileName.GetBuffer(), errno);
        }
    }
    else
    {
        OUTPUT_ERR("Either files [ %s ] is not opened or it is not opened to write file, error code [ %p ]", mFileName.GetBuffer(), errno);
    }

    return result;
}

unsigned int CEFile::Write(const unsigned char* buffer, unsigned int size)
{
    int result = 0;
    if ( IsOpened() && CanWrite() )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        if ((buffer != NULL) && (size != 0))
        {
            if ( (result = write(file->fd, buffer, size)) != static_cast<int>(size) )
            {
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.GetBuffer(), errno);
                result = 0;
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
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (IsOpened())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);
        switch (startAt)
        {
        case IECursorPosition::POSITION_BEGIN:
            result = static_cast<unsigned int>( lseek(file->fd, offset, SEEK_SET) );
            break;

        case IECursorPosition::POSITION_CURRENT:
            result = static_cast<unsigned int>( lseek(file->fd, offset, SEEK_CUR) );
            break;

        case IECursorPosition::POSITION_END:
            result = static_cast<unsigned int>( lseek(file->fd, offset, SEEK_END));
            break;

        default:
            OUTPUT_ERR("Unexpected cursor position value [ %d ]!", startAt);
            break;
        }
    }

    return result;
}

unsigned int CEFile::GetPosition( void ) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (IsOpened())
    {
        sPosixFile *file = reinterpret_cast<sPosixFile *>(mFileHandle);
        result = lseek(file->fd, 0, SEEK_CUR);
    }

    return result;
}

unsigned int CEFile::GetLength( void ) const
{
    unsigned int result = 0;
    if (IsOpened())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);
        unsigned int curPos = static_cast<unsigned int>(lseek(file->fd, 0, SEEK_CUR));
        unsigned int endPos = static_cast<unsigned int>(lseek(file->fd, 0, SEEK_END));
        if (curPos != IECursorPosition::INVALID_CURSOR_POSITION)
        {
            lseek(file->fd, curPos, SEEK_SET);
        }

        result = endPos;
    }
    /**********************************************
    struct stat buf;
    if (RETURNED_OK == stat(mFileName.GetBuffer(), &buf))
    {
        result = static_cast<unsigned int>(buf.st_size);
    }
    **********************************************/

    return result;
}

unsigned int CEFile::Reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName.IsValid() ? static_cast<const char *>(mFileName) : "NULL");

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (IsOpened() && CanWrite())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);
        unsigned int setSize= newSize > 0 ? newSize : 0;
        unsigned int curPos = lseek(file->fd, 0, SEEK_CUR);
        unsigned int oldSize= lseek(file->fd, 0, SEEK_END);

        // move begin
        lseek(file->fd, 0, SEEK_SET);

        result = curPos;
        if ( RETURNED_OK != ftruncate(file->fd, setSize) )
        {
            setSize = oldSize;
            result  = IECursorPosition::INVALID_CURSOR_POSITION;
        }

        if (curPos > setSize)
        {
            lseek(file->fd, 0, SEEK_END);
            result = setSize;
        }
        else if (curPos != 0)
        {
            lseek(file->fd, curPos, SEEK_SET);
        }
    }

    return result;
}

bool CEFile::PurgeData( void )
{
    bool result = false;
    if (IsOpened() && CanWrite())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        result = RETURNED_OK == ftruncate(file->fd, 0);
    }

    return result;
}

void CEFile::Flush( void )
{
    if ( IsOpened() )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        fsync(file->fd);
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool CEFile::FileDelete( const char* filePath )
{
    return ( (NEString::isEmpty<char>(filePath) == false) && (RETURNED_OK == unlink(filePath)) );
}

bool CEFile::DirectoryCreate( const char* dirPath )
{
    return ( (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == mkdir(dirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) );
}

bool CEFile::DirectoryDelete( const char* dirPath )
{
    return (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == rmdir(dirPath));
}

bool CEFile::FileMove( const char* oldPath, const char* newPath )
{
    return ( (NEString::isEmpty<char>(oldPath) == false) && (NEString::isEmpty<char>(newPath) == false) && (RETURNED_OK == rename(oldPath, newPath)) );
}

CEString CEFile::GetCurrentDir( void )
{
    CEString result;
    char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        result = static_cast<const char *>(getcwd(buffer, CEFile::MAXIMUM_PATH));
        delete [] buffer;
    }

    return result;
}

bool CEFile::SetCurrentDir( const char* dirPath )
{
    return ( (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == chdir(dirPath)) );
}

bool CEFile::FileCopy( const char* originPath, const char* newPath, bool copyForce )
{
    static const int BUF_SIZE   = 4 * 1024;

    bool result = false;
    if ( (NEString::isEmpty<char>(originPath) == false) && (NEString::isEmpty<char>(newPath) == false) )
    {
        mode_t modes= 0;
        int options = 0;
        struct stat buf;
        if ( RETURNED_OK == stat(newPath, &buf) )
        {
            if (S_ISREG(buf.st_mode) && copyForce)
            {
                // keep current mode, add read-write permission for owner and group
                modes   = (buf.st_mode & (~S_IFREG)) | S_IWUSR | S_IRUSR | S_IRGRP | S_IWGRP;
                options = buf.st_size != 0 ? O_WRONLY | O_TRUNC : O_WRONLY; // for file with zero size, there is no need to truncate.
            }
        }
        else
        {
            // set default mode rw-rw-rw-
            modes   = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
            options = O_WRONLY | O_CREAT;
            if (copyForce)
            {
                CEString dir = CEFile::GetFileDirectory(newPath);
                CEFile::CreateFoldersCascaded(dir);
            }
        }

        if (modes != 0)
        {
            int fdRead = open(originPath, O_RDONLY);
            int fdWrite= open(newPath, options, modes);
            unsigned char * buffer = DEBUG_NEW unsigned char[BUF_SIZE];

            if ((buffer != NULL) && (fdRead != POSIX_INVALID_FD) && (fdWrite != POSIX_INVALID_FD))
            {
                int readSize = 0;
                result = true;
                while ( (readSize = read(fdRead, buffer, BUF_SIZE)) > 0 )
                {
                    if (write(fdWrite, buffer, readSize) < 0)
                    {
                        OUTPUT_ERR("Failed to copy [ %s ] into [ %s ], error code [ %p ]", originPath, newPath, errno);
                        result = false;
                        break;
                    }
                }
            }
            else
            {
                OUTPUT_ERR("Error during copying file. Allocated buffer address [ %p ], read fd [ %d ], write fd [ %d ], error [ %p ]", buffer, fdRead, fdWrite, errno);
            }

            if (buffer != NULL)
                delete [] buffer;

            if (fdRead != POSIX_INVALID_FD)
                close(fdRead);

            if (fdWrite != POSIX_INVALID_FD)
                close(fdWrite);
        }
    }

    return result;
}

CEString CEFile::GetTempFolder( void )
{
    char const * dirTemp = getenv("TMPDIR");
    return CEString( dirTemp != NULL ? dirTemp : "/tmp");
}

CEString CEFile::CreateTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    CEString result;
    char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];

    if (buffer != NULL)
    {
        unsigned int tmpUnique  = unique ? 0 : static_cast<unsigned int>( CEDateTime::GetTickCount() );
        prefix = prefix == NULL ? CEFile::TEMP_FILE_PREFIX : prefix;
        CEString folder = inTempFolder ? CEFile::GetTempFolder() : CEFile::GetCurrentDir();
        if (folder.IsEmpty() == false)
            folder += "/";

        sprintf(buffer, "%s%s%dXXXXXX.tmp", folder.IsEmpty() == false ? folder.GetBuffer() : "", prefix, tmpUnique);
        int fd = mkstemp(buffer);

        result = static_cast<const char *>(fd != POSIX_INVALID_FD ? buffer : prefix);

        delete [] buffer;
    }

    return result;
}

bool CEFile::IsDirectoryExist( const char* dirPath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        struct stat buf;
        if (RETURNED_OK == stat(dirPath, &buf))
        {
            result = S_ISDIR(buf.st_mode);
        }
    }
    
    return result;
}

bool CEFile::IsFileExist( const char* filePath )
{
    return fileExist(filePath);
}

CEString CEFile::GetFileFullPath( const char* filePath )
{
    CEString result  = filePath;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        char * buffer = DEBUG_NEW char[CEFile::MAXIMUM_PATH + 1];
        if (buffer != NULL)
        {
            const char * temp   = realpath(filePath, buffer);
            if (temp != NULL)
            {
                result = temp;
            }
            else
            {
                OUTPUT_ERR("Could not retrieve the full path of file [ %s ], error code [ %p ]", filePath, errno);
            }

            delete [] buffer;
        }
    }

    return result;
}

bool CEFile::_createFolder( const char * dirPath )
{
    // no need to validate string. Put in assertion.
    ASSERT( NEString::isEmpty<char>(dirPath) == false );
    return ( RETURNED_OK ==  mkdir(dirPath, S_IRGRP | S_IROTH | S_IRUSR | S_IWGRP | S_IWOTH | S_IWUSR) );
}

CEString CEFile::GetSpecialFolder(eSpecialFolder specialFolder)
{

    char * buffer = DEBUG_NEW char [CEFile::MAXIMUM_PATH + 1];
    const char * filePath   = NULL;

    if (buffer != NULL)
    {
        switch ( specialFolder )
        {
        case CEFile::SpecialUserHome:
            filePath = getenv("HOME");
            if (filePath != NULL)
                sprintf(buffer, "%s", filePath != NULL ? filePath : "");
            break;

        case CEFile::SpecialPersonal:
            filePath = getenv("HOME");
            sprintf(buffer, "%s%c%s", filePath != NULL ? filePath : "", CEFile::PATH_SEPARATOR, "docs");
            break;

        case CEFile::SpecialAppData:
            filePath = getenv("HOME");
            if (filePath != NULL)
            {
                sprintf(  buffer, "%s%c.%s%cprofiles"
                    , filePath
                    , static_cast<int>(CEFile::PATH_SEPARATOR)
                    , CEProcess::GetProcess().GetAppName()
                    , static_cast<int>(CEFile::PATH_SEPARATOR));
            }
            break;

        case CEFile::SpecialTemp:
            filePath = getenv("TMPDIR");
            if (filePath == NULL)
                filePath = getenv("TMP");
            if (filePath == NULL)
                filePath = getenv("TEMP");

            if (filePath != NULL)
                sprintf(buffer, "%s", filePath);
            break;

        default:
            break;
        }

        delete [] buffer;
    }

    return CEString(filePath);
}

#endif // _POSIX
