/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/OptionParserTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Command line option parser tests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "aregextend/console/OptionParser.hpp"

#include <vector>
#include <string>

//!< Basic options with no data passed as a command line strings
TEST( OptionParserTest, command_line_option_simple_no_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-b", "-b", 2, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-c", "-c", 3, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
    };

    const wchar_t * cmdLine[]{ L"OptionParserTest", L"-a", L"-b", L"-c" };

    uint32_t setupCount = std::size( setup );
    uint32_t optCount = std::size( cmdLine );

    areg::ext::OptionParser parser( setup, setupCount );

    ASSERT_TRUE(parser.parse_command_line( cmdLine, optCount ));

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), optCount - 1 );
    for (uint32_t i = 1; i < optCount; ++i)
    {
        ASSERT_EQ( opts[ i - 1 ].inCommand, static_cast<int>(i) );
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opts[ i - 1 ].inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opts[ i - 1 ].inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ i - 1 ].inField ) );
    }
}

//!< Basic options with data passed as a command line strings
TEST( OptionParserTest, command_line_option_simple_with_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, areg::ext::OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a=1", L"-b=2.3", L"-c=something" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, std::size( cmdLine ) ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size( ), std::size( setup ) );

    ASSERT_EQ( opts[ 0u ].inCommand, 1 );
    ASSERT_TRUE(  areg::ext::OptionParser::is_integer(opts[ 0u ].inField) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 0u ].inField ) );
    ASSERT_EQ( opts[ 0u ].inValue.valInt, 1 );

    ASSERT_EQ( opts[ 1u ].inCommand, 2 );
    ASSERT_TRUE(  areg::ext::OptionParser::is_float( opts[ 1u ].inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 1u ].inField ) );
    ASSERT_EQ( opts[ 1u ].inValue.valFloat, 2.3f );

    ASSERT_EQ( opts[ 2u ].inCommand, 3 );
    ASSERT_TRUE(  areg::ext::OptionParser::is_string( opts[ 2u ].inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 2u ].inField ) );
    ASSERT_EQ( opts[ 2u ].inString.size(), 1u );
    ASSERT_EQ( opts[ 2u ].inString[0], "something" );
}

//!< Options with data, but no assignment symbol passed as a command line strings
TEST( OptionParserTest, command_line_option_with_data_no_assignment )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, areg::ext::OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"1", L"-b", L"2.3", L"-c", L"some", L"thing" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, std::size( cmdLine ) ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size( ), std::size( setup ) );

    ASSERT_EQ( opts[ 0u ].inCommand, 1 );
    ASSERT_TRUE( areg::ext::OptionParser::is_integer( opts[ 0u ].inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 0u ].inField ) );
    ASSERT_EQ( opts[ 0u ].inValue.valInt, 1 );

    ASSERT_EQ( opts[ 1u ].inCommand, 2 );
    ASSERT_TRUE( areg::ext::OptionParser::is_float( opts[ 1u ].inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 1u ].inField ) );
    ASSERT_EQ( opts[ 1u ].inValue.valFloat, 2.3f );

    ASSERT_EQ( opts[ 2u ].inCommand, 3 );
    ASSERT_TRUE( areg::ext::OptionParser::is_string( opts[ 2u ].inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ 2u ].inField ) );
    ASSERT_EQ( opts[ 2u ].inString.size( ), 2u );
    ASSERT_EQ( opts[ 2u ].inString[ 0u ], "some" );
    ASSERT_EQ( opts[ 2u ].inString[ 1u ], "thing" );
}

//!< Options with short and long names and no data, passed as a command line strings
TEST( OptionParserTest, command_line_option_mixed_no_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-b", "-bc"         , 2, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-d", "-def"        , 3, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {}  }
        , { "-g", "--blah-blah" , 4, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {}  }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"-bc", L"-def", L"--blah-blah"};

    uint32_t setupCount = std::size( setup );
    uint32_t optCount = std::size( cmdLine );

    areg::ext::OptionParser parser( setup, setupCount );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, optCount ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), optCount - 1u );
    for (uint32_t i = 1; i < optCount; ++i )
    {
        ASSERT_EQ( opts[ i - 1u ].inCommand, static_cast<int>(i));
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opts[ i - 1u ].inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opts[ i - 1u ].inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opts[ i - 1u ].inField ) );
    }
}

//!< Options with short and long names and with data, passed as a command line strings
TEST( OptionParserTest, command_line_option_mixed_with_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, areg::ext::OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-b", "-bc"         , 2, areg::ext::OptionParser::FLOAT_IN_RANGE   , {       }, {0.1f, 10.1f}, {} }
        , { "-d", "-def"        , 3, areg::ext::OptionParser::STRING_IN_RANGE  , {       }, {         }, {"some", "thing", "somethingelse" } }
        , { "-g", "--blah-blah" , 4, areg::ext::OptionParser::STRING_NO_RANGE  , {       }, {         }, {} }
    };

    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-a", L"=", L"1", L"-bc", L"=", L"2.3", L"-def", L"some", L"--blah-blah", L"thing", L"and", L"somethingelse" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, std::size( cmdLine ) ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size( ), std::size( setup ) );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( areg::ext::OptionParser::is_integer( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_float( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 2u ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1u );
        ASSERT_EQ( opt.inString[ 0 ], "some" );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 3u ];
        ASSERT_EQ( opt.inCommand, 4 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 3u );
        ASSERT_EQ( opt.inString[ 0 ], "thing" );
        ASSERT_EQ( opt.inString[ 1 ], "and" );
        ASSERT_EQ( opt.inString[ 2 ], "somethingelse" );
    } while ( false );
}

