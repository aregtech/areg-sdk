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

int ServiceApplicationBase::serviceMain(NESystemService::ServiceOption optStartup, const char* argument)
{
    int result{ RESULT_SUCCEEDED };
    Application::setWorkingDirectory(nullptr);
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
                                , static_cast<ConfigListener*>(this));
    return _osInitializeService();
}

void ServiceApplicationBase::serviceRelease()
{
    Application::releaseApplication();
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
    NERemoteService::RemoteServiceKind serviceType = getServiceType();
    NERemoteService::ConnectionType connectType = getConnectionType();
    if (serviceType != NERemoteService::RemoteServiceKind::Unknown)
    {
        if (mCommunication.setupServiceConnectionData(serviceType, static_cast<uint32_t>(connectType)) &&
            mCommunication.connectServiceHost())
        {
            result = setState(NESystemService::ServicePhase::Running);
        }
        else
        {
            Application::signalAppQuit();
        }
    }

    return result;
}

void ServiceApplicationBase::servicePause()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_servicePause);
    LOG_DBG("Pausing [ %s ] system service", getServiceNameA());

    setState(NESystemService::ServicePhase::Pausing);
    mCommunication.disconnectServiceHost();
    mCommunication.waitToComplete();
    setState(NESystemService::ServicePhase::Paused);
}

bool ServiceApplicationBase::serviceContinue()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceContinue);
    LOG_DBG("Resume and continuing paused [ %s ] system service", getServiceNameA());

    bool result = false;
    setState(NESystemService::ServicePhase::Continuing);
    if (mCommunication.isServiceHostSetup() && mCommunication.connectServiceHost())
    {
        result = true;
        setState(NESystemService::ServicePhase::Running);
    }
    else
    {
        LOG_ERR("Failed to resume [ %s ] system service", getServiceNameA());
        Application::signalAppQuit();
    }

    return result;
}

void ServiceApplicationBase::serviceStop()
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_serviceStop);
    LOG_WARN("Stopping [ %s ] system service", getServiceNameA());
    setState(NESystemService::ServicePhase::Stopping);
    mCommunication.disconnectServiceHost();
    mCommunication.waitToComplete();
    Application::signalAppQuit();
}

void ServiceApplicationBase::serviceShutdown()
{
    serviceStop();
}

bool ServiceApplicationBase::setState(NESystemService::ServicePhase newState)
{
    LOG_SCOPE(areg_aregextend_service_ServiceApplicationBase_setState);
    LOG_DBG( "Changing [ %s ] system service state. Old state [ %s ], new state [ %s ]"
                , getServiceNameA()
                , NESystemService::getString( mSystemServiceState )
                , NESystemService::getString( newState ) );
    return _osSetState(newState);
}

void ServiceApplicationBase::runService()
{
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
}

int ServiceApplicationBase::startServiceDispatcher()
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
    return Console::readConsoleData(buffer, bufSize);
}
