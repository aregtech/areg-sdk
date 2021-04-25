/************************************************************************
 * \file        areg/src/base/private/linux/CEFileWin.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, File object 
 *              Linux specific implementation
 *
 ************************************************************************/

#include "areg/src/base/CEFile.hpp"

#ifdef _LINUX

#include "areg/src/base/EContainers.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/base/CESharedBuffer.hpp"
#include "areg/src/base/TEString.hpp"
#include "areg/src/base/NEUtilities.hpp"

#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/sendfile.h>
#include <paths.h>

#include <filefuncs.hpp>

//////////////////////////////////////////////////////////////////////////
// CEFile class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// defined constants
//////////////////////////////////////////////////////////////////////////
void * const        CEFile::INVALID_HANDLE      = static_cast<void *>(0u);
const char          CEFile::PATH_SEPARATOR      = '/';
const char * const  CEFile::TIMESTAMP_FORMAT    = "%Y_%m_%d_%H_%M_%S_%%03d"; // yyyy_mm_dd_hh_mm_ss_ms

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool CEFile::Open( void )
{
    bool result = false;
    if (IsOpened() == false)
    {
            /*
            how the flags are handles in linux, using stdio
              FOB_INVALID: 
                    dont't do anything
              FOB_READ:
                    mode for fopen is "r" 
              FOB_WRITE:
                    mode for fopen is "a" (or "w" if FOB_TRUNCATE is set)
                    if FOB_READ is set as well, a + is added to the mode ("w+" or "a+")
              FOB_BINARY:
                    "b" is added to the mode
              FOB_TEXT:
                    no special mode char for text mode required
              FOB_SHARE_READ:
                    if set, nothing has to be done, files are opened shared anyway
                    if NOT set, the file has to be locked after opening
                    Also, locks cannot be made mandatory if the file system does not support it. 
                    possible flag combinations:
                        FOB_SHARE_READ && FOB_SHARE_WRITE: no lock required
                        FOB_SHARE_READ || FOB_SHARE_WRITE: use shared lock
                        none:                              use exclusive lock
              FOB_SHARE_WRITE:
                    see FOB_SHARE_READ
              FOB_CREATE:
                    mode for fopen is "w" (demands that FOB_WRITE is set, since in windows implementation 
                    this is checked by the open functions, we have to do it explicitly here -- the API description
                    does not specify that this flag requires write mode, but reading from an empty file won't make any sense).
                    File might not exist (see FOB_TRUNCATE)
              FOB_EXIST:
                    before opening, check if the file exists, and fail if it does not
              FOB_TRUNCATE:
                    mode for fopen is "w" (demands that FOB_WRITE is set, since in windows implementation 
                    this is checked by the open functions, we have to do it explicitly here).
                    Before opening, check that the file exist, and fail if it does not 
                    (to be compatible with the Windows API CreateFile, which fails if TRUNCATE_EXISTING is set
                    and the file does not exist)
                    is not compatible with FOB_CREATE
              FOB_ATTACH:
                    not used here
              FOB_DETACH:
                    not used here
              FOB_FORCE_DELETE:
                    not used here
              FOB_WRITE_DIRECT:
                    after opening, call setbuf(NULL) to disable buffering
              FOB_TEMP_FILE:
                    ignored for now (meaning in windows: try to cache as long as possible - there is no equivalent to this in Linux)
                    may use a larger buffer for such files?
            so we have the following conditions for the possible modes:
                r+    -- not used --
                w+    FOB_WRITE & FOB_READ & (FOB_TRUNCATE | FOB_CREATE)
                a+    FOB_WRITE & FOB_READ & !(FOB_TRUNCATE | FOB_CREATE)
                r     FOB_READ
                w     FOB_WRITE & (FOB_TRUNCATE | FOB_CREATE)
                a     FOB_WRITE & !(FOB_TRUNCATE | FOB_CREATE)
            if FOB_BINARY is set, a "b" is added
            */
        // mode sanity checks:
        if ((mFileMode & (FOB_TRUNCATE | FOB_CREATE)) && (mFileMode & FOB_WRITE) == 0)
        {
                OUTPUT_ERR("FOB_TRUNCATE and FOB_CREATE requires FOB_WRITE.");
                mFileMode = FO_MODE_INVALID;
        }
        if ((mFileMode & (FOB_READ | FOB_WRITE)) == 0) {
                OUTPUT_ERR("File open mode contains neither read nor write.");
                mFileMode = FO_MODE_INVALID;
        }
        if (mFileName != NULL && mFileMode != FO_MODE_INVALID)
        {
            mFileMode = NormalizeMode(mFileMode);
            if (mFileMode & CEFileBase::FOB_CREATE)
            {
                std::string dirName = CEFile::GetFileDirectory( static_cast<const char *>(mFileName) );
                CEFile::CreateFoldersCascaded( dirName.c_str() );
            }
            // set mode
            char   mode[4];
            size_t modePos = 1;
            if (mFileMode & FOB_WRITE)
            {
                if (mFileMode & (FOB_TRUNCATE | FOB_CREATE))
                {
                    mode[0] = 'w';
                    if (mFileMode & FOB_TRUNCATE) 
                    {
                        if (!CEFile::IsFileExist(mFileName)) {
                            // only issue a warning here:
                            OUTPUT_WARN("FOB_TRUNCATE used while opening a nonexistant file.");
                        }
                    }
                }
                else
                {
                    mode[0] = 'a';
                }
                if (mFileMode & FOB_READ)
                {
                    mode[modePos] = '+';
                    ++modePos;
                }
            }
            else if (mFileMode & FOB_READ) {
                mode[0] = 'r';
            }
            if (mFileMode & FOB_BINARY)
            {
                mode[modePos] = 'b';
                ++modePos;
            }
            mode[modePos] = '\0';
            if (mFileMode != FO_MODE_INVALID) {
                mFileHandle = fopen(mFileName, mode);
                if (mFileHandle)
                {
                    // no buffer
                    if (mFileMode & FOB_WRITE_DIRECT)
                    {
                        setbuf(mFileHandle, NULL);
                    }
                    // lock
                    if ((mFileMode & (FOB_SHARE_READ | FOB_SHARE_WRITE)) != (FOB_SHARE_READ | FOB_SHARE_WRITE))
                    {
                        // lock required:
                        // TODO!
                        OUTPUT_WARN("Exclusinve access to files not yet implemented.");
                    }
                }
            }
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
    if (IsOpened())
    {
        if (fclose(mFileHandle) == EOF) 
        {
            OUTPUT_WARN("Closing file [ %s ] failed.", mFileName);
            // handle cannot be used any more, so it should be set to invalid anyway
        }
        else if (mFileMode & FOB_TEMP_FILE)
        {
            CEFile::FileDelete(mFileName);
        }
    }
    mFileHandle = CEFile::INVALID_HANDLE;
}

unsigned int CEFile::Read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (IsOpened() == false)
    {
        OUTPUT_ERR("The file is not opened, cannot read data.");
        ASSERT(false);
    }
    else if (CanRead())
    {
        if (buffer != NULL && size > 0)
        {
            if (CanWrite())
            {
                // Buffer must be flushed, see
                // http://www.cplusplus.com/reference/cstdio/fopen/
                // but alas! Flush() is not const
                const_cast<CEFile*>(this)->Flush();
            }
            size_t sizeRead = fread(buffer, sizeof(char), static_cast<size_t>(size), mFileHandle);
            if (sizeRead != size)
            {
                int err = ferror(mFileHandle);
                if (err != 0)
                {
                    OUTPUT_ERR("Failed read file [ %s ], error code [ %x: %s ] -  read %d bytes instead of %d.", mFileName, err, strerror(err), sizeRead, size);
                }
                // else feof() should be != 0 but no need to check this
                result = sizeRead;
            }
        }
        else
        {
            OUTPUT_WARN("The length is zero, do not copy data.");
        }
    }
    else
    {
        OUTPUT_ERR("Cannot read data, check open mode.");
    }
    return result;
}

unsigned int CEFile::Read( IEByteBuffer & buffer ) const
{
    unsigned int result = 0;
    buffer.InvalidateBuffer();
    if (IsOpened() == false)
    {
        OUTPUT_ERR("The file is not opened, cannot read data.");
        ASSERT(false);
    }
    else if (CanRead())
    {
        unsigned int  sizeRead = 0;
        uint32_t      sizeReserve = 0;
       // first, read the number of bytes to be read (see Write(IEByteBuffer))
        sizeRead = Read(reinterpret_cast<unsigned char*>(&sizeReserve), sizeof(sizeReserve));

        if (   sizeRead == sizeof(uint32_t)
            && sizeReserve > 0
            && buffer.ResizeBuffer(sizeReserve) == sizeReserve)
        {
            unsigned char* data = buffer.GetBuffer();
            if (data != NULL)
            {
                sizeRead = Read(data, sizeReserve);
                if (sizeRead == sizeReserve) 
                    result = static_cast<unsigned int>(sizeRead);
                else 
                    OUTPUT_ERR("Read only [ %d ] instead of [ %d ] bytes from file [ %s ].", sizeRead, sizeReserve, mFileName);
            }
            else
                OUTPUT_ERR("Was not able to reserve [ %d ] bytes of space.", sizeReserve);
        }
        else
        {
            OUTPUT_WARN("Either wailed read integer value or the length is zero, ignoring copying data.");
        }
    }
    else
    {
        OUTPUT_ERR("Cannot read data, check open mode.");
    }
    return result;
}

unsigned int CEFile::Write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;
    if (IsOpened() == false)
    {
        OUTPUT_ERR("The file is not opened. Cannot write data");
    }
    else if ( CanWrite() )
    {
        if (buffer != NULL && size > 0)
        {
            unsigned long sizeWrite = fwrite(reinterpret_cast<const void*>(buffer), sizeof(char), static_cast<size_t>(size), mFileHandle);

            if (sizeWrite == size)
                result = size;
            else 
                OUTPUT_ERR("Failed to write [ %d ] bytes of data to file [ %s ]. Error code [ %x: %s ].", size, mFileName, ferror(mFileHandle), strerror(ferror(mFileHandle)));
        }
        else
        {
            OUTPUT_WARN("The size of data is zero, ignoring to write data.");
        }
    }
    else
    {
        OUTPUT_ERR("Writing mode is not set, current mode is [ %d ]. Forbidden to write data.", mFileMode);
    }
    return result;
}

