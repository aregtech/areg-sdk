/************************************************************************
 * \file        areg/base/private/posix/FilePosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object
 *              POSIX specific implementation
 *
 ************************************************************************/

#include "areg/base/File.hpp"

#ifdef  _POSIX


#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/EContainers.hpp"

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
// File class implementation
//////////////////////////////////////////////////////////////////////////

#define POSIX_INVALID_FD        -1

typedef struct S_PosixFile
{
    inline S_PosixFile( void )
        : fd (POSIX_INVALID_FD)
    {    }

    int     fd;     //!< The POSIX file description. Invalid or not used if -1 (POSIX_INVALID_FD)
} sPosixFile;

//////////////////////////////////////////////////////////////////////////
// local statics
//////////////////////////////////////////////////////////////////////////

static inline bool _existFile( const char * filePath )
{
    return ( (NEString::isEmpty<char>(filePath) == false) && (RETURNED_OK == ::access(filePath, R_OK)) );
}

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const char      File::PATH_SEPARATOR    = '/';
const int       File::MAXIMUM_PATH      = _POSIX_PATH_MAX;
void * const    File::INVALID_HANDLE    = static_cast<void *>(NULL);

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void File::_closeFile( void )
{
    if ( isOpened( ) )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);
        if (file->fd != POSIX_INVALID_FD)
            ::close(file->fd);

        delete file;
    }

    mFileHandle = File::INVALID_HANDLE;
}

bool File::open( void )
{
    sPosixFile * file = NULL;

    if (isOpened() == false)
    {
        file = DEBUG_NEW sPosixFile;
        if ( (mFileName.isEmpty() == false) && (file != NULL) )
        {
            mFileMode = normalizeMode(mFileMode);
            int     options = 0;
            mode_t  modes   = 0;

            if (mFileMode & FileBase::FOB_READ)
                options |= O_RDONLY;

            if (mFileMode & FileBase::FOB_WRITE)
                options |= O_RDWR;

            if (mFileMode & FileBase::FOB_SHARE_READ) 
                modes |= (S_IRGRP | S_IROTH | S_IRUSR);

            if (mFileMode & FileBase::FOB_SHARE_WRITE)
                modes |= (S_IWGRP | S_IWOTH | S_IWUSR);

            if (mFileMode & FileBase::FOB_CREATE)
                options |= O_CREAT;

            if (mFileMode & FileBase::FOB_EXIST)
                options &= ~O_CREAT;

            if (mFileMode & FileBase::FOB_TRUNCATE)
                options |= O_TRUNC;

            // if (mFileMode & FileBase::FOB_TEMP_FILE)
            //     ; // do nothing
            // if (mFileMode & FileBase::FOB_WRITE_DIRECT)
            //     ; // do nothing

            String dirName = File::getFileDirectory( static_cast<const char *>(mFileName));

            if ( options & O_CREAT )
            {
                if (_existFile(mFileName.getString()))
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
                    File::createDirCascaded(dirName);
                    if (dirName == mFileName)
                    {
                        options |= O_DIRECTORY; // set directory option
                        options &= ~O_TRUNC;    // remove truncate, since it is not applicable for directories
                        options &=~ O_CREAT;    // we don't need this, because it is automatically created cascaded
                    }
                }
            }

            file->fd = (mFileMode & FileBase::FOB_TEMP_FILE) ? ::mkstemp(mFileName.getUnsafeBuffer()) : ::open(mFileName.getString(), options, modes);
            if (file->fd != POSIX_INVALID_FD)
            {
                mFileHandle = static_cast<void *>(file);
            }
            else
            {
                OUTPUT_ERR("Failed to open file [ %s ], errno = [ %p ]", mFileName.getString(), static_cast<id_type>(errno));
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
            sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
            ASSERT(file != NULL);

            ssize_t sizeRead = ::read(file->fd, buffer, size);
            if (sizeRead > 0)
            {
                result = sizeRead;
            }
#ifdef  _DEBUG
            else if (sizeRead < 0)
            {
                OUTPUT_ERR("Failed read file [ %s ], error code [ %p ].", mFileName.getString(), static_cast<id_type>(errno));

            }
            else
            {
                OUTPUT_DBG("Finished to read file [ %s ]", mFileName.getString());
            }
#endif  // !_DEBUG
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
    int result = 0;
    if ( isOpened() && canWrite() )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        if ((buffer != NULL) && (size != 0))
        {
            if ( (result = ::write(file->fd, buffer, size)) != static_cast<int>(size) )
            {
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.getString(), static_cast<id_type>(errno));
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
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or writting mode is not set (mode = [ %d ]).", mFileName.getString(), isOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int File::setPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    if (isOpened())
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

unsigned int File::getPosition( void ) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (isOpened())
    {
        sPosixFile *file = reinterpret_cast<sPosixFile *>(mFileHandle);
        result = lseek(file->fd, 0, SEEK_CUR);
    }

    return result;
}

unsigned int File::getLength( void ) const
{
    unsigned int result = 0;
    if (isOpened())
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
    if (RETURNED_OK == stat(mFileName.getString(), &buf))
    {
        result = static_cast<unsigned int>(buf.st_size);
    }
    **********************************************/

    return result;
}

unsigned int File::reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName.isValid() ? static_cast<const char *>(mFileName) : "NULL");

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (isOpened() && canWrite())
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

bool File::truncate( void )
{
    bool result = false;
    if (isOpened() && canWrite())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        result = RETURNED_OK == ftruncate(file->fd, 0);
    }

    return result;
}

void File::flush( void )
{
    if ( isOpened() )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
        ASSERT(file != NULL);

        fsync(file->fd);
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::deleteFile( const char* filePath )
{
    return ( (NEString::isEmpty<char>(filePath) == false) && (RETURNED_OK == unlink(filePath)) );
}

bool File::createDir( const char* dirPath )
{
    return ( (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == mkdir(dirPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) );
}

bool File::deleteDir( const char* dirPath )
{
    return (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == rmdir(dirPath));
}

bool File::moveFile( const char* oldPath, const char* newPath )
{
    return ( (NEString::isEmpty<char>(oldPath) == false) && (NEString::isEmpty<char>(newPath) == false) && (RETURNED_OK == rename(oldPath, newPath)) );
}

String File::getCurrentDir( void )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];
    if (buffer != NULL)
    {
        result = static_cast<const char *>(getcwd(buffer, File::MAXIMUM_PATH));
        delete [] buffer;
    }

    return result;
}

bool File::setCurrentDir( const char* dirPath )
{
    return ( (NEString::isEmpty<char>(dirPath) == false) && (RETURNED_OK == chdir(dirPath)) );
}

bool File::copyFile( const char* originPath, const char* newPath, bool copyForce )
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
                String dir = File::getFileDirectory(newPath);
                File::createDirCascaded(dir);
            }
        }

        if (modes != 0)
        {
            int fdRead = ::open(originPath, O_RDONLY);
            int fdWrite= ::open(newPath, options, modes);
            unsigned char * buffer = DEBUG_NEW unsigned char[BUF_SIZE];

            if ((buffer != NULL) && (fdRead != POSIX_INVALID_FD) && (fdWrite != POSIX_INVALID_FD))
            {
                int readSize = 0;
                result = true;
                while ( (readSize = ::read(fdRead, buffer, BUF_SIZE)) > 0 )
                {
                    if (::write(fdWrite, buffer, readSize) < 0)
                    {
                        OUTPUT_ERR("Failed to copy [ %s ] into [ %s ], error code [ %p ]", originPath, newPath, static_cast<id_type>(errno));
                        result = false;
                        break;
                    }
                }
            }
            else
            {
                OUTPUT_ERR("Error during copying file. Allocated buffer address [ %p ], read fd [ %d ], write fd [ %d ], error [ %p ]", buffer, fdRead, fdWrite, static_cast<id_type>(errno));
            }

            if (buffer != NULL)
                delete [] buffer;

            if (fdRead != POSIX_INVALID_FD)
                ::close(fdRead);

            if (fdWrite != POSIX_INVALID_FD)
                ::close(fdWrite);
        }
    }

    return result;
}

