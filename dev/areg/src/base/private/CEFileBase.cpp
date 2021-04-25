/************************************************************************
 * \file        areg/src/base/private/CEFileBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief      
 *
 ************************************************************************/

#include "areg/src/base/CEFileBase.hpp"
#include "areg/src/base/NEUtilities.hpp"
#include "areg/src/base/CEString.hpp"
#include "areg/src/base/CEWideString.hpp"

//////////////////////////////////////////////////////////////////////////
// Local defines.
//////////////////////////////////////////////////////////////////////////

#define EOS_A           static_cast<char>('\0')
#define EOS_W           static_cast<wchar_t>('\0')
#define EOL_A           static_cast<char>('\n')
#define EOL_W           static_cast<wchar_t>('\n')
#define CR_A            static_cast<char>('\r')
#define CR_W            static_cast<wchar_t>('\r')

#define IS_EOL_A(ch)    ((ch) == EOS_A || (ch) == EOL_A || (ch) == CR_A)
#define IS_EOL_W(ch)    ((ch) == EOS_W || (ch) == EOL_W || (ch) == CR_W)

//////////////////////////////////////////////////////////////////////////
// CEFileBase class implementation.
//////////////////////////////////////////////////////////////////////////

const int          CEFileBase::AUTO_ALLOC               = static_cast<int>(-1);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEFileBase::CEFileBase( void )
    : IEIOStream       ( )
    , IECursorPosition ( )

    , mReadConvert      (static_cast<IEInStream &>(self()), static_cast<IECursorPosition &>(self()) )
    , mWriteConvert     (static_cast<IEOutStream &>(self()), static_cast<IECursorPosition &>(self()) )
{
    ; // do nothing
}

