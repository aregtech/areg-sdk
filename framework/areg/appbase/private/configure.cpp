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

#if !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)
    #pragma message("WARNING: No import / export compiler option set. Use default IMP_AREG_DLL. Refer to the GESwitches.h file in the AREG base for details.")
#endif  // !defined(EXP_AREG_DLL) && !defined(EXP_AREG_LIB) && !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB)

// By default, no AREG extended features.
#if !defined(AREG_EXTENDED) && !defined(AREG_EXTEND)
    #pragma message("The AREG_EXTENDED is not defined, setting default value 0")
#endif  // AREG_EXTENDED

// By default, compile with logs
#ifndef AREG_LOGS
    #pragma message("The AREG_LOGS is not defined, setting default value 1")
#endif  // AREG_LOGS

#ifdef  WIN32

    // Enable or disable memory leak detect only for Debug version
    #ifdef  DEBUG
        #pragma comment(lib, "dbghelp.lib")
    #endif  // DEBUG

    #pragma comment(lib, "advapi32.lib")
    #pragma comment(lib, "psapi.lib")
    #pragma comment(lib, "shell32.lib")
    #pragma comment(lib, "ws2_32.lib")

#else   // !WIN32

#endif  // WIN32
