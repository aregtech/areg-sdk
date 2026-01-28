/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/ServiceApplicationBaseWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service application base class.
 *              Win32 specific implementation.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/ServiceApplicationBase.hpp"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////
extern VOID WINAPI _win32ServiceMain(DWORD argc, LPTSTR * argv);
extern VOID WINAPI _win32ServiceCtrlHandler(DWORD);

#ifdef UNICODE
    #define     getServiceName          getServiceNameW
    #define     getServiceDisplayName   getServiceDisplayNameW
    #define     getServiceDescription   getServiceDescriptionW
#else   // UNICODE
    #define     getServiceName          getServiceNameA
    #define     getServiceDisplayName   getServiceDisplayNameA
    #define     getServiceDescription   getServiceDescriptionA
#endif  // UNICODE


namespace
{
    SERVICE_STATUS          _serviceStatus{ };
    SERVICE_STATUS_HANDLE   _statusHandle{ nullptr };
    SERVICE_TABLE_ENTRY     _serviceTable[2]{ };
} // namespace

//////////////////////////////////////////////////////////////////////////
// ServiceApplicationBase class implementation
//////////////////////////////////////////////////////////////////////////

bool ServiceApplicationBase::_osIsValid(void) const
{
    return (mSeMHandle != nullptr && mSvcHandle != nullptr);
}

void ServiceApplicationBase::_osFreeResources(void)
{
    if (mSvcHandle != nullptr)
    {
        ::CloseServiceHandle(reinterpret_cast<SC_HANDLE>(mSvcHandle));
    }

    if (mSeMHandle != nullptr)
    {
        ::CloseServiceHandle(reinterpret_cast<SC_HANDLE>(mSeMHandle));
    }

    mSvcHandle = nullptr;
    mSeMHandle = nullptr;
}

bool ServiceApplicationBase::_osInitializeService(void)
{
    NEMemory::zeroElement<SERVICE_STATUS>(_serviceStatus);
    _serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    _serviceStatus.dwCurrentState = SERVICE_STOPPED;
    _serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
    _serviceStatus.dwWin32ExitCode = NO_ERROR;
    _serviceStatus.dwServiceSpecificExitCode = 0;
    _serviceStatus.dwCheckPoint = 0;
    _serviceStatus.dwWaitHint = 0;

    return true;
}

bool ServiceApplicationBase::_osOpenService(void)
{
    if (mSeMHandle == nullptr)
    {
        mSeMHandle = reinterpret_cast<void*>(::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS));
    }

    if ((mSeMHandle != nullptr) && (mSvcHandle == nullptr))
    {
        mSvcHandle = reinterpret_cast<void*>(::OpenService(reinterpret_cast<SC_HANDLE>(mSeMHandle), getServiceName(), SERVICE_ALL_ACCESS));
    }

    return (mSvcHandle != nullptr);
}

bool ServiceApplicationBase::_osCreateService(void)
{
    if (mSeMHandle == nullptr)
    {
        mSeMHandle = reinterpret_cast<void*>(::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS));
    }

    if ((mSeMHandle != nullptr) && (mSvcHandle == nullptr))
    {
        TCHAR szPath[MAX_PATH];
        if (::GetModuleFileName(nullptr, szPath, MAX_PATH))
        {
            TEString<TCHAR> modulePath{ _T('\"') };
            modulePath += szPath;
            modulePath += _T("\" --service");

#define DEVELOPMENT_PENDING     0

#if defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
            DWORD startType = SERVICE_DEMAND_START;
#else   // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)
            DWORD startType = SERVICE_AUTO_START;
#endif  // defined(DEVELOPMENT_PENDING) && (DEVELOPMENT_PENDING != 0)

            mSvcHandle = reinterpret_cast<void*>(::CreateService( reinterpret_cast<SC_HANDLE>(mSeMHandle), getServiceName(), getServiceDisplayName(), SERVICE_ALL_ACCESS
                                                                , SERVICE_WIN32_OWN_PROCESS, startType, SERVICE_ERROR_NORMAL, modulePath.getString()
                                                                , nullptr, nullptr, nullptr, nullptr, nullptr));
            if (mSvcHandle != nullptr)
            {
                SERVICE_DESCRIPTION description;
                description.lpDescription = getServiceDescription();
                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DESCRIPTION, &description);

                SERVICE_FAILURE_ACTIONS_FLAG actionFlag { 1 };
                actionFlag.fFailureActionsOnNonCrashFailures = TRUE;
                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS_FLAG, &actionFlag);

                constexpr uint32_t count{ 5 };
                SERVICE_FAILURE_ACTIONS failures { };
                SC_ACTION actions[count];

                failures.dwResetPeriod  = INFINITE;
                failures.lpRebootMsg    = nullptr;
                failures.lpCommand      = nullptr;
                failures.cActions       = count;
                failures.lpsaActions    = actions;

                // first failure
                actions[0].Delay        = NECommon::WAIT_1_SECOND;
                actions[0].Type         = SC_ACTION_RESTART;

                // first second
                actions[1].Delay        = NECommon::WAIT_1_SECOND;
                actions[1].Type         = SC_ACTION_RESTART;

                // third failure
                actions[2].Delay        = NECommon::WAIT_5_SECONDS;
                actions[2].Type         = SC_ACTION_RESTART;

                // fourth failure
                actions[3].Delay        = NECommon::WAIT_10_SECONDS;
                actions[3].Type         = SC_ACTION_RESTART;

                actions[count - 1].Delay= NECommon::DO_NOT_WAIT;
                actions[count - 1].Type = SC_ACTION_NONE;

                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_FAILURE_ACTIONS, &failures);

