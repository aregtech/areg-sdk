/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/OptionParser.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/OptionParser.hpp"

namespace
{
    template<typename CharType>
    inline void _convertArguments( const CharType ** argv, int32_t argc, OptionParser::StrList & optList )
    {
        if ( (argv != nullptr) && (argc > 1) )
        {
            for ( int i = 1; i < argc; ++i )
            {
                const CharType * opt = argv[i];
                if (NEString::is_empty<CharType>(opt))
                    break;

                optList.push_back(String(opt));
            }
        }
    }

    template<typename CharType>
    inline void _convertArguments(CharType** argv, int32_t argc, OptionParser::StrList& optList)
    {
        if ((argv != nullptr) && (argc > 1))
        {
            for (int i = 1; i < argc; ++i)
            {
                const CharType* opt = argv[i];
                if (NEString::is_empty<CharType>(opt))
                    break;

                optList.push_back(String(opt));
            }
        }
    }

    template<typename CharType>
    inline bool isSpace(CharType ch)
    {
        constexpr CharType space{ ' ' };
        constexpr CharType endol{ '\n' };
        constexpr CharType carrg{ '\r' };
        return (ch == space) || (ch == endol) || (ch == carrg);
    }

    template<typename CharType>
    inline bool isQuote(CharType ch)
    {
        constexpr CharType quote{ '\"' };
        return (ch == quote);
    }

    template<typename CharType>
    inline bool is_equal(CharType ch)
    {
        constexpr CharType equal{ '=' };
        return (ch == equal);
    }

    template<typename CharType>
    inline bool isEofCommand(CharType ch)
    {
        constexpr CharType EofCmd{ ';' };
        return (ch == EofCmd);
    }

    template<typename CharType>
    inline bool isDelimiter(CharType ch)
    {
        return isSpace<CharType>(ch) || is_equal<CharType>(ch) || isEofCommand<CharType>(ch);
    }

    template<typename CharType>
    inline void _splitOptions( const CharType * optLine, OptionParser::StrList & optList )
    {
        constexpr CharType eos{ static_cast<CharType>(NEString::EndOfString) };

        if ( NEString::is_empty<CharType>( optLine ) )
            return;

        const CharType * begin = optLine;
        const CharType * src = optLine;
        while ( *src != eos )
        {
            if (isDelimiter<CharType>(*src))
            {
                if ( begin != src )
                {
                    String str( begin, static_cast<uint32_t>( src - begin ) );
                    optList.push_back( str );
                }
                else if (isSpace<CharType>(*src) == false)
                {
                    ++ src;
                    String str(begin, static_cast<uint32_t>(src - begin));
                    optList.push_back(str);
                }
                else
                {
                    ++ src;
                }

                while ( isSpace<CharType>(*src) )
                {
                    ++ src;
                }

                begin = src;
            }
            else if ( isQuote<CharType>(*src) )
            {
                if ( begin != src )
                {
                    String str( begin, static_cast<uint32_t>( src - begin ) );
                    optList.push_back( str );
                }

                begin = src ++;

                while ( !isQuote<CharType>(*src) && (*src != eos))
                {
                    ++ src;
                }

                if ( isQuote<CharType>(*src) )
                {
                    if ( begin != src ++ )
                    {
                        String str( begin, static_cast<uint32_t>( src - begin ) );
                        optList.push_back( str );
                    }
                }
                else if (begin != src)
                {
                    String str( begin, static_cast<uint32_t>( src - begin ) );
                    optList.push_back( str );
                }

                begin = src;
            }
            else
            {
                ++ src;
            }
        }

        if ( begin != src )
        {
            String str( begin, static_cast<uint32_t>( src - begin ) );
            optList.push_back( str );
        }
    }
} // namespace

const OptionParser::OptionSetup OptionParser::getDefaultOptionSetup()
{
    static OptionSetup _defaultSetup{ "", "", 0, STRING_NO_RANGE, { }, { }, { } };
    return _defaultSetup;
}


OptionParser::OptionParser()
    : mCmdLine      ( )
    , mSetupOptions ( )
    , mInputOptions ( )
{
    mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
}

