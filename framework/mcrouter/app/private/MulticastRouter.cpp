/************************************************************************
 * \file        mcrouter/app/private/MulticastRouter.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Router, Multicast Router Service process common part
 ************************************************************************/

#include "mcrouter/app/MulticastRouter.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
#endif  // _WINDOWS


DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceMain);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStop);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceInstall);
DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceUninstall);

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
MulticastRouter & MulticastRouter::getInstance(void)
{
    static MulticastRouter _messageRouter;
    return _messageRouter;
}

MulticastRouter::MulticastRouter( void )
    : mRouterState  ( NEMulticastRouterSettings::eRouterState::RouterStopped )
    , mServiceCmd   ( NEMulticastRouterSettings::eServiceCommand::CMD_Undefined )
    , mServiceServer( )
    , mSvcHandle    ( nullptr )
    , mSeMHandle    ( nullptr )
{
}

MulticastRouter::~MulticastRouter( void )
{
    _freeResources( );
}

void MulticastRouter::serviceMain( int argc, char ** argv )
{
    // Start only tracing and timer manager.
    Application::initApplication(true, false, false, true, false, NEApplication::DEFAULT_TRACING_CONFIG_FILE.data(), nullptr );

    do 
    {
        TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceMain);
        TRACE_DBG("Starting service. There are [ %d ] arguments in the list...", argc);

#ifdef  DEBUG
        for ( int i = 0; i < argc; ++ i )
            TRACE_DBG("... Command argument [ %d ]: [ %s ]", i, argv[i]);
#endif  // DEBUG

        if ( _registerService() || mServiceCmd == NEMulticastRouterSettings::eServiceCommand::CMD_Console )
        {
            TRACE_DBG("Starting service");
            serviceStart();
        }

        if ( mServiceCmd == NEMulticastRouterSettings::eServiceCommand::CMD_Console )
        {
            printf("Type \'quit\' or \'q\' to quit message router ...: ");
            const char quit = static_cast<int>('q' );
            char cmd[8]     = {0};
            int charRead	= 0;

            do 
            {
                if ( MACRO_SCANF( "%4s", cmd, 8 ) != 1 )
                {
                    printf( "\nERROR: Unexpected choice of command, quit application ...\n" );
                }

            } while ((NEString::makeAsciiLower<char>(*cmd) != quit) && (charRead > 0));

            Application::signalAppQuit();
        }

        Application::waitAppQuit(NECommon::WAIT_INFINITE);

        serviceStop();

        TRACE_WARN("Service Stopped and not running anymore");

        Application::releaseApplication();

    } while (false);
}

bool MulticastRouter::serviceStart(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStart);
    TRACE_DBG("Starting service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    bool result = false;
    if (  mServiceServer.configureRemoteServicing( NEApplication::DEFAULT_ROUTER_CONFIG_FILE.data() ) && mServiceServer.startRemoteServicing() )
    {
        result = setState(NEMulticastRouterSettings::eRouterState::RouterRunning);
    }
    else
    {
        Application::signalAppQuit();
    }

    return result;
}

void MulticastRouter::serviceStop(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceStop);
    TRACE_WARN("Stopping service [ %s ]", NEMulticastRouterSettings::SERVICE_NAME_ASCII);
    setState(NEMulticastRouterSettings::eRouterState::RouterStopping);
    mServiceServer.stopRemoteServicing();
    Application::signalAppQuit();
}

void MulticastRouter::servicePause(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_servicePause);
    TRACE_DBG("Pausing Router service");

    setState( NEMulticastRouterSettings::eRouterState::RouterPausing );
    mServiceServer.stopRemoteServicing();
    setState( NEMulticastRouterSettings::eRouterState::RouterPaused );
}

bool MulticastRouter::serviceContinue(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceContinue);
    TRACE_DBG("Continueing Router service");

    bool result = false;
    setState( NEMulticastRouterSettings::eRouterState::RouterContinuing );
    if ( mServiceServer.isRemoteServicingConfigured() && mServiceServer.startRemoteServicing() )
    {
        result = true;
        setState( NEMulticastRouterSettings::eRouterState::RouterRunning );
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
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceInstall);
    
    if ( _openService() == false )
    {
        _createService();
    }

    return _isValid();
}

void MulticastRouter::serviceUninstall(void)
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_serviceUninstall);

    if ( _openService() )
    {
        _deleteService();
    }

    _freeResources();
}