//!< Options with data in the quote that contains a space, passed as a command line strings
TEST( OptionParserTest, command_line_option_long_data_in_quote )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, areg::ext::OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"      , 2, areg::ext::OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, std::size( cmdLine ) ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 1u );

    const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1u );
    ASSERT_EQ( opt.inString[ 0 ], scope );
}

//!< Options with default option setup
TEST( OptionParserTest, command_line_option_default_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { areg::ext::OptionParser::default_option_setup() }
        , { "-s", "-scope"  , 2, areg::ext::OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    constexpr std::string_view scope {"* .areg_ * = DEBUG | SCOPE"};
    const wchar_t * cmdLine[ ]{ L"OptionParserTest", L"freestyle", L"-scope", L"\" * .areg_ * = DEBUG | SCOPE\"" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_command_line( cmdLine, std::size( cmdLine ) ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 2u );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 0 );
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1u );
        ASSERT_EQ( opt.inString[ 0u ], "freestyle" );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1u );
        ASSERT_EQ( opt.inString[ 0 ], scope );
    } while ( false );
}

//!< Option with no data, passed as a string separated with space
TEST( OptionParserTest, options_simple_no_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-b", "-b", 2, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
        , { "-c", "-c", 3, areg::ext::OptionParser::FREESTYLE_DATA, {}, {}, {} }
    };

    const wchar_t * optLine{ L"-a -b -c" };

    uint32_t setupCount = std::size( setup );

    areg::ext::OptionParser parser( setup, setupCount );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ(opts.size(), 3u );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 2u ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( areg::ext::OptionParser::is_freestyle( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    } while ( false );
}

//!< Option with data, passed as a string separated with space
TEST( OptionParserTest, option_simple_with_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, areg::ext::OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * optLine{ L"-a=1 -b=2.3 -c=something" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size( ), std::size( setup ) );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( areg::ext::OptionParser::is_integer( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_float( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 2u ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1u );
        ASSERT_EQ( opt.inString[ 0 ], "something" );
    } while ( false );
}

//!< Option with data and no assignment symbol, passed as a string separated with space
TEST( OptionParserTest, option_with_data_no_assignment )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-a", 1, areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {}  }
        , { "-b", "-b", 2, areg::ext::OptionParser::FLOAT_NO_RANGE  , {}, {}, {}  }
        , { "-c", "-c", 3, areg::ext::OptionParser::STRING_NO_RANGE , {}, {}, {}  }
    };

    const wchar_t * optLine{ L"-a 1 -b 2.3 -c some thing" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 3u );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( areg::ext::OptionParser::is_integer( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_float( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 2u ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 2u );
        ASSERT_EQ( opt.inString[ 0 ], "some" );
        ASSERT_EQ( opt.inString[ 1 ], "thing" );
    } while ( false );
}

//!< Mixed option with data, passed as a string separated with space
TEST( OptionParserTest, option_mixed_with_data )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-aaa"        , 1, areg::ext::OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-b", "-bc"         , 2, areg::ext::OptionParser::FLOAT_IN_RANGE   , {       }, {0.1f, 10.1f}, {} }
        , { "-d", "-def"        , 3, areg::ext::OptionParser::STRING_IN_RANGE  , {       }, {         }, {"some", "thing", "somethingelse" } }
        , { "-g", "--blah-blah" , 4, areg::ext::OptionParser::STRING_NO_RANGE  , {       }, {         }, {} }
    };

    const wchar_t * optLine{ L"-a = 1  -bc=2.3   -def some --blah-blah thing and somethingelse" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 4u );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
        ASSERT_EQ( opt.inCommand, 1 );
        ASSERT_TRUE( areg::ext::OptionParser::is_integer( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valInt, 1 );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 1u ];
        ASSERT_EQ( opt.inCommand, 2 );
        ASSERT_TRUE( areg::ext::OptionParser::is_float( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inValue.valFloat, 2.3f );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 2u ];
        ASSERT_EQ( opt.inCommand, 3 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_TRUE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 1u );
        ASSERT_EQ( opt.inString[ 0u ], "some" );
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt = opts[ 3u ];
        ASSERT_EQ( opt.inCommand, 4 );
        ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
        ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
        ASSERT_EQ( opt.inString.size( ), 3u );
        ASSERT_EQ( opt.inString[ 0 ], "thing" );
        ASSERT_EQ( opt.inString[ 1 ], "and" );
        ASSERT_EQ( opt.inString[ 2 ], "somethingelse" );
    } while ( false );
}

//!< Option with data in the quote that contains space, passed as a string separated with space
TEST( OptionParserTest, option_long_data_in_quote )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, areg::ext::OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {}, {} }
        , { "-s", "-scope"      , 2, areg::ext::OptionParser::STRING_NO_RANGE  , {       }, {}, {} }
    };

    constexpr std::string_view scope {"*.areg_* = DEBUG | SCOPE"};
    const wchar_t * optLine{ L"-scope \"*.areg_* = DEBUG | SCOPE\"" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 1u );

    const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1u );
    ASSERT_EQ( opt.inString[ 0 ], scope );
}

