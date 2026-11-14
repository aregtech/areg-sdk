/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 *
 * \file        units/LogObserverApiTest.cpp
 * \brief       Areg Platform, tests of the log observer library: the C API that fills a buffer of
 *              the caller, and the messages of the log collector read by the observer.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/persist/ConfigListener.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/private/ObserverMessageProcessor.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <thread>
#include <vector>

#if defined(_WIN32)
    #include <WinSock2.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

#if AREG_LOGGING

namespace
{
    namespace fs = std::filesystem;
    using Getter = int32_t (*)(char *, int32_t);

    constexpr char      DB_NAME[]   { "observer.sqlog" };
    constexpr char      ADDRESS[]   { "127.0.0.1" };
    constexpr char      UNTOUCHED   { 'x' };
    constexpr ITEM_ID   FIRST       { 1001u };
    constexpr ITEM_ID   SECOND      { 1002u };
    constexpr ITEM_ID   THIRD       { 1003u };
    constexpr ITEM_ID   UNKNOWN_A   { 2001u };
    constexpr ITEM_ID   UNKNOWN_B   { 2002u };
    constexpr uint32_t  HUGE_NUMBER { 0xFFFF'FFFFu };

    //!< What the observer callbacks received.
    struct Seen
    {
        std::vector<LogInstance>    connected;      //!< Every connected instance.
        std::vector<ITEM_ID>        disconnected;   //!< Every disconnected cookie.
        std::vector<ScopeInfo>      scopes;         //!< Every registered scope.
        uint32_t                    scopeCalls{ 0u };   //!< Calls of the scope callback.
        uint32_t                    configured{ 0u };   //!< Calls of the observer configuration callback.
        std::string                 address;            //!< The address of the last configuration.
        uint16_t                    port{ 0u };         //!< The port of the last configuration.
        uint32_t                    dbConfigured{ 0u }; //!< Calls of the database configuration callback.
        std::string                 dbName;             //!< The database name of the last configuration.
        std::vector<std::string>    texts;              //!< The text of every log message.
        uint32_t                    rawLogs{ 0u };      //!< Calls of the raw log message callback.
        std::vector<ITEM_ID>        states;             //!< The cookie and the state of every source state change.
        std::vector<ITEM_ID>        restored;           //!< Every cookie of a restored configuration.
        uint32_t                    updateCalls{ 0u };  //!< Calls of the scope update callback.
        std::vector<std::string>    updated;            //!< Every updated scope.
        uint32_t                    failed{ 0u };       //!< Calls of the messaging failed callback.
    };

    Seen    _seen;

    void on_connected(const LogInstance * instances, uint32_t count)
    {
        _seen.connected.insert(_seen.connected.end(), instances, instances + count);
    }

    void on_disconnected(const ITEM_ID * instances, uint32_t count)
    {
        for (uint32_t i = 0u; i < count; ++i)
        {
            _seen.disconnected.push_back(instances[i]);
        }
    }

    void on_scopes(ITEM_ID /*cookie*/, const ScopeInfo * scopes, uint32_t count)
    {
        ++_seen.scopeCalls;
        for (uint32_t i = 0u; i < count; ++i)
        {
            _seen.scopes.push_back(scopes[i]);
        }
    }

    void on_configured(bool /*isEnabled*/, const char * address, uint16_t port)
    {
        ++_seen.configured;
        _seen.address = address != nullptr ? address : "";
        _seen.port = port;
    }

    void on_db_configured(bool /*isEnabled*/, const char * dbName, const char * /*dbLocation*/, const char * /*dbUser*/)
    {
        ++_seen.dbConfigured;
        _seen.dbName = dbName != nullptr ? dbName : "";
    }

    void on_log(const LogRecord * record)
    {
        _seen.texts.push_back(record->msgLogText);
    }

    void on_raw_log(const unsigned char * buffer, uint32_t size)
    {
        AREG_CHILD_CHECK((buffer != nullptr) && (size >= areg::log_entry_head()));
        ++_seen.rawLogs;
    }

    void on_source_state(ITEM_ID cookie, unsigned char state, ITEM_ID byObserver)
    {
        _seen.states.insert(_seen.states.end(), { cookie, static_cast<ITEM_ID>(state), byObserver });
    }

    void on_restored(ITEM_ID cookie)
    {
        _seen.restored.push_back(cookie);
    }

    void on_updated(ITEM_ID /*cookie*/, const ScopeInfo * scopes, uint32_t count)
    {
        ++_seen.updateCalls;
        for (uint32_t i = 0u; i < count; ++i)
        {
            _seen.updated.push_back(scopes[i].lsName);
        }
    }

    void on_failed()
    {
        ++_seen.failed;
    }

    ObserverEvents make_events()
    {
        ObserverEvents events{ };
        events.evtInstConnected     = &on_connected;
        events.evtInstDisconnected  = &on_disconnected;
        events.evtLogRegisterScopes = &on_scopes;
        events.evtObserverConfigured = &on_configured;
        events.evtLogDbConfigured   = &on_db_configured;
        return events;
    }

    //!< Returns the events with every callback the message processor reaches.
    ObserverEvents make_all_events()
    {
        ObserverEvents events{ make_events() };
        events.evtLogMessage        = &on_log;
        events.evtLogSourceState    = &on_source_state;
        events.evtLogConfigRestored = &on_restored;
        events.evtLogUpdatedScopes  = &on_updated;
        return events;
    }

    //!< Holds a private directory with the configuration of the observer, and the initialized observer.
    class ConfiguredObserver
    {
    public:
        //!< Writes the configuration with the database engine and the log collector connection enabled or not.
        //!< With 'setupFirst', the application runs its own setup() with the same file before the observer.
        explicit ConfiguredObserver(const char * engine = "sqlite3", bool collectorEnabled = true, uint16_t port = 8282u, bool setupFirst = false)
            : mDir(fs::temp_directory_path() / ("areg-log-observer-api-" + std::to_string(areg::Process::instance().id())))
            , mSetup(setupFirst)
        {
            std::error_code error;
            fs::create_directories(mDir, error);
            const fs::path config{ mDir / "observer.init" };
            std::ofstream file(config, std::ios::trunc);
            file << "log::*::target = db\n"
                 << "log::*::enable = true\n"
                 << "log::*::enable::db = true\n"
                 << "log::*::db::engine = " << engine << "\n"
                 << "log::*::db::name = " << DB_NAME << "\n"
                 << "log::*::db::location = " << mDir.generic_string() << "\n"
                 << "logger::*::service = logcollector\n"
                 << "logger::*::connect = tcpip\n"
                 << "logger::*::enable::tcpip = " << (collectorEnabled ? "true" : "false") << "\n"
                 << "logger::*::address::tcpip = " << ADDRESS << "\n"
                 << "logger::*::port::tcpip = " << port << "\n";
            file.close();
            AREG_CHILD_CHECK(file.good());

            if (mSetup)
            {
                areg::Application::setup(false, true, false, true, false, config.generic_string().c_str());
                AREG_CHILD_CHECK(areg::Application::is_configured());
            }

            ObserverEvents events{ make_events() };
            AREG_CHILD_CHECK(log_observer_initialize(&events, config.generic_string().c_str()));
        }

        ~ConfiguredObserver()
        {
            log_observer_release();
            if (mSetup)
                areg::Application::release();
            std::error_code error;
            fs::remove_all(mDir, error);
        }

        //!< Returns the path of a file in the private directory.
        std::string path_of(const char * name) const
        {
            return (mDir / name).generic_string();
        }

    private:
        const fs::path  mDir;   //!< The private directory.
        const bool      mSetup; //!< True if the application ran its own setup() first.
    };

    //!< Checks every way to call a getter of the text.
    void check_getter(Getter getter, const std::string & text)
    {
        const int32_t length{ static_cast<int32_t>(text.length()) };
        AREG_CHILD_CHECK(length != 0);
        AREG_CHILD_CHECK(getter(nullptr, 0) == length + 1);
        AREG_CHILD_CHECK(getter(nullptr, 4096) == length + 1);

        std::vector<char> large(static_cast<std::size_t>(length) + 100u, UNTOUCHED);
        AREG_CHILD_CHECK(getter(large.data(), static_cast<int32_t>(large.size())) == length);
        AREG_CHILD_CHECK(text == large.data());

        std::vector<char> exact(static_cast<std::size_t>(length) + 1u, UNTOUCHED);
        AREG_CHILD_CHECK(getter(exact.data(), length + 1) == length);
        AREG_CHILD_CHECK(text == exact.data());

        const std::vector<char> untouched(static_cast<std::size_t>(length), UNTOUCHED);
        std::vector<char> small{ untouched };
        AREG_CHILD_CHECK(getter(small.data(), length) == length + 1);
        AREG_CHILD_CHECK(getter(small.data(), 1) == length + 1);
        AREG_CHILD_CHECK(getter(small.data(), 0) == length + 1);
        AREG_CHILD_CHECK(getter(small.data(), -1) == length + 1);
        AREG_CHILD_CHECK(small == untouched);
    }

    //!< Returns a notification of the connected instances with the cookies.
    areg::MessageEnvelope connected_message(const std::vector<areg::ConnectedInstance> & instances)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 256u) != nullptr);
        message << areg::RemoteConnectionState::Connected;
        message << static_cast<uint32_t>(instances.size());
        for (const areg::ConnectedInstance & instance : instances)
        {
            message << instance;
        }

        message.move_to_begin();
        return message;
    }

    //!< Returns a notification of the disconnected instances with the cookies.
    areg::MessageEnvelope disconnected_message(const std::vector<ITEM_ID> & cookies)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 256u) != nullptr);
        message << areg::RemoteConnectionState::Disconnected;
        message << static_cast<uint32_t>(cookies.size());
        for (const ITEM_ID & cookie : cookies)
        {
            message << cookie;
        }

        message.move_to_begin();
        return message;
    }

    //!< Returns a scope registration that says it holds 'count' scopes and holds the listed names.
    areg::MessageEnvelope scopes_message(uint32_t count, const std::vector<std::string> & names)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 1024u) != nullptr);
        message << count;
        uint32_t scopeId{ 1u };
        for (const std::string & name : names)
        {
            message << scopeId++;
            message << static_cast<uint32_t>(PrioDebug);
            message << areg::String(name.c_str());
        }

        message.move_to_begin();
        return message;
    }

    areg::ConnectedInstance make_instance(ITEM_ID cookie, const std::string & name = "source", const std::string & location = "/bin/source")
    {
        areg::ConnectedInstance instance;
        instance.ciSource   = areg::MessageSource::SourceClient;
        instance.ciBitness  = areg::InstanceBitness::Bitness64;
        instance.ciCookie   = cookie;
        instance.ciTimestamp= 1u;
        instance.ciInstance = name;
        instance.ciLocation = location;
        return instance;
    }

    //!< Sets the callbacks of the singleton client and announces three connected sources.
    void connect_three_sources(areg::logger::ObserverMessageProcessor & processor, const ObserverEvents & events)
    {
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        processor.notify_connected_clients(connected_message({ make_instance(FIRST), make_instance(SECOND), make_instance(THIRD) }));
        AREG_CHILD_CHECK(_seen.connected.size() == 3u);
    }

    //!< Returns the notification of the collector about the state of the connection of the observer.
    areg::MessageEnvelope connection_message(ITEM_ID cookie, areg::ServiceConnectionState state)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 64u) != nullptr);
        message << cookie;
        message << state;
        message.move_to_begin();
        return message;
    }

    //!< Returns the notification that the source changed its state on request of the observer.
    areg::MessageEnvelope source_state_message(ITEM_ID source, uint8_t state, ITEM_ID byObserver)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 64u) != nullptr);
        message << source;
        message << state;
        message << byObserver;
        message.move_to_begin();
        return message;
    }

    //!< Returns the message with no data, sent by the source.
    areg::MessageEnvelope empty_message(ITEM_ID source)
    {
        static constexpr areg::EventHeader header{ areg::message_notify_instances() };
        areg::MessageEnvelope message;
        AREG_CHILD_CHECK(message.init_envelope(header, 64u) != nullptr);
        message.set_source(static_cast<uint32_t>(source));
        message.move_to_begin();
        return message;
    }

    using StateText = std::pair<areg::ServiceConnectionState, std::string>;

    //!< Returns every state of the connection with the text the observer logs for it.
    const std::vector<StateText> & connection_texts()
    {
        static const std::vector<StateText> _texts
        {
              { areg::ServiceConnectionState::Connected     , "Log observer connected to log collector service." }
            , { areg::ServiceConnectionState::Pending       , "The connection to the log collector service is pending." }
            , { areg::ServiceConnectionState::ConnectionLost, "The connection to the log collector service is lost." }
            , { areg::ServiceConnectionState::Disconnected  , "Log observer disconnected from log collector service." }
            , { areg::ServiceConnectionState::Failed        , "Failed to connect to the log collector service." }
            , { areg::ServiceConnectionState::Rejected      , "The connection to the log collector service is rejected." }
            , { areg::ServiceConnectionState::Shutdown      , "The log collector service is shutting down." }
            , { areg::ServiceConnectionState::Unknown       , "Undefined log collector service connection event..." }
        };

        return _texts;
    }

    //!< Returns the remote log message of the source with the text.
    areg::MessageEnvelope log_message(ITEM_ID source, const char * text)
    {
        areg::LogEntry entry{ };
        entry.logMsgType     = areg::LogMessageType::MessageText;
        entry.logMessagePrio = areg::LogPriority::PrioDebug;
        entry.logSource      = source;
        entry.logCookie      = source;
        entry.logMessageLen  = static_cast<uint32_t>(std::strlen(text));
        areg::String::format_string(entry.logMessage, static_cast<int32_t>(areg::LOG_MSG_SIZE), "%s", text);
        return areg::create_log_message(entry, areg::LogDataType::Remote, source);
    }
}

