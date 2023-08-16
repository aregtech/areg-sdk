/************************************************************************
 * \file        units/OptionParserTest.Cpp
 * \ingroup     platform dependent files
 * \brief       AREG Framework, Command line option parser tests
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "units/GUnitTest.hpp"

#include "extensions/console/OptionParser.hpp"

#include <vector>
#include <string>

namespace
{
    template<typename CharType>
    CharType ** _createCmdLine( std::vector<std::basic_string<CharType>> & cmdList )
    {
        int count = static_cast<int>(cmdList.size( ));
        CharType ** cmdLine = count > 0 ? new CharType * [ count ] : nullptr;
        if ( cmdLine != nullptr )
        {
            for ( int i = 0; i < count; ++i )
            {
                std::basic_string<CharType> & entry = cmdList.at(i);
                cmdLine[ i ] = entry.data();
            }
        }

        return cmdLine;
    }

    template<typename CharType>
    void _deleteCmdLine( CharType ** cmdLine )
    {
        delete[ ] cmdLine;
    }
}

TEST( OptionParserTest, CommandLineOptionSimpleNoData )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-a", 1, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-b", "-b", 2, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-c", "-c", 3, OptionParser::FREESTYLE_DATA, {}, {}, {} }
    };

    const wchar_t * cmdLine[]{ L"OptionParserTest", L"-a", L"-b", L"-c" };

    int initCount = MACRO_ARRAYLEN( init );
    int optCount = MACRO_ARRAYLEN( cmdLine );

    OptionParser parser( init, initCount );

    ASSERT_TRUE(parser.parseCommandLine( cmdLine, optCount ));

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), optCount - 1 );
    for ( int i = 1; i < optCount; ++i )
    {
        ASSERT_EQ( opts[ i - 1 ].inCommand, i );
        ASSERT_TRUE( OptionParser::isFreestyle( opts[ i - 1 ].inField ) );
        ASSERT_TRUE( OptionParser::isString( opts[ i - 1 ].inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opts[ i - 1 ].inField ) );
    }
}

TEST( OptionParserTest, CommandLineOptionSimpleWithData )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a=1", L"-b=2.3", L"-c=something" };

    OptionParser parser( init, MACRO_ARRAYLEN( init ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( init ) );

    ASSERT_EQ( opts[ 0 ].inCommand, 1 );
    ASSERT_TRUE(  OptionParser::isInteger(opts[ 0 ].inField) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 0 ].inField ) );
    ASSERT_EQ( opts[ 0 ].inValue.valInt, 1 );

    ASSERT_EQ( opts[ 1 ].inCommand, 2 );
    ASSERT_TRUE(  OptionParser::isFloat( opts[ 1 ].inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 1 ].inField ) );
    ASSERT_EQ( opts[ 1 ].inValue.valFloat, 2.3f );

    ASSERT_EQ( opts[ 2 ].inCommand, 3 );
    ASSERT_TRUE(  OptionParser::isString( opts[ 2 ].inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 2 ].inField ) );
    ASSERT_EQ( opts[ 2 ].inString.size(), 1 );
    ASSERT_EQ( opts[ 2 ].inString[0], "something" );
}

TEST( OptionParserTest, CommandLineOptionWithDataNoAssignment )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"1", L"-b", L"2.3", L"-c", L"some", L"thing" };

    OptionParser parser( init, MACRO_ARRAYLEN( init ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( init ) );

    ASSERT_EQ( opts[ 0 ].inCommand, 1 );
    ASSERT_TRUE( OptionParser::isInteger( opts[ 0 ].inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 0 ].inField ) );
    ASSERT_EQ( opts[ 0 ].inValue.valInt, 1 );

    ASSERT_EQ( opts[ 1 ].inCommand, 2 );
    ASSERT_TRUE( OptionParser::isFloat( opts[ 1 ].inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 1 ].inField ) );
    ASSERT_EQ( opts[ 1 ].inValue.valFloat, 2.3f );

    ASSERT_EQ( opts[ 2 ].inCommand, 3 );
    ASSERT_TRUE( OptionParser::isString( opts[ 2 ].inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opts[ 2 ].inField ) );
    ASSERT_EQ( opts[ 2 ].inString.size( ), 2 );
    ASSERT_EQ( opts[ 2 ].inString[ 0 ], "some" );
    ASSERT_EQ( opts[ 2 ].inString[ 1 ], "thing" );
}

TEST( OptionParserTest, CommandLineOptionMixedNoData )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-aaa"        , 1, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-b", "-bc"         , 2, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-d", "-def"        , 3, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-g", "--blah-blah" , 4, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"-bc", L"-def", L"--blah-blah"};

    int initCount = MACRO_ARRAYLEN( init );
    int optCount = MACRO_ARRAYLEN( cmdLine );

    OptionParser parser( init, initCount );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, optCount ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), optCount - 1 );
    for ( int i = 1; i < optCount; ++i )
    {
        ASSERT_EQ( opts[ i - 1 ].inCommand, i );
        ASSERT_TRUE( OptionParser::isFreestyle( opts[ i - 1 ].inField ) );
        ASSERT_TRUE( OptionParser::isString( opts[ i - 1 ].inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opts[ i - 1 ].inField ) );
    }
}

TEST( OptionParserTest, CommandLineOptionMixedWithData )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-aaa"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-b", "-bc"         , 2, OptionParser::FLOAT_IN_RANGE   , {       }, {0.1f, 10.1f}, {} }
        , { "-d", "-def"        , 3, OptionParser::STRING_IN_RANGE  , {       }, {         }, {"some", "thing", "somethingelse" } }
        , { "-g", "--blah-blah" , 4, OptionParser::STRING_NO_RANGE  , {       }, {         }, {} }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"=", L"1", L"-bc", L"=", L"2.3", L"-def", L"some", L"--blah-blah", L"thing", L"and", L"somethingelse" };

    OptionParser parser( init, MACRO_ARRAYLEN( init ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( init ) );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( OptionParser::isInteger( opt.inField ) );
        ASSERT_TRUE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 1 ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( OptionParser::isFloat( opt.inField ) );
        ASSERT_TRUE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 2 ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_TRUE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1 );
        ASSERT_EQ( opt.inString[ 0 ], "some" );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 3 ];
        ASSERT_EQ( opt.inCommand, 4 );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 3 );
        ASSERT_EQ( opt.inString[ 0 ], "thing" );
        ASSERT_EQ( opt.inString[ 1 ], "and" );
        ASSERT_EQ( opt.inString[ 2 ], "somethingelse" );
    } while ( false );
}

TEST( OptionParserTest, CommandLineOptionLongDataInQuote )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "-a", "-abc"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"      , 2, OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    OptionParser parser( init, MACRO_ARRAYLEN( init ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), 1 );

    const OptionParser::sOption & opt = opts[ 0 ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( OptionParser::isString( opt.inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1 );
    ASSERT_EQ( opt.inString[ 0 ], scope );
}

TEST( OptionParserTest, CommandLineOptionDefaultData )
{
    OptionParser::sOptionSetup init[ ]
    {
          { "", ""          , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"  , 2, OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"freestyle", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    OptionParser parser( init, MACRO_ARRAYLEN( init ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), 2 );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( OptionParser::isFreestyle( opt.inField ) );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1 );
        ASSERT_EQ( opt.inString[ 0 ], "freestyle" );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 1 ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1 );
        ASSERT_EQ( opt.inString[ 0 ], scope );
    } while ( false );
}
