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
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/base/GEGlobal.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
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


#include "areg/system/GEPlatform.h"

#define OUTPUT_DEBUG_LEVEL_DEBUG    4
#define OUTPUT_DEBUG_LEVEL_INFO     3
#define OUTPUT_DEBUG_LEVEL_WARN     2
#define OUTPUT_DEBUG_LEVEL_ERROR    1
#define OUTPUT_DEBUG_LEVEL_NONE     0
#define OUTPUT_DEBUG_LEVEL_ALL      OUTPUT_DEBUG_LEVEL_DEBUG

#ifdef DEBUG
    #if !defined(OUTPUT_DEBUG_LEVEL)
        #define OUTPUT_DEBUG_LEVEL      OUTPUT_DEBUG_LEVEL_NONE
    #elif   (OUTPUT_DEBUG_LEVEL > OUTPUT_DEBUG_LEVEL_ALL)
        #define OUTPUT_DEBUG_LEVEL      OUTPUT_DEBUG_LEVEL_ALL
    #endif // !OUTPUT_DEBUG_LEVEL
#else   // !DEBUG
    #undef  OUTPUT_DEBUG_LEVEL
    #define OUTPUT_DEBUG_LEVEL          OUTPUT_DEBUG_LEVEL_NONE
#endif // DEBUG

#if defined(IMP_AREG_DLL) || defined(_IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS) || defined(_IMPORT_SHARED_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define IMP_AREG_DLL
    #define _IMP_AREG_DLL

#endif  // defined(IMP_AREG_DLL) || defined(_IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS) || defined(_IMPORT_SHARED_SYMBOLS)

#if defined(IMP_AREG_LIB) || defined(_IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS) || defined(_IMPORT_STATIC_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define IMP_AREG_LIB
    #define _IMP_AREG_LIB

#endif  // defined(IMP_AREG_LIB) || defined(_IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS) || defined(_IMPORT_STATIC_SYMBOLS)

#if defined(EXP_AREG_DLL) || defined(_EXP_AREG_DLL) || defined(EXPORT_SHARED_SYMBOLS) || defined(_EXPORT_SHARED_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define EXP_AREG_DLL
    #define _EXP_AREG_DLL

#endif  // defined(EXP_AREG_DLL) || defined(_EXP_AREG_DLL) || defined(EXPORT_SHARED_SYMBOLS) || defined(_EXPORT_SHARED_SYMBOLS)

#if defined(EXP_AREG_LIB) || defined(_EXP_AREG_LIB) || defined(EXPORT_STATIC_SYMBOLS) || defined(_EXPORT_STATIC_SYMBOLS)

    #undef  IMP_AREG_DLL
    #undef  IMP_AREG_LIB
    #undef  EXP_AREG_DLL
    #undef  EXP_AREG_LIB

    #undef  _IMP_AREG_DLL
    #undef  _IMP_AREG_LIB
    #undef  _EXP_AREG_DLL
    #undef  _EXP_AREG_LIB

    #undef  IMPORT_SHARED_SYMBOLS
    #undef  IMPORT_STATIC_SYMBOLS
    #undef  EXPORT_SHARED_SYMBOLS
    #undef  EXPORT_STATIC_SYMBOLS

    #define EXP_AREG_LIB
    #define _EXP_AREG_LIB

#endif  // defined(EXP_AREG_LIB) || defined(_EXP_AREG_LIB) || defined(EXPORT_STATIC_SYMBOLS) || defined(_EXPORT_STATIC_SYMBOLS)

/**
 * \brief   Basic types
 **/
#include "areg/base/GETypes.h"

/**
 * \brief   AREG core Switches and MACRO
 **/
#include "areg/base/GESwitches.h"
#include "areg/base/GEMacros.h"

#ifdef DEBUG
    #if (defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0)) 
        
        #undef  OUTPUT_DEBUG
        #define OUTPUT_DEBUG    1

    #else // !(defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0))

        #undef  OUTPUT_DEBUG
    
    #endif  // !(defined(OUTPUT_DEBUG) && (OUTPUT_DEBUG != 0))
#else   // _NDEBUG
        #undef  OUTPUT_DEBUG
#endif  // _DEBUG

#endif  // AREG_BASE_GEGLOBAL_H
