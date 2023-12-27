/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/private/LogObserverApi.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "observer/lib/LogObserverApi.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SynchObjects.hpp"

#include <atomic>

// Use these options if compile for Windows with MSVC
#ifdef WINDOWS
    #pragma comment(lib, "areg")
#endif // WINDOWS


namespace
{
    struct sLogObserverStruct
    {
        Mutex   losLock{ false };
        bool    losInitialized{ false };
    };

    sLogObserverStruct theObserver;
}

OBSERVER_API_IMPL bool logObserverInitialize(const char* configFilePath)
{
    Lock lock(theObserver.losLock);

    if (theObserver.losInitialized == false)
    {
        theObserver.losInitialized = true;
        Application::initApplication(true, false, false, true, false, configFilePath);
    }

    return theObserver.losInitialized;
}

OBSERVER_API_IMPL void logObserverUninitialize()
{
    Lock lock(theObserver.losLock);

    if (theObserver.losInitialized)
    {
        Application::releaseApplication();
        theObserver.losInitialized = false;
    }
}

OBSERVER_API_IMPL bool logObserverIsInitialized()
{
    Lock lock(theObserver.losLock);
    return theObserver.losInitialized;
}