//!< Option with data in the quote that contains error (quote is not closed), passed as a string separated with space
TEST( OptionParserTest, option_long_data_in_quote_with_error )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "-a", "-abc"        , 1, areg::ext::OptionParser::INTEGER_IN_RANGE , {1 , 10 }, {         }, {} }
        , { "-s", "-scope"      , 2, areg::ext::OptionParser::STRING_NO_RANGE  , {       }, {0.1f, 10.1f}, {} }
    };

    constexpr std::string_view scope {"*.areg_* = DEBUG | SCOPE"};
    constexpr std::string_view error {"-a = 5"};
    const wchar_t * optLine{ L"-scope \"*.areg_* = DEBUG | SCOPE -a = 5" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_FALSE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 1u );

    const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
    ASSERT_EQ( opt.inCommand, 2 );
    ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 1u );
    ASSERT_NE( opt.inString[0], scope );
    ASSERT_TRUE( opt.inString[0].ends_with(error) );
}

//!< Option with multiple data, passed as a string separated with space
TEST( OptionParserTest, option_multiple_parameters_ascii_char )
{
    areg::ext::OptionParser::OptionSetup setup[ ]
    {
          { "cp", "copy", 1, areg::ext::OptionParser::STRING_NO_RANGE, {}, {}, {} }
        , { "mv", "move", 2, areg::ext::OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    const char * optLine{ "copy ./file.txt ./some/" };

    areg::ext::OptionParser parser( setup, std::size( setup ) );

    ASSERT_TRUE( parser.parse_option_line( optLine ) );

    const areg::ext::OptionParser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size(), 1u );

    const areg::ext::OptionParser::InputOption & opt = opts[ 0u ];
    ASSERT_EQ( opt.inCommand, 1 );
    ASSERT_TRUE( areg::ext::OptionParser::is_string( opt.inField ) );
    ASSERT_FALSE( areg::ext::OptionParser::has_range( opt.inField ) );
    ASSERT_EQ( opt.inString.size( ), 2u );
    ASSERT_EQ( opt.inString[ 0 ], "./file.txt" );
    ASSERT_EQ( opt.inString[ 1 ], "./some/" );
}

//!< Example of options to pass Geometry figures, passed as a string separated with space, with and without quotes.
TEST( OptionParserTest, option_figures_with_data )
{
    enum class Figure
    {
          Triangle = 1
        , Rectangle = 2
    };

    const areg::ext::OptionParser::OptionSetup setup[ ]
    {
          {"-t", "--tri"  , static_cast<int>(Figure::Triangle) , areg::ext::OptionParser::STRING_NO_RANGE, {}, {}, {}}
        , { "-r", "--rect", static_cast<int>(Figure::Rectangle), areg::ext::OptionParser::STRING_NO_RANGE, {}, {}, {} }
    };

    areg::ext::OptionParser parser1( setup, std::size( setup ) );
    const char * optCase1 = "-t \"{1, 1}, { 5, 10 }, { 20, 3 }\" --rect \"{1, 10}, 5, 3\"";
    ASSERT_TRUE( parser1.parse_option_line( optCase1 ) );

    areg::ext::OptionParser parser2( setup, std::size( setup ) );
    const char * optCase2 = "-t {1,1} {5,10} {20,3} --rect {1,10} 5 3";
    ASSERT_TRUE( parser2.parse_option_line( optCase2 ) );

    ASSERT_EQ( parser1.options().size(), parser2.options().size() );
    ASSERT_EQ( parser1.options().size(), 2u );

    const areg::ext::OptionParser::InputOptionList & opts1 = parser1.options( );
    const areg::ext::OptionParser::InputOptionList & opts2 = parser2.options( );

    do
    {
        const areg::ext::OptionParser::InputOption & opt1 = opts1[ 0u ];
        const areg::ext::OptionParser::InputOption & opt2 = opts2[ 0u ];
        ASSERT_EQ( opt1.inCommand , static_cast<int>(Figure::Triangle) );
        ASSERT_EQ( opt1.inCommand , opt2.inCommand );
        ASSERT_EQ( opt1.inField   , opt2.inField );
        ASSERT_EQ( opt1.inRefSetup, opt2.inRefSetup );

        ASSERT_EQ( opt1.inString.size( ), 1u );
        ASSERT_EQ( opt1.inString[ 0u ], "{1, 1}, { 5, 10 }, { 20, 3 }" );

        ASSERT_EQ( opt2.inString.size( ), 3u );
        ASSERT_EQ( opt2.inString[ 0u ], "{1,1}");
        ASSERT_EQ( opt2.inString[ 1u ], "{5,10}");
        ASSERT_EQ( opt2.inString[ 2u ], "{20,3}");
    } while ( false );

    do
    {
        const areg::ext::OptionParser::InputOption & opt1 = opts1[ 1u ];
        const areg::ext::OptionParser::InputOption & opt2 = opts2[ 1u ];
        ASSERT_EQ( opt1.inCommand , static_cast<int>(Figure::Rectangle) );
        ASSERT_EQ( opt1.inCommand , opt2.inCommand );
        ASSERT_EQ( opt1.inField   , opt2.inField );
        ASSERT_EQ( opt1.inRefSetup, opt2.inRefSetup );

        ASSERT_EQ( opt1.inString.size( ), 1u );
        ASSERT_EQ( opt1.inString[ 0 ], "{1, 10}, 5, 3" );

        ASSERT_EQ( opt2.inString.size( ), 3u );
        ASSERT_EQ( opt2.inString[ 0 ], "{1,10}" );
        ASSERT_EQ( opt2.inString[ 1 ], "5" );
        ASSERT_EQ( opt2.inString[ 2 ], "3" );
    } while ( false );
}

namespace
{
    using Parser    = areg::ext::OptionParser;
    using SetupEntry     = areg::ext::OptionParser::OptionSetup;

    //!< SetupEntry with every option data type
    std::vector<SetupEntry> typed_setup( )
    {
        return std::vector<SetupEntry>
        {
              { "-j", "--job"   , 1, Parser::INTEGER_IN_RANGE, { 1, 32 }, {            }, {                    } }
            , { "-f", "--factor", 2, Parser::FLOAT_IN_RANGE  , {       }, { 0.5f, 2.5f }, {                    } }
            , { "-c", "--config", 3, Parser::STRING_IN_RANGE , {       }, {            }, { "Debug", "Release" } }
            , { "-n", "--num"   , 4, Parser::INTEGER_NO_RANGE, {       }, {            }, {                    } }
            , { "-h", "--help"  , 5, Parser::NO_DATA         , {       }, {            }, {                    } }
            , { "-s", "--str"   , 6, Parser::STRING_NO_RANGE , {       }, {            }, {                    } }
        };
    }
}

//!< The flag helpers report each flag independently.
TEST( OptionParserTest, flag_helpers_report_each_flag )
{
    EXPECT_FALSE( Parser::has_input_error( Parser::STRING_NO_RANGE ) );
    EXPECT_TRUE( Parser::has_input_error( Parser::STRING_NO_RANGE | static_cast<uint32_t>(Parser::OptionFlag::Error) ) );

    EXPECT_FALSE( Parser::has_range( Parser::INTEGER_NO_RANGE ) );
    EXPECT_TRUE( Parser::has_range( Parser::INTEGER_IN_RANGE ) );
    EXPECT_TRUE( Parser::has_range( Parser::FLOAT_IN_RANGE ) );
    EXPECT_TRUE( Parser::has_range( Parser::STRING_IN_RANGE ) );

    EXPECT_TRUE( Parser::is_empty_data( Parser::NO_DATA ) );
    EXPECT_FALSE( Parser::is_empty_data( Parser::STRING_NO_RANGE ) );

    EXPECT_TRUE( Parser::is_integer( Parser::INTEGER_IN_RANGE ) );
    EXPECT_FALSE( Parser::is_integer( Parser::FLOAT_NO_RANGE ) );
    EXPECT_TRUE( Parser::is_float( Parser::FLOAT_IN_RANGE ) );
    EXPECT_FALSE( Parser::is_float( Parser::STRING_NO_RANGE ) );
    EXPECT_TRUE( Parser::is_string( Parser::STRING_IN_RANGE ) );
    EXPECT_FALSE( Parser::is_string( Parser::NO_DATA ) );

    EXPECT_TRUE( Parser::is_freestyle( Parser::FREESTYLE_DATA ) );
    EXPECT_TRUE( Parser::is_freestyle( Parser::STRING_NO_RANGE ) );
    EXPECT_FALSE( Parser::is_freestyle( Parser::STRING_IN_RANGE ) );
    EXPECT_FALSE( Parser::is_freestyle( Parser::INTEGER_NO_RANGE ) );
    EXPECT_FALSE( Parser::is_freestyle( Parser::NO_DATA ) );
}

//!< The default setup has no names, command 0 and accepts any string.
TEST( OptionParserTest, default_setup_has_no_names_and_accepts_strings )
{
    const SetupEntry setup = Parser::default_option_setup( );
    EXPECT_TRUE( setup.optShort.empty( ) );
    EXPECT_TRUE( setup.optLong.empty( ) );
    EXPECT_EQ( setup.optCmmand, 0 );
    EXPECT_EQ( setup.optField, Parser::STRING_NO_RANGE );
}

//!< A parser without setup collects every word as a string of the default option.
TEST( OptionParserTest, default_parser_collects_words_as_strings )
{
    Parser parser;
    ASSERT_TRUE( parser.parse_option_line( "hello world" ) );

    const Parser::InputOptionList & opts = parser.options( );
    ASSERT_EQ( opts.size( ), 1u );
    EXPECT_EQ( opts[ 0u ].inCommand, 0 );
    EXPECT_TRUE( Parser::is_freestyle( opts[ 0u ].inField ) );
    ASSERT_EQ( opts[ 0u ].inString.size( ), 2u );
    EXPECT_EQ( opts[ 0u ].inString[ 0u ], "hello" );
    EXPECT_EQ( opts[ 0u ].inString[ 1u ], "world" );
}

//!< An empty setup list, an empty vector and a null entry list fall back to the default setup.
TEST( OptionParserTest, empty_setups_fall_back_to_default_setup )
{
    Parser::OptionSetupList emptyList;
    Parser fromList( emptyList );

    std::vector<SetupEntry> emptyVector;
    Parser fromVector( emptyVector );

    std::vector<SetupEntry> movedVector;
    Parser fromMoved( std::move( movedVector ) );

    Parser fromNull( nullptr, 3u );
    Parser fromZero( nullptr, 0u );

    Parser * parsers[ ] { &fromList, &fromVector, &fromMoved, &fromNull, &fromZero };
    for ( Parser * parser : parsers )
    {
        ASSERT_TRUE( parser->parse_option_line( "one two" ) );
        ASSERT_EQ( parser->options( ).size( ), 1u );
        EXPECT_EQ( parser->options( )[ 0u ].inCommand, 0 );
        EXPECT_EQ( parser->options( )[ 0u ].inString.size( ), 2u );
    }
}

//!< Constructors with a non-empty list, vector and moved vector keep the given setup.
TEST( OptionParserTest, constructors_keep_given_setup )
{
    Parser::OptionSetupList list;
    list.add( SetupEntry{ "-a", "--alpha", 7, Parser::INTEGER_NO_RANGE, { }, { }, { } } );
    Parser fromList( list );

    std::vector<SetupEntry> vec{ SetupEntry{ "-a", "--alpha", 7, Parser::INTEGER_NO_RANGE, { }, { }, { } } };
    Parser fromVector( vec );
    Parser fromMoved( std::move( vec ) );

    Parser * parsers[ ] { &fromList, &fromVector, &fromMoved };
    for ( Parser * parser : parsers )
    {
        ASSERT_TRUE( parser->parse_option_line( "-a 42" ) );
        ASSERT_EQ( parser->options( ).size( ), 1u );
        EXPECT_EQ( parser->options( )[ 0u ].inCommand, 7 );
        EXPECT_EQ( parser->options( )[ 0u ].inValue.valInt, 42 );
    }
}

//!< Copy construction and copy assignment copy the setup and the parsed options.
TEST( OptionParserTest, copy_keeps_setup_and_parsed_options )
{
    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_option_line( "-j 5" ) );

    Parser copied( parser );
    ASSERT_EQ( copied.options( ).size( ), 1u );
    EXPECT_EQ( copied.options( )[ 0u ].inValue.valInt, 5 );
    EXPECT_EQ( copied.input( ), parser.input( ) );

    Parser assigned;
    assigned = parser;
    ASSERT_EQ( assigned.options( ).size( ), 1u );
    EXPECT_EQ( assigned.options( )[ 0u ].inValue.valInt, 5 );
    EXPECT_EQ( assigned.input( ), parser.input( ) );

    ASSERT_TRUE( copied.parse_option_line( "-j 9" ) );
    EXPECT_EQ( copied.options( )[ 0u ].inValue.valInt, 9 );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valInt, 5 );
    ASSERT_TRUE( assigned.parse_option_line( "--job=11" ) );
    EXPECT_EQ( assigned.options( )[ 0u ].inValue.valInt, 11 );
}

