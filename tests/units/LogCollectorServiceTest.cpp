/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 *
 * \file        units/LogCollectorServiceTest.cpp
 * \brief       Areg Platform, tests of the log collector service: routing between log sources and
 *              observers, the console commands that reach sources, and the collected log database.
 *              Sources and observers are raw TCP clients of the collector running in the child.
 ************************************************************************/

#include "units/ServiceConsoleHarness.hpp"
#include "logcollector/app/LogCollector.hpp"
#include "logcollector/service/private/LogCollectorDatabase.hpp"
#include "areg/base/File.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <deque>
#include <filesystem>
#include <initializer_list>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#if defined(__linux__)
#include <sys/syscall.h>
#include <sys/timerfd.h>
#include <unistd.h>

namespace
{
    //!< While set, a one-second one-shot timer is armed shorter, as a system timer may expire early.
    std::atomic<bool>       gTimerExpiresEarly{ false };
    //!< The count of timers that were armed shorter.
    std::atomic<uint32_t>   gTimersShortened{ 0u };
}

//!< Replaces the system call of the framework timer, and arms the timer shorter while requested.
extern "C" __attribute__((visibility("default"))) int timerfd_settime(int fd, int flags, const struct itimerspec* value, struct itimerspec* previous) noexcept
{
    struct itimerspec armed{ *value };
    if (gTimerExpiresEarly.load() && (armed.it_value.tv_sec == 1) && (armed.it_value.tv_nsec == 0) && (armed.it_interval.tv_sec == 0) && (armed.it_interval.tv_nsec == 0))
    {
        armed.it_value.tv_sec  = 0;
        armed.it_value.tv_nsec = 900'000'000;
        gTimersShortened.fetch_add(1u);
    }

    return static_cast<int>(::syscall(SYS_timerfd_settime, fd, flags, &armed, previous));
}
#endif  // defined(__linux__)

namespace
{
    using namespace service_harness;
    using LogDb = areg::ext::LogSqliteDatabase;
    namespace fs = std::filesystem;

    constexpr uint32_t MAX_PAYLOAD  { 1u << 20 };
    [[maybe_unused]] constexpr uint32_t BATCH_ROWS   { 100u };
    [[maybe_unused]] constexpr uint32_t BATCH_COUNT  { 10u };
    const char* const  PROMPT       { "to quit the application" };

    areg::ext::ServiceApplicationBase& collector_application()
    {
        return LogCollector::instance();
    }

    constexpr ServiceUnderTest SERVICE{ &collector_application, "logger", areg::COOKIE_LOGGER, "logcollector_app_LogCollector", "" };

    constexpr uint32_t id_of(areg::FuncIdRange id)
    {
        return static_cast<uint32_t>(id);
    }

    //!< One message read from the collector.
    struct Received
    {
        areg::EventHeader           header{};
        std::vector<uint8_t>        payload;

        uint32_t id() const { return header.messageId; }
    };

    //!< The content of an instance notification.
    struct InstanceNotice
    {
        areg::RemoteConnectionState         state{ areg::RemoteConnectionState::Disconnected };
        std::vector<areg::ConnectedInstance> connected;
        std::vector<ITEM_ID>                disconnected;
    };

    void load(areg::SharedBuffer& buffer, const Received& message)
    {
        if (message.payload.empty() == false)
        {
            AREG_CHILD_CHECK(static_cast<size_t>(buffer.write(message.payload.data(), static_cast<uint32_t>(message.payload.size()))) == message.payload.size());
        }

        buffer.move_to_begin();
    }

    InstanceNotice parse_notice(const Received& message)
    {
        AREG_CHILD_CHECK(message.id() == id_of(areg::FuncIdRange::SystemServiceNotifyInstances));
        areg::SharedBuffer buffer;
        load(buffer, message);
        InstanceNotice notice;
        buffer >> notice.state;
        if (notice.state == areg::RemoteConnectionState::Connected)
        {
            uint32_t count{ 0u };
            buffer >> count;
            for (uint32_t i = 0u; i < count; ++i)
            {
                areg::ConnectedInstance instance;
                buffer >> instance;
                notice.connected.push_back(instance);
            }
        }
        else
        {
            areg::ArrayList<ITEM_ID> cookies;
            buffer >> cookies;
            for (uint32_t i = 0u; i < cookies.size(); ++i)
            {
                notice.disconnected.push_back(cookies[i]);
            }
        }

        return notice;
    }

    std::vector<areg::ScopeEntry> parse_scopes(const Received& message)
    {
        areg::SharedBuffer buffer;
        load(buffer, message);
        std::vector<areg::ScopeEntry> scopes;
        uint32_t count{ 0u };
        buffer >> count;
        for (uint32_t i = 0u; i < count; ++i)
        {
            areg::ScopeEntry entry;
            buffer >> entry;
            scopes.push_back(entry);
        }

        return scopes;
    }

    [[maybe_unused]] std::string row_text(const Received& message)
    {
        AREG_CHILD_CHECK(message.id() == id_of(areg::FuncIdRange::ServiceLogMessage));
        AREG_CHILD_CHECK(message.payload.size() >= areg::log_entry_head());
        const areg::LogEntry* entry{ reinterpret_cast<const areg::LogEntry*>(message.payload.data()) };
        AREG_CHILD_CHECK(message.payload.size() == areg::log_entry_size(*entry));
        return std::string(entry->logMessage);
    }

    //!< A raw client of the collector: a log source or an observer.
    class LogPeer
    {
    public:
        areg::SocketClient  socket;
        ITEM_ID             cookie;

        LogPeer(const PrivateConfig& config, areg::MessageSource kind)
            : socket("127.0.0.1", config.port)
            , cookie(areg::COOKIE_UNKNOWN)
            , mPending()
        {
            AREG_CHILD_CHECK(socket.create());
            set_io_deadline(socket.handle());
            const auto hello{ areg::create_connect_request(areg::COOKIE_UNKNOWN, SERVICE.channel, kind) };
            send(hello);
            for (;;)
            {
                Received reply;
                AREG_CHILD_CHECK(read(reply));
                if (reply.id() == id_of(areg::FuncIdRange::SystemServiceNotifyConnection))
                {
                    cookie = static_cast<ITEM_ID>(reply.header.target);
                    break;
                }

                mPending.push_back(std::move(reply));
            }

            AREG_CHILD_CHECK(cookie >= areg::COOKIE_REMOTE_SERVICE);
        }

        void send(const areg::MessageEnvelope& message) const
        {
            AREG_CHILD_CHECK(message.is_valid());
            const areg::EventHeader& header{ *message.header() };
            const int32_t size{ static_cast<int32_t>(areg::wire_size(header)) };
            AREG_CHILD_CHECK(socket.send(areg::wire_data(header), size) == size);
        }

        //!< Returns the next message, waiting for it.
        Received next()
        {
            Received message;
            if (mPending.empty() == false)
            {
                message = std::move(mPending.front());
                mPending.pop_front();
            }
            else
            {
                AREG_CHILD_CHECK(read(message));
            }

            return message;
        }

        //!< Returns the next message of the id, dropping the ones before it.
        Received wait_for(areg::FuncIdRange id)
        {
            for (;;)
            {
                Received message = next();
                if (message.id() == id_of(id))
                    return message;
            }
        }

