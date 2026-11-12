/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/base/DateTime.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/StringDefs.hpp"
#include "areg/logging/private/LoggingEvent.hpp"
#include "areg/logging/private/ScopeController.hpp"
#include "areg/logging/private/ScopeNodes.hpp"

#include <algorithm>
#include <cstring>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

//!< Test-local macros for changing/querying scope priorities (moved from areg_log.h).
#if AREG_LOGGING
    #define SCOPE_PRIORITY_CHANGE(path, method, prio)  areg::set_scope_priority(#path "." #method, static_cast<unsigned int>(prio))
    #define SCOPE_PRIORITY_GET(path, method)           areg::scope_priority(#path "." #method)

namespace {
    //!< The default config file
    constexpr   std::string_view    DEFAULT_CONFIG_FILE { areg::DEFAULT_CONFIG_FILE };

    //!< Returns the config file path owned by the running test.
    areg::String test_config_file()
    {
        return areg::String("./logs/") + ::testing::UnitTest::GetInstance()->current_test_info()->name() + ".init";
    }

    //!< Returns the entries of the running process that a configuration read from its file.
    areg::ListProperties module_entries(const areg::ConfigManager& config)
    {
        areg::ListProperties result;
        for (const auto& prop : config.readonly_properties().data())
        {
            if (prop.key().is_all_modules() == false)
            {
                result.add(prop);
            }
        }

        return result;
    }

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
        const areg::String testConfig{ test_config_file( ) };
        ASSERT_TRUE( areg::save_logging( testConfig ) );
        LOG_DBG( "Successfully saved configuration in the file [ %s ]", testConfig.as_string( ) );

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
    areg::String testConfig{ test_config_file( ) };
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
        areg::Application::config_manager().replace_module_property(module_entries(config));

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
    areg::String testConfig{ test_config_file( ) };

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
    areg::String testConfig{ test_config_file( ) };

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
    areg::Application::config_manager().replace_module_property(module_entries(config));

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

/**
 * \brief   Stopping the source sets every scope priority to PrioNotset and resuming sets back
 *          exactly the priorities the scopes had before the stop.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_first );
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_second );
TEST( LogScopeTest, source_state_stop_and_resume )
{
    LOG_TEST_SETUP( false );
    ASSERT_TRUE( LOGGING_START( DEFAULT_CONFIG_FILE.data( ) ) || !AREG_LOGGING );

    constexpr uint32_t prioFirst { static_cast<uint32_t>(areg::LogPriority::PrioDebug) | static_cast<uint32_t>(areg::LogPriority::PrioScope) };
    constexpr uint32_t prioSecond{ static_cast<uint32_t>(areg::LogPriority::PrioWarning) };
    constexpr uint32_t prioNotset{ static_cast<uint32_t>(areg::LogPriority::PrioNotset) };

    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_first , prioFirst );
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_second, prioSecond );

    ASSERT_EQ( areg::LogSourceState::Stopped, areg::set_source_state( areg::LogSourceState::Stopped ) );
    ASSERT_EQ( areg::LogSourceState::Stopped, areg::source_state( ) );
    ASSERT_EQ( prioNotset, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_first ) );
    ASSERT_EQ( prioNotset, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_second ) );

    ASSERT_EQ( areg::LogSourceState::Active, areg::set_source_state( areg::LogSourceState::Active ) );
    ASSERT_EQ( areg::LogSourceState::Active, areg::source_state( ) );
    ASSERT_EQ( prioFirst , SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_first ) );
    ASSERT_EQ( prioSecond, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_stop_and_resume_second ) );

    LOGGING_STOP( );
    LOG_TEST_RELEASE( );
}

/**
 * \brief   Pausing the source does not touch the scope priorities, stopping does.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest, source_state_pause_keeps_priority );
TEST( LogScopeTest, source_state_pause_keeps_priority )
{
    LOG_TEST_SETUP( false );
    ASSERT_TRUE( LOGGING_START( DEFAULT_CONFIG_FILE.data( ) ) || !AREG_LOGGING );

    constexpr uint32_t prio{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_pause_keeps_priority, prio );

    ASSERT_EQ( areg::LogSourceState::Paused, areg::set_source_state( areg::LogSourceState::Paused ) );
    ASSERT_EQ( areg::LogSourceState::Paused, areg::source_state( ) );
    ASSERT_EQ( prio, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_pause_keeps_priority ) );

    ASSERT_EQ( areg::LogSourceState::Active, areg::set_source_state( areg::LogSourceState::Active ) );
    ASSERT_EQ( prio, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_pause_keeps_priority ) );

    LOGGING_STOP( );
    LOG_TEST_RELEASE( );
}

/**
 * \brief   An explicit priority set while the source is stopped stays, and the source leaves the
 *          stopped state, so a later resume does not put the saved priority back.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest, source_state_priority_cancels_stop );
TEST( LogScopeTest, source_state_priority_cancels_stop )
{
    LOG_TEST_SETUP( false );
    ASSERT_TRUE( LOGGING_START( DEFAULT_CONFIG_FILE.data( ) ) || !AREG_LOGGING );

    constexpr uint32_t prioBefore{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    constexpr uint32_t prioAfter { static_cast<uint32_t>(areg::LogPriority::PrioError) };

    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_priority_cancels_stop, prioBefore );
    ASSERT_EQ( areg::LogSourceState::Stopped, areg::set_source_state( areg::LogSourceState::Stopped ) );

    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_priority_cancels_stop, prioAfter );
    ASSERT_EQ( areg::LogSourceState::Active, areg::source_state( ) );
    ASSERT_EQ( prioAfter, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_priority_cancels_stop ) );

    ASSERT_EQ( areg::LogSourceState::Active, areg::set_source_state( areg::LogSourceState::Active ) );
    ASSERT_EQ( prioAfter, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_priority_cancels_stop ) );

    LOGGING_STOP( );
    LOG_TEST_RELEASE( );
}

/**
 * \brief   Restoring the configuration applies the saved priorities and leaves the stopped state.
 **/
DEF_LOG_SCOPE( areg_unit_tests_LogScopeTest, source_state_restore_cancels_stop );
TEST( LogScopeTest, source_state_restore_cancels_stop )
{
    LOG_TEST_SETUP( false );
    ASSERT_TRUE( LOGGING_START( DEFAULT_CONFIG_FILE.data( ) ) || !AREG_LOGGING );

    const uint32_t prioSaved{ SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_restore_cancels_stop ) };
    SCOPE_PRIORITY_CHANGE( areg_unit_tests_LogScopeTest, source_state_restore_cancels_stop, static_cast<uint32_t>(areg::LogPriority::PrioDebug) );

