#ifndef AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
#define AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/console/OptionParser.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"

#include <vector>
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// OptionParser class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is parsing command line or an option string separated by space
 *          and validates the options:
 *              - by option short / long name,
 *              - by option value range,
 *              - by syntax (quote).
 *          The developers need to setup the options and pass to constructor. If constructor
 *          has no setups, it uses default setup, which is just separating the options as strings.
 *          The default option setup is setup by calling static method `OptionParser::getDefaultOptionSetup()`.
 *          Note, the default option setup entry must be the first entry in the list.
 *          The option setup includes:
 *              1. Short name of the option, for example "-h";
 *              2. Long name of the option, for example "--help";
 *              3. The unique digital representation / command (or unique ID) of the option, for example "10";
 *              4. The validity fields, these are flags indicating which values should be used;
 *              5. The range of values as signed integers (the "integer with range" flag should be valid);
 *              6. The range of values as signed floating point (the "floating point with range" flag should be valid);
 *              7. The list of strings (the "string with range" flag should be valid).
 *          If the "range" flag is present, the parser checks whether the input value is matching the range or not.
 *          If it does not match, it sets the error flag in the parsed input option. In case of strings, the range
 *          is checked by comparing match of string in the list. For example, if the possible values of the
 *          "string with range" are "Debug" or "Release", the parser indicates an error if string is "Blah".
 *          When options are parsed, they are located in the list, the parsed options have following information:
 *              1. The unique digital representation / command (or unique ID) of the option, for example "10";
 *              2. The validity fields, these are flags indicating which values should be used;
 *              3. The digit value of the options (signed integer or signed digit with floating point), depending on the flag in the field.
 *              4. The list of the strings in case if option can have multiple strings.
 *          The options are passed as list of command line strings or as a string, where each options is separated by space.
 *          Indifferent whether the option string is passed as `char` or `wchar_t` type, they should be ASCII strings.
 *          If the value of an option contains a space, it should be included in the quote like "\"some value\"".
 *          The values in the quote are treated as a single string. The quotes are cleared in the parsed options.
 *          For example, if the option value is quoted as "\"some value\"", the parsed string contains "some value".
 * 
 *          These are following possibilities to set flags in the option setup:
 *              1. INTEGER_NO_RANGE -- the value of the option is any signed integer;
 *              2. FLOAT_NO_RANGE   -- the value of the option is any signed digit with floating point;
 *              3. STRING_NO_RANGE  -- the value of the option is any string or list of strings;
 *              4. INTEGER_IN_RANGE -- the value of the option is a signed integer in the specified range (the range should be set);
 *              5. FLOAT_IN_RANGE   -- the value of the option is a signed digit with floating point in the specified range (the range should be set);
 *              6. STRING_IN_RANGE  -- the value of the option is a string or list of string of the specified list of string (the list should be set);
 *              7. FREESTYLE_DATA   -- the value of the option is any string, same as STRING_NO_RANGE.
 * 
 *          If parsing an option fails, for example, if the value is out of range or the closing quote ("\"") of the string is missed,
 *          the `inField` field of the sOption contains error flag, which can be checked by static method `hasInputError(uint32_t flags)`.
 *
 *          Example:   These are examples of setting options:
 *              1. Command line simple options: '-a', '-b', '-c'
 *                  myapp -a -b -c
 *              2. Command line options with values, set by '=': '-a' = {1}, '-b' = {2.3} -c = {"something"}
 *                  myapp -a=1 -b=2.3 -c=something
 *              3. Command line options with values, set by ' ': '-a' = {1}, '-b' = {2.3} '-c' = {"some", "thing"}
 *                  myapp -a 1 -b 2.3 -c some thing
 *              4. Command line options with short and long names: '-a', '-bc', '-def', '--ghijk-lmop'
 *                  myapp -a -bc -def --ghijk-lmop
 *              5. Command line options with value set mixed:  '-a' = {1}, '-bc' = {2.3}, '-def' =  {"some"}, '--ghijk-lmop' = {"thing" "and" "somethingelse"}
 *                  myapp -a=1 -bc = 2.3 -def some --ghijk-lmop thing and somethingelse
 *              6. Command line options with free texts: '-scope' = {"*.areg_* = DEBUG | SCOPE"}
 *                  myapp -scope "*.areg_* = DEBUG | SCOPE"
 *              7. Command line error option (wrong / not closed quotes): '-scope' = {"\"*.areg_* = DEBUG | SCOPE -a = 5"}
 *                  myapp -scope "\"*.areg_* = DEBUG | SCOPE  -a = 5"
 *              8. Option as a string separated by space: 'copy' = {"./file.txt" , "./"}
 *                  copy ./file.txt ./some/
 *              9. Options as a string separated by space: '--width' = {123}, '--height' == {456}
 *                  --width=123 --height=456
 * 
 *              This is an example of setting up the option validation:
 *              const OptionParser::sOptionSetup setup[]
 *              {
 *                    {"-c", "--copy"  , 1, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *                  , {"-m", "--move"  , 2, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *                  , {"-r", "--remove", 3, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *              };
 *              Depending on the option, the field `inCommand` of the `sOption` structure indicates the command.
 *              For example, if the option is "-m ./file.txt ./folder/", the `inCommand` value will be 2 and the
 *              `inString` value contains 2 string entries: "./file.txt" and "./folder/".
 * 
 *              If option value is restricted to a specific range, developers should specify them in the setup.
 *              The following is an example of the option with range:
 *              const OptionParser::sOptionSetup setup[]
 *              {
 *                    {"-j", "--job"   , 1, OptionParser::INTEGER_IN_RANGE, {1, 32}, {}, {}}
 *                  , {"-c", "--config", 2, OptionParser::STRING_IN_RANGE , {     }, {}, {"Debug", "Release"}}
 *              };
 * 
 *              1. The parser for option like "-j 10 --config=debug" contains 2 valid entries: 
 *                      a. {1, OptionParser::INTEGER_IN_RANGE, 0, 10, ""}
 *                      b. {1, OptionParser::INTEGER_IN_RANGE, 0, 0 , "debug"}
 *                 Note, the string values to check are not case-sensitive, means "Debug", "DEBUG" and "debug" are equal.
 * 
 *              2. The parse for option like "-j 10 --config=Any" contains 2 entries, where the second entry has error flag.
 *              3. The parse for option like "-j 100 --config=Debug" contains 1 error entry.
 * 
 *              If you need to pass a complex object, for example a `rectangle` figure, better to specify as a string
 *              with values then convert to the object. As an example, let's assume your application expects geometry figures
 *              like 'rectangle' and 'triangle', where rectangle has pattern ({x, y}, width, height) and
 *              the triangle is {{x, y}, {x, y}, {x, y}}. In this case the option setup can be done:
 * 
 *              enum Figure
 *              {
 *                    Triangle  = 1
 *                  , Rectangle = 2
 *              };
 * 
 *              const OptionParser::sOptionSetup setup[]
 *              {
 *                    {"-t", "--tri"   , static_cast<int>(Figure::Triangle) , OptionParser::STRING_NO_RANGE, {}, {}, {}}
 *                  , {"-r", "--rect"  , static_cast<int>(Figure::Rectangle), OptionParser::STRING_NO_RANGE, {}, {}, {}}
 *              };
 *              And the option string can be: '-t "{1,1}, {5, 10}, {20, 3}" --rect "{1, 10}, 5, 3"'
 *              This is parsed to create 2 sOption object with one string in each, where `inCommand` indicates
 *              '1' for triangle with string '{1,1}, {5, 10}, {20, 3}' and '2' with string '{1, 10}, 5, 3'
 *              Then you may parse the strings to set the data of the objects.
 *              Note, that since the string contains string, they should be included in the quote.
 *              If you don't specify quotes, for example, if the option command is '-t {1,1} {5, 10} {20, 3} --rect {1, 10} 5 3'
 *              then after parsing you still have 2 option entries with 3 strings in each, where
 *                  a. the triangle with `inCommand = 1` has entries "{1,1}", "{5, 10}", "{20, 3}"
 *                  b. the rectangle with `inCommand = 2` has entries "{1, 10}", "5", "3".
 *              Which one of the approach to chose is up to developer.
 **/
class OptionParser
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   The flags of options
     **/
    enum class eValidFlags : unsigned int
    {
          ValidationInvalid     = 0     //!< Invalid option,         Bits: 0000 0000
        , ValidationError       = 1     //!< The option with error,  Bits: 0000 0001
        , ValidationInRange     = 2     //!< The option has range,   Bits: 0000 0010
        , ValidationNoData      = 16    //!< The option has no data, Bits: 0001 0000
        , ValidationInteger     = 32    //!< The option is integer,  Bits: 0010 0000
        , ValidationFloat       = 64    //!< The option is float,    Bits: 0100 0000
        , ValidationString      = 128   //!< The option is string,   Bits: 1000 0000
    };

    /**
     * \brief   The Option setup or option has no data.
     **/
    static constexpr uint32_t   NO_DATA         { static_cast<uint32_t>(eValidFlags::ValidationNoData) };

    /**
     * \brief   The Option setup or option is any integer with sign.
     **/
    static constexpr uint32_t   INTEGER_NO_RANGE{ static_cast<uint32_t>(eValidFlags::ValidationInteger)};

    /**
     * \brief   The Option setup or option is any signed digit with floating point.
     **/
    static constexpr uint32_t   FLOAT_NO_RANGE  { static_cast<uint32_t>(eValidFlags::ValidationFloat) };

    /**
     * \brief   The Option setup or option is any string or list of strings.
     **/
    static constexpr uint32_t   STRING_NO_RANGE { static_cast<uint32_t>(eValidFlags::ValidationString) };

    /**
     * \brief   The Option setup or option is an integer with sign, which should be in the range.
     **/
    static constexpr uint32_t   INTEGER_IN_RANGE{ INTEGER_NO_RANGE | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    /**
     * \brief   The Option setup or option is a signed digit with floating point, which should be in the range.
     **/
    static constexpr uint32_t   FLOAT_IN_RANGE  { FLOAT_NO_RANGE   | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    /**
     * \brief   The Option setup or option is a string or list of string that should match of the possible entry.
     *          The strings are not case-sensitive.
     **/
    static constexpr uint32_t   STRING_IN_RANGE { STRING_NO_RANGE  | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    /**
     * \brief   The Option setup or option is any string, same as STRING_NO_RANGE.
     **/
    static constexpr uint32_t   FREESTYLE_DATA  { STRING_NO_RANGE  };

    /**
     * \brief   The structure to specify range.
     **/
    template<typename VALUE>
    struct Range
    {
        VALUE   valMin; //!< The minimum value of the range
        VALUE   valMax; //!< The maximum value of the range
    };

    /**
     * \brief   The union that specifies either integer or a digit with floating point.
     **/
    union uValues
    {
        int     valInt;     //!< Signed integer
        float   valFloat;   //!< Signed floating point
    };

    //!< Fixed list of strings
    typedef std::vector<std::string_view> FixedStrList;

    //!< List of strings
    typedef std::vector<String> StrList;

    /**
     * \brief   The entry to setup options for validation.
     *          It should be initialized in the list to pass in the constructor before
     *          parsing the options.
     **/
    struct sOptionSetup
    {
        std::string_view    optShort        {        }; //!< The short name of the option.
        std::string_view    optLong         {        }; //!< The long name of the option.
        int                 optCmmand       { -1     }; //!< The digital value of the command.
        uint32_t            optField        { 0      }; //!< The flag indicating the valid fields.
        Range<int>          optRangeInt     { 0, 0   }; //!< Range of valid integer values, ignored if no range.
        Range<float>        optRangeFloat   { 0., 0. }; //!< Range of valid floating point values, ignored if no range
        FixedStrList        optRangeStrings {        }; //!< Range of valid strings, no case sensitive
    };

    /**
     * \brief   The input option entry
     **/
    struct sOption
    {
        int         inCommand   { -1 }; //!< The digital value of the command.
        uint32_t    inField     { 0  }; //!< The flag indicating the valid fields and values.
        int         inRefSetup  { -1 }; //!< The reference index in the list of options setup, -1 is invalid index
        uValues     inValue     { 0  }; //!< Either integer or float value, depending on `inField`.
        StrList     inString    {    }; //!< The list of the strings, the `inField` should indicate string value
    };

    //!< The list of option validity setup.
    using OptionSetupList   = TEArrayList<sOptionSetup>;
    //!< The list of input options.
    using InputOptionList   = TEArrayList<sOption>;

private:
    //!< Delimiter for "equal"
    static constexpr std::string_view    DELIMITER_EQUAL { "=" };
    //!< Space delimiter
    static constexpr std::string_view    DELIMITER_SPACE { " " };
    //!< The quote of string
    static constexpr std::string_view    DELIMITER_STRING{ "\"" };

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns true if the flag has error field.
     **/
    static inline bool hasInputError( uint32_t flags );

    /**
     * \brief   Returns true if the flag has value in range flag
     **/
    static inline bool hasRange( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that option may have data.
     **/
    static inline bool isEmptyData( uint32_t flags );

    /**
     * \brief   Returns true if the flag as field of free-style string
     **/
    static inline bool isFreestyle( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is integer
     **/
    static inline bool isInteger( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is floating point
     **/
    static inline bool isFloat( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is string or list of string
     **/
    static inline bool isString( uint32_t flags );

    /**
     * \brief   Returns the option default validation entry.
     *          If using, set as a first entry in the setup list.
     **/
    static const OptionParser::sOptionSetup getDefaultOptionSetup( void );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets the option default validation entry,
     *          so that all entries are strings.
     **/
    OptionParser( void );

    /**
     * \brief   Copies the option validation setup from the given list.
     *          If empty, creates sets the option default validation entry.
     **/
    explicit OptionParser( const OptionSetupList & initList );

    /**
     * \brief   Copies the option validation setup from the given list.
     *          If empty, creates sets the option default validation entry.
     **/
    explicit OptionParser( const std::vector<sOptionSetup> & initList );

    /**
     * \brief   Copies the option validation setup from the given list.
     *          If empty, creates sets the option default validation entry.
     **/
    explicit OptionParser( std::vector<sOptionSetup> && initList ) noexcept;

    /**
     * \brief   Copies the option validation setup from the given list.
     *          If empty, creates sets the option default validation entry.
     * \param   initEntries     The list of entries.
     * \param   count           The number of entries in the list.
     **/
    OptionParser( const sOptionSetup * initEntries, uint32_t count );

    /**
     * \brief   Copies the option validation setup from the given source.
     **/
    OptionParser( const OptionParser & src );

    /**
     * \brief   Moves the option validation setup from the given source.
     **/
    OptionParser( OptionParser && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~OptionParser( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns entries from the given source
     **/
    OptionParser & operator = ( const OptionParser & src );

    /**
     * \brief   Moves entries from the given source
     **/
    OptionParser & operator = ( OptionParser && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Parses the string passed as a command line. Because of this:
     *           1. the first entry is ignored, assuming it is a binary name;
     *           2. the parsing completes either when reach 'count' elements or
     *              if any entry of 'cmdLine' is an empty string, assuming that
     *              command line options may end with empty string like "".
     * \param   cmdLine The list of the strings passed as a command line in the binary
     *                  same as 'argv' parameter in 'main' function.
     * \param   count   The number of strings in the 'cmdLine' list.
     * \return  Returns true if succeeded to parse without error.
     *          Otherwise, returns false.
     **/
    bool parseCommandLine( const char ** cmdLine,  uint32_t count);
    bool parseCommandLine( const wchar_t ** cmdLine, uint32_t count );
    bool parseCommandLine( char** IN cmdLine, uint32_t count);
    bool parseCommandLine( wchar_t** IN cmdLine, uint32_t count);

    /**
     * \brief   Parses the string passed as an options separated by space ' '.
     *          Useful when wait for input from user and need to parse the input.
     * \param   optLine The options passed in the string. If a value can be a string with the space
     *                  the string should be quoted "\"like this\"".
     * \return  Returns true if succeeded to parse without error.
     *          Otherwise, returns false.
     **/
    bool parseOptionLine( const char * optLine );
    bool parseOptionLine( const wchar_t * optLine );

    /**
     * \brief   Parses the list of strings. Each entry is either an option or a value.
     *          The value(s) should follow the option entry in the list.
     *          For example, if first entry is an option "-a" and it may have value,
     *          then the values should immediately follow this option entry before the next option.
     * \param   optList The list of options and value.
     * \return  Returns true if succeeded to parse without error.
     *          Otherwise, returns false.
     **/
    bool parseOptions( StrList & optList );

    /**
     * \ brief  Returns the list of parsed options.
     **/
    inline const InputOptionList & getOptions( void ) const;

    /**
     * \brief   Returns the input string, which was parsed.
     **/
    inline const String & getInput(void) const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Creates and sets up input option structure based on the validity index.
     * \param   isShort     True, if matches the short name of the option. Otherwise, should be false.
     * \param   cmdLine     The string that contains an option. With the option, the string may contain value
     *                      if the value is separated with the '=' equal sign.
     *                      Otherwise, it should contain only the option string.
     * \param   refSetup    The index of option validation entries that had matching.
     **/
    OptionParser::sOption _setupInput( bool isShort, String cmdLine, uint32_t refSetup );

    /**
     * \brief   Called to set value in the option. Depending on the flags and the validation range
     *          the 'inFlag' may contain error flag.
     * \param   newValue    The value as a string to set.
     * \param   opt         The option to set the value.
     * \param   refSetup    The index in the option validation setup to look for validation.
     **/
    void _setInputValue( String & newValue, sOption & opt, uint32_t refSetup );

    /**
     * \brief   Sets the integer value in the option. If needed, checks the validation in the range.
     **/
    void _setValue( int newValue, sOption & opt, const sOptionSetup & valid );

    /**
     * \brief   Sets the float value in the option. If needed, checks the validation in the range.
     **/
    void _setValue( float newValue, sOption & opt, const sOptionSetup & valid );

    /**
     * \brief   Sets the string value in the option. If needed, checks the validation in the range.
     **/
    void _setValue( const String & newValue, sOption & opt, const sOptionSetup & valid );

    /**
     * \brief   Checks the matching of the option. The option should not be
     *          following of any alpha-numeric symbol. The valid symbols are:
     *              a. End-of-string ('\0');
     *              b. Equal symbol ('=')
     *              c. Space symbol (' ')
     * \param   input   The option input string to check
     * \param   optCmd  The command of option to match.
     * \return  Returns true if `input` started with `optCmd`.
     **/
    bool _matchOption( const String & input, const String & optCmd ) const;

    /**
     * \brief   Cleans the string from the quotes ('\"').
     *          If a string has opening quote, it should have closing quote.
     *          Otherwise, the function returns false and 'data' should be considered
     *          as a wrong input.
     **/
    bool _cleanQuote( String & data ) const;

//////////////////////////////////////////////////////////////////////////
// Hidden members
//////////////////////////////////////////////////////////////////////////
private:
    //!< The accumulative command line as a string separated by space.
    String          mCmdLine;
    //!< The list of option validation setup
    OptionSetupList mSetupOptions;
    //!< The list of parsed options
    InputOptionList mInputOptions;
};

//////////////////////////////////////////////////////////////////////////
// OptionParser inline methods
//////////////////////////////////////////////////////////////////////////

inline bool OptionParser::hasInputError( uint32_t flags  )
{
    return (flags & static_cast<uint32_t>(eValidFlags::ValidationError)) != 0;
}

inline bool OptionParser::hasRange( uint32_t flags  )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationInRange)) != 0);
}

inline bool OptionParser::isEmptyData( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationNoData)) != 0);
}

inline bool OptionParser::isFreestyle( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationString))  != 0) &&
            ((flags & static_cast<uint32_t>(eValidFlags::ValidationInRange)) == 0);
}

inline bool OptionParser::isInteger( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationInteger)) != 0);
}

inline bool OptionParser::isFloat( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationFloat)) != 0);
}

inline bool OptionParser::isString( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationString)) != 0);
}

inline const OptionParser::InputOptionList & OptionParser::getOptions( void ) const
{
    return mInputOptions;
}

inline const String & OptionParser::getInput(void) const
{
    return mCmdLine;
}

#endif  // AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