        void close()
        {
            socket.close();
        }

    private:
        bool read_all(uint8_t* data, uint32_t size) const
        {
            uint32_t done{ 0u };
            while (done < size)
            {
                const int32_t count{ socket.receive(data + done, static_cast<int32_t>(size - done)) };
                if (count <= 0)
                    return false;

                done += static_cast<uint32_t>(count);
            }

            return true;
        }

        bool read(Received& message) const
        {
            if (read_all(areg::wire_header(message.header), areg::WIRE_HEADER_SIZE) == false)
                return false;

            if (message.header.bufHeader.biUsed > MAX_PAYLOAD)
                return false;

            message.payload.resize(message.header.bufHeader.biUsed);
            return message.payload.empty() || read_all(message.payload.data(), static_cast<uint32_t>(message.payload.size()));
        }

        std::deque<Received> mPending;
    };

    //!< Builds a scope list message with the id, from the peer, to the target. The scope ids are 100, 101, ...
    areg::MessageEnvelope scopes_message(areg::FuncIdRange id, const LogPeer& from, ITEM_ID target, std::initializer_list<std::pair<const char*, uint32_t>> scopes)
    {
        areg::ScopeNames names;
        uint32_t scopeId{ 100u };
        for (const auto& scope : scopes)
        {
            names.add(areg::ScopeEntry(scope.first, scopeId++, scope.second));
        }

        areg::MessageEnvelope message{ areg::message_update_scopes(from.cookie, target, names) };
        AREG_CHILD_CHECK(message.is_valid());
        message.set_message_id(id_of(id));
        return message;
    }

    areg::MessageEnvelope saved_message(const LogPeer& from)
    {
        areg::MessageEnvelope message{ areg::message_configuration_saved() };
        message.set_source(static_cast<uint32_t>(from.cookie));
        return message;
    }

    areg::MessageEnvelope state_updated_message(const LogPeer& from, areg::LogSourceState state, ITEM_ID byObserver)
    {
        return areg::message_source_state_updated(from.cookie, areg::COOKIE_LOGGER, state, byObserver);
    }

    [[maybe_unused]] areg::MessageEnvelope row_message(const LogPeer& from, const std::string& text)
    {
        areg::MessageEnvelope message{ areg::make_log_message(areg::LogMessageType::MessageText, 1u, 1u, 0, areg::LogPriority::PrioInfo
                                                            , text.c_str(), static_cast<uint32_t>(text.size())) };
        AREG_CHILD_CHECK(message.is_valid());
        areg::finalize_log_message(message, areg::LogDataType::Remote, from.cookie);
        message.set_target(static_cast<uint32_t>(areg::COOKIE_LOGGER));
        return message;
    }

    //!< Waits until the collector has handled everything the source sent before, using the observer.
    [[maybe_unused]] void sync_with_source(LogPeer& source, LogPeer& observer)
    {
        source.send(state_updated_message(source, areg::LogSourceState::Active, areg::COOKIE_UNKNOWN));
        const Received notice = observer.wait_for(areg::FuncIdRange::ServiceLogSourceStateUpdated);
        AREG_CHILD_CHECK(notice.header.source == static_cast<uint32_t>(source.cookie));
    }

    void check_request(const Received& message, areg::FuncIdRange id, ITEM_ID from, ITEM_ID to)
    {
        AREG_CHILD_CHECK(message.id() == id_of(id));
        AREG_CHILD_CHECK(message.header.source == static_cast<uint32_t>(from));
        AREG_CHILD_CHECK(message.header.target == static_cast<uint32_t>(to));
    }

    void remove_database(const std::string& path)
    {
        std::error_code error;
        for (const char* suffix : { "", "-wal", "-shm", "-journal" })
        {
            fs::remove(path + suffix, error);
        }
    }

    //!< Rewrites the private configuration with the extra lines, optionally enabling the database target.
    void extend_config(PrivateConfig& config, const std::string& extra, bool enableDatabase)
    {
        const std::string off{ "log::*::enable::db = false" };
        const size_t pos{ config.contents.find(off) };
        AREG_CHILD_CHECK(pos != std::string::npos);
        if (enableDatabase)
        {
            config.contents.replace(pos, off.size(), "log::*::enable::db = true");
        }

        config.contents += extra;
        std::ofstream output(config.path);
        output << config.contents;
        AREG_CHILD_CHECK(output.good());
    }

    std::string database_lines(const PrivateConfig& config, const char* engine)
    {
        return std::string("log::*::db::engine = ") + engine + "\nlog::*::db::location = " + config.directory
             + "\nlog::*::db::name = configured.sqlog\n";
    }

    std::string configured_database(const PrivateConfig& config)
    {
        return (fs::path(config.directory) / "configured.sqlog").string();
    }

    bool exists(const std::string& path)
    {
        std::error_code error;
        return fs::exists(path, error);
    }

    uint32_t priority_of(const char* name)
    {
        return areg::make_priorities(areg::String(name));
    }

    areg::ConnectedInstance make_instance(ITEM_ID cookie, const char* name, const char* location)
    {
        areg::ConnectedInstance instance;
        instance.ciSource    = areg::MessageSource::SourceClient;
        instance.ciBitness   = areg::InstanceBitness::Bitness64;
        instance.ciCookie    = cookie;
        instance.ciTimestamp = 1000;
        instance.ciInstance  = name;
        instance.ciLocation  = location;
        return instance;
    }

    [[maybe_unused]] std::string temp_database(const char* name)
    {
        const fs::path directory{ fs::path(testing::TempDir()) / ("logcollector-db-" + std::to_string(areg::Process::instance().id())) };
        std::error_code error;
        fs::create_directories(directory, error);
        const std::string path{ (directory / name).string() };
        remove_database(path);
        return path;
    }

    [[maybe_unused]] void remove_temp_directory(const std::string& path)
    {
        std::error_code error;
        fs::remove(fs::path(path).parent_path(), error);
    }

    [[maybe_unused]] const areg::LogEntry* log_of(const areg::SharedBuffer& buffer)
    {
        return reinterpret_cast<const areg::LogEntry*>(buffer.buffer());
    }

    [[maybe_unused]] std::vector<std::string> stored_rows(LogDb& db, ITEM_ID cookie)
    {
        std::vector<std::string> rows;
        for (const areg::SharedBuffer& buffer : db.log_inst_messages(cookie))
        {
            rows.push_back(std::string(log_of(buffer)->logMessage));
        }

        std::sort(rows.begin(), rows.end());
        return rows;
    }

    //////////////////////////////////////////////////////////////////////
    // Scenarios, each runs in a child process
    //////////////////////////////////////////////////////////////////////

    void scenario_instance_notices()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer observer(config, areg::MessageSource::SourceObserver);
        LogPeer first(config, areg::MessageSource::SourceClient);
        const InstanceNotice one = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(one.state == areg::RemoteConnectionState::Connected);
        AREG_CHILD_CHECK(one.connected.size() == 1u);
        AREG_CHILD_CHECK(one.connected[0].ciCookie == first.cookie);
        AREG_CHILD_CHECK(one.connected[0].ciSource == areg::MessageSource::SourceClient);

