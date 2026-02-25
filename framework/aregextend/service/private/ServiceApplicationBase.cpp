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
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStart);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_servicePause);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceContinue);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStop);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_setState);

ServiceApplicationBase::ServiceApplicationBase(ServiceCommunicationBase& commBase)
    : SystemServiceBase ( commBase )
    , mServiceSetup     (false)
{
}

int32_t ServiceApplicationBase::serviceMain(NESystemService::ServiceOption optStartup, const char* argument)
{
    int32_t result{ RESULT_SUCCEEDED };
    Application::set_working_directory(nullptr);
    mSystemServiceOption = optStartup;
    switch (optStartup)
    {
    case NESystemService::ServiceOption::CMD_Install:
        if (serviceInstall() == false)
        {
            result = ServiceApplicationBase::RESULT_FAILED_INSTALL;
        }
        break;

    case NESystemService::ServiceOption::CMD_Uninstall:
        serviceUninstall();
        break;

    case NESystemService::ServiceOption::CMD_Service:
        result = startServiceDispatcher( );
        if (result == RESULT_IGNORED)
        {
            result = SystemServiceBase::serviceMain(optStartup, argument);
            mCommunication.waitToComplete();
        }
        break;

    case NESystemService::ServiceOption::CMD_Load:     // fall through
    case NESystemService::ServiceOption::CMD_Console:  // fall through
    case NESystemService::ServiceOption::CMD_Custom:
        result = SystemServiceBase::serviceMain(optStartup, argument);
        mCommunication.waitToComplete();
        break;

    case NESystemService::ServiceOption::CMD_Help:
    case NESystemService::ServiceOption::CMD_Verbose:
    break;

    case NESystemService::ServiceOption::CMD_Undefined:
    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

bool ServiceApplicationBase::serviceInitialize(NESystemService::ServiceOption /*option*/, const char* /*value*/, const char * fileConfig)
{
    // Start only tracing and timer manager.
    if (NEString::is_empty(fileConfig))
    {
        if (mFileConfig.is_empty())
        {
            fileConfig = NEApplication::DEFAULT_CONFIG_FILE.data();
        }
        else
        {
            fileConfig = mFileConfig.as_string();
        }
    }

    Application::init_application( true
                                , true
                                , false
                                , true
                                , false
                                , fileConfig
                                , static_cast<ConfigListener*>(this));
    return _osInitializeService();
}

void ServiceApplicationBase::serviceRelease()
{
    Application::release_application();
}

bool ServiceApplicationBase::serviceInstall()
{
    if (_osOpenService() == false)
    {
        _osCreateService();
    }

    return _osIsValid();
}

void ServiceApplicationBase::serviceUninstall()
{
    if (_osOpenService())
    {
        _osDeleteService();
    }

    _osFreeResources();
}

bool ServiceApplicationBase::registerService()
{
    return _osRegisterService();
}

bool ServiceApplicationBase::serviceOpen()
{
    return _osOpenService();
}

bool ServiceApplicationBase::serviceStart()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStart);
    LOG_DBG("Starting [ %s ] system service", getServiceNameA());

    bool result{ false };
    NERemoteService::RemoteServiceKind serviceType = service_type();
    NERemoteService::ConnectionType connectType = connection_type();
    if (serviceType != NERemoteService::RemoteServiceKind::Unknown)
    {
        if (mCommunication.setup_connection_data(serviceType, static_cast<uint32_t>(connectType)) &&
            mCommunication.connect_service_host())
        {
            result = set_state(NESystemService::ServicePhase::Running);
        }
        else
        {
            Application::signal_app_quit();
        }
    }

    return result;
}

void ServiceApplicationBase::servicePause()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_servicePause);
    LOG_DBG("Pausing [ %s ] system service", getServiceNameA());

    set_state(NESystemService::ServicePhase::Pausing);
    mCommunication.disconnect_service_host();
    mCommunication.waitToComplete();
    set_state(NESystemService::ServicePhase::Paused);
}

bool ServiceApplicationBase::serviceContinue()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceContinue);
    LOG_DBG("Resume and continuing paused [ %s ] system service", getServiceNameA());

    bool result = false;
    set_state(NESystemService::ServicePhase::Continuing);
    if (mCommunication.is_host_setup() && mCommunication.connect_service_host())
    {
        result = true;
        set_state(NESystemService::ServicePhase::Running);
    }
    else
    {
        LOG_ERR("Failed to resume [ %s ] system service", getServiceNameA());
        Application::signal_app_quit();
    }

    return result;
}

void ServiceApplicationBase::serviceStop()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStop);
    LOG_WARN("Stopping [ %s ] system service", getServiceNameA());
    set_state(NESystemService::ServicePhase::Stopping);
    mCommunication.disconnect_service_host();
    mCommunication.waitToComplete();
    Application::signal_app_quit();
}

void ServiceApplicationBase::serviceShutdown()
{
    serviceStop();
}

bool ServiceApplicationBase::set_state(NESystemService::ServicePhase newState)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_setState);
    LOG_DBG( "Changing [ %s ] system service state. Old state [ %s ], new state [ %s ]"
                , getServiceNameA()
                , NESystemService::as_string( mSystemServiceState )
                , NESystemService::as_string( newState ) );
    return _osSetState(newState);
}

void ServiceApplicationBase::runService()
{
    Application::wait_app_quit(NECommon::WAIT_INFINITE);
}

int32_t ServiceApplicationBase::startServiceDispatcher()
{
    int32_t result{ RESULT_IGNORED };
    if (mServiceSetup == false)
    {
        mServiceSetup = true;
        result = _osStartServiceDispatcher();
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

void ServiceApplicationBase::on_setup_configuration( const NEPersistence::ListProperties& /* listReadonly */
                                                 , const NEPersistence::ListProperties& /* listWritable */
                                                 , ConfigManager& /* config */)
{
}

bool ServiceApplicationBase::inputConsoleData(char* buffer, uint32_t bufSize)
{
    return Console::readConsoleData(buffer, bufSize);
}
