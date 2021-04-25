/************************************************************************
 * \file        areg/src/base/private/NEUtfString.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, namespace with UTF String helper methods
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/src/base/NEUtfString.hpp"
#include "areg/src/base/NEMemory.hpp"
#include "areg/src/base/GEMacros.h"

static const unsigned char  _BOM_UTF8[]     = { 0xEF, 0xBB, 0xBF        };
static const unsigned char  _BOM_UTF16_BE[] = { 0xFE, 0xFF              };
static const unsigned char  _BOM_UTF16_LE[] = { 0xFF, 0xFE              };
static const unsigned char  _BOM_UTF32_BE[] = { 0x00, 0x00, 0xFE, 0xFF  };
static const unsigned char  _BOM_UTF32_LE[] = { 0xFF, 0xFE, 0x00, 0x00  };

static const unsigned char* _BOM_Indicator[ static_cast<int>(NEUtfString::BOM_Undefined) + 1 ]   = 
{
      _BOM_UTF8
    , _BOM_UTF16_BE
    , _BOM_UTF16_LE
    , _BOM_UTF32_BE
    , _BOM_UTF32_LE
    , static_cast<unsigned char *>(NULL)
};

static const unsigned int   _BOM_Lengths[]  = 
{
      MACRO_ARRAYLEN( _BOM_UTF8        )
    , MACRO_ARRAYLEN( _BOM_UTF16_BE    )
    , MACRO_ARRAYLEN( _BOM_UTF16_LE    )
    , MACRO_ARRAYLEN( _BOM_UTF32_BE    )
    , MACRO_ARRAYLEN( _BOM_UTF32_LE    )
};

static inline NEUtfString::CharPos _getSequence1( const NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf8 & out_char )
{
    ASSERT(buffer != NULL);
    // one octet
    out_char = NEUtfString::mask8<NEUtfString::utf8>(buffer[atPos]);
    return atPos;
}

static inline NEUtfString::CharPos _setSequence1( NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf8 ch)
{
    ASSERT(buffer != NULL);
    // one octet
    buffer[atPos ++ ]   = ch;
    return atPos;
}

static inline NEUtfString::CharPos _getSequence2( const NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf16 & out_char )
{
    ASSERT(buffer != NULL);
    NEUtfString::utf16 ch1     = buffer[   atPos];
    NEUtfString::utf8  ch2     = buffer[++ atPos];
    if ( NEUtfString::isTrail<NEUtfString::utf8>( ch2 ) )
    {
        ch1 = NEUtfString::mask8<NEUtfString::utf16>( ch1 );
        // two octets
        out_char = ((ch1 << 6) & 0x07FF) + (ch2 & 0x3F);
    }
    return atPos;
}

static inline NEUtfString::CharPos _setSequence2( NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf16 ch )
{
    ASSERT( buffer != NULL );
    buffer[atPos ++] = static_cast<NEUtfString::utf8>((ch >>   6) | 0xC0);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>((ch & 0x3F) | 0x80);
    return atPos;
}

static inline NEUtfString::CharPos _getSequence3( const NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf32 & out_char )
{
    ASSERT(buffer != NULL);
    NEUtfString::utf32 ch1 = buffer[   atPos];
    NEUtfString::utf16 ch2 = buffer[++ atPos];
    NEUtfString::utf8  ch3 = buffer[++ atPos];
    if ( NEUtfString::isTrail<NEUtfString::utf16>(ch2) && NEUtfString::isTrail<NEUtfString::utf8>(ch3) )
    {
        ch1     = NEUtfString::mask8<NEUtfString::utf32>(ch1);
        ch2     = NEUtfString::mask8<NEUtfString::utf16>(ch2);
        out_char= ((ch1 << 12) & 0xFFFF) + ((ch2 << 6) & 0x0FFF) + (ch3 & 0x3F);
    }
    return atPos;
}

static inline NEUtfString::CharPos _setSequence3( NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf32 ch )
{
    ASSERT(buffer != NULL);
    // three octets
    buffer[atPos ++] = static_cast<NEUtfString::utf8>( (ch >>  12)         | 0xE0);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>(((ch >>   6) & 0x3F) | 0x80);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>( (ch         & 0x3F) | 0x80);
    return atPos;
}

static inline NEUtfString::CharPos _getSequence4( const NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf32 & out_char )
{
    ASSERT(buffer != NULL);
    NEUtfString::utf32 ch1 = buffer[   atPos];
    NEUtfString::utf32 ch2 = buffer[++ atPos];
    NEUtfString::utf16 ch3 = buffer[++ atPos];
    NEUtfString::utf8  ch4 = buffer[++ atPos];
    if ( NEUtfString::isTrail<NEUtfString::utf32>(ch2) && NEUtfString::isTrail<NEUtfString::utf16>(ch3) )
    {
        ch1     = NEUtfString::mask8<NEUtfString::utf32>(ch1);
        ch2     = NEUtfString::mask8<NEUtfString::utf32>(ch2);
        ch3     = NEUtfString::mask8<NEUtfString::utf16>(ch3);
        out_char= ((ch1 << 18) & 0x001FFFFF) + ((ch2 << 12) & 0x0003FFFF) + ((ch3 << 6) & 0x0FFF) + (ch4 & 0x3F);
    }
    return atPos;
}

static inline NEUtfString::CharPos _setSequence4( NEUtfString::utf8 * buffer, NEUtfString::CharPos atPos, NEUtfString::utf32 ch )
{
    ASSERT(buffer != NULL);
    // four octets
    buffer[atPos ++] = static_cast<NEUtfString::utf8>( (ch >> 18)         | 0xF0);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>(((ch >> 12) & 0x3F) | 0x80);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>(((ch >>  6) & 0x3F) | 0x80);
    buffer[atPos ++] = static_cast<NEUtfString::utf8>( (ch        & 0x3F) | 0x80);
    return atPos;
}

NEUtfString::UtfChar NEUtfString::getInvalidChar( void )
{
    static NEUtfString::UtfChar _invalidUtfChar;
    _invalidUtfChar.u32Bits = NEUtfString::InvalidCharUtf32;
    return _invalidUtfChar;
}

NEUtfString::UtfChar NEUtfString::getEndOfString( void )
{
    static NEUtfString::UtfChar _endUtfChar;
    _endUtfChar.u32Bits = static_cast<unsigned int>(0);
    return _endUtfChar;
}

NEUtfString::sUtfStringHeader * NEUtfString::initString( NEUtfString::eEncoding encode
                                                    , NEUtfString::CharCount charCount
                                                    , unsigned int blockSize /*= DEFAULT_BLOCK_SIZE*/ )
{
    NEUtfString::sUtfStringHeader * result = NULL;
    charCount += 1;
    blockSize  = blockSize == 0 ? 1 : blockSize;
    unsigned int hdrSize = sizeof( NEUtfString::sUtfStringHeader );
    unsigned int bpch = minBytesPerChar( encode );
    unsigned int space = MACRO_ALIGN_SIZE( charCount * bpch, blockSize );
    unsigned int size = hdrSize + space;
    unsigned char * temp = DEBUG_NEW unsigned char[size];
    if ( temp != NULL )
    {
        switch ( encode )
        {
        case  NEUtfString::EncodeAscii:
        {
            NEUtfString::sAsciiString * strAscii = NEMemory::ConstructElems<NEUtfString::sAsciiString>(temp, 1);
            strAscii->utfBuffer[0] = NEUtfString::EndofStringAscii;
            result = &(strAscii->utfHeader);
        }
        break;

        case NEUtfString::EncodeUtf8:
        {
            NEUtfString::sUtf8String * strUtf8 = NEMemory::ConstructElems<NEUtfString::sUtf8String>(temp, 1);
            strUtf8->utfBuffer[0]   = NEUtfString::EndofStringUtf8;
            result = &(strUtf8->utfHeader);
        }
        break;

        case NEUtfString::EncodeUtf16:
        {
            NEUtfString::sUtf16String * strUtf16 = NEMemory::ConstructElems<NEUtfString::sUtf16String>(temp, 1);
            strUtf16->utfBuffer[0]  = NEUtfString::EndofStringUtf16;
            result = &(strUtf16->utfHeader);
        }
        break;

        case NEUtfString::EncodeUtf32:
        {
            NEUtfString::sUtf32String * strUtf32 = NEMemory::ConstructElems<NEUtfString::sUtf32String>(temp, 1);
            strUtf32->utfBuffer[0]  = NEUtfString::EndofStringUtf32;
            result = &(strUtf32->utfHeader);
        }
        break;

        default:
            delete [] temp;
            break;
        }
    }

    if ( result != NULL )
    {
        result->utfHdrSize  = hdrSize;
        result->utfSpace    = space / bpch;
        result->utfUsed     = 0;
        result->utfCount    = 0;
        result->utfEncode   = encode;
    }
    return result;
}

