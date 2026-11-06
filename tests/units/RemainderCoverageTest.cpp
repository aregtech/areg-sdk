/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/RemainderCoverageTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of memory file state and text lines, and of the log
 *              scope nodes, log actions and log messages that no other test reaches.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/base/FatalError.hpp"
#include "areg/base/FileBuffer.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/String.hpp"
#include "areg/logging/LoggingDefs.hpp"

#if AREG_LOGGING
    #include "areg/logging/private/LoggingEvent.hpp"
    #include "areg/logging/private/ScopeNodes.hpp"
#endif  // AREG_LOGGING

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <string>

namespace
{
    using OpenMode = areg::FileBase::OpenMode;

    constexpr uint32_t MODE_BINARY{ static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Binary) };
    constexpr uint32_t MODE_TEXT  { static_cast<uint32_t>(OpenMode::Write) | static_cast<uint32_t>(OpenMode::Text) };

    //!< A fatal handler that returns, so the caller must still abort.
    void returning_fatal_handler( const char * /* reason */ ) noexcept
    {
    }

    //!< Writes the text as is and moves the cursor to the begin.
    void put_text( areg::FileBuffer & file, const char * text )
    {
        const uint32_t len{ static_cast<uint32_t>(std::strlen( text )) };
        ASSERT_EQ( file.write( reinterpret_cast<const uint8_t *>(text), len ), len );
        ASSERT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), 0u );
    }
}

//////////////////////////////////////////////////////////////////////////
// Memory file state
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A closed memory file has no length; an open one reports the used size.
 **/
TEST( RemainderCoverageTest, memory_file_reports_its_length_only_while_open )
{
    areg::FileBuffer file( MODE_BINARY );
    EXPECT_EQ( file.length( ), areg::INVALID_SIZE );

    ASSERT_TRUE( file.open( ) );
    EXPECT_EQ( file.length( ), 0u );

    const uint8_t data[3]{ 1, 2, 3 };
    ASSERT_EQ( file.write( data, sizeof( data ) ), 3u );
    EXPECT_EQ( file.length( ), 3u );
}

/**
 * \brief   reserve() below the used size cuts the used size; above the capacity it keeps the cursor.
 **/
TEST( RemainderCoverageTest, memory_file_reserve_shrinks_the_data_and_keeps_the_cursor_when_it_grows )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[8]{ 1, 2, 3, 4, 5, 6, 7, 8 };
    ASSERT_EQ( file.write( data, sizeof( data ) ), 8u );

    EXPECT_NE( file.reserve( 4u ), areg::INVALID_SIZE );
    EXPECT_EQ( file.length( ), 4u );

    ASSERT_EQ( file.set_position( 2, areg::Cursor::SeekOrigin::Begin ), 2u );
    EXPECT_EQ( file.reserve( 1024u * 1024u ), 2u );
    EXPECT_EQ( file.position( ), 2u );
    EXPECT_EQ( file.length( ), 4u );
}

/**
 * \brief   An owner removes its buffer: the file is closed and has no length.
 **/
TEST( RemainderCoverageTest, memory_file_remove_closes_an_owning_file )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    EXPECT_TRUE( file.remove( ) );
    EXPECT_FALSE( file.is_opened( ) );
    EXPECT_EQ( file.length( ), areg::INVALID_SIZE );
}

/**
 * \brief   write_invert() stores the bytes in reverse order; an empty request stores nothing.
 **/
TEST( RemainderCoverageTest, write_invert_stores_the_bytes_in_reverse_order )
{
    areg::FileBuffer file( MODE_BINARY );
    ASSERT_TRUE( file.open( ) );
    const uint8_t data[4]{ 1, 2, 3, 4 };
    EXPECT_EQ( file.write_invert( data, sizeof( data ) ), 4 );
    EXPECT_EQ( file.write_invert( data, 0u ), 0 );
    EXPECT_EQ( file.write_invert( nullptr, 4u ), 0 );
    ASSERT_EQ( file.set_position( 0, areg::Cursor::SeekOrigin::Begin ), 0u );

    uint8_t out[4]{ };
    ASSERT_EQ( file.read( out, sizeof( out ) ), 4u );
    EXPECT_EQ( out[0], 4u );
    EXPECT_EQ( out[1], 3u );
    EXPECT_EQ( out[2], 2u );
    EXPECT_EQ( out[3], 1u );
}

