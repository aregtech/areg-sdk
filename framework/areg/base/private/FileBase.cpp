/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/FileBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Base class of file objects.
 *
 ************************************************************************/

#include "areg/base/FileBase.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"

#include <algorithm>
#include <cstring>
#include <memory>
#include <new>
#include <string>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// Local methods.
//////////////////////////////////////////////////////////////////////////
namespace {

//!< Number of characters handled per read of text.
constexpr uint32_t TEXT_CHUNK{ 256u };

//!< Bytes of one character unit of text in a file: narrow text is bytes, wide text is UTF-16LE.
template<typename CharType>
constexpr uint32_t _unit_size() noexcept
{
    return (sizeof(CharType) == 1u ? 1u : 2u);
}

/**
 * \brief   Reads up to 'count' characters at the cursor into 'chars' and returns their number.
 *          'ends[i]' receives the bytes taken in the file by the characters 0 to i.
 **/
template<typename CharType>
uint32_t _read_chars(const FileBase& file, CharType* chars, uint32_t count, uint32_t* ends)
{
    constexpr uint32_t UNIT{ _unit_size<CharType>() };
    if constexpr (sizeof(CharType) == UNIT)
    {
        const uint32_t result{ file.read(reinterpret_cast<uint8_t*>(chars), count * UNIT) / UNIT };
        for (uint32_t i = 0u; i < result; ++i)
        {
            ends[i] = (i + 1u) * UNIT;
        }

        return result;
    }
    else
    {
        uint16_t units[TEXT_CHUNK];
        const uint32_t total{ file.read(reinterpret_cast<uint8_t*>(units), count * UNIT) / UNIT };
        uint32_t result{ 0u };
        for (uint32_t i = 0u; i < total; ++i)
        {
            const uint32_t unit{ units[i] };
            const bool isHigh{ (unit & 0xFC00u) == 0xD800u };
            if (isHigh && ((i + 1u) < total) && ((units[i + 1u] & 0xFC00u) == 0xDC00u))
            {
                chars[result] = static_cast<CharType>(0x10000u + ((unit - 0xD800u) << 10u) + (static_cast<uint32_t>(units[i + 1u]) - 0xDC00u));
                ++i;
            }
            else if (isHigh && ((i + 1u) == total) && (total == count) && (result != 0u))
            {
                break;
            }
            else
            {
                chars[result] = static_cast<CharType>(unit);
            }

            ends[result ++] = (i + 1u) * UNIT;
        }

        return result;
    }
}

/**
 * \brief   Reads text at the cursor and passes it to 'sink' until a stop character, the end of the data
 *          or 'limit' characters. A stop character is a non-printable one, or a line end if 'isLine' is
 *          true; it is consumed and not passed, a CR LF counts as one line end. Returns the number of
 *          characters consumed.
 **/
template<typename CharType, typename Sink>
int32_t _read_text(const FileBase& file, bool isLine, uint32_t limit, Sink sink)
{
    CharType chars[TEXT_CHUNK];
    uint32_t ends[TEXT_CHUNK];
    uint32_t result{ 0u };
    while (result < limit)
    {
        const uint32_t start{ file.position() };
        const uint32_t count{ _read_chars<CharType>(file, chars, std::min(limit - result, TEXT_CHUNK), ends) };
        if (count == 0u)
            break;

        uint32_t take{ 0u };
        while ((take < count) && areg::is_printable<CharType>(chars[take]) && ((isLine == false) || (areg::is_eol<CharType>(chars[take]) == false)))
        {
            ++take;
        }

        sink(chars, take);
        if (take == count)
        {
            file.set_position(static_cast<int32_t>(start + ends[count - 1u]), Cursor::SeekOrigin::Begin);
            result += count;
            continue;
        }

        uint32_t used{ take + 1u };
        const bool isCR{ isLine && areg::is_carriage_return<CharType>(chars[take]) };
        if (isCR && (used < count) && (chars[used] == static_cast<CharType>('\n')))
        {
            ++ used;
        }

        file.set_position(static_cast<int32_t>(start + ends[used - 1u]), Cursor::SeekOrigin::Begin);
        result += used;
        if (isCR && (used == count))
        {
            CharType next{ };
            uint32_t nextEnd{ 0u };
            const uint32_t at{ file.position() };
            const bool isLF{ (_read_chars<CharType>(file, &next, 1u, &nextEnd) == 1u) && (next == static_cast<CharType>('\n')) };
            file.set_position(static_cast<int32_t>(isLF ? at + nextEnd : at), Cursor::SeekOrigin::Begin);
            result += isLF ? 1u : 0u;
        }

        break;
    }

    return static_cast<int32_t>(result);
}

template<typename CharType, typename ClassType>
inline int32_t _read_into(const FileBase& file, ClassType& outValue, bool isLine)
{
    outValue.clear();
    if (!file.is_opened() || !file.can_read())
        return 0;

    return _read_text<CharType>(file, isLine, static_cast<uint32_t>(areg::VALUE_MAX_INT32),
                                [&outValue](const CharType* chars, uint32_t count)
                                {
                                    outValue.append(chars, static_cast<areg::CharCount>(count));
                                });
}

template<typename CharType>
inline int32_t _read_into(const FileBase& file, CharType* buffer, int32_t charCount, bool isLine)
{
    if (!file.is_opened() || !file.can_read() || (buffer == nullptr) || (charCount <= 1))
        return 0;

    uint32_t stored{ 0u };
    const int32_t result{ _read_text<CharType>(file, isLine, static_cast<uint32_t>(charCount) - 1u,
                                               [buffer, &stored](const CharType* chars, uint32_t count)
                                               {
                                                   areg::mem_copy(buffer + stored, count * sizeof(CharType), chars, count * sizeof(CharType));
                                                   stored += count;
                                               }) };
    buffer[stored] = static_cast<CharType>(areg::EndOfString);
    return result;
}

//!< Writes the characters as text units; returns true if all of them were written.
template<typename CharType>
inline bool _write_chars(FileBase& file, const CharType* buffer, uint32_t count)
{
    if (count == 0u)
        return true;

    if constexpr (sizeof(CharType) == _unit_size<CharType>())
    {
        const uint32_t size{ count * static_cast<uint32_t>(sizeof(CharType)) };
        return (file.write(reinterpret_cast<const uint8_t*>(buffer), size) == size);
    }
    else
    {
        uint32_t units{ count };
        for (uint32_t i = 0u; i < count; ++i)
        {
            units += ((static_cast<uint32_t>(buffer[i]) - 0x10000u) <= 0xFFFFFu) ? 1u : 0u;
        }

        return (file.write_units(buffer, count) == units * _unit_size<CharType>());
    }
}

template<typename CharType>
inline bool _writeString(FileBase& file, const CharType* buffer, int32_t strLen = static_cast<int32_t>(areg::COUNT_ALL))
{
    if (!file.is_opened() || !file.can_write() || (buffer == nullptr))
        return false;

    // In binary mode include null terminator; in text mode exclude it
    uint32_t len = (strLen >= 0) ? static_cast<uint32_t>(strLen) : static_cast<uint32_t>(areg::string_length<CharType>(buffer));
    if (file.is_binary_mode())
    {
        const CharType eos{ static_cast<CharType>(areg::EndOfString) };
        return _write_chars(file, buffer, len) && _write_chars(file, &eos, 1u);
    }

    return _write_chars(file, buffer, len);
}

template<typename CharType>
inline bool _writeLine(FileBase& file, const CharType* buffer)
{
    if (!file.is_opened() || !file.can_write() || (buffer == nullptr))
        return false;

    const CharType eol{ static_cast<CharType>(areg::EndOfLine) };
    return _write_chars(file, buffer, static_cast<uint32_t>(areg::string_line_length<CharType>(buffer))) && _write_chars(file, &eol, 1u);
}

template<typename CharType>
uint32_t _searchText( const FileBase&   file
                    , uint32_t          startPos
                    , const CharType*   text
                    , uint32_t          length
                    , bool              sensitive )
{
    if (!file.is_opened()
        || !file.can_read()
        || (startPos == Cursor::INVALID_CURSOR_POSITION)
        || areg::is_empty<CharType>(text)
        || (length == 0)
        || (length > (UINT32_MAX / 2u)))
    {
        return Cursor::INVALID_CURSOR_POSITION;
    }

    uint32_t posSearch = file.set_position(static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin);

    const uint32_t winSize  = 2u * length;
    const uint32_t charSize = static_cast<uint32_t>(sizeof(CharType));

    std::unique_ptr<CharType[]> buffer{ new (std::nothrow) CharType[winSize] };
    if (buffer == nullptr)
    {
        return Cursor::INVALID_CURSOR_POSITION;
    }

    CharType* const fileData = buffer.get();

    // comparator, do not recreate lambda per iteration
    auto matches = [&](const CharType* candidate) -> bool
    {
        return areg::compare_strings<CharType, CharType>(candidate, text, static_cast<areg::CharCount>(length), sensitive) == areg::Ordering::Equal;
    };

    uint32_t inBuf = 0u;
    while (true)
    {
        // Slide window: keep the back half for overlap between chunks
        if (inBuf >= length)
        {
            areg::mem_move(fileData, fileData + length, (inBuf - length) * charSize);
            inBuf -= length;
        }

        const uint32_t toRead  = (winSize - inBuf) * charSize;
        const uint32_t didRead = file.read(reinterpret_cast<uint8_t*>(fileData + inBuf), toRead) / charSize;
        inBuf += didRead;
        if (inBuf < length)
            break;  // not enough data to match

        const uint32_t scanEnd = inBuf - length;
        for (uint32_t i = 0u; i <= scanEnd; ++i)
        {
            if (matches(fileData + i))
                return posSearch + i * charSize;
        }

        posSearch += length * charSize;
        if (didRead == 0)
            break;  // EOF
    }

    return Cursor::INVALID_CURSOR_POSITION;
}

//!< Searches wide text, which is UTF-16LE in the file.
inline uint32_t _searchWide(const FileBase& file, uint32_t startPos, const wchar_t* text, uint32_t length, bool sensitive)
{
    if constexpr (sizeof(wchar_t) == 2u)
    {
        return _searchText<wchar_t>(file, startPos, text, length, sensitive);
    }
    else
    {
        std::u16string units;
        units.reserve(static_cast<size_t>(length) * 2u);
        for (uint32_t i = 0u; i < length; ++i)
        {
            const uint32_t code{ static_cast<uint32_t>(text[i]) };
            if (code < 0x10000u)
            {
                units += static_cast<char16_t>(code);
            }
            else if (code <= 0x10FFFFu)
            {
                units += static_cast<char16_t>(0xD800u | ((code - 0x10000u) >> 10u));
                units += static_cast<char16_t>(0xDC00u | ((code - 0x10000u) & 0x3FFu));
            }
            else
            {
                units += static_cast<char16_t>(0xFFFDu);
            }
        }

        return _searchText<char16_t>(file, startPos, units.c_str(), static_cast<uint32_t>(units.size()), sensitive);
    }
}

} // namespace

