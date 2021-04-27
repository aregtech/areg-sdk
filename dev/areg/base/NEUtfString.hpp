#ifndef AREG_BASE_NEUTFSTRING_HPP
#define AREG_BASE_NEUTFSTRING_HPP
/************************************************************************
 * \file        areg/base/NEUtfString.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, namespace with UTF String helper methods
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NECommon.hpp"
#include "areg/base/NEString.hpp"
#include "areg/base/NEMemory.hpp"

namespace NEUtfString
{
    /**
     * \brief   NEUtfString::CharPos, Definition of character position type in string
     **/
    typedef NECommon::CharPos       CharPos;
    /**
     * \brief   NEUtfString::CharCount, Definition of character counting type in string
     **/
    typedef NECommon::CharCount     CharCount;

    /**
     * \brief   NEUtfString::Char, ASCII character
     **/
    typedef char                    Char;
    /**
     * \brief   NEUtfString::utf8, UTF-8 character
     **/
    typedef unsigned char           utf8;
    /**
     * \brief   NEUtfString::utf16, UTF-16 character
     **/
    typedef unsigned short          utf16;
    /**
     * \brief   NEUtfString::utf32, UTF-32 character
     **/
    typedef unsigned int            utf32;

    const unsigned int DEFAULT_BLOCK_SIZE   = NECommon::DEFAULT_BLOCK_SIZE; // Default block size to allocate string buffer
    const CharPos InvalidPos                = NECommon::InvalidPos;
    const CharPos StartPos                  = NECommon::StartPos;
    const CharPos EndPos                    = NECommon::EndPos;


    /**
     * \brief   The Sequence Length of UTF string
     *          NEUtfString::eSeqLength
     **/
    typedef enum E_SeqLength
    {
          SeqInvalid    = 0     //<! Invalid Sequence length
        , SeqOne        = 1     //<! Sequence length 1 symbol
        , SeqTwo        = 2     //<! Sequence length 2 symbols
        , SeqThree      = 3     //<! Sequence length 3 symbols 
        , SeqFour       = 4     //<! Sequence length 4 symbols
    } eSeqLength;

    /**
     * \brief   The Encoding of UTF string
     *          NEUtfString::eEncoding
     **/
    typedef enum E_Encode
    {
          EncodeInvalid = -1    //<! Invalid encoding
        , EncodeAscii           //<! No encoding, ASCII string
        , EncodeUtf8            //<! Encoding UTF-8  string
        , EncodeUtf16           //<! Encoding UTF-16 string
        , EncodeUtf32           //<! Encoding UTF-32 string
    } eEncoding;

    /**
     * \brief   The Byte Order Mark (BOM) of a string.
     *          BOM is a magic number at the begin of string indicating:
     *              - the byte order of text (or endianness)
     *              - the encoding of the text
     *              - the indication of Unicode encoding
     **/
    typedef enum E_BOM
    {
          BOM_UTF8      // The text in string is in UTF-8 format
        , BOM_UTF16_BE  // The text in string is in UTF-16 format and the byte order is Big-endian
        , BOM_UTF16_LE  // The text in string is in UTF-16 format and the byte order is Little-endian
        , BOM_UTF32_BE  // The text in string is in UTF-32 format and the byte order is Big-endian
        , BOM_UTF32_LE  // The text in string is in UTF-32 format and the byte order is Little-endian
        , BOM_Undefined // The encoding, format and byte orders are unknown
    } eBOM;

    /**
     * \brief   ASCII character
     *          NEUtfString::Char
     **/
    typedef char            Char;
    /**
     * \brief   UTF-8 character
     *          NEUtfString::utf8
     **/
    typedef unsigned char   utf8;
    /**
     * \brief   UTF-16 character
     *          NEUtfString::utf16
     **/
    typedef unsigned short  utf16;
    /**
     * \brief   UTF-32 character
     *          NEUtfString::utf32
     **/
    typedef unsigned int    utf32;

    /**
     * \brief   Union of string characters
     *          NEUtfString::uUtfChar
     **/
    typedef union U_UtfChar
    {
        Char    u7Bits;     //<! 7-bit character, used in ASCII string
        utf8    u8Bits;     //<! 8-bit character, used in UTF-8 string
        utf16   u16Bits;    //<! 16-bit character, used in URF-16 string
        utf32   u32Bits;    //<! 32-bit character, used in UTF-32 string
    } uUtfChar;

    /**
     * \brief   Definition of URF character
     *          NEUtfString::UtfChar
     **/
    typedef uUtfChar    UtfChar;

    /**
     * \brief   UTF encoded string header, common for all strings
     *          NEUtfString::sUtfStringHeader
     **/
    typedef struct S_UtfStringHeader
    {
        unsigned int    utfHdrSize;     //<! The size of header to escape to start of string data
        unsigned int    utfSpace;       //<! The space reserved in chars
        unsigned int    utfUsed;        //<! The space used in chars
        unsigned int    utfCount;       //<! The number of characters
        eEncoding       utfEncode;      //<! The encoding
    } sUtfStringHeader;

    /**
     * \brief   UTF String, common for all strings
     *          NEUtfString::SString
     **/
    template<typename CharType>
    struct SUtfString
    {
        sUtfStringHeader    utfHeader;      //<! UTF string header
        CharType            utfBuffer[1];   //<! Begin of UTF string buffer
    };


    /**
     * \brief   Predefined type of ASCII string structure
     *          NEUtfString::sAsciiString
     **/
    typedef SUtfString<Char>    sAsciiString;
    /**
     * \brief   Predefined type of UTF-8 string structure
     *          NEUtfString::sUtf8String
     **/
    typedef SUtfString<utf8>    sUtf8String;
    /**
     * \brief   Predefined type of UTF-16 string structure
     *          NEUtfString::sUtf16String
     **/
    typedef SUtfString<utf16>   sUtf16String;
    /**
     * \brief   Predefined type of UTF-32 string structure
     *          NEUtfString::sUtf32String
     **/
    typedef SUtfString<utf32>   sUtf32String;

    /**
     * \brief   Definition of Invalid Character structure template
     *          NEUtfString::S_InvalidChar
     **/
    template<typename CharType>
    struct S_InvalidChar
    {
        /**
         * \brief   Unchanged data defined as invalid character for different encodings
         **/
        const CharType  data;
        /**
         * \brief   Default constructor to initialize internal data
         **/
        S_InvalidChar( void ) : data (static_cast<CharType>(-1)) {}
        /**
         * \brief   Conversion operator
         **/
        operator CharType( void ) const
        {   return data;    }
    };

    /**
     * \brief   Definition of End of String (EOS) character structure template
     *          NEUtfString::S_EndofString
     **/
    template<typename CharType>
    struct S_EndofString
    {
        /**
         * \brief   Unchanged data defined as end of string (EOS) character for different encodings
         **/
        const CharType  data;
        /**
         * \brief   Default constructor to initialize internal data
         **/
        S_EndofString( void ) : data (static_cast<CharType>('\0')) {}
        /**
         * \brief   Conversion operator
         **/
        operator CharType( void ) const
        {   return data;    }
    };

    /**
     * \brief   Declaration of ASCII Invalid character
     *          NEUtfString::InvalidCharAscii
     **/
    const S_InvalidChar<Char>  InvalidCharAscii;
    /**
     * \brief   Declaration of UTF-8 Invalid character
     *          NEUtfString::InvalidCharUtf8
     **/
    const S_InvalidChar<utf8>   InvalidCharUtf8;
    /**
     * \brief   Declaration of UTF-16 Invalid character
     *          NEUtfString::InvalidCharUtf16
     **/
    const S_InvalidChar<utf16>  InvalidCharUtf16;
    /**
     * \brief   Declaration of UTF-32 Invalid character
     *          NEUtfString::InvalidCharUtf32
     **/
    const S_InvalidChar<utf32>  InvalidCharUtf32;

    /**
     * \brief   Declaration of ASCII end-of-string (EOS) character
     *          NEUtfString::EndofStringAscii
     **/
    const S_EndofString<Char>  EndofStringAscii;
    /**
     * \brief   Declaration of UTF-8 end-of-string (EOS) character
     *          NEUtfString::EndofStringUtf8
     **/
    const S_EndofString<utf8>   EndofStringUtf8;
    /**
     * \brief   Declaration of UTF-16 end-of-string (EOS) character
     *          NEUtfString::EndofStringUtf16
     **/
    const S_EndofString<utf16>  EndofStringUtf16;
    /**
     * \brief   Declaration of UTF-32 end-of-string (EOS) character
     *          NEUtfString::EndofStringUtf32
     **/
    const S_EndofString<utf32>  EndofStringUtf32;

    // Unicode constants
    // Leading (high) surrogates: 0xd800 - 0xdbff
    // Trailing (low) surrogates: 0xdc00 - 0xdfff
    const unsigned short LEAD_SURROGATE_MIN     = static_cast<unsigned short>(0xD800u);
    const unsigned short LEAD_SURROGATE_MAX     = static_cast<unsigned short>(0xDBFFu);
    const unsigned short TRAIL_SURROGATE_MIN    = static_cast<unsigned short>(0xDC00u);
    const unsigned short TRAIL_SURROGATE_MAX    = static_cast<unsigned short>(0xDFFFu);
    const unsigned short LEAD_OFFSET            = LEAD_SURROGATE_MIN - static_cast<unsigned short>((0x10000 >> 10));
    const unsigned int   SURROGATE_OFFSET       = static_cast<unsigned int>(0x10000u) - (static_cast<unsigned int>(LEAD_SURROGATE_MIN) << 10) - TRAIL_SURROGATE_MIN;

    // Maximum valid value for a Unicode code point
    const unsigned int  CODE_POINT_MAX          = static_cast<unsigned int>(0x0010FFFFu);

    /**
     * \brief   Returns invalid UTF character
     **/
    UtfChar getInvalidChar( void );

    /**
     * \brief   Returns end-of-string (EOS) UTF character
     **/
    UtfChar getEndOfString( void );

    /**
     * \brief   Returns number of minimum bytes per character of specified encoding
     * \param   encode  The UTF encoding
     * \return  Returns number of minimum bytes per character of specified encoding.
     **/
    inline unsigned int minBytesPerChar( eEncoding encode );

    /**
     * \brief   Returns the space in character counts required for specified sequence length.
     * \param   seqLength   The Sequence Length.
     **/
    inline unsigned int getSequenceSpace( eSeqLength seqLength );

    /**
     * \brief   Returns string encoding based on BOM (Byte Order Mark)
     * \param   bom     The Byte Order Mark (BOM) value to convert
     **/
    inline eEncoding bom2Encode( eBOM bom );

    template<typename CharType>
    inline eEncoding getEncoding( void );
    template<>
    inline eEncoding getEncoding<Char>( void );
    template<>
    inline eEncoding getEncoding<utf8>( void );
    template<>
    inline eEncoding getEncoding<utf16>( void );
    template<>
    inline eEncoding getEncoding<utf32>( void );

    /**
     * \brief   Returns read only UTF string data buffer
     * \param   utfString   The read only instance of UTF string structure, 
     *                      which contains information of string
     * \return  Returns read only UTF string data buffer
     **/
    template<typename CharType>
    inline const CharType * getString( const SUtfString<CharType> & utfString );

    /**
     * \brief   Returns UTF string data buffer, which can be modified
     * \param   utfString   The read-write instance of UTF string structure, 
     *                      which contains information of string
     * \return  Returns UTF string data buffer, which can be modified
     **/
    template<typename CharType>
    inline CharType * getString( SUtfString<CharType> & utfString );

    /**
     * \brief   Returns character at give position. 
     *          If position is wrong, returns invalid character
     * \param   utfString   The instance of UTF string structure,
     *                      which contains information of string
     * \return  Returns string value at give position.
     *          If position is invalid, returns invalid character value.
     **/
    template<typename CharType>
    inline CharType charAtPos( const SUtfString<CharType> & utfString, CharPos pos );

    /**
     * \brief   Returns applied 8-bit mask of given character
     * \param   ch      The given character to apply 8-bit mask
     * \return  Returns applied 8-bit mask of given character
     **/
    template<typename CharType>
    inline utf8 mask8( CharType ch );

    /**
     * \brief   Returns applied 16-bit mask of given character
     * \param   ch      The given character to apply 16-bit mask
     * \return  Returns applied 16-bit mask of given character
     **/
    template<typename CharType>
    inline utf16 mask16( CharType ch );

    /**
     * \brief   Returns true if specified character is trail used in UTF string
     * \param   ch      The given character to check the trail
     * \return  Returns true if specified character is trail used in UTF string
     **/
    template<typename CharType>
    inline bool isTrail( CharType ch );

    /**
     * \brief   Returns sequence length required to create UTF-8 string
     * \param   ch      The given character to check trails
     * \return  Returns sequence length required to create UTF-8 string
     **/
    template<typename CharType>
    inline eSeqLength getSequenceLength(CharType ch);

    /**
     * \brief   Returns true if specified character has correct sequence length
     * \param   ch      The given character to check sequence
     * \return  Returns true if specified character has correct sequence length
     **/
    template<typename CharType>
    inline bool isSequenceValid(CharType ch, eSeqLength seqLen);

    /**
     * \brief   Returns true if specified character is lead surrogate
     * \param   ch      The given character to check lead surrogate
     * \return  Returns true if specified character is lead surrogate
     **/
    template<typename CharType>
    inline bool isLeadSurrogate( CharType ch );

    /**
     * \brief   Returns true if specified character is trail surrogate
     * \param   ch      The given character to check trail surrogate
     * \return  Returns true if specified character is trail surrogate
     **/
    template<typename CharType>
    inline bool isTrailSurrogate( CharType ch );

    /**
     * \brief   Returns true if specified character is surrogate
     * \param   ch      The given character to check surrogate
     * \return  Returns true if specified character is surrogate
     **/
    template<typename CharType>
    inline bool isSurrogate( CharType ch );

    /**
     * \brief   Returns true if specified code-point character is valid
     * \param   ch      The given character to check surrogate
     * \return  Returns true if specified code-point character is valid
     **/
    template<typename CharType>
    inline bool isCodePointValid(CharType ch);

    /**
     * \brief   Returns true if specified character is no invalid.
     * \param   ch      The character to check
     **/
    template<typename CharType>
    inline bool isValid( CharType ch );

    /**
     * \brief   Returns true if it is possible to read UTF character of specified sequence
     *          length at specified position.
     * \param   utfString   The source of UTF string to read character
     * \param   atPos       The position to start reading UTF character
     * \param   seqLen      The expected sequence length to read character.
     * \return  Returns true if can read UTF character at specified position of specified sequence length.
     *          Otherwise, it returns false.
     **/
    template<typename CharType>
    inline bool canRead( const SUtfString<CharType> & utfString, CharPos atPos, eSeqLength seqLen );

    /**
     * \brief   Returns true if it is possible to write UTF character of specified sequence
     *          length starting at specified position. The UTF string should have enough
     *          buffer space to write UTF character
     * \param   utfString   The UTF string to check space to write UTF character
     * \param   atPos       The position to write UTF character
     * \param   seqLen      The expected sequence length to write character.
     * \return  Returns true if can read UTF character at specified position of specified sequence length.
     *          Otherwise, it returns false.
     **/
    template<typename CharType>
    inline bool canWrite( const SUtfString<CharType> & utfString, CharPos atPos, eSeqLength seqLen );

    /**
     * \brief   Checks whether the string is valid or not. 
     *          The valid string should have valid sequence length and code point.
     * \param   utfString   The UTF string to check
     * \param   startPos    The position in buffer to start checking.
     *                      If value is NEUtfString::StartPos, it will check the complete string.
     *                      Otherwise, it starts checking at given position.
     * \return  Returns true if UTF string is valid.
     **/
    template<typename CharType>
    inline bool isValidString( const SUtfString<CharType> & utfString, CharPos startPos = StartPos );

    /**
     * \brief   Release previously allocated space for UTF string (header and data)
     * \param   utfString   The UTF string to release.
     **/
    template<typename CharType>
    inline void releaseString( SUtfString<CharType> & utfString );

    /**
     * \brief   Checks whether the passed UTF String has enough space to copy characters or not.
     *          If passed UTF string has enough space to copy characters, it makes no changes and returns NULL.
     *          Otherwise it reallocates the space for string buffer, copies the existing data into the new buffer and returns valid pointer.
     *          Note, if new space is allocated, the existing UTF String structure is no released.
     *          Call manually releaseUtfString() to release existing UTF string.
     * \param   utfString   The existing UTF string to check the space.
     * \param   charCount   Number of characters to add in UTF string.
     * \param   blockSize   The size of minimum block to allocate. When allocates string buffer,
     *                      the size is aligned to blockSize value. Bu default, it is NEUtfString::DEFAULT_BLOCK_SIZE
     * \return  Returns valid pointer if new space is allocated. If no space is allocated, i.e. the existing UTF string has enough space,
     *          or space allocation failed, the function returns NULL.
     *          Note, if function returns valid pointer, the passed 'utfString' should be manually released.
     **/
    template<typename CharType>
    SUtfString<CharType> * reserveSpace( const SUtfString<CharType> & utfString, CharCount charCount, unsigned int blockSize = DEFAULT_BLOCK_SIZE);

    /**
    * \brief   Initializes empty UTF String structure based on requested Encoding, sets initial parameters and specifies encoding.
    *          New created UTF String is empty.
    * \param   encode      The encoding of UTF String
    * \param   charCount   The minimum space to reserve to fill initial characters.
    * \param   blockSize   The size of minimum block to allocate. When allocates string buffer,
    *                      the size is aligned to blockSize value. Bu default, it is NEUtfString::DEFAULT_BLOCK_SIZE
    * \return  Returns valid pointer if space is allocated and UTF string is initialized. Otherwise, it returns NULL.
    *          Note, if function returns valid pointer, the passed 'utfString' should be manually released.
    **/
    sUtfStringHeader * initString( eEncoding encode, CharCount charCount, unsigned int blockSize = DEFAULT_BLOCK_SIZE);

    /**
     * \brief   Reads UTF-8 character of sequence length 1 starting at given position.
     *          At return, the value 'atPos' will continue the position in string buffer
     *          of next UTF character. If reached to end and cannot read character anymore
     *          the value of 'atPos' is NEUtfString::InvalidPos.
     * \param   utfString   The source of UTF String to read character
     * \param   atPos       The position to start reading UTF character.
     *                      It should be valid position in buffer. Use StartPos to start reading first character.
     *                      On return, the value is the position of next UTF character to read.
     *                      If reached to end or cannot read character, on return the value is NEUtfString::InvalidPos.
     * \return  Returns value as UTF-8 character if succeeded to read. 
     *          Otherwise, the return value is Invalid character.
     **/
    utf8 getSequenceOne( const sUtf8String & utfString, CharPos & atPos );

    /**
     * \brief   Sets character at given position of sequence length 1.
     * \param   utfString   The UTF-8 String structure, which contains string buffer.
     * \param   ch          The character to set
     * \param   atPos       The position to set character.
     * \return  Returns position to next character to set in UTF-8 buffer
     **/
    CharPos setSequenceOne( sUtf8String & utfString, utf8 ch, CharPos atPos );

    /**
     * \brief   Reads UTF-8 character of sequence length 2 starting at given position.
     *          At return, the value 'atPos' will continue the position in string buffer
     *          of next UTF character. If reached to end and cannot read character anymore
     *          the value of 'atPos' is NEUtfString::InvalidPos.
     * \param   utfString   The source of UTF String to read character
     * \param   atPos       The position to start reading UTF character.
     *                      It should be valid position in buffer. Use StartPos to start reading first character.
     *                      On return, the value is the position of next UTF character to read.
     *                      If reached to end or cannot read character, on return the value is NEUtfString::InvalidPos.
     * \return  Returns value as UTF-16 character if succeeded to read.
     *          Otherwise, the return value is Invalid character.
     **/
    utf16 getSequenceTwo( const sUtf8String & utfString, CharPos & atPos );

    /**
     * \brief   Sets character at given position of sequence length 2.
     * \param   utfString   The UTF-8 String structure, which contains string buffer.
     * \param   ch          The character to set
     * \param   atPos       The position to set character.
     * \return  Returns position to next character to set in UTF-8 buffer
     **/
    CharPos setSequenceTwo( sUtf8String & utfString, utf16 ch, CharPos atPos );

    /**
     * \brief   Reads UTF-8 character of sequence length 3 starting at given position.
     *          At return, the value 'atPos' will continue the position in string buffer
     *          of next UTF character. If reached to end and cannot read character anymore
     *          the value of 'atPos' is NEUtfString::InvalidPos.
     * \param   utfString   The source of UTF String to read character
     * \param   atPos       The position to start reading UTF character.
     *                      It should be valid position in buffer. Use StartPos to start reading first character.
     *                      On return, the value is the position of next UTF character to read.
     *                      If reached to end or cannot read character, on return the value is NEUtfString::InvalidPos.
     * \return  Returns value as UTF-32 character if succeeded to read.
     *          Otherwise, the return value is Invalid character.
     **/
    utf32 getSequenceThree( const sUtf8String & utfString, CharPos & atPos );

    /**
     * \brief   Sets character at given position of sequence length 3.
     * \param   utfString   The UTF-8 String structure, which contains string buffer.
     * \param   ch          The character to set
     * \param   atPos       The position to set character.
     * \return  Returns position to next character to set in UTF-8 buffer
     **/
    CharPos setSequenceThree( sUtf8String & utfString, utf32 ch, CharPos atPos );

    /**
     * \brief   Reads UTF-8 character of sequence length 4 starting at given position.
     *          At return, the value 'atPos' will continue the position in string buffer
     *          of next UTF character. If reached to end and cannot read character anymore
     *          the value of 'atPos' is NEUtfString::InvalidPos.
     * \param   utfString   The source of UTF String to read character
     * \param   atPos       The position to start reading UTF character.
     *                      It should be valid position in buffer. Use StartPos to start reading first character.
     *                      On return, the value is the position of next UTF character to read.
     *                      If reached to end or cannot read character, on return the value is NEUtfString::InvalidPos.
     * \return  Returns value as UTF-32 character if succeeded to read.
     *          Otherwise, the return value is Invalid character.
     **/
    utf32 getSequenceFour(  const sUtf8String & utfString, CharPos & atPos );

    /**
     * \brief   Sets character at given position of sequence length 3.
     * \param   utfString   The UTF-8 String structure, which contains string buffer.
     * \param   ch          The character to set
     * \param   atPos       The position to set character.
     * \return  Returns position to next character to set in UTF-8 buffer
     **/
    CharPos setSequenceFour( sUtf8String & utfString, utf32 ch, CharPos atPos );

    /**
     * \brief   Reads, check and validates the next UTF character at given position.
     *          The given position should be valid in buffer. Use StartPos to start reading first character.
     * \param   utfString   The source of UTF String to read character
     * \param   atPos       The position to start reading UTF character.
     *                      It should be valid position in buffer. Use StartPos to start reading first character.
     *                      On return, the value is the position of next UTF character to read.
     *                      If reached to end or cannot read character, on return the value is NEUtfString::InvalidPos.
     * \return  Returns value as UTF-32 character if succeeded to read and it is validated.
     *          Otherwise, the return value is Invalid character.
     **/
    UtfChar validateNext( const sUtf8String & utfString, CharPos & atPos );

    /**
     * \brief   Searches for invalid UTF character in UTF string starting at
     *          specified position in string buffer until end of the string.
     *          To validate complete string, start validation process from 
     *          NEUtfString::StartPos position.
     * \param   utfString   The UTF String to validate
     * \param   startPos    The position in string buffer to start validation
     *                      If value is NEUtfString::StartPos, the complete string is validated.
     * \return  Returns the position of found invalid UTF character. 
     *          If could not find invalid character, i.e. string is valid,
     *          the return value is NEUtfString::InvalidPos.
     **/
    CharPos findInvalid( const sUtf8String & utfString, CharPos startPos = StartPos );

    /**
     * \brief   Checks the existence of BOM (Byte Order Mark) at the begin of string buffer.
     *          If the buffer contains BOM, it detects and returns the appropriate value.
     *          Otherwise, it returns NEUtfString::BOM_Undefined
     *          BOM is a magic number at the begin of string indicating:
     *              - the byte order of text (or endianness)
     *              - the encoding of the text
     *              - the indication of Unicode encoding
     * \param   utfString   The UTF String structure with data to check
     * \return  Returns BOM value, if it exists at the begin of buffer.
     **/
    eBOM getStringBOM( const sUtf8String & utfString );

    /**
     * \brief   Checks the BOM (byte order mark) and returns true if the string starts
     *          with BOM. BOM is a magic number at the begin of text indicating:
     *              - the byte order of text (or endianness)
     *              - text encoding is Unicode
     **/
    bool startsWithUtf8Bom( const sUtf8String & utfString );

    /**
     * \brief   Sets specified character in UTF-8 String at given position of string buffer
     *          and returns the position in buffer of next character. If the given
     *          position is equal NEUtfString::EndPos, then the character is appended to string.
     *          The string buffer should have enough space to set character. If the buffer
     *          has not enough space, the return value is NEUtfString::InvalidPos.
     * \param   newChar     The character to set in string buffer.
     * \param   utfString   The UTF String object with buffer to set character.
     * \param   atPos       The position in string buffer to set character.
     * \return  Returns position in string buffer to set next character. If character
     *          is added at the end of string, returns the value equal to used space in string.
     **/
    CharPos setAt( utf32 newChar, sUtf8String & utfString, CharPos atPos = EndPos );

    /**
     * \brief   Appends given character at the end of UTF-8 String. If string does not have
     *          enough space to append character, the function will reallocate space then append character.
     * \param   utfString   The pointer of valid UTF-8 String structure to append character.
     * \param   newChar     The character to append at the end of string.
     * \return  Returns position in string buffer to set next character. If character
     *          is added at the end of string, returns the value equal to used space in string.
     **/
    CharPos appendChar( sUtf8String *& utfString, utf32 newChar );

    /**
     * \brief   Inserts given character at the given position of UTF-8 String. If string does not have
     *          enough space to insert character, the function will reallocate space then insert character.
     * \param   utfString   The pointer of valid UTF-8 String structure to insert character.
     * \param   newChar     The character to insert at given position of string.
     * \return  Returns position in string buffer to set next character.
     **/
    CharPos insertAt( utf32 newChar, sUtf8String *& utfString, CharPos atPos = StartPos );

    /**
     * \brief   Extracts char from UTF-8 string at given position and returns the sequence length of character.
     * \param   out_char    On return, this contains extracted character at given position
     * \param   utfString   The UTF-8 structure, which contains the string buffer.
     * \param   atPos       The position to start reading the character.
     * \return  Returns the sequence length of character extracted from given position.
     **/
    eSeqLength getAt( utf32 & out_Char, const sUtf8String & utfString, CharPos atPos );

    /**
     * \brief   Extracts character from UTF-8 string at the given position and returns
     *          the next position of next character.
     * \param   utfString   The UTF-8 structure, which contains the string buffer.
     * \param   startPos    The position to start reading the character.
     * \return  Returns character at given position.
     **/
    utf32 getNext( const sUtf8String & utfString, CharPos & startPos );

    /**
     * \brief   Searches the position of previous character, extracts character 
     *          from UTF-8 string at previous position and returns
     *          the previous position.
     * \param   utfString   The UTF-8 structure, which contains the string buffer.
     * \param   startPos    The position to start searching previous character.
     * \return  Returns character at previous position.
     **/
    utf32 getPrev( const sUtf8String & utfString, CharPos & startPos );

    /**
     * \brief   Skips the number of symbols starting at given position.
     * \param   utfString   The UTF-8 structure, which contains the string buffer.
     * \param   startPos    The starting position to skip characters. 
     *                      On returns, this parameter contains index of position
     *                      calculated after skipping symbols
     * \return  Returns number of characters skipped
     **/
    void skipChars( const sUtf8String & utfString, CharPos & startPos, CharCount charCount );

    /**
     * \brief   Counts number of symbols starting at given position until the given end position.
     *          if 'startPos' is NEUtfString::StartPos, it will count number of character from begining.
     *          If 'endPos' is NEUtfString::EndPos, it will count number of characters until the end.
     * \return  Returns number symbols starting at 'startPos' until given 'endPos'.
     **/
    CharCount charCount(const sUtf8String & utfString, CharPos startPos = StartPos, CharPos endPos = EndPos);

    /**
     * \brief   Converts given UTF-16 string to UTF-8. The conversion starts at characters from
     *          position at 'startPos' and converts 'charCount' symbols. If 'charCount' is equal to
     *          NEUtfString::EndPos, it converts until end of string. The converted characters are added
     *          at the end of destination UTF-8 string.
     * \param   utf8Destination     The pointer to destination UTF-8 string to copy converted characters.
     *                              The function increases the size of UTF-8 string if there is a need to increase space.
     *                              If the pointer is invalid, the function returns NEUtfString::InvalidPos value.
     * \param   utf16Source         The instance of source UTF-16 string to extract data.
     * \param   startPos            The starting position in UTF-16 string buffer to get characters.
     * \param   endPos              The number of symbols to convert. If equal to NEUtfString::EndPos,
     *                              if converts until end of string.
     * \return  The number of characters copied to destination. If the destination UTF-8 string is NULL,
     *          the return value is MNFUtfString::InvalidPos.
     **/
    CharCount convertUtf16ToUtf8( sUtf8String *& utf8Destination, const sUtf16String &utf16Source, CharPos startPos = StartPos, CharCount charCount = EndPos );

    /**
     * \brief   Converts given UTF-8 string to UTF-16. The conversion starts at characters from
     *          position at 'startPos' and converts 'charCount' symbols. If 'charCount' is equal to
     *          NEUtfString::EndPos, it converts until end of string. The converted characters are added
     *          at the end of destination UTF-16 string.
     * \param   utf16Destination    The pointer to destination UTF-16 string to copy converted characters.
     *                              The function increases the size of UTF-16 string if there is a need to increase space.
     *                              If the pointer is invalid, the function returns NEUtfString::InvalidPos value.
     * \param   utf8Source          The instance of source UTF-8 string to extract data.
     * \param   startPos            The starting position in UTF-8 string buffer to get characters.
     * \param   endPos              The number of symbols to convert. If equal to NEUtfString::EndPos,
     *                              if converts until end of string.
     * \return  The number of characters copied to destination. If the destination UTF-16 string is NULL,
     *          the return value is MNFUtfString::InvalidPos.
     **/
    CharCount convertUtf8ToUtf16( sUtf16String *& utf16Destination, const sUtf8String &utf8Source, CharPos startPos = StartPos, CharCount charCount = EndPos );

    /**
     * \brief   Converts given UTF-32 string to UTF-8. The conversion starts at characters from
     *          position at 'startPos' and converts 'charCount' symbols. If 'charCount' is equal to
     *          NEUtfString::EndPos, it converts until end of string. The converted characters are added
     *          at the end of destination UTF-8 string.
     * \param   utf8Destination     The pointer to destination UTF-8 string to copy converted characters.
     *                              The function increases the size of UTF-8 string if there is a need to increase space.
     *                              If the pointer is invalid, the function returns NEUtfString::InvalidPos value.
     * \param   utf32Source         The instance of source UTF-32 string to extract data.
     * \param   startPos            The starting position in UTF-32 string buffer to get characters.
     * \param   endPos              The number of symbols to convert. If equal to NEUtfString::EndPos,
     *                              if converts until end of string.
     * \return  The number of characters copied to destination. If the destination UTF-8 string is NULL,
     *          the return value is MNFUtfString::InvalidPos.
     **/
    CharCount convertUtf32ToUtf8( sUtf8String *& utf8Destination, const sUtf32String &utf32Source, CharPos startPos = StartPos, CharCount charCount = EndPos );

    /**
     * \brief   Converts given UTF-8 string to UTF-32. The conversion starts at characters from
     *          position at 'startPos' and converts 'charCount' symbols. If 'charCount' is equal to
     *          NEUtfString::EndPos, it converts until end of string. The converted characters are added
     *          at the end of destination UTF-32 string.
     * \param   utf32Destination    The pointer to destination UTF-32 string to copy converted characters.
     *                              The function increases the size of UTF-32 string if there is a need to increase space.
     *                              If the pointer is invalid, the function returns NEUtfString::InvalidPos value.
     * \param   utf8Source          The instance of source UTF-8 string to extract data.
     * \param   startPos            The starting position in UTF-8 string buffer to get characters.
     * \param   endPos              The number of symbols to convert. If equal to NEUtfString::EndPos,
     *                              if converts until end of string.
     * \return  The number of characters copied to destination. If the destination UTF-32 string is NULL,
     *          the return value is MNFUtfString::InvalidPos.
     **/
    CharCount convertUtf8ToUtf32( sUtf32String *& utf32Destination, const sUtf8String &utf8Source, CharPos startPos = StartPos, CharCount charCount = EndPos );

}