/**
 * \brief   write_invert() on a closed file writes nothing.
 **/
TEST( RemainderCoverageTest, write_invert_on_a_closed_file_writes_nothing )
{
    areg::FileBuffer file( MODE_BINARY );
    const uint8_t data[2]{ 1, 2 };
    EXPECT_EQ( file.write_invert( data, sizeof( data ) ), 0 );
}

//////////////////////////////////////////////////////////////////////////
// Text lines
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   A CR LF pair ends one line and is consumed together.
 **/
TEST( RemainderCoverageTest, read_line_consumes_a_carriage_return_line_feed_pair_as_one_line_end )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    put_text( file, "ab\r\ncd" );

    areg::String line;
    EXPECT_EQ( file.read_line( line ), 4 );
    EXPECT_STREQ( line.as_string( ), "ab" );
    EXPECT_EQ( file.read_line( line ), 2 );
    EXPECT_STREQ( line.as_string( ), "cd" );
    EXPECT_EQ( file.read_line( line ), 0 );
}

/**
 * \brief   A lone CR and a lone LF each end a line.
 **/
TEST( RemainderCoverageTest, read_line_ends_a_line_at_a_lone_carriage_return_and_at_a_lone_line_feed )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    put_text( file, "a\rb\nc" );

    areg::String line;
    EXPECT_EQ( file.read_line( line ), 2 );
    EXPECT_STREQ( line.as_string( ), "a" );
    EXPECT_EQ( file.read_line( line ), 2 );
    EXPECT_STREQ( line.as_string( ), "b" );
    EXPECT_EQ( file.read_line( line ), 1 );
    EXPECT_STREQ( line.as_string( ), "c" );
}

/**
 * \brief   A CR as the last byte of the data ends the line and leaves nothing to read.
 **/
TEST( RemainderCoverageTest, read_line_handles_a_carriage_return_at_the_end_of_the_data )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    put_text( file, "x\r" );

    areg::String line;
    EXPECT_EQ( file.read_line( line ), 2 );
    EXPECT_STREQ( line.as_string( ), "x" );
    EXPECT_EQ( file.read_line( line ), 0 );
}

/**
 * \brief   A line read into a character array stops at the capacity and terminates the text.
 **/
TEST( RemainderCoverageTest, read_line_into_an_array_is_cut_at_the_capacity_and_terminated )
{
    areg::FileBuffer file( MODE_TEXT );
    ASSERT_TRUE( file.open( ) );
    put_text( file, "abcdef" );

    char out[4]{ 'x', 'x', 'x', 'x' };
    EXPECT_EQ( file.read_line( out, 4 ), 3 );
    EXPECT_STREQ( out, "abc" );
    EXPECT_EQ( file.read_line( static_cast<char *>(nullptr), 4 ), 0 );
    EXPECT_EQ( file.read_line( out, 1 ), 0 );
}

//////////////////////////////////////////////////////////////////////////
// Log actions, messages and scope nodes
//////////////////////////////////////////////////////////////////////////

#if AREG_LOGGING

/**
 * \brief   Every log action has its own name.
 **/
TEST( RemainderCoverageTest, every_log_action_has_a_distinct_name )
{
    using Action = areg::LoggingEventData::LogAction;
    const Action actions[]
    {
          Action::Undefined, Action::StartLogs, Action::StopLogs, Action::EnableLogs, Action::DisableLogs
        , Action::SaveScopes, Action::UpdateScopes, Action::QueryScopes, Action::OpenRemote
    };

    std::string seen;
    for ( const Action action : actions )
    {
        const std::string name{ areg::LoggingEventData::as_string( action ) };
        EXPECT_EQ( name.rfind( "LoggingEventData::LogAction::", 0u ), 0u ) << name;
        EXPECT_EQ( seen.find( name + "|" ), std::string::npos ) << name;
        seen += name + "|";
    }
}

/**
 * \brief   A leaf has no children: adding one is refused and the path and grouping are unchanged.
 **/
TEST( RemainderCoverageTest, scope_leaf_refuses_children_and_keeps_the_path )
{
    areg::ScopeLeaf leaf;
    areg::ScopeLeaf other;
    EXPECT_FALSE( leaf.add_child_node( other ).second );

    areg::String path( "a_b" );
    EXPECT_FALSE( leaf.add_child_node( path, 0u ).second );
    EXPECT_EQ( leaf.group_child_nodes( ), 0u );
    EXPECT_TRUE( leaf.is_leaf( ) );
}