        LogPeer second(config, areg::MessageSource::SourceTest);
        const InstanceNotice two = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(two.state == areg::RemoteConnectionState::Connected);
        AREG_CHILD_CHECK(two.connected.size() == 2u);
        AREG_CHILD_CHECK(two.connected[0].ciCookie != two.connected[1].ciCookie);

        first.send(areg::create_disconnect_request(first.cookie, areg::COOKIE_LOGGER));
        const InstanceNotice left = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(left.state == areg::RemoteConnectionState::Disconnected);
        AREG_CHILD_CHECK(left.disconnected.size() == 1u && left.disconnected[0] == first.cookie);

        second.close();
        const InstanceNotice lost = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(lost.state == areg::RemoteConnectionState::Disconnected);
        AREG_CHILD_CHECK(lost.disconnected.size() == 1u && lost.disconnected[0] == second.cookie);
        console.quit();
    }

    void scenario_query_instances()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceTest);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        observer.send(areg::message_query_instances(observer.cookie, areg::COOKIE_LOGGER));
        const InstanceNotice listed = parse_notice(observer.next());
        AREG_CHILD_CHECK(listed.state == areg::RemoteConnectionState::Connected);
        AREG_CHILD_CHECK(listed.connected.size() == 2u);
        for (const areg::ConnectedInstance& instance : listed.connected)
        {
            AREG_CHILD_CHECK(instance.ciCookie == first.cookie || instance.ciCookie == second.cookie);
            AREG_CHILD_CHECK(instance.ciSource != areg::MessageSource::SourceObserver);
        }

        // A source gets no list, and a query that is not addressed to the collector gets no answer.
        first.send(areg::message_query_instances(first.cookie, areg::COOKIE_LOGGER));
        observer.send(areg::message_query_instances(observer.cookie, first.cookie));
        LogPeer third(config, areg::MessageSource::SourceClient);
        observer.send(areg::message_query_instances(observer.cookie, areg::COOKIE_LOGGER));
        const InstanceNotice announced = parse_notice(observer.next());
        AREG_CHILD_CHECK(announced.connected.size() == 3u);
        const InstanceNotice answered = parse_notice(observer.next());
        AREG_CHILD_CHECK(answered.connected.size() == 3u);

        observer.send(areg::message_update_scope(observer.cookie, first.cookie, "query_probe", 0u, priority_of("NOTSET")));
        check_request(first.next(), areg::FuncIdRange::ServiceLogUpdateScopes, observer.cookie, first.cookie);
        console.quit();
    }

    void scenario_register_scopes()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer source(config, areg::MessageSource::SourceClient);
        LogPeer first(config, areg::MessageSource::SourceObserver);
        LogPeer second(config, areg::MessageSource::SourceObserver);

        source.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, source, areg::COOKIE_LOGGER
                                 , { {"register_scope_a", priority_of("DEBUG")}, {"register_scope_b", priority_of("NOTSET")} }));
        for (LogPeer* observer : { &first, &second })
        {
            const Received message = observer->wait_for(areg::FuncIdRange::ServiceLogRegisterScopes);
            check_request(message, areg::FuncIdRange::ServiceLogRegisterScopes, source.cookie, observer->cookie);
            const std::vector<areg::ScopeEntry> scopes{ parse_scopes(message) };
            AREG_CHILD_CHECK(scopes.size() == 2u);
            AREG_CHILD_CHECK(scopes[0].scopeName == "register_scope_a" && scopes[0].scopeId == 100u);
            AREG_CHILD_CHECK(scopes[0].scopePrio == priority_of("DEBUG"));
            AREG_CHILD_CHECK(scopes[1].scopeName == "register_scope_b" && scopes[1].scopeId == 101u);
        }

        console.expect("Received scope registration from source " + std::to_string(source.cookie) + ", sent 2 scopes");
        console.quit();
    }

    void scenario_scope_routing()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        observer.send(areg::message_update_scope(observer.cookie, first.cookie, "route_one", 7u, priority_of("DEBUG")));
        const Received one = first.next();
        check_request(one, areg::FuncIdRange::ServiceLogUpdateScopes, observer.cookie, first.cookie);
        const std::vector<areg::ScopeEntry> scopes{ parse_scopes(one) };
        AREG_CHILD_CHECK(scopes.size() == 1u);
        AREG_CHILD_CHECK(scopes[0].scopeName == "route_one" && scopes[0].scopeId == 7u && scopes[0].scopePrio == priority_of("DEBUG"));

        // The second source sees only the broadcast, so the update addressed to the first one never reached it.
        observer.send(areg::message_update_scope(observer.cookie, areg::TARGET_ALL, "route_all", 8u, priority_of("NOTSET")));
        const Received all = first.next();
        check_request(all, areg::FuncIdRange::ServiceLogUpdateScopes, observer.cookie, first.cookie);
        AREG_CHILD_CHECK(parse_scopes(all)[0].scopeName == "route_all");
        const Received other = second.next();
        check_request(other, areg::FuncIdRange::ServiceLogUpdateScopes, observer.cookie, second.cookie);
        AREG_CHILD_CHECK(parse_scopes(other)[0].scopeName == "route_all");

        observer.send(areg::message_query_scopes(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, observer.cookie, second.cookie);
        observer.send(areg::message_query_scopes(observer.cookie, areg::TARGET_ALL));
        check_request(first.next(), areg::FuncIdRange::ServiceLogQueryScopes, observer.cookie, first.cookie);
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, observer.cookie, second.cookie);

        // A source cannot steer another source.
        first.send(areg::message_update_scope(first.cookie, second.cookie, "rogue_scope", 9u, priority_of("DEBUG")));
        first.send(areg::message_query_scopes(first.cookie, second.cookie));
        observer.send(areg::message_query_scopes(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, observer.cookie, second.cookie);
        console.quit();
    }

    void scenario_scopes_updated_and_restored()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer source(config, areg::MessageSource::SourceClient);
        LogPeer first(config, areg::MessageSource::SourceObserver);
        LogPeer second(config, areg::MessageSource::SourceObserver);

        source.send(scopes_message(areg::FuncIdRange::ServiceLogScopesUpdated, source, areg::COOKIE_LOGGER, { {"updated_scope", priority_of("DEBUG")} }));
        for (LogPeer* observer : { &first, &second })
        {
            const Received message = observer->wait_for(areg::FuncIdRange::ServiceLogScopesUpdated);
            check_request(message, areg::FuncIdRange::ServiceLogScopesUpdated, source.cookie, observer->cookie);
            AREG_CHILD_CHECK(parse_scopes(message)[0].scopeName == "updated_scope");
        }

        first.send(areg::message_restore_configuration(first.cookie, source.cookie));
        check_request(source.next(), areg::FuncIdRange::ServiceLogRestoreConfiguration, first.cookie, source.cookie);
        areg::MessageEnvelope restored{ areg::message_configuration_restored() };
        restored.set_source(static_cast<uint32_t>(source.cookie));
        source.send(restored);
        for (LogPeer* observer : { &first, &second })
        {
            check_request(observer->wait_for(areg::FuncIdRange::ServiceLogConfigurationRestored)
                        , areg::FuncIdRange::ServiceLogConfigurationRestored, source.cookie, observer->cookie);
        }

        console.quit();
    }

    void scenario_save_configuration_in_turn()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        observer.send(areg::message_save_configuration(observer.cookie, areg::TARGET_ALL));
        check_request(first.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, first.cookie);

        // The second source is asked only after the first one answered: its first message is the probe.
        observer.send(areg::message_query_scopes(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, observer.cookie, second.cookie);
        first.send(saved_message(first));
        check_request(second.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, second.cookie);
        second.send(saved_message(second));

        observer.send(areg::message_save_configuration(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, second.cookie);
        second.send(saved_message(second));

        observer.send(areg::message_save_configuration(observer.cookie, areg::COOKIE_LOGGER));
        check_request(first.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, first.cookie);
        console.quit();
    }

    void scenario_save_configuration_timeout()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        observer.send(areg::message_save_configuration(observer.cookie, first.cookie));
        check_request(first.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, first.cookie);
        observer.send(areg::message_save_configuration(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, second.cookie);
        console.quit();
    }

#if defined(__linux__)
    // Reproduces the defect: an expiry the clock measured as early was dropped and the save queue stopped for good.
    void scenario_save_configuration_early_expiry()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        gTimerExpiresEarly.store(true);
        observer.send(areg::message_save_configuration(observer.cookie, first.cookie));
        check_request(first.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, first.cookie);
        observer.send(areg::message_save_configuration(observer.cookie, second.cookie));
        check_request(second.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, second.cookie);
        gTimerExpiresEarly.store(false);
        AREG_CHILD_CHECK(gTimersShortened.load() != 0u);
        console.quit();
    }
