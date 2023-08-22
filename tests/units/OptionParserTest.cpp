/************************************************************************
 * \file        units/OptionParserTest.Cpp
 * \ingroup     platform dependent files
 * \brief       AREG Framework, Command line option parser tests
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "units/GUnitTest.hpp"

#include "extend/console/OptionParser.hpp"

#include <vector>
#include <string>

//!< Basic options with no data passed as a command line strings
TEST( OptionParserTest, CommandLineOptionSimpleNoData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-b", "-b", 2, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-c", "-c", 3, OptionParser::FREESTYLE_DATA, {}, {}, {} }
    };

    const wchar_t * cmdLine[]{ L"OptionParserTest", L"-a", L"-b", L"-c" };

    int setupCount = MACRO_ARRAYLEN( setup );
    int optCount = MACRO_ARRAYLEN( cmdLine );

    OptionParser parser( setup, setupCount );

    ASSERT_TRUE(parser.parseCommandLine( cmdLine, optCount ));

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), optCount - 1 );
    for ( int i = 1; i < optCount; ++i )
    {
        ASSERT_EQ( opts[ i - 1 ].inCommand, i );
        ASSERT_TRUE( OptionParser::isFreestyle( opts[ i - 1 ].inField ) );
        ASSERT_TRUE( OptionParser::isString( opts[ i - 1 ].inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opts[ i - 1 ].inField ) );
    }
}

//!< Basic options with data passed as a command line strings
TEST( OptionParserTest, CommandLineOptionSimpleWithData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a=1", L"-b=2.3", L"-c=something" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( setup ) );

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

//!< Options with data, but no assignment symbol passed as a command line strings
TEST( OptionParserTest, CommandLineOptionWithDataNoAssignment )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"1", L"-b", L"2.3", L"-c", L"some", L"thing" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( setup ) );

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

//!< Options with short and long names and no data, passed as a command line strings
TEST( OptionParserTest, CommandLineOptionMixedNoData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-b", "-bc"         , 2, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-d", "-def"        , 3, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-g", "--blah-blah" , 4, OptionParser::FREESTYLE_DATA, {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"-bc", L"-def", L"--blah-blah"};

    int setupCount = MACRO_ARRAYLEN( setup );
    int optCount = MACRO_ARRAYLEN( cmdLine );

    OptionParser parser( setup, setupCount );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, optCount ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), optCount - 1 );
    for ( int i = 1; i < optCount; ++i )
    {
        ASSERT_EQ( opts[ i - 1 ].inCommand, i );
        ASSERT_TRUE( OptionParser::isFreestyle( opts[ i - 1 ].inField ) );
        ASSERT_TRUE( OptionParser::isString( opts[ i - 1 ].inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opts[ i - 1 ].inField ) );
    }
}

//!< Options with short and long names and with data, passed as a command line strings
TEST( OptionParserTest, CommandLineOptionMixedWithData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-b", "-bc"         , 2, OptionParser::FLOAT_IN_RANGE   , {       }, {0.1f, 10.1f}, {} }
        , { "-d", "-def"        , 3, OptionParser::STRING_IN_RANGE  , {       }, {         }, {"some", "thing", "somethingelse" } }
        , { "-g", "--blah-blah" , 4, OptionParser::STRING_NO_RANGE  , {       }, {         }, {} }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"=", L"1", L"-bc", L"=", L"2.3", L"-def", L"some", L"--blah-blah", L"thing", L"and", L"somethingelse" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( setup ) );

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

//!< Options with data in the quote that contains a space, passed as a command line strings
TEST( OptionParserTest, CommandLineOptionLongDataInQuote )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"      , 2, OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 1 );

    const OptionParser::sOption & opt = opts[ 0 ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( OptionParser::isString( opt.inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1 );
    ASSERT_EQ( opt.inString[ 0 ], scope );
}

//!< Options with default option setup
TEST( OptionParserTest, CommandLineOptionDefaultData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { OptionParser::getDefaultOptionSetup() }
        , { "-s", "-scope"  , 2, OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"freestyle", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseCommandLine( cmdLine, MACRO_ARRAYLEN( cmdLine ) ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 2 );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 0 );
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

//!< Option with no data, passed as a string separated with space
TEST( OptionParserTest, OptionsSimpleNoData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-b", "-b", 2, OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-c", "-c", 3, OptionParser::FREESTYLE_DATA, {}, {}, {} }
    };

    const wchar_t * optLine{ L"-a -b -c" };

    int setupCount = MACRO_ARRAYLEN( setup );

    OptionParser parser( setup, setupCount );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 3 );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( OptionParser::isFreestyle( opt.inField ) );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 1 ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( OptionParser::isFreestyle( opt.inField ) );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 2 ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( OptionParser::isFreestyle( opt.inField ) );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    } while ( false );
}

//!< Option with data, passed as a string separated with space
TEST( OptionParserTest, OptionSimpleWithData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * optLine{ L"-a=1 -b=2.3 -c=something" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( opts.getSize( ), MACRO_ARRAYLEN( setup ) );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( OptionParser::isInteger( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 1 ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( OptionParser::isFloat( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 2 ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1 );
        ASSERT_EQ( opt.inString[ 0 ], "something" );
    } while ( false );
}

//!< Option with data and no assignment symbol, passed as a string separated with space
TEST( OptionParserTest, OptionWithDataNoAssignment )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-a", 1, OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * optLine{ L"-a 1 -b 2.3 -c some thing" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 3 );

    do
    {
        const OptionParser::sOption & opt = opts[ 0 ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( OptionParser::isInteger( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 1 ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( OptionParser::isFloat( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const OptionParser::sOption & opt = opts[ 2 ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( OptionParser::isString( opt.inField ) );
        ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 2 );
        ASSERT_EQ( opt.inString[ 0 ], "some" );
        ASSERT_EQ( opt.inString[ 1 ], "thing" );
    } while ( false );
}

//!< Mixed option with data, passed as a string separated with space
TEST( OptionParserTest, OptionMixedWithData )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-b", "-bc"         , 2, OptionParser::FLOAT_IN_RANGE   , {       }, {0.1f, 10.1f}, {} }
        , { "-d", "-def"        , 3, OptionParser::STRING_IN_RANGE  , {       }, {         }, {"some", "thing", "somethingelse" } }
        , { "-g", "--blah-blah" , 4, OptionParser::STRING_NO_RANGE  , {       }, {         }, {} }
    };

    const wchar_t * optLine{ L"-a = 1  -bc=2.3   -def some --blah-blah thing and somethingelse" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 4 );

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

//!< Option with data in the quote that contains space, passed as a string separated with space
TEST( OptionParserTest, OptionLongDataInQuote )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {}, {} }
        , { "-s", "-scope"      , 2, OptionParser::STRING_NO_RANGE  , {       }, {}, {} }
    };

    constexpr std::string_view scope {"*.areg_* = DEBUG | SCOPE"};
    const wchar_t * optLine{ L"-scope \"*.areg_* = DEBUG | SCOPE\"" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 1 );

    const OptionParser::sOption & opt = opts[ 0 ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( OptionParser::isString( opt.inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1 );
    ASSERT_EQ( opt.inString[ 0 ], scope );
}

//!< Option with data in the quote that contains error (quote is not closed), passed as a string separated with space
TEST( OptionParserTest, OptionLongDataInQuoteWithError )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"      , 2, OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"*.areg_* = DEBUG | SCOPE"};
    constexpr std::string_view error {"-a = 5"};
    const wchar_t * optLine{ L"-scope \"*.areg_* = DEBUG | SCOPE -a = 5" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_FALSE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 1 );

    const OptionParser::sOption & opt = opts[ 0 ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( OptionParser::isString( opt.inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1 );
    ASSERT_NE( opt.inString[0], scope );
    ASSERT_TRUE( opt.inString[0].endsWith(error) );
}

//!< Option with multiple data, passed as a string separated with space
TEST( OptionParserTest, OptionMultipleParametersAsciiChar )
{
    OptionParser::sOptionSetup setup[ ]
    {
          { "cp", "copy", 1, OptionParser::STRING_NO_RANGE, {}, {}, {} }
        , { "mv", "move", 2, OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    const char * optLine{ "copy ./file.txt ./some/" };

    OptionParser parser( setup, MACRO_ARRAYLEN( setup ) );

    ASSERT_TRUE( parser.parseOptionLine( optLine ) );

    const OptionParser::InputOptionList & opts = parser.getOptions( );
    ASSERT_EQ( static_cast<int>(opts.getSize( )), 1 );

    const OptionParser::sOption & opt = opts[ 0 ];
    ASSERT_EQ( opt.inCommand, 1 );
    ASSERT_TRUE( OptionParser::isString( opt.inField ) );
    ASSERT_FALSE( OptionParser::hasRange( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 2 );
    ASSERT_EQ( opt.inString[ 0 ], "./file.txt" );
    ASSERT_EQ( opt.inString[ 1 ], "./some/" );
}

//!< Example of options to pass Geometry figures, passed as a string separated with space, with and without quotes.
TEST( OptionParserTest, OptionFiguresWithData )
{
    enum Figure
    {
          Triangle = 1
        , Rectangle = 2
    };

    const OptionParser::sOptionSetup setup[ ]
    {
          {"-t", "--tri"  , static_cast<int>(Figure::Triangle) , OptionParser::STRING_NO_RANGE, {}, {}, {}}
        , { "-r", "--rect", static_cast<int>(Figure::Rectangle), OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    OptionParser parser1( setup, MACRO_ARRAYLEN( setup ) );
    const char * optCase1 = "-t \"{1, 1}, { 5, 10 }, { 20, 3 }\" --rect \"{1, 10}, 5, 3\"";
    ASSERT_TRUE( parser1.parseOptionLine( optCase1 ) );

    OptionParser parser2( setup, MACRO_ARRAYLEN( setup ) );
    const char * optCase2 = "-t {1,1} {5,10} {20,3} --rect {1,10} 5 3";
    ASSERT_TRUE( parser2.parseOptionLine( optCase2 ) );

    ASSERT_EQ( parser1.getOptions().getSize(), parser2.getOptions().getSize() );
    ASSERT_EQ( parser1.getOptions().getSize(), static_cast<uint32_t>(2) );

    const OptionParser::InputOptionList & opts1 = parser1.getOptions( );
    const OptionParser::InputOptionList & opts2 = parser2.getOptions( );

    do
    {
        const OptionParser::sOption & opt1 = opts1[ 0 ];
        const OptionParser::sOption & opt2 = opts2[ 0 ];
        ASSERT_EQ( opt1.inCommand , static_cast<int>(Figure::Triangle) );
        ASSERT_EQ( opt1.inCommand , opt2.inCommand );
        ASSERT_EQ( opt1.inField   , opt2.inField );
        ASSERT_EQ( opt1.inRefSetup, opt2.inRefSetup );

        ASSERT_EQ( opt1.inString.size( ), 1 );
        ASSERT_EQ( opt1.inString[ 0 ], "{1, 1}, { 5, 10 }, { 20, 3 }" );

        ASSERT_EQ( opt2.inString.size( ), 3 );
        ASSERT_EQ( opt2.inString[ 0 ], "{1,1}");
        ASSERT_EQ( opt2.inString[ 1 ], "{5,10}");
        ASSERT_EQ( opt2.inString[ 2 ], "{20,3}");
    } while ( false );

    do
    {
        const OptionParser::sOption & opt1 = opts1[ 1 ];
        const OptionParser::sOption & opt2 = opts2[ 1 ];
        ASSERT_EQ( opt1.inCommand , static_cast<int>(Figure::Rectangle) );
        ASSERT_EQ( opt1.inCommand , opt2.inCommand );
        ASSERT_EQ( opt1.inField   , opt2.inField );
        ASSERT_EQ( opt1.inRefSetup, opt2.inRefSetup );

        ASSERT_EQ( opt1.inString.size( ), 1 );
        ASSERT_EQ( opt1.inString[ 0 ], "{1, 10}, 5, 3" );

        ASSERT_EQ( opt2.inString.size( ), 3 );
        ASSERT_EQ( opt2.inString[ 0 ], "{1,10}" );
        ASSERT_EQ( opt2.inString[ 1 ], "5" );
        ASSERT_EQ( opt2.inString[ 2 ], "3" );
    } while ( false );
}
