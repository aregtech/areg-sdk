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

#include "observer/lib/private/LoggerClient.hpp"

#include <atomic>

// Use these options if compile for Windows with MSVC
#ifdef WINDOWS
    #pragma comment(lib, "areg")
#endif // WINDOWS


namespace
{
    struct sLogObserverStruct
    {
        Mutex           losLock         { false };
        bool            losInitialized  { false };
        sObserverEvents losEvents       { };
    };

    sLogObserverStruct theObserver;

    void setCallbacks(const sObserverEvents* srcCallbacks, sObserverEvents & dstCallbacks)
    {
        if (srcCallbacks != nullptr)
        {
            dstCallbacks.evtServiceConnected = srcCallbacks->evtServiceConnected;
            dstCallbacks.evtMessagingFailed  = srcCallbacks->evtMessagingFailed;
        }
        else
        {
            dstCallbacks.evtServiceConnected = nullptr;
            dstCallbacks.evtMessagingFailed  = nullptr;
        }
    }
}

OBSERVER_API_IMPL bool logObserverInitialize(const sObserverEvents * callbacks, const char* configFilePath /* = nullptr */)
{
    Lock lock(theObserver.losLock);

    if (theObserver.losInitialized == false)
    {
        theObserver.losInitialized  = true;
        setCallbacks(callbacks, theObserver.losEvents);
        Application::initApplication(true, false, false, true, false, configFilePath);

        LoggerClient& client = LoggerClient::getInstance();
        client.setCallbacks(&theObserver.losEvents);
    }

    return theObserver.losInitialized;
}

OBSERVER_API_IMPL bool logObserverConnectLogger(const char* ipAddress /*= nullptr*/, unsigned short portNr /* = 0 */)
{
    return LoggerClient::getInstance().startLoggerClient(ipAddress, portNr);
}

OBSERVER_API_IMPL void logObserverDisconnectLogger()
{
    LoggerClient::getInstance().stopLoggerClient();
}

OBSERVER_API_IMPL void logObserverRelease()
{
    Lock lock(theObserver.losLock);

    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.setCallbacks(nullptr);
        client.stopLoggerClient();
        Application::releaseApplication();
        setCallbacks(nullptr, theObserver.losEvents);
        theObserver.losInitialized = false;
    }
}

OBSERVER_API_IMPL bool logObserverIsInitialized()
{
    Lock lock(theObserver.losLock);
    return theObserver.losInitialized;
}

OBSERVER_API_IMPL bool logObserverIsConnected()
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConnectedState();
    }

    return result;
}

OBSERVER_API_IMPL const char* logObserverLoggerAddress()
{
    Lock lock(theObserver.losLock);
    const char * result{ nullptr };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostAddress().getString();
    }

    return result;
}

OBSERVER_API_IMPL unsigned short logObserverLoggerPort()
{
    Lock lock(theObserver.losLock);
    unsigned short result{ NESocket::InvalidPort };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostPort();
    }

    return result;
}

OBSERVER_API_IMPL bool logObserverConfigLoggerEnabled()
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConfigLoggerConnectEnabled();
    }

    return result;
}

OBSERVER_API_IMPL bool logObserverConfigLoggerAddress(char* addrBuffer, int count)
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        String addr{ client.getConfigLoggerAddress() };
        if ((addrBuffer != nullptr) && (addr.getLength() > static_cast<NEString::CharCount>(count)))
        {
            result = NEString::copyString<char, char>(addrBuffer, count, addr.getString(), addr.getLength()) > 0;
        }
    }

    return result;
}

OBSERVER_API_IMPL unsigned short logObserverConfigLoggerPort()
{
    Lock lock(theObserver.losLock);
    uint16_t result{ NESocket::InvalidPort };
    if (theObserver.losInitialized)
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getConfigLoggerPort();
    }

    return result;
}

OBSERVER_API_IMPL void logObserverRequestInstances()
{
    Lock lock(theObserver.losLock);
    if (theObserver.losInitialized)
    {
        LoggerClient::getInstance().requestConnectedInstances();
    }
}

OBSERVER_API_IMPL void logObserverRequestScopes(unsigned long long target /* = ID_IGNORED */)
{
    Lock lock(theObserver.losLock);
    if (theObserver.losInitialized)
    {
        LoggerClient::getInstance().requestScopes(target);
    }
}

OBSERVER_API_IMPL void logObserverRequestChangeScopePrio(unsigned long long target, const sLogScope* scopes, unsigned int count)
{
    Lock lock(theObserver.losLock);
    if (theObserver.losInitialized && (target != ID_IGNORE))
    {
        NETrace::ScopeNames scopeList(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            scopeList.add(NETrace::sScopeInfo(scopes[i].lsName, scopes[i].lsId, scopes[i].lsPrio));
        }

        LoggerClient::getInstance().requestChangeScopePrio( scopeList, target);
    }
}

OBSERVER_API_IMPL void logObserverRequestSaveConfig(unsigned long long target /* = ID_IGNORED */)
{
    Lock lock(theObserver.losLock);
    if (theObserver.losInitialized)
    {
        LoggerClient::getInstance().requestSaveConfiguration(target);
    }
}
