/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/File.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, File object
 *              common parts
 ************************************************************************/

#include "areg/base/File.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/Containers.hpp"

#include <filesystem>

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

const char File::getPathSeparator( void )
{
    return static_cast<char>(std::filesystem::path::preferred_separator);
}

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
File::File( void )
    : FileBase    ( )
    , mFileHandle   (File::_osGetInvalidHandle())
{
}

File::File(const String& fileName, unsigned int mode /* = (FileBase::FO_MODE_WRITE | FileBase::FO_MODE_BINARY) */)
    : FileBase    ( )
    , mFileHandle   (File::_osGetInvalidHandle())
{
    mFileName = File::normalizePath(fileName);
    mFileMode = mode;
}

File::~File( void )
{
    _osCloseFile();

    mFileHandle = File::_osGetInvalidHandle();
    mFileMode   = FileBase::FO_MODE_INVALID;
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool File::open(const String& fileName, unsigned int mode)
{
    bool result = false;
    if ((isOpened() == false) && (fileName.isEmpty() == false))
    {
        mFileMode = mode;
        mFileName = File::normalizePath(fileName);

        result = open();
    }
    else
    {
        OUTPUT_WARN("File is already opened. Close file.");
    }

    return result;
}

bool File::open(void)
{
    return _osOpenFile();
}

void File::close(void)
{
    _osCloseFile();
}

unsigned int File::getSizeReadable( void ) const
{
    unsigned int lenRead = 0;
    unsigned int lenUsed = 0;
    if (isOpened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.getObject(), err);

        lenRead = _osGetPositionFile();
        lenUsed = !err ? static_cast<unsigned int>(sz) : 0;
    }

    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

unsigned int File::getSizeWritable( void ) const
{
    unsigned int lenWritten     = 0;
    unsigned int lenAvailable   = 0;
    if (isOpened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.getObject(), err);

        lenWritten  = _osGetPositionFile();
        lenAvailable = !err ? static_cast<unsigned int>(sz) : 0;
    }

    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

bool File::remove( void )
{
    bool result = false;

    if (isOpened())
    {
        ASSERT(mFileName.isEmpty() == false);

        close();
        std::error_code err;
        result = std::filesystem::remove(mFileName.getObject(), err);
    }

    mFileHandle = File::_osGetInvalidHandle();
    mFileMode   = FileBase::FO_MODE_INVALID;
    mFileName   = String::getEmptyString();

    return result;
}

bool File::isOpened() const
{
    return (mFileHandle != File::_osGetInvalidHandle());
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

inline bool File::_nameHasCurrentFolder(const char * filePath, bool skipSep)
{
    bool result = false;
    if (NEString::isEmpty<char>(filePath) == false)
    {
        if (filePath[0] == File::DIR_CURRENT[0])
        {
            result = (skipSep && filePath[1] == NEString::EndOfString)  || 
                     (filePath[1] == File::UNIX_SEPARATOR)              ||
                     (filePath[1] == File::DOS_SEPARATOR);
        }
    }

    return result;
}

inline bool File::_nameHasParentFolder(const char * filePath, bool skipSep)
{
    bool result{ false };
    if (NEString::isEmpty<char>(filePath) == false)
    {
        if ((filePath[0] == File::DIR_PARENT[0]) && (filePath[1] == File::DIR_PARENT[1]))
        {
            result = (skipSep && filePath[2] == NEString::EndOfString) || (filePath[2] == File::UNIX_SEPARATOR) ||  (filePath[2] == File::DOS_SEPARATOR);
        }
    }

    return result;
}

String File::genTempFileName(const char* prefix, bool unique, bool inTempFolder)
{
    String result;

    result.reserve(File::MAXIMUM_PATH);
    unsigned int space{ 0 };
    if (result.getCapacity() != 0)
    {
        char* buffer = result.getBuffer();
        unsigned int ticks = unique ? 0 : static_cast<unsigned int>(DateTime::getSystemTickCount());
        prefix = prefix == nullptr ? File::TEMP_FILE_PREFIX.data() : prefix;
        String dir = inTempFolder ? File::getTempDir() : File::getCurrentDir();
        if (dir.isEmpty() == false)
        {
            space = _osCreateTempFile(buffer, dir.getString(), prefix, ticks);
        }
    }

    result.resize(space);

    return result;
}

String File::genTempFileName()
{
    return genTempFileName(nullptr, true, true);
}

const String & File::getExecutableDir(void)
{
    return Process::getInstance().getPath();
}

String File::getFileNameWithExtension( const char* filePath )
{
    const char * result = nullptr;
    if ( NEString::isEmpty<char>(filePath) == false )
    {
        NEString::CharPos pos = NEString::getStringLength<char>(filePath) - 1;
        if (filePath[pos] != File::getPathSeparator())
        {
            pos = NEString::findLast<char>(File::getPathSeparator(), filePath, pos - 1, nullptr);
            if (NEString::isPositionValid(pos))
            {
                result = filePath + pos + 1;
            }
        }
    }

    return (result != nullptr ? String(result) : String::getEmptyString());
}

String File::getFileName( const char* filePath )
{
    String result(File::getFileNameWithExtension(filePath));
    NEString::CharPos pos = result.findLast(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (NEString::isPositionValid(pos) && NEString::isAlphanumeric<char>(result[pos + 1]) )
    {
        result.substring(0, pos);
    }

    return result;
}

String File::getFileExtension( const char* filePath )
{
    String result;
    String fileName(File::getFileNameWithExtension(filePath));
    NEString::CharPos pos = fileName.findLast(File::EXTENSION_SEPARATOR, NEString::END_POS, true);
    if (NEString::isPositionValid(pos) && NEString::isAlphanumeric<char>(fileName[pos + 1]) )
    {
        fileName.substring(result, pos, NEString::COUNT_ALL);
    }

    return result;
}

String File::getFileDirectory(const char* filePath)
{
    NEString::CharPos pos = NEString::isEmpty<char>(filePath) ? NEString::INVALID_POS : NEString::findLast<char>(File::getPathSeparator(), filePath, NEString::END_POS, nullptr);
    return (NEString::isPositionValid(pos) ? String(filePath, pos) : String(String::getEmptyString()));
}

bool File::createDirCascaded( const char* dirPath )
{
    bool result{ false };
    if ( NEString::isEmpty<char>( dirPath ) == false )
    {
        std::error_code err;
        std::filesystem::create_directories( dirPath, err );
        result = static_cast<bool>(err) == false;
    }

    return result;
}

String File::normalizePath(const char* fileName)
{
    String result;
    if (NEString::isEmpty<char>(fileName) == false)
    {
        result = fileName;
        FileBase::normalizeName(result);
        std::error_code err;
        std::filesystem::path fp = std::filesystem::absolute(result.getObject(), err);
        if (!err)
        {
            result = fp.string();
        }
    }

    return result;
}

bool File::findParent(const char * filePath, const char ** nextPos, const char * lastPos /*= nullptr*/)
{
    bool result = false;
    if ( NEString::isEmpty<char>(filePath) == false)
    {
        int length = 0;
        if (nextPos != nullptr)
            *nextPos = nullptr;
        
        if (lastPos != nullptr)
        {
            length = lastPos == filePath ? 0 : static_cast<int>(lastPos - filePath - 1);
        }
        else
        {
            length = NEString::getStringLength<char>(filePath); 
            if ( filePath[length - 1] == File::getPathSeparator() )
                -- length;
        }

        if (length != 0)
        {
            int pos = NEString::findLast(File::getPathSeparator(), filePath, NEString::END_POS, nextPos);
            if ((pos > 0) && (pos < length))
            {
                result = true;
                if (nextPos != nullptr)
                    *nextPos = filePath + pos;
            }
        }
    }

    return result;
}

String File::getParentDir(const char * filePath)
{
    String result;
    const char * end = nullptr;
    if (File::findParent(filePath, &end))
    {
        result.assign(filePath, MACRO_ELEM_COUNT(filePath, end));
    }

    return result;
}

int File::splitPath(const char * filePath, StringList & in_out_List)
{
    int oldCount        { static_cast<int>(in_out_List.getSize()) };
    const char * start  { filePath };
    const char * end    { filePath };

    while (*end != NEString::EndOfString)
    {
        if ((*end == File::UNIX_SEPARATOR) || (*end == File::DOS_SEPARATOR))
        {
            String node(start, MACRO_ELEM_COUNT(start, end));
            if (node.isEmpty() == false)
                in_out_List.pushLast( node );

            start = ++ end;
        }
        else
        {
            ++ end;
        }
    }

    if (start != end)
    {
        String node(start, MACRO_ELEM_COUNT(start, end));
        if (node.isEmpty() == false)
            in_out_List.pushLast( node );
    }

    return (in_out_List.getSize() - oldCount);
}

unsigned int File::read(IEByteBuffer & buffer) const
{
    return FileBase::read(buffer);
}

unsigned int File::read(String & asciiString) const
{
    return FileBase::read(asciiString);
}

unsigned int File::read(WideString & wideString) const
{
    return FileBase::read(wideString);
}

unsigned int File::read(unsigned char* buffer, unsigned int size) const
{
    unsigned int result = 0;
    if (isOpened() && canRead())
    {
        if ((buffer != nullptr) && (size > 0))
        {
            result = _osReadFile(buffer, size);
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

unsigned int File::write(const IEByteBuffer & buffer)
{
    return FileBase::write(buffer);
}

unsigned int File::write(const String & asciiString)
{
    return FileBase::write(asciiString);
}

unsigned int File::write(const WideString & wideString)
{
    return FileBase::write(wideString);
}

unsigned int File::write(const unsigned char* buffer, unsigned int size)
{
    unsigned int result = 0;
    if (isOpened() && canWrite())
    {
        if ((buffer != nullptr) && (size > 0))
        {
            result = _osWriteFile(buffer, size);
        }
        else
        {
            OUTPUT_WARN("The size of data is zero, ignoring to write data.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file [ %s ] is not opened [ %s ], or writing mode is not set (mode = [ %d ]).", mFileName.getString(), isOpened() ? "true" : "false", mFileMode);
    }

    return result;
}

unsigned int File::setPosition(int offset, IECursorPosition::eCursorPosition startAt) const
{
    return (isOpened() ? _osSetPositionFile(offset, startAt) : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int File::getPosition(void) const
{
    return (isOpened() ? _osGetPositionFile() : IECursorPosition::INVALID_CURSOR_POSITION);
}

unsigned int File::getLength(void) const
{
    unsigned int result{ 0 };
    if (isOpened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.getObject(), err);
        result = !err ? static_cast<unsigned int>(sz) : 0;
    }
    return result;
}

unsigned int File::reserve(unsigned int newSize)
{
    OUTPUT_DBG("Going to reserve [ %u ] of data for file [ %s ].", newSize, mFileName.isEmpty() == false ? static_cast<const char*>(mFileName) : "null");
    unsigned int result = IECursorPosition::INVALID_CURSOR_POSITION;
    if (isOpened() && canWrite())
    {
        unsigned int curPos = _osGetPositionFile();
        close();

        std::error_code err;
        std::filesystem::resize_file(mFileName.getObject(), newSize, err);
        if (open() && !err)
        {
            if (newSize == 0)
            {
                if (moveToBegin())
                {
                    result = IECursorPosition::START_CURSOR_POSITION;
                }
            }
            else if (newSize <= curPos)
            {
                if (moveToEnd())
                {
                    result = newSize;
                }
            }
            else
            {
                result = setPosition(static_cast<int>(curPos), IECursorPosition::eCursorPosition::PositionBegin);
            }
        }
    }

    return result;
}

bool File::truncate(void)
{
    return (isOpened() && canWrite() ? _osTruncateFile() : false);
}

void File::flush(void)
{
    if (isOpened())
    {
        _osFlushFile();
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::deleteFile(const char* filePath)
{
    std::error_code err;
    return (NEString::isEmpty<char>(filePath) == false ? std::filesystem::remove(filePath, err) : false);
}

bool File::createDir(const char* dirPath)
{
    std::error_code err;
    return (NEString::isEmpty<char>(dirPath) == false ? std::filesystem::create_directory(dirPath, err) : false);
}

bool File::deleteDir(const char* dirPath)
{
    bool result{ false };
    if (NEString::isEmpty<char>(dirPath) == false)
    {
        std::error_code err;
        std::filesystem::remove_all(dirPath, err);
        result = !err;
    }

    return result;
}

bool File::moveFile(const char* oldPath, const char* newPath)
{
    bool result{ false };
    if ( (NEString::isEmpty<char>(oldPath) == false) && (NEString::isEmpty<char>(newPath) == false) )
    {
        std::error_code err;
        std::filesystem::rename(oldPath, newPath, err);
        result = !err;
    }
    return result;
}

String File::getCurrentDir(void)
{
    std::error_code err;
    return String(std::filesystem::current_path(err).string());
}

bool File::setCurrentDir(const char* dirPath)
{
    bool result{ false };
    if (NEString::isEmpty<char>(dirPath) == false)
    {
        std::error_code err;
        std::filesystem::current_path(dirPath, err);
        result = !err;
    }

    return result;
}

bool File::copyFile( const char* originPath, const char* newPath, bool copyForce )
{
    bool result{ false };
    if ( (NEString::isEmpty<char>(originPath) == false) && (NEString::isEmpty<char>(newPath) == false) )
    {
        std::filesystem::copy_options opt = copyForce ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::skip_existing;
        std::error_code err;
        result = std::filesystem::copy_file(originPath, newPath, opt, err);
    }

    return result;
}

String File::getTempDir(void)
{
    std::error_code err;
    return String(std::filesystem::temp_directory_path(err).string());
}

bool File::existDir(const char* dirPath)
{
    std::error_code err;
    return (NEString::isEmpty<char>(dirPath) == false ? std::filesystem::is_directory(dirPath, err) : false);
}

bool File::existFile(const char* filePath)
{
    std::error_code err;
    return (NEString::isEmpty<char>(filePath) == false ? std::filesystem::is_regular_file(filePath, err) : false);
}

String File::getFileFullPath(const char* filePath)
{
    String result;
    if (NEString::isEmpty<char>(filePath) == false)
    {
        std::error_code err;
        std::filesystem::path fp = std::filesystem::absolute(filePath, err);
        result = !err ? fp.string() : filePath;
    }

    return result;
}

String File::getSpecialDir(File::eSpecialFolder specialFolder)
{
    String result;
    
    result.reserve(File::MAXIMUM_PATH);
    unsigned int space = result.getCapacity() != 0 ? _osGetSpecialDir(result.getBuffer(), File::MAXIMUM_PATH, specialFolder) : 0;
    result.resize(space);

    return result;
}
