/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/FilePosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, File object
 *              POSIX specific implementation
 *
 ************************************************************************/

#include "areg/base/File.hpp"

#if defined(_POSIX) || defined(POSIX)


#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/Containers.hpp"

#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< POSIX invalid file descriptor.
    constexpr int   POSIX_INVALID_FD        { -1 };
    constexpr char  DIR_NAME_DOCUMENTS[]    { "Documents" };
    constexpr char  DIR_NAME_APPDATA[]      { "profile" };
    constexpr char  ENV_USER_HOME[]         { "HOME" };
    constexpr char  USER_HOME_DIR[]         { "~" };
    constexpr char  USER_TEMP_DIR[]         { "~/tmp" };

    typedef struct S_PosixFile
    {
        //!< The POSIX file description. Invalid or not used if -1 (POSIX_INVALID_FD)
        int fd  = POSIX_INVALID_FD;
    } sPosixFile;

    //////////////////////////////////////////////////////////////////////////
    // local statics
    //////////////////////////////////////////////////////////////////////////

    inline bool _existFile( const char * filePath )
    {
        return ((NEString::isEmpty<char>( filePath ) == false) && (RETURNED_OK == ::access( filePath, R_OK )));
    }

    inline const char * _getUserHomeDir( void )
    {
        const char *homedir = getenv(ENV_USER_HOME);
        if (homedir == nullptr)
        {
            struct passwd *pw = getpwuid(getuid());
            homedir = pw != nullptr ? pw->pw_dir : USER_HOME_DIR;
        }

        return homedir;
    }

    inline const char * _getTempDir( void )
    {
        const char * tempDir = getenv("TMPDIR");
        if (tempDir == nullptr )
        {
            tempDir = getenv( "TMP" );
            if ( tempDir == nullptr )
            {
                tempDir = getenv( "TEMP" );
            }
        }

        return (tempDir != nullptr ? tempDir : USER_TEMP_DIR);
    }
}

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
const char      File::PATH_SEPARATOR    = '/';
const int       File::MAXIMUM_PATH      = 256;
void * const    File::INVALID_HANDLE    = nullptr;

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

void File::_closeFile( void )
{
    if ( isOpened( ) )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);

        if (file->fd != POSIX_INVALID_FD)
        {
            ::close( file->fd );
        }

        delete file;
    }

    mFileHandle = File::INVALID_HANDLE;
}

