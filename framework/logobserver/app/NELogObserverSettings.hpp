#ifndef AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
#define AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
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

#if !defined(IMP_LOGOBSERVER_DLL) && !defined(IMP_LOGOBSERVER_LIB)
    #define     IMP_LOGOBSERVER_DLL
#endif  // !defined(IMP_LOGOBSERVER_DLL) || !defined(IMP_LOGOBSERVER_LIB)

#if 0
#if !defined(AREG_EXTENDED)
    #define AREG_EXTENDED           1
#elif (AREG_EXTENDED == 0)
    #undef  AREG_EXTENDED
    #define AREG_EXTENDED           1
#endif  // !defined(AREG_EXTENDED)
#endif 

/************************************************************************
 * Include files.
 ************************************************************************/
#include "logobserver/lib/LogObserverSwitches.h"

#include <string_view>

namespace NELogObserverSettings
{
    /**
     * \brief   The name of main thread in the console log observer application.
     **/
    constexpr char      LOG_LOGOBSERVER_THREAD_NAME[]  { "_AREG_LOG_OBSERVER_THREAD_" };

    /**
     * \brief   The title to display on console when run application.
     **/
    constexpr std::string_view   APP_TITLE{ "AREG Log Observer console application ..." };

}

#endif // AREG_LOGOBSERVER_APP_NELOGOBSERVERSETTINGS_HPP
