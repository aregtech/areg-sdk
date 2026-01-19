/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areglogger/client/private/LogObserverApi.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *
 ************************************************************************/

#include "areglogger/client/LogObserverApi.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SyncObjects.hpp"

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
    
    sLogObserverStruct& logObserverData(void)
    {
        static sLogObserverStruct _theObserver;
        return _theObserver;
    }

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
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    ++theObserver.losCounter;

    if (_isInitialized(theObserver.losState) == false)
    {
        LoggerClient& client = LoggerClient::getInstance();
        theObserver.losState = eObserverStates::ObserverDisconnected;
        _setCallbacks(theObserver.losEvents, callbacks);
        client.setCallbacks(&theObserver.losEvents);
        Application::initApplication(false, false, false, true, false, configFilePath, static_cast<IEConfigurationListener *>(&client));
    }

    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverConnectLogger(const char* dbPath, const char* ipAddress /*= nullptr*/, uint16_t portNr /* = 0 */)
{
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    bool result{ _isInitialized(theObserver.losState) };
    if (_isConnected(theObserver.losState))
    {
        theObserver.losState = doPause ? eObserverStates::ObserverPaused : eObserverStates::ObserverConnected;
        LoggerClient::getInstance().setPaused(doPause);
    }

    return result;
}

LOGGER_API_IMPL bool logObserverStopLogging(bool doStop, const char* dbPath /* = NULL*/)
{
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    bool result{ false };
    if (_isConnected(theObserver.losState))
    {
        LoggerClient& client = LoggerClient::getInstance();
        if (doStop)
        {
            theObserver.losState = eObserverStates::ObserverPaused;
            client.setPaused(true);
            client.closeLoggingDatabase();
            result = true;
        }
        else
        {
            if (client.openLoggingDatabase(dbPath))
            {
                theObserver.losState = eObserverStates::ObserverConnected;
                client.setPaused(false);
                result = true;
            }
        }
    }

    return result;
}


LOGGER_API_IMPL eObserverStates logObserverCurrentState()
{
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return theObserver.losState;
}

LOGGER_API_IMPL void logObserverRelease()
{
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);

    if (theObserver.losCounter != 0)
        --theObserver.losCounter;

    if ((theObserver.losCounter == 0) && _isInitialized(theObserver.losState))
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
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return _isInitialized(theObserver.losState);
}

LOGGER_API_IMPL bool logObserverIsConnected()
{
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
    Lock lock(theObserver.losLock);
    return _isStarted(theObserver.losState);
}

LOGGER_API_IMPL const char* logObserverLoggerAddress()
{
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
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
    sLogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState) && (target != ID_IGNORE))
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
    sLogObserverStruct& theObserver { logObserverData() };
    bool result{ false };
    Lock lock(theObserver.losLock);
    if (_isInitialized(theObserver.losState))
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