unsigned int CEFile::SetPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;

    /*
    IMPORTANT NOTE: fseek/ftell work different as expected if used in text mode.
    */
    if (IsOpened())
    {
        int      moveMethod = SEEK_SET;
        off_t    moveOffset = static_cast<off_t>(offset);

        // see documentation: text mode only supports SEEK_SET, and offset should be 0 or a value returned by ftell
        if (IsTextMode()) {
            if (startAt != IECursorPosition::POSITION_BEGIN) {
                OUTPUT_ERR("When setting position in [ %s ]: text mode only supports POSITION_BEGIN, setting position not possible.");
                return IECursorPosition::INVALID_CURSOR_POSITION;
            }
            else if (offset != 0) {
                OUTPUT_INFO("When setting position in [ %s ]: non-zero offset should be a value returned by GetPosition.");
            }
        }
        switch (startAt)
        {
        case    IECursorPosition::POSITION_BEGIN:
            moveMethod  = SEEK_SET;
            break;

        case    IECursorPosition::POSITION_CURRENT:
            moveMethod  = SEEK_CUR;
            break;

        case    IECursorPosition::POSITION_END:
            moveMethod = SEEK_END;
            OUTPUT_WARN("SetPositon in [ %s ] uses POSITION_END, this might not be supported by the library implementation.", mFileName);
            break;

        default:
            OUTPUT_ERR("Unexpected CEFileBase::eCursorPosition type.");
            moveMethod  = SEEK_CUR;
            moveOffset  = 0;
        }

        if (fseeko(mFileHandle, moveOffset, moveMethod) == 0)
        {
            // To be compatible with windows implementation, return current file position
            off_t pos = ftello(mFileHandle);
            if (pos >= 0) {
                // might not fit in an unsigned int!
                if (sizeof(pos) > sizeof(result) && pos > static_cast<off_t>(UINT_MAX))
                {
                    OUTPUT_ERR("SetPosition for [ %s ]: current position is too large to fit in the result, returning UINT_MAX.", mFileName);
                    result = UINT_MAX;
                }
                else
                    result = static_cast<unsigned int>(pos);
            }
            else 
                OUTPUT_ERR("Retrieving current file pointer of [ %s ] failed with errcode [ %x: %s ].", mFileName, errno, strerror(errno));
        }
        else
            OUTPUT_ERR("Setting current file pointer of [ %s ] failed with errcode [ %x: %s ].", mFileName, ferror(mFileHandle), strerror(ferror(mFileHandle)));
    }
    return result;
}

