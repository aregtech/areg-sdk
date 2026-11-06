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
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areglogger/client/LogObserverApi.h"
#include "areglogger/client/private/LoggerClient.hpp"
#include "areglogger/client/private/ObserverMessageProcessor.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

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

    ObserverEvents make_events()
    {
        ObserverEvents events{ };
        events.evtInstConnected     = &on_connected;
        events.evtInstDisconnected  = &on_disconnected;
        events.evtLogRegisterScopes = &on_scopes;
        return events;
    }

    //!< Holds a private directory with the configuration of the observer, and the initialized observer.
    class ConfiguredObserver
    {
    public:
        ConfiguredObserver()
            : mDir(fs::temp_directory_path() / ("areg-log-observer-api-" + std::to_string(areg::Process::instance().id())))
        {
            std::error_code error;
            fs::create_directories(mDir, error);
            const fs::path config{ mDir / "observer.init" };
            std::ofstream file(config, std::ios::trunc);
            file << "log::*::target = db\n"
                 << "log::*::enable = true\n"
                 << "log::*::enable::db = true\n"
                 << "log::*::db::engine = sqlite3\n"
                 << "log::*::db::name = " << DB_NAME << "\n"
                 << "log::*::db::location = " << mDir.generic_string() << "\n"
                 << "logger::*::service = logcollector\n"
                 << "logger::*::connect = tcpip\n"
                 << "logger::*::enable::tcpip = true\n"
                 << "logger::*::address::tcpip = " << ADDRESS << "\n"
                 << "logger::*::port::tcpip = 8282\n";
            file.close();
            AREG_CHILD_CHECK(file.good());

            ObserverEvents events{ make_events() };
            AREG_CHILD_CHECK(log_observer_initialize(&events, config.generic_string().c_str()));
        }

        ~ConfiguredObserver()
        {
            log_observer_release();
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
}

/**
 * Reproduces the defect: the five path and name getters read the size of the buffer the wrong way round.
 * A buffer that holds the text stayed empty, and a smaller one received a part of it.
 **/
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

#endif  // AREG_LOGGING
