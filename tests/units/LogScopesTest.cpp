/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/LogScopesTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Log scopes tests.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"
#include "areg/persist/ConfigListener.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <string_view>

//!< Test-local macros for changing/querying scope priorities (moved from areg_log.h).
#if AREG_LOGGING
    #define SCOPE_PRIORITY_CHANGE(path, method, prio)  areg::set_scope_priority(#path "." #method, static_cast<unsigned int>(prio))
    #define SCOPE_PRIORITY_GET(path, method)           areg::scope_priority(#path "." #method)
#else
    #define SCOPE_PRIORITY_CHANGE(path, method, prio)  ((3-2) > 0)
    #define SCOPE_PRIORITY_GET(path, method)           static_cast<unsigned int>(areg::LogPriority::PrioInvalid)
#endif

namespace {
    //!< The default config file
    constexpr   std::string_view    DEFAULT_CONFIG_FILE { areg::DEFAULT_CONFIG_FILE };

    //!< Config file for testing
    constexpr   std::string_view    TEST_CONFIG_FILE    { "./logs/test_log.init" };

    class TestConfigListener final : public areg::ConfigListener
    {

    protected:
        void prepare_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void post_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void prepare_read_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void on_setup_configuration(const areg::ListProperties& /*listReadonly*/, const areg::ListProperties& /*listWritable*/, areg::ConfigManager& /*config*/) final
        {
        }

        void post_read_configuration(areg::ConfigManager& config) final
        {
            config.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, false, false);
            config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, false);
        }

    };

} // namespace

#define LOG_TEST_SETUP(start)                                                   \
TestConfigListener  logConfig;                                                  \
areg::Application::setup(start, true, false, true, false, nullptr, &logConfig); \
areg::Application::set_working_directory( nullptr );


#define  LOG_TEST_RELEASE()     \
areg::Application::release();

/************************************************************************
 * Testing scopes
 ************************************************************************/

/**
 * \brief   This is a basic test to start and stop the logging.
 **/
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, start_and_stop_logging);
TEST( LogScopeTest, start_and_stop_logging )
{
    LOG_TEST_SETUP(false);

    ASSERT_TRUE( LOGGING_START(DEFAULT_CONFIG_FILE.data()) || !AREG_LOGGING );
    ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGGING );
    
    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, start_and_stop_logging);
        LOG_DBG( "The logging started." );
    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGGING );

    LOG_TEST_RELEASE();
}

/**
 * \brief   This test load and saves the log configuration without a change.
 **/
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, load_and_save_configuration);
TEST( LogScopeTest, load_and_save_configuration )
{
    LOG_TEST_SETUP(false);

    ASSERT_TRUE( LOGGING_START(DEFAULT_CONFIG_FILE.data()) || !AREG_LOGGING );
    ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGGING );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, load_and_save_configuration);
        ASSERT_TRUE( areg::save_logging( TEST_CONFIG_FILE.data( ) ) );
        LOG_DBG( "Successfully saved configuration in the file [ %s ]", TEST_CONFIG_FILE.data( ) );

    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( (AREG_LOGGING == 0) || (IS_LOGGING_STARTED( ) == false) );

    LOG_TEST_RELEASE();
}

/**
 * \brief   This test saves the log configuration and loads again.
 **/
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, load_saved_log_configuration_part1);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, load_saved_log_configuration_part2);
TEST( LogScopeTest, load_saved_log_configuration_part1 )
{
    LOG_TEST_SETUP(false);

    areg::String defaultConfig{ DEFAULT_CONFIG_FILE };
    areg::String testConfig{ TEST_CONFIG_FILE };
    bool isLogEnabled{ false };

    do
    {
        if ( LOGGING_START(defaultConfig) || !AREG_LOGGING)
        {
            LOG_SCOPE( areg_unit_tests_LogScopeTest, load_saved_log_configuration_part1);
            ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGGING );
            isLogEnabled = IS_LOGGING_ENABLED( );
            LOG_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.as_string( ) );

            ASSERT_TRUE( areg::save_logging( testConfig ) );
        }

        LOGGING_STOP( );
        ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGGING );
    } while ( false );

    do
    {
        areg::ConfigManager config;
        config.read_config(testConfig);
        areg::Application::config_manager().replace_module_property(config.module_properties());

        if ( LOGGING_START( testConfig ) || !AREG_LOGGING)
        {
            LOG_SCOPE( areg_unit_tests_LogScopeTest, load_saved_log_configuration_part2);
            ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGGING );
            ASSERT_TRUE(isLogEnabled == IS_LOGGING_ENABLED( ));
            LOG_DBG( ">>> Configured logging from saved file [ %s ]", testConfig.as_string( ) );
        }

        LOGGING_STOP( );
        ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGGING );
    } while ( false );

    LOG_TEST_RELEASE();
}

