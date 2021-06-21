/************************************************************************
 * \file        mcrouter/private/MulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/MulticastRouter.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceMain);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceStart);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceStop);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_servicePause);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceContinue);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceInstall);
DEF_TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceUninstall);

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
MulticastRouter & MulticastRouter::getInstance(void)
{
    static MulticastRouter _messageRouter;
    return _messageRouter;
}

MulticastRouter::MulticastRouter( void )
    : mRouterState  ( NEMulticastRouterSettings::RouterStopped )
    , mServiceCmd   ( MulticastRouter::CMD_Undefined )
    , mServiceServer( )
    , mSvcHandle    ( NULL )
    , mSeMHandle    ( NULL )
{
    ; // do nothing
}

MulticastRouter::~MulticastRouter( void )
{
    _freeResources( );
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    Application::initApplication(true, false, false, true, NEApplication::DEFAULT_TRACING_CONFIG_FILE, NULL);

    do 
    {
        TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceMain);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        if ( _registerService() || mServiceCmd == MulticastRouter::CMD_Console )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        if ( mServiceCmd == MulticastRouter::CMD_Console )
        {
            printf("Type \'quit\' or \'q\' to quit message router ...: ");
            const char quit = static_cast<int>('q' );
            char cmd[8]     = {0};

            do 
            {
#ifdef _WINDOWS
                scanf_s("%4s", cmd, 8);
#else   // !_WINDOWS
                scanf("%4s", cmd);
#endif  // !_WINDOWS
            } while (NEString::makeAsciiLower<char>(*cmd) != quit);

            Application::signalAppQuit();
        }

        Application::waitAppQuit(IESynchObject::WAIT_INFINITE);

        serviceStop();

        TRACE_WARN("Service Stopped and not running anymore");

        Application::releaseApplication();

    } while (false);
}

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    bool result = false;
    if (  mServiceServer.configureRemoteServicing( NEApplication::DEFAULT_ROUTER_CONFIG_FILE ) && mServiceServer.startRemoteServicing() )
    {
        result = setState(NEMulticastRouterSettings::RouterRunning);
    }
    else
    {
        Application::signalAppQuit();
    }

    return result;
}

void MulticastRouter::serviceStop(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NEMulticastRouterSettings::ServiceNameA);
    setState(NEMulticastRouterSettings::RouterStopping);
    mServiceServer.stopRemoteServicing();
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NEMulticastRouterSettings::RouterPausing );
    mServiceServer.stopRemoteServicing();
    setState( NEMulticastRouterSettings::RouterPaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceContinue);
    TRACE_DBG("Continueing Router service");

    bool result = false;
    setState( NEMulticastRouterSettings::RouterContinuing );
    if ( mServiceServer.isRemoteServicingConfigured() && mServiceServer.startRemoteServicing() )
    {
        result = true;
        setState( NEMulticastRouterSettings::RouterRunning );
    }
    else
    {
        TRACE_ERR("Failed to restart remote servicing");
        Application::signalAppQuit();
    }

    return result;
}

bool MulticastRouter::serviceInstall(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceInstall);
    
    if ( _openService() == false )
    {
        _createService();
    }

    return _isValid();
}

void MulticastRouter::serviceUninstall(void)
{
    TRACE_SCOPE(areg_mcrouter_MulticastRouter_serviceUninstall);

    if ( _openService() )
    {
        _deleteService();
    }

    _freeResources();
}