/**
 * Reproduces the defect: the five path and name getters read the size of the buffer the wrong way round.
 * A buffer that holds the text stayed empty, and a smaller one received a part of it.
 **/
TEST(LogObserverApiTest, initialize_reports_the_configuration_once)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        AREG_CHILD_CHECK(_seen.configured == 1u);
        AREG_CHILD_CHECK((_seen.address == ADDRESS) && (_seen.port == 8282u));
        AREG_CHILD_CHECK((_seen.dbConfigured == 1u) && (_seen.dbName == DB_NAME));
    );
}

// Reproduces the defect: an observer initialized after the application's own setup() never reported the configuration.
TEST(LogObserverApiTest, initialize_after_application_setup_still_reports_the_configuration_once)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer("sqlite3", true, 8282u, true);
        AREG_CHILD_CHECK(_seen.configured == 1u);
        AREG_CHILD_CHECK((_seen.address == ADDRESS) && (_seen.port == 8282u));
        AREG_CHILD_CHECK((_seen.dbConfigured == 1u) && (_seen.dbName == DB_NAME));
    );
}

TEST(LogObserverApiTest, path_getters_fill_only_a_buffer_that_holds_the_text)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        const std::string dbPath{ observer.path_of("session.sqlog") };
        AREG_CHILD_CHECK(areg::logger::LoggerClient::instance().open_logging_database(dbPath.c_str()));

        const areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        AREG_CHILD_CHECK(client.config_database_name() == DB_NAME);
        AREG_CHILD_CHECK(client.config_database_path().find_first(DB_NAME) > 0);
        AREG_CHILD_CHECK(client.active_database_path().find_first("session.sqlog") > 0);
        AREG_CHILD_CHECK(client.initial_database_path().find_first("session.sqlog") > 0);

        check_getter(&log_observer_config_database_name, client.config_database_name().as_string());
        check_getter(&log_observer_config_database_path, client.config_database_path().as_string());
        check_getter(&log_observer_config_database_location, client.config_database_location().as_string());
        check_getter(&log_observer_active_database_path, client.active_database_path().as_string());
        check_getter(&log_observer_initial_database_path, client.initial_database_path().as_string());

        areg::logger::LoggerClient::instance().close_logging_database();
    );
}