NEUtfString::utf8 NEUtfString::getSequenceOne( const NEUtfString::sUtf8String & utfString, unsigned int & atPos )
{
    NEUtfString::utf8 result = NEUtfString::InvalidCharUtf8;
    if ( canRead<NEUtfString::utf8>(utfString, atPos, NEUtfString::SeqOne) )
    {
        // one octet
        atPos = _getSequence1( getString(utfString), atPos, result);
    }
    else
    {
        atPos = InvalidPos;
    }
    return result;
}

NEUtfString::CharPos NEUtfString::setSequenceOne( NEUtfString::sUtf8String & utfString, NEUtfString::utf8 ch, NEUtfString::CharPos atPos )
{
    atPos = atPos == NEUtfString::EndPos ? utfString.utfHeader.utfUsed : atPos;
    if ( canWrite<utf8>(utfString, atPos, NEUtfString::SeqOne) )
    {
        // one octet
        atPos = _setSequence1( getString(utfString), atPos, ch);
    }
    else
    {
        atPos = NEUtfString::InvalidPos;
    }
    return atPos;
}

NEUtfString::utf16 NEUtfString::getSequenceTwo( const NEUtfString::sUtf8String & utfString, unsigned int & atPos )
{
    NEUtfString::utf16 result = NEUtfString::InvalidCharUtf16;
    if ( canRead<NEUtfString::utf8>( utfString, atPos, NEUtfString::SeqTwo ) )
    {
        // one octet
        atPos = _getSequence2( getString(utfString), atPos, result );
    }
    else
    {
        atPos = InvalidPos;
    }
    return result;
}

