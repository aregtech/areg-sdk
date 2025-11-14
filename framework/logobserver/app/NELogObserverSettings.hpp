#ifndef AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
#define AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logobserver/app/NELogObserverSettings.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer console application,
 *              Log Observer Settings.
 ************************************************************************/

#if !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB) && !defined(IMPORT_SHARED_SYMBOLS) && !defined(IMPORT_STATIC_SYMBOLS)
    #define IMP_AREG_DLL
#elif defined(IMP_AREG_DLL) || defined(IMPORT_SHARED_SYMBOLS)
    #undef  IMP_AREG_DLL
    #define IMP_AREG_DLL
#elif defined(IMP_AREG_LIB) || defined(IMPORT_STATIC_SYMBOLS)
    #undef  IMP_AREG_LIB
    #define IMP_AREG_LIB
#endif  // !defined(IMP_AREG_DLL) && !defined(IMP_AREG_LIB) && !defined(IMPORT_SHARED_SYMBOLS) && !defined(IMPORT_STATIC_SYMBOLS)

#if !defined(IMP_LOGGER_DLL) && !defined(IMP_LOGGER_LIB)
    #define     IMP_LOGGER_DLL
#endif  // !defined(IMP_LOGGER_DLL) || !defined(IMP_LOGGER_LIB)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areglogger/client/LogObserverSwitches.h"

#endif // AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
