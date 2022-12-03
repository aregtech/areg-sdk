/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/MulticastRouterWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/app/MulticastRouter.hpp"

#ifdef WINDOWS

#pragma comment(lib, "areg.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "kernel32.lib")

#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

#include <shellapi.h>

#include "mcrouter/resources/resource.hpp"

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////
VOID WINAPI _win32ServiceMain(DWORD argc, LPTSTR * argv);
VOID WINAPI _win32ServiceCtrlHandler(DWORD);

namespace
{
#ifdef UNICODE
    wchar_t * _serviceDescribe = NEApplication::ROUTER_SERVICE_NAME_WIDE;
#else
    char * _serviceDescribe = NEApplication::ROUTER_SERVICE_NAME_ASCII;
#endif // UNICODE

    SERVICE_STATUS          _serviceStatus  { 0 };
    SERVICE_STATUS_HANDLE   _statusHandle   { nullptr };
    SERVICE_TABLE_ENTRY     _serviceTable[] { {_serviceDescribe, &::_win32ServiceMain}, {nullptr, nullptr} };

    inline char ** _convertArguments( TCHAR ** argv, int argc )
    {
        char ** argvTemp = argc != 0 ? DEBUG_NEW char * [argc] : nullptr;
        if ( argvTemp != nullptr )
        {
            for ( int i = 0; i < static_cast<int>(argc); ++i )
            {
                TCHAR * entry = argv[i];
                uint32_t length = static_cast<uint32_t>(NEString::getStringLength<TCHAR>( entry ));
                uint32_t size = length + 1u;
                char * arg = DEBUG_NEW char[size];
                NEString::copyString<char, TCHAR>( arg, size, entry );
                argvTemp[i] = arg;
            }
        }
        return argvTemp;
    }

    inline void _deleteArguments( char ** argv, int argc )
    {
        if ( argv != nullptr )
        {
            for ( int i = 0; i < static_cast<int>(argc); ++i )
                delete[] argv[i];
            delete[] argv;
        }
    }

} // namespace

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int result      = 0;
    char ** argvTemp = _convertArguments(argv, argc);
    MulticastRouter & router = MulticastRouter::getInstance();
    if (router.parseOptions(argc, argvTemp) == false)
    {
        router.resetDefaultOptions();
    }

    _deleteArguments(argvTemp, argc);

    switch ( router.getCurrentCommand() )
    {
    case NEMulticastRouterSettings::eServiceCommand::CMD_Install:
        result = router.serviceInstall() ? 0 : -2;
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Uninstall:
        router.serviceUninstall();
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Console:
        ::_win32ServiceMain(argc, argv);
        router.serviceStop();
        break;

    case NEMulticastRouterSettings::eServiceCommand::CMD_Service:
        result = ::StartServiceCtrlDispatcher(_serviceTable) ? 0 : -1;
        break;

    default:
        ASSERT(false);  // unexpected
        break;
    }

    return result;
}

VOID WINAPI _win32ServiceMain( DWORD argc, LPTSTR * argv )
{
    try
    {

        NEMemory::zeroElement<SERVICE_STATUS>( _serviceStatus );
        _serviceStatus.dwServiceType            = SERVICE_WIN32_OWN_PROCESS;
        _serviceStatus.dwCurrentState           = SERVICE_STOPPED;
        _serviceStatus.dwControlsAccepted       = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
        _serviceStatus.dwWin32ExitCode          = NO_ERROR;
        _serviceStatus.dwServiceSpecificExitCode= 0;
        _serviceStatus.dwCheckPoint             = 0;
        _serviceStatus.dwWaitHint               = 0;

        MulticastRouter & router = MulticastRouter::getInstance( );
        router.setState( NEMulticastRouterSettings::eRouterState::RouterStarting );

        char ** argvTemp = _convertArguments( argv, static_cast<int>(argc) );
        router.serviceMain( static_cast<int>(argc), argvTemp );
        _deleteArguments( argvTemp, static_cast<int>(argc) );

        router.setState( NEMulticastRouterSettings::eRouterState::RouterStopped );
    }
    catch ( const std::exception& )
    {
        ASSERT(false);
    }

}

VOID WINAPI _win32ServiceCtrlHandler(DWORD CtrlCode)
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
bool MulticastRouter::_osIsValid( void ) const
{
    return (mSeMHandle != nullptr && mSvcHandle != nullptr);
}

