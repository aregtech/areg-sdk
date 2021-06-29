/************************************************************************
 * \file        areg/appbase/private/configure.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Configuration Header file
 *              Set required configuration includes here
 *
 ************************************************************************/
#include "areg/appbase/private/configure.hpp"

#ifdef  WIN32

    // Enable or disable memory leak detect only for Debug version
    #ifdef  _DEBUG
        #if (defined(COMPILE_ENABLED_LEAK_DETECT) && COMPILE_ENABLED_LEAK_DETECT)
            #include "vld/vld.h"
        #endif  // (defined(COMPILE_ENABLED_LEAK_DETECT) && COMPILE_ENABLED_LEAK_DETECT)

        #pragma comment(lib, "dbghelp.lib")
    #endif  // _DEBUG

    #pragma comment(lib, "psapi.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "ws2_32.lib")
    #pragma comment(lib, "advapi32.lib")

#else   // WIN32

#endif  // WIN32
