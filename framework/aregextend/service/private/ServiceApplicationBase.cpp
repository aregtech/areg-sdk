/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceApplicationBase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service application base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/ServiceApplicationBase.hpp"

#include "aregextend/service/ServiceCommunicationBase.hpp"
#include "aregextend/console/Console.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/logging/areg_log.h"

namespace areg::ext {

DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase, service_start);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase, service_pause);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase, service_continue);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase, service_stop);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase, set_state);

ServiceApplicationBase::ServiceApplicationBase(ServiceCommunicationBase& commBase)
    : SystemServiceBase ( commBase )
    , mServiceSetup     (false)
{
}

int32_t ServiceApplicationBase::service_main(areg::ext::ServiceOption optStartup, const char* argument)
{
    int32_t result{ RESULT_SUCCEEDED };
    Application::set_working_directory(nullptr);
    mSystemServiceOption = optStartup;
    switch (optStartup)
    {
    case areg::ext::ServiceOption::CMD_Install:
        if (service_install() == false)
        {
            result = ServiceApplicationBase::RESULT_FAILED_INSTALL;
        }
        break;

    case areg::ext::ServiceOption::CMD_Uninstall:
        service_uninstall();
        break;

    case areg::ext::ServiceOption::CMD_Service:
        result = start_service_dispatcher( );
        if (result == RESULT_IGNORED)
        {
            result = SystemServiceBase::service_main(optStartup, argument);
            mCommunication.wait_to_complete();
        }
        break;

    case areg::ext::ServiceOption::CMD_Load:     // fall through
    case areg::ext::ServiceOption::CMD_Console:  // fall through
    case areg::ext::ServiceOption::CMD_Custom:
        result = SystemServiceBase::service_main(optStartup, argument);
        mCommunication.wait_to_complete();
        break;

    case areg::ext::ServiceOption::CMD_Help:
    case areg::ext::ServiceOption::CMD_Verbose:
    break;

    case areg::ext::ServiceOption::CMD_Undefined:
    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

bool ServiceApplicationBase::service_initialize(areg::ext::ServiceOption /*option*/, const char* /*value*/, const char * fileConfig)
{
    // Start only tracing and timer manager.
    if (areg::is_empty(fileConfig))
    {
        if (mFileConfig.is_empty())
        {
            fileConfig = areg::DEFAULT_CONFIG_FILE.data();
        }
        else
        {
            fileConfig = mFileConfig.as_string();
        }
    }

    Application::setup( true
                                , true
                                , false
                                , true
                                , false
                                , fileConfig
                                , static_cast<ConfigListener*>(this));
    return _os_initialize_service();
}

void ServiceApplicationBase::service_release()
{
    Application::release();
}

bool ServiceApplicationBase::service_install()
{
    if (_os_open_service() == false)
    {
        _os_create_service();
    }

    return _os_is_valid();
}

void ServiceApplicationBase::service_uninstall()
{
    if (_os_open_service())
    {
        _os_delete_service();
    }

    _os_free_resources();
}

bool ServiceApplicationBase::register_service()
{
    return _os_register_service();
}

bool ServiceApplicationBase::service_open()
{
    return _os_open_service();
}

bool ServiceApplicationBase::service_start()
{
    LOG_SCOPE( areg_aregextend_service_ServiceApplicationBase, service_start );
    LOG_DBG("Starting [ %s ] system service", service_name_a());

    bool result{ false };
    areg::RemoteServiceKind serviceType = service_type();
    areg::ConnectionType connectType = connection_type();
    if (serviceType != areg::RemoteServiceKind::Unknown)
    {
        if (mCommunication.setup_connection_data(serviceType, static_cast<uint32_t>(connectType)) &&
            mCommunication.connect_service_host())
        {
            result = set_state(areg::ext::ServicePhase::Running);
        }
        else
        {
            Application::signal_quit();
        }
    }

    return result;
}

void ServiceApplicationBase::service_pause()
{
    LOG_SCOPE( areg_aregextend_service_ServiceApplicationBase, service_pause );
    LOG_DBG("Pausing [ %s ] system service", service_name_a());

    set_state(areg::ext::ServicePhase::Pausing);
    mCommunication.disconnect_service_host();
    mCommunication.wait_to_complete();
    set_state(areg::ext::ServicePhase::Paused);
}

bool ServiceApplicationBase::service_continue()
{
    LOG_SCOPE( areg_aregextend_service_ServiceApplicationBase, service_continue );
    LOG_DBG("Resume and continuing paused [ %s ] system service", service_name_a());

    bool result = false;
    set_state(areg::ext::ServicePhase::Continuing);
    if (mCommunication.is_host_setup() && mCommunication.connect_service_host())
    {
        result = true;
        set_state(areg::ext::ServicePhase::Running);
    }
    else
    {
        LOG_ERR("Failed to resume [ %s ] system service", service_name_a());
        Application::signal_quit();
    }

    return result;
}

void ServiceApplicationBase::service_stop()
{
    LOG_SCOPE( areg_aregextend_service_ServiceApplicationBase, service_stop );
    LOG_WARN("Stopping [ %s ] system service", service_name_a());
    set_state(areg::ext::ServicePhase::Stopping);
    mCommunication.disconnect_service_host();
    mCommunication.wait_to_complete();
    Application::signal_quit();
}

void ServiceApplicationBase::service_shutdown()
{
    service_stop();
}

bool ServiceApplicationBase::set_state(areg::ext::ServicePhase newState)
{
    LOG_SCOPE( areg_aregextend_service_ServiceApplicationBase, set_state );
    LOG_DBG( "Changing [ %s ] system service state. Old state [ %s ], new state [ %s ]"
                , service_name_a()
                , areg::ext::as_string( mSystemServiceState )
                , areg::ext::as_string( newState ) );
    return _os_set_state(newState);
}

void ServiceApplicationBase::run_service()
{
    Application::wait_quit(areg::WAIT_INFINITE);
}

int32_t ServiceApplicationBase::start_service_dispatcher()
{
    int32_t result{ RESULT_IGNORED };
    if (mServiceSetup == false)
    {
        mServiceSetup = true;
        result = _os_start_service_dispatcher();
        mServiceSetup = (result == RESULT_SUCCEEDED) || (result == RESULT_IGNORED);
    }

    return result;
}

void ServiceApplicationBase::prepare_save_configuration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::post_save_configuration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::prepare_read_configuration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::post_read_configuration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::on_setup_configuration( const areg::ListProperties& /* listReadonly */
                                                 , const areg::ListProperties& /* listWritable */
                                                 , ConfigManager& /* config */)
{
}

bool ServiceApplicationBase::input_console_data(char* buffer, uint32_t bufSize)
{
    return Console::read_console_data(buffer, bufSize);
}

} // namespace areg::ext