bool File::open( void )
{
    sPosixFile * file = nullptr;

    if (isOpened() == false)
    {
        file = DEBUG_NEW sPosixFile;
        if ( (mFileName.isEmpty() == false) && (file != nullptr) )
        {
            mFileMode = normalizeMode(mFileMode);
            int     flag = 0;
            mode_t  mode = 0;

            if (mFileMode & FileBase::FOB_READ)
            {
                flag |= O_RDONLY;
            }

            if (mFileMode & FileBase::FOB_WRITE)
            {
                flag &= ~O_RDONLY;
                flag |= O_RDWR;
            }

            if (mFileMode & FileBase::FOB_CREATE)
            {
                flag |= O_CREAT;
            }

            if (mFileMode & FileBase::FOB_EXIST)
            {
                flag &= ~O_CREAT;
            }

            if ( (mFileMode & FileBase::FOB_TRUNCATE) || ((flag & O_CREAT) != 0) )
            {
                flag |= O_TRUNC;
            }

            if (mFileMode & FileBase::FOB_SHARE_READ)
            {
                // modes |= (S_IRGRP | S_IROTH | S_IRUSR);
                mode |= (S_IRUSR | S_IXUSR) | (S_IRGRP | S_IXGRP) | (S_IROTH | S_IXOTH);
            }

            if (mFileMode & FileBase::FOB_SHARE_WRITE)
            {
                // modes |= (S_IWGRP | S_IWOTH | S_IWUSR);
                mode |= S_IRWXU | S_IRWXG | S_IRWXO;
            }

            // if (mFileMode & FileBase::FOB_TEMP_FILE)
            //     ; // do nothing
            // if (mFileMode & FileBase::FOB_WRITE_DIRECT)
            //     ; // do nothing

            String dirName(File::getFileDirectory(mFileName));

            if ( (flag & O_CREAT) != 0 )
            {
                if (_existFile(mFileName.getString()))
                {
                    flag &= ~O_CREAT;
                    if (dirName == mFileName)
                    {
                        flag |= O_DIRECTORY; // set directory option
                        flag &= ~O_TRUNC;    // remove truncate, since it is not applicable for directories
                    }
                }
                else
                {
                    File::createDirCascaded(dirName);
                    if (dirName == mFileName)
                    {
                        flag |= O_DIRECTORY; // set directory option
                        flag &= ~O_TRUNC;    // remove truncate, since it is not applicable for directories
                        flag &=~ O_CREAT;    // we don't need this, because it is automatically created cascaded
                    }
                }
            }

            if (mFileMode & FileBase::FOB_TEMP_FILE)
            {
                file->fd =  ::mkstemp(mFileName.getBuffer());
            }
            else
            {
                file->fd =  ::open(mFileName.getString(), flag, mode);
            }

            if (file->fd != POSIX_INVALID_FD)
            {
                mFileHandle = static_cast<FILEHANDLE>(file);
            }
            else
            {
                OUTPUT_ERR("Failed to open file [ %s ], errno = [ %p ]", mFileName.getString(), static_cast<id_type>(errno));
                delete file;
                file = nullptr;
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

    return (file != nullptr);
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
            sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);

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
        ASSERT(file != nullptr);

        if ((buffer != nullptr) && (size != 0))
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
        switch (startAt)
        {
        case IECursorPosition::eCursorPosition::PositionBegin:
            result = static_cast<unsigned int>( lseek(file->fd, offset, SEEK_SET) );
            break;

        case IECursorPosition::eCursorPosition::PositionCurrent:
            result = static_cast<unsigned int>( lseek(file->fd, offset, SEEK_CUR) );
            break;

        case IECursorPosition::eCursorPosition::PositionEnd:
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
        unsigned int curPos = static_cast<unsigned int>(lseek(file->fd, 0, SEEK_CUR));
        unsigned int endPos = static_cast<unsigned int>(lseek(file->fd, 0, SEEK_END));
        if (curPos != IECursorPosition::INVALID_CURSOR_POSITION)
        {
            lseek(file->fd, curPos, SEEK_SET);
        }

        result = endPos;
    }

    return result;
}

unsigned int File::reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName.isValid() ? static_cast<const char *>(mFileName) : "null");

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (isOpened() && canWrite())
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
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
        result = RETURNED_OK == ftruncate(file->fd, 0);
    }

    return result;
}