CEFileBase::~CEFileBase( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////

int CEFileBase::NormalizeMode(int mode) const
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
        mode &= ~(FOB_FORCE_DELETE | FOB_EXIST | FOB_ATTACH | FOB_DETACH | FOB_SHARE_READ | FOB_SHARE_WRITE);
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

int CEFileBase::Append( const unsigned char* buffer, int length )
{
    int result = 0;
    if (buffer != NULL && IsOpened() && CanWrite())
    {
        MoveToEnd();
        result = Write(buffer, length);
    }
    return result;
}

int CEFileBase::ReadInvert( unsigned char* buffer, int length ) const
{
    int result = 0;
    if (IsOpened() && CanRead())
    {
        result = Read(buffer, length);
        for (int i = 0; i < result / 2; ++ i)
        {
            unsigned char ch = buffer[i];
            buffer[i] = buffer[length - 1 - i];
            buffer[length - 1 - i] = ch;
        }
    }
    return result;
}

int CEFileBase::WriteInvert( const unsigned char* buffer, int length )
{
    int result = 0;
    if (IsOpened() && CanWrite())
    {
        unsigned char* temp = buffer != NULL && length > 0 ? DEBUG_NEW unsigned char[length] : NULL;
        if (temp != NULL)
        {
            for (int i = 0; i < length; ++ i)
                temp[i] = buffer[length - 1 - i];
            result = Write(temp, length);
            delete [] temp;
        }
    }
    return result;
}

int CEFileBase::ReadString( char * buffer, int elemCount) const
{
    int result = 0;
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if (buffer != NULL && elemCount > 0)
    {
        int strLength       = elemCount - 1;
        buffer[0]           = '\0';
        buffer[strLength]   = '\0';
        int curPos          = GetPosition();
        char * context      = NULL;
        int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(char)) / sizeof(char);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = '\0';

        if ( readLength > 0 )
        {
            NEString::getPrintable<char>( buffer, elemCount, &context );
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? static_cast<int>( context - buffer ) : readLength;
            SetPosition(curPos + result * sizeof(char), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }
    return result;
}

int CEFileBase::ReadString( wchar_t * buffer, int elemCount ) const
{
    int result = 0;
    if ((IsOpened() == false) || (CanRead() == false))
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if ((buffer != NULL) && (elemCount > 0))
    {
        int strLength       = elemCount - 1;
        buffer[0]           = '\0';
        buffer[strLength]   = '\0';
        int curPos          = GetPosition();
        wchar_t * context   = NULL;
        int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(wchar_t)) / sizeof(wchar_t);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = '\0';
        if ( readLength > 0 )
        {
            NEString::getPrintable<wchar_t>( buffer, elemCount, &context );
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? static_cast<int>( context - buffer) : readLength;
            SetPosition(curPos + result * sizeof(wchar_t), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }
    return result;
}

int CEFileBase::ReadString(CEString & outValue ) const
{
    static const unsigned int maxBufSize = 1024;

    int result = 0;
    outValue.Clear();
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    } 
    
    char * buffer = DEBUG_NEW char[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        char * context      = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = '\0';
            buffer[strLength]   = '\0';
            int curPos          = GetPosition();
            int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(char)) / sizeof(char);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = '\0';

            length              = readLength;
            if ( readLength > 0 )
            {
                const char * str = NEString::getPrintable<char>( buffer, readLength, &context );
                length = context != NULL ? static_cast<int>( context - buffer) : readLength;
                outValue+= str;
                result  += length;
                SetPosition(curPos + result * sizeof(char), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }
    return result;
}

int CEFileBase::ReadString(CEWideString & outValue) const
{
    static const unsigned int maxBufSize = 1024;

    outValue.Clear();
    int result = 0;
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    }


    wchar_t * buffer = DEBUG_NEW wchar_t[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        wchar_t * context   = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = L'\0';
            buffer[strLength]   = L'\0';
            int curPos          = GetPosition();
            int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(wchar_t)) / sizeof(wchar_t);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = L'\0';

            length  = readLength;
            if ( readLength > 0 )
            {
                const wchar_t * str = NEString::getPrintable<wchar_t>( buffer, readLength, &context );
                length = context != NULL ? static_cast<int>( context - buffer) : readLength;
                outValue+= str;
                result  += length;
                SetPosition(curPos + result * sizeof(wchar_t), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }

    return result;
}

int CEFileBase::ReadLine( char * buffer, int elemCount) const
{
    int result = 0;
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if (buffer != NULL && elemCount > 0)
    {
        int strLength       = elemCount - 1;
        buffer[0]           = '\0';
        buffer[strLength]   = '\0';
        int curPos          = GetPosition();
        char * context      = NULL;
        int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(char)) / sizeof(char);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = '\0';
        if ( readLength > 0 )
        {
            NEString::getLine<char>(buffer, elemCount, &context);
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? static_cast<int>( context - buffer) : readLength;
            SetPosition(curPos + result * sizeof(char), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }
    return result;
}

int CEFileBase::ReadLine( wchar_t * buffer, int elemCount ) const
{
    int result = 0;
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
    } 
    else if (buffer != NULL && elemCount > 0)
    {
        int strLength       = elemCount - 1;
        buffer[0]           = '\0';
        buffer[strLength]   = '\0';
        int curPos          = GetPosition();
        wchar_t * context   = NULL;
        int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(wchar_t)) / sizeof(wchar_t);
        readLength          = MACRO_MIN(strLength, readLength);
        buffer[readLength]  = '\0';
        if ( readLength > 0 )
        {
            NEString::getLine<wchar_t>( buffer, elemCount, &context );
            ASSERT(context == NULL || context > buffer);
            result = context != NULL ? static_cast<int>( context - buffer) : readLength;
            SetPosition(curPos + result * sizeof(wchar_t), IECursorPosition::POSITION_BEGIN);
        }
    }
    else
    {
        OUTPUT_ERR("The required buffer to fill is empty, cannot read string");
    }
    return result;
}

int CEFileBase::ReadLine( CEString & outValue) const
{
    static const unsigned int maxBufSize = 1024;

    int result = 0;
    outValue.Clear();
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    } 


    char * buffer = DEBUG_NEW char[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        char * context      = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = '\0';
            buffer[strLength]   = '\0';
            int curPos          = GetPosition();
            int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(char)) / sizeof(char);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = '\0';

            length              = readLength;
            if ( readLength > 0 )
            {
                const char * str = NEString::getLine<char>( buffer, readLength, &context );
                length   = context != NULL ? MACRO_64_LO_BYTE32(static_cast<size_t>( context - buffer)) : readLength;
                outValue+= str;
                result  += length;
                SetPosition(curPos + result * sizeof(char), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }
    return result;
}

int CEFileBase::ReadLine(CEWideString & outValue) const
{
    static const unsigned int maxBufSize = 1024;

    outValue.Clear();
    int result = 0;
    if (IsOpened() == false || CanRead() == false)
    {
        OUTPUT_ERR("Either file is not opened or forbidden to read data. Data cannot be read.");
        return result;
    } 
    
    wchar_t * buffer = DEBUG_NEW wchar_t[maxBufSize];
    if (buffer != NULL)
    {
        int strLength       = maxBufSize - 1;
        wchar_t * context   = NULL;
        int length          = 0;
        do 
        {
            buffer[0]           = L'\0';
            buffer[strLength]   = L'\0';
            int curPos          = GetPosition();
            int readLength      = Read(reinterpret_cast<unsigned char *>(buffer), strLength * sizeof(wchar_t)) / sizeof(wchar_t);
            readLength          = MACRO_MIN(strLength, readLength);
            buffer[readLength]  = L'\0';

            length  = readLength;
            if ( readLength > 0 )
            {
                const wchar_t * str = NEString::getLine<wchar_t>( buffer, readLength, &context );
                length   = context != NULL ? MACRO_64_LO_BYTE32(static_cast<size_t>( context - buffer)) : readLength;
                outValue+= str;
                result  += length;
                SetPosition(curPos + result * sizeof(wchar_t), IECursorPosition::POSITION_BEGIN);
                if ( context != buffer + readLength)
                    length = 0; // break loop
            }
        } while ( length > 0 );

        delete [] buffer;
    }
    return result;
}

bool CEFileBase::WriteString( const char* inValue )
{
    static const unsigned int size = sizeof(char);

    bool result = false;
    if (IsOpened() && CanWrite())
    {
        if (inValue != NULL)
        {
            unsigned int len = 0;
            while (inValue[len] != EOS_A)
                ++ len;
            len += IsBinaryMode() ? 1 : 0;
            unsigned int byteLen = len * size;

            result = Write( reinterpret_cast<const unsigned char *>(inValue), byteLen ) == byteLen;
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

bool CEFileBase::WriteString( const wchar_t* inValue )
{
    static const unsigned int size = sizeof(wchar_t);

    bool result = false;
    if (IsOpened() && CanWrite())
    {
        if (inValue != NULL)
        {
            unsigned int len = 0;
            while (inValue[len] != EOS_W)
                ++ len;
            len += IsBinaryMode() ? 1 : 0;
            unsigned int byteLen = len * size;

            result = Write( reinterpret_cast<const unsigned char *>(inValue), byteLen ) == byteLen;
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

bool CEFileBase::WriteLine( const char* inValue )
{
    static const unsigned int size = sizeof(char);

    bool result = false;
    if (IsOpened() && CanWrite())
    {
        if (inValue != NULL)
        {
            unsigned int len = 0;
            while (NEString::isEndOfLine<char>(inValue[len]) == false)
                ++ len;
            unsigned int byteLen = len * size;
            if ( Write(reinterpret_cast<const unsigned char *>(inValue), byteLen) == byteLen )
                result = WriteChar(EOL_A);
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

bool CEFileBase::WriteLine( const wchar_t* inValue )
{
    static const unsigned int size = sizeof(wchar_t);

    bool result = false;
    if (IsOpened() && CanWrite())
    {
        if (inValue != NULL)
        {
            unsigned int len = 0;
            while ( NEString::isEndOfLine<wchar_t>( inValue[len] ) == false )
                ++ len;

            unsigned int byteLen = len * size;
            if (Write((const unsigned char *)inValue, byteLen) == byteLen)
                result = WriteWideChar(EOL_W);
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

unsigned int CEFileBase::ReserveAndFill( int newSize, unsigned char fillValue )
{
    unsigned int curPos = GetPosition();
    unsigned int newPos = Reserve(newSize);
    if (newPos != IECursorPosition::INVALID_CURSOR_POSITION)
    {
        if (newPos > curPos)
        {
            SetPosition(curPos, IECursorPosition::POSITION_BEGIN);
            for (unsigned int i = 0; i < newPos; ++ i)
                Write(&fillValue, sizeof(unsigned char));
            ASSERT(GetPosition() == newPos);
        }
    }
    return newPos;
}

void CEFileBase::ResetCursor(void) const
{
    SetPosition(0, IECursorPosition::POSITION_BEGIN);
}

void CEFileBase::Flush(void)
{
    ; // do nothing
}
