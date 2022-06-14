#pragma once
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/base/TEString.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, ASCII string Class.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NEString.hpp"
#include "areg/base/TETemplateBase.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class IEInStream;
class IEOutStream;

//////////////////////////////////////////////////////////////////////////
// TEString class template declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The basic string class template, which contains basic operations 
 *          to set, append, insert, search and replace string, as well to get substring.
 *          When deriving class, should be paid special attention on string structure
 *          validation. The string should be allocated and released via methods 
 *          defined in NEString namespace.
 *          The string class template constructors and destructor are protected.
 *          To be able to instantiate string, the class should be derived to define
 *          basic operations.
 * 
 * \tparam  CharType    The type of character. Normally, either 8-bit character or 16-bit wide character.
 * \tparam  Helper      The name of class where the basic string alphabetic, comparing and 
 *                      stream read/write operations are Helpered.
 **/
template<typename CharType, class Helper = TEStringImpl<CharType> >
class TEString
{
//////////////////////////////////////////////////////////////////////////
// Public constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Empty character constant
     **/
    static constexpr CharType   EmptyChar       { static_cast<CharType>('\0') };

    /**
     * \brief   New line constant
     **/
    static constexpr CharType   NewLine         { static_cast<CharType>('\n') };

    /**
     * \brief   DOS format new line
     **/
    static constexpr CharType   NewLineDos[2]   {static_cast<CharType>('\r'), static_cast<CharType>('\n')};

//////////////////////////////////////////////////////////////////////////
// Protected constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Default constructor creates empty string.
     **/
    inline TEString( void );

    /**
     * \brief   Sets the char to string.
     * \param   chSource    The character to set at the begin of string.
     **/
    inline TEString( CharType chSource );

    /**
     * \brief   Copies string from given source.
     * \param   strSource   The initial string to initialize. If nullptr, empty string is created.
     **/
    inline TEString( const CharType * strSource );

    /**
     * \brief   Copies specified amount of symbols from given string.
     * \param   strSource   The null-terminated string buffer to initialize. If nullptr, empty string is created.
     * \param   charCount   The number of character to initialize from given string source. If zero, empty string is created.
     **/
    inline TEString( const CharType * strSource, NEString::CharCount charCount );

    /**
     * \brief   Copies string structure.
     * \param   strSource   The string structure as source to initialize.
     **/
    inline TEString( const NEString::SString<CharType> & strSource );

    /**
     * \brief   Copy constructor.
     * \param   strSource   The source to copy data.
     **/
    inline TEString( const TEString<CharType, Helper> & strSource );

