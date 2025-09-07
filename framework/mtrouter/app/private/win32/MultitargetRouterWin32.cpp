/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/MultitargetRouterWin32.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Router, Multitarget Router Service process
 ************************************************************************/
#include "mtrouter/app/MultitargetRouter.hpp"

#ifdef _WIN32

    #ifdef _MSC_VER
        #pragma comment(lib, "areg")
        #pragma comment(lib, "aregextend")
        #pragma comment(lib, "advapi32")
        #pragma comment(lib, "kernel32")
    #endif // _MSC_VER

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

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

#ifndef _MINGW
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    static_cast<void>(envp);
    int result{ ServiceApplicationBase::RESULT_FAILED_RUN };
    char** argvTemp = NESystemService::convertArguments<TCHAR>(argv, argc);
    MultitargetRouter& router = MultitargetRouter::getInstance();
    router.parseOptions(static_cast<int>(argc), argvTemp, NESystemService::ServiceOptionSetup, MACRO_ARRAYLEN(NESystemService::ServiceOptionSetup));
    result = router.serviceMain(router.getCurrentOption(), nullptr);
    NESystemService::deleteArguments(argvTemp, argc);

    return result;
}
#else   // _MINGW
int main(int argc, char* argv[], char* envp[])
{
    MultitargetRouter& router = MultitargetRouter::getInstance();
    router.parseOptions(argc, argv, NESystemService::ServiceOptionSetup, MACRO_ARRAYLEN(NESystemService::ServiceOptionSetup));
    return router.serviceMain(router.getCurrentOption(), nullptr);
}
#endif  // _MINGW

VOID WINAPI _win32ServiceMain( DWORD argc, LPTSTR * argv )
{
    try
    {
        MultitargetRouter& router = MultitargetRouter::getInstance();
        router.setState(NESystemService::eSystemServiceState::ServiceStarting);
        char** argvTemp = NESystemService::convertArguments<TCHAR>(argv, static_cast<int>(argc));
        router.serviceMain(NESystemService::eServiceOption::CMD_Service, argvTemp != nullptr ? argvTemp[0] : nullptr);
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
        MultitargetRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceStop);
        break;

    case SERVICE_CONTROL_PAUSE:
        MultitargetRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServicePause);
        break;

    case SERVICE_CONTROL_CONTINUE:
        MultitargetRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceContinue);
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        MultitargetRouter::getInstance().controlService(SystemServiceBase::eServiceControl::ServiceShutdown);
        break;

    default:
        break;  // do nothing
    }
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // _WIN32