#endif  // defined(__linux__)

    void scenario_save_configuration_source_leaves()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        observer.send(areg::message_save_configuration(observer.cookie, areg::TARGET_ALL));
        check_request(first.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, first.cookie);
        first.close();
        check_request(second.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, second.cookie);
        console.quit();
    }

    void scenario_source_states()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer paused(config, areg::MessageSource::SourceClient);
        LogPeer stopped(config, areg::MessageSource::SourceClient);
        LogPeer first(config, areg::MessageSource::SourceObserver);

        first.send(areg::message_update_source_state(first.cookie, paused.cookie, areg::LogSourceState::Paused));
        const Received request = paused.next();
        check_request(request, areg::FuncIdRange::ServiceLogUpdateSourceState, first.cookie, paused.cookie);

        paused.send(state_updated_message(paused, areg::LogSourceState::Paused, first.cookie));
        stopped.send(state_updated_message(stopped, areg::LogSourceState::Stopped, first.cookie));
        for (const LogPeer* source : { &paused, &stopped })
        {
            const Received message = first.wait_for(areg::FuncIdRange::ServiceLogSourceStateUpdated);
            check_request(message, areg::FuncIdRange::ServiceLogSourceStateUpdated, source->cookie, first.cookie);
        }

        // An observer that arrives late is told at once which sources are silent.
        LogPeer second(config, areg::MessageSource::SourceObserver);
        std::vector<ITEM_ID> told;
        for (int32_t i = 0; i < 2; ++i)
        {
            const Received message = second.wait_for(areg::FuncIdRange::ServiceLogSourceStateUpdated);
            areg::SharedBuffer buffer;
            load(buffer, message);
            ITEM_ID source{ 0u };
            uint8_t state{ 0u };
            ITEM_ID by{ 0u };
            buffer >> source;
            buffer >> state;
            buffer >> by;
            AREG_CHILD_CHECK(source == message.header.source);
            AREG_CHILD_CHECK(by == first.cookie);
            AREG_CHILD_CHECK(state == static_cast<uint8_t>(source == paused.cookie ? areg::LogSourceState::Paused : areg::LogSourceState::Stopped));
            told.push_back(source);
        }

        std::sort(told.begin(), told.end());
        AREG_CHILD_CHECK(told[0] != told[1]);

        // The last observer leaving puts a paused source back to sending and leaves a stopped one alone.
        first.close();
        second.close();
        const Received resumed = paused.next();
        check_request(resumed, areg::FuncIdRange::ServiceLogUpdateSourceState, areg::COOKIE_LOGGER, paused.cookie);
        areg::SharedBuffer buffer;
        load(buffer, resumed);
        ITEM_ID target{ 0u };
        uint8_t state{ 0u };
        buffer >> target;
        buffer >> state;
        AREG_CHILD_CHECK(target == paused.cookie && state == static_cast<uint8_t>(areg::LogSourceState::Active));

        LogPeer third(config, areg::MessageSource::SourceObserver);
        third.send(areg::message_update_scope(third.cookie, stopped.cookie, "after_stop", 1u, priority_of("NOTSET")));
        check_request(stopped.next(), areg::FuncIdRange::ServiceLogUpdateScopes, third.cookie, stopped.cookie);
        console.quit();
    }

    void scenario_misaddressed_messages()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        // A message that names another peer as its source is dropped, the next one of the same peer is handled.
        first.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, second, areg::COOKIE_LOGGER, { {"spoofed_scope", 1u} }));
        first.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, first, areg::COOKIE_LOGGER, { {"genuine_scope", 1u} }));
        const Received registered = observer.wait_for(areg::FuncIdRange::ServiceLogRegisterScopes);
        check_request(registered, areg::FuncIdRange::ServiceLogRegisterScopes, first.cookie, observer.cookie);
        AREG_CHILD_CHECK(parse_scopes(registered)[0].scopeName == "genuine_scope");

        // A registration from an observer reaches nobody.
        observer.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, observer, areg::COOKIE_LOGGER, { {"observer_scope", 1u} }));
        observer.send(scopes_message(areg::FuncIdRange::ServiceLogScopesUpdated, observer, areg::COOKIE_LOGGER, { {"observer_scope", 1u} }));
        observer.send(areg::message_query_instances(observer.cookie, areg::COOKIE_LOGGER));
        AREG_CHILD_CHECK(observer.next().id() == id_of(areg::FuncIdRange::SystemServiceNotifyInstances));
        console.quit();
    }

