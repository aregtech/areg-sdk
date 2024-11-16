/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/LogScopesTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              Log scopes tests.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/logging/GELog.h"
#include "areg/appbase/Application.hpp"

#include <string_view>

namespace
{
    //!< The default config file
    constexpr   std::string_view    DEFAULT_CONFIG_FILE { NEApplication::DEFAULT_CONFIG_FILE };

    //!< Config file for testing
    constexpr   std::string_view    TEST_CONFIG_FILE    { "./logs/test_log.init" };
}

/************************************************************************
 * Testing scopes
 ************************************************************************/

/**
 * \brief   This is a basic test to start and stop the logging.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging_exp );
TEST( LogScopeTest, StartAndStopLogging )
{
    Application::setWorkingDirectory( nullptr );
    ASSERT_TRUE( LOGGING_START(DEFAULT_CONFIG_FILE.data()) || !AREG_LOGS );
    ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGS );
    
    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_StartAndStopLogging_exp );
        LOG_DBG( "The logging started." );
    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGS );
}

/**
 * \brief   This test load and saves the log configuration without a change.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadAndSaveConfiguration );
TEST( LogScopeTest, LoadAndSaveConfiguration )
{
    Application::setWorkingDirectory( nullptr );
    ASSERT_TRUE( LOGGING_START(DEFAULT_CONFIG_FILE.data()) || !AREG_LOGS );
    ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGS );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadAndSaveConfiguration );
        ASSERT_TRUE( NELogging::saveLogging( TEST_CONFIG_FILE.data( ) ) );
        LOG_DBG( "Successfully saved configuration in the file [ %s ]", TEST_CONFIG_FILE.data( ) );

    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( (AREG_LOGS == 0) || (IS_LOGGING_STARTED( ) == false) );
}

/**
 * \brief   This test saves the log configuration and loads again.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part1 );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part2 );
TEST( LogScopeTest, LoadSavedLogConfiguration )
{
    Application::setWorkingDirectory( nullptr );
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };
    bool isLogEnabled{ false };

    do
    {
        if ( LOGGING_START(defaultConfig) || !AREG_LOGS)
        {
            LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part1 );
            ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGS );
            isLogEnabled = IS_LOGGING_ENABLED( );
            LOG_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.getString( ) );

            ASSERT_TRUE( NELogging::saveLogging( testConfig ) );
        }

        LOGGING_STOP( );
        ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGS );
    } while ( false );

    do
    {
        ConfigManager config;
        config.readConfig(testConfig);
        Application::getConfigManager().replaceModuleProperty(config.getModuleProperties());

        if ( LOGGING_START( testConfig ) || !AREG_LOGS)
        {
            LOG_SCOPE( areg_unit_tests_LogScopeTest_LoadSavedLogConfiguration_part2 );
            ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGS );
            ASSERT_TRUE(isLogEnabled == IS_LOGGING_ENABLED( ));
            LOG_DBG( ">>> Configured logging from saved file [ %s ]", testConfig.getString( ) );
        }

        LOGGING_STOP( );
        ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGS );
    } while ( false );

}

/**
 * \brief   This test changes the scope priority and logs messages.
 **/
// Define log scopes for testing
DEF_LOG_SCOPE( areg_scopeLeaf1 );
DEF_LOG_SCOPE( areg_scopeLeaf2 );
DEF_LOG_SCOPE( areg_node1_scopeLeaf1 );
DEF_LOG_SCOPE( areg_node2_scopeLeaf2 );
DEF_LOG_SCOPE( areg_node3_scopeLeaf3 );
DEF_LOG_SCOPE( areg_node4_scopeLeaf4 );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_debug );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_info );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_warning );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_error );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_fatal );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_nolog );

TEST( LogScopeTest, ChangeScopePrioAndSaveConfig )
{
    Application::setWorkingDirectory( nullptr );
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };

    LOGGING_START( defaultConfig );
    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
        ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGS );
        LOG_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.getString( ) );
        LOG_DBG( "Change logging priorities" );

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
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_debug );
        LOG_DBG( "This scope logs everything" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_info );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_warning );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_error );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_fatal );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error message is invisible!!!" );
        LOG_FATAL( "This fatal error priority messages is visible" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope );
        LOG_DBG( "This log has no scope output" );
        LOG_INFO( "Check the log output by scope ID" );
        LOG_WARN( "Enabling scope output and you'll see only exit" );
        SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope, PRIO_LOG_ALL );
    } while ( false );

    // bring back the prio settings:
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_noscope, PRIO_NOSCOPES( PRIO_WARNING ) );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig_nolog );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error message is invisible!!!" );
        LOG_FATAL( "This fatal message is invisible!!!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ChangeScopePrioAndSaveConfig );
        LOG_DBG( "Saving current state in the [ %s ] file", testConfig.getString( ) );
        ASSERT_TRUE( NELogging::saveLogging( testConfig ) );
        LOG_DBG( "Successfully saved [ %s ] file", testConfig.getString( ) );
    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGS );
}