//////////////////////////////////////////////////////////////////////////
// FileBase class implementation.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
FileBase::FileBase()
    : IOStream      ( )
    , Cursor        ( )

    , mFileName     (String::empty_string())
    , mFileMode     (static_cast<uint32_t>(OpenMode::Invalid))
    , mReadConvert  (static_cast<InStream &>(self()), static_cast<Cursor &>(self()) )
    , mWriteConvert (static_cast<OutStream &>(self()), static_cast<Cursor &>(self()) )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

uint32_t FileBase::normalize_mode(uint32_t mode) const noexcept
{
    if ((mode != static_cast<uint32_t>(OpenMode::Invalid)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Read);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitWrite)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Write);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitTemp)) != 0)
    {
        // A temporary file is created when it is opened and deleted when it is closed.
        mode &= ~(static_cast<uint32_t>(OpenFlag::BitExist) | static_cast<uint32_t>(OpenFlag::BitOpenAlways));
        mode |= static_cast<uint32_t>(OpenMode::CreateTemp) | static_cast<uint32_t>(OpenFlag::BitCreateNew) | static_cast<uint32_t>(OpenFlag::BitDelete);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitText)) != 0)
    {
        mode &= ~static_cast<uint32_t>(OpenFlag::BitBinary);
        mode |= static_cast<uint32_t>(OpenMode::Text);
    }
    else
    {
        mode |= static_cast<uint32_t>(OpenMode::Binary);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitBinary)) != 0)
    {
        mode &= ~static_cast<uint32_t>(OpenFlag::BitText);
        mode |= static_cast<uint32_t>(OpenMode::Binary);
    }

    if (((mode & static_cast<uint32_t>(OpenFlag::BitCreateNew)) != 0) && ((mode & static_cast<uint32_t>(OpenFlag::BitExist)) != 0))
    {
        mode &= ~(  static_cast<uint32_t>(OpenFlag::BitCreateNew)
                  | static_cast<uint32_t>(OpenFlag::BitExist)
                  | static_cast<uint32_t>(OpenFlag::BitTruncate));
        mode |= (   static_cast<uint32_t>(OpenMode::OpenAlways)
                  | static_cast<uint32_t>(OpenMode::Write)
                  | static_cast<uint32_t>(OpenMode::Read));
    }
    else if ((mode & static_cast<uint32_t>(OpenFlag::BitOpenAlways)) != 0)
    {
        mode &= ~(   static_cast<uint32_t>(OpenFlag::BitCreateNew)
                   | static_cast<uint32_t>(OpenFlag::BitExist)
                   | static_cast<uint32_t>(OpenFlag::BitTruncate)
                 );
        mode |= static_cast<uint32_t>(OpenMode::Write);
        mode |= static_cast<uint32_t>(OpenMode::Read);
    }
    else if ((mode & static_cast<uint32_t>(OpenFlag::BitCreateNew)) != 0)
    {
        mode &= ~static_cast<uint32_t>(OpenMode::Exist);
        mode |= static_cast<uint32_t>(OpenMode::Create);
        mode |= static_cast<uint32_t>(OpenMode::Write);
    }
    else if (((mode & static_cast<uint32_t>(OpenFlag::BitWrite)) == 0) && ((mode & static_cast<uint32_t>(OpenFlag::BitRead)) != 0))
    {
        mode |= static_cast<uint32_t>(OpenFlag::BitExist);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitExist)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Exist);
    }
    else
    {
        mode &= ~static_cast<uint32_t>(OpenFlag::BitTruncate);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitTruncate)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Truncate);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitDirect)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::WriteDirect);
    }

    return mode;
}