#if AREG_LOGGING

    void scenario_log_rows()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer source(config, areg::MessageSource::SourceClient);
        LogPeer first(config, areg::MessageSource::SourceObserver);
        LogPeer second(config, areg::MessageSource::SourceObserver);

        source.send(row_message(source, "first row"));
        for (LogPeer* observer : { &first, &second })
        {
            const Received row = observer->wait_for(areg::FuncIdRange::ServiceLogMessage);
            check_request(row, areg::FuncIdRange::ServiceLogMessage, source.cookie, observer->cookie);
            AREG_CHILD_CHECK(row_text(row) == "first row");
        }

        // A row that is cut or has a wrong size is dropped, the next one of the source is delivered.
        areg::MessageEnvelope broken{ row_message(source, "broken row") };
        broken.set_size_used(broken.size_used() - 1u);
        source.send(broken);
        const std::string full(areg::LOG_MSG_SIZE - 1u, 'x');
        source.send(row_message(source, full));
        for (LogPeer* observer : { &first, &second })
        {
            AREG_CHILD_CHECK(row_text(observer->wait_for(areg::FuncIdRange::ServiceLogMessage)) == full);
        }

        // A row sent by an observer is not forwarded.
        first.send(row_message(first, "from observer"));
        first.send(areg::message_query_instances(first.cookie, areg::COOKIE_LOGGER));
        AREG_CHILD_CHECK(first.next().id() == id_of(areg::FuncIdRange::SystemServiceNotifyInstances));

        // Rows arrive complete and in order, in batches the observers read before the next batch is sent.
        uint32_t number{ 0u };
        for (uint32_t batch = 0u; batch < BATCH_COUNT; ++batch)
        {
            const uint32_t begin{ number };
            for (uint32_t i = 0u; i < BATCH_ROWS; ++i)
            {
                source.send(row_message(source, "numbered row " + std::to_string(number++)));
            }

            for (LogPeer* observer : { &first, &second })
            {
                for (uint32_t i = 0u; i < BATCH_ROWS; ++i)
                {
                    AREG_CHILD_CHECK(row_text(observer->wait_for(areg::FuncIdRange::ServiceLogMessage)) == "numbered row " + std::to_string(begin + i));
                }
            }
        }

        console.quit();
    }

    void scenario_save_commands()
    {
        PrivateConfig config(SERVICE);
        const std::string path{ (fs::path(config.directory) / "collected.sqlog").string() };
        ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
        do
        {
            ConsoleSession console(config);
            LogPeer source(config, areg::MessageSource::SourceClient);
            LogPeer observer(config, areg::MessageSource::SourceObserver);
            cookie = source.cookie;

            console.command("--save " + path, "Started to save the collected logs in the database");
            console.command("--save " + path, "already saved in the database");
            source.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, source, areg::COOKIE_LOGGER
                                     , { {"stored_scope_a", priority_of("DEBUG")}, {"stored_scope_b", priority_of("NOTSET")} }));
            for (const char* text : { "stored row 1", "stored row 2", "stored row 3" })
            {
                source.send(row_message(source, text));
            }

            sync_with_source(source, observer);
            console.command("--unsave", "Stopped saving the collected logs in the database");
            console.command("--unsave", "The collected logs are not saved in a database");
            console.quit();
        } while (false);

        LogDb db;
        AREG_CHILD_CHECK(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
        const std::vector<areg::ConnectedInstance> instances{ db.log_instance_infos() };
        AREG_CHILD_CHECK(instances.size() == 1u && instances[0].ciCookie == cookie);
        AREG_CHILD_CHECK(instances[0].ciSource == areg::MessageSource::SourceClient);
        std::vector<areg::ScopeEntry> scopes{ db.log_inst_scopes(cookie) };
        std::sort(scopes.begin(), scopes.end(), [](const areg::ScopeEntry& l, const areg::ScopeEntry& r) { return l.scopeId < r.scopeId; });
        AREG_CHILD_CHECK(scopes.size() == 2u);
        AREG_CHILD_CHECK(scopes[0].scopeName == "stored_scope_a" && scopes[0].scopePrio == priority_of("DEBUG"));
        AREG_CHILD_CHECK(scopes[1].scopeName == "stored_scope_b");
        AREG_CHILD_CHECK((stored_rows(db, cookie) == std::vector<std::string>{ "stored row 1", "stored row 2", "stored row 3" }));
        db.disconnect();
        remove_database(path);
    }

    void scenario_configured_database()
    {
        PrivateConfig config(SERVICE);
        extend_config(config, database_lines(config, "sqlite3"), true);
        const std::string path{ configured_database(config) };
        ITEM_ID cookie{ areg::COOKIE_UNKNOWN };
        do
        {
            ConsoleSession console(config);
            LogPeer source(config, areg::MessageSource::SourceClient);
            cookie = source.cookie;
            console.command("--save " + path, "already saved in the database");
            console.quit();
        } while (false);

        AREG_CHILD_CHECK(exists(path));
        LogDb db;
        AREG_CHILD_CHECK(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
        const std::vector<areg::ConnectedInstance> instances{ db.log_instance_infos() };
        AREG_CHILD_CHECK(instances.size() == 1u && instances[0].ciCookie == cookie);
        db.disconnect();
        remove_database(path);
    }

#endif  // AREG_LOGGING

    void scenario_console_scope_commands()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);

        console.command("--scope *::console_scope_all=NOTSET", PROMPT);
        for (LogPeer* source : { &first, &second })
        {
            const Received message = source->next();
            check_request(message, areg::FuncIdRange::ServiceLogUpdateScopes, areg::COOKIE_LOGGER, source->cookie);
            const std::vector<areg::ScopeEntry> scopes{ parse_scopes(message) };
            AREG_CHILD_CHECK(scopes.size() == 1u);
            AREG_CHILD_CHECK(scopes[0].scopeName == "console_scope_all" && scopes[0].scopePrio == priority_of("NOTSET"));
        }

        console.command("--scope " + std::to_string(first.cookie) + "::console_scope_one=DEBUG", PROMPT);
        const Received one = first.next();
        check_request(one, areg::FuncIdRange::ServiceLogUpdateScopes, areg::COOKIE_LOGGER, first.cookie);
        AREG_CHILD_CHECK(parse_scopes(one)[0].scopeName == "console_scope_one");
        AREG_CHILD_CHECK(parse_scopes(one)[0].scopePrio == priority_of("DEBUG"));

        // The second source saw no update of the first one, its next message is the query.
        console.command("--query " + std::to_string(second.cookie), PROMPT);
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, areg::COOKIE_LOGGER, second.cookie);
        console.command("--query *", PROMPT);
        check_request(first.next(), areg::FuncIdRange::ServiceLogQueryScopes, areg::COOKIE_LOGGER, first.cookie);
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, areg::COOKIE_LOGGER, second.cookie);

        // A target that is not a cookie queries nobody.
        console.command("--query not_a_cookie", PROMPT);
        console.command("--scope " + std::to_string(first.cookie) + "::console_marker=NOTSET", PROMPT);
        const Received marker = first.next();
        check_request(marker, areg::FuncIdRange::ServiceLogUpdateScopes, areg::COOKIE_LOGGER, first.cookie);
        AREG_CHILD_CHECK(parse_scopes(marker)[0].scopeName == "console_marker");
        console.quit();
    }

    void scenario_save_command_failure()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        console.command("--save " + config.directory, "Failed to open the logging database");
        console.command("--unsave", "The collected logs are not saved in a database");
        console.quit();
    }

    void scenario_unknown_engine()
    {
        PrivateConfig config(SERVICE);
        extend_config(config, database_lines(config, "unknown_engine"), true);
        const std::string path{ configured_database(config) };
        do
        {
            ConsoleSession console(config);
            console.command("--save " + (fs::path(config.directory) / "manual.sqlog").string(), "Started to save the collected logs in the database");
            console.command("--unsave", "Stopped saving the collected logs in the database");
            console.quit();
        } while (false);

        AREG_CHILD_CHECK(!exists(path));
        remove_database((fs::path(config.directory) / "manual.sqlog").string());
    }

    void scenario_log_option(bool useDatabase)
    {
        PrivateConfig config(SERVICE);
        extend_config(config, database_lines(config, "sqlite3"), !useDatabase);
        const std::string path{ configured_database(config) };
        AREG_CHILD_CHECK(parse(SERVICE, { useDatabase ? "--log=db" : "--log=nodb" }));
        do
        {
            ConsoleSession console(config);
            if (useDatabase)
            {
                console.command("--save " + path, "already saved in the database");
            }
            else
            {
                console.command("--save " + (fs::path(config.directory) / "manual.sqlog").string(), "Started to save the collected logs in the database");
            }

            console.quit();
        } while (false);

        AREG_CHILD_CHECK(exists(path) == useDatabase);
        remove_database(path);
        remove_database((fs::path(config.directory) / "manual.sqlog").string());
    }

    void scenario_log_option_path()
    {
        PrivateConfig config(SERVICE);
        const std::string path{ (fs::path(config.directory) / "option.sqlog").string() };
        AREG_CHILD_CHECK(parse(SERVICE, { "--log=db", path }));
        do
        {
            ConsoleSession console(config);
            console.command("--save " + path, "already saved in the database");
            console.quit();
        } while (false);

        AREG_CHILD_CHECK(exists(path));
        remove_database(path);
    }

