/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/posix/FilePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, File object
 *              POSIX specific implementation
 *
 ************************************************************************/

#include "areg/base/File.hpp"

#if defined(_POSIX) || defined(POSIX)


#include "areg/base/SharedBuffer.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/Containers.hpp"

#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <filesystem>

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

    struct PosixFile
    {
        //!< The POSIX file description. Invalid or not used if -1 (POSIX_INVALID_FD)
        int fd  = POSIX_INVALID_FD;
    };

    //////////////////////////////////////////////////////////////////////////
    // local statics
    //////////////////////////////////////////////////////////////////////////

    inline const char * _getUserHomeDir()
    {
        const char *homedir = getenv(ENV_USER_HOME);
        if (homedir == nullptr)
        {
            struct passwd *pw = ::getpwuid(getuid());
            homedir = pw != nullptr ? pw->pw_dir : USER_HOME_DIR;
        }

        return homedir;
    }

    inline const char * _getTempDir()
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
// Methods
//////////////////////////////////////////////////////////////////////////

FILEHANDLE File::_osGetInvalidHandle()
{
    return static_cast<FILEHANDLE>(nullptr);
}

void File::_osCloseFile()
{
    if ( isOpened( ) )
    {
        PosixFile * file = reinterpret_cast<PosixFile *>(mFileHandle);

        if (file->fd != POSIX_INVALID_FD)
        {
            ::close( file->fd );
        }

        delete file;
    }

    mFileHandle = File::_osGetInvalidHandle();
}

bool File::_osOpenFile()
{
    PosixFile * file = nullptr;

    if (isOpened() == false)
    {
        std::error_code err;
        file = DEBUG_NEW PosixFile;
        if ( (mFileName.isEmpty() == false) && (file != nullptr) )
        {
            mFileMode = normalizeMode(mFileMode);
            int     flag = 0;
            mode_t  mode = 0;

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitRead)) != 0)
            {
                flag |= O_RDONLY;
            }

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitWrite)) != 0)
            {
                flag &= ~O_RDONLY;
                flag |= O_RDWR;
            }

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitCreate)) != 0)
            {
                flag |= O_CREAT;
            }

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitExist)) != 0)
            {
                flag &= ~O_CREAT;
            }

            if ( (mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTruncate)) || ((flag & O_CREAT) != 0) )
            {
                flag |= O_TRUNC;
            }

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareRead)) != 0)
            {
                mode |= (S_IRUSR | S_IRGRP | S_IROTH);
            }

            if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareWrite)) != 0)
            {
                mode |= (S_IWUSR | S_IRUSR) | (S_IRGRP | S_IRGRP) | (S_IROTH | S_IWOTH);
            }
            else if (((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitCreate)) != 0) ||
                     ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTruncate)) != 0))
            {
                mode |= (S_IWUSR | S_IRUSR) | (S_IRGRP | S_IRGRP) | (S_IROTH | S_IWOTH);
            }

            areg::String dirName(File::getFileDirectory(mFileName));
            if ( (flag & O_CREAT) != 0 )
            {
                if (std::filesystem::exists(mFileName.getData(), err))
                {
                    if (dirName == mFileName)
                    {
                        flag &= ~O_CREAT;   // remove create flag for directories
                        flag &= ~O_TRUNC;   // remove truncate, since it is not applicable for directories
                        flag |= O_DIRECTORY;// set directory option
                    }
                    else
                    {
                        flag |= O_TRUNC;
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

            if (mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTemp))
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
                AREG_OUTPUT_ERR("Failed to open file [ %s ], errno = [ %p ]", mFileName.getString(), static_cast<id_type>(errno));
                delete file;
                file = nullptr;
            }
        }
        else
        {
            AREG_OUTPUT_ERR("Either file name or file open mode is not set.");
        }
    }
    else
    {
        AREG_OUTPUT_WARN("File is already opened. Close file.");
    }

    return (file != nullptr);
}

uint32_t File::_osReadFile(uint8_t* buffer, uint32_t size) const
{
    ASSERT(mFileHandle != nullptr);
    ASSERT((buffer != nullptr) && (size > 0));

    uint32_t result{ 0 };
    ssize_t sizeRead = ::read(reinterpret_cast<PosixFile*>(mFileHandle)->fd, buffer, size);
    if (sizeRead > 0)
    {
        result = static_cast<uint32_t>(sizeRead);
    }
#ifdef  _DEBUG
    else if (sizeRead < 0)
    {
        AREG_OUTPUT_ERR("Failed read file [ %s ], error code [ %p ].", mFileName.getString(), static_cast<id_type>(errno));
    }
    else
    {
        AREG_OUTPUT_DBG("Finished to read file [ %s ]", mFileName.getString());
    }
#endif  // !_DEBUG

    return result;
}