int32_t FileBase::append( const uint8_t* buffer, uint32_t length )
{
    uint32_t result = 0;
    if ((length != 0) && (buffer != nullptr) && is_opened() && can_write())
    {
        move_to_end();
        result = write(buffer, length);
    }

    return static_cast<int32_t>(result);
}

int32_t FileBase::read_invert( uint8_t * buffer, uint32_t length ) const
{
    uint32_t result = 0;
    if ((length != 0) && (buffer != nullptr) && is_opened() && can_read())
    {
        result = read(buffer, length);
        uint32_t count = result / 2;
        for (uint32_t i = 0; i < count; ++ i)
        {
            uint8_t ch = buffer[i];
            buffer[i] = buffer[length - 1 - i];
            buffer[length - 1 - i] = ch;
        }
    }

    return static_cast<int32_t>(result);
}

int32_t FileBase::write_invert( const uint8_t * buffer, uint32_t length )
{
    uint32_t result = 0;
    if ((length != 0) && (buffer != nullptr) && is_opened() && can_write())
    {
        uint8_t* temp = new (std::nothrow) uint8_t[length];
        if (temp != nullptr )
        {
            for (uint32_t i = 0; i < length; ++ i)
                temp[i] = buffer[length - 1 - i];
            
            result = write(temp, length);
            delete [] temp;
        }
    }

    return static_cast<int32_t>(result);
}