template<typename CharType>
inline NEUtfString::eEncoding NEUtfString::getEncoding( void )
{
    return NEUtfString::EncodeAscii;
}

template<>
inline NEUtfString::eEncoding NEUtfString::getEncoding<NEUtfString::Char>( void )
{
    return NEUtfString::EncodeAscii;
}

template<>
inline NEUtfString::eEncoding NEUtfString::getEncoding<NEUtfString::utf8>( void )
{
    return NEUtfString::EncodeUtf8;
}

template<>
inline NEUtfString::eEncoding NEUtfString::getEncoding<NEUtfString::utf16>( void )
{
    return NEUtfString::EncodeUtf16;
}

template<>
inline NEUtfString::eEncoding NEUtfString::getEncoding<NEUtfString::utf32>( void )
{
    return NEUtfString::EncodeUtf32;
}

inline unsigned int NEUtfString::minBytesPerChar( NEUtfString::eEncoding encode )
{
    switch ( encode )
    {
    case NEUtfString::EncodeAscii:
        return sizeof( Char );
    case NEUtfString::EncodeUtf8:
        return sizeof( utf8 );
    case NEUtfString::EncodeUtf16:
        return sizeof( utf16 );
    case NEUtfString::EncodeUtf32:
        return sizeof( utf32 );
    default:
        return 0;
    }
}