void MulticastRouter::_osFreeResources( void )
{
    if ( mSvcHandle != nullptr )
    {
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSvcHandle) );
    }

    if ( mSeMHandle != nullptr )
    {
        ::CloseServiceHandle( reinterpret_cast<SC_HANDLE>(mSeMHandle) );
    }

    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool MulticastRouter::_osOpenService(void)
{
    if ( mSeMHandle == nullptr )
    {
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager( nullptr, nullptr, SC_MANAGER_ALL_ACCESS ));
    }
    
    if ( (mSeMHandle != nullptr) && (mSvcHandle == nullptr) )
    {
        mSvcHandle = reinterpret_cast<void *>(::OpenService( reinterpret_cast<SC_HANDLE>(mSeMHandle), ::_serviceDescribe, SERVICE_ALL_ACCESS ));
    }
    
    return ( mSvcHandle != nullptr );
}

bool MulticastRouter::_osCcreateService(void)
{
    if ( mSeMHandle == nullptr )
    {
        mSeMHandle = reinterpret_cast<void *>(::OpenSCManager( nullptr, nullptr, SC_MANAGER_ALL_ACCESS ));
    }
    
    if ( (mSeMHandle != nullptr) && (mSvcHandle == nullptr) )
    {
        TCHAR szPath[MAX_PATH];
        if( ::GetModuleFileName( nullptr, szPath, MAX_PATH ) )
        {

#define DEVELOPMENT_PENDING     0

#if defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
            DWORD startType = SERVICE_DEMAND_START;
#else   // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
            DWORD startType = SERVICE_AUTO_START;
#endif  // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)

            mSvcHandle = reinterpret_cast<void *>(::CreateService(reinterpret_cast<SC_HANDLE>(mSeMHandle), _serviceDescribe, _serviceDescribe, SERVICE_ALL_ACCESS
                                                                , SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, szPath
                                                                , nullptr, nullptr, nullptr, nullptr, nullptr));
            if ( mSvcHandle != nullptr )
            {
                SERVICE_DESCRIPTION description;
                description.lpDescription = _serviceDescribe;
                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DESCRIPTION, &description);

                SERVICE_FAILURE_ACTIONS_FLAG actionFlag = {1};
                actionFlag.fFailureActionsOnNonCrashFailures = TRUE;
                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &actionFlag);

                SERVICE_FAILURE_ACTIONS failures = {0};
                SC_ACTION actions[4];
                int count = 4;

                failures.dwResetPeriod  = INFINITE;
                failures.lpRebootMsg    = nullptr;
                failures.lpCommand      = nullptr;
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
    
    return ( mSvcHandle != nullptr );
}

void MulticastRouter::_osDeleteService( void )
{
    if ( mSvcHandle != nullptr )
    {
        ::DeleteService( reinterpret_cast<SC_HANDLE>(mSvcHandle) );
    }
}

bool MulticastRouter::_osRegisterService( void )
{
    if ( mServiceCmd == NEMulticastRouterSettings::eServiceCommand::CMD_Service )
    {
        _statusHandle = ::RegisterServiceCtrlHandler( _serviceDescribe, _win32ServiceCtrlHandler );
    }

    return (_statusHandle != nullptr);
}

bool MulticastRouter::_osSetState( NEMulticastRouterSettings::eRouterState newState )
{
    bool result{ true };

    _serviceStatus.dwControlsAccepted   = 0;
    _serviceStatus.dwWin32ExitCode      = 0;

    if ( newState != mRouterState )
    {
        switch ( newState )
        {
        case NEMulticastRouterSettings::eRouterState::RouterStopped:
            _serviceStatus.dwCurrentState       = SERVICE_STOPPED;
            _serviceStatus.dwControlsAccepted   = 0;
            _serviceStatus.dwCheckPoint         = 7;
            _serviceStatus.dwWin32ExitCode      = ERROR_SUCCESS;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterStarting:
            _serviceStatus.dwCurrentState       = SERVICE_START_PENDING;
            _serviceStatus.dwCheckPoint         = 1;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterStopping:
            _serviceStatus.dwCurrentState       = SERVICE_STOP_PENDING;
            _serviceStatus.dwCheckPoint         = 6;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterRunning:
            _serviceStatus.dwCurrentState       = SERVICE_RUNNING;
            _serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            _serviceStatus.dwCheckPoint         = 2;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterContinuing:
            _serviceStatus.dwCurrentState       = SERVICE_CONTINUE_PENDING;
            _serviceStatus.dwCheckPoint         = 5;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterPausing:
            _serviceStatus.dwCurrentState       = SERVICE_PAUSE_PENDING;
            _serviceStatus.dwCheckPoint         = 3;
            break;

        case NEMulticastRouterSettings::eRouterState::RouterPaused:
            _serviceStatus.dwCurrentState       = SERVICE_PAUSED;
            _serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            _serviceStatus.dwCheckPoint         = 4;
            break;

        default:
            ASSERT(false);
        }
        mRouterState = newState;
        if ( _statusHandle != nullptr )
        {
            result = ::SetServiceStatus( _statusHandle, &_serviceStatus ) != FALSE;
        }
    }

    return result;
}

#endif // WINDOWS