unsigned int CEFile::GetPosition( void ) const
{
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (IsOpened())
    {
        off_t pos = ftell(mFileHandle);
        if (pos >= 0)
        {
            if (sizeof(pos) > sizeof(result) && pos > static_cast<off_t>(UINT_MAX))
            {
                OUTPUT_ERR("GetPosition for [ %s ]: current position is too large to fit in the result, returning UINT_MAX.", mFileName);
                result = UINT_MAX;
            }
            else
                result = static_cast<unsigned int>(pos);
        }
        else
            OUTPUT_ERR("Retrieving current file pointer of [ %s ] failed with errcode [ %x: %s ].", mFileName, errno, strerror(errno));
    }
    return result;
}

unsigned int CEFile::GetLength( void ) const
{
    /* 
    do NOT use fseek(end) & ftell, see 
    https://www.securecoding.cert.org/confluence/display/seccode/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file
    
    Advantage: works also if file is not opened
    */
    unsigned int result = NEMemory::INVALID_SIZE;

    if (mFileName != NULL) {
        struct stat  st;

        if (stat(GetFullPath().c_str(), &st) == 0) {
            // file size might be larger than 32 bit!
            if (sizeof(st.st_size) > sizeof(result) && st.st_size > static_cast<off_t>(UINT_MAX)) {
                OUTPUT_ERR("Error retrieving file size for [ %s ], error code [ %x: %s ].", mFileName, errno, strerror(errno));
            }
            else 
                result = static_cast<unsigned int>(st.st_size);
        }
        else
            OUTPUT_ERR("Error retrieving file size for [ %s ], error code [ %x: %s ].", mFileName, errno, strerror(errno));
    }
    return result;
}