    ASSERT_EQ( areg::LogSourceState::Stopped, areg::set_source_state( areg::LogSourceState::Stopped ) );
    areg::restore_logging( );

    ASSERT_EQ( areg::LogSourceState::Active, areg::source_state( ) );
    ASSERT_EQ( prioSaved, SCOPE_PRIORITY_GET( areg_unit_tests_LogScopeTest, source_state_restore_cancels_stop ) );

    LOGGING_STOP( );
    LOG_TEST_RELEASE( );
}

/**
 * \brief   An undefined state leaves the source untouched.
 **/
TEST( LogScopeTest, source_state_rejects_undefined )
{
    LOG_TEST_SETUP( false );
    ASSERT_TRUE( LOGGING_START( DEFAULT_CONFIG_FILE.data( ) ) || !AREG_LOGGING );

    ASSERT_EQ( areg::LogSourceState::Paused, areg::set_source_state( areg::LogSourceState::Paused ) );
    ASSERT_EQ( areg::LogSourceState::Paused, areg::set_source_state( areg::LogSourceState::Undefined ) );
    ASSERT_EQ( areg::LogSourceState::Paused, areg::source_state( ) );

    ASSERT_EQ( areg::LogSourceState::Active, areg::set_source_state( areg::LogSourceState::Active ) );

    LOGGING_STOP( );
    LOG_TEST_RELEASE( );
}

// Known priority names convert correctly, while unknown names return the invalid value.
TEST(LoggingCoreTest, canonical_priorities_convert_and_unknown_names_are_explicit)
{
    const struct
    {
        areg::LogPriority priority;
        const char* name;
    } cases[]
    {
          { areg::LogPriority::PrioNotset, "NOTSET" }
        , { areg::LogPriority::PrioScope, "SCOPE" }
        , { areg::LogPriority::PrioFatal, "FATAL" }
        , { areg::LogPriority::PrioError, "ERROR" }
        , { areg::LogPriority::PrioWarning, "WARN" }
        , { areg::LogPriority::PrioInfo, "INFO" }
        , { areg::LogPriority::PrioDebug, "DEBUG" }
    };
    for (const auto& entry : cases)
    {
        EXPECT_EQ(areg::priority_to_string(entry.priority), entry.name);
        EXPECT_EQ(areg::string_to_priority(areg::String(entry.name)), entry.priority);
        EXPECT_EQ(areg::make_priorities(areg::String(entry.name)), static_cast<uint32_t>(entry.priority));
        EXPECT_EQ(areg::make_prio_string(static_cast<uint32_t>(entry.priority)), entry.name);
    }
    EXPECT_EQ(areg::string_to_priority(areg::String("unknown")), areg::LogPriority::PrioIgnoreLayout);
    EXPECT_EQ(areg::make_priorities(areg::String("unknown")), static_cast<uint32_t>(areg::LogPriority::PrioInvalid));
    EXPECT_TRUE(areg::priority_to_string(areg::LogPriority::PrioIgnoreLayout).is_empty());
    EXPECT_TRUE(areg::priority_to_string(areg::LogPriority::PrioInvalid).is_empty());
}

