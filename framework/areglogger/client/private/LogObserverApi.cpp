/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LogObserverApi.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Observer library API.
 *
 ************************************************************************/

#include "areglogger/client/LogObserverApi.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include "areglogger/client/private/LoggerClient.hpp"

#include <atomic>

// Use these options if compile for Windows with MSVC
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")

#if defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "sqlite3")
#else   // defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
    #pragma comment(lib, "aregsqlite3")
#endif  //defined(USE_SQLITE_PACKAGE) && (USE_SQLITE_PACKAGE != 0)
#endif // _MSC_VER


namespace
{
    struct LogObserverStruct
    {
        /// The mutex to protect the log observer state and counter
        Mutex           losLock     { false };
        /// The log observer initialized counter
        uint32_t        losCounter  { 0 };
        /// The log observer state
        ObserverState losState    { ObserverState::ObserverUninitialized };
        /// The log observer events
        ObserverEvents losEvents   { };
    };
    
    LogObserverStruct& logObserverData()
    {
        static LogObserverStruct _theObserver;
        return _theObserver;
    }

    void _setCallbacks(ObserverEvents & dstCallbacks, const ObserverEvents* srcCallbacks)
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

    inline bool _isInitialized(ObserverState state)
    {
        return (state != ObserverState::ObserverUninitialized);
    }

    inline bool _isDisconnected(ObserverState state)
    {
        return (state == ObserverState::ObserverDisconnected);
    }

    inline bool _isConnected(ObserverState state)
    {
        return (state >= ObserverState::ObserverConnected);
    }

    inline bool _is_started(ObserverState state)
    {
        return (state == ObserverState::ObserverConnected);
    }
}

LOGGER_API_IMPL bool logObserverInitialize(const ObserverEvents * callbacks, const char* configFilePath /* = nullptr */)
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    ++theObserver.losCounter;

    if (_isInitialized(theObserver.losState) == false)
    {
        LoggerClient& client = LoggerClient::instance();
        theObserver.losState = ObserverState::ObserverDisconnected;
        _setCallbacks(theObserver.losEvents, callbacks);
        client.setCallbacks(&theObserver.losEvents);
        Application::init_application(false, false, false, true, false, configFilePath, static_cast<ConfigListener *>(&client));
    }

    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverConnectLogger(const char* dbPath, const char* ipAddress /*= nullptr*/, uint16_t portNr /* = 0 */)
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    if (_isDisconnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        client.openLoggingDatabase(dbPath);
        if (client.startLoggerClient(ipAddress, portNr))
        {
            theObserver.losState = ObserverState::ObserverConnected;
        }
    }

    return _isConnected(theObserver.losState);
}

LOGGER_API_IMPL void logObserverDisconnectLogger()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    if (_isConnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        client.stopLoggerClient();
        client.closeLoggingDatabase();
        theObserver.losState = ObserverState::ObserverDisconnected;
    }
}

LOGGER_API_IMPL bool logObserverPauseLogging(bool doPause)
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    bool result{ _isInitialized(theObserver.losState) };
    if (_isConnected(theObserver.losState))
    {
        theObserver.losState = doPause ? ObserverState::ObserverPaused : ObserverState::ObserverConnected;
        LoggerClient::instance().setPaused(doPause);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverStopLogging(bool doStop, const char* dbPath /* = NULL*/)
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isConnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        if (doStop)
        {
            theObserver.losState = ObserverState::ObserverPaused;
            client.setPaused(true);
            client.closeLoggingDatabase();
            result = true;
        }
        else
        {
            if (client.openLoggingDatabase(dbPath))
            {
                theObserver.losState = ObserverState::ObserverConnected;
                client.setPaused(false);
                result = true;
            }
        }
    }

    return result;
}


LOGGER_API_IMPL ObserverState logObserverCurrentState()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return theObserver.losState;
}

LOGGER_API_IMPL void logObserverRelease()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    if (theObserver.losCounter != 0)
        --theObserver.losCounter;

    if ((theObserver.losCounter == 0) && _isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        client.setCallbacks(nullptr);
        client.stopLoggerClient();
        Application::release_application();
        _setCallbacks(theObserver.losEvents, nullptr);
        theObserver.losState = ObserverState::ObserverUninitialized;
    }
}

LOGGER_API_IMPL bool logObserverIsInitialized()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverIsConnected()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        result = client.is_connected_state();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverIsStarted()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return _is_started(theObserver.losState);
}