//!< Move construction and move assignment transfer the setup and the parsed options.
TEST( OptionParserTest, move_transfers_setup_and_parsed_options )
{
    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_option_line( "-j 5" ) );

    Parser moved( std::move( parser ) );
    ASSERT_EQ( moved.options( ).size( ), 1u );
    EXPECT_EQ( moved.options( )[ 0u ].inValue.valInt, 5 );
    ASSERT_TRUE( moved.parse_option_line( "-j 6" ) );
    EXPECT_EQ( moved.options( )[ 0u ].inValue.valInt, 6 );

    Parser assigned;
    assigned = std::move( moved );
    ASSERT_EQ( assigned.options( ).size( ), 1u );
    EXPECT_EQ( assigned.options( )[ 0u ].inValue.valInt, 6 );
    ASSERT_TRUE( assigned.parse_option_line( "-n 3" ) );
    EXPECT_EQ( assigned.options( )[ 0u ].inValue.valInt, 3 );
}

//!< Assigning an object to itself keeps its state.
TEST( OptionParserTest, self_assignment_keeps_state )
{
    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_option_line( "-j 5" ) );

    Parser & alias = parser;
    parser = alias;
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valInt, 5 );

    parser = std::move( alias );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valInt, 5 );
}

//!< A parser can parse a line after it was the source of a move.
TEST( OptionParserTest, moved_from_parser_still_collects_a_word )
{
    Parser source;
    Parser target( std::move( source ) );
    EXPECT_TRUE( target.parse_option_line( "x" ) );

    ASSERT_TRUE( source.parse_option_line( "word" ) );
    ASSERT_EQ( source.options( ).size( ), 1u );
    ASSERT_EQ( source.options( )[ 0u ].inString.size( ), 1u );
    EXPECT_EQ( source.options( )[ 0u ].inString[ 0u ], "word" );
}