/**
 * Reproduces the defect: with nothing to copy the call returned 0 and left the buffer of the caller as it was.
 **/
TEST(LogObserverApiTest, path_getter_with_an_empty_text_terminates_the_buffer)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        char buffer[8];
        std::memset(buffer, UNTOUCHED, sizeof(buffer));
        AREG_CHILD_CHECK(log_observer_active_database_path(buffer, static_cast<int32_t>(sizeof(buffer))) == 0);
        AREG_CHILD_CHECK(buffer[0] == '\0');
        AREG_CHILD_CHECK(buffer[1] == UNTOUCHED);
        AREG_CHILD_CHECK(log_observer_active_database_path(nullptr, 0) == 0);
    );
}

/**
 * Reproduces the defect: the address getter answered true only for a buffer smaller than the address,
 * and wrote a cut address there; a buffer of the documented size stayed empty and the call failed.
 **/
TEST(LogObserverApiTest, address_getter_fills_only_a_buffer_that_holds_the_address)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        const uint32_t size{ static_cast<uint32_t>(sizeof(ADDRESS)) };

        char large[16]{ };
        AREG_CHILD_CHECK(log_observer_config_logger_address(large, static_cast<uint32_t>(sizeof(large))));
        AREG_CHILD_CHECK(std::strcmp(large, ADDRESS) == 0);

        char exact[sizeof(ADDRESS)];
        std::memset(exact, UNTOUCHED, sizeof(exact));
        AREG_CHILD_CHECK(log_observer_config_logger_address(exact, size));
        AREG_CHILD_CHECK(std::strcmp(exact, ADDRESS) == 0);

        char limit[16]{ };
        AREG_CHILD_CHECK(log_observer_config_logger_address(limit, HUGE_NUMBER));
        AREG_CHILD_CHECK(std::strcmp(limit, ADDRESS) == 0);

        char small[sizeof(ADDRESS)];
        std::memset(small, UNTOUCHED, sizeof(small));
        AREG_CHILD_CHECK(log_observer_config_logger_address(small, size - 1u) == false);
        AREG_CHILD_CHECK(log_observer_config_logger_address(small, 0u) == false);
        AREG_CHILD_CHECK(small[0] == UNTOUCHED);
        AREG_CHILD_CHECK(log_observer_config_logger_address(nullptr, size) == false);
    );
}

