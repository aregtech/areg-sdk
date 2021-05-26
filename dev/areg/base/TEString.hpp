#ifndef AREG_BASE_TESTRING_HPP
#define AREG_BASE_TESTRING_HPP
/************************************************************************
 * \file        areg/base/TEString.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, String Class. Supports ASCII, UTF-8, UTF-16 and UTF-32
 *              string encodings.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/NEString.hpp"
#include "areg/base/ETemplateBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// TEString class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Class template of string.
 *          The basic string class, which contains basic operations 
 *          to set, append, insert, search and replace string, as well to get substring.
 *          When deriving class, should be paid special attention on string structure
 *          validation. The string should be allocated and released via methods 
 *          defined in NEString namespace.
 *          The string class template constructors and destructor are protected.
 *          To be able to instantiate string, the class should be derived to define
 *          basic operations.
 * 
 * \tparam  CharType    The type of character. Normally, either 8-bit character or 16-bit wide character.
 * \tparam  Implement   The name of class where the basic string alphabetic, comparing and 
 *                      stream read/write operations are implemented.
 **/
template<typename CharType, class Implement = TEStringImpl<CharType>>
class TEString  : protected Implement
{
//////////////////////////////////////////////////////////////////////////
// Public constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empty character constant
     **/
    static const CharType   EmptyChar       /*= static_cast<CharType>('\0')*/;

    /**
     * \brief   New line constant
     **/
    static const CharType   NewLine         /*= static_cast<CharType>('\n')*/;

    /**
     * \brief   DOS format new line
     **/
    static const CharType   NewLineDos[2]   /*= {static_cast<CharType>('\r'), static_cast<CharType>('\n')}*/;

//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Sets the encoding
     * \param   encode      The initialized encoding of string.
     **/
    inline TEString( NEString::eEncoding encode );

    /**
     * \brief   Sets the encoding, converts char to string.
     * \param   chSource    The character to set at the begin of string.
     * \param   encode      The initialized encoding of string.
     **/
    inline TEString( CharType chSource, NEString::eEncoding encode );

    /**
     * \brief   Sets encoding, copies string from given source.
     * \param   strSource   The initial string to initialize. If NULL, invalid string is created.
     * \param   encode      The initialized encoding of string.
     **/
    inline TEString( const CharType * strSource, NEString::eEncoding encode );

    /**
     * \brief   Sets encoding, copies specified amount of symbols from given string.
     * \param   strSource   The null-terminated string buffer to initialize. If NULL, invalid string is created.
     * \param   charCount   The number of character to initialize from given string source. If zero, invalid string is created.
     * \param   encode      The initialized encoding of string.
     **/
    inline TEString( const CharType * strSource, NEString::CharCount charCount, NEString::eEncoding encode );

    /**
     * \brief   Sets encoding, copies specified amount of symbols from given string structure.
     * \param   strSource   The string structure as source to initialize.
     * \param   encode      The initialized encoding of string.
     **/
    inline TEString( const NEString::SString<CharType> & strSource, NEString::eEncoding encode );

    /**
     * \brief   Copy constructor.
     * \param   strSource   The source to copy data.
     **/
    inline TEString( const TEString<CharType, Implement> & strSource );