NEUtfString::CharPos NEUtfString::setSequenceTwo( NEUtfString::sUtf8String & utfString, NEUtfString::utf16 ch, NEUtfString::CharPos atPos )
{
    atPos = atPos == NEUtfString::EndPos ? utfString.utfHeader.utfUsed : atPos;
    if ( canWrite<utf8>( utfString, atPos, NEUtfString::SeqTwo ) )
    {
        // two octets
        atPos = _setSequence2(getString<utf8>( utfString ), atPos, ch);
    }
    else
    {
        atPos = NEUtfString::InvalidPos;
    }
    return atPos;
}

NEUtfString::utf32 NEUtfString::getSequenceThree( const NEUtfString::sUtf8String & utfString, unsigned int & atPos )
{
    NEUtfString::utf32 result = NEUtfString::InvalidCharUtf32;
    if ( canRead<NEUtfString::utf8>( utfString, atPos, NEUtfString::SeqTwo ) )
    {
        atPos = _getSequence3( getString(utfString), atPos, result);
    }
    else
    {
        atPos = InvalidPos;
    }
    return result;
}

NEUtfString::CharPos NEUtfString::setSequenceThree( NEUtfString::sUtf8String & utfString, NEUtfString::utf32 ch, NEUtfString::CharPos atPos )
{
    atPos = atPos == NEUtfString::EndPos ? utfString.utfHeader.utfUsed : atPos;
    if ( canWrite<utf8>( utfString, atPos, NEUtfString::SeqThree ) )
    {
        // 3 octets
        atPos = _setSequence3( getString<utf8>( utfString ), atPos, ch );
    }
    else
    {
        atPos = NEUtfString::InvalidPos;
    }
    return atPos;
}