unsigned int CEFile::Reserve(int newSize)
{
    OUTPUT_DBG("Going to reserve [ %d ] of data for file [ %s ].", newSize, mFileName != NULL ? mFileName : "NULL");

    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (IsOpened() && CanWrite())
    {
        unsigned int curPos = newSize <= 0 ? 0 : GetPosition();
        curPos = newSize > static_cast<int>(curPos) ? curPos : newSize;
        if (ftruncate(fileno(mFileHandle), newSize < 0 ? 0 : newSize) == 0)
        {
            result = SetPosition(curPos, IECursorPosition::POSITION_BEGIN);
        }
        else
        {
            OUTPUT_ERR("Failed to file size of [ %s ] to [ %d ] with errcode [ %x: %s ].", mFileName, newSize, errno, strerror(errno));
        }
    }
    else
    {
        // ftruncate could do the job anyway, but to show same behaviour as windows version, do nothing.
        OUTPUT_ERR("Cannot set file size of [ %s ] to [ %d ], file must be opened for writing.", mFileName, newSize);
    }
    return result;
}

bool CEFile::PurgeData( void )
{
    return Reserve(0) != IECursorPosition::INVALID_CURSOR_POSITION;
}

void CEFile::Flush( void )
{
    if (IsOpened())
        if (fflush(mFileHandle) != 0)
            OUTPUT_ERR("Flushing [ %s ] failed with errcode [ %x: %s ].", mFileName, ferror(mFileHandle), strerror(ferror(mFileHandle)));
}

std::string CEFile::GetFullPath( void ) const
{
    /*
    Tehre is no standard way to obtain a file name from a FILE* or file descriptor. 
    One possibility is using readlink on /proc/self/fd/<fdnum>
    however, it seems that this won't work in an kernel module / sys call
    and also might not work on all systems
    also it most  problably is costly
    But since the file name is already known, just use that
    */
    return CEFile::GetFileFullPath(mFileName);
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

std::string CEFile::GetFileName( const char* filePath )
{
    std::string result;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        // should return empty string if filePath is actually a directory
        struct stat st;
        bool        ok = true; // if stat fails, we assume it is a nonexistent file and not a directory

        if (stat(filePath, &st) == 0)
        {
            if (S_ISDIR(st.st_mode))
                ok = false;
        }
        if (ok)
            result = basename(filePath);
    }
    return result;
}

