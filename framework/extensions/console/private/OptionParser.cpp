/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extensions/console/OptionParser.cpp
 * \author      Artak Avetyan
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework, extensions
 * \brief       Console option parser extension.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "extensions/console/OptionParser.hpp"

#ifdef WINDOWS

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <tchar.h>

#endif  // WINDOWS
namespace
{
    template<typename CharType>
    inline char ** _convertArguments( const CharType ** argv, int argc )
    {
        char ** argvTemp = argc != 0 ? DEBUG_NEW char * [ argc ] : nullptr;
        if ( argvTemp != nullptr )
        {
            for ( int i = 0; i < static_cast<int>(argc); ++i )
            {
                const CharType * entry = argv[ i ];
                uint32_t length = static_cast<uint32_t>(NEString::getStringLength<CharType>( entry ));
                uint32_t size = length + 1u;
                char * arg = DEBUG_NEW char[ size ];
                NEString::copyString<char, CharType>( arg, size, entry );
                argvTemp[ i ] = arg;
            }
        }
        return argvTemp;
    }

    template<typename CharType>
    inline void _deleteArguments( char ** argv, int argc )
    {
        if ( argv != nullptr )
        {
            for ( int i = 0; i < static_cast<int>(argc); ++i )
                delete[ ] argv[ i ];
            delete[ ] argv;
        }
    }
}

OptionParser::OptionParser( int reserve )
    : mCmdLine      ( )
    , mSetupOptions ( reserve )
    , mInputOptions ( )
{
}

OptionParser::OptionParser( const InitOptionList & optList )
    : mCmdLine      ( )
    , mSetupOptions ( optList )
    , mInputOptions ( )
{
}

OptionParser::OptionParser( const std::vector<sOptionSetup> & initList )
    : mCmdLine      ( )
    , mSetupOptions ( initList )
    , mInputOptions ( )
{
}

OptionParser::OptionParser( std::vector<sOptionSetup> && initList ) noexcept
    : mCmdLine      ( )
    , mSetupOptions  ( std::move(initList) )
    , mInputOptions ( )
{
}

OptionParser::OptionParser( const sOptionSetup * initEntries, int count )
    : mCmdLine      ( )
    , mSetupOptions ( count )
    , mInputOptions ( )
{
    if ( initEntries != nullptr )
    {
        for ( int i = 0; i < count; ++ i )
        {
            mSetupOptions[i] = initEntries[ i ];
        }
    }
}

OptionParser::OptionParser( const OptionParser & src )
    : mCmdLine      ( src.mCmdLine )
    , mSetupOptions  ( src.mSetupOptions )
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
    char ** cmds = _convertArguments<char>( cmdLine, count );

    bool result = _parseCommands( cmds, count );

    _deleteArguments<char>( cmds, count );
    return result;
}

bool OptionParser::parseCommandLine( const wchar_t ** cmdLine, int count )
{
    char ** cmds = _convertArguments<wchar_t>( cmdLine, count );

    bool result = _parseCommands(cmds, count);

    _deleteArguments<char>( cmds, count );
    return result;
}

bool OptionParser::_parseCommands( char ** const cmdLine, int count )
{
    bool result{ true };
    mInputOptions.clear( );
    mCmdLine.clear( );
    int initSize = mSetupOptions.getSize( );
    for ( int i = 1; (i < count) && result; ++ i )
    {
        String input{ cmdLine[ i ] };
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
                if ( setup.optShort.isEmpty( ) && setup.optLong.isEmpty( ) )
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

    cmdLine.substring( isShort ? setup.optShort.getLength() : setup.optLong.getLength() );
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

    _cleanQuote( newValue );

    if ( newValue.isEmpty( ) == false )
    {
        ASSERT( (refSetup >= 0) && (refSetup < static_cast<int>(mSetupOptions.getSize( ))) );
        const sOptionSetup & setup = mSetupOptions[ refSetup ];
        if ( OptionParser::isInteger( setup.optField ) )
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
            _setValue( newValue.toFloat( ), opt, setup );
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
        const std::vector<String> & range = setup.optRangeStrings;
        opt.inField |= static_cast<uint32_t>(eValidFlags::ValidationError);
        for ( const String & entry : range )
        {
            if (entry.compare( newValue, false) == NEMath::eCompare::Equal)
            {
                opt.inField &= ~static_cast<uint32_t>(eValidFlags::ValidationError);
                break;
            }
        }
    }
}

inline bool OptionParser::_matchOption( const String & input, const String & optCmd ) const
{
    return (input.startsWith( optCmd, true ) && (NEString::isAlphanumeric( input.getAt(optCmd.getLength()) ) == false));
}

inline void OptionParser::_cleanQuote( String & data ) const
{
    data.trimAll( );
    if ( data.startsWith( DELIMITER_STRING ) && data.endsWith( DELIMITER_STRING ) )
    {
        data.substring( 1, data.getLength( ) - 2 );
        data.trimAll( );
    }
}