int32_t FileBase::read_string( char * buffer, int32_t charCount) const
{
    return _read_into<char>(self(), buffer, charCount, false);
}

int32_t FileBase::read_string( wchar_t * buffer, int32_t charCount ) const
{
    return _read_into<wchar_t>(self(), buffer, charCount, false);
}

int32_t FileBase::read_string(String & outValue ) const
{
    return _read_into<char>(self(), outValue, false);
}

int32_t FileBase::read_string(WideString & outValue) const
{
    return _read_into<wchar_t>(self(), outValue, false);
}

int32_t FileBase::read_line( char * buffer, int32_t charCount) const
{
    return _read_into<char>(self(), buffer, charCount, true);
}

int32_t FileBase::read_line( wchar_t * buffer, int32_t charCount ) const
{
    return _read_into<wchar_t>(self(), buffer, charCount, true);
}

int32_t FileBase::read_line( String & buffer) const
{
    return _read_into<char>(self(), buffer, true);
}

int32_t FileBase::read_line(WideString & buffer) const
{
    return _read_into<wchar_t>(self(), buffer, true);
}

bool FileBase::write_string( const char* buffer )
{
    return _writeString<char>(self(), buffer, -1);
}

bool FileBase::write_string( const wchar_t* buffer)
{
    return _writeString<wchar_t>(self(), buffer, -1);
}

