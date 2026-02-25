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

//////////////////////////////////////////////////////////////////////////
// Local methods.
//////////////////////////////////////////////////////////////////////////
namespace {

template<typename CharType, class ClassType>
inline int32_t _readString(const FileBase & file, ClassType & outValue)
{
    uint32_t result = 0;
    outValue.clear();
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        return static_cast<int32_t>(result);
    } 

    constexpr uint32_t maxBufSize = 1024;
    CharType buffer[maxBufSize] { };
    uint32_t strLength  { maxBufSize - 1 };
    CharType * context      { nullptr };
    uint32_t length     { 0 };
    do 
    {
        buffer[0]               = areg::EndOfString;
        uint32_t oldPos     = file.getPosition();
        uint32_t readLength = file.read(reinterpret_cast<uint8_t *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
        readLength              = std::min(strLength, readLength);
        buffer[readLength]      = areg::EndOfString;

        length = readLength;
        if ( readLength != 0 )
        {
            const CharType * str = areg::getPrintable<CharType>( buffer, static_cast<areg::CharCount>(readLength), &context );
            length = context != nullptr ? static_cast<uint32_t>( context - buffer ) : readLength;

            outValue    += str;
            result      += length;
            int32_t newPos   = static_cast<int32_t>(result * sizeof(CharType)) + static_cast<int32_t>(oldPos);
            file.setPosition(newPos, Cursor::SeekOrigin::Begin);
            if ( context != (buffer + readLength) )
            {
                length = 0; // break loop
            }
        }

    } while ( length != 0 );

    return static_cast<int32_t>(result);
}

template<typename CharType, class ClassType>
inline int32_t _readLine(const FileBase & file, ClassType & outValue)
{
    uint32_t result = 0;
    outValue.clear();
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        return static_cast<int32_t>(result);
    } 