inline unsigned int NEUtfString::getSequenceSpace( NEUtfString::eSeqLength seqLength )
{
    return static_cast<unsigned int>(seqLength);
}

inline NEUtfString::eEncoding NEUtfString::bom2Encode( NEUtfString::eBOM bom )
{
    switch ( bom )
    {
    case NEUtfString::BOM_UTF8:
        return NEUtfString::EncodeUtf8;
    
    case NEUtfString::BOM_UTF16_BE:    // fall through
    case NEUtfString::BOM_UTF16_LE:
        return NEUtfString::EncodeUtf16;
    
    case NEUtfString::BOM_UTF32_BE:    // fall through
    case NEUtfString::BOM_UTF32_LE:
        return NEUtfString::EncodeUtf32;
    
    case NEUtfString::BOM_Undefined:
        return NEUtfString::EncodeAscii;

    default:
        return NEUtfString::EncodeInvalid;
    }
}

template<typename CharType>
inline const CharType * NEUtfString::getString( const NEUtfString::SUtfString<CharType> & utfString )
{
    return utfString.utfBuffer;
}

template<typename CharType>
inline CharType * NEUtfString::getString( NEUtfString::SUtfString<CharType> & utfString )
{
    return utfString.utfBuffer;
}

template<typename CharType>
inline CharType NEUtfString::charAtPos( const NEUtfString::SUtfString<CharType> & utfString, NEUtfString::CharPos pos )
{
    return( utfString.utfHeader.utfUsed > pos ? utfString.utfBuffer[pos] : static_cast<CharType>(getInvalidChar().u32Bits) );
}

