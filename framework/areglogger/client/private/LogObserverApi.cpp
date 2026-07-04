/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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


namespace {
    struct LogObserverStruct
    {
        areg::Mutex     losLock     { false };  //!< The mutex to protect the log observer state and counter
        uint32_t        losCounter  { 0 };      //!< The log observer initialized counter
        ObserverState   losState    { ObserverState::ObserverUninitialized };   //!< The log observer state
        ObserverEvents  losEvents   { };        //!< The log observer events
    };
    
    LogObserverStruct& log_observer_data()
    {
        static LogObserverStruct _theObserver;
        return _theObserver;
    }

    void _set_callbacks(ObserverEvents & dstCallbacks, const ObserverEvents* srcCallbacks)
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

    inline bool _is_initialized(ObserverState state)
    {
        return (state != ObserverState::ObserverUninitialized);
    }

    inline bool _is_disconnected(ObserverState state)
    {
        return (state == ObserverState::ObserverDisconnected);
    }

    inline bool _is_connected(ObserverState state)
    {
        return (state >= ObserverState::ObserverConnected);
    }

    inline bool _is_started(ObserverState state)
    {
        return (state == ObserverState::ObserverConnected);
    }
} // namespace

LOGGER_API_IMPL bool log_observer_initialize(const ObserverEvents * callbacks, const char* configFilePath /* = nullptr */)
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);

    ++theObserver.losCounter;

    if (_is_initialized(theObserver.losState) == false)
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        theObserver.losState = ObserverState::ObserverDisconnected;
        _set_callbacks(theObserver.losEvents, callbacks);
        client.set_callbacks(&theObserver.losEvents);
        areg::Application::setup(false, false, false, true, false, configFilePath, static_cast<areg::ConfigListener *>(&client));
    }

    return _is_initialized(theObserver.losState);
}

LOGGER_API_IMPL bool log_observer_connect_logger(const char* dbPath, const char* ipAddress /*= nullptr*/, uint16_t portNr /* = 0 */)
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);

    if (_is_disconnected(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        client.open_logging_database(dbPath);
        if (client.start_logger_client(ipAddress, portNr))
        {
            theObserver.losState = ObserverState::ObserverConnected;
        }
    }

    return _is_connected(theObserver.losState);
}

LOGGER_API_IMPL void log_observer_disconnect_logger()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);

    if (_is_connected(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        client.stop_logger_client();
        client.close_logging_database();
        theObserver.losState = ObserverState::ObserverDisconnected;
    }
}

LOGGER_API_IMPL bool log_observer_pause_logging(bool doPause)
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);

    bool result{ _is_initialized(theObserver.losState) };
    if (_is_connected(theObserver.losState))
    {
        theObserver.losState = doPause ? ObserverState::ObserverPaused : ObserverState::ObserverConnected;
        areg::logger::LoggerClient::instance().set_paused(doPause);
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_stop_logging(bool doStop, const char* dbPath /* = NULL*/)
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    bool result{ false };
    if (_is_connected(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        if (doStop)
        {
            theObserver.losState = ObserverState::ObserverPaused;
            client.set_paused(true);
            client.close_logging_database();
            result = true;
        }
        else
        {
            if (client.open_logging_database(dbPath))
            {
                theObserver.losState = ObserverState::ObserverConnected;
                client.set_paused(false);
                result = true;
            }
        }
    }

    return result;
}


LOGGER_API_IMPL ObserverState log_observer_current_state()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    return theObserver.losState;
}

LOGGER_API_IMPL void log_observer_release()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);

    if (theObserver.losCounter != 0)
        --theObserver.losCounter;

    if ((theObserver.losCounter == 0) && _is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        client.set_callbacks(nullptr);
        client.stop_logger_client();
        areg::Application::release();
        _set_callbacks(theObserver.losEvents, nullptr);
        theObserver.losState = ObserverState::ObserverUninitialized;
    }
}

LOGGER_API_IMPL bool log_observer_is_initialized()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    return _is_initialized(theObserver.losState);
}

LOGGER_API_IMPL bool log_observer_is_connected()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    bool result{ false };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        result = client.is_connected_state();
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_is_started()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    return _is_started(theObserver.losState);
}

LOGGER_API_IMPL const char* log_observer_logger_address()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    const char * result{ nullptr };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        result = client.address().host_address().as_string();
    }

    return result;
}

LOGGER_API_IMPL uint16_t log_observer_logger_port()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    uint16_t result{ areg::InvalidPort };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        result = client.address().host_port();
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_config_logger_enabled()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    bool result{ false };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        result = client.is_config_logger_connect_enabled();
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_config_logger_address(char* addrBuffer, uint32_t space)
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    bool result{ false };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        areg::String addr{ client.config_logger_address() };
        if ((addrBuffer != nullptr) && (addr.length() > static_cast<areg::CharCount>(space)))
        {
            result = areg::copy_string<char, char>(addrBuffer, static_cast<areg::CharCount>(space), addr.as_string(), addr.length()) > 0;
        }
    }

    return result;
}

