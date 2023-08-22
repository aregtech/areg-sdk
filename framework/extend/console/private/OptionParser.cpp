/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/console/OptionParser.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG platform, Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "extend/console/OptionParser.hpp"

namespace
{
    template<typename CharType>
    inline void _convertArguments( const CharType ** argv, int argc, OptionParser::StrList & optList )
    {
        if ( (argv != nullptr) && (argc > 1) )
        {
            for ( int i = 1; i < static_cast<int>(argc); ++i )
            {
                optList.push_back( String( argv[ i ] ) );
            }
        }
    }

    template<typename CharType>
    inline void _splitOptions( const CharType * optLine, OptionParser::StrList & optList )
    {
        constexpr CharType space{ ' ' };
        constexpr CharType quote{ '\"' };
        constexpr CharType eos{ static_cast<CharType>(NEString::EndOfString) };

        if ( NEString::isEmpty<CharType>( optLine ) )
            return;

        const CharType * begin = optLine;
        const CharType * src = optLine;
        while ( *src != eos )
        {
            if ( *src == space )
            {
                if ( begin != src )
                {
                    String str( begin, MACRO_ELEM_COUNT( begin, src ) );
                    optList.push_back( str );
                }

                while ( (*src == space) && (*src != eos) )
                {
                    ++ src;
                }

                begin = src;
            }
            else if ( *src == quote )
            {
                if ( begin != src )
                {
                    String str( begin, MACRO_ELEM_COUNT( begin, src ) );
                    optList.push_back( str );
                }

                begin = src ++;

                while ( (*src != quote) && (*src != eos) )
                {
                    ++ src;
                }

                if (*src == quote)
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
}

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

OptionParser::OptionParser( const sOptionSetup * initEntries, int count )
    : mCmdLine      ( )
    , mSetupOptions ( )
    , mInputOptions ( )
{
    if ( initEntries != nullptr )
    {
        for ( int i = 0; i < count; ++ i )
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

bool OptionParser::parseCommandLine( const char ** cmdLine, int count )
{
    StrList optList;
    _convertArguments<char>( cmdLine, count, optList );
    return parseOptions( optList );
}

bool OptionParser::parseCommandLine( const wchar_t ** cmdLine, int count )
{
    StrList optList;
    _convertArguments<wchar_t>( cmdLine, count, optList );
    return parseOptions( optList );
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
    int initSize = mSetupOptions.getSize( );
    for ( String & input : optList )
    {
        input.trimAll( );

        mCmdLine += input;
        mCmdLine += DELIMITER_SPACE;
        sOption opt;
        ASSERT( opt.inRefSetup == NECommon::INVALID_INDEX );

        for ( int j = 0; j < initSize; ++ j )
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
                _setInputValue( input, last, last.inRefSetup );
                result = OptionParser::hasInputError( last.inField ) == false;
            }
            else if ( mSetupOptions.isEmpty() == false)
            {
                const sOptionSetup & setup = mSetupOptions[ 0 ];
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
            result = OptionParser::hasInputError( opt.inField ) == false;
        }
    }

    return result;
}

OptionParser::sOption OptionParser::_setupInput( bool isShort, String cmdLine, int refSetup )
{
    ASSERT( (refSetup >= 0) && (refSetup < static_cast<int>(mSetupOptions.getSize())) );

    const sOptionSetup & setup = mSetupOptions[ refSetup ];
    OptionParser::sOption opt;
    opt.inField     = setup.optField;
    opt.inCommand   = setup.optCmmand;
    opt.inRefSetup  = refSetup;

    cmdLine.substring( static_cast<NEString::CharPos>(isShort ? setup.optShort.length() : setup.optLong.length()) );
    cmdLine.trimAll( );
    if ( cmdLine.isEmpty( ) == false )
    {
        _setInputValue( cmdLine, opt, refSetup );
    }

    return opt;
}

void OptionParser::_setInputValue( String & newValue, sOption & opt, int refSetup )
{
    if ( newValue.startsWith( DELIMITER_EQUAL, true ) )
    {
        newValue.substring( 1 );
        newValue.trimAll( );
    }

    bool cleaned = _cleanQuote( newValue );

    if ( newValue.isEmpty( ) == false )
    {
        ASSERT( (refSetup >= 0) && (refSetup < static_cast<int>(mSetupOptions.getSize( ))) );
        const sOptionSetup & setup = mSetupOptions[ refSetup ];
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