    /**
     * \brief   Move constructor.
     * \param   strSource   The source to move data.
     **/
    inline TEString( TEString<CharType, Helper> && strSource );

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
     *          Otherwise, returns NEString::INVALID_POS.
     * \param   chars       The one or more characters as set in null-terminated string to search.
     * \param   startPos    The start position in string to search. Should be valid.
     *                      By default, starts to search at begin of string, i.e. at 'NEString::START_POS'
     * \return  Returns valid string position value, if any occurrence of given characters found.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findOneOf( const CharType * chars, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirstOf( CharType chSearch, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the first occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the begin of string,
     *                          i.e. at 'NEString::START_POS'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \param   wholeWord       If true, the search of phrase should match the whole word.
     *                          Otherwise, the match is by any match. By default, the search is by any phrase.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findFirstOf( const CharType * phrase, NEString::CharPos startPos = NEString::START_POS, bool caseSensitive = true, bool wholeWord = false ) const;
    
    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the character match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLastOf( CharType chSearch, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Find the last occurrence of given character in the string. If found, returns valid
     *          position value in the string. Otherwise, it returns NEString::INVALID_POS value.
     * \param   chSearch        The character to search in the string.
     * \param   startPos        The start position in string to search. Should be valid.
     *                          By default, starts to search at the end of string,
     *                          i.e. at 'NEString::END_POS'
     * \param   caseSensitive   If true, the search of phrase should be match should be by exact, i.e. case-sensitive.
     *                          Otherwise, the search is by upper and lower case.
     * \return  Returns valid string position value, if found given character.
     *          Otherwise, returns NEString::InvalildPos value.
     **/
    NEString::CharPos findLastOf( const CharType * phrase, NEString::CharPos startPos = NEString::END_POS, bool caseSensitive = true ) const;

    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     * \param   strOther        The given other string to compare
     * \param   startAt         The position of string to start comparing. 
     *                          By default, it compares at the begin of string.
     * \param   charCount       The number of characters to compare.
     *                          By default, it compares until end of string
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     **/
    inline NEMath::eCompare compare( const CharType * strOther, NEString::CharPos startAt = NEString::START_POS, NEString::CharCount charCount = NEString::COUNT_ALL, bool caseSensitive = true) const;
    
    /**
     * \brief   Compares with the given string at given position. The comparing is either complete string or
     *          given number of characters. Comparing either by exact match (case-sensitive) or ignore case.
     *          The function returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     * \param   other           The string to compare with.
     * \param   caseSensitive   If true, compares by exact match, i.e. case-sensitive.
     *                          Otherwise, it ignores upper and lower cases.
     * \return  Returns:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     **/
    inline NEMath::eCompare compare(const TEString<CharType, Helper> & other, bool caseSensitive = true ) const;

    /**
     * \brief   Extracts the substring starting at startPos and given number of characters.
     * \param   outResult   On output, this parameter contains resulting substring.
     * \param   startPos    Starting position of the substring to create.
     *                      By default, the substring starts at the begin, i.e. NEString::START_POS
     * \param   charCount   The number of characters in substring, starting at given starting position.
     *                      By default, it gets characters until end of string.
     * \return  Returns true if could create substring. Otherwise, returns false.
     *          The substring can fail if method is called for invalid string, if given position is invalid.
     **/
    bool substring( TEString<CharType, Helper> & outResult, NEString::CharPos startPos = NEString::START_POS, NEString::CharCount charCount = NEString::COUNT_ALL ) const;

    /**
     * \brief   Searches given phrase in the string starting from given position until the end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after phrase. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   strPhrase   The phrase to search in the string.
     * \param   startPos    The starting position to search the string.
     * \return  Returns next position after searched phrase and value are followings:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the phrase is not at the end;
     *              -   NEString::END_POS if found the phrase at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     *
     * \example     Example of Substr()
     * \code [.cpp]
     *
     *  String test("0123 456 789 0123");
     *  String result;
     *  NEString::CharPos next = NEString::START_POS;
     *  next = test.Substr(result, "0123", next);   // results: next == 4, result == ""
     *  next = test.Substr(result, "0123", next);   // results: next == NEString::END_POS, result == " 456 789 "
     *
     *  next = NEString::START_POS;
     *  next = test.Substr(result, " ", next);      // results: next == 5, result == "0123"
     *  next = test.Substr(result, " ", next);      // results: next == 9, result == "456"
     *  next = test.Substr(result, " ", next);      // results: next == NEString::INVALID_POS, result == "0123"
     * \endcode
     **/
    NEString::CharPos substring( TEString<CharType, Helper> & outResult, const CharType * strPhrase, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Searches given symbol in the string starting from given position until end of string.
     *          If found, copies the string data into the result until the found position and returns position
     *          next after symbol. If not found, will copy complete string until end.
     * \param   outResult   On output, the string contains result of copied data until found position
     *                      or contains the complete string until the end.
     * \param   chSymbol    The symbol to search in the string.
     * \param   startPos    The starting position to search the symbol.
     * \return  Returns next position after searched symbol and value are followings:
     *              -   Valid string position not equal to NEString::END_POS,
     *                  if found phrase and the symbol is not at the end;
     *              -   NEString::END_POS if found the symbol at end of string;
     *              -   NEString::INVALID_POS if could not find the phrase.
     **/
    NEString::CharPos substring( TEString<CharType, Helper> & outResult, CharType chSymbol, NEString::CharPos startPos = NEString::START_POS ) const;

    /**
     * \brief   Copies given amount of characters of given string and returns the amount of copied characters.
     *          If string has not enough space to copy characters, it will reallocate the space.
     * \param   strSource   The source of string to copy characters.
     * \param   charCount   The number of characters to copy. By default, it copies all characters.
     * \return  Returns number of copied characters. If zero, nothing was copied.
     **/
    NEString::CharCount copy( const CharType * strSource, NEString::CharCount charCount = NEString::COUNT_ALL );

    /**
     * \brief   Appends given amount of characters of given string at the end of the string
     *          and returns the amount of appended characters. If string has not enough space
     *          to append characters, it will reallocate the space.
     * \param   strSource   The source of string to append characters.
     * \param   charCount   The number of characters to append. By default, it copies all characters.
     * \return  Returns number of append characters. If zero, nothing was added.
     **/
    NEString::CharCount append( const CharType * strSource, NEString::CharCount charCount = NEString::COUNT_ALL );

    /**
     * \brief   Inserts given source of character at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
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
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   strSource   The source of character to insert.
     * \param   atPos       The position in the string to insert.
     * \param   charCount   The number of characters in string to insert.
     *                      By default it inserts complete source of string.
     **/
    void insertAt( const CharType * strSource, NEString::CharPos atPos, NEString::CharCount charCount = NEString::COUNT_ALL );

    /**
     * \brief   Inserts given source of string at given position in the string. 
     *          The character can be inserted at any position, including
     *          begin of string or at the end of string (append). The caller should make sure
     *          that the specified position is valid and not more end-of-string.
     *          If string has not enough space to insert character, it will reallocate the space.
     * \param   strSource   The source of character to insert.
     * \param   atPos       The position in the string to insert.
     * \param   charCount   The number of characters in string to insert.
     *                      By default it inserts complete source of string.
     **/
    void insertAt( const TEString<CharType, Helper> & strSrc, NEString::CharPos atPos, NEString::CharCount charCount = NEString::COUNT_ALL );

    /**
     * \brief   Searches and replaces given character by another given. The search and replacement starts at given position.
     *          The method either replaces first match of character and returns next position,
     *          or replaces all matches of given characters and returns end-of-string position (NEString::END_POS).
     * \param   chOrigin    The character to be searched to replace.
     * \param   chReplace   The character, which should be set
     * \param   startPos    The position to start searching character to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matched characters in the string and returns End Position (NEString::END_POS).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'chOrigin' character and replaced, where 'replaceAll' parameter is false.
     *          - NEString::END_POS if replaced character was at the end of string or could replace at least once, where 'replaceAll' is true.
     *          - NEString::INVALID_POS if could not find and replace any character.
     **/
    NEString::CharPos replace( CharType chOrigin, CharType chReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces first match of string and returns next position,
     *          or replaces all matches of given string and returns end-of-string position (NEString::END_POS).
     * \param   strOrigin   The string to be searched to replace.
     * \param   strReplace  The string to set
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches of string and returns End Position (NEString::END_POS).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'strOrigin' string and replaced, where 'replaceAll' parameter is false.
     *          - NEString::END_POS if replaced string was at the end of data or could replace at least once, where 'replaceAll' is true.
     *          - NEString::INVALID_POS if could not find and replace any match of string.
     **/
    NEString::CharPos replace( const CharType * strOrigin, const CharType * strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true );

    /**
     * \brief   Searches and replaces given string by another string. The search and replacement starts at given position.
     *          The method either replaces first match of string and returns next position,
     *          or replaces all matches of given string and returns end-of-string position (NEString::END_POS).
     * \param   strOrigin   The string to be searched to replace.
     * \param   strReplace  The string to set
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   replaceAll  If true, replaces all matches of string and returns End Position (NEString::END_POS).
     *                      Otherwise, replace first matched.
     * \return  Returns:
     *          - valid next position value if found 'strOrigin' string and replaced, where 'replaceAll' parameter is false.
     *          - NEString::END_POS if replaced string was at the end of data or could replace at least once, where 'replaceAll' is true.
     *          - NEString::INVALID_POS if could not find and replace any match of string.
     **/
    NEString::CharPos replace( const TEString<CharType, Helper> & strOrigin, const TEString<CharType, Helper> & strReplace, NEString::CharPos startPos = NEString::START_POS, bool replaceAll = true );

    /**
     * \brief   Replaces specified amount of characters starting at given position by given string.
     *          Returns the next position after replacing string.
     * \param   strReplace  The string to set.
     * \param   startPos    The position to start setting string.
     * \param   charsRemove Amount of characters to replace in string.
     * \return  Returns next position after replacing string. If specified position in invalid
     (          returns NEString::INVALID_POS. If string was inserted at the end, returns NEString::END_POS.
     **/
    NEString::CharPos replace( const CharType * strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove );

    /**
     * \brief   Replaces specified amount of characters starting at given position by given string.
     *          Returns the next position after replacing string.
     * \param   strReplace  The string to set.
     * \param   startPos    The position to start setting string.
     * \param   charsRemove Amount of characters to replace in string.
     * \return  Returns next position after replacing string. If specified position in invalid
     (          returns NEString::INVALID_POS. If string was inserted at the end, returns NEString::END_POS.
     **/
    NEString::CharPos replace( const TEString<CharType, Helper> & strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove );

    /**
     * \brief   Searches and removes given phrase of string. The search starts at given position.
     *          The method either removes only first match of string and returns next position,
     *          or removes all matches of given string and returns end-of-string position (NEString::END_POS).
     * \param   strRemove   The string to search and remove.
     * \param   startPos    The position to start searching string to replace.
     *                      By default, it searches at the begin of string.
     * \param   removeAll   If true, removes all matches of string and returns End Position (NEString::END_POS).
     *                      Otherwise, removes only first matched.
     * \return  Returns:
     *          - valid next position value if found 'strRemove' string phrase and removed, where 'removeAll' parameter is false.
     *          - NEString::END_POS if removed string was at the end of data or could remove at least once, where 'removeAll' is true.
     *          - NEString::INVALID_POS if could not find any match of string.
     **/
    NEString::CharPos remove( const CharType * strRemove, NEString::CharPos startPos = NEString::START_POS, bool removeAll = true );

    /**
     * \brief   Removes specified amount of characters in string at specified starting position.
     *          If 'charCount' is NEString::COUNT_ALL, it will remove all until end, i.e. sets
     *          end-of-string character at specified position.
     * \param   startPos    The starting position to remove string. Operation is ignored if not valid position.
     * \param   charCount   The amount of characters to remove. If 'NEString::COUNT_ALL' removes all until end of string,
     *                      i.e. sets end-of-string value at specified position.
     **/
    inline void remove( NEString::CharPos startPos, NEString::CharCount charCount = NEString::COUNT_ALL );

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
    void swap( NEString::CharPos startPos = NEString::START_POS, NEString::CharCount charCount = NEString::COUNT_ALL );

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
     *          is valid for read. Otherwise, it returns nullptr
     **/
    inline const CharType * getBuffer( NEString::CharPos startAt = NEString::START_POS ) const;

    /**
     * \brief   Returns null-terminated buffer of string or nullptr pointer if string is invalid.
     **/
    inline const CharType * getString( void ) const;

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
    inline void setAt( CharType ch, NEString::CharPos atPos = NEString::END_POS );

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
     *          and returns the actual length of the string.
     **/
    inline int trimLeft( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin of string, 
     *          i.e. removes all whitespace characters at the begin of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline int trimLeft( TEString<CharType, Helper> & OUT strResult ) const;

    /**
     * \brief   Removes whitespace characters from right side, i.e. from the end of string
     *          and returns the actual length of the string.
     **/
    inline int trimRight( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the end of string, 
     *          i.e. removes all whitespace characters at the end of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline int trimRight( TEString<CharType, Helper> & OUT strResult ) const;

    /**
     * \brief   Removes whitespace characters from left and right sides, i.e. from begin and end of string
     *          and returns the actual length of the string.
     **/
    inline int trimAll( void );

    /**
     * \brief   Copies data into given string without trailing whitespace at the begin and end of string, 
     *          i.e. removes all whitespace characters at the begin and end of string, and returns number of
     *          characters copied to the resulting destination string, i.e. the length of resulting string.
     *          The existing string string remain unmodified.
     * \param   strResult   The destination string to copy result.
     * \return  Returns number of characters copied to the resulting string.
     **/
    inline int trimAll( TEString<CharType, Helper> & OUT strResult ) const;

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
// Methods that can be overwritten in Helper class template
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
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     *          The comparing starts from given position, compares given
     *          amount of character of until the end if 'charCount' is NEString::COUNT_ALL,
     *          and the comparing is either by exact match or ignores cases sensitive.
     * \param   startPos        The position in string buffer to start comparing.
     *                          If equal to NEString::START_POS, compares from beginning of string.
     * \param   strOther        The given string to compare
     * \param   charCount       The amount of characters to compare.
     *                          If equal to NEString::COUNT_ALL compares until end of string.
     * \param   caseSensitive   If true, compares exact match of string. Otherwise, compares lower and upper cases.
     * \return  Return:
     *              NEMath::Smaller if string is less than given string
     *              NEMath::Equal   if strings have equal
     *              NEMath::Bigger if string is more than given string
     **/
    inline NEMath::eCompare compareString( NEString::CharPos startPos, const CharType * strOther, NEString::CharCount charCount = NEString::COUNT_ALL, bool caseSensitive = true ) const;

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
     * \brief   Reallocates string buffer.
     * \param   charsAdd    The amount of characters expected to add in string after reallocating buffer
     **/
    inline void reallocate( NEString::CharCount charsAdd );

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
     *          Returns invalid pointer (nullptr) if string is invalid or specified position is invalid for reading.
     **/
    inline CharType * getChars( NEString::CharPos atPos );

    /**
     * \brief   Returns the string buffer starting at specified position.
     *          The buffer is valid for reading only. The specified position should be valid
     *          for reading, i.e. should be less than the string length.
     * \param   atPos       The position in string buffer to get pointer.
     * \return  Returns valid pointer of string if string is valid and specified position is valid for read.
     *          Returns invalid pointer (nullptr) if string is invalid or specified position is invalid for reading.
     **/
    inline const CharType * getChars( NEString::CharPos atPos ) const;

    /**
     * \brief   Return true if string data pointer is nullptr.
     **/
    inline bool isNull( void ) const;

    /**
     * \brief   Returns true if string data structure has valid pointer.
     **/
    inline bool isValidPtr( void ) const;

    /**
     * \brief   Returns the pointer of string buffer starting at the end of string, where the pointer shows end-of-string.
     *          If string is invalid, returns nullptr. The returned pointer is for writing.
     **/
    inline CharType * getStringEnd( void );

    /**
     * \brief   Returns the pointer of string buffer starting at the end of string, where the pointer shows end-of-string.
     *          If string is invalid, returns nullptr. The returned pointer is for read only.
     **/
    inline const CharType * getStringEnd( void ) const;

    /**
     * \brief   Searches string and if found, replace by another.
     * \param   strOrigin   The string to search.
     * \param   lenOrigin   The length of string to search or amount of characters to search.
     * \param   strReplace  The string to replace
     * \param   lenReplace  The amount of characters to replace
     * \param   startPos    The starting position to start searching.
     * \return  Returns next position after replacing string. Returns NEString::INVALID_POS if could not find specified string.
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
     * \return  Returns next position after replacing string. Returns NEString::INVALID_POS if could not find specified string.
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
     * \brief   The instance of helper object that makes char conversions.
     **/
    Helper                          mHelper;

#ifdef DEBUG
    const CharType *                mString;
#endif // DEBUG

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
};

//////////////////////////////////////////////////////////////////////////
// Template class TEString Helperation
//////////////////////////////////////////////////////////////////////////

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isSymbol( CharType ch ) const
{
    return mHelper.implIsSymbol(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isWhitespace( CharType ch ) const
{
    return mHelper.implIsWhitespace(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isAlphanumeric( CharType ch ) const
{
    return mHelper.implIsAlphanumeric(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isNumeric( CharType ch ) const
{
    return mHelper.implIsNumeric(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isLetter( CharType ch ) const
{
    return mHelper.implIsLetter(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Helper>::makeLower( CharType ch ) const
{
    return mHelper.implMakeLower(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Helper>::makeUpper( CharType ch ) const
{
    return mHelper.implMakeUpper(ch);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Helper>::compareString(   NEString::CharPos startPos
                                                                      , const CharType * strOther
                                                                      , NEString::CharCount charCount/*= NEString::COUNT_ALL  */
                                                                      , bool caseSensitive           /*= true                */ ) const
{
    const CharType * leftSide = getChars(startPos);
    const CharType * rightSide= strOther;

    return mHelper.implCompareString(leftSide, rightSide, charCount, caseSensitive);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::readStream( const IEInStream & stream )
{
    int len     = 0;
    stream >> len;

    reallocate( static_cast<NEString::CharCount>(len) );
    CharType * dst  = getChars(NEString::END_POS);

    int count = mHelper.implReadStream(stream, dst, len);

    mData->strUsed += count;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::writeStream( IEOutStream & stream ) const
{
    NEString::CharCount len     = getLength();
    const CharType * src        = getString();
    stream << static_cast<int>(len);

    mHelper.implWriteStream(stream, src);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Helper>::replaceWith(  NEString::CharPos startPos
                                                                 , NEString::CharCount charCount
                                                                 , const CharType * strReplace
                                                                 , NEString::CharCount lenReplace )
{
    NEString::CharPos nextPos = NEString::INVALID_POS;
    if ( (startPos != NEString::INVALID_POS) && (startPos != NEString::END_POS) )
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

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Helper>::replaceWith( const CharType * strOrigin
                                                                    , NEString::CharCount lenOrigin
                                                                    , const CharType * strReplace
                                                                    , NEString::CharCount lenReplace
                                                                    , NEString::CharPos startPos )
{
    ASSERT( isValid() );
    return replaceWith( findFirstOf( strOrigin, startPos, true ), lenOrigin, strReplace, lenReplace);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Helper>::getStringEnd( void )
{
    return ( isValid( ) ? mData->strBuffer + mData->strUsed : nullptr );
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Helper>::getStringEnd( void ) const
{
    return (isValid( ) ? mData->strBuffer + mData->strUsed : nullptr);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isNull( void ) const
{
    return ( mData == nullptr );
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isValidPtr( void ) const
{
    return ( mData != nullptr );
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Helper>::getChars( NEString::CharPos atPos ) const
{
    const CharType * result = static_cast<const CharType *>(&TEString<CharType, Helper>::EmptyChar);
    if ( isValidPtr() )
    {
        // strUsed is always less than NEString::END_POS
        atPos   = mData->strUsed < atPos ? mData->strUsed : atPos;
        result  = mData->strBuffer + atPos;
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Helper>::getChars( NEString::CharPos atPos )
{
    atPos = atPos == NEString::END_POS ? getLength( ) : atPos;
    return (isValid( ) && (mData->strUsed >= atPos) ? mData->strBuffer + atPos : nullptr);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::SString<CharType> & TEString<CharType, Helper>::getDataString( void )
{
    return (isValid( ) ? *mData : *NEString::getInvalidString<CharType>( ));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline const NEString::SString<CharType> & TEString<CharType, Helper>::getDataString( void ) const
{
    return (isValid( ) ? *mData : *NEString::getInvalidString<CharType>( ));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::reallocate( NEString::CharCount charsAdd )
{
    NEString::SString<CharType> * tmp = NEString::reallocSpace<CharType, CharType>( *mData, charsAdd );
    if ( NEString::isValid<CharType>(tmp))
    {
        NEString::releaseSpace<CharType>( mData );
        mData   = tmp;

#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::canRead( NEString::CharPos atPos ) const
{
    return (isValidPtr() && NEString::canRead<CharType>(*mData, atPos));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::canWrite( NEString::CharPos atPos ) const
{
    return (isValidPtr() && NEString::canWrite<CharType>( *mData, atPos ));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::release( void )
{
    NEString::releaseSpace<CharType>( mData );
    mData   = NEString::getInvalidString<CharType>( );

#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( void )
    : mData     ( NEString::initString<CharType>(static_cast<NEString::CharCount>(0) ) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( CharType ch )
    : mData     ( NEString::initString<CharType, CharType>(&ch, 1) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( const CharType * strSource )
    : mData     ( NEString::initString<CharType, CharType>(strSource, NEString::COUNT_ALL) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( const CharType * strSource, NEString::CharCount charCount )
    : mData     ( NEString::initString<CharType, CharType>(strSource, charCount) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( const NEString::SString<CharType> & strSource )
    : mData     ( NEString::initString<CharType, CharType>(strSource) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( const TEString<CharType, Helper> & strSource )
    : mData     ( NEString::initString<CharType, CharType>( strSource.getDataString() ) )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline TEString<CharType, Helper>::TEString( TEString<CharType, Helper> && strSource )
    : mData     ( strSource.mData )
    , mHelper   ( )
#ifdef DEBUG
    , mString   ( mData->strBuffer )
#endif // DEBUG
{
    strSource.mData     = nullptr;
#ifdef DEBUG
    strSource.mString   = nullptr;
#endif // DEBUG

}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
TEString<CharType, Helper>::~TEString( void )
{
    NEString::releaseSpace<CharType>(mData);
    mData = NEString::getInvalidString<CharType>();
#ifdef DEBUG
    mString = mData->strBuffer;
#endif // DEBUG
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::findOneOf( const CharType * chars, NEString::CharPos startPos /*= NEString::START_POS*/ ) const
{
    NEString::CharPos result= NEString::INVALID_POS;
    if ( canRead(startPos) && (chars != nullptr) )
    {
        const CharType * strThis    = getBuffer( startPos );
        for ( ; (result == NEString::INVALID_POS) && (*strThis != NEString::EndOfString); ++strThis, ++ startPos)
        {
            CharType chThis = *strThis;
            for ( const CharType * strOther = chars; (result == NEString::INVALID_POS) && (*strOther != NEString::EndOfString); ++ strOther)
            {
                CharType chOther = *strOther;
                result = chThis == chOther ? startPos : NEString::INVALID_POS;
            }
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::findFirstOf( CharType chSearch, NEString::CharPos startPos /*= NEString::START_POS*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result = NEString::INVALID_POS;

    if ( canRead( startPos ) )
    {
        CharType chUpper = caseSensitive ? chSearch : makeUpper( chSearch );
        CharType chLower = caseSensitive ? chSearch : makeLower( chSearch );
        const CharType * strThis = getBuffer( startPos );
        for ( ; (result == NEString::INVALID_POS) && (*strThis != NEString::EndOfString); ++strThis, ++ startPos )
        {
            result = (*strThis == chUpper) || (*strThis == chLower) ? startPos : NEString::INVALID_POS;
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::findFirstOf( const CharType * phrase, NEString::CharPos startPos /*= NEString::START_POS*/, bool caseSensitive /*= true*/, bool wholeWord /*= false*/ ) const
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( canRead( startPos ) && (phrase != nullptr) )
    {
        NEString::CharCount lenThis = getLength();
        NEString::CharCount lenOther= NEString::getStringLength<CharType>(phrase);
        if ( (lenThis - startPos) >= lenOther )
        {
            ASSERT( lenThis >= lenOther);
            NEString::CharPos endPos= lenThis - lenOther + 1;
            const CharType * buffer = nullptr;
            CharType beginPrev      = static_cast<CharType>(NEString::EndOfString);
            CharType beginCurr      = static_cast<CharType>(NEString::EndOfString);
            CharType endCurr        = static_cast<CharType>(NEString::EndOfString);
            CharType endNext        = static_cast<CharType>(NEString::EndOfString);

            if ( wholeWord )
            {
                buffer      = getChars( startPos );
                beginPrev   = startPos == NEString::START_POS ? static_cast<CharType>(NEString::EndOfString) : *(buffer - 1);
                endNext     = *(buffer + lenOther - 1);
            }

            for ( NEString::CharPos pos = startPos; (result == NEString::INVALID_POS) && (pos < endPos); ++ pos )
            {
                bool isWord = true;
                if ( buffer != nullptr )
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
                if ( isWord && (compareString( pos, phrase, lenOther, caseSensitive ) == NEMath::eCompare::Equal) )
                {
                    result = pos;
                }
            }
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::findLastOf( CharType chSearch, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result = NEString::INVALID_POS;
    NEString::CharCount strLen = getLength( );
    if ( startPos == NEString::END_POS )
    {
        startPos = strLen != 0 ? strLen - 1 : 0;
    }
    if ( canRead( startPos ) )
    {
        CharType chUpper = caseSensitive ? chSearch : makeUpper( chSearch );
        CharType chLower = caseSensitive ? chSearch : makeLower( chSearch );

        const CharType * begin  = getBuffer(NEString::START_POS);
        const CharType * end    = getBuffer(startPos);
        for ( ; (result == NEString::INVALID_POS) && (end >= begin); -- end, -- startPos)
        {
            result = (*end == chUpper) || (*end == chLower) ? startPos : NEString::INVALID_POS;
        }
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::findLastOf( const CharType * phrase, NEString::CharPos startPos /*= NEString::END_POS*/, bool caseSensitive /*= true*/ ) const
{
    NEString::CharPos result    = NEString::INVALID_POS;
    NEString::CharCount count   = NEString::getStringLength<CharType>( phrase );
    NEString::CharCount strLen  = getLength();

    if ( startPos == NEString::END_POS )
    {
        startPos = strLen != 0 ? strLen - 1 : 0;
    }
    if ( canRead(startPos) && (count != 0))
    {
        const CharType * begin  = getBuffer( NEString::START_POS );
        const CharType * end    = getBuffer( startPos );

        for ( NEString::CharPos pos = startPos; (result == NEString::INVALID_POS) && (end >= begin); --end, --pos )
        {
            if ( (compareString( pos, phrase, count, caseSensitive ) == NEMath::eCompare::Equal) )
            {
                result = pos;
            }
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Helper>::compare( const CharType * what, NEString::CharPos startAt /*= NEString::START_POS*/, NEString::CharCount charCount /*= NEString::COUNT_ALL*/, bool caseSensitive /*= true*/) const
{   return compareString( startAt, what, charCount, caseSensitive );   }

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEMath::eCompare TEString<CharType, Helper>::compare(const TEString<CharType, Helper> & other, bool caseSensitive /*= true*/ ) const
{
    return compare(other.getChars(NEString::START_POS), NEString::START_POS, NEString::COUNT_ALL, caseSensitive);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
bool TEString<CharType, Helper>::substring(TEString<CharType, Helper> & outResult, NEString::CharPos startPos /* = NEString::START_POS */, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ ) const
{
    bool result = false;
    outResult.release();
    if ( canRead(startPos) )
    {
        const CharType * buffer     = getBuffer( startPos );
        NEString::CharCount remain  = static_cast<NEString::CharCount>(mData->strUsed) - startPos;
        charCount = MACRO_MIN(charCount, remain);
        result = outResult.copy(buffer, charCount) != 0;
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::substring( TEString<CharType, Helper> & outResult, const CharType * strPhrase, NEString::CharPos startPos /*= NEString::START_POS*/ ) const
{
    NEString::CharPos result= NEString::INVALID_POS;
    const CharType * buffer = getBuffer(startPos);
    const CharType * next   = nullptr;

    NEString::CharPos pos   = NEString::findFirstOf<CharType>(strPhrase, buffer, NEString::START_POS, &next);
    NEString::CharPos nextPos= next != nullptr ? startPos + static_cast<NEString::CharPos>(next - buffer) : NEString::INVALID_POS;
    outResult.copy( buffer, pos != NEString::INVALID_POS ? pos : NEString::COUNT_ALL );

    if ( nextPos < getLength() )
        result = nextPos;
    else if ( nextPos == getLength() )
        result = NEString::END_POS;
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::substring( TEString<CharType, Helper> & outResult, CharType chSymbol, NEString::CharPos startPos /* = NEString::START_POS */ ) const
{
    NEString::CharPos result= NEString::INVALID_POS;
    const CharType * buffer = getBuffer(startPos);
    const CharType * next   = nullptr;

    NEString::CharPos pos = NEString::findFirstOf<CharType>(chSymbol, buffer, NEString::START_POS, &next);
    NEString::CharPos nextPos = next != nullptr ? startPos + static_cast<NEString::CharPos>(next - buffer) : NEString::INVALID_POS;
    outResult.copy(buffer, pos != NEString::INVALID_POS ? pos : NEString::COUNT_ALL);

    if ( nextPos < getLength( ) )
        result = nextPos;
    else if ( nextPos == getLength( ) )
        result = NEString::END_POS;
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isNumeric( bool signIgnore /*= true*/ ) const
{
    bool result = false;
    if ( isValid() )
    {
        result = true;
        const CharType * src = getChars( NEString::START_POS );
        if ( signIgnore && ( (*src == '-') || (*src == '+') ))
            ++src; // Escape sign, if it should.

        for ( ; result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = isNumeric( *src );
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isAlphanumeric( void ) const
{
    bool result = false;
    if ( isValid() )
    {
        result = true;
        for (const CharType * src = getChars( NEString::START_POS ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = isAlphanumeric(*src);
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isValidName( void ) const
{
    bool result = false;
    if ( isValid( ) )
    {
        result = true;
        for ( const CharType * src = getChars( NEString::START_POS ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
        {
            if ( isAlphanumeric( *src ) == false )
                result = (*src == '-') || (*src == '_') || (*src == '.');
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::validate( const CharType * validityList ) const
{
    bool result = false;
    if ( isValid( ) && (NEString::isEmpty<CharType>(validityList) == false) )
    {
        result = true;
        for ( const CharType * src = getChars( NEString::START_POS ); result && (*src != static_cast<CharType>(NEString::EndOfString)); ++ src )
            result = NEString::findFirstOf<CharType>(*src, validityList) != NEString::INVALID_POS;
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::truncate( NEString::CharCount maxChars )
{
    if ( maxChars == 0 )
    {
        release();
    }
    else
    {
        NEString::CharCount spaceNeed = static_cast<NEString::CharCount>(NEString::geStringRequiredSize<CharType>(maxChars) / sizeof(CharType));
        if ( spaceNeed < getActualLength() )
        {
            ASSERT( isValidPtr() );
            NEString::SString<CharType> * tmp = NEString::initString<CharType>(getString(), maxChars);
            if (tmp != nullptr)
            {
                NEString::releaseSpace<CharType>(mData);
                mData   = tmp;

#ifdef DEBUG
                mString = mData->strBuffer;
#endif // DEBUG
            }
        }
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::remove( NEString::CharPos startPos, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
{
    if ( canRead( startPos ) )
    {
        NEString::CharCount len    = getLength();
        NEString::CharCount remain = len - startPos;
        charCount = MACRO_MIN(charCount, remain);

        move( startPos + charCount, -1 * static_cast<int>(charCount) );
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::compact( void )
{
    if ( isValid() )
    {
        NEString::CharCount len   = getLength();
        unsigned int single = sizeof(CharType);
        unsigned int space  = NEString::geStringRequiredSize<CharType>( len );
        NEString::CharCount charCount = static_cast<NEString::CharCount>(space / single);

        if ( charCount < getActualLength() )
        {
            NEString::SString<CharType> * tmp = NEString::initString<CharType>( getString( ), len );
            if ( tmp != nullptr )
            {
                NEString::releaseSpace<CharType>( mData );
                mData   = tmp;

#ifdef DEBUG
                mString = mData->strBuffer;
#endif // DEBUG
            }
        }
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharCount TEString<CharType, Helper>::copy(const CharType * strSource, NEString::CharCount charCount /*= NEString::COUNT_ALL */)
{
    NEString::CharCount result = 0;

    charCount = charCount == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strSource) : charCount;
    resize(charCount);
    if ( isValid() )
    {
        result = NEString::copyString<CharType, CharType>(*mData, strSource, NEString::START_POS, charCount);
#ifdef DEBUG
        mString = mData->strBuffer;
#endif // DEBUG
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharCount  TEString<CharType, Helper>::append(const CharType * strSource, NEString::CharCount charCount /*= NEString::COUNT_ALL*/)
{
    NEString::CharCount result = 0;
    charCount = charCount == NEString::COUNT_ALL ? NEString::getStringLength<CharType>(strSource) : charCount;
    resize( getLength() + charCount);
    if ( isValid( ) )
    {
        result = NEString::appendString<CharType, CharType>(*mData, strSource, charCount);
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharCount TEString<CharType, Helper>::move( NEString::CharPos startPos, int moveTo )
{
    NEString::CharCount result = 0;
    if ( moveTo < 0 )
    {
        NEString::CharCount cnt = static_cast<NEString::CharCount>( -1 * moveTo);
        NEString::CharPos dstPos= startPos >= cnt ? startPos + moveTo : NEString::START_POS;
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

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::swap( NEString::CharPos startPos /*= NEString::START_POS*/, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
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

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::insertAt( CharType chSrc, NEString::CharPos atPos )
{
    if ( move(atPos, 1) != 0 )
        mData->strBuffer[atPos] = chSrc;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::insertAt( const CharType * strSrc, NEString::CharPos atPos, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
{
    if ( strSrc != nullptr )
    {
        NEString::CharCount eos = getLength( );
        NEString::CharCount len = NEString::getStringLength( strSrc );
        atPos = atPos == NEString::END_POS ? eos : atPos;
        charCount = MACRO_MIN( len, charCount );
        if ( move( atPos, charCount ) != 0 )
        {
            CharType * dst = getChars( atPos );
            while ( *strSrc != static_cast<CharType>(NEString::EndOfString) )
                *dst ++ = *strSrc ++;
            if ( atPos == eos )
                *dst = static_cast<CharType>(NEString::EndOfString);
        }
#ifdef DEBUG
        mString = mData != nullptr ? mData->strBuffer : nullptr;
#endif // DEBUG
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
void TEString<CharType, Helper>::insertAt( const TEString<CharType, Helper> & strSrc, NEString::CharPos atPos, NEString::CharCount charCount /*= NEString::COUNT_ALL*/ )
{
    insertAt( strSrc.getString(), atPos, charCount);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::replace( CharType chOrigin, CharType chReplace, NEString::CharPos startPos /*= NEString::START_POS*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( canRead(startPos) )
    {
        CharType * buffer     = getChars( NEString::START_POS );
        NEString::CharPos len = getLength();
        NEString::CharPos pos = startPos;
        for ( ; (result == NEString::INVALID_POS) && (pos < len); ++ pos )
        {
            if ( buffer[pos] == chOrigin )
            {
                buffer[pos] = chReplace;
                if ( replaceAll == false )
                    result = pos + 1;
            }
        }
        result = (result == NEString::INVALID_POS) || (result == len) ? NEString::END_POS : result;
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::replace( const CharType * strOrigin, const CharType * strReplace, NEString::CharPos startPos /*= NEString::START_POS*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::INVALID_POS;
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
                if ( result != NEString::INVALID_POS )
                    replacedOnce = true;
            } while ( replaceAll && (result != NEString::INVALID_POS) );

            result = ( (result == NEString::INVALID_POS) && replacedOnce ? NEString::END_POS : result );
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::replace( const TEString<CharType, Helper> & strOrigin, const TEString<CharType, Helper> & strReplace, NEString::CharPos startPos /*= NEString::START_POS*/, bool replaceAll /*= true*/ )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenOrigin  = strOrigin.getLength();
        NEString::CharPos lenReplace = strReplace.getLength();
        if ( (lenOrigin != 0) && (lenReplace != 0) )
        {
            bool replacedOnce = false;
            const CharType * bufOrigin = strOrigin.getString();
            const CharType * bufReplace= strReplace.getString();
            ASSERT( (bufOrigin != nullptr) && (bufReplace != nullptr) );
    
            do
            {
                result = replaceWith( bufOrigin, lenOrigin, bufReplace, lenReplace, startPos );
                startPos = result;
                if ( result != NEString::INVALID_POS )
                    replacedOnce = true;
            } while ( replaceAll && (result != NEString::INVALID_POS) );

            result = ((result == NEString::INVALID_POS) && replacedOnce ? NEString::END_POS : result);
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::replace( const CharType * strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenReplace = NEString::getStringLength<CharType>(strReplace);
        if ( (charsRemove != 0) && (lenReplace != 0) )
        {
            result = replaceWith(startPos, charsRemove, strReplace, lenReplace);
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
NEString::CharPos TEString<CharType, Helper>::replace( const TEString<CharType, Helper> & strReplace, NEString::CharPos startPos, NEString::CharCount charsRemove )
{
    NEString::CharPos result = NEString::INVALID_POS;
    if ( canRead( startPos ) )
    {
        NEString::CharPos lenReplace = strReplace.getLength();
        if ( (charsRemove != 0) && (lenReplace != 0) )
        {
            result = replaceWith( startPos, charsRemove, strReplace.getString(), lenReplace );
        }
    }
    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharPos TEString<CharType, Helper>::remove(const CharType * strRemove, NEString::CharPos startPos /*= NEString::START_POS*/, bool removeAll /*= true*/)
{
    return replace(strRemove, &EmptyChar, startPos, removeAll);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isEmpty( void ) const
{
    return ( (isValid() == false) || (mData->strUsed == 0) );
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::isValid( void ) const
{
    return NEString::isValid<CharType>(mData);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharCount TEString<CharType, Helper>::getLength( void ) const
{
    return (isValid() ? static_cast<NEString::CharCount>(mData->strUsed) : 0);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharCount TEString<CharType, Helper>::getActualLength( void ) const
{
    return (isValid() ? mData->strSpace : static_cast<NEString::CharCount>(0));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline unsigned int TEString<CharType, Helper>::getUsedSpace(void) const
{
    return (isValid() ? static_cast<unsigned int>(mData->strUsed + 1) * sizeof(CharType) : 0);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::clear( void )
{
    release();
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Helper>::getBuffer( NEString::CharPos startAt /*= NEString::START_POS*/ ) const
{
    return getChars(startAt);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline const CharType * TEString<CharType, Helper>::getString( void ) const
{
    return static_cast<const CharType *>(isValid() ? mData->strBuffer : nullptr);
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType TEString<CharType, Helper>::getAt( NEString::CharPos atPos ) const
{
    return (canRead(atPos) ? mData->strBuffer[atPos] : static_cast<CharType>(NEString::EndOfString));
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline void TEString<CharType, Helper>::setAt( CharType ch, NEString::CharPos atPos /*= NEString::END_POS*/ )
{
    if ( isValidPtr() )
    {
        NEString::setAt<CharType, CharType>( *mData, ch, atPos );
    }
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline bool TEString<CharType, Helper>::resize( NEString::CharCount maxSpace )
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

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimLeft( void )
{
    int result = 0;
    if (isEmpty() == false)
    {
        result = NEString::trimLeft<CharType>( getChars(NEString::START_POS), getLength() );
        ASSERT(result <= static_cast<int>(mData->strUsed));
        mData->strUsed = result;
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimLeft(TEString<CharType, Helper> & OUT strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        NEString::CharCount len = getLength();
        const CharType * begin  = getString();
        const CharType * end    = begin + len;
        while (isWhitespace(*begin))
            ++ begin;

        ASSERT(begin <= end);
        NEString::CharCount remain = static_cast<NEString::CharCount>(end - begin);
        if (remain > 0)
        {
            result = static_cast<int>(strResult.copy(begin, remain));
        }
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimRight( void )
{
    int result = 0;
    if ( isEmpty() == false )
    {
        result = NEString::trimRight<CharType>( getChars(NEString::START_POS), getLength() );
        ASSERT(result <= static_cast<int>(mData->strUsed));
        mData->strUsed = result;
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimRight(TEString<CharType, Helper> & OUT strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        NEString::CharCount len = getLength();
        const CharType * begin  = getString();
        const CharType * end    = begin + len - 1;

        while ((end > begin) && isWhitespace(*end) )
            -- end;

        ++ end;

        NEString::CharCount remain = static_cast<NEString::CharCount>(end - begin);
        if (remain > 0)
        {
            result = strResult.copy(begin, remain);
        }
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimAll( void )
{
    int result = 0;
    if ( isEmpty() == false )
    {
        result = NEString::trimAll<CharType>(getChars(NEString::START_POS), getLength());
        ASSERT(result <= mData->strUsed);
        mData->strUsed = result;
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline int TEString<CharType, Helper>::trimAll(TEString<CharType, Helper> & strResult) const
{
    int result = 0;
    strResult.clear();
    if (isEmpty() == false)
    {
        NEString::CharCount len = getLength();
        const CharType * begin  = getString();
        const CharType * end    = begin + len - 1;

        while ((end > begin) && isWhitespace(*end))
            -- end;

        ++ end;

        while ((begin < end) && isWhitespace(*begin))
            ++ begin;

        NEString::CharCount remain = static_cast<NEString::CharCount>(end - begin);
        if (remain > 0)
        {
            result = strResult.copy(begin, remain);
        }
    }

    return result;
}

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline NEString::CharCount TEString<CharType, Helper>::makeAlphanumeric(void)
{
    NEString::CharCount result = 0;
    if (isEmpty() == false)
    {
        CharType * begin= getChars( NEString::START_POS);
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

template<typename CharType, class Helper /*= TEStringImpl<CharType>*/>
inline CharType * TEString<CharType, Helper>::getUnsafeBuffer(void)
{
    return (isEmpty() == false ? mData->strBuffer : nullptr);
}