NEUtfString::utf32 NEUtfString::getSequenceFour( const NEUtfString::sUtf8String & utfString, unsigned int & atPos )
{
    NEUtfString::utf32 result = NEUtfString::InvalidCharUtf32;
    if ( canRead<NEUtfString::utf8>( utfString, atPos, NEUtfString::SeqTwo ) )
    {
        atPos = _getSequence4( getString(utfString), atPos, result);
    }
    else
    {
        atPos = InvalidPos;
    }
    return result;
}

NEUtfString::CharPos NEUtfString::setSequenceFour( NEUtfString::sUtf8String & utfString, NEUtfString::utf32 ch, NEUtfString::CharPos atPos )
{
    atPos = atPos == NEUtfString::EndPos ? utfString.utfHeader.utfUsed : atPos;
    if ( canWrite<utf8>( utfString, atPos, NEUtfString::SeqFour ) )
    {
        atPos = _setSequence4(getString<utf8>( utfString ), atPos, ch);
    }
    else
    {
        atPos = NEUtfString::InvalidPos;
    }
    return atPos;
}

NEUtfString::UtfChar NEUtfString::validateNext( const NEUtfString::sUtf8String & utfString, unsigned int & atPos )
{
    NEUtfString::UtfChar result = getInvalidChar();
    // Save the original value of it so we can go back in case of failure
    // Of course, it does not make much sense with i.e. stream iterators
    unsigned int pos = atPos;
    utf8 ch = charAtPos<utf8>(utfString, pos);
    // Determine the sequence length based on the lead octet
    NEUtfString::eSeqLength seqLen = getSequenceLength<utf8>( ch );
    // Get trail octets and calculate the code point
    switch ( seqLen )
    {
    case NEUtfString::SeqOne:
        result.u32Bits = getSequenceOne( utfString, pos );
        break;
    case NEUtfString::SeqTwo:
        result.u32Bits = getSequenceTwo( utfString, pos );
        break;
    case NEUtfString::SeqThree:
        result.u32Bits = getSequenceThree( utfString, pos );
        break;
    case NEUtfString::SeqFour:
        result.u32Bits = getSequenceFour( utfString, pos );
        break;
    default:
        break;
    }

    if ( isValid<utf32>(result.u32Bits) )
    {
        if ( isCodePointValid(result.u32Bits) && isSequenceValid(result.u32Bits, seqLen) )
        {
            // Decoding succeeded. Now, security checks...
            // Passed! make changes to returns success.
            atPos = ++ pos;
        }
        else
        {
            result = getInvalidChar( );
        }
    }

    return result;
}

NEUtfString::CharPos NEUtfString::findInvalid( const NEUtfString::sUtf8String & utfString, NEUtfString::CharPos startPos /*= StartPos*/ )
{
    CharPos result = startPos;
    NEUtfString::UtfChar ch = getInvalidChar();
    do 
    {
        ch = validateNext(utfString, result);
    } while ( isValid<utf32>(ch.u32Bits) );
    return (result != utfString.utfHeader.utfUsed ? result : NEUtfString::InvalidPos);
}

NEUtfString::eBOM NEUtfString::getStringBOM( const NEUtfString::sUtf8String & utfString )
{
    // Byte order mark
    NEUtfString::eBOM result = NEUtfString::BOM_Undefined;
    const NEUtfString::sUtfStringHeader & hdr  = utfString.utfHeader;
    const NEUtfString::utf8 * buffer       = getString<utf8>(utfString);

    unsigned int bom = 0;
    while ( (result == NEUtfString::BOM_Undefined) && (_BOM_Indicator[bom] != static_cast<const unsigned char *>(NULL)) )
    {
        const unsigned char *   entry   = _BOM_Indicator[bom];
        const unsigned int      count   = _BOM_Lengths[bom];
        if ( ( hdr.utfUsed * minBytesPerChar(bom2Encode(static_cast<NEUtfString::eBOM>(bom))) ) >= count )
        {
            unsigned int i = 0;
            for ( ; (entry[i] == buffer[i]) && (i < count); ++ i )
                ;

            result = (i == count ? static_cast<NEUtfString::eBOM>(bom) : NEUtfString::BOM_Undefined);
        }
        ++ bom;
    }
    return result;
}

