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


#if defined(_POSIX) || defined(POSIX)

#include "areg/base/File.hpp"
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

namespace {
    //!< POSIX invalid file descriptor.
    constexpr int   POSIX_INVALID_FD        { -1 };
    constexpr char  DIR_NAME_DOCUMENTS[]    { "Documents" };
    constexpr char  DIR_NAME_APPDATA[]      { "profile" };
    constexpr char  ENV_USER_HOME[]         { "HOME" };
    constexpr char  USER_HOME_DIR[]         { "~" };
    constexpr char  USER_TEMP_DIR[]         { "~/tmp" };

    struct PosixFile
    {
        int fd{ POSIX_INVALID_FD };

        PosixFile() noexcept = default;
        PosixFile(const PosixFile & src) noexcept = default;
        PosixFile(int _fd) noexcept : fd(_fd) {}
        PosixFile(FILEHANDLE fh) noexcept : fd(static_cast<int>(reinterpret_cast<intptr_t>(fh))) {}

        void operator = (int _fd) noexcept { fd = _fd; }
        void operator = (FILEHANDLE fh) noexcept { fd = static_cast<int>(reinterpret_cast<intptr_t>(fh)); }

        constexpr bool operator == (int _fd) const noexcept { return fd == _fd; }
        constexpr bool operator != (int _fd) const noexcept { return fd != _fd; }

        bool operator == (FILEHANDLE fh) const noexcept { return fd == static_cast<int>(reinterpret_cast<intptr_t>(fh)); }
        bool operator != (FILEHANDLE fh) const noexcept { return fd != static_cast<int>(reinterpret_cast<intptr_t>(fh)); }

        explicit constexpr operator int32_t() const noexcept { return fd; }
        explicit operator FILEHANDLE() const noexcept { return reinterpret_cast<FILEHANDLE>(fd); }
    };


    //////////////////////////////////////////////////////////////////////////
    // local statics
    //////////////////////////////////////////////////////////////////////////

    inline const char * _posix_user_home()
    {
        const char *homedir = getenv(ENV_USER_HOME);
        if (homedir == nullptr)
        {
            struct passwd *pw = ::getpwuid(getuid());
            homedir = pw != nullptr ? pw->pw_dir : USER_HOME_DIR;
        }

        return homedir;
    }

    inline const char * _posix_temp()
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
} // namespace

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

namespace areg {

FILEHANDLE File::_os_invalid_handle() noexcept
{
    return reinterpret_cast<FILEHANDLE>(POSIX_INVALID_FD);
}

void File::_os_close_file() noexcept
{
    if ( is_opened( ) )
    {
        PosixFile fd{ mFileHandle };
        if (fd != POSIX_INVALID_FD)
        {
            ::close( static_cast<int32_t>(fd) );
        }

        if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitDelete)) != 0)
        {
            ::unlink(mFileName.as_string());
        }
    }

    mFileHandle = File::_os_invalid_handle();
}

bool File::_os_open_file() noexcept
{
    if (is_opened())
    {
        AREG_OUTPUT_WARN("File is already opened. Close file.");
        return true;
    }

    if (mFileName.is_empty())
    {
        AREG_OUTPUT_ERR("File name is not set. Cannot open file.");
        return false;
    }

    PosixFile file;
    std::error_code err;
    mFileMode = normalize_mode(mFileMode);
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

    if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitCreateNew)) != 0)
    {
        flag |= (O_CREAT | O_TRUNC);
    }

    if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitOpenAlways)) != 0)
    {
        flag |= O_CREAT;
    }

    if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitExist)) != 0)
    {
        flag &= ~O_CREAT;
    }

    if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitTruncate)) != 0)
    {
        flag |= O_TRUNC;
    }

    // Creation permissions: only used by open(2) when O_CREAT is set.
    // Start with private owner access, then expand for sharing flags.
    if ((flag & O_CREAT) != 0)
    {
        mode = S_IRUSR | S_IWUSR;                  // owner read+write (baseline)
        if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareRead)) != 0)
            mode |= S_IRGRP;                        // same-group read
        if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitShareWrite)) != 0)
            mode |= S_IRGRP | S_IWGRP;              // same-group read+write
    }

    if ((mFileMode & static_cast<uint32_t>(FileBase::OpenFlag::BitDirect)) != 0)
    {
        flag |= O_SYNC;                             // flush writes to storage on each write() call
    }

    String dirName(File::file_directory(mFileName));
    if ( (flag & O_CREAT) != 0 )
    {
        if (std::filesystem::exists(mFileName.data(), err))
        {
            if (dirName == mFileName)
            {
                flag &= ~O_CREAT;   // remove create flag for directories
                flag &= ~O_TRUNC;   // remove truncate, since it is not applicable for directories
                flag |= O_DIRECTORY;// set directory option
            }
            // else: regular file exists.
            // BitCreateNew: O_TRUNC already set above -> truncates as expected.
            // BitOpenAlways: no O_TRUNC -> opens existing file without truncating.
        }
        else
        {
            File::create_dir_cascaded(dirName);
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
        file =  ::mkstemp(mFileName.buffer());
    }
    else
    {
        file =  ::open(mFileName.as_string(), flag, mode);
    }

#ifdef DEBUG
    if (POSIX_INVALID_FD == static_cast<int>(file.fd))
    {
        AREG_OUTPUT_ERR("Failed to open file [ %s ], errno = [ %p ]", mFileName.as_string(), static_cast<id_type>(errno));
    }
#endif // DEBUG

    mFileHandle = static_cast<FILEHANDLE>(file);
    return (mFileHandle != _os_invalid_handle());
}