LOGGER_API_IMPL const char* logObserverLoggerAddress()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    const char * result{ nullptr };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        result = client.address().host_address().as_string();
    }

    return result;
}

LOGGER_API_IMPL uint16_t logObserverLoggerPort()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    uint16_t result{ NESocket::InvalidPort };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        result = client.address().host_port();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerEnabled()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        result = client.isConfigLoggerConnectEnabled();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverConfigLoggerAddress(char* addrBuffer, uint32_t space)
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        String addr{ client.getConfigLoggerAddress() };
        if ((addrBuffer != nullptr) && (addr.length() > static_cast<NEString::CharCount>(space)))
        {
            result = NEString::copy_string<char, char>(addrBuffer, static_cast<NEString::CharCount>(space), addr.as_string(), addr.length()) > 0;
        }
    }

    return result;
}

LOGGER_API_IMPL uint16_t logObserverConfigLoggerPort()
{
    LogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    uint16_t result{ NESocket::InvalidPort };
    if (_isInitialized(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::instance();
        result = client.getConfigLoggerPort();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestInstances()
{
    LogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::instance().requestConnectedInstances();
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestScopes(ITEM_ID target /* = ID_IGNORED */)
{
    LogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::instance().requestScopes(target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestChangeScopePrio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count)
{
    LogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState) && (target != ID_IGNORE))
    {
        NELogging::ScopeNames scopeList(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            scopeList.add(NELogging::ScopeEntry(scopes[i].lsName, scopes[i].lsId, scopes[i].lsPrio));
        }

        result = LoggerClient::instance().requestChangeScopePrio( scopeList, target);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverRequestSaveConfig(ITEM_ID target /* = ID_IGNORED */)
{
    LogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
    {
        result = LoggerClient::instance().requestSaveConfiguration(target);
    }

    return result;
}

LOGGER_API_IMPL int32_t logObserverGetActiveDatabasePath(char* dbPath, int32_t space)
{
    String path{ LoggerClient::instance().getActiveDatabasePath() };
    if ((dbPath != nullptr) && (path.length() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int32_t>(NEString::copy_string<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t logObserverGetInitialDatabasePath(char* dbPath, int32_t space)
{
    String path{ LoggerClient::instance().getInitialDatabasePath() };
    if ((dbPath != nullptr) && (path.length() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int32_t>(NEString::copy_string<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t logObserverGetConfigDatabasePath(char* dbPath, int32_t space)
{
    String path{ LoggerClient::instance().getConfigDatabasePath() };
    if ((dbPath != nullptr) && (path.length() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int32_t>(NEString::copy_string<char, char>(dbPath, static_cast<NEString::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t logObserverGetConfigDatabaseLocation(char* dbLocation, int32_t space)
{
    String location{ LoggerClient::instance().getConfigDatabaseLocation() };
    if ((dbLocation != nullptr) && (location.length() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int32_t>(NEString::copy_string<char, char>(dbLocation, static_cast<NEString::CharCount>(space), location.as_string(), location.length()));
    }
    else
    {
        return (location.is_empty() ? 0 : static_cast<int32_t>(location.length() + 1));
    }
}

LOGGER_API_IMPL bool logObserverSetConfigDatabaseLocation(const char* dbLocation)
{
    return LoggerClient::instance().setConfigDatabaseLocation(dbLocation);
}

LOGGER_API_IMPL int32_t logObserverGetConfigDatabaseName(char* dbName, int32_t space)
{
    String name{ LoggerClient::instance().getConfigDatabaseName() };
    if ((dbName != nullptr) && (name.length() > static_cast<NEString::CharCount>(space)))
    {
        return static_cast<int32_t>(NEString::copy_string<char, char>(dbName, static_cast<NEString::CharCount>(space), name.as_string(), name.length()));
    }
    else
    {
        return (name.is_empty() ? 0 : static_cast<int32_t>(name.length() + 1));
    }
}

LOGGER_API_IMPL bool logObserverSetConfigDatabaseName(const char* dbName)
{
    return LoggerClient::instance().setConfigDatabaseName(dbName);
}

LOGGER_API_IMPL bool logObserverConfigUpdate(const char* address, uint16_t port, const char* dbFilePath, bool makeSave)
{
    LoggerClient& logger = LoggerClient::instance();
    bool result{ false };
    if (logger.setConfigLoggerConnection(address, port) && logger.setConfigDatabasePath(dbFilePath, true))
    {
        result = makeSave ? logger.requestSaveConfiguration() : true;
    }

    return result;
}