/**
 * Reproduces the defect: the list of the callback was filled at the index of the message and counted by
 * hits, so a cookie the observer does not know left an uninitialized entry and cut off a known one.
 **/
TEST(LogObserverApiTest, disconnect_reports_only_the_known_cookies)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());
        connect_three_sources(processor, events);

        processor.notify_connected_clients(disconnected_message({ UNKNOWN_A, SECOND, UNKNOWN_B, THIRD }));
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ SECOND, THIRD }));

        processor.notify_connected_clients(disconnected_message({ FIRST }));
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ SECOND, THIRD, FIRST }));
    );
}

namespace
{
    //!< True while the next disconnect callback also delivers the receive thread's disconnect of every source.
    bool _receiveOnReport{ false };

    //!< Records the disconnected instances; the first call delivers the disconnect of every source as the receive thread does.
    void on_disconnected_while_receiving(const ITEM_ID * instances, uint32_t count)
    {
        on_disconnected(instances, count);
        if (_receiveOnReport)
        {
            _receiveOnReport = false;
            areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());
            processor.notify_connected_clients(disconnected_message({ FIRST, SECOND, THIRD }));
        }
    }

    //!< A loopback socket that listens and never answers; the observer connects and waits for the handshake.
    class SilentCollector
    {
    public:
        SilentCollector()
            : mInitialized(areg::socket_initialize())
            , mSocket(areg::socket_create())
            , mPort(0u)
        {
            AREG_CHILD_CHECK(mInitialized);
            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
#if defined(_WIN32)
            int size{ sizeof(address) };
#else
            socklen_t size{ sizeof(address) };
#endif
            AREG_CHILD_CHECK(areg::is_valid_socket(mSocket));
            AREG_CHILD_CHECK(::bind(mSocket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == 0);
            AREG_CHILD_CHECK(::listen(mSocket, 4) == 0);
            AREG_CHILD_CHECK(::getsockname(mSocket, reinterpret_cast<sockaddr *>(&address), &size) == 0);
            mPort = ntohs(address.sin_port);
        }

        ~SilentCollector()
        {
            areg::socket_close(mSocket);
            areg::socket_release();
        }

        uint16_t port() const
        {
            return mPort;
        }

    private:
        bool            mInitialized;   //!< True if the socket library is initialized.
        SOCKETHANDLE    mSocket;        //!< The listening socket.
        uint16_t        mPort;          //!< The port of the listening socket.
    };
}

/**
 * Reproduces the defect: the stop of the observer reported the instances from the map that the receive
 * thread changes, without the lock. Each instance known at the stop is reported once, and the collector's
 * disconnect of the same instances, delivered meanwhile, finds the map empty.
 **/
TEST(LogObserverApiTest, stop_reports_each_live_instance_once_and_leaves_the_map_empty_before_the_callbacks)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        SilentCollector collector;
        AREG_CHILD_CHECK(log_observer_connect_logger(observer.path_of("stop.sqlog").c_str(), ADDRESS, collector.port()));
        ObserverEvents events{ make_events() };
        events.evtInstDisconnected = &on_disconnected_while_receiving;
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());
        connect_three_sources(processor, events);

        _receiveOnReport = true;
        log_observer_disconnect_logger();
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ FIRST, SECOND, THIRD }));
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverDisconnected);

        processor.notify_connected_clients(disconnected_message({ FIRST, SECOND, THIRD }));
        AREG_CHILD_CHECK(_seen.disconnected.size() == 3u);
        areg::logger::LoggerClient::instance().set_callbacks(nullptr);
    );
}

