#ifndef AREG_BASE_GEGLOBAL_H
#define AREG_BASE_GEGLOBAL_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/base/GEGlobal.h
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, global include header file
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

/**
 * \brief   This header file is included in every file of a project.
 *          The header file contains macro defines for MC Windows OS,
 *          includes standard headers, header of switches 
 *          to use import / exports and standard global macro.
 *          Anything global, which is relevant for all includes,
 *          should be included and/or defined here.
 **/

#include "system/GEPlatform.h"

#define OUTPUT_DEBUG_LEVEL_DEBUG    4
#define OUTPUT_DEBUG_LEVEL_INFO     3
#define OUTPUT_DEBUG_LEVEL_WARN     2
#define OUTPUT_DEBUG_LEVEL_ERROR    1
#define OUTPUT_DEBUG_LEVEL_NONE     0
#define OUTPUT_DEBUG_LEVEL_ALL      OUTPUT_DEBUG_LEVEL_DEBUG

#ifdef DEBUG
    #ifndef OUTPUT_DEBUG_LEVEL
        #define OUTPUT_DEBUG_LEVEL      OUTPUT_DEBUG_LEVEL_NONE
    #endif // !OUTPUT_DEBUG_LEVEL
#else
    #undef  OUTPUT_DEBUG_LEVEL
    #define OUTPUT_DEBUG_LEVEL          OUTPUT_DEBUG_LEVEL_NONE
#endif // DEBUG


/**
 * \brief   Basic types
 **/
#include "areg/base/GETypes.h"

/**
 * \brief   AREG core Switches and MACRO
 **/
#include "areg/base/GESwitches.h"
#include "areg/base/GEMacros.h"

#ifndef AREG_EXTENSIONS
    #ifdef ENABLE_AREG_EXTENSIONS
        // AREG extensions enable global flag.
        #define AREG_EXTENSIONS     1
    #else   // !ENABLE_AREG_EXTENSIONS
        // If not directly defined, disable AREG extensions
        #define AREG_EXTENSIONS     0
    #endif // ENABLE_AREG_EXTENSIONS
#endif // ENABLE_AREG_EXTENSIONS


#ifdef _DEBUG
    #if (defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0)) 
        
        #undef  OUTPUT_DEBUG
        #define OUTPUT_DEBUG

    #else // !(defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0))

        #undef  OUTPUT_DEBUG
    
    #endif  // !(defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0))
#else   // _NDEBUG
        #undef  OUTPUT_DEBUG
#endif  // _DEBUG

#endif  // AREG_BASE_GEGLOBAL_H
