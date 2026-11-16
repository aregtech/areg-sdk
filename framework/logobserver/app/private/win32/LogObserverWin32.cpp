/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/app/private/LogObserverWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       LogObserver, Win32 specific implementation
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/app/LogObserver.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"

#ifdef _WIN32

    #ifdef _MSC_VER
        #pragma comment(lib, "areg")
        #pragma comment(lib, "aregextend")
        #pragma comment(lib, "areglogger")
    #endif // _MSC_VER

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <tchar.h>

#include <cstdlib>

//////////////////////////////////////////////////////////////////////////
// Global functions, Begin
//////////////////////////////////////////////////////////////////////////

#ifndef _MINGW
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
    static_cast<void>(envp);
    char** argvTemp = areg::ext::convert_arguments<TCHAR>(argv, static_cast<int32_t>(argc));
    if ((argvTemp == nullptr) && (argc > 0))
    {
        return EXIT_FAILURE;
    }

    LogObserver::instance().log_main(static_cast<int32_t>(argc), argvTemp);
    areg::ext::delete_arguments(argvTemp, static_cast<int32_t>(argc));

    return 0;
}
#else
int main(int argc, char* argv[], char* envp[])
{
    LogObserver::instance().log_main(static_cast<int32_t>(argc), argv);
    return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////
// Global functions, End
//////////////////////////////////////////////////////////////////////////

#endif // _WIN32
