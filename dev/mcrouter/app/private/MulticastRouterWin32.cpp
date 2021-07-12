/************************************************************************
 * \file        mcrouter/app/private/MulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/app/MulticastRouter.hpp"

#ifdef WINDOWS

#pragma comment(lib, "areg.lib")
#pragma comment(lib, "advapi32.lib")

#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/WideString.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/trace/GETrace.h"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shellapi.h>
#include "mcrouter/resources/resource.hpp"

#ifdef UNICODE
    #define ServiceName     NEMulticastRouterSettings::ServiceNameW
#else   // UNICODE
    #define ServiceName     NEMulticastRouterSettings::ServiceNameA
#endif  // UNICODE

DEF_TRACE_SCOPE(mcrouter_app_MulticastRouter_setState);

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////
VOID WINAPI GServiceMain(DWORD argc, LPTSTR * argv);
VOID WINAPI GServiceCtrlHandler(DWORD);

#ifdef UNICODE
    static WideString _serviceDescribe(NEApplication::ROUTER_SERVICE_NAME_WIDE);
#else
    static String     _serviceDescribe(NEApplication::ROUTER_SERVICE_NAME_ASCII);
#endif // UNICODE

static SERVICE_STATUS           gServiceStatus  = {0};
static SERVICE_STATUS_HANDLE    gStatusHandle   = NULL;
static SERVICE_TABLE_ENTRY      gServiceTable[ ]= 
{
      {_serviceDescribe.getUnsafeBuffer(), &::GServiceMain}
    , {NULL, NULL}
};

inline static char ** _convertArguments( TCHAR ** argv, int argc)
{
    char ** argvTemp = argc != 0 ? DEBUG_NEW char * [argc] : NULL;
    if ( argvTemp != NULL )
    {
        for ( int i = 0; i < static_cast<int>(argc); ++ i )
        {
            TCHAR * entry   = argv[i];
            int length      = NEString::getStringLength<TCHAR>(entry);
            char * arg      = DEBUG_NEW char[ length + 1];
            NEString::copyString<char, TCHAR>(arg, length + 1, entry);
            argvTemp[i] = arg;
        }
    }
    return argvTemp;
}

inline static void _deleteArguments( char ** argv, int argc )
{
    if ( argv != NULL )
    {
        for ( int i = 0; i < static_cast<int>(argc); ++ i )
            delete [] argv[i];
        delete [] argv;
    }
}

inline static bool _isEqual( const char * strLeft, const char * strRight )
{
    return (NEString::compareStrings<char, char>(strLeft, strRight, NEString::CountAll, false) == 0);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int result      = 0;
    char ** argvTemp = _convertArguments(argv, argc);
    MulticastRouter & router = MulticastRouter::getInstance();
    router.setCurrentCommand( MulticastRouter::CMD_Console );

    if ( argc > 1 )
    {
        const char * cmd = argvTemp[1];

        if ( _isEqual( cmd, NEMulticastRouterSettings::ServiceCommandInstall) )
            router.setCurrentCommand( MulticastRouter::CMD_Install );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandUninstall) )
            router.setCurrentCommand( MulticastRouter::CMD_Uninstall );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandConsole) )
            router.setCurrentCommand( MulticastRouter::CMD_Console );
        else if ( _isEqual(cmd, NEMulticastRouterSettings::ServiceCommandService) )
            router.setCurrentCommand(MulticastRouter::CMD_Service);
    }

    _deleteArguments(argvTemp, argc);

    switch ( MulticastRouter::getInstance().getCurrentCommand() )
    {
    case MulticastRouter::CMD_Install:
        result = MulticastRouter::getInstance().serviceInstall() ? 0 : -2;
        break;

    case MulticastRouter::CMD_Uninstall:
        MulticastRouter::getInstance().serviceUninstall();
        break;

    case MulticastRouter::CMD_Console:
        ::GServiceMain(argc, argv);
        MulticastRouter::getInstance().serviceStop();
        break;

    case MulticastRouter::CMD_Service:
        result = ::StartServiceCtrlDispatcher(gServiceTable) ? 0 : -1;
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

	return result;
}

VOID WINAPI GServiceMain( DWORD argc, LPTSTR * argv )
{
    NEMemory::zeroData<SERVICE_STATUS>(gServiceStatus);
    gServiceStatus.dwServiceType            = SERVICE_WIN32_OWN_PROCESS;
    gServiceStatus.dwCurrentState           = SERVICE_STOPPED;
    gServiceStatus.dwControlsAccepted       = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
    gServiceStatus.dwWin32ExitCode          = NO_ERROR;
    gServiceStatus.dwServiceSpecificExitCode= 0;
    gServiceStatus.dwCheckPoint             = 0;    
    gServiceStatus.dwWaitHint               = 0;

    MulticastRouter & router = MulticastRouter::getInstance();
    router.setState(NEMulticastRouterSettings::RouterStarting);

    char ** argvTemp = _convertArguments(argv, static_cast<int>(argc));
    router.serviceMain( static_cast<int>(argc), argvTemp);
    _deleteArguments(argvTemp, static_cast<int>(argc));

    router.setState( NEMulticastRouterSettings::RouterStopped );
}

VOID WINAPI GServiceCtrlHandler (DWORD CtrlCode)
{
    switch ( CtrlCode )
    {
    case SERVICE_CONTROL_STOP:
        MulticastRouter::getInstance().serviceStop();
        break;

    case SERVICE_CONTROL_PAUSE:
        MulticastRouter::getInstance().servicePause();
        break;

    case SERVICE_CONTROL_CONTINUE:
        MulticastRouter::getInstance().serviceContinue();
        break;

    default:
        break;  // do nothing
    }
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MulticastRouter class implementation
//////////////////////////////////////////////////////////////////////////
bool MulticastRouter::_isValid( void ) const
{
    return (mSeMHandle != NULL && mSvcHandle != NULL);
}

void MulticastRouter::_freeResources( void )
{
    if ( mSvcHandle != NULL )
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSvcHandle) );
    if ( mSeMHandle != NULL )
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSeMHandle) );

    mSvcHandle = NULL;
    mSeMHandle = NULL;
}

bool MulticastRouter::_openService(void)
{
    if ( mSeMHandle == NULL )
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
    
    if ( mSeMHandle != NULL )
    {
        if ( mSvcHandle == NULL )
            mSvcHandle = reinterpret_cast<void *>(::OpenService(reinterpret_cast<SC_HANDLE>(mSeMHandle), ServiceName, SERVICE_ALL_ACCESS));
    }
    
    return ( mSvcHandle != NULL );
}

bool MulticastRouter::_createService(void)
{
    if ( mSeMHandle == NULL )
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS));
    
    if ( mSeMHandle != NULL )
    {
        if ( mSvcHandle == NULL )
        {
            TCHAR szPath[MAX_PATH];
            if( ::GetModuleFileName( NULL, szPath, MAX_PATH ) )
            {

#define DEVELOPMENT_PENDING     1

#if defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
                DWORD startType = SERVICE_DEMAND_START;
#else   // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
                DWORD startType = SERVICE_AUTO_START;
#endif  // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)

                mSvcHandle = reinterpret_cast<void *>(::CreateService(reinterpret_cast<SC_HANDLE>(mSeMHandle), ServiceName, ServiceName, SERVICE_ALL_ACCESS
                                                                    , SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath
                                                                    , NULL, NULL, NULL, NULL, NULL));
                if ( mSvcHandle != NULL )
                {
                    SERVICE_DESCRIPTION description;
                    description.lpDescription = _serviceDescribe.getUnsafeBuffer();
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DESCRIPTION, &description);

                    SERVICE_FAILURE_ACTIONS_FLAG actionFlag = {1};
                    actionFlag.fFailureActionsOnNonCrashFailures = TRUE;
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &actionFlag);

                    SERVICE_FAILURE_ACTIONS failures = {0};
                    SC_ACTION actions[4];
                    int count = 4;

                    failures.dwResetPeriod  = INFINITE;
                    failures.lpRebootMsg    = NULL;
                    failures.lpCommand      = NULL;
                    failures.cActions       = count;
                    failures.lpsaActions    = actions;

                    // first failure
                    actions[0].Delay        = 500;
                    actions[0].Type         = SC_ACTION_RESTART;

                    // first second
                    actions[1].Delay        = 500;
                    actions[1].Type         = SC_ACTION_RESTART;

                    // third failure
                    actions[2].Delay        = 500;
                    actions[2].Type         = SC_ACTION_RESTART;

                    // fourth failure
                    actions[3].Delay        = 0;
                    actions[3].Type         = SC_ACTION_NONE;

                    actions[count - 1].Delay= 0;
                    actions[count - 1].Type = SC_ACTION_NONE;

                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS, &failures);

#if !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
                    SERVICE_DELAYED_AUTO_START_INFO delay = {1};
                    delay.fDelayedAutostart = TRUE;
                    ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delay);
#endif // !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
                }
            }
        }
    }
    
    return ( mSvcHandle != NULL );
}

void MulticastRouter::_deleteService( void )
{
    if ( reinterpret_cast<SC_HANDLE>(mSvcHandle) != reinterpret_cast<SC_HANDLE>(NULL) )
        ::DeleteService( reinterpret_cast<SC_HANDLE>(mSvcHandle) );

}

bool MulticastRouter::_registerService( void )
{
    if ( mServiceCmd == MulticastRouter::CMD_Service )
        gStatusHandle = ::RegisterServiceCtrlHandler(ServiceName, GServiceCtrlHandler);
    return (gStatusHandle != NULL);
}

bool MulticastRouter::setState( NEMulticastRouterSettings::eRouterState newState )
{
    TRACE_SCOPE(mcrouter_app_MulticastRouter_setState);
    TRACE_DBG("Changing Service Router state. Old state [ %s ], new state [ %s ]", NEMulticastRouterSettings::GetString(mRouterState), NEMulticastRouterSettings::GetString(newState));

    bool result = true;
    gServiceStatus.dwControlsAccepted   = 0;
    gServiceStatus.dwWin32ExitCode      = 0;

    if ( newState != mRouterState )
    {
        switch ( newState )
        {
        case NEMulticastRouterSettings::RouterStopped:
            gServiceStatus.dwCurrentState       = SERVICE_STOPPED;
            gServiceStatus.dwControlsAccepted   = 0;
            gServiceStatus.dwCheckPoint         = 7;
            gServiceStatus.dwWin32ExitCode      = ERROR_SUCCESS;
            break;

        case NEMulticastRouterSettings::RouterStarting:
            gServiceStatus.dwCurrentState       = SERVICE_START_PENDING;
            gServiceStatus.dwCheckPoint         = 1;
            break;

        case NEMulticastRouterSettings::RouterStopping:
            gServiceStatus.dwCurrentState       = SERVICE_STOP_PENDING;
            gServiceStatus.dwCheckPoint         = 6;
            break;

        case NEMulticastRouterSettings::RouterRunning:
            gServiceStatus.dwCurrentState       = SERVICE_RUNNING;
            gServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            gServiceStatus.dwCheckPoint         = 2;
            break;

        case NEMulticastRouterSettings::RouterContinuing:
            gServiceStatus.dwCurrentState       = SERVICE_CONTINUE_PENDING;
            gServiceStatus.dwCheckPoint         = 5;
            break;

        case NEMulticastRouterSettings::RouterPausing:
            gServiceStatus.dwCurrentState       = SERVICE_PAUSE_PENDING;
            gServiceStatus.dwCheckPoint         = 3;
            break;

        case NEMulticastRouterSettings::RouterPaused:
            gServiceStatus.dwCurrentState       = SERVICE_PAUSED;
            gServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            gServiceStatus.dwCheckPoint         = 4;
            break;

        default:
            ASSERT(false);
        }
        mRouterState = newState;
        if ( gStatusHandle != NULL )
            result = ::SetServiceStatus(gStatusHandle, &gServiceStatus) != FALSE;
    }

    return result;
}

#endif // WINDOWS