/**
 * \brief   This test changes the scope priority and logs messages.
 **/
// Define log scopes for testing
DEF_LOG_SCOPE(areg, scope_leaf1);
DEF_LOG_SCOPE(areg, scope_leaf2);
DEF_LOG_SCOPE(areg_node1, scope_leaf1);
DEF_LOG_SCOPE(areg_node2, scope_leaf2);
DEF_LOG_SCOPE(areg_node3, scope_leaf3);
DEF_LOG_SCOPE(areg_node4, scope_leaf4);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_debug);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_info);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_warning);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_error);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_fatal);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_noscope);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_nolog);

TEST( LogScopeTest, change_scope_prio_and_save_config )
{
    LOG_TEST_SETUP(false);

    areg::String defaultConfig{ DEFAULT_CONFIG_FILE };
    areg::String testConfig{ TEST_CONFIG_FILE };

    LOGGING_START( defaultConfig );
    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config);
        ASSERT_TRUE( IS_LOGGING_STARTED() || !AREG_LOGGING );
        LOG_DBG( ">>> Configured logging from default file, going to save in [ %s ]", testConfig.as_string( ) );
        LOG_DBG( "Change logging priorities" );

        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_debug   , PRIO_LOG_ALL ));
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_info    , PRIO_INFO ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_warning , PRIO_WARNING ));
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_error   , PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_fatal   , PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_noscope , PRIO_NOSCOPES( PRIO_WARNING ) ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_nolog   , PRIO_NOLOGS ) );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_debug );
        LOG_DBG( "This scope logs everything" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_info );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_warning );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_error );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error and higher priority messages are visible" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_fatal );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error message is invisible!!!" );
        LOG_FATAL( "This fatal error priority messages is visible" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_noscope );
        LOG_DBG( "This log has no scope output" );
        LOG_INFO( "Check the log output by scope ID" );
        LOG_WARN( "Enabling scope output and you'll see only exit" );
        SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_noscope, PRIO_LOG_ALL );
    } while ( false );

    // bring back the prio settings:
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_noscope, PRIO_NOSCOPES( PRIO_WARNING ) );
    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config_nolog );
        LOG_DBG( "This debug message is invisible!!!" );
        LOG_INFO( "This info message is invisible!!!" );
        LOG_WARN( "This warning message is invisible!!!" );
        LOG_ERR( "This error message is invisible!!!" );
        LOG_FATAL( "This fatal message is invisible!!!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, change_scope_prio_and_save_config );
        LOG_DBG( "Saving current state in the [ %s ] file", testConfig.as_string( ) );
        ASSERT_TRUE( areg::save_logging( testConfig ) );
        LOG_DBG( "Successfully saved [ %s ] file", testConfig.as_string( ) );
    } while ( false );

    LOGGING_STOP( );
    ASSERT_TRUE( !IS_LOGGING_STARTED( ) || !AREG_LOGGING );

    LOG_TEST_RELEASE();
}