//!< Command line of narrow strings: non-const and const argument lists.
TEST( OptionParserTest, command_line_narrow_argument_lists )
{
    char arg0[ ] = "app";
    char arg1[ ] = "-j";
    char arg2[ ] = "7";
    char * argv[ ] { arg0, arg1, arg2 };

    Parser parserNonConst( typed_setup( ) );
    ASSERT_TRUE( parserNonConst.parse_command_line( argv, 3u ) );
    ASSERT_EQ( parserNonConst.options( ).size( ), 1u );
    EXPECT_EQ( parserNonConst.options( )[ 0u ].inCommand, 1 );
    EXPECT_EQ( parserNonConst.options( )[ 0u ].inValue.valInt, 7 );

    const char * cargv[ ] { "app", "--job=9", "-n", "-4" };
    Parser parserConst( typed_setup( ) );
    ASSERT_TRUE( parserConst.parse_command_line( cargv, 4u ) );
    ASSERT_EQ( parserConst.options( ).size( ), 2u );
    EXPECT_EQ( parserConst.options( )[ 0u ].inValue.valInt, 9 );
    EXPECT_EQ( parserConst.options( )[ 1u ].inCommand, 4 );
    EXPECT_EQ( parserConst.options( )[ 1u ].inValue.valInt, -4 );
}