/**
 * A cookie listed twice is reported once, and a list with no known cookie reports nothing.
 **/
TEST(LogObserverApiTest, disconnect_ignores_a_repeated_or_unknown_cookie)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());
        connect_three_sources(processor, events);

        processor.notify_connected_clients(disconnected_message({ FIRST, FIRST, SECOND }));
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ FIRST, SECOND }));

        processor.notify_connected_clients(disconnected_message({ UNKNOWN_A, UNKNOWN_B }));
        processor.notify_connected_clients(disconnected_message({ }));
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ FIRST, SECOND }));
    );
}

/**
 * Reproduces the defect: a name too long for the field of LogInstance was copied without its
 * null-terminated character, so the caller read past the field.
 **/
TEST(LogObserverApiTest, connected_instance_names_are_always_terminated)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        const std::string name(LENGTH_NAME + 10u, 'n');
        const std::string location(LENGTH_LOCATION + 10u, 'l');
        processor.notify_connected_clients(connected_message({ make_instance(FIRST, name, location) }));

        AREG_CHILD_CHECK(_seen.connected.size() == 1u);
        const LogInstance & instance{ _seen.connected[0] };
        AREG_CHILD_CHECK(instance.liCookie == FIRST);
        AREG_CHILD_CHECK(instance.liName[LENGTH_NAME - 1] == '\0');
        AREG_CHILD_CHECK(instance.liLocation[LENGTH_LOCATION - 1] == '\0');
        AREG_CHILD_CHECK(name.compare(0u, LENGTH_NAME - 1u, instance.liName) == 0);
        AREG_CHILD_CHECK(location.compare(0u, LENGTH_LOCATION - 1u, instance.liLocation) == 0);
    );
}

/**
 * Reproduces the defect: the receive thread read the pause flag without synchronization while
 * another thread changed it. A paused observer drops the log and a resumed one delivers it.
 **/
TEST(LogObserverApiTest, pause_changes_while_log_messages_arrive)
{
    AREG_EXPECT_IN_CHILD(
        constexpr uint32_t ROUNDS{ 500u };
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        client.set_callbacks(&events);
        areg::SocketClient socket(areg::String(ADDRESS), 8383u);
        AREG_CHILD_CHECK(socket.create_fd());
        // Converts to the inaccessible base class that the receive thread calls.
        areg::RemoteMessageHandler & handler{ (areg::RemoteMessageHandler &)client };
        areg::MessageEnvelope entry{ log_message(FIRST, "text") };

        std::thread toggle([&client]() {
            for (uint32_t i = 0u; i < ROUNDS; ++i)
            {
                client.set_paused((i % 2u) == 0u);
            }
        });

        for (uint32_t i = 0u; i < ROUNDS; ++i)
        {
            handler.process_received_message(entry, socket);
        }

        toggle.join();
        client.set_paused(true);
        const std::size_t delivered{ _seen.texts.size() };
        handler.process_received_message(entry, socket);
        AREG_CHILD_CHECK(_seen.texts.size() == delivered);

        client.set_paused(false);
        handler.process_received_message(entry, socket);
        AREG_CHILD_CHECK(_seen.texts.size() == delivered + 1u);
        client.set_callbacks(nullptr);
    );
}

/**
 * Reproduces the defect: a scope too long for the field of ScopeInfo was copied without its
 * null-terminated character, so the caller read past the field.
 **/
TEST(LogObserverApiTest, scope_names_are_always_terminated)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        const std::string name(LENGTH_SCOPE + 10u, 's');
        processor.notify_log_register_scopes(scopes_message(1u, { name }));

        AREG_CHILD_CHECK(_seen.scopes.size() == 1u);
        AREG_CHILD_CHECK(_seen.scopes[0].lsName[LENGTH_SCOPE - 1] == '\0');
        AREG_CHILD_CHECK(name.compare(0u, LENGTH_SCOPE - 1u, _seen.scopes[0].lsName) == 0);
    );
}

/**
 * Reproduces the defect: the observer allocated the number of scopes the message promised, so one
 * small message with a forged number ended the process with an uncaught exception.
 **/
TEST(LogObserverApiTest, forged_scope_count_reports_only_the_scopes_of_the_message)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_register_scopes(scopes_message(HUGE_NUMBER, { "scope_one", "scope_two" }));

        AREG_CHILD_CHECK(_seen.scopeCalls == 1u);
        AREG_CHILD_CHECK(_seen.scopes.size() == 2u);
        AREG_CHILD_CHECK(std::strcmp(_seen.scopes[0].lsName, "scope_one") == 0);
        AREG_CHILD_CHECK(std::strcmp(_seen.scopes[1].lsName, "scope_two") == 0);
        AREG_CHILD_CHECK(_seen.scopes[1].lsId == 2u);
        AREG_CHILD_CHECK(_seen.scopes[1].lsPrio == static_cast<uint32_t>(PrioDebug));
    );
}

/**
 * A count that is honest and a message with no scope are both read as they are.
 **/