String File::getTempDir( void )
{
    char const * dirTemp = getenv("TMPDIR");
    return String( dirTemp != NULL ? dirTemp : "/tmp");
}

String File::genTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];

    if (buffer != NULL)
    {
        unsigned int tmpUnique  = unique ? 0 : static_cast<unsigned int>( DateTime::getSystemTickCount() );
        prefix = prefix == NULL ? File::TEMP_FILE_PREFIX : prefix;
        String folder = inTempFolder ? File::getTempDir() : File::getCurrentDir();
        if (folder.isEmpty() == false)
            folder += "/";

        sprintf(buffer, "%s%s%dXXXXXX.tmp", folder.isEmpty() == false ? folder.getString() : "", prefix, tmpUnique);
        int fd = mkstemp(buffer);

        result = static_cast<const char *>(fd != POSIX_INVALID_FD ? buffer : prefix);

        delete [] buffer;
    }

    return result;
}

bool File::existDir( const char* dirPath )
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

bool File::existFile( const char* filePath )
{
    return _existFile(filePath);
}

String File::getFileFullPath( const char* filePath )
{
    String result  = filePath;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        char * pathCanonical  = realpath(filePath, NULL);
        if (pathCanonical != NULL)
        {
            result = pathCanonical;
            free(pathCanonical);
        }
        else
        {
            OUTPUT_ERR("Could not retrieve the full path of file [ %s ], error code [ %p ]", filePath, static_cast<id_type>(errno));
        }
    }

    return result;
}

bool File::_createFolder( const char * dirPath )
{
    // no need to validate string. Put in assertion.
    ASSERT( NEString::isEmpty<char>(dirPath) == false );
    return ( RETURNED_OK ==  mkdir(dirPath, S_IRGRP | S_IROTH | S_IRUSR | S_IWGRP | S_IWOTH | S_IWUSR) );
}

String File::getSpecialDir(eSpecialFolder specialFolder)
{

    char * buffer = DEBUG_NEW char [File::MAXIMUM_PATH + 1];
    const char * filePath   = NULL;

    if (buffer != NULL)
    {
        switch ( specialFolder )
        {
        case File::SpecialUserHome:
            filePath = getenv("HOME");
            if (filePath != NULL)
                sprintf(buffer, "%s", filePath != NULL ? filePath : "");
            break;

        case File::SpecialPersonal:
            filePath = getenv("HOME");
            sprintf(buffer, "%s%c%s", filePath != NULL ? filePath : "", File::PATH_SEPARATOR, "docs");
            break;

        case File::SpecialAppData:
            filePath = getenv("HOME");
            if (filePath != NULL)
            {
                sprintf(  buffer, "%s%c.%s%cprofiles"
                    , filePath
                    , static_cast<int>(File::PATH_SEPARATOR)
                    , Process::getInstance().getAppName()
                    , static_cast<int>(File::PATH_SEPARATOR));
            }
            break;

        case File::SpecialTemp:
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

    return String(filePath);
}

#endif // _POSIX
