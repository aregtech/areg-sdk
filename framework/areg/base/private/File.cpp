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
namespace areg {

//////////////////////////////////////////////////////////////////////////
// File class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructors / destructor
//////////////////////////////////////////////////////////////////////////
File::File()
    : FileBase    ( )
    , mFileHandle   (File::_os_invalid_handle())
{
}

File::File(const String& fileName, uint32_t mode /* = (static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary)) */)
    : FileBase    ( )
    , mFileHandle   (File::_os_invalid_handle())
{
    mFileName = File::normalize_path(fileName);
    mFileMode = mode;
}

File::~File()
{
    _os_close_file();

    mFileHandle = File::_os_invalid_handle();
    mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

bool File::open(const String& fileName, uint32_t mode)
{
    bool result = false;
    if ((is_opened() == false) && (fileName.is_empty() == false))
    {
        mFileMode = mode;
        mFileName = File::normalize_path(fileName);

        result = open();
    }

    return result;
}

bool File::open()
{
    return _os_open_file();
}

void File::close()
{
    _os_close_file();
}

uint32_t File::size_readable() const
{
    uint32_t lenRead = 0;
    uint32_t lenUsed = 0;
    if (is_opened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.data(), err);

        lenRead = _os_file_position();
        lenUsed = !err ? static_cast<uint32_t>(sz) : 0;
    }

    ASSERT(lenRead <= lenUsed);
    return (lenUsed - lenRead);
}

uint32_t File::size_writable() const
{
    uint32_t lenWritten     = 0;
    uint32_t lenAvailable   = 0;
    if (is_opened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.data(), err);

        lenWritten  = _os_file_position();
        lenAvailable = !err ? static_cast<uint32_t>(sz) : 0;
    }

    ASSERT(lenWritten <= lenAvailable);
    return (lenAvailable - lenWritten);
}

bool File::remove()
{
    bool result = false;

    if (is_opened())
    {
        ASSERT(mFileName.is_empty() == false);

        close();
        std::error_code err;
        result = std::filesystem::remove(mFileName.data(), err);
    }

    mFileHandle = File::_os_invalid_handle();
    mFileMode   = static_cast<uint32_t>(FileBase::OpenMode::Invalid);
    mFileName   = String::empty_string();

    return result;
}