// Formatting a priority mask retains its enabled bits in severity order.
TEST(LoggingCoreTest, priority_list_preserves_all_enabled_bits_in_severity_order)
{
    const auto list{ areg::priority_to_string_list(static_cast<uint32_t>(areg::LogPriority::PrioScopeLogs)) };
    const char* expected[]{ "SCOPE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
    ASSERT_EQ(list.size(), 6u);
    for (uint32_t index{ 0u }; index < list.size(); ++index)
    {
        EXPECT_EQ(list[index], expected[index]);
    }
    EXPECT_TRUE(areg::priority_to_string_list(0u).is_empty());
    EXPECT_TRUE(areg::is_valid_priority(areg::LogPriority::PrioDebug));
    EXPECT_FALSE(areg::is_valid_priority(areg::LogPriority::PrioInvalid));
    EXPECT_TRUE(areg::is_log_priority(areg::LogPriority::PrioInfo));
    EXPECT_FALSE(areg::is_log_priority(areg::LogPriority::PrioIgnoreLayout));
    EXPECT_TRUE(areg::is_enabling_log(areg::LogPriority::PrioFatal));
    EXPECT_FALSE(areg::is_enabling_log(areg::LogPriority::PrioScope));
    EXPECT_TRUE(areg::is_log_scope(areg::LogPriority::PrioScope));
    EXPECT_FALSE(areg::is_log_scope(areg::LogPriority::PrioDebug));
    EXPECT_TRUE(areg::is_disabling_log(areg::LogPriority::PrioNotset));
    EXPECT_TRUE(areg::is_disabling_log(areg::LogPriority::PrioInvalid));
    EXPECT_FALSE(areg::is_disabling_log(areg::LogPriority::PrioFatal));
}

// Scope identifiers and serialized entries retain their names, IDs, and priorities.
TEST(LoggingCoreTest, scope_ids_and_scope_entry_round_trip_preserve_identity)
{
    areg::LogScope scope{ "loggingcore_identity.method", areg::LogPriority::PrioInfo };
    EXPECT_EQ(scope.id(), areg::make_id("loggingcore_identity.method"));
    EXPECT_EQ(scope.id(), areg::make_scope_id_ex("loggingcore_identity.method"));
    EXPECT_EQ(areg::make_scope_id_ex("loggingcore_identity.*"), areg::CHECKSUM_IGNORE);
    EXPECT_NE(scope.id(), areg::make_id("loggingcore_identity.other"));
    areg::SharedBuffer buffer;
    buffer << scope;
    buffer.move_to_begin();
    areg::ScopeEntry entry;
    buffer >> entry;
    EXPECT_EQ(entry.scopeId, scope.id());
    EXPECT_EQ(entry.scopePrio, scope.priority());
    EXPECT_EQ(entry.scopeName, scope.name().data());
    EXPECT_EQ(buffer.position(), buffer.size_used());
    areg::SharedBuffer encoded;
    encoded << entry;
    encoded.move_to_begin();
    areg::ScopeEntry decoded;
    encoded >> decoded;
    EXPECT_EQ(decoded.scopeId, entry.scopeId);
    EXPECT_EQ(decoded.scopePrio, entry.scopePrio);
    EXPECT_EQ(decoded.scopeName, entry.scopeName);
}

// Scope batches survive serialization in order, including an empty batch.
TEST(LoggingCoreTest, scope_batch_round_trip_preserves_entry_order_and_empty_lists)
{
    areg::ScopeNames scopes;
    scopes.add(areg::ScopeEntry("loggingcore_batch.first", 123u, static_cast<uint32_t>(areg::LogPriority::PrioInfo)));
    scopes.add(areg::ScopeEntry("loggingcore_batch.*", 0u, static_cast<uint32_t>(areg::LogPriority::PrioWarning)));
    const areg::MessageEnvelope message{ areg::message_update_scopes(1001u, 1002u, scopes) };
    ASSERT_TRUE(message.is_valid());
    message.move_to_begin();
    uint32_t count{ 0u };
    message >> count;
    ASSERT_EQ(count, scopes.size());
    for (uint32_t index{ 0u }; index < count; ++index)
    {
        areg::ScopeEntry entry;
        message >> entry;
        EXPECT_EQ(entry.scopeId, scopes[index].scopeId);
        EXPECT_EQ(entry.scopePrio, scopes[index].scopePrio);
        EXPECT_EQ(entry.scopeName, scopes[index].scopeName);
    }
    EXPECT_EQ(message.position(), message.size_used());
    const auto empty{ areg::message_update_scopes(1001u, 1002u, areg::ScopeNames()) };
    ASSERT_TRUE(empty.is_valid());
    empty.move_to_begin();
    empty >> count;
    EXPECT_EQ(count, 0u);
    EXPECT_EQ(empty.position(), empty.size_used());
}

// A scope update with an unknown endpoint is rejected.
TEST(LoggingCoreTest, scope_update_rejects_unknown_endpoints)
{
    const areg::ScopeNames scopes;
    EXPECT_FALSE(areg::message_update_scopes(areg::COOKIE_UNKNOWN, 1002u, scopes).is_valid());
    EXPECT_FALSE(areg::message_update_scopes(1001u, areg::COOKIE_UNKNOWN, scopes).is_valid());
    EXPECT_FALSE(areg::message_update_scope(areg::COOKIE_UNKNOWN, 1002u, "scope", 1u, 1u).is_valid());
    EXPECT_FALSE(areg::message_update_scope(1001u, areg::COOKIE_UNKNOWN, "scope", 1u, 1u).is_valid());
}

// Every log protocol message that names an endpoint is refused when that endpoint is unknown.
TEST(LoggingCoreTest, protocol_messages_reject_an_unknown_source_or_target)
{
    constexpr ITEM_ID SRC{ 1001u };
    constexpr ITEM_ID DST{ 1002u };
    constexpr ITEM_ID NONE{ areg::COOKIE_UNKNOWN };
    for (const auto& ends : { std::pair<ITEM_ID, ITEM_ID>{ NONE, DST }, std::pair<ITEM_ID, ITEM_ID>{ SRC, NONE } })
    {
        EXPECT_FALSE(areg::message_query_instances(ends.first, ends.second).is_valid());
        EXPECT_FALSE(areg::message_query_scopes(ends.first, ends.second).is_valid());
        EXPECT_FALSE(areg::message_save_configuration(ends.first, ends.second).is_valid());
        EXPECT_FALSE(areg::message_restore_configuration(ends.first, ends.second).is_valid());
        EXPECT_FALSE(areg::message_update_source_state(ends.first, ends.second, areg::LogSourceState::Paused).is_valid());
        EXPECT_FALSE(areg::message_source_state_updated(ends.first, ends.second, areg::LogSourceState::Paused, 1003u).is_valid());
    }

    EXPECT_TRUE(areg::message_query_scopes(SRC, DST).is_valid());
    EXPECT_TRUE(areg::message_source_state_updated(SRC, DST, areg::LogSourceState::Active, 1003u).is_valid());
}

// The NOTSET bit beside a real level is not listed: the list names only the enabled levels.
TEST(LoggingCoreTest, notset_bit_beside_a_level_is_not_listed)
{
    const uint32_t prios{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) | static_cast<uint32_t>(areg::LogPriority::PrioNotset) };
    const areg::StringArray names{ areg::priority_to_string_list(prios) };
    ASSERT_EQ(names.size(), 1u);
    EXPECT_EQ(names[0u], areg::String(areg::PRIO_DEBUG_STR));
}

// The duration of a scope exit entry is the time since the scope was entered; other entries have none.
TEST(LoggingCoreTest, scope_exit_entry_measures_the_time_since_enter)
{
    const TIME64 entered{ static_cast<TIME64>(areg::DateTime::now()) };
    const areg::LogEntry exitEntry(areg::LogMessageType::ScopeExit, 1u, 1u, entered, areg::LogPriority::PrioScope, "scope", 5u);
    EXPECT_GE(exitEntry.logTimestamp, entered);
    EXPECT_EQ(exitEntry.logDuration, static_cast<uint32_t>(exitEntry.logTimestamp - entered));
    const areg::LogEntry plain(areg::LogMessageType::MessageText, 1u, 1u, 0u, areg::LogPriority::PrioInfo, "row", 3u);
    EXPECT_EQ(plain.logDuration, 0u);
}

// Reading a truncated scope entry stays within the available bytes.
TEST(LoggingCoreTest, truncated_scope_entry_never_reads_beyond_available_data)
{
    areg::SharedBuffer buffer;
    buffer << 123u << static_cast<uint32_t>(areg::LogPriority::PrioInfo) << 8u;
    const uint8_t name[]{ 'n', 'a', 'm', 'e' };
    ASSERT_EQ(buffer.write(name, sizeof(name)), sizeof(name));
    buffer.move_to_begin();
    areg::ScopeEntry entry;
    entry.scopeName = "stale";
    buffer >> entry;
    EXPECT_EQ(entry.scopeId, 123u);
    EXPECT_EQ(entry.scopePrio, static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_TRUE(entry.scopeName.is_empty());
    EXPECT_LE(buffer.position(), buffer.size_used());
    EXPECT_EQ(buffer.position(), buffer.size_used());
}

// Exact and wildcard priority changes affect only the scopes they match.
TEST(LoggingCoreTest, controller_exact_and_group_changes_affect_only_matching_scopes)
{
    areg::LogScope first{ "loggingcore_group.first", areg::LogPriority::PrioInfo };
    areg::LogScope second{ "loggingcore_group.second", areg::LogPriority::PrioInfo };
    areg::LogScope other{ "loggingcore_other.first", areg::LogPriority::PrioInfo };
    areg::ScopeController controller;
    controller.register_scope(first);
    controller.register_scope(second);
    controller.register_scope(other);
    EXPECT_TRUE(controller.is_scope_registered(first.id()));
    EXPECT_TRUE(controller.is_scope_registered(first.name().data()));
    EXPECT_EQ(controller.scope(first.id()), &first);
    EXPECT_EQ(controller.scope(first.name().data()), &first);
    EXPECT_EQ(controller.scope("loggingcore_missing.first"), nullptr);
    controller.set_scope_priority(areg::String(first.name().data()), areg::String("ERROR"));
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioError));
    EXPECT_EQ(second.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(controller.set_group_priority("loggingcore_group.*", "WARN"), 2);
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(second.priority(), first.priority());
    EXPECT_EQ(other.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(controller.add_group_priority("loggingcore_group.*", "SCOPE"), 2);
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning) | static_cast<uint32_t>(areg::LogPriority::PrioScope));
    EXPECT_EQ(controller.remove_group_priority("loggingcore_group.*", "SCOPE"), 2);
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(controller.set_group_priority("", 0u), 0);
    EXPECT_EQ(controller.add_group_priority("", areg::LogPriority::PrioScope), 0);
    EXPECT_EQ(controller.remove_group_priority("", areg::LogPriority::PrioScope), 0);
    EXPECT_EQ(controller.set_group_priority("loggingcore_group*", 0u), 0);
    EXPECT_EQ(controller.set_group_priority("loggingcore_absent.*", 0u), 0);
    EXPECT_EQ(controller.set_group_priority("loggingcore_*", static_cast<uint32_t>(areg::LogPriority::PrioFatal)), 3);
    EXPECT_EQ(other.priority(), static_cast<uint32_t>(areg::LogPriority::PrioFatal));
    controller.add_scope_priority(first.id(), areg::LogPriority::PrioScope);
    EXPECT_TRUE((first.priority() & static_cast<uint32_t>(areg::LogPriority::PrioScope)) != 0u);
    controller.remove_scope_priority(first.id(), areg::LogPriority::PrioScope);
    EXPECT_EQ(first.priority(), other.priority());
    controller.set_scope_priority(areg::make_id("missing"), 0u);
    controller.add_scope_priority(areg::make_id("missing"), areg::LogPriority::PrioScope);
    controller.remove_scope_priority(areg::make_id("missing"), areg::LogPriority::PrioScope);
    controller.unregister_scope(first);
    EXPECT_FALSE(controller.is_scope_registered(first.id()));
    EXPECT_EQ(controller.scope_list().size(), 2u);
}

