/************************************************************************
 * \file        areg/appbase/private/ApplicationWin.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Windows OS specific Application methods implementation
 *              Windows apecifix API calls.
 ************************************************************************/
#include "areg/appbase/Application.hpp"

#ifdef _WINDOWS

#include <Windows.h>

void Application::_osSetupHandlers( void )
{
    Application & theApp = Application::getInstance();
    Lock lock(theApp.mLock);

    if (theApp.mSetup == false)
    {
        theApp.mSetup   = true;
    }
}

void Application::_osReleaseHandlers(void)
{
    Application& theApp = Application::getInstance();
    Lock lock(theApp.mLock);

    if (theApp.mSetup)
    {
        theApp.mSetup = false;
    }
}

/**
 * \brief   Windows OS specific implementation of method.
 **/
bool Application::_osStartLocalService(const wchar_t* serviceName, const wchar_t* /*serviceExecutable*/)
{
    ASSERT(NEString::isEmpty<wchar_t>(serviceName) == false);
    bool result = false;

    DWORD rights = SC_MANAGER_CONNECT | SC_MANAGER_ENUMERATE_SERVICE | SC_MANAGER_QUERY_LOCK_STATUS | STANDARD_RIGHTS_READ;
    SC_HANDLE SeMHandle = ::OpenSCManager(nullptr, nullptr, rights);
    if (SeMHandle != nullptr)
    {
        rights = SERVICE_PAUSE_CONTINUE | SERVICE_QUERY_STATUS | SERVICE_START;
        SC_HANDLE SvcHandle = ::OpenService(SeMHandle, serviceName, rights);
        if (SvcHandle != nullptr)
        {
            SERVICE_STATUS serviceStatus = { 0 };
            if (::QueryServiceStatus(SvcHandle, &serviceStatus))
            {
                switch (serviceStatus.dwCurrentState)
                {
                    /*nothing to do*/
                case SERVICE_CONTINUE_PENDING:  // fall through
                case SERVICE_START_PENDING:     // fall through
                case SERVICE_RUNNING:
                    result = true;
                    break;

                    /*service was paused*/
                case SERVICE_PAUSE_PENDING:
                case SERVICE_PAUSED:
                    result = ::ControlService(SvcHandle, SERVICE_CONTROL_CONTINUE, &serviceStatus) != FALSE;
                    break;

                    /*service not running*/
                case SERVICE_STOP_PENDING:
                case SERVICE_STOPPED:
                    result = ::StartService(SvcHandle, 0, nullptr) != FALSE;
                    break;

                    /*in all other cases*/
                default:
                    break;
                }
            }

            ::CloseServiceHandle(SvcHandle);
        }

        ::CloseServiceHandle(SeMHandle);
    }

    return result;
}

#endif // _WINDOWS
