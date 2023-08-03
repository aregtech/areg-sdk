/************************************************************************
 * \file        tests/units/DemoTest.Cpp
 * \ingroup     platform dependent files
 * \brief       AREG Framework, DemoTest to check the AREG engine and
 *              google test compilation
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/Thread.hpp"

namespace
{
    //!< The default config file
    constexpr   std::string_view    DEFAULT_CONFIG_FILE { NEApplication::DEFAULT_TRACING_CONFIG_FILE };

    //!< Config file for testing
    constexpr   std::string_view    TEST_CONFIG_FILE    { "./logs/test_log.init" };
}

/************************************************************************
 * Testing scopes
 ************************************************************************/

/**
 * \brief   This is a basic test to start and stop the logging.
 **/
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging );
TEST( LogScopeTest, StartAndStopLogging )
{
    ASSERT_TRUE( TRACER_START_LOGGING( DEFAULT_CONFIG_FILE.data( ) ) );
    ASSERT_TRUE( IS_TRACE_STARTED( ) );
    
    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging );
        TRACE_DBG( "The logging started." );
    } while ( false );

    TRACER_STOP_LOGGING( );
    ASSERT_FALSE( IS_TRACE_STARTED( ) );
}

/**
 * \brief   This test load and saves the log configuration without a change.
 **/
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadAndSaveConfiguration );
TEST( LogScopeTest, LoadAndSaveConfiguration )
{
    ASSERT_TRUE( TRACER_START_LOGGING( DEFAULT_CONFIG_FILE.data( ) ) );
    ASSERT_TRUE( IS_TRACE_STARTED( ) );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadAndSaveConfiguration );
        ASSERT_TRUE( NETrace::saveLogging( TEST_CONFIG_FILE.data( ) ) );
        TRACE_DBG( "Successfully saved configuration in the file [ %s ]", TEST_CONFIG_FILE.data( ) );

    } while ( false );

    TRACER_STOP_LOGGING( );
    ASSERT_FALSE( IS_TRACE_STARTED( ) );
}

/**
 * \brief   This test saves the log configuration and loads again.
 **/
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part1 );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part2 );
TEST( LogScopeTest, LoadSavedLogConfiguration )
{
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };
    bool isLogEnabled{ false };

    do
    {
        if ( TRACER_START_LOGGING( defaultConfig ) )
        {
            TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part1 );
            ASSERT_TRUE( IS_TRACE_STARTED( ) );
            isLogEnabled = IS_LOG_ENABLED( );
            TRACE_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.getString( ) );

            ASSERT_TRUE( NETrace::saveLogging( testConfig ) );
        }

        TRACER_STOP_LOGGING( );
        ASSERT_FALSE( IS_TRACE_STARTED( ) );
    } while ( false );

    do
    {
        if ( TRACER_START_LOGGING( testConfig ) )
        {
            TRACE_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part2 );
            ASSERT_TRUE( IS_TRACE_STARTED( ) );
            ASSERT_TRUE(isLogEnabled == IS_LOG_ENABLED( ));
            TRACE_DBG( ">>> Configured logging from saved file [ %s ]", testConfig.getString( ) );
        }

        TRACER_STOP_LOGGING( );
        ASSERT_FALSE( IS_TRACE_STARTED( ) );
    } while ( false );

}

/**
 * \brief   This test changes the scope priority and logs messages.
 **/