void File::flush( void )
{
    if ( isOpened() )
    {
        sPosixFile * file = reinterpret_cast<sPosixFile *>(mFileHandle);
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
    if (buffer != nullptr)
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

bool File::copyFile( const char* srcPath, const char* newPath, bool copyForce )
{
    static const int BUF_SIZE   = 4 * 1024;

    bool result = false;
    if ( (NEString::isEmpty<char>(srcPath) == false) && (NEString::isEmpty<char>(newPath) == false) )
    {
        mode_t mode = 0;
        int flag    = 0;
        struct stat buf;
        if ( RETURNED_OK == stat(newPath, &buf) )
        {
            if (S_ISREG(buf.st_mode) && copyForce)
            {
                // keep current mode, add read-write permission for owner and group
                mode = (buf.st_mode & (~S_IFREG)) | S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
                flag = buf.st_size != 0 ? O_WRONLY | O_TRUNC : O_WRONLY; // for file with zero size, there is no need to truncate.
            }
        }
        else
        {
            // set default mode rw-rw-r--
            mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
            flag = O_WRONLY | O_CREAT | O_TRUNC;
            if (copyForce)
            {
                String dir = File::getFileDirectory(newPath);
                File::createDirCascaded(dir);
            }
        }

        if (mode != 0)
        {
            int fdRead = ::open(srcPath, O_RDONLY);
            int fdWrite= ::open(newPath, flag, mode);
            unsigned char * buffer = DEBUG_NEW unsigned char[BUF_SIZE];

            if (buffer != nullptr)
            {
                if ( (fdRead != POSIX_INVALID_FD) && (fdWrite != POSIX_INVALID_FD) )
                {
                    int readSize = 0;
                    result = true;
                    while ( (readSize = ::read( fdRead, buffer, BUF_SIZE )) > 0 )
                    {
                        if ( ::write( fdWrite, buffer, readSize ) < 0 )
                        {
                            OUTPUT_ERR( "Failed to copy [ %s ] into [ %s ], error code [ %p ]", srcPath, newPath, static_cast<id_type>(errno) );
                            result = false;
                            break;
                        }
                    }
                }
                else
                {
                    OUTPUT_ERR( "Error during copying file. Read fd [ %d ], write fd [ %d ], error [ %p ]", buffer, fdRead, fdWrite, static_cast<id_type>(errno) );
                }

                if ( fdRead != POSIX_INVALID_FD )
                {
                    ::close( fdRead );
                }

                if ( fdWrite != POSIX_INVALID_FD )
                {
                    ::close( fdWrite );
                }

                delete [] buffer;
            }
            else
            {
                OUTPUT_ERR("Failed to allocate buffer to copy files.");
            }
        }
    }

    return result;
}

String File::getTempDir( void )
{
    char const * dirTemp = getenv("TMPDIR");
    return String( dirTemp != nullptr ? dirTemp : "/tmp");
}

String File::genTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    String result;
    char * buffer = DEBUG_NEW char[File::MAXIMUM_PATH + 1];

    if (buffer != nullptr)
    {
        unsigned int tmpUnique  = unique ? 0 : static_cast<unsigned int>( DateTime::getSystemTickCount() );
        prefix = prefix == nullptr ? File::TEMP_FILE_PREFIX.data() : prefix;
        String folder = inTempFolder ? File::getTempDir() : File::getCurrentDir();
        if (folder.isEmpty() == false)
        {
            folder += File::PATH_SEPARATOR;
        }

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
    String result{filePath};
    if ( result.isEmpty() == false )
    {
        char * pathCanonical  = realpath(result.getString(), nullptr);
        if (pathCanonical != nullptr)
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
    // mode_t mode = S_IRGRP | S_IROTH | S_IRUSR | S_IWGRP | S_IWOTH | S_IWUSR;
    mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;

    return ( RETURNED_OK ==  mkdir(dirPath, mode) );
}

String File::getSpecialDir(eSpecialFolder specialFolder)
{
    String result;

    char * buffer = DEBUG_NEW char [File::MAXIMUM_PATH + 1];
    const char * filePath   = nullptr;

    if (buffer != nullptr)
    {
        buffer[0] = '\0';

        switch ( specialFolder )
        {
        case File::eSpecialFolder::SpecialUserHome:
            filePath = _getUserHomeDir();
            ASSERT(filePath != nullptr);
            sprintf( buffer, "%s", filePath != nullptr ? filePath : "~" );
            break;

        case File::eSpecialFolder::SpecialPersonal:
            filePath = _getUserHomeDir();
            ASSERT( filePath != nullptr );
            sprintf( buffer, "%s%c%s", filePath != nullptr ? filePath : "", File::PATH_SEPARATOR, DIR_NAME_DOCUMENTS );
            break;

        case File::eSpecialFolder::SpecialAppData:
            filePath = _getUserHomeDir();
            ASSERT(filePath != nullptr);
            sprintf(  buffer, "%s%c.%s%c%s"
                    , filePath
                    , static_cast<int>(File::PATH_SEPARATOR)
                    , Process::getInstance().getAppName().getString()
                    , static_cast<int>(File::PATH_SEPARATOR)
                    , DIR_NAME_APPDATA);
            break;

        case File::eSpecialFolder::SpecialTemp:
            filePath = _getTempDir();
            ASSERT(filePath != nullptr );
            sprintf( buffer, "%s", filePath );
            break;

        default:
            break;
        }

        result = buffer;
        delete [] buffer;
    }

    return result;
}

#endif //  defined(_POSIX) || defined(POSIX)
