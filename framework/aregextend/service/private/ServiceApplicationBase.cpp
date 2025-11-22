/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceApplicationBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service application base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/ServiceApplicationBase.hpp"

#include "aregextend/service/ServiceCommunicatonBase.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStart);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_servicePause);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceContinue);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStop);
DEF_LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_setState);

ServiceApplicationBase::ServiceApplicationBase(ServiceCommunicatonBase& commBase)
    : SystemServiceBase ( commBase )
    , mServiceSetup     (false)
{
}

int ServiceApplicationBase::serviceMain(NESystemService::eServiceOption optStartup, const char* argument)
{
    int result{ RESULT_SUCCEEDED };
    Application::setWorkingDirectory(nullptr);
    mSystemServiceOption = optStartup;
    switch (optStartup)
    {
    case NESystemService::eServiceOption::CMD_Install:
        if (serviceInstall() == false)
        {
            result = ServiceApplicationBase::RESULT_FAILED_INSTALL;
        }
        break;

    case NESystemService::eServiceOption::CMD_Uninstall:
        serviceUninstall();
        break;

    case NESystemService::eServiceOption::CMD_Service:
        result = startServiceDispatcher( );
        if (result == RESULT_IGNORED)
        {
            result = SystemServiceBase::serviceMain(optStartup, argument);
            mCommunication.waitToComplete();
        }
        break;

    case NESystemService::eServiceOption::CMD_Load:     // fall through
    case NESystemService::eServiceOption::CMD_Console:  // fall through
    case NESystemService::eServiceOption::CMD_Custom:
        result = SystemServiceBase::serviceMain(optStartup, argument);
        mCommunication.waitToComplete();
        break;

    case NESystemService::eServiceOption::CMD_Help:
    case NESystemService::eServiceOption::CMD_Verbose:
    break;

    case NESystemService::eServiceOption::CMD_Undefined:
    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

bool ServiceApplicationBase::serviceInitialize(NESystemService::eServiceOption /*option*/, const char* /*value*/, const char * fileConfig)
{
    // Start only tracing and timer manager.
    if (NEString::isEmpty(fileConfig))
    {
        if (mFileConfig.isEmpty())
        {
            fileConfig = NEApplication::DEFAULT_CONFIG_FILE.data();
        }
        else
        {
            fileConfig = mFileConfig.getString();
        }
    }

    Application::initApplication( true
                                , true
                                , false
                                , true
                                , false
                                , fileConfig
                                , static_cast<IEConfigurationListener*>(this));
    return _osInitializeService();
}

void ServiceApplicationBase::serviceRelease(void)
{
    Application::releaseApplication();
}

bool ServiceApplicationBase::serviceInstall(void)
{
    if (_osOpenService() == false)
    {
        _osCreateService();
    }

    return _osIsValid();
}

void ServiceApplicationBase::serviceUninstall(void)
{
    if (_osOpenService())
    {
        _osDeleteService();
    }

    _osFreeResources();
}

bool ServiceApplicationBase::registerService(void)
{
    return _osRegisterService();
}

bool ServiceApplicationBase::serviceOpen(void)
{
    return _osOpenService();
}

bool ServiceApplicationBase::serviceStart(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStart);
    LOG_DBG("Starting [ %s ] system service", getServiceNameA());

    bool result{ false };
    NERemoteService::eRemoteServices serviceType = getServiceType();
    NERemoteService::eConnectionTypes connectType = getConnectionType();
    if (serviceType != NERemoteService::eRemoteServices::ServiceUnknown)
    {
        if (mCommunication.setupServiceConnectionData(serviceType, static_cast<uint32_t>(connectType)) &&
            mCommunication.connectServiceHost())
        {
            result = setState(NESystemService::eSystemServiceState::ServiceRunning);
        }
        else
        {
            Application::signalAppQuit();
        }
    }

    return result;
}

void ServiceApplicationBase::servicePause(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_servicePause);
    LOG_DBG("Pausing [ %s ] system service", getServiceNameA());

    setState(NESystemService::eSystemServiceState::ServicePausing);
    mCommunication.disconnectServiceHost();
    mCommunication.waitToComplete();
    setState(NESystemService::eSystemServiceState::ServicePaused);
}

bool ServiceApplicationBase::serviceContinue(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceContinue);
    LOG_DBG("Resume and continuing paused [ %s ] system service", getServiceNameA());

    bool result = false;
    setState(NESystemService::eSystemServiceState::ServiceContinuing);
    if (mCommunication.isServiceHostSetup() && mCommunication.connectServiceHost())
    {
        result = true;
        setState(NESystemService::eSystemServiceState::ServiceRunning);
    }
    else
    {
        LOG_ERR("Failed to resume [ %s ] system service", getServiceNameA());
        Application::signalAppQuit();
    }

    return result;
}

void ServiceApplicationBase::serviceStop(void)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStop);
    LOG_WARN("Stopping [ %s ] system service", getServiceNameA());
    setState(NESystemService::eSystemServiceState::ServiceStopping);
    mCommunication.disconnectServiceHost();
    mCommunication.waitToComplete();
    Application::signalAppQuit();
}

void ServiceApplicationBase::serviceShutdown(void)
{
    serviceStop();
}

bool ServiceApplicationBase::setState(NESystemService::eSystemServiceState newState)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_setState);
    LOG_DBG( "Changing [ %s ] system service state. Old state [ %s ], new state [ %s ]"
                , getServiceNameA()
                , NESystemService::getString( mSystemServiceState )
                , NESystemService::getString( newState ) );
    return _osSetState(newState);
}

void ServiceApplicationBase::runService(void)
{
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
}

int ServiceApplicationBase::startServiceDispatcher( void )
{
    int result{ RESULT_IGNORED };
    if (mServiceSetup == false)
    {
        mServiceSetup = true;
        result = _osStartServiceDispatcher();
        mServiceSetup = (result == RESULT_SUCCEEDED) || (result == RESULT_IGNORED);
    }

    return result;
}

void ServiceApplicationBase::prepareSaveConfiguration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::postSaveConfiguration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::prepareReadConfiguration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::postReadConfiguration(ConfigManager& /* config */)
{
}

void ServiceApplicationBase::onSetupConfiguration( const NEPersistence::ListProperties& /* listReadonly */
                                                 , const NEPersistence::ListProperties& /* listWritable */
                                                 , ConfigManager& /* config */)
{
}

bool ServiceApplicationBase::inputConsoleData(char* buffer, unsigned int bufSize)
{
    return _osWaitUserInput(buffer, bufSize);
}
