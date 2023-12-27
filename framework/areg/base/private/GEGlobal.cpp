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

#ifdef  WINDOWS

    #if     WIN32
        MACRO_COMPILER_MESSAGE("Compiling with Win32 API")
    #else   // WIN32
        MACRO_COMPILER_MESSAGE( "Compiling in Windows with unknown API" )
    #endif // WIN32

    #if     OUTPUT_DEBUG
        MACRO_COMPILER_MESSAGE("Compiling the sources WITH debug outputs, the output debug level is" OUTPUT_DEBUG_LEVEL)
    #else   // !OUTPUT_DEBUG
        MACRO_COMPILER_MESSAGE("Compiling the sources WITHOUT debug outputs")
    #endif // OUTPUT_DEBUG

    #if     AREG_LOGS
        MACRO_COMPILER_MESSAGE("Compiling the sources WITH logs")
    #else   // !AREG_LOGS
        MACRO_COMPILER_MESSAGE("Compiling the sources WITHOUT logs")
    #endif // AREG_LOGS

    #if     AREG_EXTENDED
        MACRO_COMPILER_MESSAGE("Compiling the sources WITH extended features")
    #else   // !AREG_EXTENDED
        MACRO_COMPILER_MESSAGE("Compiling the sources WITHOUT extended features")
    #endif // AREG_EXTENDED

#endif  // WINDOWS
