#ifndef AREG_EXTENSIONS_CONSOLE_OPTIONPARSER_HPP
#define AREG_EXTENSIONS_CONSOLE_OPTIONPARSER_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extensions/console/OptionParser.hpp
 * \author      Artak Avetyan
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework, extensions
 * \brief       Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "extensions/Extensions.hpp"

#include "areg/base/NECommon.hpp"
#include "areg/base/String.hpp"
#include "areg/base/TEArrayList.hpp"

/**
 * \examples
 *              1. Simple options: '-a', '-b', '-c'
 *                  myapp -a -b -c
 *              2. Options with values, set by '=': '-a' = {1}, '-b' = {2.3} -c = {"something"}
 *                  myapp -a=1 -b=2.3 -c=something
 *              3. Options with values, set by ' ': '-a' = {1}, '-b' = {2.3} '-c' = {"some", "thing"}
 *                  myapp -a 1 -b 2.3 -c some thing
 *              4. Options with short and long names: '-a', '-bc', '-def', '--ghijk-lmop'
 *                  myapp -a -bc -def --ghijk-lmop
 *              5. Options with value set mixed:  '-a' = {1}, '-bc' = {2.3}, '-def' =  {"some"}, '--ghijk-lmop' = {"thing" "and" "somethingelse"}
 *                  myapp -a=1 -bc = 2.3 -def some --ghijk-lmop thing and somethingelse
 *              6. Options with free texts: '-scope' = {"*.areg_* = DEBUG | SCOPE"}
 *                  myapp -scope "*.areg_* = DEBUG | SCOPE"
 **/
class OptionParser
{
private:
    static constexpr std::string_view    DELIMITER_EQUAL { "=" };
    static constexpr std::string_view    DELIMITER_SPACE { " " };
    static constexpr std::string_view    DELIMITER_STRING{ "\"" };

public:

    enum class eValidFlags : unsigned int
    {
          ValidationInvalid     = 0     //!< Bits: 0000 0000
        , ValidationError       = 1     //!< Bits: 0000 0001
        , ValidationInRange     = 2     //!< Bits: 0000 0010
        , ValidationInteger     = 16    //!< Bits: 0001 0000
        , ValidationFloat       = 32    //!< Bits: 0010 0000
        , ValidationString      = 64    //!< Bits: 0100 0000
    };

    static constexpr uint32_t   INTEGER_NO_RANGE{ static_cast<uint32_t>(eValidFlags::ValidationInteger)};

    static constexpr uint32_t   FLOAT_NO_RANGE  { static_cast<uint32_t>(eValidFlags::ValidationFloat) };

    static constexpr uint32_t   STRING_NO_RANGE { static_cast<uint32_t>(eValidFlags::ValidationString) };

    static constexpr uint32_t   INTEGER_IN_RANGE{ INTEGER_NO_RANGE | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    static constexpr uint32_t   FLOAT_IN_RANGE  { FLOAT_NO_RANGE   | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    static constexpr uint32_t   STRING_IN_RANGE { STRING_NO_RANGE  | static_cast<uint32_t>(eValidFlags::ValidationInRange) };

    static constexpr uint32_t   FREESTYLE_DATA  { STRING_NO_RANGE  };

    template<typename VALUE>
    struct Range
    {
        VALUE   valMin;
        VALUE   valMax;
    };

    union uValues
    {
        int     valInt;
        float   valFloat;
    };

    typedef std::vector<String> StrList;

    /**
     * \brief   The option entry to parse in command line.
     *          Every single entry represent the possible option value and meaning.
     *          There are possible to enter short name of the option and the long name.
     *          For example, the "-h" and "--help" could have same meaning.
     *          And every such entry has a meaning 'optOption', which represents the
     *          digital value of the option.
     *          The entry is used to construct predefined option list as well as
     *          when build the option list after parsing.
     **/

    struct sOptionSetup
    {
        String          optShort        { };        //!< The short name of the option.
        String          optLong         { };        //!< The long name of the option.
        int             optCmmand       { -1 };     //!< The digital value of the command.
        uint32_t        optField        { 0 };      //!< The flag indicating the valid fields.
        Range<int>      optRangeInt     { 0, 0 };   //!< Range of valid integer values, ignored if no range.
        Range<float>    optRangeFloat   { 0., 0. }; //!< Range of valid floating point values, ignored if no range
        StrList         optRangeStrings { };        //!< Range of valid strings, no case sensitive
    };

    /**
     * \brief   The option
     **/
    struct sOption
    {
        int     inCommand   { -1 };
        int     inField     { 0 };
        int     inRefSetup  { NECommon::INVALID_INDEX };
        uValues inValue     { 0 };
        StrList inString    { };
    };

    using InitOptionList    = TEArrayList<sOptionSetup>;
    using InputOptionList   = TEArrayList<sOption>;

public:

    static inline bool hasInputError( uint32_t flags );

    static inline bool hasRange( uint32_t flags );

    static inline bool isFreestyle( uint32_t flags );

    static inline bool isInteger( uint32_t flags );

    static inline bool isFloat( uint32_t flags );

    static inline bool isString( uint32_t flags );

public:
    OptionParser( void ) = default;
    explicit OptionParser( int reserve );
    explicit OptionParser( const InitOptionList & initList );
    explicit OptionParser( const std::vector<sOptionSetup> & initList );
    explicit OptionParser( std::vector<sOptionSetup> && initList ) noexcept;
    OptionParser( const sOptionSetup * initEntries, int count );
    OptionParser( const OptionParser & src );
    OptionParser( OptionParser && src ) noexcept;
    ~OptionParser( void ) = default;

public:
    OptionParser & operator = ( const OptionParser & src );

    OptionParser & operator = ( OptionParser && src ) noexcept;

public:

    bool parseCommandLine( const char ** cmdLine,  int count);

    bool parseCommandLine( const wchar_t ** cmdLine, int count );

    inline const InputOptionList & getOptions( void ) const;

private:

    bool _parseCommands( char ** const cmdLine, int count );

    OptionParser::sOption _setupInput( bool isShort, String cmdLine, int refSetup );

    void _setInputValue( String & newValue, sOption & opt, int refSetup );

    void _setValue( int newValue, sOption & opt, const sOptionSetup & valid );

    void _setValue( float newValue, sOption & opt, const sOptionSetup & valid );

    void _setValue( const String & newValue, sOption & opt, const sOptionSetup & valid );

    bool _matchOption( const String & input, const String & optCmd ) const;

    void _cleanQuote( String & data ) const;

private:

    String          mCmdLine;
    InitOptionList  mSetupOptions;
    InputOptionList mInputOptions;
};

inline bool OptionParser::hasInputError( uint32_t flags  )
{
    return (flags & static_cast<uint32_t>(eValidFlags::ValidationError)) != 0;
}

inline bool OptionParser::hasRange( uint32_t flags  )
{
    return  ((flags & static_cast<uint32_t>(eValidFlags::ValidationInRange)) != 0);
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

#endif  // AREG_EXTENSIONS_CONSOLE_OPTIONPARSER_HPP