bool File::is_opened() const
{
    return (mFileHandle != File::_os_invalid_handle());
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

inline bool File::_has_current_dir(const char * filePath, bool skipSep)
{
    bool result = false;
    if (areg::is_empty<char>(filePath) == false)
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

inline bool File::_has_parent_dir(const char * filePath, bool skipSep)
{
    bool result{ false };
    if (areg::is_empty<char>(filePath) == false)
    {
        if ((filePath[0] == File::DIR_PARENT[0]) && (filePath[1] == File::DIR_PARENT[1]))
        {
            result = (skipSep && filePath[2] == areg::EndOfString) || (filePath[2] == File::UNIX_SEPARATOR) ||  (filePath[2] == File::DOS_SEPARATOR);
        }
    }

    return result;
}

String File::temp_name(const char* prefix, bool unique, bool inTempFolder)
{
    char buffer[File::MAXIMUM_PATH];
    String pref(prefix == nullptr ? Process::instance().app_name().as_string() : prefix);
    String name;
    if (unique)
    {
        uint32_t ticks = unique ? static_cast<uint32_t>(DateTime::system_tick_count()) : 0u;
        DateTime timestamp{ DateTime::now() };
        int32_t len = String::format_string( buffer, File::MAXIMUM_PATH, "%s%u%u%llu"
                                      , pref.as_string()
                                      , static_cast<uint32_t>(Process::instance().id())
                                      , ticks
                                      , timestamp.time());
        name.assign(buffer, len > 0 ? len : 0);
    }
    else
    {
        name = pref;
    }

    if (inTempFolder)
    {
        String dir = File::temp_dir();
        uint32_t len = _os_temp_name(buffer, dir.as_string(), name.as_string(), unique);
        if (len != 0)
        {
            name.assign(buffer, static_cast<areg::CharCount>(len));
        }
        else
        {
            std::filesystem::path filePath = std::filesystem::path(dir.as_string()) / name.data();
            name = filePath.string();
        }
    }
    else
    {
        String dir = File::current_dir();
        std::filesystem::path filePath = std::filesystem::path(dir.as_string()) / name.data();
        name = filePath.string();
    }

    return name;
}

String File::temp_name()
{
    return temp_name(nullptr, true, true);
}

const String & File::executable_dir()
{
    return Process::instance().path();
}

String File::name_with_extension( const char* filePath )
{
    if (areg::is_empty<char>(filePath))
        return String::empty_string();

    areg::CharPos pos = areg::string_length<char>(filePath) - 1;
    if (filePath[pos] == File::PATH_SEPARATOR)
        return String::empty_string();

    const char* result = nullptr;
    pos = areg::find_last<char>(File::PATH_SEPARATOR, filePath, pos - 1, true, nullptr);
    if (areg::is_position_valid(pos))
    {
        result = filePath + pos + 1;
    }

    return (result != nullptr ? String(result) : String::empty_string());
}

String File::file_name( const char* filePath )
{
    String result(File::name_with_extension(filePath));
    areg::CharPos pos = result.find_last(File::EXTENSION_SEPARATOR, areg::END_POS, true);
    if (areg::is_position_valid(pos) && areg::is_alphanumeric<char>(result[pos + 1]) )
    {
        result.substring(0, pos);
    }

    return result;
}

String File::file_extension( const char* filePath )
{
    String result;
    String fileName(File::name_with_extension(filePath));
    areg::CharPos pos = fileName.find_last(File::EXTENSION_SEPARATOR, areg::END_POS, true);
    if (areg::is_position_valid(pos) && areg::is_alphanumeric<char>(fileName[pos + 1]) )
    {
        fileName.substring(result, pos, areg::COUNT_ALL);
    }

    return result;
}

String File::file_directory(const char* filePath)
{
    constexpr char separator{ File::PATH_SEPARATOR };
    areg::CharPos pos = areg::is_empty<char>(filePath) ? areg::INVALID_POS : areg::find_last<char>( separator, filePath, areg::END_POS, true, nullptr);
    if (areg::is_position_valid( pos ) )
    {
        return String( filePath, static_cast<uint32_t>(*(filePath + pos) == separator ? pos : pos + 1) );
    }
    else
    {
        return String::EmptyString;
    }
}

bool File::create_dir_cascaded( const char* dirPath )
{
    bool result{ false };
    if (areg::is_empty<char>( dirPath ) == false )
    {
        std::error_code err;
        std::filesystem::create_directories( dirPath, err );
        result = static_cast<bool>(err) == false;
    }

    return result;
}

String File::normalize_path(const char* fileName)
{
    String result;
    if (areg::is_empty<char>(fileName) == false)
    {
        result = fileName;
        FileBase::normalize_name(result);
        std::error_code err;
        std::filesystem::path fp = std::filesystem::absolute(result.data(), err);
        if (!err)
        {
            result = fp.string();
        }
    }

    return result;
}

bool File::find_parent(const char * filePath, const char ** nextPos, const char * lastPos /*= nullptr*/)
{
    if (areg::is_empty<char>(filePath))
        return false;

    int32_t length = 0;
    if (nextPos != nullptr)
        *nextPos = nullptr;

    if (lastPos != nullptr)
    {
        length = lastPos == filePath ? 0 : static_cast<int32_t>(lastPos - filePath - 1);
    }
    else
    {
        length = areg::string_length<char>(filePath);
        if (filePath[length - 1] == File::PATH_SEPARATOR)
            --length;
    }

    if (length == 0)
        return false;

    bool result = false;
    int32_t pos = areg::find_last(File::PATH_SEPARATOR, filePath, areg::END_POS, nextPos);
    if ((pos > 0) && (pos < length))
    {
        result = true;
        if (nextPos != nullptr)
            *nextPos = filePath + pos;
    }

    return result;
}

String File::parent_dir(const char * filePath)
{
    String result;
    const char * end = nullptr;
    if (File::find_parent(filePath, &end))
    {
        result.assign(filePath, static_cast<areg::CharCount>(end - filePath) );
    }

    return result;
}

int32_t File::split_path(const char * filePath, StringList & in_out_List)
{
    int32_t oldCount        { static_cast<int32_t>(in_out_List.size()) };
    const char * start  { filePath };
    const char * end    { filePath };

    while (*end != areg::EndOfString)
    {
        if ((*end == File::UNIX_SEPARATOR) || (*end == File::DOS_SEPARATOR))
        {
            String node(start, static_cast<uint32_t>(end - start));
            if (node.is_empty() == false)
                in_out_List.push_last( node );

            start = ++ end;
        }
        else
        {
            ++ end;
        }
    }

    if (start != end)
    {
        String node(start, static_cast<uint32_t>(end - start));
        if (node.is_empty() == false)
            in_out_List.push_last( node );
    }

    return static_cast<int32_t>(in_out_List.size() - static_cast<uint32_t>(oldCount));
}

String File::make_full_path(const char* dirName, const char* fileName)
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

uint32_t File::read(String & ascii) const
{
    return FileBase::read(ascii);
}

uint32_t File::read(WideString & wide) const
{
    return FileBase::read(wide);
}

uint32_t File::read(uint8_t* buffer, uint32_t size) const
{
    uint32_t result = 0;
    if (is_opened() && can_read())
    {
        if ((buffer != nullptr) && (size > 0))
        {
            result = _os_read_file(buffer, size);
        }
    }

    return result;
}

uint32_t File::write(const ByteBuffer & buffer)
{
    return FileBase::write(buffer);
}

uint32_t File::write(const String & ascii)
{
    return FileBase::write(ascii);
}

uint32_t File::write(const WideString & wide)
{
    return FileBase::write(wide);
}

uint32_t File::write(const uint8_t* buffer, uint32_t size)
{
    uint32_t result = 0;
    if (is_opened() && can_write())
    {
        if ((buffer != nullptr) && (size > 0))
        {
            result = _os_write_file(buffer, size);
        }
    }

    return result;
}

uint32_t File::set_position(int32_t offset, Cursor::SeekOrigin startAt) const
{
    return (is_opened() ? _os_set_position(offset, startAt) : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t File::position() const
{
    return (is_opened() ? _os_file_position() : Cursor::INVALID_CURSOR_POSITION);
}

uint32_t File::length() const
{
    uint32_t result{ 0 };
    if (is_opened())
    {
        std::error_code err;
        std::uintmax_t sz = std::filesystem::file_size(mFileName.data(), err);
        result = !err ? static_cast<uint32_t>(sz) : 0;
    }
    return result;
}

uint32_t File::reserve(uint32_t newSize)
{
    if (!is_opened() || !can_write())
        return Cursor::INVALID_CURSOR_POSITION;

    uint32_t curPos = _os_file_position();
    if (!_os_reserve(newSize))
        return Cursor::INVALID_CURSOR_POSITION;

    // Restore the pointer: keep curPos when the file grew or stayed equal;
    // clamp to the new end when the file shrank past the old position.
    uint32_t result{ Cursor::INVALID_CURSOR_POSITION };
    if (newSize == 0)
    {
        result = move_to_begin() ? Cursor::START_CURSOR_POSITION : Cursor::INVALID_CURSOR_POSITION;
    }
    else if (newSize <= curPos)
    {
        result = move_to_end() ? newSize : Cursor::INVALID_CURSOR_POSITION;
    }
    else
    {
        result = set_position(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
    }

    return result;
}

bool File::truncate()
{
    return (is_opened() && can_write() && _os_truncate_file());
}

void File::flush()
{
    if (is_opened())
    {
        _os_flush_file();
    }
}

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////

bool File::delete_file(const char* filePath)
{
    std::error_code err;
    return (!areg::is_empty<char>(filePath) && std::filesystem::remove(filePath, err) );
}

bool File::create_dir(const char* dirPath)
{
    std::error_code err;
    return (!areg::is_empty<char>(dirPath) && std::filesystem::create_directory(dirPath, err));
}

bool File::delete_dir(const char* dirPath)
{
    std::error_code err;
    return (!areg::is_empty<char>(dirPath) && std::filesystem::remove_all(dirPath, err));
}

bool File::move_file(const char* oldPath, const char* newPath)
{
    if (areg::is_empty<char>(oldPath) || areg::is_empty<char>(newPath))
        return false;

    std::error_code err;
    std::filesystem::rename(oldPath, newPath, err);
    return !err;
}

String File::current_dir()
{
    std::error_code err;
    return String(std::filesystem::current_path(err).string());
}

bool File::set_current_dir(const char* dirPath)
{
    if (areg::is_empty<char>(dirPath))
        return false;

    std::error_code err;
    std::filesystem::current_path(dirPath, err);
    return !err;
}

bool File::copy_file( const char* originPath, const char* newPath, bool copyForce )
{
    if (areg::is_empty<char>(originPath) || areg::is_empty<char>(newPath))
        return false;

    std::filesystem::copy_options opt = copyForce ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::skip_existing;
    std::error_code err;
    return std::filesystem::copy_file(originPath, newPath, opt, err);
}

String File::temp_dir()
{
    std::error_code err;
    return String(std::filesystem::temp_directory_path(err).string());
}

bool File::has_dir(const char* dirPath)
{
    std::error_code err;
    return (areg::is_empty<char>(dirPath) == false ? std::filesystem::is_directory(dirPath, err) : false);
}

bool File::has_file(const char* filePath)
{
    std::error_code err;
    return (areg::is_empty<char>(filePath) == false ? std::filesystem::is_regular_file(filePath, err) : false);
}

String File::file_full_path(const char* filePath)
{
    if (areg::is_empty<char>(filePath))
        return String::empty_string();

    std::error_code err;
    std::filesystem::path fp = std::filesystem::absolute(filePath, err);
    return String(!err ? fp.string() : filePath);
}

String File::special_dir(File::SpecialFolder specialFolder)
{
    char buffer[File::MAXIMUM_PATH];
    uint32_t space = _os_special_dir(buffer, File::MAXIMUM_PATH, specialFolder);

    return String(buffer, space);
}

} // namespace areg
