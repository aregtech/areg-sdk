/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/File.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, File object
 *              common parts
 ************************************************************************/

#include "areg/base/File.hpp"

#include "areg/base/SharedBuffer.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/base/Containers.hpp"

#include <filesystem>

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // File class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Constructors / destructor
    //////////////////////////////////////////////////////////////////////////
    File::File()
        : FileBase    ( )
        , mFileHandle   (File::_osGetInvalidHandle())
    {
    }

    File::File(const areg::String& fileName, uint32_t mode /* = (static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary)) */)
        : FileBase    ( )
        , mFileHandle   (File::_osGetInvalidHandle())
    {
        mFileName = File::normalizePath(fileName);
        mFileMode = mode;
    }

    File::~File()
    {
        _osCloseFile();

        mFileHandle = File::_osGetInvalidHandle();
        mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
    }

    //////////////////////////////////////////////////////////////////////////
    // Methods
    //////////////////////////////////////////////////////////////////////////

    bool File::open(const areg::String& fileName, uint32_t mode)
    {
        bool result = false;
        if ((isOpened() == false) && (fileName.isEmpty() == false))
        {
            mFileMode = mode;
            mFileName = File::normalizePath(fileName);

            result = open();
        }

        return result;
    }

    bool File::open()
    {
        return _osOpenFile();
    }

    void File::close()
    {
        _osCloseFile();
    }

    uint32_t File::getSizeReadable() const
    {
        uint32_t lenRead = 0;
        uint32_t lenUsed = 0;
        if (isOpened())
        {
            std::error_code err;
            std::uintmax_t sz = std::filesystem::file_size(mFileName.getData(), err);

            lenRead = _osGetPositionFile();
            lenUsed = !err ? static_cast<uint32_t>(sz) : 0;
        }

        ASSERT(lenRead <= lenUsed);
        return (lenUsed - lenRead);
    }

    uint32_t File::getSizeWritable() const
    {
        uint32_t lenWritten     = 0;
        uint32_t lenAvailable   = 0;
        if (isOpened())
        {
            std::error_code err;
            std::uintmax_t sz = std::filesystem::file_size(mFileName.getData(), err);

            lenWritten  = _osGetPositionFile();
            lenAvailable = !err ? static_cast<uint32_t>(sz) : 0;
        }

        ASSERT(lenWritten <= lenAvailable);
        return (lenAvailable - lenWritten);
    }

    bool File::remove()
    {
        bool result = false;

        if (isOpened())
        {
            ASSERT(mFileName.isEmpty() == false);

            close();
            std::error_code err;
            result = std::filesystem::remove(mFileName.getData(), err);
        }

        mFileHandle = File::_osGetInvalidHandle();
        mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
        mFileName   = areg::String::getEmptyString();

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
        if (areg::isEmpty<char>(filePath) == false)
        {
            if (filePath[0] == File::DIR_CURRENT[0])
            {
                result = (skipSep && filePath[1] == areg::EndOfString)  || 
                        (filePath[1] == File::UNIX_SEPARATOR)              ||
                        (filePath[1] == File::DOS_SEPARATOR);
            }
        }

        return result;
    }

    inline bool File::_nameHasParentFolder(const char * filePath, bool skipSep)
    {
        bool result{ false };
        if (areg::isEmpty<char>(filePath) == false)
        {
            if ((filePath[0] == File::DIR_PARENT[0]) && (filePath[1] == File::DIR_PARENT[1]))
            {
                result = (skipSep && filePath[2] == areg::EndOfString) || (filePath[2] == File::UNIX_SEPARATOR) ||  (filePath[2] == File::DOS_SEPARATOR);
            }
        }

        return result;
    }

    areg::String File::genTempFileName(const char* prefix, bool unique, bool inTempFolder)
    {
        char buffer[File::MAXIMUM_PATH];
        areg::String pref(prefix == nullptr ? areg::Process::getInstance().getAppName().getString() : prefix);
        areg::String name;
        if (unique)
        {
            uint32_t ticks = unique ? static_cast<uint32_t>(DateTime::getSystemTickCount()) : 0u;
            DateTime timestamp{ DateTime::getNow() };
            int32_t len = areg::String::formatString( buffer, File::MAXIMUM_PATH, "%s%u%u%llu"
                                        , pref.getString()
                                        , static_cast<uint32_t>(areg::Process::getInstance().getId())
                                        , ticks
                                        , timestamp.getTime());
            name.assign(buffer, len > 0 ? len : 0);
        }
        else
        {
            name = pref;
        }

        if (inTempFolder)
        {
            areg::String dir = File::getTempDir();
            uint32_t len = _osCreateTempFileName(buffer, dir.getString(), name.getString(), unique);
            if (len != 0)
            {
                name.assign(buffer, static_cast<areg::CharCount>(len));
            }
            else
            {
                std::filesystem::path filePath = std::filesystem::path(dir.getString()) / name.getData();
                name = filePath.string();
            }
        }
        else
        {
            areg::String dir = File::getCurrentDir();
            std::filesystem::path filePath = std::filesystem::path(dir.getString()) / name.getData();
            name = filePath.string();
        }

        return name;
    }

    areg::String File::genTempFileName()
    {
        return genTempFileName(nullptr, true, true);
    }

    const areg::String & File::getExecutableDir()
    {
        return areg::Process::getInstance().getPath();
    }

    areg::String File::getFileNameWithExtension( const char* filePath )
    {
        const char * result = nullptr;
        if ( areg::isEmpty<char>(filePath) == false )
        {
            areg::CharPos pos = areg::getStringLength<char>(filePath) - 1;
            if (filePath[pos] != File::PATH_SEPARATOR )
            {
                pos = areg::findLast<char>( File::PATH_SEPARATOR, filePath, pos - 1, true, nullptr);
                if (areg::isPositionValid(pos))
                {
                    result = filePath + pos + 1;
                }
            }
        }

        return (result != nullptr ? areg::String(result) : areg::String::getEmptyString());
    }

    areg::String File::getFileName( const char* filePath )
    {
        areg::String result(File::getFileNameWithExtension(filePath));
        areg::CharPos pos = result.findLast(File::EXTENSION_SEPARATOR, areg::END_POS, true);
        if (areg::isPositionValid(pos) && areg::isAlphanumeric<char>(result[pos + 1]) )
        {
            result.substring(0, pos);
        }

        return result;
    }

    areg::String File::getFileExtension( const char* filePath )
    {
        areg::String result;
        areg::String fileName(File::getFileNameWithExtension(filePath));
        areg::CharPos pos = fileName.findLast(File::EXTENSION_SEPARATOR, areg::END_POS, true);
        if (areg::isPositionValid(pos) && areg::isAlphanumeric<char>(fileName[pos + 1]) )
        {
            fileName.substring(result, pos, areg::COUNT_ALL);
        }

        return result;
    }

    areg::String File::getFileDirectory(const char* filePath)
    {
        constexpr char separator{ File::PATH_SEPARATOR };
        areg::CharPos pos = areg::isEmpty<char>(filePath) ? areg::INVALID_POS : areg::findLast<char>( separator, filePath, areg::END_POS, true, nullptr);
        if ( areg::isPositionValid( pos ) )
        {
            return areg::String( filePath, static_cast<uint32_t>(*(filePath + pos) == separator ? pos : pos + 1) );
        }
        else
        {
            return areg::String::EmptyString;
        }
    }

    bool File::createDirCascaded( const char* dirPath )
    {
        bool result{ false };
        if ( areg::isEmpty<char>( dirPath ) == false )
        {
            std::error_code err;
            std::filesystem::create_directories( dirPath, err );
            result = static_cast<bool>(err) == false;
        }

        return result;
    }

    areg::String File::normalizePath(const char* fileName)
    {
        areg::String result;
        if (areg::isEmpty<char>(fileName) == false)
        {
            result = fileName;
            FileBase::normalizeName(result);
            std::error_code err;
            std::filesystem::path fp = std::filesystem::absolute(result.getData(), err);
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
        if ( areg::isEmpty<char>(filePath) == false)
        {
            int32_t length = 0;
            if (nextPos != nullptr)
                *nextPos = nullptr;
            
            if (lastPos != nullptr)
            {
                length = lastPos == filePath ? 0 : static_cast<int32_t>(lastPos - filePath - 1);
            }
            else
            {
                length = areg::getStringLength<char>(filePath); 
                if ( filePath[length - 1] == File::PATH_SEPARATOR )
                    -- length;
            }

            if (length != 0)
            {
                int32_t pos = areg::findLast( File::PATH_SEPARATOR, filePath, areg::END_POS, nextPos);
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

    areg::String File::getParentDir(const char * filePath)
    {
        areg::String result;
        const char * end = nullptr;
        if (File::findParent(filePath, &end))
        {
            result.assign(filePath, static_cast<areg::CharCount>(end - filePath) );
        }

        return result;
    }

    int32_t File::splitPath(const char * filePath, StringList & in_out_List)
    {
        int32_t oldCount        { static_cast<int32_t>(in_out_List.getSize()) };
        const char * start  { filePath };
        const char * end    { filePath };

        while (*end != areg::EndOfString)
        {
            if ((*end == File::UNIX_SEPARATOR) || (*end == File::DOS_SEPARATOR))
            {
                areg::String node(start, static_cast<uint32_t>(end - start));
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
            areg::String node(start, static_cast<uint32_t>(end - start));
            if (node.isEmpty() == false)
                in_out_List.pushLast( node );
        }

        return static_cast<int32_t>(in_out_List.getSize() - static_cast<uint32_t>(oldCount));
    }

    areg::String File::makeFileFullPath(const char* dirName, const char* fileName)
    {
        std::error_code err;
        std::filesystem::path filePath(dirName);
        filePath /= std::string(fileName);
        std::filesystem::path fp = std::filesystem::absolute(filePath, err);
        return (!err ? fp.string() : filePath.string());
    }

    uint32_t File::read(ByteBuffer & buffer) const
    {
        return FileBase::read(buffer);
    }

    uint32_t File::read(areg::String & ascii) const
    {
        return FileBase::read(ascii);
    }

    uint32_t File::read(areg::WideString & wide) const
    {
        return FileBase::read(wide);
    }

    uint32_t File::read(uint8_t* buffer, uint32_t size) const
    {
        uint32_t result = 0;
        if (isOpened() && canRead())
        {
            if ((buffer != nullptr) && (size > 0))
            {
                result = _osReadFile(buffer, size);
            }
        }

        return result;
    }

    uint32_t File::write(const ByteBuffer & buffer)
    {
        return FileBase::write(buffer);
    }

    uint32_t File::write(const areg::String & ascii)
    {
        return FileBase::write(ascii);
    }

    uint32_t File::write(const areg::WideString & wide)
    {
        return FileBase::write(wide);
    }

    uint32_t File::write(const uint8_t* buffer, uint32_t size)
    {
        uint32_t result = 0;
        if (isOpened() && canWrite())
        {
            if ((buffer != nullptr) && (size > 0))
            {
                result = _osWriteFile(buffer, size);
            }
        }

        return result;
    }

    uint32_t File::setPosition(int32_t offset, Cursor::SeekOrigin startAt) const
    {
        return (isOpened() ? _osSetPositionFile(offset, startAt) : Cursor::INVALID_CURSOR_POSITION);
    }

    uint32_t File::getPosition() const
    {
        return (isOpened() ? _osGetPositionFile() : Cursor::INVALID_CURSOR_POSITION);
    }

    uint32_t File::getLength() const
    {
        uint32_t result{ 0 };
        if (isOpened())
        {
            std::error_code err;
            std::uintmax_t sz = std::filesystem::file_size(mFileName.getData(), err);
            result = !err ? static_cast<uint32_t>(sz) : 0;
        }
        return result;
    }

    uint32_t File::reserve(uint32_t newSize)
    {
        uint32_t result = Cursor::INVALID_CURSOR_POSITION;
        if (isOpened() && canWrite())
        {
            uint32_t curPos = _osGetPositionFile();
            close();

            std::error_code err;
            std::filesystem::resize_file(mFileName.getData(), newSize, err);
            if (open() && !err)
            {
                if (newSize == 0)
                {
                    if (moveToBegin())
                    {
                        result = Cursor::START_CURSOR_POSITION;
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
                    result = setPosition(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
                }
            }
        }

        return result;
    }

    bool File::truncate()
    {
        return (isOpened() && canWrite() ? _osTruncateFile() : false);
    }

    void File::flush()
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
        return (areg::isEmpty<char>(filePath) == false ? std::filesystem::remove(filePath, err) : false);
    }

    bool File::createDir(const char* dirPath)
    {
        std::error_code err;
        return (areg::isEmpty<char>(dirPath) == false ? std::filesystem::create_directory(dirPath, err) : false);
    }

    bool File::deleteDir(const char* dirPath)
    {
        bool result{ false };
        if (areg::isEmpty<char>(dirPath) == false)
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
        if ( (areg::isEmpty<char>(oldPath) == false) && (areg::isEmpty<char>(newPath) == false) )
        {
            std::error_code err;
            std::filesystem::rename(oldPath, newPath, err);
            result = !err;
        }
        return result;
    }

    areg::String File::getCurrentDir()
    {
        std::error_code err;
        return areg::String(std::filesystem::current_path(err).string());
    }

    bool File::setCurrentDir(const char* dirPath)
    {
        bool result{ false };
        if (areg::isEmpty<char>(dirPath) == false)
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
        if ( (areg::isEmpty<char>(originPath) == false) && (areg::isEmpty<char>(newPath) == false) )
        {
            std::filesystem::copy_options opt = copyForce ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::skip_existing;
            std::error_code err;
            result = std::filesystem::copy_file(originPath, newPath, opt, err);
        }

        return result;
    }

    areg::String File::getTempDir()
    {
        std::error_code err;
        return areg::String(std::filesystem::temp_directory_path(err).string());
    }

    bool File::existDir(const char* dirPath)
    {
        std::error_code err;
        return (areg::isEmpty<char>(dirPath) == false ? std::filesystem::is_directory(dirPath, err) : false);
    }

    bool File::existFile(const char* filePath)
    {
        std::error_code err;
        return (areg::isEmpty<char>(filePath) == false ? std::filesystem::is_regular_file(filePath, err) : false);
    }

    areg::String File::getFileFullPath(const char* filePath)
    {
        areg::String result;
        if (areg::isEmpty<char>(filePath) == false)
        {
            std::error_code err;
            std::filesystem::path fp = std::filesystem::absolute(filePath, err);
            result = !err ? fp.string() : filePath;
        }

        return result;
    }

    areg::String File::getSpecialDir(File::SpecialFolder specialFolder)
    {
        char buffer[File::MAXIMUM_PATH];
        uint32_t space = _osGetSpecialDir(buffer, File::MAXIMUM_PATH, specialFolder);

        return areg::String(buffer, space);
    }

} // namespace areg