std::string CEFile::GetFileDirectory(const char* filePath)
{
    std::string result;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        result = filefuncs::dirname(filePath);
    }
    return result;
}


bool CEFile::FileDelete( const char* filePath )
{
    bool res = false;
    if ( NEString::isEmpty<char>(filePath) == false) 
    {
        if (remove(filePath) == 0)
            res = true;
        else
            OUTPUT_ERR("Deleting file [ %s ] failed with errcode [ %x: %s ]", filePath, errno, strerror(errno));
    }
    return res;
}

bool CEFile::DirectoryCreate( const char* dirPath )
{
    bool result = false;

    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        if (mkdir(dirPath, 0750/*TODO how to specify the desired rights?*/) == 0)
            result = true;
        else
            OUTPUT_ERR("Creating directory [ %s ] failed with errcode [ %x: %s ]", dirPath, errno, strerror(errno));
    }
    return result;
}

bool CEFile::DirectoryDelete( const char* dirPath )
{
    bool result = false;

    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        if (rmdir(dirPath) == 0)
            result = true;
        else
            OUTPUT_ERR("Deleting directory [ %s ] failed with errcode [ %x: %s ]", dirPath, errno, strerror(errno));
    }
    return result;
}

bool CEFile::FileMove( const char* oldPath, const char* newPath )
{
    bool result = false;

    if ( NEString::isEmpty<char>(oldPath) == false && NEString::isEmpty<char>(newPath) == false )
    {
        // note that rename fails if both paths do not point to the same file system.
        if (rename(oldPath, newPath) == 0)
            result = true;
        else
            OUTPUT_ERR("Moving [ %s ] to [ %s ] failed with errcode [ %x: %s ]", oldPath, newPath, errno, strerror(errno));
    }
    return result;
}

std::string CEFile::GetCurrentDir( void )
{
    std::string result;
    // see http://linux.die.net/man/2/getcwd: if the buffer given to geetcwd is NULL, and size is 0, a buffer with just the required size is allocated
    char        *buf = getcwd(NULL, 0);

    if (buf != NULL)
    {
        result = buf;
        free(buf);
    }
    else
        OUTPUT_ERR("Obtaining the current directory failed with error code [ %x: %s ].", errno, strerror(errno));
    return result;
}

bool CEFile::SetCurrentDir( const char* dirPath )
{
    bool result = false;

    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        if (chdir(dirPath) == 0)
            result = true;
        else
            OUTPUT_ERR("Changing current directory to [ %s ] failed with errcode [ %x: %s ]", dirPath, errno, strerror(errno));
    }
    return result;
}


bool CEFile::FileCopy( const char* originPath, const char* newPath, bool copyForce )
{
    /*
    there is no simple API call to copy a file
    see http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c for possibilities
    using sendfile should be fastest
    */
    bool result = false;

    if ( NEString::isEmpty<char>(originPath) == false && NEString::isEmpty<char>(newPath) == false )
    {
        int src = open(originPath, O_RDONLY);
        if (src < 0)
            OUTPUT_ERR("FileCopy: open source path [ %s ] failed with error code [ %x: %s ].", originPath, errno, strerror(errno));
        else {
            int dst = open(newPath, O_WRONLY | (copyForce ? O_TRUNC : (O_CREAT | O_EXCL) ) );
            if (dst < 0)
                OUTPUT_ERR("FileCopy: open destination path [ %s ] failed with error code [ %x: %s ].", newPath, errno, strerror(errno));
            else {
                struct stat st;

                if (fstat(src, &st) != 0)
                    OUTPUT_ERR("FileCopy: rad stats of [ %s ] failed with error code [ %x: %s ]." ,originPath, errno, strerror(errno));
                else if (sendfile(dst, src, NULL, st.st_size) == 0)
                    result = true;
                else
                    OUTPUT_ERR("FileCopy: copy data from [ %s ] to [ %s ] failed with error code [ %x: %s ]." ,originPath, newPath, errno, strerror(errno));
                if (close(dst) != 0)
                        OUTPUT_WARN("Closing [ %s ] failed with error code [ %x: %s ].", newPath, errno, strerror(errno));
            }
            if (close(src) != 0)
                    OUTPUT_WARN("Closing [ %s ] failed with error code [ %x: %s ].", originPath, errno, strerror(errno));
        }
    }
    return result;
}