bool FileBase::write_string(const String& buffer)
{
    return _writeString<char>(self(), buffer.as_string(), static_cast<int32_t>(buffer.length()));
}

bool FileBase::write_string(const WideString& buffer)
{
    return _writeString<wchar_t>(self(), buffer.as_string(), static_cast<int32_t>(buffer.length()));
}

bool FileBase::write_line( const char* buffer)
{
    return _writeLine<char>(self(), buffer);
}

bool FileBase::write_line( const wchar_t* buffer)
{
    return _writeLine<wchar_t>(self(), buffer);
}

bool FileBase::write_line(const String& buffer)
{
    return _writeLine<char>(self(), buffer);
}

bool FileBase::write_line(const WideString& buffer)
{
    return _writeLine<wchar_t>(self(), buffer);
}

uint32_t FileBase::resize_and_fill(uint32_t newSize, uint8_t fillValue )
{
    const uint32_t curPos{ position() };
    if (newSize == 0u)
        return curPos;

    const uint64_t oldLength{ length() };
    if (reserve(newSize) == Cursor::INVALID_CURSOR_POSITION)
        return Cursor::INVALID_CURSOR_POSITION;

    if (newSize > oldLength)
    {
        constexpr uint32_t chunkSize{ 256u };
        uint8_t chunk[chunkSize];
        ::memset(chunk, fillValue, chunkSize);

        uint32_t remain{ newSize - static_cast<uint32_t>(oldLength) };
        set_position(static_cast<int32_t>(oldLength), Cursor::SeekOrigin::Begin);
        while (remain != 0u)
        {
            const uint32_t count{ remain < chunkSize ? remain : chunkSize };
            if (write(chunk, count) != count)
                break;

            remain -= count;
        }

        set_position(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
    }

    return position();
}

void FileBase::reset() const noexcept
{
    set_position(0, Cursor::SeekOrigin::Begin);
}

uint32_t FileBase::read(SharedBuffer& buffer) const
{
    uint32_t result = 0;
    buffer.invalidate();

    if ( is_opened() && can_read() )
    {
        const uint32_t curPos{ position() };
        int32_t sizeData{ 0 };
        const bool hasSize{ read_int(sizeData) };
        if (hasSize && (sizeData == 0))
        {
            result = sizeof(int32_t);
        }
        else if (hasSize && (sizeData > 0))
        {
            const uint32_t size{ static_cast<uint32_t>(sizeData) };
            uint8_t * data = buffer.reserve(size, false) >= size ? buffer.buffer() : nullptr;
            if ( (data != nullptr) && (read(data, size) == size) )
            {
                buffer.set_size_used(size);
                buffer.move_to_begin();
                result = size + sizeof(int32_t);
            }
        }

        if (result == 0u)
        {
            buffer.invalidate();
            set_position(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
        }
    }

    return result;
}

uint32_t FileBase::write(const SharedBuffer& buffer)
{
    uint32_t result = 0;

    if ( is_opened() && can_write() )
    {
        const uint8_t * data  = buffer.buffer();
        uint32_t sizeUsed     = buffer.size_used();

        if (write_int(static_cast<int32_t>(sizeUsed)) && (write(data, sizeUsed) == sizeUsed))
        {
            result = sizeUsed + sizeof(int32_t);
        }
    }

    return result;
}

bool FileBase::write(const char * ascii)
{
    return write_string(ascii);
}

bool FileBase::write(const wchar_t * wide)
{
    return write_string(wide);
}

uint32_t FileBase::search_data( uint32_t        startPos
                               , const uint8_t*  buffer
                               , uint32_t        length ) const
{
    if (!is_opened()
        || !can_read()
        || (startPos == Cursor::INVALID_CURSOR_POSITION)
        || (buffer == nullptr)
        || (length == 0)
        || (length > (UINT32_MAX / 2u)))
    {
        return Cursor::INVALID_CURSOR_POSITION;
    }

    uint32_t posSearch = set_position(static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin);
    if (posSearch == Cursor::INVALID_CURSOR_POSITION)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t winSize = 2u * length;
    std::unique_ptr<uint8_t[]> storage{ new (std::nothrow) uint8_t[winSize] };
    if (storage == nullptr)
        return Cursor::INVALID_CURSOR_POSITION;

    uint8_t* const fileData = storage.get();

    // comparator, avoid recreating lambda per iteration
    auto matches = [&](const uint8_t* candidate) -> bool
                    {
                        return areg::mem_compare(candidate, buffer, length) == areg::Ordering::Equal;
                    };

    uint32_t inBuf = 0u;  // valid bytes currently in buffer
    while (true)
    {
        // Slide window: preserve back half for cross-chunk overlap
        if (inBuf >= length)
        {
            areg::mem_move(fileData, fileData + length, inBuf - length);
            inBuf -= length;
        }

        const uint32_t didRead = read(fileData + inBuf, winSize - inBuf);
        inBuf += didRead;

        if (inBuf < length)
            break;  // not enough data to match

        // Fast scan: use memchr to skip to first candidate byte
        const uint8_t  firstByte = buffer[0];
        const uint8_t* scanBegin = fileData;
        const uint8_t* scanEnd   = fileData + inBuf - length;

        while (scanBegin <= scanEnd)
        {
            const uint8_t* hit = static_cast<const uint8_t*>(::memchr(scanBegin, firstByte, static_cast<std::size_t>(scanEnd - scanBegin) + 1u));

            if (hit == nullptr)
                break;

            if (matches(hit))
                return posSearch + static_cast<uint32_t>(hit - fileData);

            scanBegin = hit + 1u;
        }

        posSearch += length;
        if (didRead == 0)
            break;  // EOF
    }

    return Cursor::INVALID_CURSOR_POSITION;
}

uint32_t FileBase::search_data( uint32_t startPos, const SharedBuffer& buffer ) const
{
    return search_data(startPos, buffer.buffer(), buffer.size_used());
}

uint32_t FileBase::search_text( uint32_t startPos, const char * text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text, static_cast<uint32_t>(areg::string_length<char>( text )), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const wchar_t * text, bool caseSensitive ) const
{
    return _searchWide( *this, startPos, text, static_cast<uint32_t>(areg::string_length<wchar_t>( text )), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const String & text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text.as_string(), static_cast<uint32_t>(text.length()), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const WideString & text, bool caseSensitive ) const
{
    return _searchWide( *this, startPos, text.as_string( ), static_cast<uint32_t>(text.length( )), caseSensitive );
}

void FileBase::flush() noexcept
{
}

void FileBase::normalize_name(String & name)
{
    // replace all "%time%"
    char fmt[128] { 0 };
    areg::CalendarTime st;
    DateTime::now(st, true);
    String::format_string(fmt, 128, FileBase::TIMESTAMP_FORMAT.data(), st.stYear, st.stMonth, st.stDay, st.stHour, st.stMinute, st.stSecond, st.stMillisecs);
    name.replace(FileBase::FILE_MASK_TIMESTAMP, fmt, areg::START_POS, true);

    // replace all "%appname%"
    const String & appName = Process::instance().app_name();
    name.replace(FileBase::FILE_MASK_APPNAME, appName, areg::START_POS, true);
}

const uint8_t* FileBase::read_view(uint32_t /*size*/) const noexcept
{
    return nullptr;
}

uint8_t* FileBase::write_view(uint32_t /*size*/) noexcept
{
    return nullptr;
}

} // namespace areg