//!< Command line of wide strings: non-const argument list.
TEST( OptionParserTest, command_line_wide_non_const_argument_list )
{
    wchar_t arg0[ ] = L"app";
    wchar_t arg1[ ] = L"-f";
    wchar_t arg2[ ] = L"1.5";
    wchar_t * argv[ ] { arg0, arg1, arg2 };

    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_command_line( argv, 3u ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 2 );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valFloat, 1.5f );
}

//!< A null list, a count below two and an empty entry end the command line without options.
TEST( OptionParserTest, command_line_null_short_and_empty_entries )
{
    char ** nullNarrow = nullptr;
    const char ** nullConstNarrow = nullptr;
    wchar_t ** nullWide = nullptr;
    const wchar_t ** nullConstWide = nullptr;

    Parser parser( typed_setup( ) );
    EXPECT_TRUE( parser.parse_command_line( nullNarrow, 3u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_command_line( nullConstNarrow, 3u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_command_line( nullWide, 3u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_command_line( nullConstWide, 3u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );

    const char * onlyName[ ] { "app", "-h" };
    EXPECT_TRUE( parser.parse_command_line( onlyName, 1u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_command_line( onlyName, 0u ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );

    char arg0[ ] = "app";
    char arg1[ ] = "-h";
    char arg2[ ] = "";
    char arg3[ ] = "-s";
    char * narrow[ ] { arg0, arg1, arg2, arg3 };
    ASSERT_TRUE( parser.parse_command_line( narrow, 4u ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 5 );

    const char * constNarrow[ ] { "app", "-h", "", "-s" };
    ASSERT_TRUE( parser.parse_command_line( constNarrow, 4u ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );

    wchar_t warg0[ ] = L"app";
    wchar_t warg1[ ] = L"-h";
    wchar_t warg2[ ] = L"";
    wchar_t warg3[ ] = L"-s";
    wchar_t * wide[ ] { warg0, warg1, warg2, warg3 };
    ASSERT_TRUE( parser.parse_command_line( wide, 4u ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );

    const wchar_t * constWide[ ] { L"app", L"-h", L"", L"-s" };
    ASSERT_TRUE( parser.parse_command_line( constWide, 4u ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
}

//!< An empty or null option line gives no option and no error.
TEST( OptionParserTest, empty_option_line_gives_no_options )
{
    Parser parser( typed_setup( ) );
    const char * nullNarrow = nullptr;
    const wchar_t * nullWide = nullptr;

    EXPECT_TRUE( parser.parse_option_line( "" ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_option_line( L"" ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_option_line( nullNarrow ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
    EXPECT_TRUE( parser.parse_option_line( nullWide ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
}

//!< An integer in range is accepted on both borders; outside the range it is flagged.
TEST( OptionParserTest, integer_range_borders_and_errors )
{
    Parser parser( typed_setup( ) );

    for ( const char * line : { "-j 1", "-j 32", "--job=16", "--job = 8" } )
    {
        ASSERT_TRUE( parser.parse_option_line( line ) ) << line;
        ASSERT_EQ( parser.options( ).size( ), 1u ) << line;
        EXPECT_TRUE( Parser::is_integer( parser.options( )[ 0u ].inField ) );
        EXPECT_TRUE( Parser::has_range( parser.options( )[ 0u ].inField ) );
        EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) ) << line;
    }

    for ( const char * line : { "-j 0", "-j 33", "-j -1", "--job=100" } )
    {
        EXPECT_FALSE( parser.parse_option_line( line ) ) << line;
        ASSERT_EQ( parser.options( ).size( ), 1u ) << line;
        EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) ) << line;
    }
}

//!< An integer value that is not a whole number is flagged.
TEST( OptionParserTest, integer_not_a_number_is_error )
{
    Parser parser( typed_setup( ) );

    for ( const char * line : { "-n abc", "-n 12x", "-n=7.5" } )
    {
        EXPECT_FALSE( parser.parse_option_line( line ) ) << line;
        ASSERT_EQ( parser.options( ).size( ), 1u ) << line;
        EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) ) << line;
    }

    ASSERT_TRUE( parser.parse_option_line( "-n=-5" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valInt, -5 );
}

//!< A float in range is accepted on both borders; outside the range or not a number it is flagged.
TEST( OptionParserTest, float_range_borders_and_errors )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-f 0.5" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::is_float( parser.options( )[ 0u ].inField ) );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valFloat, 0.5f );

    ASSERT_TRUE( parser.parse_option_line( "--factor=2.5" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valFloat, 2.5f );

    for ( const char * line : { "-f 2.6", "-f 0.1", "-f -1", "-f x", "-f 1.5z" } )
    {
        EXPECT_FALSE( parser.parse_option_line( line ) ) << line;
        ASSERT_EQ( parser.options( ).size( ), 1u ) << line;
        EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) ) << line;
    }
}

//!< A string in range matches without case sensitivity; any other string is flagged.
TEST( OptionParserTest, string_range_matches_ignoring_case )
{
    Parser parser( typed_setup( ) );

    for ( const char * line : { "-c Debug", "-c debug", "--config=RELEASE" } )
    {
        ASSERT_TRUE( parser.parse_option_line( line ) ) << line;
        ASSERT_EQ( parser.options( ).size( ), 1u ) << line;
        EXPECT_TRUE( Parser::has_range( parser.options( )[ 0u ].inField ) );
        EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) ) << line;
        ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 1u );
    }

    EXPECT_FALSE( parser.parse_option_line( "--config=Blah" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 0u ], "Blah" );
}

//!< An option without data is flagged when a value follows; alone it is valid.
TEST( OptionParserTest, no_data_option_rejects_a_value )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-h" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::is_empty_data( parser.options( )[ 0u ].inField ) );
    EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    EXPECT_EQ( parser.options( )[ 0u ].inString.size( ), 0u );

    EXPECT_FALSE( parser.parse_option_line( "-h extra" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 0u ], "extra" );

    EXPECT_FALSE( parser.parse_option_line( "--help=extra" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}

//!< A word that is not an option and has no option before it is rejected.
TEST( OptionParserTest, unknown_option_without_previous_option_is_rejected )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "--unknown" ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );

    EXPECT_FALSE( parser.parse_option_line( "-jobs" ) );
    EXPECT_EQ( parser.options( ).size( ), 0u );
}

//!< An option name followed by another symbol than end, space or equal is not that option.
TEST( OptionParserTest, option_name_must_end_at_delimiter )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-s -sx" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 6 );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 0u ], "-sx" );
}

//!< A repeated option gives one entry per occurrence, in the order of the line.
TEST( OptionParserTest, repeated_option_gives_entry_per_occurrence )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-n 1 -n 2 --num=3" ) );
    ASSERT_EQ( parser.options( ).size( ), 3u );
    for ( uint32_t i = 0; i < 3u; ++ i )
    {
        EXPECT_EQ( parser.options( )[ i ].inCommand, 4 );
        EXPECT_EQ( parser.options( )[ i ].inValue.valInt, static_cast<int32_t>(i + 1u) );
    }
}

//!< Each parse replaces the options and the input line of the previous one.
TEST( OptionParserTest, parse_replaces_previous_result )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-h -s one" ) );
    EXPECT_EQ( parser.options( ).size( ), 2u );
    EXPECT_EQ( parser.input( ), "-h -s one " );

    ASSERT_TRUE( parser.parse_option_line( "-n 2" ) );
    EXPECT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.input( ), "-n 2 " );
}

//!< find_option returns the position of the first option with the command, or the invalid position.
TEST( OptionParserTest, find_option_returns_position_or_invalid )
{
    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_option_line( "-s -h -n 4" ) );
    ASSERT_EQ( parser.options( ).size( ), 3u );

    EXPECT_EQ( parser.find_option( 6 ), 0u );
    EXPECT_EQ( parser.find_option( 5 ), 1u );
    EXPECT_EQ( parser.find_option( 4 ), 2u );
    EXPECT_EQ( parser.find_option( 99 ), static_cast<uint32_t>(areg::INVALID_POSITION) );

    Parser empty( typed_setup( ) );
    EXPECT_EQ( empty.find_option( 1 ), static_cast<uint32_t>(areg::INVALID_POSITION) );
}

//!< sort orders the options by ascending command.
TEST( OptionParserTest, sort_orders_options_by_command )
{
    Parser parser( typed_setup( ) );
    ASSERT_TRUE( parser.parse_option_line( "-s -h -f 1.0 -j 3" ) );
    ASSERT_EQ( parser.options( ).size( ), 4u );

    parser.sort( );

    ASSERT_EQ( parser.options( ).size( ), 4u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 1 );
    EXPECT_EQ( parser.options( )[ 1u ].inCommand, 2 );
    EXPECT_EQ( parser.options( )[ 2u ].inCommand, 5 );
    EXPECT_EQ( parser.options( )[ 3u ].inCommand, 6 );
    EXPECT_EQ( parser.find_option( 6 ), 3u );
}

//!< Line breaks, carriage returns and surplus spaces separate options like a space.
TEST( OptionParserTest, option_line_separators )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "  -h\n-s\r-n   5 " ) );
    ASSERT_EQ( parser.options( ).size( ), 3u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 5 );
    EXPECT_EQ( parser.options( )[ 1u ].inCommand, 6 );
    EXPECT_EQ( parser.options( )[ 2u ].inCommand, 4 );
    EXPECT_EQ( parser.options( )[ 2u ].inValue.valInt, 5 );

    ASSERT_TRUE( parser.parse_option_line( L"  -h\n-s\r-n   5 " ) );
    ASSERT_EQ( parser.options( ).size( ), 3u );
    EXPECT_EQ( parser.options( )[ 2u ].inValue.valInt, 5 );
}