    constexpr uint32_t maxBufSize = 1024;
    CharType buffer[maxBufSize] { };
    uint32_t strLength  { maxBufSize - 1 };
    CharType * context      { nullptr };
    uint32_t length     { 0 };
    do 
    {
        buffer[0]               = areg::EndOfString;
        uint32_t oldPos     = file.getPosition();
        uint32_t readLength = file.read(reinterpret_cast<uint8_t *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
        readLength              = std::min(strLength, readLength);
        buffer[readLength]      = areg::EndOfString;

        length = readLength;
        if ( readLength != 0 )
        {
            const CharType * str = areg::getLine<CharType>( buffer, static_cast<areg::CharCount>(readLength), &context );
            length   = context != nullptr ? static_cast<uint32_t>(context - buffer) : readLength;
            outValue+= str;
            result  += length;
            int32_t newPos  = static_cast<int32_t>( (result * sizeof(CharType)) + oldPos );
            file.setPosition(newPos, Cursor::SeekOrigin::Begin);
            if ( context != (buffer + readLength) )
            {
                length = 0; // break loop
            }
        }
    } while ( length > 0 );

    return static_cast<int32_t>(result);
}

template<typename CharType>
inline int32_t _readString(const FileBase & file, CharType * buffer, int32_t charCount)
{
    uint32_t result = 0;
    if (file.isOpened() && file.canRead())
    {
        if ((buffer != nullptr) && (charCount > 1))
        {
            uint32_t strLength  = static_cast<uint32_t>(charCount) - 1;
            buffer[0]               = areg::EndOfString;
            uint32_t oldPos     = file.getPosition();
            CharType * context      = nullptr;
            uint32_t readLength = file.read(reinterpret_cast<uint8_t *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
            readLength              = std::min(strLength, readLength);
            buffer[readLength]      = areg::EndOfString;

            if ( readLength != 0 )
            {
                areg::getPrintable<CharType>( buffer, charCount, &context );
                ASSERT((context == nullptr) || (context >= buffer));
                result = context != nullptr ? static_cast<uint32_t>( context - buffer ) : readLength;
                int32_t newPos = static_cast<int32_t>( (result * sizeof(CharType)) + oldPos );
                file.setPosition(newPos, Cursor::SeekOrigin::Begin);
            }
        }
    }

    return static_cast<int32_t>(result);
}

template<typename CharType>
inline int32_t _readLine(const FileBase & file, CharType * buffer, int32_t charCount)
{
    uint32_t result = 0;
    if (file.isOpened() && file.canRead())
    {
        if ((buffer != nullptr) && (charCount > 1))
        {
            uint32_t strLength  = static_cast<uint32_t>(charCount) - 1;
            buffer[0]               = areg::EndOfString;
            uint32_t oldPos     = file.getPosition();
            CharType * context      = nullptr;
            uint32_t readLength = file.read(reinterpret_cast<uint8_t *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
            readLength              = std::min(strLength, readLength);
            buffer[readLength]      = areg::EndOfString;
            if ( readLength != 0 )
            {
                areg::getLine<CharType>(buffer, charCount, &context);
                ASSERT((context == nullptr) || (context >= buffer));
                result = context != nullptr ? static_cast<uint32_t>(context - buffer) : readLength;
                int32_t newPos = static_cast<int32_t>( (result * sizeof(CharType)) + oldPos );
                file.setPosition(newPos, Cursor::SeekOrigin::Begin);
            }
        }
    }

    return static_cast<int32_t>(result);
}

template<typename CharType>
inline bool _writeString(FileBase & file, const CharType * buffer, int32_t strLen = static_cast<int32_t>(areg::COUNT_ALL) )
{
    bool result = false;
    if (file.isOpened() && file.canWrite())
    {
        if (buffer != nullptr)
        {
            uint32_t len = strLen >= 0 ? static_cast<uint32_t>(strLen) : static_cast<uint32_t>(areg::getStringLength<CharType>( buffer ));
            len += file.isBinaryMode() ? 1 : 0;
            len *= sizeof(CharType);

            result = (file.write( reinterpret_cast<const uint8_t *>(buffer), len ) == len);
        }
    }

    return result;
}

template<typename CharType>
inline  bool _writeLine(FileBase & file, const CharType * buffer)
{
    bool result = false;
    if (file.isOpened() && file.canWrite())
    {
        if (buffer != nullptr)
        {
            uint32_t len = static_cast<uint32_t>(areg::getStringLineLength<CharType>(buffer));
            len *= sizeof(CharType);
            if ( file.write(reinterpret_cast<const uint8_t *>(buffer), len) == len )
                result = file.writeChar( areg::StringBase<CharType>::NewLine );
        }
    }

    return result;
}

template<typename DataType>
areg::Ordering _compareData( const DataType * memBuffer1, const DataType * memBuffer2, std::function<areg::Ordering( const DataType *, const DataType * )> func )
{
    return func( memBuffer1, memBuffer2 );
}

template<typename CharType>
uint32_t _searchText( const FileBase & file, uint32_t startPos, const CharType * text, uint32_t length, bool sensitive )
{
    uint32_t result{ Cursor::INVALID_CURSOR_POSITION };
    if ( file.canRead( ) && (startPos != Cursor::INVALID_CURSOR_POSITION) )
    {
        uint32_t posSearch = file.setPosition( static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin );
        if ( (areg::isEmpty<CharType>(text) == false) && (length != 0) )
        {
            uint32_t dataLen = length * 2;
            uint32_t bufLen  = length + 1;
            uint32_t readLen = 0;
            CharType * fileData = new CharType[ bufLen ];

            while ( result == Cursor::INVALID_CURSOR_POSITION )
            {
                if ( readLen != 0 )
                {
                    areg::memMove( fileData, fileData + length, readLen - length );
                    readLen -= length;
                }

                uint32_t inBuf = readLen;
                readLen = file.read( reinterpret_cast<uint8_t *>(fileData), (dataLen - readLen) * sizeof( CharType) ) / sizeof(CharType);
                if ( (readLen == 0) || (readLen < static_cast<uint32_t>(length)) )
                    break;

                readLen += inBuf;
                for ( uint32_t i = 0; (readLen - i) >= static_cast<uint32_t>(length); ++i )
                {
                    areg::Ordering comp = _compareData<CharType>( (fileData + i)
                                                                , text
                                                                , [length, sensitive]( const CharType * buf1, const CharType * buf2 ) -> areg::Ordering
                                                                {
                                                                    return areg::compareStrings<CharType, CharType>( buf1, buf2, static_cast<areg::CharCount>(length), sensitive );
                                                                }
                    );

                    if ( comp == areg::Ordering::Equal )
                    {
                        posSearch += i * sizeof(CharType);
                        result = posSearch;
                        break;
                    }
                }

                posSearch += static_cast<uint32_t>(length) * sizeof(CharType);
            }

            delete [] fileData;
        }
    }

    return result;
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

    , mFileName         (areg::String::getEmptyString())
    , mFileMode         (static_cast<uint32_t>(OpenMode::Invalid))
    , mReadConvert      (static_cast<InStream &>(self()), static_cast<Cursor &>(self()) )
    , mWriteConvert     (static_cast<OutStream &>(self()), static_cast<Cursor &>(self()) )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

uint32_t FileBase::normalizeMode(uint32_t mode) const
{
    if ((mode != static_cast<uint32_t>(OpenMode::Invalid)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Read);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitWrite)) != 0)
    {
        mode |= static_cast<uint32_t>(OpenMode::Write);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitAttach)) != 0)
    {
        mode &= ~(    static_cast<uint32_t>(OpenFlag::BitDetach)
                    | static_cast<uint32_t>(OpenFlag::BitTruncate)
                    | static_cast<uint32_t>(OpenFlag::BitTemp)
                    | static_cast<uint32_t>(OpenFlag::BitShareWrite)
                    | static_cast<uint32_t>(OpenFlag::BitWrite)
                );
        mode |= static_cast<uint32_t>(OpenMode::Attach);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitDetach)) != 0)
    {
        mode &= ~(    static_cast<uint32_t>(OpenFlag::BitAttach)
                    | static_cast<uint32_t>(OpenFlag::BitTemp)
                    | static_cast<uint32_t>(OpenFlag::BitShareWrite)
                );
        mode |= static_cast<uint32_t>(OpenMode::Detach);
    }

    if ((mode & static_cast<uint32_t>(OpenFlag::BitTemp)) != 0)
    {
        mode &= ~(    static_cast<uint32_t>(OpenFlag::BitDelete)
                    | static_cast<uint32_t>(OpenFlag::BitExist)
                    | static_cast<uint32_t>(OpenFlag::BitAttach)
                    | static_cast<uint32_t>(OpenFlag::BitDetach)
                    | static_cast<uint32_t>(OpenFlag::BitShareRead)
                    | static_cast<uint32_t>(OpenFlag::BitShareWrite)
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

    if (((mode & static_cast<uint32_t>(OpenFlag::BitWrite)) == 0) && ((mode & static_cast<uint32_t>(OpenFlag::BitRead)) != 0))
    {
        mode |= static_cast<uint32_t>(OpenFlag::BitExist);
    }
    else if ((mode & static_cast<uint32_t>(OpenFlag::BitCreate)) != 0)
    {
        mode &= ~static_cast<uint32_t>(OpenMode::Exist);
        mode |= static_cast<uint32_t>(OpenMode::Create);
        mode |= static_cast<uint32_t>(OpenMode::Write);
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
    if ((length != 0) && (buffer != nullptr) && isOpened() && canWrite())
    {
        moveToEnd();
        result = write(buffer, length);
    }

    return static_cast<int32_t>(result);
}

int32_t FileBase::readInvert( uint8_t * buffer, uint32_t length ) const
{
    uint32_t result = 0;
    if ((length != 0) && (buffer != nullptr) && isOpened() && canRead())
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

int32_t FileBase::writeInvert( const uint8_t * buffer, uint32_t length )
{
    uint32_t result = 0;
    if ((length != 0) && (buffer != nullptr) && isOpened() && canWrite())
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

int32_t FileBase::readString( char * buffer, int32_t charCount) const
{
    return _readString<char>(self(), buffer, charCount);
}

int32_t FileBase::readString( wchar_t * buffer, int32_t charCount ) const
{
    return _readString<wchar_t>(self(), buffer, charCount);
}

int32_t FileBase::readString(areg::String & outValue ) const
{
    return _readString<char, areg::String>(self(), outValue);
}

int32_t FileBase::readString(areg::WideString & outValue) const
{
    return _readString<char, areg::WideString>(self(), outValue);
}

int32_t FileBase::readLine( char * buffer, int32_t charCount) const
{
    return _readLine<char>(self(), buffer, charCount);
}

int32_t FileBase::readLine( wchar_t * buffer, int32_t charCount ) const
{
    return _readLine<wchar_t>(self(), buffer, charCount);
}

int32_t FileBase::readLine( areg::String & buffer) const
{
    return _readLine<char, areg::String>(self(), buffer);
}

int32_t FileBase::readLine(areg::WideString & buffer) const
{
    return _readLine<wchar_t, areg::WideString>(self(), buffer);
}

bool FileBase::writeString( const char* buffer )
{
    return _writeString<char>(self(), buffer, -1);
}

bool FileBase::writeString( const wchar_t* buffer)
{
    return _writeString<wchar_t>(self(), buffer, -1);
}

bool FileBase::writeString(const areg::String& buffer)
{
    return _writeString<char>(self(), buffer.getString(), static_cast<int32_t>(buffer.getLength()));
}

bool FileBase::writeString(const areg::WideString& buffer)
{
    return _writeString<wchar_t>(self(), buffer.getString(), static_cast<int32_t>(buffer.getLength()));
}

bool FileBase::writeLine( const char* buffer)
{
    return _writeLine<char>(self(), buffer);
}

bool FileBase::writeLine( const wchar_t* buffer)
{
    return _writeLine<wchar_t>(self(), buffer);
}

bool FileBase::writeLine(const areg::String& buffer)
{
    return _writeLine<char>(self(), buffer);
}

bool FileBase::writeLine(const areg::WideString& buffer)
{
    return _writeLine<wchar_t>(self(), buffer);
}

uint32_t FileBase::resizeAndFill(uint32_t newSize, uint8_t fillValue )
{
    uint32_t curPos = getPosition();
    uint32_t result = curPos;

    if (newSize > 0)
    {
        uint32_t newPos = reserve(newSize);
        if ((newPos != Cursor::INVALID_CURSOR_POSITION) && (newPos > curPos))
        {
            setPosition(static_cast<int32_t>(curPos), Cursor::SeekOrigin::Begin);
            for (uint32_t i = 0; i < newPos; ++ i)
            {
                write( &fillValue, sizeof( uint8_t ) );
            }

            ASSERT(getPosition() == newPos);
            result = newPos;
        }
    }

    return result;
}

void FileBase::resetCursor() const
{
    setPosition(0, Cursor::SeekOrigin::Begin);
}

uint32_t FileBase::read(ByteBuffer & buffer) const
{
    uint32_t result = 0;
    buffer.invalidate();

    if ( isOpened() && canRead() )
    {
        int32_t sizeReserve = 0;
        uint32_t sizeRead   = 0;

        if (readInt(sizeReserve) && (sizeReserve > 0))
        {
            sizeRead = buffer.reserve(static_cast<uint32_t>(sizeReserve), false);
            uint8_t * data = sizeRead != 0 ? buffer.getBuffer() : nullptr;
            if ( (data != nullptr) && (read(data, sizeRead) == sizeRead) )
            {
                result = sizeRead + sizeof(int32_t);
            }
        }
    }

    return result;
}

uint32_t FileBase::read(areg::String & ascii) const
{
    return static_cast<uint32_t>(readString(ascii));
}

uint32_t FileBase::read(areg::WideString & wide) const
{
    return static_cast<uint32_t>(readString(wide));
}

uint32_t FileBase::write(const ByteBuffer & buffer)
{
    uint32_t result = 0;

    if ( isOpened() && canWrite() )
    {
        const uint8_t * data  = buffer.getBuffer();
        uint32_t sizeUsed       = buffer.getSizeUsed();

        if (writeInt(static_cast<int32_t>(sizeUsed)) && (write(data, sizeUsed) == sizeUsed))
        {
            result = sizeUsed + sizeof(int32_t);
        }
    }

    return result;
}

uint32_t FileBase::write(const areg::String & ascii)
{
    const char * buffer = ascii.getString();
    uint32_t space  = isTextMode() != 0 ? static_cast<uint32_t>(ascii.getLength()) * sizeof(char) : ascii.getSpace();

    return write(reinterpret_cast<const uint8_t *>(buffer), space);
}

uint32_t FileBase::write(const areg::WideString & wide)
{
    const wchar_t * buffer  = wide.getString();
    uint32_t space      = isTextMode() != 0 ? static_cast<uint32_t>(wide.getLength()) * sizeof(wchar_t) : wide.getSpace();

    return write(reinterpret_cast<const uint8_t *>(buffer), space);
}

bool FileBase::write(const char * ascii)
{
    return writeString(ascii);
}

bool FileBase::write(const wchar_t * wide)
{
    return writeString(wide);
}

uint32_t FileBase::searchData( uint32_t startPos, const uint8_t * buffer, uint32_t length ) const
{
    uint32_t result{ Cursor::INVALID_CURSOR_POSITION };
    if ( canRead( ) && (startPos != Cursor::INVALID_CURSOR_POSITION))
    {
        uint32_t posSearch = setPosition( static_cast<int32_t>(startPos), Cursor::SeekOrigin::Begin );
        if ( (buffer != nullptr) && (length != 0) )
        {
            uint32_t dataLen = length * 2;
            uint32_t readLen = 0;
            uint8_t * fileData = new uint8_t[ dataLen ];

            while ((result == Cursor::INVALID_CURSOR_POSITION) && (posSearch != Cursor::INVALID_CURSOR_POSITION))
            {
                if ( readLen != 0 )
                {
                    areg::memMove( fileData, fileData + length, readLen - length );
                    readLen = length;
                }

                readLen = read( fileData, dataLen - readLen ) + readLen;
                if ( (readLen == 0) || (readLen < static_cast<uint32_t>(length)) )
                    break;

                for ( uint32_t i = 0; (readLen - i) >= length; ++i )
                {
                    areg::Ordering comp = _compareData<uint8_t>( (fileData + i)
                                                                       , buffer
                                                                       , [length]( const uint8_t * buf1, const uint8_t * buf2 ) -> areg::Ordering
                                                                         {
                                                                             return areg::memCompare( buf1, buf2, length );
                                                                         }
                                                                        );

                    if ( comp == areg::Ordering::Equal )
                    {
                        posSearch += i;
                        result = posSearch;
                        break;
                    }
                }

                posSearch += length;
            }

            delete[ ] fileData;
        }
    }

    return result;
}

uint32_t FileBase::searchData( uint32_t startPos, const ByteBuffer & buffer ) const
{
    return searchData(startPos, buffer.getBuffer(), buffer.getSizeUsed());
}

uint32_t FileBase::searchText( uint32_t startPos, const char * text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text, static_cast<uint32_t>(areg::getStringLength<char>( text )), caseSensitive );
}

uint32_t FileBase::searchText( uint32_t startPos, const wchar_t * text, bool caseSensitive ) const
{
    return _searchText<wchar_t>( *this, startPos, text, static_cast<uint32_t>(areg::getStringLength<wchar_t>( text )), caseSensitive );
}

uint32_t FileBase::searchText( uint32_t startPos, const areg::String & text, bool caseSensitive ) const
{
    return _searchText<char>( *this, startPos, text.getString(), static_cast<uint32_t>(text.getLength()), caseSensitive );
}

uint32_t FileBase::searchText( uint32_t startPos, const areg::WideString & text, bool caseSensitive ) const
{
    return _searchText<wchar_t>( *this, startPos, text.getString( ), static_cast<uint32_t>(text.getLength( )), caseSensitive );
}

void FileBase::flush()
{
}

void FileBase::normalizeName(areg::String & name)
{
    // replace all "%time%"
    char fmt[128] { 0 };
    areg::CalendarTime st;
    DateTime::getNow(st, true);
    areg::String::formatString(fmt, 128, FileBase::TIMESTAMP_FORMAT.data(), st.stYear, st.stMonth, st.stDay, st.stHour, st.stMinute, st.stSecond, st.stMillisecs);
    name.replace(FileBase::FILE_MASK_TIMESTAMP, fmt, areg::START_POS, true);

    // replace all "%appname%"
    const areg::String & appName = areg::Process::getInstance().getAppName();
    name.replace(FileBase::FILE_MASK_APPNAME, appName, areg::START_POS, true);
}