template<typename CharType>
inline NEUtfString::utf8 NEUtfString::mask8( CharType ch )
{
    return static_cast<CharType>(0xFF & ch);
}

template<typename CharType>
inline NEUtfString::utf16 NEUtfString::mask16( CharType ch )
{
    return static_cast<CharType>(0xFFFF & ch);
}

template<typename CharType>
inline bool NEUtfString::isTrail( CharType ch )
{
    return ((NEUtfString::mask8<CharType>(ch) >> 6) == 0x02u);
}

template<typename CharType>
inline NEUtfString::eSeqLength NEUtfString::getSequenceLength( CharType ch )
{
    NEUtfString::utf8 lead = NEUtfString::mask8<CharType>(ch);
    if ( lead < 0x80 )
        return NEUtfString::SeqOne;
    else if ( (lead >> 5) == 0x06 )
        return NEUtfString::SeqTwo;
    else if ( (lead >> 4) == 0x0E )
        return NEUtfString::SeqThree;
    else if ( (lead >> 3) == 0x1E )
        return NEUtfString::SeqFour;
    return NEUtfString::SeqInvalid;
}

template<typename CharType>
inline bool NEUtfString::isSequenceValid( CharType ch, NEUtfString::eSeqLength seqLen )
{
    if ( ch < 0x80 )
        return (seqLen == NEUtfString::SeqOne);
    else if ( ch < 0x800 )
        return (seqLen == NEUtfString::SeqTwo);
    else if ( ch < 0x10000 )
        return (seqLen == NEUtfString::SeqThree);
    return true;
}