#if AREG_LOGGING

    void scenario_database_object()
    {
        const std::string path{ temp_database("object.sqlog") };
        do
        {
            LogCollectorDatabase database;
            AREG_CHILD_CHECK(database.is_active() == false);
            AREG_CHILD_CHECK(database.database_path().is_empty());
            database.save_instance_disconnected(1u);
            AREG_CHILD_CHECK(database.start(areg::String(path.c_str())));
            AREG_CHILD_CHECK(database.is_active());
            AREG_CHILD_CHECK(database.start(areg::String(path.c_str())));
            AREG_CHILD_CHECK(database.database_path().is_empty() == false);

            database.save_instance_connected(make_instance(41u, "object_instance", "/object/location"));
            areg::ScopeNames names;
            names.add(areg::ScopeEntry("object_scope_a", 1u, priority_of("DEBUG")));
            names.add(areg::ScopeEntry("object_scope_b", 2u, priority_of("NOTSET")));
            database.save_scopes(41u, areg::message_update_scopes(41u, areg::COOKIE_LOGGER, names));
            for (const char* text : { "object row 1", "object row 2" })
            {
                areg::MessageEnvelope row{ areg::make_log_message(areg::LogMessageType::MessageText, 1u, 1u, 0, areg::LogPriority::PrioInfo
                                                                , text, static_cast<uint32_t>(std::strlen(text))) };
                AREG_CHILD_CHECK(row.is_valid());
                areg::finalize_log_message(row, areg::LogDataType::Remote, 41u);
                database.save_log_message(row);
            }

            database.save_instance_connected(make_instance(42u, "second_instance", ""));
            database.save_instance_disconnected(42u);
            database.stop();
            AREG_CHILD_CHECK(database.is_active() == false);
            AREG_CHILD_CHECK(database.database_path().is_empty());
            database.stop();
            database.save_log_message(areg::MessageEnvelope());
        } while (false);

        LogDb db;
        AREG_CHILD_CHECK(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
        const std::vector<areg::ConnectedInstance> instances{ db.log_instance_infos() };
        AREG_CHILD_CHECK(instances.size() == 2u);
        bool found{ false };
        for (const areg::ConnectedInstance& instance : instances)
        {
            if (instance.ciCookie == 41u)
            {
                found = (instance.ciInstance == "object_instance") && (instance.ciLocation == "/object/location")
                     && (instance.ciBitness == areg::InstanceBitness::Bitness64);
            }
        }

        AREG_CHILD_CHECK(found);
        std::vector<areg::ScopeEntry> scopes{ db.log_inst_scopes(41u) };
        std::sort(scopes.begin(), scopes.end(), [](const areg::ScopeEntry& l, const areg::ScopeEntry& r) { return l.scopeId < r.scopeId; });
        AREG_CHILD_CHECK(scopes.size() == 2u);
        AREG_CHILD_CHECK(scopes[0].scopeName == "object_scope_a" && scopes[0].scopePrio == priority_of("DEBUG"));
        AREG_CHILD_CHECK((stored_rows(db, 41u) == std::vector<std::string>{ "object row 1", "object row 2" }));
        db.disconnect();
        remove_database(path);
        remove_temp_directory(path);
    }

    void scenario_database_truncated_scopes()
    {
        const std::string path{ temp_database("truncated.sqlog") };
        do
        {
            LogCollectorDatabase database;
            AREG_CHILD_CHECK(database.start(areg::String(path.c_str())));
            areg::ScopeNames names;
            names.add(areg::ScopeEntry("kept_scope", 1u, priority_of("DEBUG")));
            names.add(areg::ScopeEntry("", 2u, priority_of("DEBUG")));
            names.add(areg::ScopeEntry("after_empty_scope", 3u, priority_of("DEBUG")));
            database.save_scopes(51u, areg::message_update_scopes(51u, areg::COOKIE_LOGGER, names));

            areg::ScopeNames one;
            one.add(areg::ScopeEntry("counted_scope", 4u, priority_of("DEBUG")));
            areg::MessageEnvelope oversized{ areg::message_update_scopes(52u, areg::COOKIE_LOGGER, one) };
            AREG_CHILD_CHECK(oversized.is_valid());
            oversized.set_position(0, areg::Cursor::SeekOrigin::Begin);
            oversized << static_cast<uint32_t>(1000u);
            oversized.move_to_end();
            database.save_scopes(52u, oversized);

            areg::MessageEnvelope row{ areg::make_log_message(areg::LogMessageType::MessageText, 1u, 1u, 0, areg::LogPriority::PrioInfo, "still stored", 12u) };
            areg::finalize_log_message(row, areg::LogDataType::Remote, 52u);
            database.save_log_message(row);
            database.stop();
        } while (false);

        LogDb db;
        AREG_CHILD_CHECK(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
        const std::vector<areg::ScopeEntry> first{ db.log_inst_scopes(51u) };
        AREG_CHILD_CHECK(first.size() == 1u && first[0].scopeName == "kept_scope");
        const std::vector<areg::ScopeEntry> second{ db.log_inst_scopes(52u) };
        AREG_CHILD_CHECK(second.size() == 1u && second[0].scopeName == "counted_scope");
        AREG_CHILD_CHECK((stored_rows(db, 52u) == std::vector<std::string>{ "still stored" }));
        db.disconnect();
        remove_database(path);
        remove_temp_directory(path);
    }

    void scenario_database_invalid_scopes()
    {
        const std::string path{ temp_database("invalid.sqlog") };
        do
        {
            LogCollectorDatabase database;
            AREG_CHILD_CHECK(database.start(areg::String(path.c_str())));
            database.save_scopes(71u, areg::MessageEnvelope());

            areg::ScopeNames names;
            names.add(areg::ScopeEntry("after_invalid_message", 1u, priority_of("DEBUG")));
            database.save_scopes(72u, areg::message_update_scopes(72u, areg::COOKIE_LOGGER, names));
            database.stop();
        } while (false);

        LogDb db;
        AREG_CHILD_CHECK(db.connect(areg::String(path.c_str()), areg::DatabaseRole::Viewer));
        AREG_CHILD_CHECK(db.log_inst_scopes(71u).empty());
        const std::vector<areg::ScopeEntry> scopes{ db.log_inst_scopes(72u) };
        AREG_CHILD_CHECK(scopes.size() == 1u && scopes[0].scopeName == "after_invalid_message");
        db.disconnect();
        remove_database(path);
        remove_temp_directory(path);
    }

#endif  // AREG_LOGGING

    void scenario_database_unusable_path()
    {
        LogCollectorDatabase database;
        const std::string directory{ (fs::path(testing::TempDir()) / ("logcollector-dir-" + std::to_string(areg::Process::instance().id()))).string() };
        std::error_code error;
        fs::create_directories(directory, error);
        AREG_CHILD_CHECK(database.start(areg::String(directory.c_str())) == false);
        AREG_CHILD_CHECK(database.is_active() == false);
        AREG_CHILD_CHECK(database.database_path().is_empty());
        database.save_instance_connected(make_instance(61u, "ignored", ""));
        database.save_instance_disconnected(61u);
        database.stop();
        fs::remove(directory, error);
    }

    //!< Returns the names of the scopes of a scope update the source receives from the console.
    std::string console_update_of(LogPeer& source)
    {
        const Received message = source.next();
        check_request(message, areg::FuncIdRange::ServiceLogUpdateScopes, areg::COOKIE_LOGGER, source.cookie);
        const std::vector<areg::ScopeEntry> scopes{ parse_scopes(message) };
        AREG_CHILD_CHECK(scopes.size() == 1u);
        return scopes[0].scopeName.as_string();
    }

    //!< Sends a marker scope from the console, so the next message of every source tells what reached it before.
    void console_marker(ConsoleSession& console, const std::vector<LogPeer*>& sources, const char* marker)
    {
        console.command(std::string("--scope *::") + marker + "=NOTSET", PROMPT);
        for (LogPeer* source : sources)
        {
            AREG_CHILD_CHECK(console_update_of(*source) == marker);
        }
    }

    void scenario_console_scope_lists()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer first(config, areg::MessageSource::SourceClient);
        LogPeer second(config, areg::MessageSource::SourceClient);
        const std::vector<LogPeer*> sources{ &first, &second };

        console.command("--scope *::list_a=NOTSET;*::list_b=DEBUG", PROMPT);
        for (LogPeer* source : sources)
        {
            AREG_CHILD_CHECK(console_update_of(*source) == "list_a");
            AREG_CHILD_CHECK(console_update_of(*source) == "list_b");
        }

        console.command("--scope scope::prefixed_scope=DEBUG", PROMPT);
        console.command("--scope bare_scope=DEBUG", PROMPT);
        for (LogPeer* source : sources)
        {
            AREG_CHILD_CHECK(console_update_of(*source) == "prefixed_scope");
            AREG_CHILD_CHECK(console_update_of(*source) == "bare_scope");
        }

        // A list with an entry that is not a scope sends nothing, and neither does a target below the first cookie.
        console.command("--scope *::no_value;*::after_broken=DEBUG", PROMPT);
        console.command("--scope *::before_broken=DEBUG;*::no_value", PROMPT);
        console.command("--scope 5::low_target=DEBUG", PROMPT);
        console_marker(console, sources, "after_refused");

        console.command("--query " + std::to_string(first.cookie) + " *", PROMPT);
        check_request(first.next(), areg::FuncIdRange::ServiceLogQueryScopes, areg::COOKIE_LOGGER, first.cookie);
        check_request(second.next(), areg::FuncIdRange::ServiceLogQueryScopes, areg::COOKIE_LOGGER, second.cookie);
        console.quit();
    }

    void scenario_console_scope_without_sources()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        console.command("--scope *::nobody_scope=DEBUG", PROMPT);
        console.command("--query *", PROMPT);
        LogPeer source(config, areg::MessageSource::SourceClient);
        console_marker(console, { &source }, "first_update");
        console.quit();
    }

    void scenario_observer_requests_to_no_source()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer source(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);
        LogPeer other(config, areg::MessageSource::SourceObserver);

        // A request to an observer or to a cookie nobody has reaches no source and no observer.
        observer.send(areg::message_update_scope(observer.cookie, other.cookie, "to_observer", 1u, priority_of("DEBUG")));
        observer.send(areg::message_update_scope(observer.cookie, 9999u, "to_nobody", 2u, priority_of("DEBUG")));
        observer.send(areg::message_save_configuration(observer.cookie, 9998u));
        // Reproduces the defect: a save request for an observer or an unknown cookie was queued, and the observer got it.
        observer.send(areg::message_save_configuration(observer.cookie, other.cookie));
        observer.send(areg::message_update_scope(observer.cookie, source.cookie, "addressed", 3u, priority_of("DEBUG")));
        const Received addressed = source.next();
        check_request(addressed, areg::FuncIdRange::ServiceLogUpdateScopes, observer.cookie, source.cookie);
        AREG_CHILD_CHECK(parse_scopes(addressed)[0].scopeName == "addressed");

        // The save request of the unknown cookie left nothing pending, so the next one is sent at once.
        observer.send(areg::message_save_configuration(observer.cookie, source.cookie));
        check_request(source.next(), areg::FuncIdRange::ServiceSaveLogConfiguration, areg::COOKIE_LOGGER, source.cookie);

        // A scope registration addressed to another source reaches no observer.
        LogPeer target(config, areg::MessageSource::SourceClient);
        source.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, source, target.cookie, { {"misrouted_scope", 1u} }));
        source.send(scopes_message(areg::FuncIdRange::ServiceLogRegisterScopes, source, observer.cookie, { {"direct_scope", 1u} }));
        const Received direct = observer.wait_for(areg::FuncIdRange::ServiceLogRegisterScopes);
        check_request(direct, areg::FuncIdRange::ServiceLogRegisterScopes, source.cookie, observer.cookie);
        AREG_CHILD_CHECK(parse_scopes(direct)[0].scopeName == "direct_scope");

        // The update addressed to the observer did not reach the other observer either.
        other.send(areg::message_query_instances(other.cookie, areg::COOKIE_LOGGER));
        AREG_CHILD_CHECK(other.next().id() == id_of(areg::FuncIdRange::SystemServiceNotifyInstances));
        console.quit();
    }

    void scenario_service_peer_is_neither_source_nor_observer()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer observer(config, areg::MessageSource::SourceObserver);
        LogPeer service(config, areg::MessageSource::SourceService);
        LogPeer source(config, areg::MessageSource::SourceClient);

        // The observer is told about the source only: the service peer is not a log source.
        const InstanceNotice notice = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(notice.connected.size() == 1u && notice.connected[0].ciCookie == source.cookie);

        // A service peer that leaves is not announced, the source that leaves is.
        service.close();
        source.close();
        const InstanceNotice left = parse_notice(observer.wait_for(areg::FuncIdRange::SystemServiceNotifyInstances));
        AREG_CHILD_CHECK(left.state == areg::RemoteConnectionState::Disconnected);
        AREG_CHILD_CHECK(left.disconnected.size() == 1u && left.disconnected[0] == source.cookie);
        console.quit();
    }

    void scenario_state_of_unknown_source_is_not_remembered()
    {
        PrivateConfig config(SERVICE);
        ConsoleSession console(config);
        LogPeer source(config, areg::MessageSource::SourceClient);
        LogPeer observer(config, areg::MessageSource::SourceObserver);

        areg::MessageEnvelope state{ state_updated_message(source, areg::LogSourceState::Paused, observer.cookie) };
        state.set_position(0, areg::Cursor::SeekOrigin::Begin);
        state << areg::COOKIE_UNKNOWN;
        state.move_to_end();
        source.send(state);
        const Received forwarded = observer.wait_for(areg::FuncIdRange::ServiceLogSourceStateUpdated);
        check_request(forwarded, areg::FuncIdRange::ServiceLogSourceStateUpdated, source.cookie, observer.cookie);

        // A late observer is told no silent source: its first message is the answer to its query.
        LogPeer late(config, areg::MessageSource::SourceObserver);
        late.send(areg::message_query_instances(late.cookie, areg::COOKIE_LOGGER));
        AREG_CHILD_CHECK(late.next().id() == id_of(areg::FuncIdRange::SystemServiceNotifyInstances));
        console.quit();
    }
}

