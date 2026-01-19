/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/OptionParser.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/OptionParser.hpp"

namespace
{
    template<typename CharType>
    inline void _convertArguments( const CharType ** argv, int argc, OptionParser::StrList & optList )
    {
        if ( (argv != nullptr) && (argc > 1) )
        {
            for ( int i = 1; i < argc; ++i )
            {
                const CharType * opt = argv[i];
                if (NEString::isEmpty<CharType>(opt))
                    break;

                optList.push_back(String(opt));
            }
        }
    }

    template<typename CharType>
    inline void _convertArguments(CharType** argv, int argc, OptionParser::StrList& optList)
    {
        if ((argv != nullptr) && (argc > 1))
        {
            for (int i = 1; i < argc; ++i)
            {
                const CharType* opt = argv[i];
                if (NEString::isEmpty<CharType>(opt))
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
    inline bool isEqual(CharType ch)
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
        return isSpace<CharType>(ch) || isEqual<CharType>(ch) || isEofCommand<CharType>(ch);
    }

    template<typename CharType>
    inline void _splitOptions( const CharType * optLine, OptionParser::StrList & optList )
    {
        constexpr CharType eos{ static_cast<CharType>(NEString::EndOfString) };

        if ( NEString::isEmpty<CharType>( optLine ) )
            return;

        const CharType * begin = optLine;
        const CharType * src = optLine;
        while ( *src != eos )
        {
            if (isDelimiter<CharType>(*src))
            {
                if ( begin != src )
                {
                    String str( begin, MACRO_ELEM_COUNT( begin, src ) );
                    optList.push_back( str );
                }
                else if (isSpace<CharType>(*src) == false)
                {
                    ++ src;
                    String str(begin, MACRO_ELEM_COUNT(begin, src));
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
                    String str( begin, MACRO_ELEM_COUNT( begin, src ) );
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
                        String str( begin, MACRO_ELEM_COUNT( begin, src ) );
                        optList.push_back( str );
                    }
                }
                else if (begin != src)
                {
                    String str( begin, MACRO_ELEM_COUNT( begin, src ) );
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
            String str( begin, MACRO_ELEM_COUNT( begin, src ) );
            optList.push_back( str );
        }
    }
} // namespace

const OptionParser::sOptionSetup OptionParser::getDefaultOptionSetup( void )
{
    static sOptionSetup _defaultSetup{ "", "", 0, STRING_NO_RANGE, { }, { }, { } };
    return _defaultSetup;
}


OptionParser::OptionParser( void )
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
    if ( mSetupOptions.isEmpty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( const std::vector<sOptionSetup> & initList )
    : mCmdLine      ( )
    , mSetupOptions ( initList )
    , mInputOptions ( )
{
    if ( mSetupOptions.isEmpty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( std::vector<sOptionSetup> && initList ) noexcept
    : mCmdLine      ( )
    , mSetupOptions ( std::move(initList) )
    , mInputOptions ( )
{
    if ( mSetupOptions.isEmpty( ) )
    {
        mSetupOptions.add( OptionParser::getDefaultOptionSetup( ) );
    }
}

OptionParser::OptionParser( const sOptionSetup * initEntries, uint32_t count )
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

    if ( mSetupOptions.isEmpty( ) )
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
    _convertArguments<char>( cmdLine, static_cast<int>(count), optList );
    return parseOptions( optList );
}

bool OptionParser::parseCommandLine( const wchar_t ** cmdLine, uint32_t count )
{
    StrList optList;
    _convertArguments<wchar_t>( cmdLine, static_cast<int>(count), optList );
    return parseOptions( optList );
}

bool OptionParser::parseCommandLine(char** cmdLine, uint32_t count)
{
    StrList optList;
    _convertArguments<char>(cmdLine, static_cast<int>(count), optList);
    return parseOptions(optList);
}

bool OptionParser::parseCommandLine(wchar_t** cmdLine, uint32_t count)
{
    StrList optList;
    _convertArguments<wchar_t>(cmdLine, static_cast<int>(count), optList);
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
    uint32_t initSize = mSetupOptions.getSize( );
    for ( String & input : optList )
    {
        input.trimAll( );

        mCmdLine += input;
        mCmdLine += DELIMITER_SPACE;
        sOption opt;
        ASSERT( opt.inRefSetup == NECommon::INVALID_INDEX );

        for (uint32_t j = 0; j < initSize; ++ j )
        {
            const sOptionSetup & entry = mSetupOptions[ j ];
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
            if ( mInputOptions.isEmpty() == false )
            {
                sOption & last = mInputOptions.lastEntry();
                ASSERT( last.inRefSetup != NECommon::INVALID_INDEX );
                _setInputValue( input, last, static_cast<uint32_t>(last.inRefSetup) );
                result = OptionParser::hasInputError( static_cast<uint32_t>(last.inField) ) == false;
            }
            else if ( mSetupOptions.isEmpty() == false)
            {
                const sOptionSetup & setup = mSetupOptions[ 0u ];
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

uint32_t OptionParser::findOption(int optId) const
{
    uint32_t result{ NECommon::INVALID_POSITION };
    for (uint32_t i = 0; i < mInputOptions.getSize(); ++i)
    {
        if (mInputOptions.getAt(i).inCommand == optId)
        {
            result = i;
            break;
        }
    }

    return result;
}

void OptionParser::sort(void)
{
    mInputOptions.sort([](const OptionParser::sOption& opt1, const OptionParser::sOption& opt2)
                        {
                            // sort ascending
                            return (opt1.inCommand < opt2.inCommand);
                         });
}

OptionParser::sOption OptionParser::_setupInput( bool isShort, String cmdLine, uint32_t refSetup )
{
    ASSERT( (refSetup >= 0) && (refSetup < mSetupOptions.getSize()) );

    const sOptionSetup & setup = mSetupOptions[ refSetup ];
    OptionParser::sOption opt;
    opt.inField     = setup.optField;
    opt.inCommand   = setup.optCmmand;
    opt.inRefSetup  = static_cast<int32_t>(refSetup);

    cmdLine.substring( static_cast<NEString::CharPos>(isShort ? setup.optShort.length() : setup.optLong.length()) );
    cmdLine.trimAll( );
    if ( cmdLine.isEmpty( ) == false )
    {
        _setInputValue( cmdLine, opt, refSetup );
    }

    return opt;
}

void OptionParser::_setInputValue( String & newValue, sOption & opt, uint32_t refSetup )
{
    const sOptionSetup& setup{ mSetupOptions[refSetup] };

    if ( newValue.startsWith( DELIMITER_EQUAL, true ) )
    {
        if ((OptionParser::isString(setup.optField) ==  false) || (opt.inString.size() == 0))
        {
            newValue.substring(1);
            ASSERT(*newValue.getString() != *DELIMITER_SPACE.data());
        }
    }
    else
    {

    }

    bool cleaned = _cleanQuote( newValue );

    if ( newValue.isEmpty( ) == false )
    {
        ASSERT( (refSetup >= 0) && (refSetup < mSetupOptions.getSize( )) );
        if ( OptionParser::isEmptyData( setup.optField ) )
        {
            // the option should not have data. It is an error
            opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
            opt.inString.push_back( newValue );
        }
        else if ( OptionParser::isInteger( setup.optField ) )
        {
            const char * end = nullptr;
            int32_t val = String::makeInt32( newValue.getString( ), NEString::eRadix::RadixDecimal, &end );
            _setValue( val, opt, setup );
            if ( NEString::isEmpty<char>( end ) == false )
            {
                opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
            }
        }
        else if ( OptionParser::isFloat( setup.optField ) )
        {
            const char * end = nullptr;
            float val = String::makeFloat( newValue.getString( ), &end );
            _setValue( val, opt, setup );
            if ( NEString::isEmpty<char>( end ) == false )
            {
                opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
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
        opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
    }
}

inline void OptionParser::_setValue( int newValue, sOption & opt, const sOptionSetup & setup )
{
    opt.inValue.valInt = newValue;
    if ( OptionParser::hasRange( setup.optField ) )
    {
        if ( (newValue < setup.optRangeInt.valMin) || (newValue > setup.optRangeInt.valMax) )
        {
            opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
        }
    }
}

inline void OptionParser::_setValue( float newValue, sOption & opt, const sOptionSetup & setup )
{
    opt.inValue.valFloat = newValue;
    if ( OptionParser::hasRange( setup.optField ) )
    {
        if ( (newValue < setup.optRangeFloat.valMin) || (newValue > setup.optRangeFloat.valMax) )
        {
            opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
        }
    }
}

inline void OptionParser::_setValue( const String & newValue, sOption & opt, const sOptionSetup & setup )
{
    opt.inString.push_back( newValue );
    if ( OptionParser::isFreestyle(setup.optField) == false )
    {
        const std::vector<std::string_view> & range = setup.optRangeStrings;
        opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
        for ( const std::string_view & entry : range )
        {
            if ( newValue.compare( entry, false) == NEMath::eCompare::Equal)
            {
                opt.inField &= ~static_cast<uint32_t>(eValidFlags::ValidationError);
                break;
            }
        }
    }
}

inline bool OptionParser::_matchOption( const String & input, const String & optCmd ) const
{
    bool result{ false };
    if ( input.startsWith( optCmd, true ) )
    {
        constexpr char eos{ '\0' };
        constexpr char equal{ '=' };
        constexpr char space{ ' ' };

        char sym = input.getAt( optCmd.getLength( ) );
        result = (sym == eos) || (sym == equal) || (sym == space);
    }

    return result;
}

inline bool OptionParser::_cleanQuote( String & data ) const
{
    bool result{ true };
    data.trimAll( );
    if ( data.startsWith( DELIMITER_STRING ) )
    {
        if ( data.endsWith( DELIMITER_STRING ) )
        {
            data.substring( 1, data.getLength( ) - 2 );
            data.trimAll( );
        }
        else
        {
            result = false;
        }
    }

    return result;
}