OptionParser::OptionParser( const OptionSetupList & optList )
    : mCmdLine      ( )
    , mSetupOptions ( optList )
    , mInputOptions ( )
{
    if ( mSetupOptions.is_empty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( const std::vector<OptionSetup> & initList )
    : mCmdLine      ( )
    , mSetupOptions ( initList )
    , mInputOptions ( )
{
    if ( mSetupOptions.is_empty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( std::vector<OptionSetup> && initList ) noexcept
    : mCmdLine      ( )
    , mSetupOptions ( std::move(initList) )
    , mInputOptions ( )
{
    if ( mSetupOptions.is_empty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( const OptionSetup * initEntries, uint32_t count )
    : mCmdLine      ( )
    , mSetupOptions ( )
    , mInputOptions ( )
{
    if ( initEntries != nullptr )
    {
        for ( uint32_t i = 0; i < count; ++ i )
        {
            mSetupOptions.add(initEntries[ i ]);
        }
    }

    if ( mSetupOptions.is_empty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( const OptionParser & src )
    : mCmdLine      ( src.mCmdLine )
    , mSetupOptions ( src.mSetupOptions )
    , mInputOptions ( src.mInputOptions)
{
}

OptionParser::OptionParser( OptionParser && src ) noexcept
    : mCmdLine      ( std::move( src.mCmdLine ))
    , mSetupOptions ( std::move( src.mSetupOptions ))
    , mInputOptions ( std::move( src.mInputOptions ))
{
}

OptionParser & OptionParser::operator=( const OptionParser & src )
{
    if ( &src != static_cast<const OptionParser *>(this) )
    {
        mCmdLine        = src.mCmdLine;
        mSetupOptions   = src.mSetupOptions;
        mInputOptions   = src.mInputOptions;
    }

    return (*this);
}

OptionParser & OptionParser::operator=( OptionParser && src ) noexcept
{
    if ( &src != static_cast<OptionParser *>(this) )
    {
        mCmdLine        = std::move( src.mCmdLine );
        mSetupOptions   = std::move( src.mSetupOptions );
        mInputOptions   = std::move( src.mInputOptions );
    }

    return (*this);
}

bool OptionParser::parseCommandLine( const char ** cmdLine, uint32_t count )
{
    StrList optList;
    _convertArguments<char>( cmdLine, static_cast<int32_t>(count), optList );
    return parseOptions( optList );
}

bool OptionParser::parseCommandLine( const wchar_t ** cmdLine, uint32_t count )
{
    StrList optList;
    _convertArguments<wchar_t>( cmdLine, static_cast<int32_t>(count), optList );
    return parseOptions( optList );
}

bool OptionParser::parseCommandLine(char** cmdLine, uint32_t count)
{
    StrList optList;
    _convertArguments<char>(cmdLine, static_cast<int32_t>(count), optList);
    return parseOptions(optList);
}

bool OptionParser::parseCommandLine(wchar_t** cmdLine, uint32_t count)
{
    StrList optList;
    _convertArguments<wchar_t>(cmdLine, static_cast<int32_t>(count), optList);
    return parseOptions(optList);
}

bool OptionParser::parseOptionLine( const char * optLine )
{
    StrList optList;
    _splitOptions<char>( optLine, optList );
    return parseOptions( optList );
}

bool OptionParser::parseOptionLine( const wchar_t * optLine )
{
    StrList optList;
    _splitOptions<wchar_t>( optLine, optList );
    return parseOptions( optList );
}

bool OptionParser::parseOptions( StrList & optList )
{
    bool result{ true };
    mInputOptions.clear( );
    mCmdLine.clear( );
    uint32_t initSize = mSetupOptions.size( );
    for ( String & input : optList )
    {
        input.trim_all( );

        mCmdLine += input;
        mCmdLine += DELIMITER_SPACE;
        InputOption opt;
        ASSERT( opt.inRefSetup == NECommon::INVALID_INDEX );

        for (uint32_t j = 0; j < initSize; ++ j )
        {
            const OptionSetup & entry = mSetupOptions[ j ];
            if ( _matchOption(input, entry.optLong) )
            {
                opt = _setupInput( false, input, j );
                break;
            }
            else if ( _matchOption(input, entry.optShort) )
            {
                opt = _setupInput( true, input, j );
                break;
            }
        }

        if ( opt.inRefSetup == NECommon::INVALID_INDEX )
        {
            if ( mInputOptions.is_empty() == false )
            {
                InputOption & last = mInputOptions.last_entry();
                ASSERT( last.inRefSetup != NECommon::INVALID_INDEX );
                _setInputValue( input, last, static_cast<uint32_t>(last.inRefSetup) );
                result = OptionParser::hasInputError( static_cast<uint32_t>(last.inField) ) == false;
            }
            else if ( mSetupOptions.is_empty() == false)
            {
                const OptionSetup & setup = mSetupOptions[ 0u ];
                if ( setup.optShort.empty( ) && setup.optLong.empty( ) )
                {
                    // default option
                    opt.inCommand = setup.optCmmand;
                    opt.inRefSetup = 0;
                    opt.inField = FREESTYLE_DATA;
                    opt.inString.push_back( input );
                    mInputOptions.add( opt );
                }
                else
                {
                    result = false;
                }
            }
            else
            {
                opt.inField = FREESTYLE_DATA;
                opt.inString.push_back( input );
                mInputOptions.add( opt );
            }
        }
        else 
        {
            mInputOptions.add( opt );
            result = OptionParser::hasInputError( static_cast<uint32_t>(opt.inField) ) == false;
        }
    }

    return result;
}

uint32_t OptionParser::findOption(int32_t optId) const
{
    uint32_t result{ NECommon::INVALID_POSITION };
    for (uint32_t i = 0; i < mInputOptions.size(); ++i)
    {
        if (mInputOptions.at(i).inCommand == optId)
        {
            result = i;
            break;
        }
    }

    return result;
}

void OptionParser::sort()
{
    mInputOptions.sort([](const OptionParser::InputOption& opt1, const OptionParser::InputOption& opt2)
                        {
                            // sort ascending
                            return (opt1.inCommand < opt2.inCommand);
                         });
}

OptionParser::InputOption OptionParser::_setupInput( bool isShort, String cmdLine, uint32_t refSetup )
{
    ASSERT( (refSetup >= 0) && (refSetup < mSetupOptions.size()) );

    const OptionSetup & setup = mSetupOptions[ refSetup ];
    OptionParser::InputOption opt;
    opt.inField     = setup.optField;
    opt.inCommand   = setup.optCmmand;
    opt.inRefSetup  = static_cast<int32_t>(refSetup);

    cmdLine.substring( static_cast<NEString::CharPos>(isShort ? setup.optShort.length() : setup.optLong.length()) );
    cmdLine.trim_all( );
    if ( cmdLine.is_empty( ) == false )
    {
        _setInputValue( cmdLine, opt, refSetup );
    }

    return opt;
}

void OptionParser::_setInputValue( String & newValue, InputOption & opt, uint32_t refSetup )
{
    const OptionSetup& setup{ mSetupOptions[refSetup] };

    if ( newValue.starts_with( DELIMITER_EQUAL, true ) )
    {
        if ((OptionParser::isString(setup.optField) ==  false) || (opt.inString.size() == 0))
        {
            newValue.substring(1);
            ASSERT(*newValue.as_string() != *DELIMITER_SPACE.data());
        }
    }
    else
    {

    }

    bool cleaned = _cleanQuote( newValue );

    if ( newValue.is_empty( ) == false )
    {
        ASSERT( (refSetup >= 0) && (refSetup < mSetupOptions.size( )) );
        if ( OptionParser::isEmptyData( setup.optField ) )
        {
            // the option should not have data. It is an error
            opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
            opt.inString.push_back( newValue );
        }
        else if ( OptionParser::isInteger( setup.optField ) )
        {
            const char * end = nullptr;
            int32_t val = String::make_int32( newValue.as_string( ), NEString::Radix::Decimal, &end );
            _setValue( val, opt, setup );
            if ( NEString::is_empty<char>( end ) == false )
            {
                opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
            }
        }
        else if ( OptionParser::isFloat( setup.optField ) )
        {
            const char * end = nullptr;
            float val = String::make_float( newValue.as_string( ), &end );
            _setValue( val, opt, setup );
            if ( NEString::is_empty<char>( end ) == false )
            {
                opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
            }
        }
        else if ( OptionParser::isString( setup.optField ) )
        {
            _setValue( newValue, opt, setup );
        }
        else
        {
            // should not happen
            ASSERT( false );
        }
    }

    if ( cleaned == false )
    {
        opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
    }
}

inline void OptionParser::_setValue( int32_t newValue, InputOption & opt, const OptionSetup & setup )
{
    opt.inValue.valInt = newValue;
    if ( OptionParser::hasRange( setup.optField ) )
    {
        if ( (newValue < setup.optRangeInt.valMin) || (newValue > setup.optRangeInt.valMax) )
        {
            opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
        }
    }
}

inline void OptionParser::_setValue( float newValue, InputOption & opt, const OptionSetup & setup )
{
    opt.inValue.valFloat = newValue;
    if ( OptionParser::hasRange( setup.optField ) )
    {
        if ( (newValue < setup.optRangeFloat.valMin) || (newValue > setup.optRangeFloat.valMax) )
        {
            opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
        }
    }
}

inline void OptionParser::_setValue( const String & newValue, InputOption & opt, const OptionSetup & setup )
{
    opt.inString.push_back( newValue );
    if ( OptionParser::isFreestyle(setup.optField) == false )
    {
        const std::vector<std::string_view> & range = setup.optRangeStrings;
        opt.inField |= static_cast<uint32_t>(OptionFlag::Error);
        for ( const std::string_view & entry : range )
        {
            if ( newValue.compare( entry, false) == NEMath::Ordering::Equal)
            {
                opt.inField &= ~static_cast<uint32_t>(OptionFlag::Error);
                break;
            }
        }
    }
}

inline bool OptionParser::_matchOption( const String & input, const String & optCmd ) const
{
    bool result{ false };
    if ( input.starts_with( optCmd, true ) )
    {
        constexpr char eos{ '\0' };
        constexpr char equal{ '=' };
        constexpr char space{ ' ' };

        char sym = input.at( optCmd.length( ) );
        result = (sym == eos) || (sym == equal) || (sym == space);
    }

    return result;
}

inline bool OptionParser::_cleanQuote( String & data ) const
{
    bool result{ true };
    data.trim_all( );
    if ( data.starts_with( DELIMITER_STRING ) )
    {
        if ( data.ends_with( DELIMITER_STRING ) )
        {
            data.substring( 1, data.length( ) - 2 );
            data.trim_all( );
        }
        else
        {
            result = false;
        }
    }

    return result;
}