// Configured priorities resolve exact names and wildcard prefixes in the intended order.
TEST(LoggingCoreTest, configured_exact_method_and_prefix_priorities_have_defined_precedence)
{
    areg::LogScope exact{ "loggingcore_config.exact" };
    areg::LogScope method{ "loggingcore_config.other" };
    areg::LogScope prefix{ "loggingcore_sibling.method" };
    areg::LogScope unmatched{ "unmatchedlogging.method" };
    areg::ScopeController controller;
    for (areg::LogScope* scope : { &exact, &method, &prefix, &unmatched })
    {
        controller.register_scope(*scope);
    }
    controller.configure_scopes("*", static_cast<uint32_t>(areg::LogPriority::PrioFatal), areg::ScopeLayer::Shared);
    controller.configure_scopes("loggingcore_*", static_cast<uint32_t>(areg::LogPriority::PrioWarning), areg::ScopeLayer::Module);
    controller.configure_scopes("loggingcore_config.*", static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    controller.configure_scopes("loggingcore_config.exact", static_cast<uint32_t>(areg::LogPriority::PrioDebug));
    controller.set_scope_activity(true);
    EXPECT_EQ(exact.priority(), static_cast<uint32_t>(areg::LogPriority::PrioDebug));
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(prefix.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(unmatched.priority(), static_cast<uint32_t>(areg::LogPriority::PrioFatal));
    controller.set_scope_activity("loggingcore_config.exact", 0u, static_cast<uint32_t>(areg::LogPriority::PrioError));
    EXPECT_EQ(exact.priority(), static_cast<uint32_t>(areg::LogPriority::PrioError));
    controller.set_scope_activity("loggingcore_config.*", 0u, static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    controller.set_scope_activity(false);
    EXPECT_EQ(exact.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    controller.reset();
    controller.activate_defaults();
    controller.set_scope_activity(true);
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    controller.force_activate_scopes();
    controller.set_scope_activity(true);
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug));
    controller.force_activate_scopes(false);
    controller.set_scope_activity(true);
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

// Scope-tree children are sorted without splitting a leaf's dotted name.
TEST(LoggingCoreTest, tree_children_are_sorted_and_dot_leaf_names_remain_intact)
{
    areg::ScopeRoot root;
    areg::String last{ "zeta.last" };
    areg::String second{ "alpha.second_part" };
    areg::String first{ "alpha.first" };
    EXPECT_EQ(root.add_child_recursive(last, static_cast<uint32_t>(areg::LogPriority::PrioFatal)), 2u);
    EXPECT_EQ(root.add_child_recursive(second, static_cast<uint32_t>(areg::LogPriority::PrioWarning)), 2u);
    EXPECT_EQ(root.add_child_recursive(first, static_cast<uint32_t>(areg::LogPriority::PrioInfo)), 2u);
    ASSERT_EQ(root.nodes().size(), 2u);
    EXPECT_EQ(root.nodes()[0u].node_name(), "alpha");
    EXPECT_EQ(root.nodes()[1u].node_name(), "zeta");
    const auto& alpha{ root.nodes()[0u] };
    ASSERT_EQ(alpha.leafs().size(), 2u);
    EXPECT_EQ(alpha.leafs()[0u].node_name(), "first");
    EXPECT_EQ(alpha.leafs()[1u].node_name(), "second_part");
    areg::ConfigManager config;
    EXPECT_EQ(root.update_config_node(config, ""), 3u);
    const auto scopes{ config.module_log_scopes() };
    ASSERT_EQ(scopes.size(), 3u);
    EXPECT_EQ(scopes[0].key().position(), "alpha.first");
    EXPECT_EQ(scopes[1].key().position(), "alpha.second_part");
    EXPECT_EQ(scopes[2].key().position(), "zeta.last");
}

// Copying, moving, and grouping a scope tree preserve its state, including empty paths.
TEST(LoggingCoreTest, tree_copy_move_grouping_and_empty_paths_preserve_state)
{
    areg::ScopeNode node;
    node.set_node_name("branch");
    areg::String first{ ".first" };
    areg::String second{ ".second" };
    ASSERT_TRUE(node.add_child_node(first, static_cast<uint32_t>(areg::LogPriority::PrioInfo)).second);
    ASSERT_TRUE(node.add_child_node(second, static_cast<uint32_t>(areg::LogPriority::PrioInfo)).second);
    areg::ScopeNode copied{ node };
    areg::ScopeNode moved{ std::move(copied) };
    areg::ScopeNode assigned;
    assigned = moved;
    areg::ScopeNode moveAssigned;
    moveAssigned = std::move(assigned);
    EXPECT_EQ(moveAssigned.child_node_count(), 2u);
    EXPECT_EQ(moveAssigned.make_scope_path("parent_"), "parent_branch_");
    EXPECT_EQ(moveAssigned.group_recursive(), 2u);
    EXPECT_TRUE(moveAssigned.is_empty());
    EXPECT_EQ(moveAssigned.make_config_string("parent_"), "parent_branch_*");
    EXPECT_EQ(node.child_node_count(), 2u);
    areg::ScopeLeaf leaf{ node.leafs()[0u] };
    areg::ScopeLeaf movedLeaf{ std::move(leaf) };
    EXPECT_TRUE(movedLeaf.is_leaf());
    EXPECT_EQ(movedLeaf.make_scope_path("branch."), "branch.first");
    areg::String empty;
    EXPECT_FALSE(node.make_child_node(empty, 0u).is_valid());
    EXPECT_FALSE(movedLeaf.add_child_node(empty, 0u).first.is_valid());
    EXPECT_EQ(movedLeaf.group_recursive(), 0u);
    EXPECT_EQ(movedLeaf.remove_priority_nodes(0u), 0u);
    EXPECT_TRUE(movedLeaf.is_empty());
}

// Removing the nodes of a priority drops the leaves of that priority and keeps the others.
TEST(LoggingCoreTest, tree_removes_the_leaves_of_a_priority)
{
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    const uint32_t debug{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    areg::ScopeNode node;
    node.set_node_name("branch");
    areg::String first{ ".first" };
    areg::String second{ ".second" };
    areg::String third{ ".third" };
    ASSERT_TRUE(node.add_child_node(first, info).second);
    ASSERT_TRUE(node.add_child_node(second, debug).second);
    ASSERT_TRUE(node.add_child_node(third, info).second);
    ASSERT_EQ(node.leafs().size(), 3u);

    EXPECT_EQ(node.remove_priority_nodes(info), 2u);
    ASSERT_EQ(node.leafs().size(), 1u);
    EXPECT_EQ(node.leafs()[0u].node_name(), "second");
    EXPECT_FALSE(node.is_empty());

    EXPECT_EQ(node.remove_priority_nodes(debug), 1u);
    EXPECT_TRUE(node.is_empty());
}

// A path without a separator is one leaf of the node.
TEST(LoggingCoreTest, path_without_a_separator_is_a_leaf_of_the_node)
{
    areg::ScopeNode node;
    node.set_node_name("branch");
    areg::String path{ "method" };
    ASSERT_TRUE(node.add_child_node(path, static_cast<uint32_t>(areg::LogPriority::PrioInfo)).second);
    EXPECT_TRUE(path.is_empty());
    ASSERT_EQ(node.leafs().size(), 1u);
    EXPECT_EQ(node.leafs()[0u].node_name(), "method");
    EXPECT_TRUE(node.nodes().is_empty());
}

// Removing a priority goes down into a child node of another priority and drops the leaves it finds.
TEST(LoggingCoreTest, tree_removes_the_leaves_of_a_priority_inside_a_child_node)
{
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    const uint32_t debug{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    areg::ScopeRoot root;
    areg::String first{ "alpha.first" };
    areg::String second{ "alpha.second" };
    ASSERT_EQ(root.add_child_recursive(first, info), 2u);
    ASSERT_EQ(root.add_child_recursive(second, debug), 2u);
    ASSERT_EQ(root.nodes().size(), 1u);
    ASSERT_EQ(root.nodes()[0u].leafs().size(), 2u);

    root.remove_priority_nodes(debug);
    ASSERT_EQ(root.nodes().size(), 1u);
    ASSERT_EQ(root.nodes()[0u].leafs().size(), 1u);
    EXPECT_EQ(root.nodes()[0u].leafs()[0u].node_name(), "first");
}

// Removing a priority drops a child node of that priority when nothing is left in it.
TEST(LoggingCoreTest, tree_removes_a_child_node_that_has_only_the_priority)
{
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    areg::ScopeRoot root;
    areg::String path{ "alpha.first" };
    ASSERT_EQ(root.add_child_recursive(path, info), 2u);
    ASSERT_EQ(root.nodes().size(), 1u);

    EXPECT_EQ(root.remove_priority_nodes(info), 2u);
    EXPECT_TRUE(root.nodes().is_empty());
    EXPECT_TRUE(root.is_empty());
}

// A leaf made from a node keeps its name and priority.
TEST(LoggingCoreTest, leaf_is_made_from_a_node_with_its_name_and_priority)
{
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    areg::ScopeNode node;
    node.set_node_name("branch");
    areg::String name{ ".first" };
    ASSERT_TRUE(node.add_child_node(name, info).second);
    ASSERT_EQ(node.leafs().size(), 1u);

    const areg::ScopeNodeBase& source{ node.leafs()[0u] };
    const areg::ScopeLeaf leaf{ source };
    EXPECT_TRUE(leaf.is_leaf());
    EXPECT_EQ(leaf.node_name(), source.node_name());
    EXPECT_EQ(leaf.priority(), info);
}

// Copied and moved logging events retain their action and data.
TEST(LoggingCoreTest, logging_event_data_copy_and_move_preserve_actions)
{
    using Data = areg::LoggingEventData;
    Data empty;
    EXPECT_EQ(empty.logging_action(), Data::LogAction::Undefined);
    Data original{ Data::LogAction::UpdateScopes };
    Data copied{ original };
    Data moved{ std::move(copied) };
    empty = moved;
    EXPECT_EQ(empty.logging_action(), Data::LogAction::UpdateScopes);
    empty.set_action(Data::LogAction::QueryScopes);
    original = std::move(empty);
    EXPECT_EQ(original.logging_action(), Data::LogAction::QueryScopes);
    EXPECT_STREQ(Data::as_string(original.logging_action()), "LoggingEventData::LogAction::QueryScopes");
}

// Each logging action has its own name, and a value outside the enumeration is reported as undefined.
TEST(LoggingCoreTest, logging_action_names_are_distinct_and_outside_value_is_reported)
{
    using Action = areg::LoggingEventData::LogAction;
    const std::vector<std::pair<Action, std::string>> names{
          { Action::Undefined,    "LoggingEventData::LogAction::Undefined" }
        , { Action::StartLogs,    "LoggingEventData::LogAction::StartLogs" }
        , { Action::StopLogs,     "LoggingEventData::LogAction::StopLogs" }
        , { Action::EnableLogs,   "LoggingEventData::LogAction::EnableLogs" }
        , { Action::DisableLogs,  "LoggingEventData::LogAction::DisableLogs" }
        , { Action::SaveScopes,   "LoggingEventData::LogAction::SaveScopes" }
        , { Action::UpdateScopes, "LoggingEventData::LogAction::UpdateScopes" }
        , { Action::QueryScopes,  "LoggingEventData::LogAction::QueryScopes" }
        , { Action::OpenRemote,   "LoggingEventData::LogAction::OpenRemote" } };
    for (const auto& entry : names)
    {
        EXPECT_EQ(std::string(areg::LoggingEventData::as_string(entry.first)), entry.second);
    }

    volatile uint32_t outside{ 0x7Fu };
    EXPECT_STREQ(areg::LoggingEventData::as_string(static_cast<Action>(outside)),
                 "ERR: Undefined LoggingEventData::LogAction value!");
}

// A serialized log entry retains its metadata and message text.
TEST(LoggingCoreTest, log_entry_stream_round_trip_preserves_metadata_and_text)
{
    areg::LogEntry entry;
    entry.logMsgType = areg::LogMessageType::ScopeExit;
    entry.logScopeId = 987u;
    entry.logSessionId = 654u;
    entry.logDuration = 321u;
    entry.logTimestamp = 123456u;
    entry.logMessagePrio = areg::LogPriority::PrioScope;
    entry.logMessageLen = static_cast<uint32_t>(areg::copy_string(entry.logMessage, areg::LOG_MSG_SIZE, "loggingcore_roundtrip.method"));
    areg::SharedBuffer buffer;
    buffer << entry;
    EXPECT_EQ(buffer.size_used(), areg::log_entry_size(entry));
    buffer.move_to_begin();
    areg::LogEntry decoded;
    buffer >> decoded;
    EXPECT_EQ(decoded.logMsgType, entry.logMsgType);
    EXPECT_EQ(decoded.logScopeId, entry.logScopeId);
    EXPECT_EQ(decoded.logSessionId, entry.logSessionId);
    EXPECT_EQ(decoded.logDuration, entry.logDuration);
    EXPECT_EQ(decoded.logTimestamp, entry.logTimestamp);
    EXPECT_EQ(decoded.logMessagePrio, entry.logMessagePrio);
    EXPECT_EQ(decoded.logMessageLen, entry.logMessageLen);
    EXPECT_STREQ(decoded.logMessage, entry.logMessage);
    areg::LogEntry copied{ decoded };
    areg::LogEntry assigned;
    assigned = copied;
    EXPECT_STREQ(assigned.logMessage, entry.logMessage);
    EXPECT_EQ(assigned.logSessionId, entry.logSessionId);
}

// Priority names must accept different letter cases.
TEST(LoggingCoreTest, priority_names_are_case_insensitive)
{
    EXPECT_EQ(areg::string_to_priority(areg::String("debug")), areg::LogPriority::PrioDebug);
    EXPECT_EQ(areg::string_to_priority(areg::String("iNfO")), areg::LogPriority::PrioInfo);
    EXPECT_EQ(areg::string_to_priority(areg::String("Warn")), areg::LogPriority::PrioWarning);
}

// Formatting the disabled priority must not turn logging back on.
TEST(LoggingCoreTest, notset_priority_list_keeps_logging_disabled)
{
    const auto list{ areg::priority_to_string_list(static_cast<uint32_t>(areg::LogPriority::PrioNotset)) };
    ASSERT_EQ(list.size(), 1u);
    EXPECT_EQ(list[0u], "NOTSET");
}

// A single-scope update must use the same wire format as a scope batch.
TEST(LoggingCoreTest, single_scope_update_uses_the_batch_wire_format)
{
    const areg::ScopeEntry entry{ "loggingcore_wire.method", 123u, static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    areg::ScopeNames scopes;
    scopes.add(entry);
    const auto batch{ areg::message_update_scopes(1001u, 1002u, scopes) };
    const auto single{ areg::message_update_scope(1001u, 1002u, entry.scopeName, entry.scopeId, entry.scopePrio) };
    ASSERT_TRUE(batch.is_valid());
    ASSERT_TRUE(single.is_valid());
    ASSERT_EQ(single.size_used(), batch.size_used());
    EXPECT_EQ(std::memcmp(single.buffer(), batch.buffer(), batch.size_used()), 0);
}

// Converting combined priorities to text and back must retain every flag.
TEST(LoggingCoreTest, combined_priority_names_round_trip_without_losing_flags)
{
    const uint32_t priority{ static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    EXPECT_EQ(areg::make_priorities(areg::String("SCOPE|DEBUG")), priority);
    EXPECT_EQ(areg::make_priorities(areg::make_prio_string(priority)), priority);
}

// Adding an existing scope-tree leaf again must update its priority.
TEST(LoggingCoreTest, readding_a_leaf_updates_its_priority)
{
    areg::ScopeNode node;
    node.set_node_name("branch");
    areg::String first{ ".leaf" };
    ASSERT_TRUE(node.add_child_node(first, static_cast<uint32_t>(areg::LogPriority::PrioInfo)).second);
    areg::String again{ ".leaf" };
    const auto update{ node.add_child_node(again, static_cast<uint32_t>(areg::LogPriority::PrioError)) };
    EXPECT_FALSE(update.second);
    EXPECT_EQ(node.child_node_count(), 1u);
    EXPECT_EQ(update.first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioError));
}

// Saving and reloading grouped dotted scopes must retain their priority.
TEST(LoggingCoreTest, grouped_dot_scopes_keep_their_priority_after_reload)
{
    areg::LogScope first{ "loggingcorealpha.first", areg::LogPriority::PrioInfo };
    areg::LogScope second{ "loggingcorealpha.second", areg::LogPriority::PrioInfo };
    areg::LogScope other{ "loggingcorebeta.other", areg::LogPriority::PrioFatal };
    areg::ScopeRoot root;
    root.add_child_recursive(first);
    root.add_child_recursive(second);
    root.add_child_recursive(other);
    root.group_recursive();
    areg::ConfigManager config;
    EXPECT_GT(root.update_config_node(config, ""), 0u);
    areg::ScopeController controller;
    controller.register_scope(first);
    controller.register_scope(second);
    controller.register_scope(other);
    for (const auto& property : config.module_log_scopes())
    {
        controller.configure_scopes(property);
    }
    controller.set_scope_activity(true);
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(second.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(other.priority(), static_cast<uint32_t>(areg::LogPriority::PrioFatal));
}

// Priority masks format in table order, parse with spaces and any case, and skip unknown names.
TEST(LoggingCoreTest, priority_masks_format_and_parse_lists)
{
    const uint32_t scopeDebug{ static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    EXPECT_EQ(areg::make_prio_string(scopeDebug), "SCOPE|DEBUG");
    EXPECT_EQ(areg::make_prio_string(static_cast<uint32_t>(areg::LogPriority::PrioWarning)), areg::PRIO_WARNING_STR);
    EXPECT_EQ(areg::make_prio_string(static_cast<uint32_t>(areg::LogPriority::PrioNotset)), areg::PRIO_NOTSET_STR);
    EXPECT_EQ(areg::make_prio_string(static_cast<uint32_t>(areg::LogPriority::PrioIgnore)), areg::PRIO_NOTSET_STR);
    EXPECT_EQ(areg::make_priorities(areg::String(" scope | Debug | bogus ")), scopeDebug);
    EXPECT_EQ(areg::make_priorities(areg::String("warn")), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_EQ(areg::make_priorities(areg::String("bogus")), static_cast<uint32_t>(areg::LogPriority::PrioInvalid));
    EXPECT_EQ(areg::make_priorities(areg::String()), static_cast<uint32_t>(areg::LogPriority::PrioInvalid));
    EXPECT_EQ(areg::string_to_priority(areg::String("WARNING")), areg::LogPriority::PrioIgnoreLayout);
    EXPECT_EQ(areg::string_to_priority(areg::String()), areg::LogPriority::PrioIgnoreLayout);
}

// A grouped node saves one wildcard for its nodes and one for its methods, and a partial group keeps every method.
TEST(LoggingCoreTest, grouped_nodes_and_methods_keep_their_priority_after_reload)
{
    const uint32_t scopeDebug{ static_cast<uint32_t>(areg::LogPriority::PrioScope) | static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    areg::LogScope nested{ "loggingcoregamma_inner.call", areg::LogPriority::PrioNotset };
    areg::LogScope same{ "loggingcoregamma.same", areg::LogPriority::PrioNotset };
    areg::LogScope other{ "loggingcoregamma.other", areg::LogPriority::PrioDebug };
    areg::LogScope both{ "loggingcoredelta_inner.call", areg::LogPriority::PrioInfo };
    areg::LogScope method{ "loggingcoredelta.method", areg::LogPriority::PrioInfo };
    nested.set_priority(scopeDebug);
    same.set_priority(scopeDebug);
    areg::ScopeRoot root;
    for (const areg::LogScope* scope : { &nested, &same, &other, &both, &method })
    {
        root.add_child_recursive(*scope);
    }

    root.group_recursive();
    areg::ConfigManager config;
    EXPECT_GT(root.update_config_node(config, ""), 0u);
    areg::ScopeController controller;
    for (areg::LogScope* scope : { &nested, &same, &other, &both, &method })
    {
        controller.register_scope(*scope);
    }

    for (const auto& property : config.module_log_scopes())
    {
        controller.configure_scopes(property);
    }

    controller.set_scope_activity(true);
    EXPECT_EQ(nested.priority(), scopeDebug);
    EXPECT_EQ(same.priority(), scopeDebug);
    EXPECT_EQ(other.priority(), static_cast<uint32_t>(areg::LogPriority::PrioDebug));
    EXPECT_EQ(both.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(method.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
}

// A method keeps its underscores: after a dot, and in a name without a dot, which is one method.
TEST(LoggingCoreTest, snake_case_methods_are_saved_whole)
{
    areg::LogScope dotted{ "loggingcoreepsilon_path.this_is_a_method", areg::LogPriority::PrioInfo };
    areg::LogScope sibling{ "loggingcoreepsilon_path.other_method", areg::LogPriority::PrioError };
    areg::LogScope dotless{ "loggingcoreepsilon_dotless_method", areg::LogPriority::PrioWarning };
    areg::ScopeRoot root;
    for (const areg::LogScope* scope : { &dotted, &sibling, &dotless })
    {
        root.add_child_recursive(*scope);
    }

    root.group_recursive();
    areg::ConfigManager config;
    EXPECT_EQ(root.update_config_node(config, ""), 3u);
    std::vector<std::string> keys;
    for (const auto& property : config.module_log_scopes())
    {
        keys.emplace_back(property.key().position().as_string());
    }

    std::sort(keys.begin(), keys.end());
    const std::vector<std::string> expected{ "loggingcoreepsilon_dotless_method", "loggingcoreepsilon_path.other_method", "loggingcoreepsilon_path.this_is_a_method" };
    EXPECT_EQ(keys, expected);
}

// Resuming the controller restores saved scopes and applies configuration to new ones.
TEST(LoggingCoreTest, controller_stop_resume_restores_saved_and_newly_configured_scopes)
{
    areg::LogScope first{ "loggingcore_resume.first", areg::LogPriority::PrioError };
    areg::LogScope second{ "loggingcore_resume.second", areg::LogPriority::PrioInfo };
    areg::LogScope late{ "loggingcore_resume.late", areg::LogPriority::PrioNotset };
    areg::ScopeController controller;
    controller.register_scope(first);
    controller.register_scope(second);
    const areg::Property property{ areg::String("log::*::scope::loggingcore_resume.* = WARN") };
    ASSERT_TRUE(property.key().is_valid());
    controller.configure_scopes(property);
    EXPECT_FALSE(controller.is_stopped());
    EXPECT_FALSE(controller.resume_scopes());
    EXPECT_TRUE(controller.stop_scopes());
    EXPECT_TRUE(controller.is_stopped());
    EXPECT_FALSE(controller.stop_scopes());
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    EXPECT_EQ(second.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    controller.register_scope(late);
    EXPECT_TRUE(controller.resume_scopes());
    EXPECT_FALSE(controller.is_stopped());
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioError));
    EXPECT_EQ(second.priority(), static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    EXPECT_EQ(late.priority(), static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    EXPECT_FALSE(controller.resume_scopes());
    EXPECT_TRUE(controller.stop_scopes());
    EXPECT_TRUE(controller.discard_saved_scopes());
    EXPECT_FALSE(controller.discard_saved_scopes());
    EXPECT_FALSE(controller.resume_scopes());
    EXPECT_EQ(first.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
}

// A node whose leaves share its priority, with a deeper child node of the same priority, is grouped whole.
TEST(LoggingCoreTest, grouping_removes_deeper_nodes_of_the_grouped_priority)
{
    constexpr uint32_t DEBUG_PRIO{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    areg::ScopeRoot root;
    for (const char* name : { "grp.first", "grp.second", "grp.inner.leaf", "grp.inner.deep.last" })
    {
        areg::String scope{ name };
        EXPECT_GT(root.add_child_recursive(scope, DEBUG_PRIO), 0u);
    }

    ASSERT_EQ(root.nodes().size(), 1u);
    EXPECT_GT(root.group_recursive(), 0u);
    ASSERT_EQ(root.nodes().size(), 1u);
    const auto& group{ root.nodes()[0u] };
    EXPECT_EQ(group.node_name(), "grp");
    EXPECT_TRUE(group.leafs().is_empty());
    EXPECT_TRUE(group.nodes().is_empty());
    EXPECT_EQ(group.priority(), DEBUG_PRIO);
}

// The priority of a scope changes by the names of priorities, given as C string and as string.
TEST(LoggingCoreTest, scope_priority_is_added_and_removed_by_name)
{
    areg::LogScope scope{ "loggingcore_priority_names.method", areg::LogPriority::PrioInfo };
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    const uint32_t warning{ static_cast<uint32_t>(areg::LogPriority::PrioWarning) };

    scope.set_priority(0u);
    EXPECT_EQ(scope.priority(), 0u);

    scope.add_priority("INFO");
    EXPECT_EQ(scope.priority(), info);
    scope.add_priority(areg::String("WARN"));
    EXPECT_EQ(scope.priority(), info | warning);

    scope.remove_priority("INFO");
    EXPECT_EQ(scope.priority(), warning);
    scope.remove_priority(areg::String("WARN"));
    EXPECT_EQ(scope.priority(), 0u);
}

/**
 * \brief   Reproduces the defect: a priority list or an unknown name given by name switched every
 *          log level of the scope on, because the text was taken as one name.
 **/
TEST(LoggingCoreTest, scope_priority_by_name_takes_lists_and_skips_unknown_names)
{
    const uint32_t debug{ static_cast<uint32_t>(areg::LogPriority::PrioDebug) };
    const uint32_t warning{ static_cast<uint32_t>(areg::LogPriority::PrioWarning) };
    const uint32_t error{ static_cast<uint32_t>(areg::LogPriority::PrioError) };
    const uint32_t scopes{ static_cast<uint32_t>(areg::LogPriority::PrioScope) };
    areg::LogScope scope{ "loggingcore_priority_lists.method", areg::LogPriority::PrioError };

    scope.set_priority("DEBUG | SCOPE");
    EXPECT_EQ(scope.priority(), debug | scopes);
    scope.set_priority(areg::String("WARN"));
    EXPECT_EQ(scope.priority(), warning);
    scope.set_priority("VERBOSE");
    EXPECT_EQ(scope.priority(), warning);
    scope.set_priority(areg::String());
    EXPECT_EQ(scope.priority(), warning);
    scope.set_priority("NOTSET");
    EXPECT_EQ(scope.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));

    scope.set_priority(warning);
    scope.add_priority("VERBOSE");
    EXPECT_EQ(scope.priority(), warning);
    scope.add_priority(areg::String("SCOPE | ERROR"));
    EXPECT_EQ(scope.priority(), warning | scopes | error);
    scope.remove_priority("SCOPE | ERROR");
    EXPECT_EQ(scope.priority(), warning);
    scope.remove_priority(areg::String("VERBOSE"));
    EXPECT_EQ(scope.priority(), warning);

    areg::ScopeMessage message{ scope };
    EXPECT_TRUE(message.is_warn_enabled());
    EXPECT_FALSE(message.is_info_enabled());
    EXPECT_FALSE(message.is_dbg_enabled());
}

// The controller takes priority lists by name and leaves scopes unchanged for unknown names.
TEST(LoggingCoreTest, controller_priority_by_name_takes_lists_and_skips_unknown_names)
{
    const uint32_t info{ static_cast<uint32_t>(areg::LogPriority::PrioInfo) };
    const uint32_t scopes{ static_cast<uint32_t>(areg::LogPriority::PrioScope) };
    areg::LogScope first{ "loggingcore_names.first", areg::LogPriority::PrioInfo };
    areg::LogScope second{ "loggingcore_names.second", areg::LogPriority::PrioInfo };
    areg::ScopeController controller;
    controller.register_scope(first);
    controller.register_scope(second);

    controller.set_scope_priority(first.id(), areg::String("VERBOSE"));
    EXPECT_EQ(first.priority(), info);
    controller.add_scope_priority(first.id(), areg::String("VERBOSE"));
    EXPECT_EQ(first.priority(), info);
    EXPECT_EQ(controller.set_group_priority("loggingcore_names.*", "VERBOSE"), 0);
    EXPECT_EQ(second.priority(), info);

    EXPECT_EQ(controller.set_group_priority("loggingcore_names.*", "INFO | SCOPE"), 2);
    EXPECT_EQ(first.priority(), info | scopes);
    EXPECT_EQ(second.priority(), info | scopes);
    controller.remove_scope_priority(areg::String(first.name().data()), areg::String("SCOPE"));
    EXPECT_EQ(first.priority(), info);
    controller.add_scope_priority(areg::String(first.name().data()), areg::String("SCOPE"));
    EXPECT_EQ(first.priority(), info | scopes);
    controller.remove_scope_priority(first.id(), areg::String("SCOPE | INFO"));
    EXPECT_EQ(first.priority(), 0u);
    controller.unregister_scope(first);
    controller.unregister_scope(second);
}

// The queries of a scope message follow the priority the scope had when the message was made.
TEST(LoggingCoreTest, scope_message_queries_follow_the_scope_priority)
{
    areg::LogScope scope{ "loggingcore_message_queries.method", areg::LogPriority::PrioInvalid };
    {
        areg::ScopeMessage none{ scope };
        EXPECT_FALSE(none.is_log_enabled());
        EXPECT_FALSE(none.is_scope_enabled());
        EXPECT_FALSE(none.is_fatal_enabled());
        EXPECT_FALSE(none.is_prio_enabled(areg::LogPriority::PrioScope));
    }

    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    {
        areg::ScopeMessage notset{ scope };
        EXPECT_FALSE(notset.is_log_enabled());
        EXPECT_FALSE(notset.is_scope_enabled());
    }

    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioScope));
    {
        areg::ScopeMessage onlyScope{ scope };
        EXPECT_TRUE(onlyScope.is_log_enabled());
        EXPECT_TRUE(onlyScope.is_scope_enabled());
        EXPECT_TRUE(onlyScope.is_prio_enabled(areg::LogPriority::PrioScope));
        EXPECT_FALSE(onlyScope.is_prio_enabled(areg::LogPriority::PrioFatal));
    }

    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioError));
    {
        areg::ScopeMessage errors{ scope };
        EXPECT_TRUE(errors.is_log_enabled());
        EXPECT_FALSE(errors.is_scope_enabled());
        EXPECT_FALSE(errors.is_prio_enabled(areg::LogPriority::PrioScope));
        EXPECT_TRUE(errors.is_prio_enabled(areg::LogPriority::PrioFatal));
        EXPECT_TRUE(errors.is_prio_enabled(areg::LogPriority::PrioError));
        EXPECT_FALSE(errors.is_prio_enabled(areg::LogPriority::PrioWarning));
    }
}

// The session of a scope that has not logged yet is zero.
TEST(LoggingCoreTest, scope_session_starts_at_zero)
{
    areg::LogScope scope{ "loggingcore_session.method", areg::LogPriority::PrioInfo };
    EXPECT_EQ(scope.session(), 0u);
}

#endif // AREG_LOGGING
