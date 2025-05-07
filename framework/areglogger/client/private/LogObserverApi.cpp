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
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "aregextend.lib")

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "sqlite3")
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "aregsqlite3.lib")
#endif  //defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
#endif // WINDOWS


namespace
{
    struct sLogObserverStruct
    {
        /// The mutex to protect the log observer state and counter
        Mutex           losLock     { false };
        /// The log observer initialized counter
        uint32_t        losCounter  { 0 };
        /// The log observer state
        eObserverStates losState    { eObserverStates::ObserverUninitialized };
        /// The log observer events
        sObserverEvents losEvents   { };
    };

    sLogObserverStruct _theObserver;

    void _setCallbacks(sObserverEvents & dstCallbacks, const sObserverEvents* srcCallbacks)
    {
        if (srcCallbacks != nullptr)
        {
            dstCallbacks.evtObserverConfigured  = srcCallbacks->evtObserverConfigured;
            dstCallbacks.evtLogDbConfigured     = srcCallbacks->evtLogDbConfigured;
            dstCallbacks.evtServiceConnected    = srcCallbacks->evtServiceConnected;
            dstCallbacks.evtLoggingStarted      = srcCallbacks->evtLoggingStarted;
            dstCallbacks.evtLogDbCreated        = srcCallbacks->evtLogDbCreated;
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
            dstCallbacks.evtLogDbCreated        = nullptr;
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
    Lock lock(_theObserver.losLock);

    ++_theObserver.losCounter;

    if (_isInitialized(_theObserver.losState) == false)
    {
        LoggerClient& client = LoggerClient::getInstance();
        _theObserver.losState = eObserverStates::ObserverDisconnected;
        _setCallbacks(_theObserver.losEvents, callbacks);
        client.setCallbacks(&_theObserver.losEvents);
        Application::initApplication(true, false, false, true, false, configFilePath, static_cast<IEConfigurationListener *>(&client));
    }

    return _isInitialized(_theObserver.losState);
}

LOGGER_API_IMPL bool logObserverConnectLogger(const char* dbPath, const char* ipAddress /*= nullptr*/, uint16_t portNr /* = 0 */)
{
    Lock lock(_theObserver.losLock);

    if (_isDisconnected(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.openLoggingDatabase(dbPath);
        if (client.startLoggerClient(ipAddress, portNr))
        {
            _theObserver.losState = eObserverStates::ObserverConnected;
        }
    }

    return _isConnected(_theObserver.losState);
}

LOGGER_API_IMPL void logObserverDisconnectLogger()
{
    Lock lock(_theObserver.losLock);

    if (_isConnected(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.stopLoggerClient();
        client.closeLoggingDatabase();
        _theObserver.losState = eObserverStates::ObserverDisconnected;
    }
}

LOGGER_API_IMPL bool logObserverPauseLogging(bool doPause)
{
    Lock lock(_theObserver.losLock);

    bool result{ _isInitialized(_theObserver.losState) };
    if (_isConnected(_theObserver.losState))
    {
        _theObserver.losState = doPause ? eObserverStates::ObserverPaused : eObserverStates::ObserverConnected;
        LoggerClient::getInstance().setPaused(doPause);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverStopLogging(bool doStop, const char* dbPath /* = NULL*/)
{
    Lock lock(_theObserver.losLock);
    bool result{ false };
    if (_isConnected(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        if (doStop)
        {
            _theObserver.losState = eObserverStates::ObserverPaused;
            client.setPaused(true);
            client.closeLoggingDatabase();
            result = true;
        }
        else
        {
            if (client.openLoggingDatabase(dbPath))
            {
                _theObserver.losState = eObserverStates::ObserverConnected;
                client.setPaused(false);
                result = true;
            }
        }
    }

    return result;
}


LOGGER_API_IMPL eObserverStates logObserverCurrentState()
{
    Lock lock(_theObserver.losLock);
    return _theObserver.losState;
}

LOGGER_API_IMPL void logObserverRelease()
{
    Lock lock(_theObserver.losLock);

    if (_theObserver.losCounter != 0)
        --_theObserver.losCounter;

    if ((_theObserver.losCounter == 0) && _isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        client.setCallbacks(nullptr);
        client.stopLoggerClient();
        Application::releaseApplication();
        _setCallbacks(_theObserver.losEvents, nullptr);
        _theObserver.losState = eObserverStates::ObserverUninitialized;
    }
}

LOGGER_API_IMPL bool logObserverIsInitialized()
{
    Lock lock(_theObserver.losLock);
    return _isInitialized(_theObserver.losState);
}

LOGGER_API_IMPL bool logObserverIsConnected()
{
    Lock lock(_theObserver.losLock);
    bool result{ false };
    if (_isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConnectedState();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverIsStarted()
{
    Lock lock(_theObserver.losLock);
    return _isStarted(_theObserver.losState);
}

LOGGER_API_IMPL const char* logObserverLoggerAddress()
{
    Lock lock(_theObserver.losLock);
    const char * result{ nullptr };
    if (_isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostAddress().getString();
    }

    return result;
}

LOGGER_API_IMPL unsigned short logObserverLoggerPort()
{
    Lock lock(_theObserver.losLock);
    unsigned short result{ NESocket::InvalidPort };
    if (_isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getAddress().getHostPort();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerEnabled()
{
    Lock lock(_theObserver.losLock);
    bool result{ false };
    if (_isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.isConfigLoggerConnectEnabled();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerAddress(char* addrBuffer, uint32_t space)
{
    Lock lock(_theObserver.losLock);
    bool result{ false };
    if (_isInitialized(_theObserver.losState))
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
    Lock lock(_theObserver.losLock);
    uint16_t result{ NESocket::InvalidPort };
    if (_isInitialized(_theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        result = client.getConfigLoggerPort();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestInstances()
{
    bool result{ false };
    Lock lock(_theObserver.losLock);
    if (_isInitialized(_theObserver.losState))
    {
        result = LoggerClient::getInstance().requestConnectedInstances();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestScopes(ITEM_ID target /* = ID_IGNORED */)
{
    bool result{ false };
    Lock lock(_theObserver.losLock);
    if (_isInitialized(_theObserver.losState))
    {
        result = LoggerClient::getInstance().requestScopes(target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestChangeScopePrio(ITEM_ID target, const sLogScope* scopes, uint32_t count)
{
    bool result{ false };
    Lock lock(_theObserver.losLock);
    if (_isInitialized(_theObserver.losState) && (target != ID_IGNORE))
    {
        NELogging::ScopeNames scopeList(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            scopeList.add(NELogging::sScopeInfo(scopes[i].lsName, scopes[i].lsId, scopes[i].lsPrio));
        }

        result = LoggerClient::getInstance().requestChangeScopePrio( scopeList, target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestSaveConfig(ITEM_ID target /* = ID_IGNORED */)
{
    bool result{ false };
    Lock lock(_theObserver.losLock);
    if (_isInitialized(_theObserver.losState))
    {
        result = LoggerClient::getInstance().requestSaveConfiguration(target);
    }

    return result;
}

LOGGER_API_IMPL int logObserverGetActiveDatabasePath(char* dbPath, int space)
{
    String path{ LoggerClient::getInstance().getActiveDatabasePath() };
    if ((dbPath != nullptr) && (path.getLength() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int>(NEString::copyString<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.getString(), path.getLength()));
    }
    else
    {
        return (path.isEmpty() ? 0 : static_cast<int>(path.getLength() + 1));
    }
}

LOGGER_API_IMPL int logObserverGetInitialDatabasePath(char* dbPath, int space)
{
    String path{ LoggerClient::getInstance().getInitialDatabasePath() };
    if ((dbPath != nullptr) && (path.getLength() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int>(NEString::copyString<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.getString(), path.getLength()));
    }
    else
    {
        return (path.isEmpty() ? 0 : static_cast<int>(path.getLength() + 1));
    }
}

LOGGER_API_IMPL int logObserverGetConfigDatabasePath(char* dbPath, int space)
{
    String path{ LoggerClient::getInstance().getConfigDatabasePath() };
    if ((dbPath != nullptr) && (path.getLength() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int>(NEString::copyString<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.getString(), path.getLength()));
    }
    else
    {
        return (path.isEmpty() ? 0 : static_cast<int>(path.getLength() + 1));
    }
}

LOGGER_API_IMPL int logObserverGetConfigDatabaseLocation(char* dbLocation, int space)
{
    String location{ LoggerClient::getInstance().getConfigDatabaseLocation() };
    if ((dbLocation != nullptr) && (location.getLength() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int>(NEString::copyString<char, char>(dbLocation, static_cast<NEString::CharCount>(space), location.getString(), location.getLength()));
    }
    else
    {
        return (location.isEmpty() ? 0 : static_cast<int>(location.getLength() + 1));
    }
}

LOGGER_API_IMPL bool logObserverSetConfigDatabaseLocation(const char* dbLocation)
{
    return LoggerClient::getInstance().setConfigDatabaseLocation(dbLocation);
}

LOGGER_API_IMPL int logObserverGetConfigDatabaseName(char* dbName, int space)
{
    String name{ LoggerClient::getInstance().getConfigDatabaseName() };
    if ((dbName != nullptr) && (name.getLength() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int>(NEString::copyString<char, char>(dbName, static_cast<NEString::CharCount>(space), name.getString(), name.getLength()));
    }
    else
    {
        return (name.isEmpty() ? 0 : static_cast<int>(name.getLength() + 1));
    }
}

LOGGER_API_IMPL bool logObserverSetConfigDatabaseName(const char* dbName)
{
    return LoggerClient::getInstance().setConfigDatabaseName(dbName);
}

LOGGER_API_IMPL bool logObserverConfigUpdate(const char* address, uint16_t port, const char* dbFilePath, bool makeSave)
{
    LoggerClient& logger = LoggerClient::getInstance();
    bool result{ false };
    if (logger.setConfigLoggerConnection(address, port) && logger.setConfigDatabasePath(dbFilePath, true))
    {
        result = makeSave ? logger.requestSaveConfiguration() : true;
    }

    return result;
}