TEST(LogObserverApiTest, scope_registration_keeps_an_honest_list)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_register_scopes(scopes_message(0u, { }));
        AREG_CHILD_CHECK(_seen.scopeCalls == 1u);
        AREG_CHILD_CHECK(_seen.scopes.empty());

        processor.notify_log_register_scopes(scopes_message(3u, { "a", "b", "c" }));
        AREG_CHILD_CHECK(_seen.scopeCalls == 2u);
        AREG_CHILD_CHECK(_seen.scopes.size() == 3u);
    );
}

/**
 * Every state of the connection that the collector reports reaches the observer as a log message with its own text.
 **/
TEST(LogObserverApiTest, connection_states_reach_the_observer_as_log_texts)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        const std::vector<StateText> & expected{ connection_texts() };

        for (const auto & entry : expected)
        {
            processor.notify_service_connection(connection_message(FIRST, entry.first));
        }

        AREG_CHILD_CHECK(_seen.texts.size() == expected.size());
        for (uint32_t i = 0u; i < static_cast<uint32_t>(expected.size()); ++i)
        {
            AREG_CHILD_CHECK(_seen.texts[i] == expected[i].second);
        }
    );
}

/**
 * Without callbacks the observer still tracks the sources: a source connected then is reported when it disconnects later.
 **/
TEST(LogObserverApiTest, without_callbacks_the_sources_are_still_tracked)
{
    AREG_EXPECT_IN_CHILD(
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        client.set_callbacks(nullptr);
        areg::logger::ObserverMessageProcessor processor(client);

        processor.notify_connected_clients(connected_message({ make_instance(FIRST), make_instance(SECOND) }));
        processor.notify_log_register_scopes(scopes_message(1u, { "scope_one" }));
        processor.notify_log_update_scopes(scopes_message(1u, { "scope_one" }));
        processor.notify_log_source_state(source_state_message(FIRST, static_cast<uint8_t>(areg::LogSourceState::Paused), SECOND));
        processor.notify_log_configuration_restored(empty_message(FIRST));
        processor.notify_log_message(log_message(FIRST, "unseen"));
        processor.notify_connected_clients(disconnected_message({ SECOND }));

        ObserverEvents events{ make_all_events() };
        client.set_callbacks(&events);
        AREG_CHILD_CHECK(_seen.connected.empty() && _seen.scopes.empty() && _seen.texts.empty());
        AREG_CHILD_CHECK(_seen.states.empty() && _seen.restored.empty() && (_seen.updateCalls == 0u));

        processor.notify_connected_clients(disconnected_message({ FIRST, SECOND }));
        AREG_CHILD_CHECK(_seen.disconnected == std::vector<ITEM_ID>({ FIRST }));
    );
}

/**
 * A source state reaches the observer only for a known source cookie and a state a source can take.
 **/
TEST(LogObserverApiTest, source_state_is_reported_only_when_valid)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_source_state(source_state_message(areg::COOKIE_UNKNOWN, static_cast<uint8_t>(areg::LogSourceState::Active), SECOND));
        processor.notify_log_source_state(source_state_message(FIRST, 200u, SECOND));
        processor.notify_log_source_state(source_state_message(FIRST, static_cast<uint8_t>(areg::LogSourceState::Undefined), SECOND));
        AREG_CHILD_CHECK(_seen.states.empty());

        processor.notify_log_source_state(source_state_message(FIRST, static_cast<uint8_t>(areg::LogSourceState::Stopped), SECOND));
        AREG_CHILD_CHECK(_seen.states == std::vector<ITEM_ID>({ FIRST, static_cast<ITEM_ID>(areg::LogSourceState::Stopped), SECOND }));

        events.evtLogSourceState = nullptr;
        processor.notify_log_source_state(source_state_message(FIRST, static_cast<uint8_t>(areg::LogSourceState::Active), SECOND));
        AREG_CHILD_CHECK(_seen.states.size() == 3u);
    );
}

/**
 * A restored configuration is reported with the cookie of the source, and never for an unknown cookie.
 **/
TEST(LogObserverApiTest, restored_configuration_is_reported_with_the_source_cookie)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_configuration_restored(empty_message(areg::COOKIE_UNKNOWN));
        AREG_CHILD_CHECK(_seen.restored.empty());

        processor.notify_log_configuration_restored(empty_message(THIRD));
        AREG_CHILD_CHECK(_seen.restored == std::vector<ITEM_ID>({ THIRD }));

        events.evtLogConfigRestored = nullptr;
        processor.notify_log_configuration_restored(empty_message(FIRST));
        AREG_CHILD_CHECK(_seen.restored == std::vector<ITEM_ID>({ THIRD }));
    );
}

/**
 * Updated scope priorities reach their own callback, not the callback of the registration.
 **/
TEST(LogObserverApiTest, updated_scopes_reach_the_update_callback)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_update_scopes(scopes_message(2u, { "scope_a", "scope_b" }));
        AREG_CHILD_CHECK(_seen.updateCalls == 1u);
        AREG_CHILD_CHECK(_seen.updated == std::vector<std::string>({ "scope_a", "scope_b" }));
        AREG_CHILD_CHECK(_seen.scopeCalls == 0u);

        processor.notify_log_update_scopes(scopes_message(0u, { }));
        AREG_CHILD_CHECK(_seen.updateCalls == 2u);
        AREG_CHILD_CHECK(_seen.updated.size() == 2u);
    );
}

/**
 * A log message shorter than its header is dropped: no callback, and no log ID.
 **/