template<typename CharType>
inline bool NEUtfString::isLeadSurrogate( CharType ch )
{
    return ((NEUtfString::LEAD_SURROGATE_MIN <= ch) && (NEUtfString::LEAD_SURROGATE_MAX >= ch));
}

template<typename CharType>
inline bool NEUtfString::isTrailSurrogate( CharType ch )
{
    return ((NEUtfString::TRAIL_SURROGATE_MIN <= ch) && (NEUtfString::TRAIL_SURROGATE_MAX >= ch));
}

template<typename CharType>
inline bool NEUtfString::isSurrogate( CharType ch )
{
    return ((NEUtfString::LEAD_SURROGATE_MIN <= ch) && (NEUtfString::TRAIL_SURROGATE_MAX >= ch));
}

template<typename CharType>
inline bool NEUtfString::isCodePointValid( CharType ch )
{
    return ((ch <= NEUtfString::CODE_POINT_MAX) && (NEUtfString::isSurrogate( ch ) == false));
}

template<typename CharType>
inline bool NEUtfString::isValid( CharType ch )
{
    return (ch != static_cast<CharType>(getInvalidChar().u32Bits));
}

template<typename CharType>
inline bool NEUtfString::isValidString( const NEUtfString::SUtfString<CharType> & utfString, NEUtfString::CharPos startPos /*= StartPos*/ )
{
    return (findInvalid(utfString, startPos) == utfString.utfHeader.utfUsed);
}