uint32_t File::_osWriteFile(const uint8_t* buffer, uint32_t size)
{
    ASSERT(mFileHandle != nullptr);
    ASSERT((buffer != nullptr) && (size != 0));

    int32_t result = ::write(reinterpret_cast<PosixFile*>(mFileHandle)->fd, buffer, size);
    if (result != static_cast<int32_t>(size))
    {
        AREG_OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.getString(), static_cast<id_type>(errno));
        result = 0;
    }

    return result;
}

uint32_t File::_osSetPositionFile(int32_t offset, Cursor::SeekOrigin startAt) const
{
    ASSERT(mFileHandle != nullptr);
    uint32_t result = Cursor::INVALID_CURSOR_POSITION;

    PosixFile* file = reinterpret_cast<PosixFile*>(mFileHandle);
    switch (startAt)
    {
    case Cursor::SeekOrigin::Begin:
        result = static_cast<uint32_t>(lseek(file->fd, offset, SEEK_SET));
        break;

    case Cursor::SeekOrigin::Current:
        result = static_cast<uint32_t>(lseek(file->fd, offset, SEEK_CUR));
        break;

    case Cursor::SeekOrigin::End:
        result = static_cast<uint32_t>(lseek(file->fd, offset, SEEK_END));
        break;

    default:
        AREG_OUTPUT_ERR("Unexpected cursor position value [ %d ]!", startAt);
        break;
    }

    return result;
}

uint32_t File::_osGetPositionFile() const
{
    ASSERT(mFileHandle != nullptr);
    return static_cast<uint32_t>( lseek(reinterpret_cast<PosixFile*>(mFileHandle)->fd, 0, SEEK_CUR) );
}

bool File::_osTruncateFile()
{
    ASSERT(mFileHandle != nullptr);
    return (areg::RETURNED_OK == ftruncate(reinterpret_cast<PosixFile*>(mFileHandle)->fd, 0));
}

void File::_osFlushFile()
{
    ASSERT(mFileHandle != nullptr);
    fsync(reinterpret_cast<PosixFile*>(mFileHandle)->fd);
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

uint32_t File::_osCreateTempFileName(char* buffer, const char* folder, const char* prefix, uint32_t /*unique*/)
{
    ASSERT(buffer != nullptr);
    ASSERT(folder != nullptr);
    ASSERT(prefix != nullptr);

    constexpr char fmtFileName[]{ "%s%c%sXXXXXX" };
    constexpr char fmtFdName[]{ "/proc/self/fd/%d" };
    sprintf(buffer, fmtFileName, folder, File::PATH_SEPARATOR, prefix);
    int fno = ::mkstemp(buffer);
    if (fno > 1)
    {
        char temp[128];
        sprintf(temp, fmtFdName, fno);
        int count = readlink(temp, buffer, File::MAXIMUM_PATH);
        buffer[count > 0 ? count : 0] = areg::String::EmptyChar;
        ::close(fno);
    }

    return static_cast<uint32_t>(strlen(buffer));
}

uint32_t File::_osGetSpecialDir(char* buffer, uint32_t /*length*/, const File::SpecialFolder specialFolder)
{
    ASSERT(buffer != nullptr);
    buffer[0] = areg::EndOfString;
    const char * filePath = nullptr;

    switch (specialFolder)
    {
    case File::SpecialFolder::UserHome:
        filePath = _getUserHomeDir();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s", filePath != nullptr ? filePath : "~");
        break;

    case File::SpecialFolder::Personal:
        filePath = _getUserHomeDir();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s%c%s", filePath != nullptr ? filePath : "", File::PATH_SEPARATOR, DIR_NAME_DOCUMENTS);
        break;

    case File::SpecialFolder::AppData:
        filePath = _getUserHomeDir();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s%c.%s%c%s"
                    , filePath
                    , static_cast<int32_t>(File::PATH_SEPARATOR)
                    , Process::getInstance().getAppName().getString()
                    , static_cast<int32_t>(File::PATH_SEPARATOR)
                    , DIR_NAME_APPDATA);
        break;

    case File::SpecialFolder::Temp:
        filePath = _getTempDir();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s", filePath);
        break;

    default:
        break;
    }

    return static_cast<uint32_t>(strlen(buffer));
}

#endif //  defined(_POSIX) || defined(POSIX)
