/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/private/GEGlobal.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform global include
 *
 ************************************************************************/
#include "areg/base/GEGlobal.h"


    #if (defined(WIN32) && WIN32)
        #ifdef _DEBUG
            #pragma message("Compiling with Win32 API, Debug version")
#error _DEBUG
        #else
            #pragma message("Compiling with Win32 API, Release version")
        #endif
    #else   // WIN32
        #ifdef _DEBUG
            #pragma message("Compiling in Windows with unknown API, Debug version")
#error _DEBUG
        #else
            #pragma message("Compiling in Windows with unknown API, Release version")
#error _NDEBUG
        #endif
    #endif // WIN32

    #if (defined(AREG_LOGS) && AREG_LOGS)
        #pragma message("Compiling the sources WITH logs")
    #else   // !AREG_LOGS
        #pragma message("Compiling the sources WITHOUT logs")
    #endif // AREG_LOGS

    #if (defined(AREG_EXTENDED) && AREG_EXTENDED)
        #pragma message("Compiling the sources WITH extended features")
    #else   // !AREG_EXTENDED
        #pragma message("Compiling the sources WITHOUT extended features")
    #endif // AREG_EXTENDED