/**
 * \brief   Nodes order by type first, then by name; equal type and name are equal.
 **/
TEST( RemainderCoverageTest, scope_nodes_order_by_type_then_by_name )
{
    areg::ScopeLeaf leafA;
    areg::ScopeLeaf leafB;
    leafA.set_node_name( areg::String( "a" ) );
    leafB.set_node_name( areg::String( "b" ) );
    areg::ScopeNode node;
    node.set_node_name( areg::String( "a" ) );

    EXPECT_TRUE( leafA < leafB );
    EXPECT_TRUE( leafB > leafA );
    EXPECT_TRUE( leafA != leafB );
    EXPECT_TRUE( leafB < node );
    EXPECT_TRUE( node > leafB );
    EXPECT_FALSE( node < leafA );

    areg::ScopeLeaf copy( leafA );
    EXPECT_TRUE( copy == leafA );
}

/**
 * \brief   extract_node_name() cuts the first node name and leaves the rest.
 **/
namespace
{
    //!< Gives the tests access to the protected helper of the node base.
    class NodeProbe : public areg::ScopeNode
    {
    public:
        static areg::String extract( areg::String & path )
        {
            return extract_node_name( path );
        }
    };
}

TEST( RemainderCoverageTest, extract_node_name_splits_at_the_first_separator )
{
    areg::String path( "mod_sub_leaf" );
    EXPECT_STREQ( NodeProbe::extract( path ).as_string( ), "mod" );
    EXPECT_STREQ( path.as_string( ), "sub_leaf" );

    areg::String leading( "__mod_x" );
    EXPECT_STREQ( NodeProbe::extract( leading ).as_string( ), "__mod" );
    EXPECT_STREQ( leading.as_string( ), "x" );

    areg::String plain( "only" );
    EXPECT_STREQ( NodeProbe::extract( plain ).as_string( ), "only" );
    EXPECT_TRUE( plain.is_empty( ) );
}

#endif  // AREG_LOGGING

//////////////////////////////////////////////////////////////////////////
// Fatal error
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   The default fatal handler prints the reason to the error output and aborts.
 **/
TEST( RemainderCoverageDeathTest, default_fatal_handler_prints_the_reason_and_aborts )
{
    AREG_EXPECT_CHILD_DIES(
        { static_cast<void>(std::signal( SIGABRT, SIG_DFL )); static_cast<void>(areg::set_fatal_handler( nullptr )); areg::fatal_error( "reason-text" ); }
      , "areg fatal error: reason-text" );
}

/**
 * \brief   A missing reason is reported as unknown.
 **/
TEST( RemainderCoverageDeathTest, default_fatal_handler_reports_a_missing_reason_as_unknown )
{
    AREG_EXPECT_CHILD_DIES(
        { static_cast<void>(std::signal( SIGABRT, SIG_DFL )); static_cast<void>(areg::set_fatal_handler( nullptr )); areg::fatal_error( nullptr ); }
      , "areg fatal error: unknown" );
}

/**
 * \brief   A handler that returns does not return the process: fatal_error() aborts.
 **/
TEST( RemainderCoverageDeathTest, fatal_error_aborts_when_the_handler_returns )
{
    AREG_EXPECT_CHILD_DIES(
        { static_cast<void>(std::signal( SIGABRT, SIG_DFL )); static_cast<void>(areg::set_fatal_handler( &returning_fatal_handler )); areg::fatal_error( "returned" ); }
      , "" );
}

#if !defined(_WIN32)

/**
 * \brief   A lock failure is fatal when AREG_LOCK_FAILURE_FATAL is 1.
 **/
TEST( RemainderCoverageDeathTest, lock_failure_is_fatal_when_the_environment_asks_for_it )
{
    AREG_EXPECT_CHILD_DIES(
        { static_cast<void>(std::signal( SIGABRT, SIG_DFL )); static_cast<void>(::setenv( "AREG_LOCK_FAILURE_FATAL", "1", 1 )); static_cast<void>(areg::set_fatal_handler( nullptr )); areg::report_lock_failure( "lock-text" ); }
      , "areg fatal error: lock-text" );
}

#endif  // !defined(_WIN32)