bool NEUtfString::startsWithUtf8Bom( const NEUtfString::sUtf8String & utfString )
{
    return (getStringBOM(utfString) == BOM_UTF8);
}

NEUtfString::CharPos NEUtfString::setAt( NEUtfString::utf32 newChar
                                         , NEUtfString::sUtf8String & utfString
                                         , unsigned int atPos /*= EndPos */ )
{
    NEUtfString::eSeqLength seqLen = getSequenceLength<utf32>( newChar );
    switch ( seqLen )
    {
    case NEUtfString::SeqOne:
        // one octet
        atPos = setSequenceOne(utfString, newChar, atPos);
        break;

    case NEUtfString::SeqTwo:
        // two octets
        atPos = setSequenceTwo( utfString, newChar, atPos );
        break;

    case NEUtfString::SeqThree:
        // three octets
        atPos = setSequenceThree( utfString, newChar, atPos );
        break;

    case NEUtfString::SeqFour:
        // four octets
        atPos = setSequenceFour( utfString, newChar, atPos );
        break;

    default:
        break;
    }

    if ( (atPos != NEUtfString::InvalidPos) && (atPos > utfString.utfHeader.utfUsed) )
    {
        utf8 * buffer = getString<utf8>(utfString);
        utfString.utfHeader.utfUsed = atPos;
        utfString.utfHeader.utfCount += 1;
        buffer[atPos] = NEUtfString::EndofStringUtf8;
    }

    return atPos;
}

NEUtfString::CharPos NEUtfString::appendChar( NEUtfString::sUtf8String *& utfString, NEUtfString::utf32 newChar )
{
    CharPos result = NEUtfString::InvalidPos;
    NEUtfString::eSeqLength seqLen = NEUtfString::getSequenceLength( newChar );
    if ( (seqLen != NEUtfString::SeqInvalid) && (utfString != static_cast<sUtf8String *>(NULL)) )
    {
        if ( canWrite(*utfString, utfString->utfHeader.utfUsed, seqLen) == false )
        {
            NEUtfString::sUtf8String * newUtf  = reserveSpace<utf8>(*utfString, getSequenceSpace(seqLen) + utfString->utfHeader.utfUsed, NEUtfString::DEFAULT_BLOCK_SIZE);
            if ( newUtf != utfString )
            {
                releaseString( *utfString );
                utfString = newUtf;
            }
        }
        result = setAt( newChar, *utfString, NEUtfString::EndPos );
    }
    return result;
}

NEUtfString::CharPos NEUtfString::insertAt( NEUtfString::utf32 newChar
                                    , NEUtfString::sUtf8String *& utfString
                                    , NEUtfString::CharPos atPos /*= StartPos*/ )
{
    CharPos result = NEUtfString::InvalidPos;
    NEUtfString::eSeqLength seqLen = NEUtfString::getSequenceLength( newChar );
    if ( (seqLen != NEUtfString::SeqInvalid) && (utfString != static_cast<sUtf8String *>(NULL)) )
    {
        if ( canWrite( *utfString, utfString->utfHeader.utfUsed, seqLen ) == false )
        {
            NEUtfString::sUtf8String * newUtf = reserveSpace<utf8>( *utfString, getSequenceSpace(seqLen)+ utfString->utfHeader.utfUsed, NEUtfString::DEFAULT_BLOCK_SIZE );
            if ( newUtf != utfString )
            {
                releaseString( *utfString );
                utfString = newUtf;
            }
        }
        NEUtfString::sUtfStringHeader &hdr = utfString->utfHeader;
        if ( canWrite( *utfString, hdr.utfUsed, seqLen ) )
        {
            atPos = atPos == NEUtfString::EndPos ? hdr.utfUsed : atPos;
            if ( (atPos + getSequenceSpace(seqLen)) < hdr.utfSpace )
            {
                utf8 * buffer = getString<utf8>( *utfString );
                unsigned int len = getSequenceSpace( seqLen );
                ASSERT( buffer != NULL );
                NEMemory::MoveElems<utf8>( buffer + atPos + len, buffer + atPos, hdr.utfUsed + 1 );
                hdr.utfCount += 1;
                hdr.utfUsed  += len;

                switch ( seqLen )
                {
                case NEUtfString::SeqOne:
                    // one octet
                    result = setSequenceOne( *utfString, newChar, atPos );
                    break;

                case NEUtfString::SeqTwo:
                    // two octets
                    result = setSequenceTwo( *utfString, newChar, atPos );
                    break;

                case NEUtfString::SeqThree:
                    // three octets
                    result = setSequenceThree( *utfString, newChar, atPos );
                    break;

                case NEUtfString::SeqFour:
                    // four octets
                    result = setSequenceFour( *utfString, newChar, atPos );
                    break;

                default:
                    ASSERT(false);
                    break;
                }
            }
        }
    }
    return result;
}

