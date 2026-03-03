/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/FileBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Base class of file objects.
 *
 ************************************************************************/

#include "areg/base/FileBase.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/WideString.hpp"
#include "areg/base/ByteBuffer.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/Process.hpp"

#include <algorithm>
namespace areg {

//////////////////////////////////////////////////////////////////////////
// Local methods.
//////////////////////////////////////////////////////////////////////////
namespace {

template<typename CharType, typename ClassType, typename Parser>
inline int32_t _readParsed(const FileBase& file, ClassType& outValue, Parser parse)
{
    outValue.clear();
    if (!file.is_opened() || !file.can_read())
        return 0;

    constexpr uint32_t  maxBufSize = 1024u;
    constexpr uint32_t  strLength  = maxBufSize - 1u;
    CharType            buffer[maxBufSize];

    uint32_t result = 0u;
    while (true)
    {
        const uint32_t oldPos   = file.position();
        uint32_t readLength     = file.read(reinterpret_cast<uint8_t*>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
        readLength              = std::min(strLength, readLength);
        buffer[readLength]      = static_cast<CharType>(areg::EndOfString);

        if (readLength == 0)
            break;

        CharType*         context = nullptr;
        const CharType*   str     = parse(buffer, static_cast<areg::CharCount>(readLength), &context);

        const uint32_t length = (context != nullptr) ? static_cast<uint32_t>(context - buffer) : readLength;
        outValue += str;
        result   += length;

        // Reposition file cursor to just after consumed portion
        file.set_position(static_cast<int32_t>(oldPos + length * sizeof(CharType)), Cursor::SeekOrigin::Begin);
        // context == buffer + readLength means entire buffer was consumed -> continue
        if (context != (buffer + readLength))
            break;
    }

    return static_cast<int32_t>(result);
}

template<typename CharType, typename ClassType>
inline int32_t _readString(const FileBase& file, ClassType& outValue)
{
    return _readParsed<CharType>(file, outValue,
                        [](CharType* buf, areg::CharCount len, CharType** ctx) -> const CharType*
                        {
                            return areg::printable<CharType>(buf, len, ctx);
                        });
}

template<typename CharType, typename ClassType>
inline int32_t _readLine(const FileBase& file, ClassType& outValue)
{
    return _readParsed<CharType>(file, outValue,
                        [](CharType* buf, areg::CharCount len, CharType** ctx) -> const CharType*
                        {
                            return areg::line<CharType>(buf, len, ctx);
                        });
}

template<typename CharType>
inline int32_t _readString(const FileBase& file, CharType* buffer, int32_t charCount)
{
    if (!file.is_opened() || !file.can_read() || (buffer == nullptr) || (charCount <= 1))
        return 0;

    const uint32_t strLength = static_cast<uint32_t>(charCount) - 1u;
    const uint32_t oldPos    = file.position();

    uint32_t readLength = file.read(reinterpret_cast<uint8_t*>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
    readLength          = std::min(strLength, readLength);
    buffer[readLength]  = static_cast<CharType>(areg::EndOfString);

    if (readLength == 0)
        return 0;

    CharType* context = nullptr;
    areg::printable<CharType>(buffer, static_cast<areg::CharCount>(readLength), &context);
    ASSERT((context == nullptr) || (context >= buffer));

    const uint32_t result = (context != nullptr) ? static_cast<uint32_t>(context - buffer) : readLength;
    file.set_position(static_cast<int32_t>(oldPos + result * sizeof(CharType)), Cursor::SeekOrigin::Begin);

    return static_cast<int32_t>(result);
}

template<typename CharType>
inline int32_t _readLine(const FileBase& file, CharType* buffer, int32_t charCount)
{
    if (!file.is_opened() || !file.can_read() || (buffer == nullptr) || (charCount <= 1))
        return 0;

    const uint32_t strLength = static_cast<uint32_t>(charCount) - 1u;
    const uint32_t oldPos    = file.position();

    uint32_t readLength = file.read(reinterpret_cast<uint8_t*>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
    readLength          = std::min(strLength, readLength);
    buffer[readLength]  = static_cast<CharType>(areg::EndOfString);

    if (readLength == 0)
        return 0;

    CharType* context = nullptr;
    areg::line<CharType>(buffer, static_cast<areg::CharCount>(readLength), &context);
    ASSERT((context == nullptr) || (context >= buffer));

    const uint32_t result = (context != nullptr)
        ? static_cast<uint32_t>(context - buffer)
        : readLength;

    file.set_position(
        static_cast<int32_t>(oldPos + result * sizeof(CharType)),
        Cursor::SeekOrigin::Begin);

    return static_cast<int32_t>(result);
}

template<typename CharType>
inline bool _writeString(FileBase& file, const CharType* buffer, int32_t strLen = static_cast<int32_t>(areg::COUNT_ALL))
{
    if (!file.is_opened() || !file.can_write() || (buffer == nullptr))
        return false;

    // In binary mode include null terminator; in text mode exclude it
    uint32_t len = (strLen >= 0) ? static_cast<uint32_t>(strLen) : static_cast<uint32_t>(areg::string_length<CharType>(buffer));

    if (file.is_binary_mode())
        ++len;  // include null terminator

    const uint32_t byteLen = len * sizeof(CharType);
    return file.write(reinterpret_cast<const uint8_t*>(buffer), byteLen) == byteLen;
}

template<typename CharType>
inline bool _writeLine(FileBase& file, const CharType* buffer)
{
    if (!file.is_opened() || !file.can_write() || (buffer == nullptr))
        return false;

    const uint32_t byteLen = static_cast<uint32_t>(areg::string_line_length<CharType>(buffer)) * sizeof(CharType);
    return (file.write(reinterpret_cast<const uint8_t*>(buffer), byteLen) == byteLen) && file.write_char(StringBase<CharType>::NewLine);
}

template<typename DataType>
areg::Ordering _compareData( const DataType * memBuffer1, const DataType * memBuffer2, std::function<areg::Ordering( const DataType *, const DataType * )> func )
{
    return func( memBuffer1, memBuffer2 );
}

template<typename CharType>
uint32_t _searchText( const FileBase&   file
                    , uint32_t          startPos
                    , const CharType*   text
                    , uint32_t          length
                    , bool              sensitive )
{
    if (!file.can_read()
        || (startPos == Cursor::INVALID_CURSOR_POSITION)
        || areg::is_empty<CharType>(text)
        || (length == 0))
    {
        return Cursor::INVALID_CURSOR_POSITION;
    }

    uint32_t posSearch = file.set_position(static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin);

    const uint32_t winSize  = 2u * length;
    const uint32_t charSize = static_cast<uint32_t>(sizeof(CharType));

    auto buffer = std::make_unique<CharType[]>(winSize);
    CharType* const fileData = buffer.get();

    // comparator, do not recreate lambda per iteration
    auto matches = [&](const CharType* candidate) -> bool
    {
        return areg::compare_strings<CharType, CharType>(candidate, text, static_cast<areg::CharCount>(length), sensitive) == areg::Ordering::Equal;
    };

    uint32_t inBuf = 0u;  // valid chars currently in buffer
    while (true)
    {
        // Slide window: keep the back half for overlap between chunks
        if (inBuf >= length)
        {
            areg::mem_move(fileData, fileData + length, (inBuf - length) * charSize);
            inBuf -= length;
        }

        // Fill remainder of buffer from file
        const uint32_t toRead  = (winSize - inBuf) * charSize;
        const uint32_t didRead = file.read(reinterpret_cast<uint8_t*>(fileData + inBuf), toRead) / charSize;

        inBuf += didRead;

        if (inBuf < length)
            break;  // not enough data to match

        // Search all candidate positions in current window
        const uint32_t scanEnd = inBuf - length;
        for (uint32_t i = 0u; i <= scanEnd; ++i)
        {
            if (matches(fileData + i))
                return posSearch + i * charSize;
        }

        // Advance file position tracker by one chunk (length chars)
        posSearch += length * charSize;

        if (didRead == 0)
            break;  // EOF
    }

    return Cursor::INVALID_CURSOR_POSITION;
}

} // namespace

//////////////////////////////////////////////////////////////////////////
// FileBase class implementation.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
FileBase::FileBase()
    : IOStream        ( )
    , Cursor  ( )

    , mFileName         (String::empty_string())
    , mFileMode         (static_cast<uint32_t>(OpenMode::Invalid))
    , mReadConvert      (static_cast<InStream &>(self()), static_cast<Cursor &>(self()) )
    , mWriteConvert     (static_cast<OutStream &>(self()), static_cast<Cursor &>(self()) )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

uint32_t FileBase::normalize_mode(uint32_t mode) const
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
        mode &= ~(    static_cast<uint32_t>(OpenFlag::BitExist)
                    | static_cast<uint32_t>(OpenFlag::BitShareRead)
                    | static_cast<uint32_t>(OpenFlag::BitShareWrite)
                    | static_cast<uint32_t>(OpenFlag::BitOpenAlways)
                );
        mode |= static_cast<uint32_t>(OpenMode::CreateTemp);
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

    if ((mode & static_cast<uint32_t>(OpenFlag::BitOpenAlways)) != 0)
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
        uint8_t* temp = (buffer != nullptr) && (length > 0) ? DEBUG_NEW uint8_t[length] : nullptr;
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
    return _readString<char>(self(), buffer, charCount);
}

int32_t FileBase::read_string( wchar_t * buffer, int32_t charCount ) const
{
    return _readString<wchar_t>(self(), buffer, charCount);
}

int32_t FileBase::read_string(String & outValue ) const
{
    return _readString<char, String>(self(), outValue);
}

int32_t FileBase::read_string(WideString & outValue) const
{
    return _readString<char, WideString>(self(), outValue);
}

int32_t FileBase::read_line( char * buffer, int32_t charCount) const
{
    return _readLine<char>(self(), buffer, charCount);
}

int32_t FileBase::read_line( wchar_t * buffer, int32_t charCount ) const
{
    return _readLine<wchar_t>(self(), buffer, charCount);
}

int32_t FileBase::read_line( String & buffer) const
{
    return _readLine<char, String>(self(), buffer);
}

int32_t FileBase::read_line(WideString & buffer) const
{
    return _readLine<wchar_t, WideString>(self(), buffer);
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
    uint32_t curPos = position();
    uint32_t result = curPos;

    if (newSize > 0)
    {
        uint32_t newPos = reserve(newSize);
        if ((newPos != Cursor::INVALID_CURSOR_POSITION) && (newPos > curPos))
        {
            set_position(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
            for (uint32_t i = 0; i < newPos; ++ i)
            {
                write( &fillValue, sizeof( uint8_t ) );
            }

            ASSERT(position() == newPos);
            result = newPos;
        }
    }

    return result;
}

void FileBase::reset() const
{
    set_position(0, Cursor::SeekOrigin::Begin);
}

uint32_t FileBase::read(ByteBuffer & buffer) const
{
    uint32_t result = 0;
    buffer.invalidate();

    if ( is_opened() && can_read() )
    {
        int32_t sizeReserve = 0;
        uint32_t sizeRead   = 0;

        if (read_int(sizeReserve) && (sizeReserve > 0))
        {
            sizeRead = buffer.reserve(static_cast<uint32_t>(sizeReserve), false);
            uint8_t * data = sizeRead != 0 ? buffer.buffer() : nullptr;
            if ( (data != nullptr) && (read(data, sizeRead) == sizeRead) )
            {
                result = sizeRead + sizeof(int32_t);
            }
        }
    }

    return result;
}

uint32_t FileBase::read(String & ascii) const
{
    return static_cast<uint32_t>(read_string(ascii));
}

uint32_t FileBase::read(WideString & wide) const
{
    return static_cast<uint32_t>(read_string(wide));
}

uint32_t FileBase::write(const ByteBuffer & buffer)
{
    uint32_t result = 0;

    if ( is_opened() && can_write() )
    {
        const uint8_t * data  = buffer.buffer();
        uint32_t sizeUsed       = buffer.size_used();

        if (write_int(static_cast<int32_t>(sizeUsed)) && (write(data, sizeUsed) == sizeUsed))
        {
            result = sizeUsed + sizeof(int32_t);
        }
    }

    return result;
}

uint32_t FileBase::write(const String & ascii)
{
    const char * buffer = ascii.as_string();
    uint32_t space  = is_text_mode() != 0 ? static_cast<uint32_t>(ascii.length()) * sizeof(char) : ascii.space();

    return write(reinterpret_cast<const uint8_t *>(buffer), space);
}

uint32_t FileBase::write(const WideString & wide)
{
    const wchar_t * buffer  = wide.as_string();
    uint32_t space      = is_text_mode() != 0 ? static_cast<uint32_t>(wide.length()) * sizeof(wchar_t) : wide.space();

    return write(reinterpret_cast<const uint8_t *>(buffer), space);
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
    if (!can_read()
        || (startPos == Cursor::INVALID_CURSOR_POSITION)
        || (buffer == nullptr)
        || (length == 0))
    {
        return Cursor::INVALID_CURSOR_POSITION;
    }

    uint32_t posSearch = set_position(static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin);
    if (posSearch == Cursor::INVALID_CURSOR_POSITION)
        return Cursor::INVALID_CURSOR_POSITION;

    const uint32_t winSize = 2u * length;
    auto   storage = std::make_unique<uint8_t[]>(winSize);
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

        posSearch += length;  // advance window tracker

        if (didRead == 0)
            break;  // EOF
    }

    return Cursor::INVALID_CURSOR_POSITION;
}

uint32_t FileBase::search_data( uint32_t startPos, const ByteBuffer & buffer ) const
{
    return search_data(startPos, buffer.buffer(), buffer.size_used());
}

uint32_t FileBase::search_text( uint32_t startPos, const char * text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text, static_cast<uint32_t>(areg::string_length<char>( text )), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const wchar_t * text, bool caseSensitive ) const
{
    return _searchText<wchar_t>( *this, startPos, text, static_cast<uint32_t>(areg::string_length<wchar_t>( text )), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const String & text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text.as_string(), static_cast<uint32_t>(text.length()), caseSensitive );
}

uint32_t FileBase::search_text( uint32_t startPos, const WideString & text, bool caseSensitive ) const
{
    return _searchText<wchar_t>( *this, startPos, text.as_string( ), static_cast<uint32_t>(text.length( )), caseSensitive );
}

void FileBase::flush()
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

} // namespace areg