std::string CEFile::GetTempFolder( void )
{
    // see http://stackoverflow.com/questions/31068/how-do-i-find-the-temp-directory-in-linux#answer-18500978
    // Alas! current libc does not implement issetugid!
    // use alternative (still better than doing nothing)
    std::string  result = CEProcess::GetProcess().GetSafeEnvVariable("TMPDIR");

    if (result.empty()) // process tainted, or TMPDIR not set
        result = P_tmpdir;
    if (result.empty())
        result = _PATH_TMP;
    if (result.empty()) // still nothing? use the default
        result = "/tmp";  // or "/var/tmp", which isn't cleared at boot? 
    return result;
}

std::string CEFile::CreateTempFileName( const char* prefix, bool unique, bool inTempFolder )
{
    /*
    the result is either in temp folder or current folder, 
    starts with refix or TEMP_FILE_PREFIX
    and the has 6 hexadecimal digits
    no file extension
    */
    std::string result = inTempFolder ? CEFile::GetTempFolder() : ".";
    char        tmp[10];

    result += "/";
    result += prefix != NULL ? prefix : CEFile::TEMP_FILE_PREFIX;
    if (unique) // ensure that there is such file
    {
        std::string base = result;
        int         r = rand();

        do {
            result = base + NEStd::IntToStringA(r & 0xFFFFFF, tmp, 10, 16);
            ++r;
        } while (CEFile::IsFileExist(result.c_str()));
    }
    else // don't care if the file anready exists
        result += NEStd::IntToStringA(rand() & 0xFFFFFF, tmp, 10, 16);
    return result;
}

bool CEFile::IsDirectoryExist( const char* dirPath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        struct stat st;
        if (stat(dirPath, &st) == 0)
            // TODO: what if dirPath points to a symlink, which points to a directory?
            // technically it would not be a directory, however, it can be used like a directory ...
            result = S_ISDIR(st.st_mode);
        else 
            OUTPUT_ERR("Getting file attributes of [ %s ] failed with error code [ %x: %s ].", dirPath, errno, strerror(errno));
    }
    return result;
}

bool CEFile::IsFileExist( const char* filePath )
{
    bool result     = false;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        struct stat st;
        if (stat(filePath, &st) == 0)
            // consistent with windows implementation: true if exists and not a directory
            // TODO: what about a symlink that points to a directory?
            result = !S_ISDIR(st.st_mode);
        else 
            OUTPUT_ERR("Getting file attributes of [ %s ] failed with error code [ %x: %s ].", filePath, errno, strerror(errno));
    }
    return result;
}

std::string CEFile::SearchFile( const char* fileName, const char* fileExtension, const char* searchInDirectory )
{
    std::string result;
    if ( NEString::isEmpty<char>(fileName) == false )
    {
        /*
        The windows API function searches 
        - if searchInDirectory != NULL, only in searchInDirectory
        - else depending on some registry setting
          either first the current folder and then the system path (default)
          or system path and then the current folder
        */
        if (searchInDirectory != NULL) {
            std::string path = searchInDirectory;
            struct stat st;

            if (path[path.size()-1] != CEFile::PATH_SEPARATOR)
                path += CEFile::PATH_SEPARATOR;
            path += fileName;
            // fileExtension: must contain dot, and is only used if fileName does not contain a dot
            if (fileExtension != NULL && fileExtension[0] == '.' && strstr(fileName, ".") == NULL)
                path += fileExtension;
            if (stat(path.c_str(), &st) == 0)
                result = path;
            else if (errno != ENOENT)
                OUTPUT_ERR("Searching for [ %s ] failed with error code [ %x: %s ].", path.c_str(), errno, strerror(errno));
        }
        else {
            // there is no registry setting, so use the default behavior: search current folder first,
            // then in PATH
            CETokenizer folders(CEProcess::GetProcess().GetSafeEnvVariable("PATH"), ":", false);
            size_t      i;

            folders.InsertAt(0, CEFile::GetCurrentDir());
            for (i = 0; i < folders.GetSize() && result.empty(); i++)
            {
                result = CEFile::SearchFile(fileName, fileExtension, folders[i]);
            }
        }
    }
    return result;
}