NEUtfString::eSeqLength NEUtfString::getAt( NEUtfString::utf32 & out_Char
                                    , const NEUtfString::sUtf8String & utfString
                                    , NEUtfString::CharPos atPos )
{
    out_Char = NEUtfString::InvalidCharUtf32;
    NEUtfString::eSeqLength result = getSequenceLength( charAtPos(utfString, atPos) );
    switch ( result )
    {
    case NEUtfString::SeqOne:
        out_Char = getSequenceOne( utfString, atPos );
        break;

    case NEUtfString::SeqTwo:
        out_Char = getSequenceTwo( utfString, atPos );
        break;

    case NEUtfString::SeqThree:
        out_Char = getSequenceThree( utfString, atPos );
        break;

    case NEUtfString::SeqFour:
        out_Char = getSequenceFour( utfString, atPos );
        break;

    default:
        break;
    }
    return result;
}


NEUtfString::utf32 NEUtfString::getNext( const NEUtfString::sUtf8String & utfString, NEUtfString::CharPos & startPos )
{
    utf32 result = NEUtfString::InvalidCharUtf32;
    NEUtfString::eSeqLength seqLen = getAt(result, utfString, startPos);
    startPos = seqLen != NEUtfString::SeqInvalid ? (startPos + getSequenceSpace(seqLen)) : NEUtfString::InvalidPos ;
    return result;
}

NEUtfString::utf32 NEUtfString::getPrev( const NEUtfString::sUtf8String & utfString, NEUtfString::CharPos & startPos )
{
    NEUtfString::utf8 ch       = NEUtfString::InvalidCharUtf8;
    NEUtfString::utf32 result  = NEUtfString::InvalidCharUtf32;
    NEUtfString::CharPos  atPos    = startPos;
    do 
    {
        ch = charAtPos<utf8>(utfString, -- atPos);
    } while ( canRead(utfString, startPos, NEUtfString::SeqOne) && isTrail(ch) );
    
    startPos = atPos;
    getAt( result, utfString, atPos );
    return result;
}

void NEUtfString::skipChars(  const NEUtfString::sUtf8String & utfString
                            , NEUtfString::CharPos & startPos
                            , NEUtfString::CharCount charCount )

{
    for ( unsigned int i = 0; canRead(utfString, startPos, NEUtfString::SeqOne)  && (i < charCount); ++ i )
        static_cast<void>( getNext(utfString, startPos) );
}

NEUtfString::CharCount NEUtfString::charCount(const NEUtfString::sUtf8String & utfString
                                            , NEUtfString::CharPos startPos /*= StartPos*/
                                            , CharPos endPos /*= EndPos*/)
{
    CharCount result = NEUtfString::InvalidPos;
    if ( (startPos == NEUtfString::StartPos) && (endPos == NEUtfString::EndPos) )
    {
        result = utfString.utfHeader.utfCount;
    }
    else
    {
        for ( ; (startPos < endPos) && canRead( utfString, startPos, NEUtfString::SeqOne ); ++ result )
            static_cast<void>(getNext( utfString, startPos ));
    }
    return result;
}