template<typename CharType>
inline bool NEUtfString::canRead( const NEUtfString::SUtfString<CharType> & utfString, NEUtfString::CharPos atPos, NEUtfString::eSeqLength seqLen )
{
    return (seqLen != NEUtfString::SeqInvalid ? utfString.utfHeader.utfUsed >= (atPos + static_cast<CharPos>(seqLen)) : false);
}

template<typename CharType>
inline bool NEUtfString::canWrite( const NEUtfString::SUtfString<CharType> & utfString, unsigned int atPos, NEUtfString::eSeqLength seqLen )
{
    return (seqLen != NEUtfString::SeqInvalid ? utfString.utfHeader.utfSpace > (atPos + static_cast<CharPos>(seqLen)) : false);
}

template<typename CharType>
inline void NEUtfString::releaseString( NEUtfString::SUtfString<CharType> & utfString )
{
    delete [] reinterpret_cast<unsigned char *>(&utfString);
}

template<typename CharType>
NEUtfString::SUtfString<CharType> * NEUtfString::reserveSpace(  const NEUtfString::SUtfString<CharType> & utfString
                                                        , NEUtfString::CharCount charCount
                                                        , unsigned int blockSize /*= DEFAULT_BLOCK_SIZE*/ )
{
    NEUtfString::SUtfString<CharType> * result = NULL;
    if ( charCount >= utfString.utfHeader.utfSpace )
    {
        NEUtfString::sUtfStringHeader * hdr = initString( utfString.utfHeader.utfEncode, charCount, blockSize );
        if ( hdr != NULL )
        {
            hdr->utfUsed    = utfString.utfHeader.utfUsed;
            hdr->utfCount   = utfString.utfHeader.utfCount;
            result          = reinterpret_cast<NEUtfString::SUtfString<CharType> *>(hdr);
            NEMemory::CopyElems<CharType, CharType>( result->utfBuffer, utfString.utfBuffer, utfString.utfHeader.utfUsed );
        }
    }
    return result;
}

#endif  // AREG_BASE_NEUTFSTRING_HPP