    /**
     * \brief   Destructor.
     **/
    ~TEString( void );

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Find the first occurrence of any of the characters in string buffer 'chars'. 
     *          The search starts at given 'startPos'. If any character is found, returns valid position.
     *          Otherwise, returns NEString::InvalidPos.
     * \param   chars       The one or more characters as set in null-terminated string to search.
     * \param   startPos    The start position in string to search. Should be valid.
     *                      By default, starts to search at begin of string, i.e. at 'NEString::StartPos'
     * \return  Returns valid string position value, if any occurrence of given characters found.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findOneOf( const CharType * chars, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::InvalidPos value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::StartPos'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirstOf( CharType chSearch, NEString::CharPos startPos = NEString::StartPos, bool caseSensitive = true ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::InvalidPos value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::StartPos'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should match the whole word.
     *                          Otherwise, the match is by any match. By default, the search is by any phrase.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirstOf( const CharType * phrase, NEString::CharPos startPos = NEString::StartPos, bool caseSensitive = true, bool wholeWord = false ) const;
    
    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::InvalidPos value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::EndPos'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLastOf( CharType chSearch, NEString::CharPos startPos = NEString::EndPos, bool caseSensitive = true ) const;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::InvalidPos value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::EndPos'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLastOf( const CharType * phrase, NEString::CharPos startPos = NEString::EndPos, bool caseSensitive = true ) const;

    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     * \param   strOther        The given other string to compare
     * \param   startAt         The position of string to start comparing. 
     *                          By default, it compares at the begin of string.
     * \param   charCount       The number of characters to compare.
     *                          By default, it compares until end of string
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     **/
    inline NEMath::eCompare compare( const CharType * strOther, NEString::CharPos startAt = NEString::StartPos, NEString::CharCount charCount = NEString::CountAll, bool caseSensitive = true) const;
    
    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     * \param   other           The string to compare with.
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     **/
    inline NEMath::eCompare compare(const TEString<CharType, Implement> & other, bool caseSensitive = true ) const;

    /**
     * \brief   Extracts the substring starting at startPos and given number of characters.
     * \param   outResult   On output, this parameter contains resulting substring.
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::StartPos
     * \param   charCount   The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     * \return  Returns true if could create substring. Otherwise, returns false.
     *          The substring can fail if method is called for invalid string, if given position is invalid.
     **/
    bool substring( TEString<CharType, Implement> & outResult, NEString::CharPos startPos = NEString::StartPos, NEString::CharCount charCount = NEString::CountAll ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after phrase. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   strPhrase   The phrase to search in the string.
     * \param   startPos    The starting position to search the string.
     * \return  Returns next position after searched phrase and value are followings:
     *              -   Valid string position not equal to NEString::EndPos,
     *                  if found phrase and the phrase is not at the end;
     *              -   NEString::EndPos if found the phrase at end of string;
     *              -   NEString::InvalidPos if could not find the phrase.
     *
     * \example     Example of Substr()
     * \code [.cpp]
     *
     *  String test("0123 456 789 0123");
     *  String result;
     *  NEString::CharPos next = NEString::StartPos;
     *  next = test.Substr(result, "0123", next);   // results: next == 4, result == ""
     *  next = test.Substr(result, "0123", next);   // results: next == NEString::EndPos, result == " 456 789 "
     *
     *  next = NEString::StartPos;
     *  next = test.Substr(result, " ", next);      // results: next == 5, result == "0123"
     *  next = test.Substr(result, " ", next);      // results: next == 9, result == "456"
     *  next = test.Substr(result, " ", next);      // results: next == NEString::InvalidPos, result == "0123"
     * \endcode
     **/
    NEString::CharPos substring( TEString<CharType, Implement> & outResult, const CharType * strPhrase, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after symbol. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   chSymbol    The symbol to search in the string.
     * \param   startPos    The starting position to search the symbol.
     * \return  Returns next position after searched symbol and value are followings:
     *              -   Valid string position not equal to NEString::EndPos,
     *                  if found phrase and the symbol is not at the end;
     *              -   NEString::EndPos if found the symbol at end of string;
     *              -   NEString::InvalidPos if could not find the phrase.
     **/
    NEString::CharPos substring( TEString<CharType, Implement> & outResult, CharType chSymbol, NEString::CharPos startPos = NEString::StartPos ) const;

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string is Invalid, it will re-initialize string and copy characters. 
     *          If string has not enough space to copy characters, it will reallocate the space.
     *          The encoding remains unchanged.
     * \param   strSource   The source of string to copy characters.
     * \param   charCount   The number of characters to copy.
     *                      By default, it copies all characters.
     * \return  Returns number of copied characters. If zero, nothing was copied.
     **/
    NEString::CharCount copy( const CharType * strSource, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Appends given amount of characters of given string at the end and returns the amount of appended characters.
     *          If string is Invalid, it will re-initialize string and copy characters at the begin.
     *          If string has not enough space to append characters, it will reallocate the space.
     * \param   strSource   The source of string to append characters.
     * \param   charCount   The number of characters to append.
     *                      By default, it copies all characters.
     * \return  Returns number of append characters. If zero, nothing was added.
     **/
    NEString::CharCount append( const CharType * strSource, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Inserts given source of character at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string is Invalid, it will re-initialize string and insert character at specified position.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   chSource    The source of character to insert.
     * \param   atPos       The position in the string to insert.
     **/
    void insertAt( CharType chSource, NEString::CharPos atPos );

    /**
     * \brief   Inserts given source of string at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string is Invalid, it will re-initialize string and insert character at specified position.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   strSource   The source of character to insert.
     * \param   atPos       The position in the string to insert.
     * \param   charCount   The number of characters in string to insert.
     *                      By default it inserts complete source of string.
     **/
    void insertAt( const CharType * strSource, NEString::CharPos atPos, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Inserts given source of string at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string is Invalid, it will re-initialize string and insert character at specified position.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   strSource   The source of character to insert.
     * \param   atPos       The position in the string to insert.
     * \param   charCount   The number of characters in string to insert.
     *                      By default it inserts complete source of string.
     **/
    void insertAt( const TEString<CharType, Implement> & strSrc, NEString::CharPos atPos, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Searches and replaces given character by another given. The search and replacement starts at given position.
     *          The method either replaces first match of character and returns next position,
     *          or replaces all matches of given characters and returns end-of-string position (NEString::EndPos).
     * \param   chOrigin    The character to be searched to replace.
     * \param   chReplace   The character, which should be set
     * \param   startPos    The position to start searching character to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matched characters in the string and returns End Position (NEString::EndPos).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'chOrigin' character and replaced, where 'replaceAll' parameter is false.
     *          - NEString::EndPos if replaced character was at the end of string or could replace at least once, where 'replaceAll' is true.
     *          - NEString::InvalidPos if could not find and replace any character.
     **/
    NEString::CharPos replace( CharType chOrigin, CharType chReplace, NEString::CharPos startPos = NEString::StartPos, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces first match of string and returns next position,
     *          or replaces all matches of given string and returns end-of-string position (NEString::EndPos).
     * \param   strOrigin   The string to be searched to replace.
     * \param   strReplace  The string to set
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches of string and returns End Position (NEString::EndPos).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'strOrigin' string and replaced, where 'replaceAll' parameter is false.
     *          - NEString::EndPos if replaced string was at the end of data or could replace at least once, where 'replaceAll' is true.
     *          - NEString::InvalidPos if could not find and replace any match of string.
     **/
    NEString::CharPos replace( const CharType * strOrigin, const CharType * strReplace, NEString::CharPos startPos = NEString::StartPos, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces first match of string and returns next position,
     *          or replaces all matches of given string and returns end-of-string position (NEString::EndPos).
     * \param   strOrigin   The string to be searched to replace.
     * \param   strReplace  The string to set
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches of string and returns End Position (NEString::EndPos).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'strOrigin' string and replaced, where 'replaceAll' parameter is false.
     *          - NEString::EndPos if replaced string was at the end of data or could replace at least once, where 'replaceAll' is true.
     *          - NEString::InvalidPos if could not find and replace any match of string.
     **/
    NEString::CharPos replace( const TEString<CharType, Implement> & strOrigin, const TEString<CharType, Implement> & strReplace, NEString::CharPos startPos = NEString::StartPos, bool replaceAll = true );

    /**
     * \brief   Replaces specified amount of characters starting at given position by given string.
     *          Returns the next position after replacing string.
     * \param   strReplace  The string to set.
     * \param   startPos    The position to start setting string.
     * \param   charsRemove Amount of characters to replace in string.
     * \return  Returns next position after replacing string. If specified position in invalid
     (          returns NEString::InvalidPos. If string was inserted at the end, returns NEString::EndPos.
     **/
    NEString::CharPos replace( const CharType * strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove );

    /**
     * \brief   Replaces specified amount of characters starting at given position by given string.
     *          Returns the next position after replacing string.
     * \param   strReplace  The string to set.
     * \param   startPos    The position to start setting string.
     * \param   charsRemove Amount of characters to replace in string.
     * \return  Returns next position after replacing string. If specified position in invalid
     (          returns NEString::InvalidPos. If string was inserted at the end, returns NEString::EndPos.
     **/
    NEString::CharPos replace( const TEString<CharType, Implement> & strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove );

    /**
     * \brief   Searches and removes given phrase of string. The search starts at given position.
     *          The method either removes only first match of string and returns next position,
     *          or removes all matches of given string and returns end-of-string position (NEString::EndPos).
     * \param   strRemove   The string to search and remove.
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   removeAll   If true, removes all matches of string and returns End Position (NEString::EndPos).
     *                      Otherwise, removes only first matched.
     * \return  Returns:
     *          - valid next position value if found 'strRemove' string phrase and removed, where 'removeAll' parameter is false.
     *          - NEString::EndPos if removed string was at the end of data or could remove at least once, where 'removeAll' is true.
     *          - NEString::InvalidPos if could not find any match of string.
     **/
    NEString::CharPos remove( const CharType * strRemove, NEString::CharPos startPos = NEString::StartPos, bool removeAll = true );

    /**
     * \brief   Removes specified amount of characters in string at specified starting position.
     *          If 'charCount' is NEString::CountAll, it will remove all until end, i.e. sets
     *          end-of-string character at specified position.
     * \param   startPos    The starting position to remove string. Operation is ignored if not valid position.
     * \param   charCount   The amount of characters to remove. If 'NEString::CountAll' removes all until end of string,
     *                      i.e. sets end-of-string value at specified position.
     **/
    inline void remove( NEString::CharPos startPos, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Moves string inside string buffer starting at specified position.
     *          It moves left if 'moveTo' value is negative and moves right if 'moveTo' is positive.
     *          If 'moveTo' is zero, no change happens. The method is reallocating new space if 
     *          string requires more space to move right.
     * \param   startPos    The position to start moving string. The specified position should be valid for reading.
     * \param   moveTo      Specifies number of characters and direction to move string.
     *                      If negative, moves string to left side. String buffer is not reallocated.
     *                      If positive, moves string to right. And if needed, reallocates string buffer.
     *                      if zero, no change happens.
     * \return  Returns amount of characters actually moved. Normally equal to absolute value of 'moveTo'
     **/
    NEString::CharCount move( NEString::CharPos startPos, int moveTo );

    /**
     * \brief   Swaps characters in string buffer starting at given position and given number of characters.
     *          By default, it swaps all characters in the string buffer.
     * \param   startPos    The starting position in string buffer to swap.
     *                      By default, it starts at the begin of string.
     * \param   charCount   The amount of characters to swap.
     *                      By default, swaps all characters until end of string.
     **/
    void swap( NEString::CharPos startPos = NEString::StartPos, NEString::CharCount charCount = NEString::CountAll );

    /**
     * \brief   Truncates the string to maximum allowed length.
     *          Nothing happens if length of string is less than the specified maximum.
     * \param   maxLength   The maximum length of string.
     **/
    inline void truncate( NEString::CharCount maxChars );

    /**
     * \brief   Clears the string, makes it empty and invalid.
     **/
    inline void clear( void );

    /**
     * \brief   Compacts the string. If required buffer is less than actually allocated, new buffer is initialized and
     *          existing string is copied
     **/
    inline void compact( void );

    /**
     * \brief   Checks whether the string is numeric. Checks only integer numbers.
     * \param   signIgnore      If true, ignores '-' and '+' signs in front of number
     * \return  Returns true if characters are between '0' and '9', or optional '+' or '-' in front if
     *          signs should be ignored.
     **/
    inline bool isNumeric( bool signIgnore = true ) const;

    /**
     * \brief   Returns true if string is contains only letters and numbers. No symbols or other controls are allowed.
     **/
    inline bool isAlphanumeric( void ) const;

    /**
     * \brief   Returns true if string is valid for name.
     *          The name is valid if it contains letters, numbers and following symbols {'-', '_', '.'}.
     *          All other symbols are rejected.
     **/
    inline bool isValidName( void ) const;

    /**
     * \brief   Checks validity of the string. The valid characters are listed in the passed list, which is null-terminated string.
     * \param   validityList    Null-terminated string of valid characters to check the string.
     * \return  Returns true, if string does not contain any character different in validity list.
     *          Otherwise, it returns false
     **/
    inline bool validate( const CharType * validityList ) const;

    /**
     * \brief   Returns true if string is empty
     **/
    inline bool isEmpty( void ) const;

    /**
     * \brief   Returns true if string valid, i.e. it is allocated and not equal to NEString::BadString
     **/
    inline bool isValid( void ) const;

    /**
     * \brief   Returns the length of string in number of characters without null-terminated character at the and of the string.
     **/
    inline NEString::CharCount getLength( void ) const;

    /**
     * \brief   Returns the actual length of allocated buffer for string, which is bigger than the length of string.
     *          The returned value is number of characters that can be written in the string without resizing.
     **/
    inline NEString::CharCount getActualLength( void ) const;

    /**
     * \brief   Returns used size of buffer in bytes, including null-terminated character of string.
     **/
    inline unsigned int getUsedSpace( void ) const;

    /**
     * \brief   Returns read-only string buffer starting at specified valid position.
     * \param   startAt     The position in string to return read-only buffer.
     *                      By default it returns begin of string buffer.
     * \return  Returns valid pointer of read-only string buffer if specified position
     *          is valid for read. Otherwise, it returns NULL
     **/
    inline const CharType * getBuffer( NEString::CharPos startAt = NEString::StartPos ) const;

    /**
     * \brief   Returns null-terminated buffer of string or NULL pointer if string is invalid.
     **/
    inline const CharType * getString( void ) const;

    /**
     * \brief   Returns the encoding of string
     **/
    inline NEString::eEncoding getEncoding( void ) const;

    /**
     * \brief   Returns character at specified string position.
     * \param   atPos   The position to return character value.
     * \return  Returns character at specified position of string buffer.
     *          Returns NEString::EndOfString if position is invalid or
     *          equal to string length.
     **/
    inline CharType getAt( NEString::CharPos atPos ) const;

    /**
     * \brief   Sets character at specified position.
     * \param   ch      The character to set.
     * \param   atPos   The position where to set character.
     *                  The operation is ignored if position is more than allocated buffer size.
     *                  If position is more or equal to length of string, it sets character
     *                  at specified position and increase string length.
     **/
    inline void setAt( CharType ch, NEString::CharPos atPos = NEString::EndPos );

    /**
     * \brief   Makes sure to allocate string buffer with specified space.
     *          The operation is ignored if the existing space is more or equal
     *          requested size to allocate.
     * \param   maxSpace    The size of buffer to allocate.
     * \return  Returns true if string buffer size is not less than requested 'maxSpace'
     **/
    inline bool resize( NEString::CharCount maxSpace );

    /**
     * \brief   Removes whitespace characters from left side, i.e. from begin of string
     *          and returns number of characters removed.
     **/
    inline int trimLeft( void );

    /**
     * \brief   Removes whitespace characters from left side, i.e. from begin of string,
     *          copy the result in given destination string object and returns number of characters removed.
     *          The method does not modify existing string.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters removed from existing string.
     **/
    inline int trimLeft( TEString<CharType, Implement> & strResult ) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from end of string
     *          and returns number of characters removed.
     **/
    inline int trimRight( void );

    /**
     * \brief   Removes whitespace characters from right side, i.e. from end of string,
     *          copy the result in given destination string object and returns number of characters removed.
     *          The method does not modify existing string.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters removed from existing string.
     **/
    inline int trimRight( TEString<CharType, Implement> & strResult ) const;

    /**
     * \brief   Removes whitespace characters from left and right sides, i.e. from begin and end of string
     *          and returns number of characters removed.
     **/
    inline int trimAll( void );

    /**
     * \brief   Removes whitespace characters from left and right sides, i.e. from begin and end of string,
     *          copy the result in given destination string object and returns number of characters removed.
     *          The method does not modify existing string.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters removed from existing string.
     **/
    inline int trimAll( TEString<CharType, Implement> & strResult ) const;

    /**
     * \brief   In existing string removes all characters, which are not alphanumeric.
     *          Returns zero if string is not valid or all characters are removed.
     * \return  Returns the length of string after making alphanumeric
     **/
    inline NEString::CharCount makeAlphanumeric( void );

    /**
     * \brief   Returns unsafe buffer of string. The method exists for converting purpose
     *          rather than to provide business logic. Any direct modifications of buffer may 
     *          destabilize logic of object.
     **/
    inline CharType *  getUnsafeBuffer( void ) ;

/************************************************************************/
// Methods that can be overwritten in implementation class template
/************************************************************************/
protected:

    /**
     * \brief   Makes the given character lower case.
     *          Override method to convert non ASCII characters
     *          By default, the conversion is done based on ISO8859
     * \param   ch      The character to convert to lower case
     * \return  Returns lower case character
     **/
    inline CharType makeLower( CharType ch ) const;

    /**
     * \brief   Makes the given character upper case 
     *          Override method to convert non ASCII characters
     *          By default, the conversion is done based on ISO8859
     * \param   ch      The character to convert to upper case
     * \return  Returns upper case character
     **/
    inline CharType makeUpper( CharType ch ) const;

    /**
     * \brief   Returns true if given character is a letter.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool isLetter( CharType ch ) const;

    /**
     * \brief   Returns true if given character is a number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool isNumeric( CharType ch ) const;

    /**
     * \brief   Returns true if given character is a letter or number.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool isAlphanumeric( CharType ch ) const;

    /**
     * \brief   Returns true if given character is a symbol.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool isSymbol( CharType ch ) const;

    /**
     * \brief   Returns true if given character is a white-space.
     *          By default, the checkup is done based on ISO8859
     * \param   ch      The character to check
     **/
    inline bool isWhitespace( CharType ch ) const;

    /**
     * \brief   Compares the existing string with given string and returns:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     *          The comparing starts from given position, compares given
     *          amount of character of until the end if 'charCount' is NEString::CountAll,
     *          and the comparing is either by exact match or ignores cases sensitive.
     * \param   startPos        The position in string buffer to start comparing.
     *                          If equal to NEString::StartPos, compares from beginning of string.
     * \param   strOther        The given string to compare
     * \param   charCount       The amount of characters to compare.
     *                          If equal to NEString::CountAll compares until end of string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, compares lower and upper cases.
     * \return  Return:
     *              NEMath::CompSmaller if string is less than given string
     *              NEMath::CompEqual   if strings have equal
     *              NEMath::CompGreater if string is more than given string
     **/
    inline NEMath::eCompare compareString( NEString::CharPos startPos, const CharType * strOther, NEString::CharCount charCount = NEString::CountAll, bool caseSensitive = true ) const;

    /**
     * \brief   Reads string data from streaming object.
     * \param   stream  The streaming object, which contains string source data
     **/
    inline void readStream(const IEInStream & stream);

    /**
     * \brief   Writes string data to streaming object.
     * \param   stream  The streaming object to write string data.
     **/
    inline void writeStream(IEOutStream & stream) const;

/************************************************************************/
// Protected methods, can be assessed only from derived class
/************************************************************************/
protected:

    /**
     * \brief   Returns string initialized encoding, which is set via constructor.
     **/
    inline NEString::eEncoding getInitEncoding( void ) const;

    /**
     * \brief   Reallocates string buffer.
     * \param   charsAdd    The amount of characters expected to add in string after reallocating buffer
     * \param   encode      The encoding to set in case if string does not have encoding.
     **/
    inline void reallocate( NEString::CharCount charsAdd, NEString::eEncoding encode);

    /**
     * \brief   Checks whether possible to read data at specified position.
     *          To read data, the position should be less than the length of string.
     * \param   atPos       The position in string buffer to read data.
     * \return  Returns true, if specified position is valid and can read data.
     *          Returns false, if string is invalid or position is not less than the length of string.
     **/
    inline bool canRead( NEString::CharPos atPos ) const;

    /**
     * \brief   Checks whether possible to write data at specified position.
     *          To write data, the position should be less than the string buffer size.
     *          This means the position for writing can be more or equal than length of string.
     * \param   atPos       The position in string buffer to write data.
     * \return  Returns true, if specified position is less than string buffer size.
     *          Returns false, if string is invalid or position is not less than string buffer size.
     **/
    inline bool canWrite( NEString::CharPos atPos ) const;

    /**
     * \brief   Release previously allocated string, free spaces and make string invalid.
     **/
    inline void release( void );

    /**
     * \brief   Return string structure object valid for writing.
     **/
    inline NEString::SString<CharType> & getDataString( void );

    /**
     * \brief   Returns string structure object valid only for reading.
     **/
    inline const NEString::SString<CharType> & getDataString( void ) const;

    /**
     * \brief   Returns the string buffer starting at specified position.
     *          The buffer is valid for writing. The specified position should be valid
     *          for reading, i.e. should be less than the string length.
     * \param   atPos       The position in string buffer to get pointer.
     * \return  Returns valid pointer of string if string is valid and specified position is valid for read.
     *          Returns invalid pointer (NULL) if string is invalid or specified position is invalid for reading.
     **/
    inline CharType * getChars( NEString::CharPos atPos );

    /**
     * \brief   Returns the string buffer starting at specified position.
     *          The buffer is valid for reading only. The specified position should be valid
     *          for reading, i.e. should be less than the string length.
     * \param   atPos       The position in string buffer to get pointer.
     * \return  Returns valid pointer of string if string is valid and specified position is valid for read.
     *          Returns invalid pointer (NULL) if string is invalid or specified position is invalid for reading.
     **/
    inline const CharType * getChars( NEString::CharPos atPos ) const;

    /**
     * \brief   Return true if string data pointer is NULL.
     **/
    inline bool isNull( void ) const;

    /**
     * \brief   Returns true if string data structure has non-NULL pointer.
     **/
    inline bool isValidPtr( void ) const;

    /**
     * \brief   Returns the pointer of string buffer starting at the end of string, where the pointer shows end-of-string.
     *          If string is invalid, returns NULL. The returned pointer is for writing.
     **/
    inline CharType * getStringEnd( void );

    /**
     * \brief   Returns the pointer of string buffer starting at the end of string, where the pointer shows end-of-string.
     *          If string is invalid, returns NULL. The returned pointer is for read only.
     **/
    inline const CharType * getStringEnd( void ) const;

    /**
     * \brief   Searches string and if found, replace by another.
     * \param   strOrigin   The string to search.
     * \param   lenOrigin   The length of string to search or amount of characters to search.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \param   startPos    The starting position to start searching.
     * \return  Returns next position after replacing string. Returns NEString::InvalidPos if could not find specified string.
     **/
    inline NEString::CharPos replaceWith( const CharType * strOrigin
                                        , NEString::CharCount lenOrigin
                                        , const CharType * strReplace
                                        , NEString::CharCount lenReplace
                                        , NEString::CharPos startPos);

    /**
     * \brief   Replaces string in string buffer starting at specified position.
     * \param   startPos    The position in string buffer to start replacing characters. Should be valid position for reading.
     * \param   charCount   The amount characters to replace is string buffer.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \return  Returns next position after replacing string. Returns NEString::InvalidPos if could not find specified string.
     **/
    inline NEString::CharPos replaceWith( NEString::CharPos startPos
                                        , NEString::CharCount charCount
                                        , const CharType * strReplace
                                        , NEString::CharCount lenReplace);

//////////////////////////////////////////////////////////////////////////
// Protected member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The pointer to string structure, which contains string buffer
     **/
    NEString::SString<CharType> *   mData;
    /**
     * \brief   The assigned encoding for string
     **/
    NEString::eEncoding             mEncode;

#ifdef DEBUG
    const CharType *                mString;
#endif // DEBUG


//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    TEString( void );   // no default constructor is allowed.
};

//////////////////////////////////////////////////////////////////////////
// Template class TEString implementation
//////////////////////////////////////////////////////////////////////////

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
const CharType  TEString<CharType, Implement>::EmptyChar        = static_cast<CharType>('\0');

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
const CharType  TEString<CharType, Implement>::NewLine          = static_cast<CharType>('\n');

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
const CharType  TEString<CharType, Implement>::NewLineDos[2]    = {static_cast<CharType>('\r'), static_cast<CharType>('\n')};

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isSymbol( CharType ch ) const
{
    return Implement::implIsSymbol(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isWhitespace( CharType ch ) const
{
    return Implement::implIsWhitespace(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isAlphanumeric( CharType ch ) const
{
    return Implement::implIsAlphanumeric(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isNumeric( CharType ch ) const
{
    return Implement::implIsNumeric(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isLetter( CharType ch ) const
{
    return Implement::implIsLetter(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Implement>::makeLower( CharType ch ) const
{
    // return ((ch >= static_cast<CharType>('A')) && (ch <= static_cast<CharType>('Z')) ? ch - 'A' + 'a' : ch);
    return Implement::implMakeLower(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Implement>::makeUpper( CharType ch ) const
{
    // return ((ch >= static_cast<CharType>('a')) && (ch <= static_cast<CharType>('z')) ? ch - 'a' + 'A' : ch);
    return Implement::implMakeUpper(ch);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Implement>::compareString(   NEString::CharPos startPos
                                                                      , const CharType * strOther
                                                                      , NEString::CharCount charCount/*= NEString::CountAll  */
                                                                      , bool caseSensitive           /*= true                */ ) const
{
    const CharType * leftSide = getChars(startPos);
    const CharType * rightSide= strOther;

    return Implement::implCompareString(leftSide, rightSide, charCount, caseSensitive);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::readStream( const IEInStream & stream )
{
    unsigned int len    = 0;
    unsigned int encode = static_cast<unsigned int>(NEString::EncodeInvalid);

    stream >> len;
    stream >> encode;

    reallocate( static_cast<NEString::CharCount>(len), static_cast<NEString::eEncoding>(encode) );
    CharType * dst  = getChars(NEString::EndPos);

    int count = Implement::implReadStream(stream, dst, len);

    mData->strUsed += count;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::writeStream( IEOutStream & stream ) const
{
    NEString::CharCount len     = getLength();
    NEString::eEncoding encode  = mData->strEncoding != NEString::EncodeInvalid ? mData->strEncoding : NEString::EncodeAscii;
    const CharType * src        = getString();
    stream << static_cast<unsigned int>(len);
    stream << static_cast<unsigned int>(encode);

    Implement::implWriteStream(stream, src);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Implement>::replaceWith(  NEString::CharPos startPos
                                                                    , NEString::CharCount charCount
                                                                    , const CharType * strReplace
                                                                    , NEString::CharCount lenReplace )
{
    NEString::CharPos nextPos = NEString::InvalidPos;
    if ( (startPos != NEString::InvalidPos) && (startPos != NEString::EndPos) )
    {
        int diff = static_cast<int>(lenReplace - charCount);
        NEString::CharPos endPos = startPos + charCount;
        move( endPos, diff );
        CharType * dst = getChars( startPos );
        while ( *strReplace != static_cast<CharType>(NEString::EndOfString) )
            *dst ++ = *strReplace ++;

        nextPos = endPos + diff;
    }
    return nextPos;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Implement>::replaceWith( const CharType * strOrigin
                                                        , NEString::CharCount lenOrigin
                                                        , const CharType * strReplace
                                                        , NEString::CharCount lenReplace
                                                        , NEString::CharPos startPos )
{
    ASSERT( isValid() );
    return replaceWith( findFirstOf( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Implement>::getStringEnd( void )
{
    return ( isValid( ) ? mData->strBuffer + mData->strUsed : static_cast<CharType *>(NULL) );
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Implement>::getStringEnd( void ) const
{
    return (isValid( ) ? mData->strBuffer + mData->strUsed : static_cast<const CharType *>(NULL));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isNull( void ) const
{
    return ( mData == static_cast<const NEString::SString<CharType> *>(NULL) );
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isValidPtr( void ) const
{
    return ( mData != static_cast<const NEString::SString<CharType> *>(NULL) );
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Implement>::getChars( NEString::CharPos atPos ) const
{
    const CharType * result = static_cast<const CharType *>(&TEString<CharType, Implement>::EmptyChar);
    if ( isValidPtr() )
    {
        // strUsed is always less than NEString::EndPos
        atPos   = mData->strUsed < atPos ? mData->strUsed : atPos;
        result  = mData->strBuffer + atPos;
    }

    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Implement>::getChars( NEString::CharPos atPos )
{
    atPos = atPos == NEString::EndPos ? getLength( ) : atPos;
    return (isValid( ) && (mData->strUsed >= atPos) ? mData->strBuffer + atPos : static_cast<CharType *>(NULL));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::SString<CharType> & TEString<CharType, Implement>::getDataString( void )
{
    return (isValid( ) ? *mData : *NEString::getInvalidString<CharType>( ));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline const NEString::SString<CharType> & TEString<CharType, Implement>::getDataString( void ) const
{
    return (isValid( ) ? *mData : *NEString::getInvalidString<CharType>( ));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::eEncoding TEString<CharType, Implement>::getInitEncoding( void ) const
{
    return mEncode;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::reallocate( NEString::CharCount charsAdd, NEString::eEncoding encode )
{
    NEString::SString<CharType> * tmp = NEString::reallocSpace<CharType, CharType>( *mData, charsAdd, isValid( ) ? mData->strEncoding : encode );
    if ( NEString::isValid<CharType>(tmp))
    {
        releaseSpace( mData );
        mData   = tmp;

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::canRead( NEString::CharPos atPos ) const
{
    return (isValidPtr() && NEString::canRead<CharType>(*mData, atPos));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::canWrite( NEString::CharPos atPos ) const
{
    return (isValidPtr() && NEString::canWrite<CharType>( *mData, atPos ));
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::release( void )
{
    NEString::releaseSpace<CharType>( mData );
    mData   = NEString::getInvalidString<CharType>( );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( NEString::eEncoding encode )
    : Implement ( )

    , mData     ( NEString::initString<CharType>(static_cast<NEString::CharCount>(0), encode) )
    , mEncode   ( encode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( CharType ch, NEString::eEncoding encode )
    : Implement ( )

    , mData     ( NEString::initString<CharType, CharType>(&ch, 1, encode) )
    , mEncode   ( encode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( const CharType * strSource, NEString::eEncoding encode )
    : Implement ( )

    , mData     ( NEString::initString<CharType, CharType>(strSource, NEString::CountAll, encode) )
    , mEncode   ( encode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( const CharType * strSource, NEString::CharCount charCount, NEString::eEncoding encode )
    : Implement ( )

    , mData     ( NEString::initString<CharType, CharType>(strSource, charCount, encode) )
    , mEncode   ( encode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( const NEString::SString<CharType> & strSource, NEString::eEncoding encode )
    : Implement ( )

    , mData     ( NEString::initString<CharType, CharType>(strSource, encode) )
    , mEncode   ( encode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Implement>::TEString( const TEString<CharType, Implement> & strSource )
    : Implement ( )

    , mData     ( NEString::initString<CharType, CharType>( strSource.getDataString(), strSource.getInitEncoding() ) )
    , mEncode   ( strSource.mEncode )

#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
TEString<CharType, Implement>::~TEString( void )
{
    NEString::releaseSpace<CharType>(mData);

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::findOneOf( const CharType * chars, NEString::CharPos startPos /*= NEString::StartPos*/ ) const
{
    NEString::CharPos result= NEString::InvalidPos;
    if ( canRead(startPos) && (chars != static_cast<const CharType *>(NULL)) )
    {
        const CharType * strThis    = getBuffer( startPos );
        for ( ; (result == NEString::InvalidPos) && (*strThis != NEString::EndOfString); ++strThis, ++ startPos)
        {
            CharType chThis = *strThis;
            for ( const CharType * strOther = chars; (result == NEString::InvalidPos) && (*strOther != NEString::EndOfString); ++ strOther)
            {
                CharType chOther = *strOther;
                result = chThis == chOther ? startPos : NEString::InvalidPos;
            }
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::findFirstOf( CharType chSearch, NEString::CharPos startPos /*= NEString::StartPos*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result = NEString::InvalidPos;

    if ( canRead( startPos ) )
    {
        CharType chUpper = caseSensitive ? chSearch : makeUpper( chSearch );
        CharType chLower = caseSensitive ? chSearch : makeLower( chSearch );
        const CharType * strThis = getBuffer( startPos );
        for ( ; (result == NEString::InvalidPos) && (*strThis != NEString::EndOfString); ++strThis, ++ startPos )
        {
            result = (*strThis == chUpper) || (*strThis == chLower) ? startPos : NEString::InvalidPos;
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::findFirstOf( const CharType * phrase, NEString::CharPos startPos /*= NEString::StartPos*/, bool caseSensitive /*= true*/, bool wholeWord /*= false*/ ) const
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead( startPos ) && (phrase != static_cast<const CharType *>(NULL)) )
    {
        NEString::CharCount lenThis = getLength();
        NEString::CharCount lenOther= NEString::getStringLength<CharType>(phrase);
        if ( (lenThis - startPos) >= lenOther )
        {
            ASSERT( lenThis >= lenOther);
            NEString::CharPos endPos= lenThis - lenOther + 1;
            const CharType * buffer = static_cast<const CharType *>(NULL);
            CharType beginPrev      = static_cast<CharType>(NEString::EndOfString);
            CharType beginCurr      = static_cast<CharType>(NEString::EndOfString);
            CharType endCurr        = static_cast<CharType>(NEString::EndOfString);
            CharType endNext        = static_cast<CharType>(NEString::EndOfString);

            if ( wholeWord )
            {
                buffer      = getChars( startPos );
                beginPrev   = startPos == NEString::StartPos ? static_cast<CharType>(NEString::EndOfString) : *(buffer - 1);
                endNext     = *(buffer + lenOther - 1);
            }

            for ( NEString::CharPos pos = startPos; (result == NEString::InvalidPos) && (pos < endPos); ++ pos )
            {
                bool isWord = true;
                if ( buffer != static_cast<const CharType *>(NULL) )
                {
                    isWord      = false;
                    endCurr     = endNext;
                    endNext     = *(buffer + lenOther);
                    beginCurr   = *buffer ++;
                    if ( (isAlphanumeric(beginPrev) == false) || (isAlphanumeric(beginCurr) == false) )
                    {
                        isWord = (isAlphanumeric(endCurr) == false) || (isAlphanumeric(endNext) == false);
                    }
                    beginPrev = beginCurr;
                }
                if ( isWord && (compareString( pos, phrase, lenOther, caseSensitive ) == NEMath::CompEqual) )
                {
                    result = pos;
                }
            }
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::findLastOf( CharType chSearch, NEString::CharPos startPos /*= NEString::EndPos*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result = NEString::InvalidPos;
    NEString::CharCount strLen = getLength( );
    if ( startPos == NEString::EndPos )
    {
        startPos = strLen != 0 ? strLen - 1 : 0;
    }
    if ( canRead( startPos ) )
    {
        CharType chUpper = caseSensitive ? chSearch : makeUpper( chSearch );
        CharType chLower = caseSensitive ? chSearch : makeLower( chSearch );

        const CharType * begin  = getBuffer(NEString::StartPos);
        const CharType * end    = getBuffer(startPos);
        for ( ; (result == NEString::InvalidPos) && (end >= begin); -- end, -- startPos)
        {
            result = (*end == chUpper) || (*end == chLower) ? startPos : NEString::InvalidPos;
        }
    }

    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::findLastOf( const CharType * phrase, NEString::CharPos startPos /*= NEString::EndPos*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result    = NEString::InvalidPos;
    NEString::CharCount count   = NEString::getStringLength<CharType>( phrase );
    NEString::CharCount strLen  = getLength();

    if ( startPos == NEString::EndPos )
    {
        startPos = strLen != 0 ? strLen - 1 : 0;
    }
    if ( canRead(startPos) && (count != 0))
    {
        const CharType * begin  = getBuffer( NEString::StartPos );
        const CharType * end    = getBuffer( startPos );

        for ( NEString::CharPos pos = startPos; (result == NEString::InvalidPos) && (end >= begin); --end, --pos )
        {
            if ( (compareString( pos, phrase, count, caseSensitive ) == NEMath::CompEqual) )
            {
                result = pos;
            }
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Implement>::compare( const CharType * what, NEString::CharPos startAt /*= NEString::StartPos*/, NEString::CharCount charCount /*= NEString::CountAll*/, bool caseSensitive /*= true*/) const
{   return compareString( startAt, what, charCount, caseSensitive );   }

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Implement>::compare(const TEString<CharType, Implement> & other, bool caseSensitive /*= true*/ ) const
{
    return compare(other.getChars(NEString::StartPos), NEString::StartPos, NEString::CountAll, caseSensitive);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
bool TEString<CharType, Implement>::substring(TEString<CharType, Implement> & outResult, NEString::CharPos startPos /* = NEString::StartPos */, NEString::CharCount charCount /*= NEString::CountAll*/ ) const
{
    bool result = false;
    outResult.release();
    // The encoding must be already set
    // outResult.mEncode   = mEncode;
    if ( canRead(startPos) )
    {
        const CharType * buffer     = getBuffer( startPos );
        NEString::CharCount remain  = mData->strUsed - startPos;
        charCount = MACRO_MIN(charCount, remain);
        result = outResult.copy(buffer, charCount) != 0;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::substring( TEString<CharType, Implement> & outResult, const CharType * strPhrase, NEString::CharPos startPos /*= NEString::StartPos*/ ) const
{
    NEString::CharPos result= NEString::InvalidPos;
    const CharType * buffer = getBuffer(startPos);
    const CharType * next   = static_cast<const CharType *>(NULL);

    NEString::CharPos pos   = NEString::findFirstOf<CharType>(strPhrase, buffer, NEString::StartPos, &next);
    NEString::CharPos nextPos= next != static_cast<const CharType *>(NULL) ? startPos + static_cast<NEString::CharPos>(next - buffer) : NEString::InvalidPos;
    outResult.copy( buffer, pos != NEString::InvalidPos ? pos : NEString::CountAll );

    if ( nextPos < getLength() )
        result = nextPos;
    else if ( nextPos == getLength() )
        result = NEString::EndPos;
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::substring( TEString<CharType, Implement> & outResult, CharType chSymbol, NEString::CharPos startPos /* = NEString::StartPos */ ) const
{
    NEString::CharPos result = NEString::InvalidPos;
    const CharType * buffer = getBuffer(startPos);
    const CharType * next = static_cast<const CharType *>(NULL);

    NEString::CharPos pos = NEString::findFirstOf<CharType>(chSymbol, buffer, NEString::StartPos, &next);
    NEString::CharPos nextPos = next != static_cast<const CharType *>(NULL) ? startPos + static_cast<NEString::CharPos>(next - buffer) : NEString::InvalidPos;
    outResult.copy(buffer, pos != NEString::InvalidPos ? pos : NEString::CountAll);

    if ( nextPos < getLength( ) )
        result = nextPos;
    else if ( nextPos == getLength( ) )
        result = NEString::EndPos;
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isNumeric( bool signIgnore /*= true*/ ) const
{
    bool result = false;
    if ( isValid() )
    {
        result = true;
        const CharType * src = getChars( NEString::StartPos );
        if ( signIgnore && ( (*src == '-') || (*src == '+') ))
            ++src; // Escape sign, if it should.

        for ( ; result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = isNumeric( *src );
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isAlphanumeric( void ) const
{
    bool result = false;
    if ( isValid() )
    {
        result = true;
        for (const CharType * src = getChars( NEString::StartPos ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = isAlphanumeric(*src);
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isValidName( void ) const
{
    bool result = false;
    if ( isValid( ) )
    {
        result = true;
        for ( const CharType * src = getChars( NEString::StartPos ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
        {
            if ( isAlphanumeric( *src ) == false )
                result = (*src == '-') || (*src == '_') || (*src == '.');
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::validate( const CharType * validityList ) const
{
    bool result = false;
    if ( isValid( ) && (NEString::isEmpty<CharType>(validityList) == false) )
    {
        result = true;
        for ( const CharType * src = getChars( NEString::StartPos ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = NEString::findFirstOf<CharType>(*src, validityList) != NEString::InvalidPos;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::truncate( NEString::CharCount maxChars )
{
    if ( maxChars == 0 )
    {
        release();
    }
    else if ( NEString::geStringRequiredSize<CharType>(maxChars) < getActualLength() )
    {
        ASSERT( isValidPtr() );
        NEString::SString<CharType> * tmp = NEString::initString<CharType>(getString(), maxChars, mEncode);
        if (tmp != static_cast<NEString::SString<CharType> *>(NULL))
        {
            releaseSpace(mData);
            mData   = tmp;

#ifdef DEBUG
            mString = mData->strBuffer;
#endif // DEBUG
        }
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::remove( NEString::CharPos startPos, NEString::CharCount charCount /*= NEString::CountAll*/ )
{
    if ( canRead( startPos ) )
    {
        NEString::CharCount len    = getLength();
        NEString::CharCount remain = len - startPos;
        charCount = MACRO_MIN(charCount, remain);

        move( startPos + charCount, -1 * static_cast<int>(charCount) );
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::compact( void )
{
    if ( isValid() )
    {
        NEString::CharCount len   = getLength();
        NEString::CharCount space = NEString::geStringRequiredSize<CharType>( len );
        if ( space < getActualLength() )
        {
            NEString::SString<CharType> * tmp = NEString::initString<CharType>( getString( ), len, mEncode );
            if ( tmp != static_cast<NEString::SString<CharType> *>(NULL) )
            {
                releaseSpace( mData );
                mData   = tmp;

#ifdef DEBUG
                mString = mData->strBuffer;
#endif // DEBUG
            }
        }
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharCount  TEString<CharType, Implement>::copy(const CharType * strSource, NEString::CharCount charCount /*= NEString::CountAll */)
{
    NEString::CharCount result = 0;

    charCount = charCount == NEString::CountAll ? NEString::getStringLength<CharType>(strSource) : charCount;
    resize(charCount);
    if ( isValid() )
    {
        result = NEString::copyString<CharType, CharType>(*mData, strSource, NEString::StartPos, charCount);
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharCount  TEString<CharType, Implement>::append(const CharType * strSource, NEString::CharCount charCount /*= NEString::CountAll*/)
{
    NEString::CharCount result = 0;
    charCount = charCount == NEString::CountAll ? NEString::getStringLength<CharType>(strSource) : charCount;
    resize( getLength() + charCount);
    if ( isValid( ) )
    {
        result = NEString::appendString<CharType, CharType>(*mData, strSource, charCount);
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharCount TEString<CharType, Implement>::move( NEString::CharPos startPos, int moveTo )
{
    NEString::CharCount result = 0;
    if ( moveTo < 0 )
    {
        NEString::CharCount cnt = static_cast<NEString::CharCount>( -1 * moveTo);
        NEString::CharPos dstPos= startPos >= cnt ? startPos + moveTo : NEString::StartPos;
        const CharType * src    = getChars(startPos);
        CharType * dst          = getChars(dstPos);
        for ( ; *src != static_cast<CharType>(NEString::EndOfString); ++ result)
            *dst ++ = *src ++;
        *dst = static_cast<CharType>(NEString::EndOfString);
        mData->strUsed  += moveTo;
    }
    else if ( moveTo > 0 )
    {
        NEString::CharPos eos = getLength();
        if ( resize( eos + moveTo ) )
        {
            const CharType * end = getBuffer( startPos );
            CharType * src = getStringEnd( );
            CharType * dst = src + moveTo;
            if ( startPos < eos )
            {
                for ( ; src >= end; ++ result )
                    *dst -- = *src --;
            }
            else
            {
                *dst    = static_cast<CharType>(NEString::EndOfString);
                result  = moveTo;
            }
            mData->strUsed += moveTo;
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::swap( NEString::CharPos startPos /*= NEString::StartPos*/, NEString::CharCount charCount /*= NEString::CountAll*/ )
{
    if ( canRead(startPos) )
    {
        NEString::CharCount len = getLength();
        ASSERT(len > 0);
        ASSERT(len > startPos);
        NEString::CharCount remain = len - startPos - 1;
        charCount = MACRO_MIN(charCount, remain);
        CharType * begin= getChars(startPos);
        CharType * end  = getChars(startPos + charCount);
        for ( ; begin < end; ++begin, --end )
        {
            CharType ch = *begin;
            *begin = *end;
            *end   = ch;
        }
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::insertAt( CharType chSrc, NEString::CharPos atPos )
{
    if ( move(atPos, 1) != 0 )
        mData->strBuffer[atPos] = chSrc;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::insertAt( const CharType * strSrc, NEString::CharPos atPos, NEString::CharCount charCount /*= NEString::CountAll*/ )
{
    if ( strSrc != static_cast<const CharType *>(NULL) )
    {
        NEString::CharCount eos = getLength( );
        NEString::CharCount len = NEString::getStringLength( strSrc );
        atPos = atPos == NEString::EndPos ? eos : atPos;
        charCount = MACRO_MIN( len, charCount );
        if ( move( atPos, charCount ) != 0 )
        {
            CharType * dst = getChars( atPos );
            while ( *strSrc != static_cast<CharType>(NEString::EndOfString) )
                *dst ++ = *strSrc ++;
            if ( atPos == eos )
                *dst = static_cast<CharType>(NEString::EndOfString);
        }
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
void TEString<CharType, Implement>::insertAt( const TEString<CharType, Implement> & strSrc, NEString::CharPos atPos, NEString::CharCount charCount /*= NEString::CountAll*/ )
{
    insertAt( strSrc.getString(), atPos, charCount);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::replace( CharType chOrigin, CharType chReplace, NEString::CharPos startPos /*= NEString::StartPos*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead(startPos) )
    {
        CharType * buffer     = getChars( NEString::StartPos );
        NEString::CharPos len = getLength();
        NEString::CharPos pos = startPos;
        for ( ; (result == NEString::InvalidPos) && (pos < len); ++ pos )
        {
            if ( buffer[pos] == chOrigin )
            {
                buffer[pos] = chReplace;
                if ( replaceAll == false )
                    result = pos + 1;
            }
        }
        result = (result == NEString::InvalidPos) || (result == len) ? NEString::EndPos : result;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::replace( const CharType * strOrigin, const CharType * strReplace, NEString::CharPos startPos /*= NEString::StartPos*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead(startPos) )
    {
        NEString::CharPos lenOrigin = NEString::getStringLength<CharType>( strOrigin );
        NEString::CharPos lenReplace= NEString::getStringLength<CharType>( strReplace );
        if ( lenOrigin != 0 )
        {
            bool replacedOnce = false;
            do
            {
                result = replaceWith( strOrigin, lenOrigin, strReplace, lenReplace, startPos );
                startPos = result;
                if ( result != NEString::InvalidPos )
                    replacedOnce = true;
            } while ( replaceAll && (result != NEString::InvalidPos) );

            result = ( (result == NEString::InvalidPos) && replacedOnce ? NEString::EndPos : result );
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::replace( const TEString<CharType, Implement> & strOrigin, const TEString<CharType, Implement> & strReplace, NEString::CharPos startPos /*= NEString::StartPos*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenOrigin  = strOrigin.getLength();
        NEString::CharPos lenReplace = strReplace.getLength();
        if ( (lenOrigin != 0) && (lenReplace != 0) )
        {
            bool replacedOnce = false;
            const CharType * bufOrigin = strOrigin.getString();
            const CharType * bufReplace= strReplace.getString();
            ASSERT( (bufOrigin != static_cast<const CharType *>(NULL)) && (bufReplace != static_cast<const CharType *>(NULL)) );
    
            do
            {
                result = replaceWith( bufOrigin, lenOrigin, bufReplace, lenReplace, startPos );
                startPos = result;
                if ( result != NEString::InvalidPos )
                    replacedOnce = true;
            } while ( replaceAll && (result != NEString::InvalidPos) );

            result = ((result == NEString::InvalidPos) && replacedOnce ? NEString::EndPos : result);
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::replace( const CharType * strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove )
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        if ( (charsRemove != 0) && (lenReplace != 0) )
        {
            result = replaceWith(startPos, charsRemove, strReplace, lenReplace);
            /*********************************
            int diff = static_cast<int>(lenReplace - lenOrigin);
            NEString::CharPos endPos = startPos + lenOrigin;
            Move( endPos, diff );
            CharType * dst = getChars( startPos );
            const CharType * src = strReplace;
            while ( *src != static_cast<CharType>(NEString::EndOfString) )
                *dst ++ = *src ++;
            result = endPos + diff;
            *********************************/
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Implement>::replace( const TEString<CharType, Implement> & strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove )
{
    NEString::CharPos result = NEString::InvalidPos;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenReplace = strReplace.getLength();
        if ( (charsRemove != 0) && (lenReplace != 0) )
        {
            result = replaceWith( startPos, charsRemove, strReplace.getString(), lenReplace );

            /*****************************
            int diff = static_cast<int>(lenReplace - lenOrigin);
            NEString::CharPos endPos = startPos + lenOrigin;
            Move( endPos, diff );
            CharType * dst = getChars( startPos );
            const CharType * src = strReplace.getString();
            while ( *src != static_cast<CharType>(NEString::EndofString) )
                *dst ++ = *src ++;
            result = endPos + diff;
            *****************************/
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Implement>::remove(const CharType * strRemove, NEString::CharPos startPos /*= NEString::StartPos*/, bool removeAll /*= true*/)
{
    return replace(strRemove, &EmptyChar, startPos, removeAll);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isEmpty( void ) const
{
    return ( (isValid() == false) || (mData->strUsed == 0) );
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::isValid( void ) const
{
    return NEString::isValid<CharType>(mData);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline unsigned int TEString<CharType, Implement>::getLength( void ) const
{
    return (isValid() ? mData->strUsed : 0);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline unsigned int TEString<CharType, Implement>::getActualLength( void ) const
{
    return (isValid() ? mData->strSpace : 0);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline unsigned int TEString<CharType, Implement>::getUsedSpace(void) const
{
    return (isValid() ? (mData->strUsed + 1) * sizeof(CharType) : 0);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::clear( void )
{
    release();
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Implement>::getBuffer( NEString::CharPos startAt /*= NEString::StartPos*/ ) const
{
    return getChars(startAt);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Implement>::getString( void ) const
{
    return static_cast<const CharType *>(isValid() ? mData->strBuffer : NULL);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::eEncoding TEString<CharType, Implement>::getEncoding( void ) const
{
    return (isValid() ? mData->strEncoding : NEString::EncodeInvalid);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Implement>::getAt( NEString::CharPos atPos ) const
{
    return (canRead(atPos) ? mData->strBuffer[atPos] : NEString::EndOfString);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Implement>::setAt( CharType ch, NEString::CharPos atPos /*= NEString::EndPos*/ )
{
    if ( isValidPtr() )
    {
        NEString::setAt<CharType, CharType>( *mData, ch, atPos );
    }
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Implement>::resize( NEString::CharCount maxSpace )
{
    NEString::CharCount len = getLength( );
    maxSpace = MACRO_MAX( maxSpace, len );
    NEString::CharCount addChars        = maxSpace - len;
    NEString::SString<CharType> * tmp   = NEString::reallocSpace<CharType, CharType>( *mData, addChars );
    if ( NEString::isValid<CharType>(tmp) )
    {
        NEString::releaseSpace<CharType>( mData );
        mData   = tmp;

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }
    return (maxSpace < mData->strSpace);
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimLeft( void )
{
    int result = 0;
    if (isEmpty() == false)
    {
        result = NEString::trimLeft<CharType>( getChars(NEString::StartPos), getLength() );
        ASSERT(result <= static_cast<int>(mData->strUsed));
        mData->strUsed -= result;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimLeft(TEString<CharType, Implement> & strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        int len = static_cast<int>(getLength());
        const CharType * begin = getString();
        while (isWhitespace(*begin ++))
            ++ result;

        int remain = len - result;
        if (remain != 0)
        {
            ASSERT(remain > 0);
            strResult.copy(begin, remain);
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimRight( void )
{
    int result = 0;
    if ( isEmpty() == false )
    {
        result = NEString::trimRight<CharType>( getChars(NEString::StartPos), getLength() );
        ASSERT(result <= static_cast<int>(mData->strUsed));
        mData->strUsed -= result;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimRight(TEString<CharType, Implement> & strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        int len = static_cast<int>(getLength());
        const CharType * begin  = getString();
        const CharType * end    = getChars(len - 1);
        while ((end != begin) && isWhitespace(*end) )
        {
            -- end;
            ++ result;
        }

        int remain = len - result;
        if (remain != 0)
        {
            ASSERT(remain > 0);
            strResult.copy(begin, remain);
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimAll( void )
{
    int result = 0;
    if ( isEmpty() == false )
    {
        result = NEString::trimAll<CharType>(getChars(NEString::StartPos), getLength());
        ASSERT(result <= static_cast<int>(mData->strUsed));
        mData->strUsed -= result;
    }

    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Implement>::trimAll(TEString<CharType, Implement> & strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        int len = static_cast<int>(getLength());
        const CharType * begin = getString();
        const CharType * end = getChars(len - 1);

        while ((end != begin) && isWhitespace(*end))
        {
            -- end;
            ++ result;
        }

        while ((begin != end) && isWhitespace(*begin))
        {
            ++ begin;
            ++ result;
        }

        int remain = len - result;
        if (remain != 0)
        {
            ASSERT(remain > 0);
            strResult.copy(begin, remain);
        }
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline NEString::CharCount TEString<CharType, Implement>::makeAlphanumeric(void)
{
    NEString::CharCount result = 0;
    if (isEmpty() == false)
    {
        CharType * begin= getChars( NEString::StartPos);
        CharType * dst  = begin;
        for (const CharType * src = begin; *src != static_cast<CharType>(NEString::EndOfString); ++src)
        {
            if ( isAlphanumeric(*src) )
                *dst ++ = *src;
        }
        *dst    = static_cast<CharType>(NEString::EndOfString);
        result  = static_cast<NEString::CharCount>(dst - begin);
        mData->strUsed = result;
    }
    return result;
}

template<typename CharType, class Implement /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Implement>::getUnsafeBuffer(void)
{
    return (isEmpty() == false ? mData->strBuffer : static_cast<CharType *>(NULL));
}

#endif  // AREG_BASE_TESTRING_HPP