NEUtfString::CharCount NEUtfString::convertUtf16ToUtf8( NEUtfString::sUtf8String *& utf8Destination
                                                , const NEUtfString::sUtf16String & utf16Source
                                                , NEUtfString::CharPos startPos /*= StartPos*/
                                                , NEUtfString::CharCount charCount /*= EndPos*/ )
{
    NEUtfString::CharCount result = NEUtfString::InvalidPos;
    if ( utf8Destination != static_cast<NEUtfString::sUtf8String *>(NULL) )
    {
        const NEUtfString::sUtfStringHeader & hdrSrc = utf16Source.utfHeader;

        const utf16 * src   = getString<utf16>( utf16Source );
        CharCount oldUsed       = utf8Destination->utfHeader.utfUsed;
        ASSERT( hdrSrc.utfEncode == NEUtfString::EncodeUtf16 );
        ASSERT( utf8Destination->utfHeader.utfEncode == NEUtfString::EncodeUtf8 );
        charCount = charCount == NEUtfString::EndPos ? (hdrSrc.utfUsed - startPos) : charCount;
        for ( ; (charCount != 0) && (canRead<utf16>( utf16Source, startPos, NEUtfString::SeqOne )); -- charCount )
        {
            utf16 ch = mask16( src[startPos ++] );
            if ( isLeadSurrogate( ch ) )
            {
                utf16 trailSurogate = mask16( src[startPos ++] );
                ch = (ch << 10) + trailSurogate + NEUtfString::SURROGATE_OFFSET;
            }
            static_cast<void>(appendChar( utf8Destination, ch ));
        }
        result = utf8Destination->utfHeader.utfUsed - oldUsed;
    }
    return result;
}

NEUtfString::CharCount NEUtfString::convertUtf8ToUtf16( NEUtfString::sUtf16String *& utf16Destination
                                                , const NEUtfString::sUtf8String & utf8Source
                                                , NEUtfString::CharPos startPos /*= StartPos*/
                                                , NEUtfString::CharCount charCount /*= EndPos*/ )
{
    NEUtfString::CharCount result = NEUtfString::InvalidPos;
    if ( utf16Destination != static_cast<NEUtfString::sUtf16String *>(NULL) )
    {
        const NEUtfString::sUtfStringHeader & hdrSrc = utf8Source.utfHeader;

        charCount = charCount == NEUtfString::EndPos ? (hdrSrc.utfUsed - startPos) : charCount;
        CharCount oldUsed = utf16Destination->utfHeader.utfUsed;
        ASSERT( hdrSrc.utfEncode == NEUtfString::EncodeUtf8 );
        ASSERT( utf16Destination->utfHeader.utfEncode == NEUtfString::EncodeUtf16 );

        utf16 * dst = getString<utf16>(*utf16Destination);
        CharCount index = utf16Destination->utfHeader.utfUsed;

        for ( ; (charCount != 0) && (canRead<utf8>( utf8Source, startPos, NEUtfString::SeqOne )); -- charCount )
        {
            utf32 ch = getNext(utf8Source, startPos);
            if ( ch > static_cast<utf32>(0x0000FFFF) )
            {
                if ( (index + 3) >= utf16Destination->utfHeader.utfSpace )
                {
                    NEUtfString::sUtf16String * tmp = reserveSpace<utf16>(*utf16Destination, index + 2, DEFAULT_BLOCK_SIZE);
                    if ( tmp != NULL )
                    {
                        releaseString<utf16>(*utf16Destination);
                        utf16Destination = tmp;
                        dst = getString<utf16>(*utf16Destination);
                    }
                    else
                    {
                        break;
                    }
                }
                dst[index ++] = static_cast<utf16>((ch >>   10) + LEAD_OFFSET);
                dst[index ++] = static_cast<utf16>((ch & 0x3ff) + TRAIL_SURROGATE_MIN);
            }
            else
            {
                if ( (index + 2) >= utf16Destination->utfHeader.utfSpace )
                {
                    NEUtfString::sUtf16String * tmp = reserveSpace<utf16>( *utf16Destination, index + 2, DEFAULT_BLOCK_SIZE );
                    if ( tmp != NULL )
                    {
                        releaseString<utf16>( *utf16Destination );
                        utf16Destination = tmp;
                        dst = getString<utf16>( *utf16Destination );
                    }
                    else
                    {
                        break;
                    }
                }
                dst[index ++] = static_cast<utf16>(ch);
            }
            utf16Destination->utfHeader.utfUsed += 1;
        }
        dst[index]  = NEUtfString::EndofStringUtf16;
        result      = utf16Destination->utfHeader.utfUsed - oldUsed;
    }
    return result;
}