// Define trace scopes for testing
DEF_TRACE_SCOPE( areg_scopeLeaf1 );
DEF_TRACE_SCOPE( areg_scopeLeaf2 );
DEF_TRACE_SCOPE( areg_node1_scopeLeaf1 );
DEF_TRACE_SCOPE( areg_node2_scopeLeaf2 );
DEF_TRACE_SCOPE( areg_node3_scopeLeaf3 );
DEF_TRACE_SCOPE( areg_node4_scopeLeaf4 );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_debug );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_info );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_warning );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_error );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_fatal );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_nolog );
TEST( LogScopeTest, ChangeScopePrioAndSaveConfig )
{
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };

    TRACER_START_LOGGING( defaultConfig );
    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
        ASSERT_TRUE( IS_TRACE_STARTED( ) );
        TRACE_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.getString( ) );
        TRACE_DBG( "Change logging priorities" );

        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_debug, PRIO_LOG_ALL ));
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_info, PRIO_INFO ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_warning, PRIO_WARNING ));
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_error, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_fatal, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope, PRIO_NOSCOPES( PRIO_WARNING ) ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_nolog, PRIO_NOLOGS ) );

    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_debug );
        TRACE_DBG( "This scope logs everything" );

    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_info );
        TRACE_DBG( "This debug message is invisible!!!" );
        TRACE_INFO( "This info and higher priority messages are visible" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_warning );
        TRACE_DBG( "This debug message is invisible!!!" );
        TRACE_INFO( "This info message is invisible!!!" );
        TRACE_WARN( "This warning and higher priority messages are visible" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_error );
        TRACE_DBG( "This debug message is invisible!!!" );
        TRACE_INFO( "This info message is invisible!!!" );
        TRACE_WARN( "This warning message is invisible!!!" );
        TRACE_ERR( "This error and higher priority messages are visible" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_fatal );
        TRACE_DBG( "This debug message is invisible!!!" );
        TRACE_INFO( "This info message is invisible!!!" );
        TRACE_WARN( "This warning message is invisible!!!" );
        TRACE_ERR( "This error message is invisible!!!" );
        TRACE_FATAL( "This fatal error priority messages is visible" );

    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope );
        TRACE_DBG( "This log has no scope output" );
        TRACE_INFO( "Check the log output by scope ID" );
        TRACE_WARN( "Enabling scope output and you'll see only exit" );
        SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope, PRIO_LOG_ALL );
    } while ( false );

    // bring back the prio settings:
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope, PRIO_NOSCOPES( PRIO_WARNING ) );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_nolog );
        TRACE_DBG( "This debug message is invisible!!!" );
        TRACE_INFO( "This info message is invisible!!!" );
        TRACE_WARN( "This warning message is invisible!!!" );
        TRACE_ERR( "This error message is invisible!!!" );
        TRACE_FATAL( "This fatal message is invisible!!!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
        TRACE_DBG( "Saving current state in the [ %s ] file", testConfig.getString( ) );
        ASSERT_TRUE( NETrace::saveLogging( testConfig ) );
        TRACE_DBG( "Successfully saved [ %s ] file", testConfig.getString( ) );
    } while ( false );

    TRACER_STOP_LOGGING( );
    ASSERT_FALSE( IS_TRACE_STARTED( ) );
}

/**
 * \brief   This test is checking the grouping by priority.
 **/
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2 );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope );
DEF_TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
TEST( LogScopeTest, ScopePriorityGroupping )
{
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };

    TRACER_START_LOGGING( defaultConfig );

    do
    {
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information, PRIO_NOSCOPES( PRIO_INFO ) ) );
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        TRACE_DBG ( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>    Change log priorities   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( IS_TRACE_STARTED( ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope, PRIO_NOSCOPES( PRIO_INFO ) ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope, PRIO_NOSCOPES( PRIO_INFO ) ) );

        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>    Make logging                  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should be invisible" );
        TRACE_ERR( "This log should visible!" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should be invisible" );
        TRACE_ERR( "This log should be invisible" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should visible!" );
        TRACE_ERR( "This log should visible!" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should visible without scopes!" );
        TRACE_WARN( "This log should visible without scopes!" );
        TRACE_ERR( "This log should visible without scopes!" );
        TRACE_FATAL( "This log should visible without scopes!" );
    } while ( false );


    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>    Saving logging configuration  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( NETrace::saveLogging( testConfig ) );

        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>    Stopping logging    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        TRACER_STOP_LOGGING( );
        ASSERT_FALSE( IS_TRACE_STARTED( ) );

        TRACE_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_ERR( ">>>>>>>>    This should not be visible!!!  >>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    TRACER_START_LOGGING( testConfig );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        TRACE_DBG( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>    Restarted logging  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        TRACE_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should be invisible" );
        TRACE_ERR( "This log should visible!" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should be invisible" );
        TRACE_ERR( "This log should be invisible" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should be invisible" );
        TRACE_WARN( "This log should visible!" );
        TRACE_ERR( "This log should visible!" );
        TRACE_FATAL( "This log should visible!" );
    } while ( false );

    do
    {
        TRACE_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
        TRACE_DBG( "This log should be invisible" );
        TRACE_INFO( "This log should visible without scopes!" );
        TRACE_WARN( "This log should visible without scopes!" );
        TRACE_ERR( "This log should visible without scopes!" );
        TRACE_FATAL( "This log should visible without scopes!" );
    } while ( false );

    TRACER_STOP_LOGGING( );
    ASSERT_FALSE( IS_TRACE_STARTED( ) );
}
