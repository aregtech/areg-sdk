/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LogObserverApi.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "areglogger/client/LogObserverApi.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SynchObjects.hpp"

#include "areglogger/client/private/LoggerClient.hpp"

#include <atomic>

// Use these options if compile for Windows with MSVC
#ifdef WINDOWS
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "sqlite3")
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "aregsqlite3")
#endif  //defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
#endif // WINDOWS


namespace
{
    struct sLogObserverStruct
    {
        Mutex           losLock     { false };
        eObserverStates losState    { eObserverStates::ObserverUninitialized };
        sObserverEvents losEvents   { };
    };

    sLogObserverStruct theObserver;

    void _setCallbacks(sObserverEvents & dstCallbacks, const sObserverEvents* srcCallbacks)
    {
        if (srcCallbacks != nullptr)
        {
            dstCallbacks.evtObserverConfigured  = srcCallbacks->evtObserverConfigured;
            dstCallbacks.evtLogDbConfigured     = srcCallbacks->evtLogDbConfigured;
            dstCallbacks.evtServiceConnected    = srcCallbacks->evtServiceConnected;
            dstCallbacks.evtLoggingStarted      = srcCallbacks->evtLoggingStarted;
            dstCallbacks.evtMessagingFailed     = srcCallbacks->evtMessagingFailed;
            dstCallbacks.evtInstConnected       = srcCallbacks->evtInstConnected;
            dstCallbacks.evtInstDisconnected    = srcCallbacks->evtInstDisconnected;
            dstCallbacks.evtLogRegisterScopes   = srcCallbacks->evtLogRegisterScopes;
            dstCallbacks.evtLogUpdatedScopes    = srcCallbacks->evtLogUpdatedScopes;
            dstCallbacks.evtLogMessage          = srcCallbacks->evtLogMessage;
            dstCallbacks.evtLogMessageEx        = srcCallbacks->evtLogMessageEx;
        }
        else
        {
            dstCallbacks.evtObserverConfigured  = nullptr;
            dstCallbacks.evtLogDbConfigured     = nullptr;
            dstCallbacks.evtServiceConnected    = nullptr;
            dstCallbacks.evtLoggingStarted      = nullptr;
            dstCallbacks.evtMessagingFailed     = nullptr;
            dstCallbacks.evtInstConnected       = nullptr;
            dstCallbacks.evtInstDisconnected    = nullptr;
            dstCallbacks.evtLogRegisterScopes   = nullptr;
            dstCallbacks.evtLogUpdatedScopes    = nullptr;
            dstCallbacks.evtLogMessage          = nullptr;
            dstCallbacks.evtLogMessageEx        = nullptr;
        }
    }

    inline bool _isInitialized(eObserverStates state)
    {
        return (state != eObserverStates::ObserverUninitialized);
    }

    inline bool _isDisconnected(eObserverStates state)
    {
        return (state == eObserverStates::ObserverDisconnected);
    }

    inline bool _isConnected(eObserverStates state)
    {
        return (state >= eObserverStates::ObserverConnected);
    }

    inline bool _isStarted(eObserverStates state)
    {
        return (state == eObserverStates::ObserverConnected);
    }
}

LOGGER_API_IMPL bool logObserverInitialize(const sObserverEvents * callbacks, const char* configFilePath /* = nullptr */)
{
    Lock lock(theObserver.losLock);

    if (_isInitialized(theObserver.losState) == false)
    {
        LoggerClient& client = LoggerClient::getInstance();
        theObserver.losState = eObserverStates::ObserverDisconnected;
        _setCallbacks(theObserver.losEvents, callbacks);
        client.setCallbacks(&theObserver.losEvents);
        Application::initApplication(true, false, false, true, false, configFilePath, static_cast<IEConfigurationListener *>(&client));
    }

    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverConnectLogger(const char* dbPath, const char* ipAddress /*= nullptr*/, uint16_t portNr /* = 0 */)
{
    Lock lock(theObserver.losLock);

    if (_isDisconnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.openLoggingDatabase(dbPath);
        if (client.startLoggerClient(ipAddress, portNr))
        {
            theObserver.losState = eObserverStates::ObserverConnected;
        }
    }

    return _isConnected(theObserver.losState);
}

LOGGER_API_IMPL void logObserverDisconnectLogger()
{
    Lock lock(theObserver.losLock);

    if (_isConnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.stopLoggerClient();
        client.closeLoggingDatabase();
        theObserver.losState = eObserverStates::ObserverDisconnected;
    }
}

LOGGER_API_IMPL bool logObserverPauseLogging(bool doPause)
{
    Lock lock(theObserver.losLock);

    bool result{ _isInitialized(theObserver.losState) };
    if (_isConnected(theObserver.losState))
    {
        theObserver.losState = doPause ? eObserverStates::ObserverPaused : eObserverStates::ObserverConnected;
        LoggerClient::getInstance().setPaused(doPause);
    }

    return result;
}

LOGGER_API_IMPL eObserverStates logObserverCurrentState()
{
    Lock lock(theObserver.losLock);
    return theObserver.losState;
}

LOGGER_API_IMPL void logObserverRelease()
{
    Lock lock(theObserver.losLock);

    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.setCallbacks(nullptr);
        client.stopLoggerClient();
        Application::releaseApplication();
        _setCallbacks(theObserver.losEvents, nullptr);
        theObserver.losState = eObserverStates::ObserverUninitialized;
    }
}

LOGGER_API_IMPL bool logObserverIsInitialized()
{
    Lock lock(theObserver.losLock);
    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverIsConnected()
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConnectedState();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverIsStarted()
{
    Lock lock(theObserver.losLock);
    return _isStarted(theObserver.losState);
}

LOGGER_API_IMPL const char* logObserverLoggerAddress()
{
    Lock lock(theObserver.losLock);
    const char * result{ nullptr };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostAddress().getString();
    }

    return result;
}

LOGGER_API_IMPL unsigned short logObserverLoggerPort()
{
    Lock lock(theObserver.losLock);
    unsigned short result{ NESocket::InvalidPort };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostPort();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerEnabled()
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConfigLoggerConnectEnabled();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerAddress(char* addrBuffer, uint32_t space)
{
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        String addr{ client.getConfigLoggerAddress() };
        if ((addrBuffer != nullptr) && (addr.getLength() > static_cast<NEString::CharCount>(space)))
        {
            result = NEString::copyString<char, char>(addrBuffer, static_cast<NEString::CharCount>(space), addr.getString(), addr.getLength()) > 0;
        }
    }

    return result;
}

LOGGER_API_IMPL unsigned short logObserverConfigLoggerPort()
{
    Lock lock(theObserver.losLock);
    uint16_t result{ NESocket::InvalidPort };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getConfigLoggerPort();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestInstances()
{
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::getInstance().requestConnectedInstances();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestScopes(ITEM_ID target /* = ID_IGNORED */)
{
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::getInstance().requestScopes(target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestChangeScopePrio(ITEM_ID target, const sLogScope* scopes, uint32_t count)
{
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState) && (target != ID_IGNORE))
    {
        NETrace::ScopeNames scopeList(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            scopeList.add(NETrace::sScopeInfo(scopes[i].lsName, scopes[i].lsId, scopes[i].lsPrio));
        }

        result = LoggerClient::getInstance().requestChangeScopePrio( scopeList, target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestSaveConfig(ITEM_ID target /* = ID_IGNORED */)
{
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::getInstance().requestSaveConfiguration(target);
    }

    return result;
}
