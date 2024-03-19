/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/MulticastRouterWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Router, Multicast Router Service process
 ************************************************************************/
#include "mcrouter/app/MulticastRouter.hpp"

#ifdef WINDOWS

#pragma comment(lib, "areg.lib")
#pragma comment(lib, "areg-extend.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "kernel32.lib")

#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>

#include <shellapi.h>

#include "mcrouter/resources/resource.hpp"

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    int result{ ServiceApplicationBase::RESULT_FAILED_RUN };
    char** argvTemp = NESystemService::convertArguments<TCHAR>(argv, argc);
    if (argvTemp != nullptr)
    {
        result = MulticastRouter::getInstance().serviceMain(argc, argvTemp);
        NESystemService::deleteArguments(argvTemp, argc);
    }

    return result;
}

VOID WINAPI _win32ServiceMain( DWORD argc, LPTSTR * argv )
{
    try
    {
        MulticastRouter & router = MulticastRouter::getInstance();
        router.setState(NESystemService::eSystemServiceState::ServiceStarting);

        char ** argvTemp = NESystemService::convertArguments<TCHAR>(argv, static_cast<int>(argc));
        router.serviceMain(static_cast<int>(argc), argvTemp);
        NESystemService::deleteArguments(argvTemp, static_cast<int>(argc));

        router.setState(NESystemService::eSystemServiceState::ServiceStopped);
    }
    catch (const std::exception & /*ex*/)
    {
        ASSERT(false);
    }
}

VOID WINAPI _win32ServiceCtrlHandler(DWORD CtrlCode)
{
    switch ( CtrlCode )
    {
    case SERVICE_CONTROL_STOP:
        MulticastRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceStop);
        break;

    case SERVICE_CONTROL_PAUSE:
        MulticastRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServicePause);
        break;

    case SERVICE_CONTROL_CONTINUE:
        MulticastRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceContinue);
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        MulticastRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceShutdown);
        break;

    default:
        break;  // do nothing
    }
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // WINDOWS
