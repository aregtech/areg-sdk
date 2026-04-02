/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        mtrouter/app/private/MultitargetRouterWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
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
#include "areg/appbase/AppDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/File.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
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
    int32_t result{ areg::ext::ServiceApplicationBase::RESULT_FAILED_RUN };
    char** argvTemp = areg::ext::convert_arguments<TCHAR>(argv, argc);
    MultitargetRouter& router = MultitargetRouter::instance();
    router.parse_options(static_cast<int32_t>(argc), argvTemp, areg::ext::ServiceOptionSetup, std::size(areg::ext::ServiceOptionSetup));
    result = router.service_main(router.current_option(), nullptr);
    areg::ext::delete_arguments(argvTemp, argc);

    return result;
}
#else   // _MINGW
int main(int argc, char* argv[], char* envp[])
{
    MultitargetRouter& router = MultitargetRouter::instance();
    router.parse_options(argc, argv, areg::ext::ServiceOptionSetup, std::size(areg::ext::ServiceOptionSetup));
    return router.service_main(router.current_option(), nullptr);
}
#endif  // _MINGW

VOID WINAPI _win32_service_main( DWORD argc, LPTSTR * argv )
{
    try
    {
        MultitargetRouter& router = MultitargetRouter::instance();
        router.set_state(areg::ext::ServicePhase::Starting);
        char** argvTemp = areg::ext::convert_arguments<TCHAR>(argv, static_cast<int32_t>(argc));
        router.service_main(areg::ext::ServiceOption::CMD_Service, argvTemp != nullptr ? argvTemp[0] : nullptr);
        areg::ext::delete_arguments(argvTemp, static_cast<int32_t>(argc));
        router.set_state(areg::ext::ServicePhase::Stopped);
    }
    catch (const std::exception & /*ex*/)
    {
        ASSERT(false);
    }
}

VOID WINAPI _win32_service_ctrl_handler(DWORD CtrlCode)
{
    switch ( CtrlCode )
    {
    case SERVICE_CONTROL_STOP:
        MultitargetRouter::instance().control_service(areg::ext::SystemServiceBase::ServiceControl::ServiceStop);
        break;

    case SERVICE_CONTROL_PAUSE:
        MultitargetRouter::instance().control_service(areg::ext::SystemServiceBase::ServiceControl::ServicePause);
        break;

    case SERVICE_CONTROL_CONTINUE:
        MultitargetRouter::instance().control_service(areg::ext::SystemServiceBase::ServiceControl::ServiceContinue);
        break;

    case SERVICE_CONTROL_SHUTDOWN:
        MultitargetRouter::instance().control_service(areg::ext::SystemServiceBase::ServiceControl::ServiceShutdown);
        break;

    default:
        break;  // do nothing
    }
}

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // _WIN32
