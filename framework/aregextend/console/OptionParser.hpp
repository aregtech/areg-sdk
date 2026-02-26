#ifndef AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
#define AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/OptionParser.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "areg/base/CommonDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/ArrayList.hpp"

#include <vector>
#include <string_view>

//////////////////////////////////////////////////////////////////////////
// OptionParser class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   This class is parsing command line or an option string separated by space
 *          and validates the options:
 *              - by option int16_t / long name,
 *              - by option value range,
 *              - by syntax (quote).
 *          The developers need to setup the options and pass to constructor. If constructor
 *          has no setups, it uses default setup, which is just separating the options as strings.
 *          The default option setup is setup by calling static method `OptionParser::default_option_setup()`.
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
 *          the `inField` field of the InputOption contains error flag, which can be checked by static method `has_input_error(uint32_t flags)`.
 *
 *          Example:   These are examples of setting options:
 *              1. Command line simple options: '-a', '-b', '-c'
 *                  myapp -a -b -c
 *              2. Command line options with values, set by '=': '-a' = {1}, '-b' = {2.3} -c = {"something"}
 *                  myapp -a=1 -b=2.3 -c=something
 *              3. Command line options with values, set by ' ': '-a' = {1}, '-b' = {2.3} '-c' = {"some", "thing"}
 *                  myapp -a 1 -b 2.3 -c some thing
 *              4. Command line options with int16_t and long names: '-a', '-bc', '-def', '--ghijk-lmop'
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
 *              const OptionParser::OptionSetup setup[]
 *              {
 *                    {"-c", "--copy"  , 1, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *                  , {"-m", "--move"  , 2, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *                  , {"-r", "--remove", 3, OptionParser::FREESTYLE_DATA, {}, {}, {}}
 *              };
 *              Depending on the option, the field `inCommand` of the `InputOption` structure indicates the command.
 *              For example, if the option is "-m ./file.txt ./folder/", the `inCommand` value will be 2 and the
 *              `inString` value contains 2 string entries: "./file.txt" and "./folder/".
 * 
 *              If option value is restricted to a specific range, developers should specify them in the setup.
 *              The following is an example of the option with range:
 *              const OptionParser::OptionSetup setup[]
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
 *              enum class Figure
 *              {
 *                    Triangle  = 1
 *                  , Rectangle = 2
 *              };
 * 
 *              const OptionParser::OptionSetup setup[]
 *              {
 *                    {"-t", "--tri"   , static_cast<int32_t>(Figure::Triangle) , OptionParser::STRING_NO_RANGE, {}, {}, {}}
 *                  , {"-r", "--rect"  , static_cast<int32_t>(Figure::Rectangle), OptionParser::STRING_NO_RANGE, {}, {}, {}}
 *              };
 *              And the option string can be: '-t "{1,1}, {5, 10}, {20, 3}" --rect "{1, 10}, 5, 3"'
 *              This is parsed to create 2 InputOption object with one string in each, where `inCommand` indicates
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
    enum class OptionFlag : uint32_t
    {
          Invalid   = 0     //!< Invalid option,         Bits: 0000 0000
        , Error     = 1     //!< The option with error,  Bits: 0000 0001
        , InRange   = 2     //!< The option has range,   Bits: 0000 0010
        , NoData    = 16    //!< The option has no data, Bits: 0001 0000
        , Integer   = 32    //!< The option is integer,  Bits: 0010 0000
        , Float     = 64    //!< The option is float,    Bits: 0100 0000
        , String    = 128   //!< The option is string,   Bits: 1000 0000
    };

    /**
     * \brief   The Option setup or option has no data.
     **/
    static constexpr uint32_t   NO_DATA         { static_cast<uint32_t>(OptionFlag::NoData) };

    /**
     * \brief   The Option setup or option is any integer with sign.
     **/
    static constexpr uint32_t   INTEGER_NO_RANGE{ static_cast<uint32_t>(OptionFlag::Integer)};

    /**
     * \brief   The Option setup or option is any signed digit with floating point.
     **/
    static constexpr uint32_t   FLOAT_NO_RANGE  { static_cast<uint32_t>(OptionFlag::Float) };

    /**
     * \brief   The Option setup or option is any string or list of strings.
     **/
    static constexpr uint32_t   STRING_NO_RANGE { static_cast<uint32_t>(OptionFlag::String) };

    /**
     * \brief   The Option setup or option is an integer with sign, which should be in the range.
     **/
    static constexpr uint32_t   INTEGER_IN_RANGE{ INTEGER_NO_RANGE | static_cast<uint32_t>(OptionFlag::InRange) };

    /**
     * \brief   The Option setup or option is a signed digit with floating point, which should be in the range.
     **/
    static constexpr uint32_t   FLOAT_IN_RANGE  { FLOAT_NO_RANGE   | static_cast<uint32_t>(OptionFlag::InRange) };

    /**
     * \brief   The Option setup or option is a string or list of string that should match of the possible entry.
     *          The strings are not case-sensitive.
     **/
    static constexpr uint32_t   STRING_IN_RANGE { STRING_NO_RANGE  | static_cast<uint32_t>(OptionFlag::InRange) };

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
    union OptValue
    {
        int32_t valInt;     //!< Signed integer
        float   valFloat;   //!< Signed floating point
    };

    using FixedStrList  = std::vector<std::string_view>;//!< Fixed list of strings
    using StrList       = std::vector<String>;          //!< List of strings

    /**
     * \brief   The entry to setup options for validation.
     *          It should be initialized in the list to pass in the constructor before
     *          parsing the options.
     **/
    struct OptionSetup
    {
        std::string_view    optShort        {        }; //!< The int16_t name of the option.
        std::string_view    optLong         {        }; //!< The long name of the option.
        int32_t             optCmmand       { -1     }; //!< The digital value of the command.
        uint32_t            optField        { 0      }; //!< The flag indicating the valid fields.
        Range<int32_t>      optRangeInt     { 0, 0   }; //!< Range of valid integer values, ignored if no range.
        Range<float>        optRangeFloat   { 0., 0. }; //!< Range of valid floating point values, ignored if no range
        FixedStrList        optRangeStrings {        }; //!< Range of valid strings, no case sensitive
    };

    /**
     * \brief   The input option entry
     **/
    struct InputOption
    {
        int32_t         inCommand   { -1 }; //!< The digital value of the command.
        uint32_t    inField     { 0  }; //!< The flag indicating the valid fields and values.
        int32_t         inRefSetup  { -1 }; //!< The reference index in the list of options setup, -1 is invalid index
        OptValue     inValue     { 0  }; //!< Either integer or float value, depending on `inField`.
        StrList     inString    {    }; //!< The list of the strings, the `inField` should indicate string value
    };

    using OptionSetupList   = ArrayList<OptionSetup>;   //!< The list of option validity setup.
    using InputOptionList   = ArrayList<InputOption>;   //!< The list of input options.

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
    static inline bool has_input_error( uint32_t flags );

    /**
     * \brief   Returns true if the flag has value in range flag.
     **/
    static inline bool has_range( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that option may have data.
     **/
    static inline bool is_empty_data( uint32_t flags );

    /**
     * \brief   Returns true if the flag as field of free-style string.
     **/
    static inline bool is_freestyle( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is integer.
     **/
    static inline bool is_integer( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is floating point.
     **/
    static inline bool is_float( uint32_t flags );

    /**
     * \brief   Returns true if the flag indicates that the valid value is string or list of string.
     **/
    static inline bool is_string( uint32_t flags );

    /**
     * \brief   Returns the option default validation entry. If using, set as a first entry in the
     *          setup list.
     **/
    static const OptionParser::OptionSetup default_option_setup();

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor. Sets the option default validation entry, so that all entries
     *          are strings.
     **/
    OptionParser();

    /**
     * \brief   Copies the option validation setup from the given list. If empty, creates sets the
     *          option default validation entry.
     **/
    explicit OptionParser( const OptionSetupList & initList );

    /**
     * \brief   Copies the option validation setup from the given list. If empty, creates sets the
     *          option default validation entry.
     **/
    explicit OptionParser( const std::vector<OptionSetup> & initList );

    /**
     * \brief   Copies the option validation setup from the given list. If empty, creates sets the
     *          option default validation entry.
     **/
    explicit OptionParser( std::vector<OptionSetup> && initList ) noexcept;

    /**
     * \brief   Copies the option validation setup from the given list. If empty, creates sets the
     *          option default validation entry.
     *
     * \param   initEntries     The list of entries.
     * \param   count           The number of entries in the list.
     **/
    OptionParser( const OptionSetup * initEntries, uint32_t count );

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
    ~OptionParser() = default;

//////////////////////////////////////////////////////////////////////////
// Operators
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Assigns entries from the given source.
     **/
    OptionParser & operator = ( const OptionParser & src );

    /**
     * \brief   Moves entries from the given source.
     **/
    OptionParser & operator = ( OptionParser && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Parses the string passed as a command line. Because of this: 1. the first entry is
     *          ignored, assuming it is a binary name; 2. the parsing completes either when reach
     *          'count' elements or if any entry of 'cmdLine' is an empty string, assuming that
     *          command line options may end with empty string like "".
     *
     * \param   cmdLine     The list of the strings passed as a command line in the binary same as
     *                      'argv' parameter in 'main' function.
     * \param   count       The number of strings in the 'cmdLine' list.
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_command_line( const char ** cmdLine, uint32_t count );
    /**
     * \brief   Parses the string passed as a command line. Because of this: 1. the first entry is
     *          ignored, assuming it is a binary name; 2. the parsing completes either when reach
     *          'count' elements or if any entry of 'cmdLine' is an empty string, assuming that
     *          command line options may end with empty string like "".
     *
     * \param   cmdLine     The list of the strings passed as a command line in the binary same as
     *                      'argv' parameter in 'main' function.
     * \param   count       The number of strings in the 'cmdLine' list.
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_command_line( const wchar_t ** cmdLine, uint32_t count );
    /**
     * \brief   Parses the string passed as a command line. Because of this: 1. the first entry is
     *          ignored, assuming it is a binary name; 2. the parsing completes either when reach
     *          'count' elements or if any entry of 'cmdLine' is an empty string, assuming that
     *          command line options may end with empty string like "".
     *
     * \param   cmdLine     The list of the strings passed as a command line in the binary same as
     *                      'argv' parameter in 'main' function.
     * \param   count       The number of strings in the 'cmdLine' list.
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_command_line( char** cmdLine, uint32_t count);
    /**
     * \brief   Parses the string passed as a command line. Because of this: 1. the first entry is
     *          ignored, assuming it is a binary name; 2. the parsing completes either when reach
     *          'count' elements or if any entry of 'cmdLine' is an empty string, assuming that
     *          command line options may end with empty string like "".
     *
     * \param   cmdLine     The list of the strings passed as a command line in the binary same as
     *                      'argv' parameter in 'main' function.
     * \param   count       The number of strings in the 'cmdLine' list.
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_command_line( wchar_t** cmdLine, uint32_t count);

    /**
     * \brief   Parses the string passed as an options separated by space ' '. Useful when wait for
     *          input from user and need to parse the input.
     *
     * \param   optLine     The options passed in the string. If a value can be a string with the
     *                      space the string should be quoted "
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_option_line( const char * optLine );
    /**
     * \brief   Parses the string passed as an options separated by space ' '. Useful when wait for
     *          input from user and need to parse the input.
     *
     * \param   optLine     The options passed in the string. If a value can be a string with the
     *                      space the string should be quoted "
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_option_line( const wchar_t * optLine );

    /**
     * \brief   Parses the list of strings. Each entry is either an option or a value. The value(s)
     *          should follow the option entry in the list. For example, if first entry is an option
     *          "-a" and it may have value, then the values should immediately follow this option
     *          entry before the next option.
     *
     * \param   optList     The list of options and value.
     * \return  Returns true if succeeded to parse without error. Otherwise, returns false.
     **/
    bool parse_options( StrList & optList );

    /**
     * \brief   Searches in the command list the option of specified ID. Returns valid index if
     *          found the option. Otherwise, returns invalid index (NECommon::INVALID_POSITION).
     *
     * \param   optId       The ID of an option to search in list.
     * \return  Returns valid index if found an option with specified ID. Otherwise, returns invalid
     *          index (NECommon::INVALID_POSITION).
     **/
    uint32_t find_option(int32_t optId) const;

    /**
     * \brief   Sort the entries of options by the ID (value) of commands. The method is useful if
     *          the command has multiple options and they need to be executed by certain priority.
     *          Here the priority is set based on command ID, so that the command with the lowest ID
     *          is listed and can be executed first.
     **/
    void sort();

    /**
     * \brief
     **/
    inline const InputOptionList & options() const;

    /**
     * \brief   Returns the input string, which was parsed.
     **/
    inline const String & input() const;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Creates and sets up input option structure based on the validity index.
     *
     * \param   isShort     True, if matches the int16_t name of the option. Otherwise, should be
     *                      false.
     * \param   cmdLine     The string that contains an option. With the option, the string may
     *                      contain value if the value is separated with the '=' equal sign.
     *                      Otherwise, it should contain only the option string.
     * \param   refSetup    The index of option validation entries that had matching.
     **/
    OptionParser::InputOption _setup_input( bool isShort, String cmdLine, uint32_t refSetup );

    /**
     * \brief   Called to set value in the option. Depending on the flags and the validation range
     *          the 'inFlag' may contain error flag.
     *
     * \param   newValue    The value as a string to set.
     * \param   opt         The option to set the value.
     * \param   refSetup    The index in the option validation setup to look for validation.
     **/
    void _set_input_value( String & newValue, InputOption & opt, uint32_t refSetup );

    /**
     * \brief   Sets the integer value in the option. If needed, checks the validation in the range.
     **/
    void _set_value( int32_t newValue, InputOption & opt, const OptionSetup & valid );

    /**
     * \brief   Sets the float value in the option. If needed, checks the validation in the range.
     **/
    void _set_value( float newValue, InputOption & opt, const OptionSetup & valid );

    /**
     * \brief   Sets the string value in the option. If needed, checks the validation in the range.
     **/
    void _set_value( const String & newValue, InputOption & opt, const OptionSetup & valid );

    /**
     * \brief   Checks the matching of the option. The option should not be following of any
     *          alpha-numeric symbol. The valid symbols are: a. End-of-string ('.
     *
     * \param   input       The option input string to check
     * \param   optCmd      The command of option to match.
     * \return  Returns true if `input` started with `optCmd`.
     **/
    bool _match_option( const String & input, const String & optCmd ) const;

    /**
     * \brief   Cleans the string from the quotes ('.
     **/
    bool _clean_quote( String & data ) const;

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

inline bool OptionParser::has_input_error( uint32_t flags  )
{
    return (flags & static_cast<uint32_t>(OptionFlag::Error)) != 0;
}

inline bool OptionParser::has_range( uint32_t flags  )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::InRange)) != 0);
}

inline bool OptionParser::is_empty_data( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::NoData)) != 0);
}

inline bool OptionParser::is_freestyle( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::String))  != 0) &&
            ((flags & static_cast<uint32_t>(OptionFlag::InRange)) == 0);
}

inline bool OptionParser::is_integer( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::Integer)) != 0);
}

inline bool OptionParser::is_float( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::Float)) != 0);
}

inline bool OptionParser::is_string( uint32_t flags )
{
    return  ((flags & static_cast<uint32_t>(OptionFlag::String)) != 0);
}

inline const OptionParser::InputOptionList & OptionParser::options() const
{
    return mInputOptions;
}

inline const String & OptionParser::input() const
{
    return mCmdLine;
}

#endif  // AREG_AREGEXTEND_CONSOLE_OPTIONPARSER_HPP