TEST(LogCollectorServiceTest, console_scope_lists_send_each_valid_scope_and_refuse_broken_lists)
{
    AREG_EXPECT_IN_CHILD(scenario_console_scope_lists());
}

TEST(LogCollectorServiceTest, console_scope_commands_without_sources_send_nothing)
{
    AREG_EXPECT_IN_CHILD(scenario_console_scope_without_sources());
}

TEST(LogCollectorServiceTest, observer_requests_to_no_source_reach_nobody)
{
    AREG_EXPECT_IN_CHILD(scenario_observer_requests_to_no_source());
}

TEST(LogCollectorServiceTest, service_peer_is_neither_announced_nor_removed_as_a_source)
{
    AREG_EXPECT_IN_CHILD(scenario_service_peer_is_neither_source_nor_observer());
}

TEST(LogCollectorServiceTest, state_of_an_unknown_source_is_forwarded_and_not_remembered)
{
    AREG_EXPECT_IN_CHILD(scenario_state_of_unknown_source_is_not_remembered());
}

TEST(LogCollectorServiceTest, observer_is_told_when_sources_connect_and_leave)
{
    AREG_EXPECT_IN_CHILD(scenario_instance_notices());
}

TEST(LogCollectorServiceTest, instance_list_goes_only_to_the_asking_observer)
{
    AREG_EXPECT_IN_CHILD(scenario_query_instances());
}