//!< A semicolon ends the word before it, so the next option is recognised.
TEST( OptionParserTest, semicolon_separates_options )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-s;-n 3" ) );
    ASSERT_EQ( parser.options( ).size( ), 2u );
    EXPECT_EQ( parser.options( )[ 0u ].inCommand, 6 );
    EXPECT_EQ( parser.options( )[ 1u ].inCommand, 4 );
    EXPECT_EQ( parser.options( )[ 1u ].inValue.valInt, 3 );
}

//!< A quoted value starting right after the option name is one value without quotes.
TEST( OptionParserTest, quote_right_after_option_name )
{
    Parser parser( typed_setup( ) );

    ASSERT_TRUE( parser.parse_option_line( "-s\"a b\"" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 0u ], "a b" );

    ASSERT_TRUE( parser.parse_option_line( L"-s\"a b\" c" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 2u );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 0u ], "a b" );
    EXPECT_EQ( parser.options( )[ 0u ].inString[ 1u ], "c" );
}

//!< A quote that is never closed is flagged for narrow and wide lines.
TEST( OptionParserTest, unclosed_quote_is_error )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "-s \"abc def" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );

    EXPECT_FALSE( parser.parse_option_line( L"-s \"abc def" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}

//!< Reproduces the defect: a valid option after an invalid one made the whole parse report success.
TEST( OptionParserTest, error_of_earlier_option_fails_the_parse )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "-j 100 -h" ) );
    ASSERT_EQ( parser.options( ).size( ), 2u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 1u ].inField ) );

    EXPECT_FALSE( parser.parse_option_line( "--unknown -h" ) );

    EXPECT_TRUE( parser.parse_option_line( "-j 10 -h" ) );
    EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}

