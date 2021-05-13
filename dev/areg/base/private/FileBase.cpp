/************************************************************************
 * \file        areg/base/private/FileBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief      
 *
 ************************************************************************/

#include "areg/base/FileBase.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/WideString.hpp"

//////////////////////////////////////////////////////////////////////////
// Local methods.
//////////////////////////////////////////////////////////////////////////
template<typename CharType, class ClassType>
inline int _readString(const FileBase & file, ClassType & outValue)
{
    static const unsigned int maxBufSize = 1024;

    int result = 0;
    outValue.clear();
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    } 

    CharType * buffer = DEBUG_NEW CharType[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        CharType * context  = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = NEString::EndOfString;
            int curPos          = file.getPosition();
            int readLength      = file.read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = NEString::EndOfString;

            length              = readLength;
            if ( readLength > 0 )
            {
                const CharType * str = NEString::getPrintable<CharType>( buffer, readLength, &context );
                length = context != NULL ? MACRO_ELEM_COUNT( buffer, context ) : readLength;

                outValue+= str;
                result  += length;
                file.setPosition(curPos + result * sizeof(CharType), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }

    return result;
}

template<typename CharType, class ClassType>
inline int _readLine(const FileBase & file, ClassType & outValue)
{
    static const unsigned int maxBufSize = 1024;

    int result = 0;
    outValue.clear();
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    } 


    CharType * buffer = DEBUG_NEW CharType[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        CharType * context  = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = NEString::EndOfString;
            int curPos          = file.getPosition();
            int readLength      = file.read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = NEString::EndOfString;

            length              = readLength;
            if ( readLength > 0 )
            {
                const CharType * str = NEString::getLine<CharType>( buffer, readLength, &context );
                length   = context != NULL ? MACRO_ELEM_COUNT(buffer, context) : readLength;
                outValue+= str;
                result  += length;
                file.setPosition(curPos + result * sizeof(CharType), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }

    return result;
}

template<typename CharType>
inline int _readString(const FileBase & file, CharType * buffer, int elemCount)
{
    int result = 0;
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if (buffer != NULL && elemCount > 0)
    {
        int strLength       = elemCount - 1;
        buffer[0]           = NEString::EndOfString;
        int curPos          = file.getPosition();
        CharType * context  = NULL;
        int readLength      = file.read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = NEString::EndOfString;

        if ( readLength > 0 )
        {
            NEString::getPrintable<CharType>( buffer, elemCount, &context );
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? MACRO_ELEM_COUNT( buffer, context ) : readLength;
            file.setPosition(curPos + result * sizeof(CharType), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }

    return result;
}

template<typename CharType>
inline int _readLine(const FileBase & file, CharType * buffer, int elemCount)
{
    int result = 0;
    if ((file.isOpened() == false) || (file.canRead() == false))
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if (buffer != NULL && elemCount > 0)
    {
        int strLength       = elemCount - 1;
        buffer[0]           = NEString::EndOfString;
        int curPos          = file.getPosition();
        CharType * context  = NULL;
        int readLength      = file.read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(CharType)) / sizeof(CharType);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = NEString::EndOfString;
        if ( readLength > 0 )
        {
            NEString::getLine<CharType>(buffer, elemCount, &context);
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? MACRO_ELEM_COUNT(buffer, context) : readLength;
            file.setPosition(curPos + result * sizeof(CharType), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }
    return result;
}

template<typename CharType>
inline bool _writeString(FileBase & file, const CharType * buffer)
{
    bool result = false;
    if (file.isOpened() && file.canWrite())
    {
        if (buffer != NULL)
        {
            unsigned int len = NEString::getStringLength<CharType>(buffer);
            len += file.isBinaryMode() ? 1 : 0;
            len *= sizeof(CharType);

            result = file.write( reinterpret_cast<const unsigned char *>(buffer), len ) == len;
        }
        else
        {
            OUTPUT_ERR("The buffer is NULL, nothing to write.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file is not opened or data cannot be written.");
    }

    return result;
}

template<typename CharType>
inline  bool _writeLine(FileBase & file, const CharType * buffer)
{
    bool result = false;
    if (file.isOpened() && file.canWrite())
    {
        if (buffer != NULL)
        {
            unsigned int len = 0;
            while (NEString::isEndOfLine<CharType>(buffer[len]) == false)
                ++ len;

            len *= sizeof(CharType);
            if ( file.write(reinterpret_cast<const unsigned char *>(buffer), len) == len )
                result = file.writeChar( TEString<CharType>::NewLine );
        }
        else
        {
            OUTPUT_ERR("The buffer is NULL, nothing to write.");
        }
    }
    else
    {
        OUTPUT_ERR("Either file is not opened or data cannot be written.");
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////
// Local defines.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// FileBase class implementation.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
FileBase::FileBase( void )
    : IEIOStream        ( )
    , IECursorPosition  ( )

    , mFileName         (String::EmptyString)
    , mFileMode         (static_cast<unsigned int>(FO_MODE_INVALID))
    , mReadConvert      (static_cast<IEInStream &>(self()), static_cast<IECursorPosition &>(self()) )
    , mWriteConvert     (static_cast<IEOutStream &>(self()), static_cast<IECursorPosition &>(self()) )
{
    ; // do nothing
}

FileBase::~FileBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

unsigned int FileBase::normalizeMode(unsigned int mode) const
{
    if (mode != FO_MODE_INVALID)
        mode |= FO_MODE_READ;

    if (mode & FOB_ATTACH)
    {
        mode &= ~(FOB_DETACH | FOB_TRUNCATE | FOB_TEMP_FILE | FOB_SHARE_WRITE | FOB_WRITE);
        mode |= FO_MODE_ATTACH;
    }

    if (mode & FOB_DETACH)
    {
        mode &= ~(FOB_ATTACH | FOB_TEMP_FILE | FOB_SHARE_WRITE);
        mode |= FO_MODE_DETACH;
    }

    if (mode & FOB_TEMP_FILE)
    {
        mode &= ~(FOB_FOR_DELETE | FOB_EXIST | FOB_ATTACH | FOB_DETACH | FOB_SHARE_READ | FOB_SHARE_WRITE);
        mode |= FO_MODE_CREATE_TEMP;
    }

    if (mode & FOB_TEXT)
    {
        mode &= ~FOB_BINARY;
        mode |= FO_MODE_TEXT;
    }
    else
    {
        mode |= FO_MODE_BINARY;
    }

    if (mode & FOB_BINARY)
    {
        mode &= ~FOB_TEXT;
        mode |= FO_MODE_BINARY;
    }

    if ((mode & FOB_WRITE) == 0 && (mode & FOB_READ) != 0)
        mode |= FOB_EXIST;
    else if (mode & FOB_CREATE)
        mode |= FO_MODE_CREATE;

    if (mode & FOB_EXIST)
        mode |= FO_MODE_EXIST;
    else
        mode &= ~FOB_TRUNCATE;

    if (mode & FOB_TRUNCATE)
        mode |= FO_MODE_TRUNCATE;

    if (mode & FOB_WRITE_DIRECT)
        mode |= FO_MODE_WRITE_DIRECT;

    return mode;
}

int FileBase::append( const unsigned char* buffer, int length )
{
    int result = 0;
    if (buffer != NULL && isOpened() && canWrite())
    {
        moveToEnd();
        result = write(buffer, length);
    }
    return result;
}

int FileBase::readInvert( unsigned char* buffer, int length ) const
{
    int result = 0;
    if (isOpened() && canRead())
    {
        result = read(buffer, length);
        for (int i = 0; i < result / 2; ++ i)
        {
            unsigned char ch = buffer[i];
            buffer[i] = buffer[length - 1 - i];
            buffer[length - 1 - i] = ch;
        }
    }
    return result;
}

int FileBase::writeInvert( const unsigned char* buffer, int length )
{
    int result = 0;
    if (isOpened() && canWrite())
    {
        unsigned char* temp = buffer != NULL && length > 0 ? DEBUG_NEW unsigned char[length] : NULL;
        if (temp != NULL)
        {
            for (int i = 0; i < length; ++ i)
                temp[i] = buffer[length - 1 - i];
            result = write(temp, length);
            delete [] temp;
        }
    }
    return result;
}

int FileBase::readString( char * buffer, int elemCount) const
{
    return _readString<char>(self(), buffer, elemCount);
}

int FileBase::readString( wchar_t * buffer, int elemCount ) const
{
    return _readString<wchar_t>(self(), buffer, elemCount);
}

int FileBase::readString(String & outValue ) const
{
    return _readString<char, String>(self(), outValue);
}

int FileBase::readString(WideString & outValue) const
{
    return _readString<char, WideString>(self(), outValue);
}

int FileBase::readLine( char * buffer, int elemCount) const
{
    return _readLine<char>(self(), buffer, elemCount);
}

int FileBase::readLine( wchar_t * buffer, int elemCount ) const
{
    return _readLine<wchar_t>(self(), buffer, elemCount);
}

int FileBase::readLine( String & outValue) const
{
    return _readLine<char, String>(self(), outValue);
}

int FileBase::readLine(WideString & outValue) const
{
    return _readLine<wchar_t, WideString>(self(), outValue);
}

bool FileBase::writeString( const char* inValue )
{
    return _writeString<char>(self(), inValue);
}

bool FileBase::writeString( const wchar_t* inValue )
{
    return _writeString<wchar_t>(self(), inValue);
}

bool FileBase::writeLine( const char* inValue )
{
    return _writeLine<char>(self(), inValue);
}

bool FileBase::writeLine( const wchar_t* inValue )
{
    return _writeLine<wchar_t>(self(), inValue);
}

unsigned int FileBase::resizeAndFill( int newSize, unsigned char fillValue )
{
    unsigned int curPos = getPosition();
    unsigned int newPos = reserve(newSize);
    if (newPos != IECursorPosition::INVALID_CURSOR_POSITION)
    {
        if (newPos > curPos)
        {
            setPosition(curPos, IECursorPosition::POSITION_BEGIN);
            for (unsigned int i = 0; i < newPos; ++ i)
                write(&fillValue, sizeof(unsigned char));
            ASSERT(getPosition() == newPos);
        }
    }
    return newPos;
}

void FileBase::resetCursor(void) const
{
    setPosition(0, IECursorPosition::POSITION_BEGIN);
}

void FileBase::flush(void)
{
    ; // do nothing
}
