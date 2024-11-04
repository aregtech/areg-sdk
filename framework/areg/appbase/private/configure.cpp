/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/appbase/private/configure.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Configuration Header file
 *              Set required configuration includes here
 *
 ************************************************************************/
#include "areg/appbase/private/configure.hpp"
#include "areg/base/GEMacros.h"

#ifdef DEBUG

    #if !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> WARNING: No import / export compiler option set. Use default IMP_AREG_DLL. Refer to the GESwitches.h file for details.")
        #else
            #pragma message ">>> WARNING: No import / export compiler option set. Use default IMP_AREG_DLL. Refer to the GESwitches.h file for details."
        #endif
    #endif  // !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)

    #if defined(WIN32)
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> Compiling with Win32 API, Debug version")
        #else
            #pragma message ">>> Compiling with Win32 API, Debug version"
        #endif
    #else   // WIN32
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> Compiling with POSIX API, Debug version")
        #else
            #pragma message ">>> Compiling with POSIX API, Debug version"
        #endif
    #endif

    #if !defined(AREG_LOGS)
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> The AREG_LOGS is not defined, setting default value 1")
        #else
            #pragma message ">>> The AREG_LOGS is not defined, setting default value 1"
        #endif
    #elif AREG_LOGS
        #ifdef MS_VISUAL_CPP
            #pragma message (">>> Compiling the sources WITH logs")
        #else
            #pragma message ">>> Compiling the sources WITH logs"
        #endif
    #else   // !AREG_LOGS
        #ifdef MS_VISUAL_CPP
            #pragma message (">>> Compiling the sources WITHOUT logs")
        #else
            #pragma message ">>> Compiling the sources WITHOUT logs"
        #endif
    #endif // AREG_LOGS

    #if !defined(AREG_EXTENDED)
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> The AREG_EXTENDED is not defined, setting default value 0")
        #else
            #pragma message ">>> The AREG_EXTENDED is not defined, setting default value 0"
        #endif
    #elif AREG_EXTENDED
        #ifdef MS_VISUAL_CPP
            #pragma message(">>> Compiling the sources WITH extended features")
        #else
            #pragma message ">>> Compiling the sources WITH extended features"
        #endif
    #else   // !AREG_EXTENDED
        #ifdef MS_VISUAL_CPP
            #pragma message (">>> Compiling the sources WITHOUT extended features")
        #else
            #pragma message ">>> Compiling the sources WITHOUT extended features"
        #endif
    #endif // AREG_EXTENDED

#endif // DEBUG

#ifdef  WIN32

    // Enable or disable memory leak detect only for Debug version
    #ifdef  DEBUG
        #pragma comment(lib, "dbghelp.lib")
    #endif  // DEBUG

    #pragma comment(lib, "advapi32.lib")
    #pragma comment(lib, "psapi.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "ws2_32.lib")

#endif  // WIN32