std::string CEFile::GetFileFullPath( const char* filePath )
{
    std::string result;
    // below C++11, there is no guaranteed way to get a safe writeable buffer of a std::string. 
    // &str[0] should work in most (all?) implementations, but let's not rely on it ...
    // so use a separate buffer
    char        buf[PATH_MAX + 1];

    if (filePath != NULL)
    {
        if (realpath(filePath, buf) != NULL)
            result = buf;
        else
            OUTPUT_ERR("Cannot determine full path for [ %s ], errcode is [ %x: %s ].",  filePath, errno, strerror(errno));
    }
    return result;
}

bool CEFile::CreateFoldersCascaded( const char* dirPath )
{
    bool result         = false;
    if ( NEString::isEmpty<char>(dirPath) == false )
    {
        CETokenizer   segments(CEFile::GetFileFullPath(dirPath), CEFile::PATH_SEPARATOR);
        unsigned int  i;
        CEString      path;

        result = true;
        for (i = 0; result && i < segments.GetSize(); i++)
        {
            path += CEString(CEFile::PATH_SEPARATOR) + segments[i];
            if (CEFile::IsDirectoryExist(path))
                continue;
            if (!CEFile::DirectoryCreate(path))
                result = false;
        }
    }
    return result;
}

std::string CEFile::NormalizeFileName( const char * fileName, bool forceTime/*=false*/ )
{
    std::string     result;

    struct timeval  tv;
    struct tm       t;
    char            fmt[32];

    if (gettimeofday(&tv, NULL) != 0) {
        OUTPUT_WARN("Calling gettimeofday failed with error code [ %x: %s ].", errno, strerror(errno));
        tv.tv_sec  = time(NULL);
        tv.tv_usec = 0;
    }
    if (localtime_r(&tv.tv_sec, &t) == NULL) 
    {
        OUTPUT_ERR("Calling localtime_r failed with error code [ %x: %s ].", errno, strerror(errno));
    }
    else
    {
        char tmpFmt[32];
        strftime(tmpFmt, 32, CEFile::TIMESTAMP_FORMAT, &t);
        NEStd::StringPrintfA(fmt, 32, tmpFmt, tv.tv_usec / 1000);

        if ( NEString::isEmpty<char>(fileName) == false )
        {
            result = fileName;
            std::string::size_type index = result.find(CEFile::FILE_MASK_TIMESTAMP);
            if ( index != std::string::npos )
            {
                result.replace(index, strlen(CEFile::FILE_MASK_TIMESTAMP), fmt);
            }
            else if (forceTime)
            {
                index = result.find_last_of('.');
                if (index != std::string::npos)
                    result.insert(index, fmt);
                else
                    result += fmt;
            }

            index = result.find( CEFile::FILE_MASK_APP_DATA );
            if ( index != std::string::npos )
            {
                std::string appFolder = CEFile::GetSpecialFolder(CEFile::SpecialAppData);
                result.replace(index, strlen(CEFile::FILE_MASK_APP_DATA), appFolder);
            }
        }
        else if (forceTime)
        {
            result = fmt;
        }
        result = CEFile::NormalizeFilePath(result.c_str());
    }
    return result;
}

std::string CEFile::GetSpecialFolder(const CEFile::eSpecialFolder & specialFolder)
{
    std::string result = "";
    switch ( specialFolder )
    {
    case CEFile::SpecialUserHome:
        {
            result = "~";
        }
        break;

    case CEFile::SpecialAppData:
        {
            result = "~/.";
            result += CEProcess::GetProcess().GetProcessName();
        }
        break;

    default:
        break;
    }
    return result;
}

#endif // _LINUX