TEST(LogObserverApiTest, truncated_log_message_is_dropped)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        AREG_CHILD_CHECK(processor.notify_log_message(empty_message(FIRST)) == 0u);
        AREG_CHILD_CHECK(_seen.texts.empty());

        processor.notify_log_message(log_message(FIRST, "delivered"));
        AREG_CHILD_CHECK(_seen.texts == std::vector<std::string>({ "delivered" }));
    );
}

/**
 * Without the record callback a log message reaches the raw callback, which receives the whole entry.
 **/
TEST(LogObserverApiTest, raw_callback_receives_the_log_without_the_record_callback)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        events.evtLogMessage   = nullptr;
        events.evtLogMessageEx = &on_raw_log;
        areg::logger::LoggerClient::instance().set_callbacks(&events);
        areg::logger::ObserverMessageProcessor processor(areg::logger::LoggerClient::instance());

        processor.notify_log_message(log_message(FIRST, "raw"));
        AREG_CHILD_CHECK(_seen.rawLogs == 1u);
        AREG_CHILD_CHECK(_seen.texts.empty());
    );
}

/**
 * A configuration update before the observer is initialized is refused and changes nothing.
 **/
TEST(LogObserverApiTest, configuration_update_before_initialize_is_refused)
{
    AREG_EXPECT_IN_CHILD(
        const std::string dbPath{ (fs::temp_directory_path() / "areg-log-observer-refused.sqlog").generic_string() };
        AREG_CHILD_CHECK(log_observer_config_update(ADDRESS, 8383u, dbPath.c_str(), false) == false);

        ConfiguredObserver observer;
        AREG_CHILD_CHECK(log_observer_config_logger_port() == 8282u);
        AREG_CHILD_CHECK(areg::logger::LoggerClient::instance().config_database_name() == DB_NAME);
    );
}

/**
 * A disabled connection to the collector is reported, and an update still sets the collector.
 **/
TEST(LogObserverApiTest, disabled_collector_connection_is_reported_and_can_be_updated)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer("sqlite3", false);
        AREG_CHILD_CHECK(log_observer_config_logger_enabled() == false);
        AREG_CHILD_CHECK(log_observer_config_update(ADDRESS, 8383u, observer.path_of("other.sqlog").c_str(), false));
        AREG_CHILD_CHECK(log_observer_config_logger_port() == 8383u);
        AREG_CHILD_CHECK(areg::logger::LoggerClient::instance().config_database_name() == "other.sqlog");
    );
}

/**
 * With an unknown database engine the database settings are empty and cannot be changed.
 **/
TEST(LogObserverApiTest, unknown_database_engine_disables_the_database_settings)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer("unknown-engine");
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        AREG_CHILD_CHECK(client.is_sqlite_engine() == false);
        AREG_CHILD_CHECK(client.config_database_name().is_empty());
        AREG_CHILD_CHECK(client.config_database_path().is_empty());
        AREG_CHILD_CHECK(client.config_database_location().is_empty());
        AREG_CHILD_CHECK(client.set_config_database_name("renamed.sqlog") == false);
        AREG_CHILD_CHECK(client.set_config_database_location(observer.path_of("elsewhere").c_str()) == false);
        AREG_CHILD_CHECK(client.set_config_database_path(observer.path_of("other.sqlog").c_str(), true) == false);
        AREG_CHILD_CHECK(log_observer_config_update(ADDRESS, 8383u, observer.path_of("other.sqlog").c_str(), false) == false);
        AREG_CHILD_CHECK(client.config_database_name().is_empty());
    );
}

/**
 * A configuration update that asks to save it needs a connection to the collector; the new settings stay.
 **/
TEST(LogObserverApiTest, configuration_update_with_save_fails_without_a_connection)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        const std::string dbPath{ observer.path_of("updated.sqlog") };
        AREG_CHILD_CHECK(log_observer_config_update(ADDRESS, 8383u, dbPath.c_str(), true) == false);
        AREG_CHILD_CHECK(client.config_logger_port() == 8383u);
        AREG_CHILD_CHECK(client.config_database_name() == "updated.sqlog");

        AREG_CHILD_CHECK(log_observer_config_update(nullptr, areg::InvalidPort, dbPath.c_str(), false));
        AREG_CHILD_CHECK(client.config_logger_port() == 8383u);
        AREG_CHILD_CHECK(client.config_logger_address() == ADDRESS);
    );
}

/**
 * A database that cannot be created in the path keeps the observer paused and refuses the restart;
 * a restart in a usable path resumes it.
 **/
TEST(LogObserverApiTest, restart_of_logging_in_an_unusable_path_is_refused_and_keeps_the_pause)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer;
        AREG_CHILD_CHECK(log_observer_stop_logging(false, nullptr) == false);
        AREG_CHILD_CHECK(log_observer_connect_logger(observer.path_of("first.sqlog").c_str(), nullptr, 0u));
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverConnected);

        AREG_CHILD_CHECK(log_observer_stop_logging(true, nullptr));
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverPaused);

        std::error_code error;
        fs::create_directories(observer.path_of("directory"), error);
        AREG_CHILD_CHECK(log_observer_stop_logging(false, observer.path_of("directory").c_str()) == false);
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverPaused);

        AREG_CHILD_CHECK(log_observer_stop_logging(false, observer.path_of("second.sqlog").c_str()));
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverConnected);
        AREG_CHILD_CHECK(fs::exists(observer.path_of("second.sqlog")));
        log_observer_disconnect_logger();
        AREG_CHILD_CHECK(log_observer_current_state() == ObserverState::ObserverDisconnected);
    );
}

