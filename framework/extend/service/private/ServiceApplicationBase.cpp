﻿/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/service/private/ServiceApplicationBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Service application base class.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "extend/service/ServiceApplicationBase.hpp"

#include "extend/service/ServiceCommunicatonBase.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceStart);
DEF_TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_servicePause);
DEF_TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceContinue);
DEF_TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceStop);
DEF_TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_setState);

ServiceApplicationBase::ServiceApplicationBase(ServiceCommunicatonBase& commBase)
    : SystemServiceBase         ( commBase )
{
}

int ServiceApplicationBase::serviceMain(int argc, char** argv)
{
    int result{ RESULT_SUCCEEDED };
    if (parseOptions(argc, argv, NESystemService::ServiceOptionSetup, MACRO_ARRAYLEN(NESystemService::ServiceOptionSetup)) == false)
    {
        resetDefaultOptions();
    }

    switch (getCurrentOption())
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

    case NESystemService::eServiceOption::CMD_Service:  // fall through
    case NESystemService::eServiceOption::CMD_Console:
        result = SystemServiceBase::serviceMain(static_cast<int>(argc), argv);
        mCommunication.waitToComplete();
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

bool ServiceApplicationBase::serviceInitialize(int argc, char** argv)
{
    // Start only tracing and timer manager.
    Application::initApplication( true
                                , true
                                , false
                                , true
                                , false
                                , NEApplication::DEFAULT_CONFIG_FILE.data()
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
        _osCcreateService();
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
    TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceStart);
    TRACE_DBG("Starting [ %s ] system service", getServiceNameA());
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
    TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_servicePause);
    TRACE_DBG("Pausing [ %s ] system service", getServiceNameA());

    setState(NESystemService::eSystemServiceState::ServicePausing);
    mCommunication.disconnectServiceHost();
    mCommunication.waitToComplete();
    setState(NESystemService::eSystemServiceState::ServicePaused);
}

bool ServiceApplicationBase::serviceContinue(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceContinue);
    TRACE_DBG("Resume and continuing paused [ %s ] system service", getServiceNameA());

    bool result = false;
    setState(NESystemService::eSystemServiceState::ServiceContinuing);
    if (mCommunication.isServiceHostSetup() && mCommunication.connectServiceHost())
    {
        result = true;
        setState(NESystemService::eSystemServiceState::ServiceRunning);
    }
    else
    {
        TRACE_ERR("Failed to resume [ %s ] system service", getServiceNameA());
        Application::signalAppQuit();
    }

    return result;
}

void ServiceApplicationBase::serviceStop(void)
{
    TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_serviceStop);
    TRACE_WARN("Stopping [ %s ] system service", getServiceNameA());
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
    TRACE_SCOPE(areg_extend_service_ServiceApplicationBase_setState);
    TRACE_DBG( "Changing [ %s ] system service state. Old state [ %s ], new state [ %s ]"
                , getServiceNameA()
                , NESystemService::getString( mSystemServiceState )
                , NESystemService::getString( newState ) );
    return _osSetState(newState);
}

void ServiceApplicationBase::runService(void)
{
    Application::waitAppQuit(NECommon::WAIT_INFINITE);
}

void ServiceApplicationBase::prepareSaveConfiguration(ConfigManager& config)
{
}

void ServiceApplicationBase::postSaveConfiguration(ConfigManager& config)
{
}

void ServiceApplicationBase::prepareReadConfiguration(ConfigManager& config)
{
}

void ServiceApplicationBase::postReadConfiguration(ConfigManager& config)
{
}

void ServiceApplicationBase::onSetupConfiguration(const NEPersistence::ListProperties& listReadonly, const NEPersistence::ListProperties& listWritable, ConfigManager& config)
{
}

bool ServiceApplicationBase::inputConsoleData(char* buffer, unsigned int bufSize)
{
    return _osWaitUserInput(buffer, bufSize);
}