uint32_t File::_os_read_file(uint8_t* buffer, uint32_t size) const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    ASSERT((buffer != nullptr) && (size > 0));

    uint32_t result{ 0 };
    PosixFile file{ mFileHandle };
    ssize_t sizeRead = ::read(file.fd, buffer, size);
    if (sizeRead > 0)
    {
        result = static_cast<uint32_t>(sizeRead);
    }
#ifdef  _DEBUG
    else if (sizeRead < 0)
    {
        AREG_OUTPUT_ERR("Failed read file [ %s ], error code [ %p ].", mFileName.as_string(), static_cast<id_type>(errno));
    }
    else
    {
        AREG_OUTPUT_DBG("Finished to read file [ %s ]", mFileName.as_string());
    }
#endif  // !_DEBUG

    return result;
}

uint32_t File::_os_write_file(const uint8_t* buffer, uint32_t size) noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    ASSERT((buffer != nullptr) && (size != 0));

    PosixFile file{ mFileHandle };
    int32_t result = ::write(file.fd, buffer, size);
    if (result != static_cast<int32_t>(size))
    {
        AREG_OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %p ].", size, mFileName.as_string(), static_cast<id_type>(errno));
        result = 0;
    }

    return result;
}

uint32_t File::_os_set_position(int32_t offset, Cursor::SeekOrigin startAt) const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    uint32_t result = Cursor::INVALID_CURSOR_POSITION;

    PosixFile file{ mFileHandle };
    switch (startAt)
    {
    case Cursor::SeekOrigin::Begin:
        result = static_cast<uint32_t>(lseek(file.fd, offset, SEEK_SET));
        break;

    case Cursor::SeekOrigin::Current:
        result = static_cast<uint32_t>(lseek(file.fd, offset, SEEK_CUR));
        break;

    case Cursor::SeekOrigin::End:
        result = static_cast<uint32_t>(lseek(file.fd, offset, SEEK_END));
        break;

    default:
        AREG_OUTPUT_ERR("Unexpected cursor position value [ %d ]!", startAt);
        break;
    }

    return result;
}

uint32_t File::_os_file_position() const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    PosixFile file{ mFileHandle };
    return static_cast<uint32_t>( lseek(file.fd, 0, SEEK_CUR) );
}

bool File::_os_truncate_file() noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    PosixFile file{ mFileHandle };
    return (areg::RETURNED_OK == ftruncate(file.fd, 0));
}

bool File::_os_reserve(uint32_t newSize) noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    PosixFile file{ mFileHandle };
    // ftruncate extends (zero-fills) or shrinks without touching the file pointer.
    return (areg::RETURNED_OK == ::ftruncate(file.fd, static_cast<off_t>(newSize)));
}

void File::_os_flush_file() noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    PosixFile file{ mFileHandle };
    fsync(file.fd);
}

uint32_t File::_os_file_length() const noexcept
{
    ASSERT(mFileHandle != _os_invalid_handle());
    PosixFile file{ mFileHandle };
    struct stat st {};
    return (::fstat(file.fd, &st) == 0) ? static_cast<uint32_t>(st.st_size) : 0u;
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

uint32_t File::_os_temp_name(char* buffer, const char* folder, const char* prefix, uint32_t /*unique*/) noexcept
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
        buffer[count > 0 ? count : 0] = String::EmptyChar;
        ::close(fno);
    }

    return static_cast<uint32_t>(strlen(buffer));
}

uint32_t File::_os_special_dir(char* buffer, uint32_t /*length*/, const File::SpecialFolder specialFolder) noexcept
{
    ASSERT(buffer != nullptr);
    buffer[0] = areg::EndOfString;
    const char * filePath = nullptr;

    switch (specialFolder)
    {
    case File::SpecialFolder::UserHome:
        filePath = _posix_user_home();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s", filePath != nullptr ? filePath : "~");
        break;

    case File::SpecialFolder::Personal:
        filePath = _posix_user_home();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s%c%s", filePath != nullptr ? filePath : "", File::PATH_SEPARATOR, DIR_NAME_DOCUMENTS);
        break;

    case File::SpecialFolder::AppData:
        filePath = _posix_user_home();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s%c.%s%c%s"
                    , filePath
                    , static_cast<int32_t>(File::PATH_SEPARATOR)
                    , Process::instance().app_name().as_string()
                    , static_cast<int32_t>(File::PATH_SEPARATOR)
                    , DIR_NAME_APPDATA);
        break;

    case File::SpecialFolder::Temp:
        filePath = _posix_temp();
        ASSERT(filePath != nullptr);
        ::sprintf(buffer, "%s", filePath);
        break;

    default:
        break;
    }

    return static_cast<uint32_t>(strlen(buffer));
}

} // namespace areg
#endif //  defined(_POSIX) || defined(POSIX)
