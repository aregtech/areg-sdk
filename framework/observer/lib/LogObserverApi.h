#ifndef AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
#define AREG_OBSERVER_LIB_LOGOBSERVERAPI_H
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/LogObserverApi.h
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "observer/lib/LogObserverSwitches.h"

/**
 * \brief   Call to initialize the log observer internals. This function should
 *          be called before any other. The multiple call of this function will not
 *          have any effect.
 * \param   configFilePath  Relative or absolute path to the application configuration file.
 *                          If NULL, it uses the default location of the config file './config/areg.init'
 * \returns Returns true, if succeeded to initialize internals. Otherwise, returns false.
 **/
OBSERVER_API bool logObserverInitialize(const char * configFilePath = 0u);

/**
 * \brief   Call to release the log observer internals and release resources. This function should
 *          be called at the end of application. Once the observer is uninitialized, it will be
 *          not possible to use any other API and trigger remote methods.
 **/
OBSERVER_API void logObserverUninitialize();

OBSERVER_API bool logObserverIsInitialized();

#endif  // AREG_OBSERVER_LIB_LOGOBSERVERAPI_H