#if !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
                SERVICE_DELAYED_AUTO_START_INFO delay = { 1 };
                delay.fDelayedAutostart = FALSE;
                ::ChangeServiceConfig2(reinterpret_cast<SC_HANDLE>(mSvcHandle), SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &delay);
#endif // !defined(DEVELOPMENT_PENDING) || (DEVELOPMENT_PENDING == 0)
            }
        }
    }

    return (mSvcHandle != nullptr);
}

void ServiceApplicationBase::_osDeleteService(void)
{
    if (mSvcHandle != nullptr)
    {
        ::DeleteService(reinterpret_cast<SC_HANDLE>(mSvcHandle));
    }
}

bool ServiceApplicationBase::_osRegisterService(void)
{
    if (mSystemServiceOption == NESystemService::eServiceOption::CMD_Service)
    {
        _statusHandle = ::RegisterServiceCtrlHandler(getServiceName(), &::_win32ServiceCtrlHandler);
    }

    return (_statusHandle != nullptr);
}

bool ServiceApplicationBase::_osSetState(NESystemService::eSystemServiceState newState)
{
    bool result{ true };

    _serviceStatus.dwControlsAccepted = 0;
    _serviceStatus.dwWin32ExitCode = 0;

    if (newState != mSystemServiceState)
    {
        switch (newState)
        {
        case NESystemService::eSystemServiceState::ServiceStopped:
            _serviceStatus.dwCurrentState       = SERVICE_STOPPED;
            _serviceStatus.dwControlsAccepted   = 0;
            _serviceStatus.dwCheckPoint         = 7;
            _serviceStatus.dwWin32ExitCode      = ERROR_SUCCESS;
            break;

        case NESystemService::eSystemServiceState::ServiceStarting:
            _serviceStatus.dwCurrentState       = SERVICE_START_PENDING;
            _serviceStatus.dwCheckPoint         = 1;
            break;

        case NESystemService::eSystemServiceState::ServiceStopping:
            _serviceStatus.dwCurrentState       = SERVICE_STOP_PENDING;
            _serviceStatus.dwCheckPoint         = 6;
            break;

        case NESystemService::eSystemServiceState::ServiceRunning:
            _serviceStatus.dwCurrentState       = SERVICE_RUNNING;
            _serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            _serviceStatus.dwCheckPoint         = 2;
            break;

        case NESystemService::eSystemServiceState::ServiceContinuing:
            _serviceStatus.dwCurrentState       = SERVICE_CONTINUE_PENDING;
            _serviceStatus.dwCheckPoint         = 5;
            break;

        case NESystemService::eSystemServiceState::ServicePausing:
            _serviceStatus.dwCurrentState       = SERVICE_PAUSE_PENDING;
            _serviceStatus.dwCheckPoint         = 3;
            break;

        case NESystemService::eSystemServiceState::ServicePaused:
            _serviceStatus.dwCurrentState       = SERVICE_PAUSED;
            _serviceStatus.dwControlsAccepted   = SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_STOP;
            _serviceStatus.dwCheckPoint         = 4;
            break;

        default:
            ASSERT(false);
        }

        mSystemServiceState = newState;
        if (_statusHandle != nullptr)
        {
            result = ::SetServiceStatus(_statusHandle, &_serviceStatus) != FALSE;
        }
    }

    return result;
}

bool ServiceApplicationBase::_osWaitUserInput(char* buffer, unsigned int bufSize)
{
#if !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    #if defined(_WIN32) && !defined(_MINGW)
        return (::gets_s(buffer, bufSize) != nullptr);
    #else   // defined(_WIN32)
        return (::fgets(buffer, bufSize, stdin) != nullptr);
    #endif  // defined(_WIN32)
#else
    return (::gets_s(buffer, bufSize) != nullptr);
#endif // _WIN32
}

int ServiceApplicationBase::_osStartServiceDispatcher(void)
{
    _serviceTable[0].lpServiceName = getServiceName();
    _serviceTable[0].lpServiceProc = &::_win32ServiceMain;
    _serviceTable[1].lpServiceName = nullptr;
    _serviceTable[1].lpServiceProc = nullptr;
    return (::StartServiceCtrlDispatcher(_serviceTable) ? RESULT_SUCCEEDED : RESULT_FAILED_INIT);
}

#endif // _WIN32