NEUtfString::CharCount NEUtfString::convertUtf32ToUtf8( NEUtfString::sUtf8String *& utf8Destination
                                                , const NEUtfString::sUtf32String & utf32Source
                                                , NEUtfString::CharPos startPos /*= StartPos*/
                                                , NEUtfString::CharCount charCount /*= EndPos */ )
{
    NEUtfString::CharCount result = NEUtfString::InvalidPos;
    if ( utf8Destination != static_cast<NEUtfString::sUtf8String *>(NULL) )
    {
        const NEUtfString::sUtfStringHeader & hdrSrc = utf32Source.utfHeader;

        const utf32 * src   = getString<utf32>( utf32Source );
        CharCount oldUsed       = utf8Destination->utfHeader.utfUsed;
        ASSERT( hdrSrc.utfEncode == NEUtfString::EncodeUtf32 );
        ASSERT( utf8Destination->utfHeader.utfEncode == NEUtfString::EncodeUtf8 );
        charCount = charCount == NEUtfString::EndPos ? (hdrSrc.utfUsed - startPos) : charCount;
        for ( ; (charCount != 0) && (canRead<utf32>( utf32Source, startPos, NEUtfString::SeqOne )); -- charCount )
        {
            static_cast<void>(appendChar( utf8Destination, src[startPos ++]));
        }
        result = utf8Destination->utfHeader.utfUsed - oldUsed;
    }
    return result;
}

NEUtfString::CharCount NEUtfString::convertUtf8ToUtf32( NEUtfString::sUtf32String *& utf32Destination
                                                , const NEUtfString::sUtf8String &utf8Source
                                                , NEUtfString::CharPos startPos /*= StartPos*/
                                                , NEUtfString::CharCount charCount /*= EndPos */ )
{
    NEUtfString::CharCount result = NEUtfString::InvalidPos;
    if ( utf32Destination != static_cast<NEUtfString::sUtf32String *>(NULL) )
    {
        const NEUtfString::sUtfStringHeader & hdrSrc = utf8Source.utfHeader;

        charCount = charCount == NEUtfString::EndPos ? (hdrSrc.utfUsed - startPos) : charCount;
        CharCount oldUsed = utf32Destination->utfHeader.utfUsed;
        ASSERT( hdrSrc.utfEncode == NEUtfString::EncodeUtf8 );
        ASSERT( utf32Destination->utfHeader.utfEncode == NEUtfString::EncodeUtf16 );

        utf32 * dst = getString<utf32>( *utf32Destination );
        CharCount index = utf32Destination->utfHeader.utfUsed;

        for ( ; (charCount != 0) && (canRead<utf8>( utf8Source, startPos, NEUtfString::SeqOne )); -- charCount )
        {
            utf32 ch = getNext( utf8Source, startPos );
            if ( (index + 2) >= utf32Destination->utfHeader.utfSpace )
            {
                NEUtfString::sUtf32String * tmp = reserveSpace<utf32>( *utf32Destination, index + 2, DEFAULT_BLOCK_SIZE );
                if ( tmp != NULL)
                {
                    releaseString<utf32>( *utf32Destination );
                    utf32Destination = tmp;
                    dst = getString<utf32>( *utf32Destination );
                }
                else
                {
                    break;
                }
            }
            dst[index ++] = static_cast<utf16>(ch);
            utf32Destination->utfHeader.utfUsed += 1;
        }
        dst[index]  = NEUtfString::EndofStringUtf16;
        result      = utf32Destination->utfHeader.utfUsed - oldUsed;
    }
    return result;
}