LOGGER_API_IMPL uint16_t log_observer_config_logger_port()
{
    LogObserverStruct& theObserver { log_observer_data() };
    areg::Lock lock(theObserver.losLock);
    uint16_t result{ areg::InvalidPort };
    if (_is_initialized(theObserver.losState))
    {
        areg::logger::LoggerClient& client = areg::logger::LoggerClient::instance();
        result = client.config_logger_port();
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_request_instances()
{
    LogObserverStruct& theObserver { log_observer_data() };
    bool result{ false };
    areg::Lock lock(theObserver.losLock);
    if (_is_initialized(theObserver.losState))
    {
        result = areg::logger::LoggerClient::instance().request_connected_instances();
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_request_scopes(ITEM_ID target /* = ID_IGNORED */)
{
    LogObserverStruct& theObserver { log_observer_data() };
    bool result{ false };
    areg::Lock lock(theObserver.losLock);
    if (_is_initialized(theObserver.losState))
    {
        result = areg::logger::LoggerClient::instance().request_scopes(target);
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_request_change_scope_prio(ITEM_ID target, const ScopeInfo* scopes, uint32_t count)
{
    LogObserverStruct& theObserver { log_observer_data() };
    bool result{ false };
    areg::Lock lock(theObserver.losLock);
    if (_is_initialized(theObserver.losState) && (target != ID_IGNORE))
    {
        areg::ScopeNames scopeList(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            scopeList.add(areg::ScopeEntry(scopes[i].lsName, scopes[i].lsId, scopes[i].lsPrio));
        }

        result = areg::logger::LoggerClient::instance().request_change_scope_prio( scopeList, target);
    }

    return result;
}

LOGGER_API_IMPL bool log_observer_request_save_config(ITEM_ID target /* = ID_IGNORED */)
{
    LogObserverStruct& theObserver { log_observer_data() };
    bool result{ false };
    areg::Lock lock(theObserver.losLock);
    if (_is_initialized(theObserver.losState))
    {
        result = areg::logger::LoggerClient::instance().request_save_configuration(target);
    }

    return result;
}

LOGGER_API_IMPL int32_t log_observer_active_database_path(char* dbPath, int32_t space)
{
    areg::String path{ areg::logger::LoggerClient::instance().active_database_path() };
    if ((dbPath != nullptr) && (path.length() > static_cast<areg::CharCount>(space)))
    {
        return static_cast<int32_t>(areg::copy_string<char, char>(dbPath, static_cast<areg::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t log_observer_initial_database_path(char* dbPath, int32_t space)
{
    areg::String path{ areg::logger::LoggerClient::instance().initial_database_path() };
    if ((dbPath != nullptr) && (path.length() > static_cast<areg::CharCount>(space)))
    {
        return static_cast<int32_t>(areg::copy_string<char, char>(dbPath, static_cast<areg::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t log_observer_config_database_path(char* dbPath, int32_t space)
{
    areg::String path{ areg::logger::LoggerClient::instance().config_database_path() };
    if ((dbPath != nullptr) && (path.length() > static_cast<areg::CharCount>(space)))
    {
        return static_cast<int32_t>(areg::copy_string<char, char>(dbPath, static_cast<areg::CharCount>(space), path.as_string(), path.length()));
    }
    else
    {
        return (path.is_empty() ? 0 : static_cast<int32_t>(path.length() + 1));
    }
}

LOGGER_API_IMPL int32_t log_observer_config_database_location(char* dbLocation, int32_t space)
{
    areg::String location{ areg::logger::LoggerClient::instance().config_database_location() };
    if ((dbLocation != nullptr) && (location.length() > static_cast<areg::CharCount>(space)))
    {
        return static_cast<int32_t>(areg::copy_string<char, char>(dbLocation, static_cast<areg::CharCount>(space), location.as_string(), location.length()));
    }
    else
    {
        return (location.is_empty() ? 0 : static_cast<int32_t>(location.length() + 1));
    }
}

LOGGER_API_IMPL bool log_observer_set_config_database_location(const char* dbLocation)
{
    return areg::logger::LoggerClient::instance().set_config_database_location(dbLocation);
}

LOGGER_API_IMPL int32_t log_observer_config_database_name(char* dbName, int32_t space)
{
    areg::String name{ areg::logger::LoggerClient::instance().config_database_name() };
    if ((dbName != nullptr) && (name.length() > static_cast<areg::CharCount>(space)))
    {
        return static_cast<int32_t>(areg::copy_string<char, char>(dbName, static_cast<areg::CharCount>(space), name.as_string(), name.length()));
    }
    else
    {
        return (name.is_empty() ? 0 : static_cast<int32_t>(name.length() + 1));
    }
}

LOGGER_API_IMPL bool log_observer_set_config_database_name(const char* dbName)
{
    return areg::logger::LoggerClient::instance().set_config_database_name(dbName);
}

LOGGER_API_IMPL bool log_observer_config_update(const char* address, uint16_t port, const char* dbFilePath, bool makeSave)
{
    areg::logger::LoggerClient& logger = areg::logger::LoggerClient::instance();
    bool result{ false };
    if (logger.set_config_logger_connection(address, port) && logger.set_config_database_path(dbFilePath, true))
    {
        result = makeSave ? logger.request_save_configuration() : true;
    }

    return result;
}