/**
 * \brief   This test is checking:
 *              1. the grouping by priority;
 *              2. saves modified scope priority;
 *              3. load scopes from saved config;
 *              4. checks the priority that are set correct.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2 );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
TEST( LogScopeTest, ScopePriorityGroupping )
{
    Application::setWorkingDirectory( nullptr );
    String defaultConfig{ DEFAULT_CONFIG_FILE };
    String testConfig{ TEST_CONFIG_FILE };

    uint32_t information{ 0 };
    uint32_t errLeaf1{ 0 };
    uint32_t errLeaf2{ 0 };
    uint32_t fatalNode1_leaf{ 0 };
    uint32_t fatalNode2_leaf{ 0 };
    uint32_t warnNode1_leaf{ 0 };
    uint32_t warnNode2_leaf{ 0 };
    uint32_t infoNode1_noScope{ 0 };
    uint32_t infoNode2_noScope{ 0 };

    LOGGING_START( defaultConfig );

    do
    {
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information, PRIO_NOSCOPES( PRIO_INFO ) ) );
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        LOG_DBG ( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Change log priorities   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGS );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope, PRIO_NOSCOPES( PRIO_INFO ) ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope, PRIO_NOSCOPES( PRIO_INFO ) ) );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Make logging                  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be invisible" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be visible!" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be visible without scopes!" );
        LOG_WARN( "This log should be visible without scopes!" );
        LOG_ERR( "This log should be visible without scopes!" );
        LOG_FATAL( "This log should be visible without scopes!" );
    } while ( false );


    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Store scope priority  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        information         = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        errLeaf1            = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
        errLeaf2            = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2 );
        fatalNode1_leaf     = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
        fatalNode2_leaf     = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf );
        warnNode1_leaf      = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
        warnNode2_leaf      = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf );
        infoNode1_noScope   = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
        infoNode2_noScope   = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Saving logging configuration  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( NELogging::saveLogging( testConfig ) );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Stopping logging    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        LOGGING_STOP( );
        ASSERT_FALSE( IS_LOGGING_STARTED( ) );

        LOG_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_ERR( ">>>>>>>>    This should not be visible!!!  >>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    ConfigManager config;
    config.readConfig(testConfig);
    Application::getConfigManager().replaceModuleProperty(config.getModuleProperties());

    LOGGING_START( testConfig );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information );
        LOG_DBG( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Restarted logging  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1 );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be invisible" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be visible!" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be visible without scopes!" );
        LOG_WARN( "This log should be visible without scopes!" );
        LOG_ERR( "This log should be visible without scopes!" );
        LOG_FATAL( "This log should be visible without scopes!" );
    } while ( false );

    ASSERT_TRUE( (information      == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_information)) || !AREG_LOGS);
    ASSERT_TRUE( (errLeaf1         == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf1)) || !AREG_LOGS);
    ASSERT_TRUE( (errLeaf2         == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_errLeaf2)) || !AREG_LOGS);
    ASSERT_TRUE( (fatalNode1_leaf  == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode1_leaf)) || !AREG_LOGS);
    ASSERT_TRUE( (fatalNode2_leaf  == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_fatalNode2_leaf)) || !AREG_LOGS);
    ASSERT_TRUE( (warnNode1_leaf   == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode1_leaf)) || !AREG_LOGS);
    ASSERT_TRUE( (warnNode2_leaf   == SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_warnNode2_leaf)) || !AREG_LOGS);
    ASSERT_TRUE( (infoNode1_noScope== SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode1_noScope)) || !AREG_LOGS);
    ASSERT_TRUE( (infoNode2_noScope== SCOPE_PRIORITY_GET(areg_unit_tests_LogScopeTest_ScopePriorityGroupping_infoNode2_noScope)) || !AREG_LOGS);

    LOGGING_STOP( );
    ASSERT_FALSE( IS_LOGGING_STARTED() );
}