//!< Reproduces the defect: the value after the invalid one reset the result of the same parse.
TEST( OptionParserTest, invalid_value_fails_the_parse_when_more_values_follow )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "-h stray -j 10" ) );
    EXPECT_FALSE( parser.parse_option_line( L"-j 100 -h" ) );
}

//!< Reproduces the defect: the command line reported success when only an earlier option was invalid.
TEST( OptionParserTest, command_line_fails_on_an_earlier_option_and_skips_the_program_name )
{
    Parser parser( typed_setup( ) );

    char arg0[ ] = "app";
    char arg1[ ] = "-j";
    char arg2[ ] = "100";
    char arg3[ ] = "-h";
    char * invalid[ ] = { arg0, arg1, arg2, arg3 };
    EXPECT_FALSE( parser.parse_command_line( invalid, 4u ) );

    char arg4[ ] = "10";
    char * valid[ ] = { arg0, arg1, arg4, arg3 };
    EXPECT_TRUE( parser.parse_command_line( valid, 4u ) );
}

//!< Reproduces the defect: a string in the range cleared the error of the earlier string out of the range.
TEST( OptionParserTest, invalid_string_stays_flagged_after_valid_string )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "-c Blah Debug" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    ASSERT_EQ( parser.options( )[ 0u ].inString.size( ), 2u );
}

//!< Strings of the range give no error, in any number and in any order of the range.
TEST( OptionParserTest, strings_in_the_range_give_no_error )
{
    Parser parser( typed_setup( ) );

    EXPECT_TRUE( parser.parse_option_line( "-c Release Debug" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
    EXPECT_EQ( parser.options( )[ 0u ].inString.size( ), 2u );

    EXPECT_FALSE( parser.parse_option_line( "-c Debug Blah" ) );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}

//!< Reproduces the defect: one quote symbol counted as an opening and a closing quote.
TEST( OptionParserTest, single_quote_symbol_is_error )
{
    Parser parser( typed_setup( ) );

    EXPECT_FALSE( parser.parse_option_line( "-s \"" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_TRUE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}

//!< Reproduces the defect: an end of entry symbol overwrote the number before it and flagged the option.
TEST( OptionParserTest, end_of_entry_symbol_is_no_value_of_a_typed_option )
{
    Parser parser( typed_setup( ) );

    EXPECT_TRUE( parser.parse_option_line( "-j 7; -h" ) );
    ASSERT_EQ( parser.options( ).size( ), 2u );
    EXPECT_EQ( parser.options( )[ 0u ].inValue.valInt, 7 );

    EXPECT_TRUE( parser.parse_option_line( "-h; -j 7" ) );
    ASSERT_EQ( parser.options( ).size( ), 2u );
    EXPECT_TRUE( parser.options( )[ 0u ].inString.empty( ) );

    EXPECT_TRUE( parser.parse_option_line( "-h;" ) );

    EXPECT_TRUE( parser.parse_option_line( "-c Debug; Release" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_EQ( parser.options( )[ 0u ].inString.size( ), 2u );
}

//!< The end of entry symbol stays a value of a free string option, its reader splits the entries on it.
TEST( OptionParserTest, end_of_entry_symbol_stays_in_a_free_string_option )
{
    Parser parser( typed_setup( ) );

    EXPECT_TRUE( parser.parse_option_line( "-s a=DEBUG; b=INFO;" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    const auto & values = parser.options( )[ 0u ].inString;
    ASSERT_EQ( values.size( ), 8u );
    EXPECT_TRUE( values[ 3u ] == ";" );
    EXPECT_TRUE( values[ 7u ] == ";" );
}

//!< Two quote symbols are a closed quote of an empty string, no error.
TEST( OptionParserTest, empty_quoted_string_is_no_error )
{
    Parser parser( typed_setup( ) );

    EXPECT_TRUE( parser.parse_option_line( "-s \"\"" ) );
    ASSERT_EQ( parser.options( ).size( ), 1u );
    EXPECT_FALSE( Parser::has_input_error( parser.options( )[ 0u ].inField ) );
}
