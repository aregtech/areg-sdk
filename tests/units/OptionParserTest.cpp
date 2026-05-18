/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