/**
 * A missing configuration file starts the observer with the default configuration: the default
 * collector port and no database.
 **/
TEST(LogObserverApiTest, missing_configuration_file_uses_the_defaults)
{
    AREG_EXPECT_IN_CHILD(
        const fs::path missing{ fs::temp_directory_path() / ("areg-log-observer-missing-" + std::to_string(areg::Process::instance().id()) + ".init") };
        ObserverEvents events{ make_events() };
        AREG_CHILD_CHECK(log_observer_initialize(&events, missing.generic_string().c_str()));
        AREG_CHILD_CHECK(log_observer_is_initialized());

        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        AREG_CHILD_CHECK(client.config_logger_port() == areg::DEFAULT_LOGGER_PORT);
        AREG_CHILD_CHECK(client.config_logger_address().is_empty() == false);
        AREG_CHILD_CHECK(client.is_sqlite_engine() == false);
        AREG_CHILD_CHECK(fs::exists(missing) == false);
        log_observer_release();
        AREG_CHILD_CHECK(log_observer_is_initialized() == false);
    );
}

/**
 * \brief   Without a configuration the collector settings are reported as not set.
 **/
TEST(LogObserverApiTest, collector_settings_without_a_configuration_are_not_set)
{
    AREG_EXPECT_IN_CHILD(
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        AREG_CHILD_CHECK(areg::Application::is_configured() == false);
        AREG_CHILD_CHECK(client.config_logger_address().is_empty());
        AREG_CHILD_CHECK(client.config_logger_port() == areg::InvalidPort);
        AREG_CHILD_CHECK(client.is_config_logger_connect_enabled() == false);
        AREG_CHILD_CHECK(client.request_save_configuration(areg::TARGET_ALL) == false);
        AREG_CHILD_CHECK(client.request_restore_configuration(areg::TARGET_ALL) == false);
        AREG_CHILD_CHECK(client.request_source_state(areg::TARGET_ALL, areg::LogSourceState::Paused) == false);
        AREG_CHILD_CHECK(client.request_change_scope_prio(areg::ScopeNames(), areg::TARGET_ALL) == false);
    );
}

/**
 * A failed receive is reported to the messaging failed callback while one is set, and to nobody once
 * the callbacks are removed. A message that cannot be processed changes nothing.
 **/
TEST(LogObserverApiTest, failed_receive_is_reported_to_the_messaging_failed_callback)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        events.evtMessagingFailed = &on_failed;
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        client.set_callbacks(&events);
        areg::SocketClient socket(areg::String(ADDRESS), 8383u);
        AREG_CHILD_CHECK(socket.create_fd());
        areg::RemoteMessageHandler & handler{ (areg::RemoteMessageHandler &)client };

        handler.failed_receive_message(socket);
        AREG_CHILD_CHECK(_seen.failed == 1u);

        const areg::MessageEnvelope unprocessed{ log_message(FIRST, "text") };
        handler.failed_process_message(unprocessed);
        AREG_CHILD_CHECK((_seen.failed == 1u) && _seen.texts.empty());

        client.set_callbacks(nullptr);
        handler.failed_receive_message(socket);
        AREG_CHILD_CHECK(_seen.failed == 1u);
    );
}

/**
 * A message of an invalid socket, an empty message, and a duplicate role notification
 * are not delivered to the application.
 **/
TEST(LogObserverApiTest, messages_that_are_invalid_or_not_meant_for_the_observer_are_not_delivered)
{
    AREG_EXPECT_IN_CHILD(
        ObserverEvents events{ make_all_events() };
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        client.set_callbacks(&events);
        areg::SocketClient valid(areg::String(ADDRESS), 8383u);
        AREG_CHILD_CHECK(valid.create_fd());
        areg::SocketClient closed(areg::String(ADDRESS), 8383u);
        areg::RemoteMessageHandler & handler{ (areg::RemoteMessageHandler &)client };

        areg::MessageEnvelope entry{ log_message(FIRST, "text") };
        handler.process_received_message(entry, closed);
        areg::MessageEnvelope empty;
        handler.process_received_message(empty, valid);

        areg::MessageEnvelope duplicate{ empty_message(FIRST) };
        duplicate.set_message_id(static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate));
        handler.process_received_message(duplicate, valid);
        AREG_CHILD_CHECK(_seen.texts.empty() && _seen.connected.empty() && _seen.scopes.empty());

        handler.process_received_message(entry, valid);
        AREG_CHILD_CHECK(_seen.texts.size() == 1u);
        client.set_callbacks(nullptr);
    );
}

/**
 * The client takes part in the save of the configuration: the file is written and the client stays configured.
 **/
TEST(LogObserverApiTest, configuration_save_with_the_client_as_listener_writes_the_file)
{
    AREG_EXPECT_IN_CHILD(
        ConfiguredObserver observer("sqlite3", false, 8282u, true);
        areg::logger::LoggerClient & client{ areg::logger::LoggerClient::instance() };
        const std::string path{ observer.path_of("saved.init") };
        AREG_CHILD_CHECK(areg::Application::config_manager().save_config(areg::String(path.c_str()), (areg::ConfigListener *)&client));
        AREG_CHILD_CHECK(fs::exists(fs::path(path)));
        AREG_CHILD_CHECK(client.config_logger_port() == 8282u);
    );
}

#endif  // AREG_LOGGING