/**
 * \brief   This test is checking:
 *              1. the grouping by priority;
 *              2. saves modified scope priority;
 *              3. load scopes from saved config;
 *              4. checks the priority that are set correct.
 **/
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf2);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node2_leaf);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node2_leaf);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode2_no_scope);
DEF_LOG_SCOPE(areg_unit_tests_LogScopeTest, scope_priority_groupping_information);
TEST( LogScopeTest, scope_priority_groupping )
{
    LOG_TEST_SETUP(false);

    areg::String defaultConfig{ DEFAULT_CONFIG_FILE };
    areg::String testConfig{ TEST_CONFIG_FILE };

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
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_information, PRIO_NOSCOPES( PRIO_INFO ) ) );
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_information );
        LOG_DBG ( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Change log priorities   >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( IS_LOGGING_STARTED( ) || !AREG_LOGGING );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf2, PRIO_ERROR ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node2_leaf, PRIO_FATAL ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node2_leaf, PRIO_WARNING ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope, PRIO_NOSCOPES( PRIO_INFO ) ) );
        ASSERT_TRUE( SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode2_no_scope, PRIO_NOSCOPES( PRIO_INFO ) ) );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Make logging                  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1 );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be invisible" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be visible!" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be visible without scopes!" );
        LOG_WARN( "This log should be visible without scopes!" );
        LOG_ERR( "This log should be visible without scopes!" );
        LOG_FATAL( "This log should be visible without scopes!" );
    } while ( false );


    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_information );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Store scope priority  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        information         = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_information );
        errLeaf1            = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1 );
        errLeaf2            = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf2 );
        fatalNode1_leaf     = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf );
        fatalNode2_leaf     = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node2_leaf );
        warnNode1_leaf      = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf );
        warnNode2_leaf      = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node2_leaf );
        infoNode1_noScope   = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope );
        infoNode2_noScope   = SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode2_no_scope );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Saving logging configuration  >>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        ASSERT_TRUE( areg::save_logging( testConfig ) );

        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Stopping logging    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

        LOGGING_STOP( );
        ASSERT_FALSE( IS_LOGGING_STARTED( ) );

        LOG_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_ERR( ">>>>>>>>    This should not be visible!!!  >>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_ERR( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    areg::ConfigManager config;
    config.read_config(testConfig);
    areg::Application::config_manager().replace_module_property(config.module_properties());

    LOGGING_START( testConfig );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_information );
        LOG_DBG( ">>>  Testing debug priority                         >>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>    Restarted logging  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );
        LOG_INFO( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" );

    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1 );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be invisible" );
        LOG_ERR( "This log should be invisible" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be invisible" );
        LOG_WARN( "This log should be visible!" );
        LOG_ERR( "This log should be visible!" );
        LOG_FATAL( "This log should be visible!" );
    } while ( false );

    do
    {
        LOG_SCOPE( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope );
        LOG_DBG( "This log should be invisible" );
        LOG_INFO( "This log should be visible without scopes!" );
        LOG_WARN( "This log should be visible without scopes!" );
        LOG_ERR( "This log should be visible without scopes!" );
        LOG_FATAL( "This log should be visible without scopes!" );
    } while ( false );

    ASSERT_TRUE( (information      == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_information )) || !AREG_LOGGING);
    ASSERT_TRUE( (errLeaf1         == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf1 )) || !AREG_LOGGING);
    ASSERT_TRUE( (errLeaf2         == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_err_leaf2 )) || !AREG_LOGGING);
    ASSERT_TRUE( (fatalNode1_leaf  == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node1_leaf )) || !AREG_LOGGING);
    ASSERT_TRUE( (fatalNode2_leaf  == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_fatal_node2_leaf )) || !AREG_LOGGING);
    ASSERT_TRUE( (warnNode1_leaf   == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node1_leaf )) || !AREG_LOGGING);
    ASSERT_TRUE( (warnNode2_leaf   == SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_warn_node2_leaf )) || !AREG_LOGGING);
    ASSERT_TRUE( (infoNode1_noScope== SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode1_no_scope )) || !AREG_LOGGING);
    ASSERT_TRUE( (infoNode2_noScope== SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, scope_priority_groupping_infoNode2_no_scope )) || !AREG_LOGGING);

    LOGGING_STOP( );
    ASSERT_FALSE( IS_LOGGING_STARTED() );

    LOG_TEST_RELEASE();
}