TEST(LogCollectorServiceTest, scope_registration_reaches_every_observer)
{
    AREG_EXPECT_IN_CHILD(scenario_register_scopes());
}

TEST(LogCollectorServiceTest, scope_updates_and_queries_reach_only_addressed_sources)
{
    AREG_EXPECT_IN_CHILD(scenario_scope_routing());
}

TEST(LogCollectorServiceTest, updated_scopes_and_restored_configuration_reach_observers)
{
    AREG_EXPECT_IN_CHILD(scenario_scopes_updated_and_restored());
}

TEST(LogCollectorServiceTest, save_configuration_asks_one_source_at_a_time)
{
    AREG_EXPECT_IN_CHILD(scenario_save_configuration_in_turn());
}

TEST(LogCollectorServiceTest, save_configuration_moves_on_after_the_answer_timeout)
{
    AREG_EXPECT_IN_CHILD(scenario_save_configuration_timeout());
}

#if defined(__linux__)
TEST(LogCollectorServiceTest, save_configuration_moves_on_when_the_timer_expires_early)
{
    AREG_EXPECT_IN_CHILD(scenario_save_configuration_early_expiry());
}
#endif  // defined(__linux__)

TEST(LogCollectorServiceTest, save_configuration_moves_on_when_the_asked_source_leaves)
{
    AREG_EXPECT_IN_CHILD(scenario_save_configuration_source_leaves());
}

TEST(LogCollectorServiceTest, source_states_are_forwarded_remembered_and_resumed)
{
    AREG_EXPECT_IN_CHILD(scenario_source_states());
}

TEST(LogCollectorServiceTest, spoofed_and_observer_scope_messages_are_ignored)
{
    AREG_EXPECT_IN_CHILD(scenario_misaddressed_messages());
}

TEST(LogCollectorServiceTest, console_scope_and_query_commands_reach_sources)
{
    AREG_EXPECT_IN_CHILD(scenario_console_scope_commands());
}

TEST(LogCollectorServiceTest, console_save_of_an_unusable_path_reports_the_failure)
{
    AREG_EXPECT_IN_CHILD(scenario_save_command_failure());
}

TEST(LogCollectorServiceTest, unknown_database_engine_keeps_the_configured_database_off)
{
    AREG_EXPECT_IN_CHILD(scenario_unknown_engine());
}

TEST(LogCollectorServiceTest, log_option_db_starts_the_database_the_configuration_left_off)
{
    AREG_EXPECT_IN_CHILD(scenario_log_option(true));
}

TEST(LogCollectorServiceTest, log_option_nodb_keeps_the_database_the_configuration_enabled_off)
{
    AREG_EXPECT_IN_CHILD(scenario_log_option(false));
}

TEST(LogCollectorServiceTest, log_option_db_takes_the_database_path)
{
    AREG_EXPECT_IN_CHILD(scenario_log_option_path());
}

TEST(LogCollectorDatabaseTest, unusable_path_is_refused_and_input_is_ignored_while_stopped)
{
    AREG_EXPECT_IN_CHILD(scenario_database_unusable_path());
}

#if AREG_LOGGING

TEST(LogCollectorServiceTest, log_rows_reach_every_observer_complete_and_in_order)
{
    AREG_EXPECT_IN_CHILD(scenario_log_rows());
}

TEST(LogCollectorServiceTest, console_save_records_instances_scopes_and_rows)
{
    AREG_EXPECT_IN_CHILD(scenario_save_commands());
}

TEST(LogCollectorServiceTest, configured_database_starts_with_the_collector)
{
    AREG_EXPECT_IN_CHILD(scenario_configured_database());
}

TEST(LogCollectorDatabaseTest, instances_scopes_and_rows_are_written_by_the_writer_thread)
{
    AREG_EXPECT_IN_CHILD(scenario_database_object());
}

TEST(LogCollectorDatabaseTest, truncated_and_empty_scope_entries_do_not_stop_the_writer)
{
    AREG_EXPECT_IN_CHILD(scenario_database_truncated_scopes());
}

TEST(LogCollectorDatabaseTest, invalid_scope_message_stores_nothing_and_the_writer_goes_on)
{
    AREG_EXPECT_IN_CHILD(scenario_database_invalid_scopes());
}

TEST(LogCollectorDatabaseTest, configured_path_uses_the_default_location_and_name_when_none_is_set)
{
    AREG_EXPECT_IN_CHILD(
        areg::Application::setup(false, true, false, true, false);
        const areg::String expected{ areg::File::make_full_path(areg::String(std::string(LogCollectorDatabase::DEFAULT_DB_LOCATION).c_str())
                                                              , areg::String(std::string(LogCollectorDatabase::DEFAULT_DB_NAME).c_str())) };
        AREG_CHILD_CHECK(expected.is_empty() == false);
        AREG_CHILD_CHECK(LogCollectorDatabase::configured_path() == expected);
        areg::Application::release();
    );
}

#endif  // AREG_LOGGING
