#include "units/GUnitTest.hpp"
#include "units/EventTestSupport.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadAddress.hpp"
#include "areg/ipc/ClientConnection.hpp"
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/ipc/ConnectionConsumer.hpp"
#include "areg/ipc/DataRateStats.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/ServerConnectionBase.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/base/private/SocketLiveness.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/persist/Property.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <cstring>
#include <functional>
#include <future>
#include <initializer_list>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#if defined(_POSIX)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#elif defined(WINDOWS)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#endif

namespace
{
    constexpr uint32_t IO_WAIT_MS{ 30'000u };
    constexpr uint32_t TCPIP{ static_cast<uint32_t>(areg::ConnectionType::Tcpip) };

    class ConfigurationScope
    {
    public:
        ConfigurationScope()
            : mConfig(areg::Application::config_manager())
            , mConfigured(mConfig.is_configured())
            , mReadonly(mConfig.readonly_properties())
            , mWritable(mConfig.module_properties())
        {
            set({});
        }

        ~ConfigurationScope()
        {
            mConfig.set_configuration(mReadonly, mWritable);
            if (!mConfigured)
            {
                mConfig.release_properties();
            }
        }

        void set(std::initializer_list<const char *> lines)
        {
            areg::ListProperties properties;
            for (const char * line : lines)
            {
                areg::Property property;
                EXPECT_TRUE(property.parse(areg::String(line)));
                properties.add(property);
            }
            mConfig.set_configuration(properties, areg::ListProperties());
        }

    private:
        areg::ConfigManager & mConfig;
        bool mConfigured;
        areg::ListProperties mReadonly;
        areg::ListProperties mWritable;
    };

    bool set_io_deadline(SOCKETHANDLE socket)
    {
#if defined(WINDOWS)
        const DWORD timeout{ IO_WAIT_MS };
#else
        const timeval timeout{ static_cast<time_t>(IO_WAIT_MS / 1000u), 0 };
#endif
        return areg::set_send_timeout(socket, IO_WAIT_MS)
            && (::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO
                           , reinterpret_cast<const char *>(&timeout), sizeof(timeout)) == 0);
    }

    class LoopbackListener
    {
    public:
        LoopbackListener()
            : mSocket()
            , mAddress()
        {
        }

        bool open(bool listen = true)
        {
            mSocket = areg::Socket::SocketHandle(areg::socket_create());
            if (!areg::is_valid_socket(mSocket.value()))
                return false;

            sockaddr_in local{};
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            if (::bind(mSocket.value(), reinterpret_cast<sockaddr *>(&local), sizeof(local)) != 0)
                return false;

#if defined(WINDOWS)
            int length{ sizeof(local) };
#else
            socklen_t length{ sizeof(local) };
#endif
            if (::getsockname(mSocket.value(), reinterpret_cast<sockaddr *>(&local), &length) != 0)
                return false;

            mAddress.from_sockaddr(local);
            return !listen || areg::server_listen(mSocket.value(), 8);
        }

        areg::SocketAccepted accept()
        {
            areg::SocketMultiplexer monitor;
            if (!monitor.register_socket(mSocket.value(), true))
            {
                return {};
            }

            areg::SocketAddress peer;
            const SOCKETHANDLE socket{ areg::server_accept(monitor, mSocket.value(), &peer, static_cast<int32_t>(IO_WAIT_MS)) };
            if (areg::is_valid_socket(socket))
            {
                EXPECT_TRUE(set_io_deadline(socket));
            }
            return areg::SocketAccepted(socket, peer);
        }

        const areg::SocketAddress & address() const
        {
            return mAddress;
        }

        bool start()
        {
            return areg::server_listen(mSocket.value(), 8);
        }

    private:
        areg::Socket::SocketHandle mSocket;
        areg::SocketAddress mAddress;
    };

    bool receive_wire(const areg::SocketAccepted & peer, areg::MessageEnvelope & message)
    {
        areg::EventHeader header{};
        if (peer.receive(areg::wire_header(header), areg::WIRE_HEADER_SIZE)
            != static_cast<int32_t>(areg::WIRE_HEADER_SIZE))
            return false;

        uint8_t * body{ areg::SocketConnectionBase::prepare_message(message, header) };
        if (body == nullptr)
            return false;

        const uint32_t size{ header.bufHeader.biUsed };
        if (size != 0u && peer.receive(body, static_cast<int32_t>(size)) != static_cast<int32_t>(size))
            return false;

        message.set_size_used(size);
        message.move_to_begin();
        return true;
    }

    bool send_wire(const areg::SocketAccepted & peer, const areg::MessageEnvelope & message)
    {
        const uint32_t size{ areg::wire_size(*message.header()) };
        return peer.send(areg::wire_data(*message.header()), static_cast<int32_t>(size)) == static_cast<int32_t>(size);
    }

    void expect_same_message(const areg::MessageEnvelope & expected, const areg::MessageEnvelope & actual)
    {
        ASSERT_TRUE(actual.is_valid());
        EXPECT_EQ(actual.message_id(), expected.message_id());
        EXPECT_EQ(actual.source(), expected.source());
        EXPECT_EQ(actual.target(), expected.target());
        ASSERT_EQ(actual.size_used(), expected.size_used());
        if (actual.size_used() != 0u)
        {
            EXPECT_EQ(std::memcmp(actual.buffer(), expected.buffer(), actual.size_used()), 0);
        }
    }

    class IpcSocketCoverageTest : public testing::Test
    {
    protected:
        void SetUp() override
        {
            ASSERT_TRUE(areg::socket_initialize());
        }

        void TearDown() override
        {
            areg::socket_release();
        }
    };

    class CommandRecorder final : public areg::ServiceEventConsumer
    {
    public:
        CommandRecorder()
            : mCommands()
            , mMessage()
        {
        }

        std::vector<uint32_t> mCommands;
        areg::MessageEnvelope mMessage;

        void on_reconnect_timer() override { mCommands.push_back(7u); }
        void on_service_start() override { mCommands.push_back(0u); }
        void on_service_stop() override { mCommands.push_back(1u); }
        void on_service_restart() override { mCommands.push_back(2u); }
        void on_connection_started() override { mCommands.push_back(3u); }
        void on_connection_stopped() override { mCommands.push_back(4u); }
        void on_connection_lost() override { mCommands.push_back(5u); }
        void on_service_exit() override { mCommands.push_back(6u); }
        void on_message_received(const areg::MessageEnvelope & message) override { mMessage = message; }
        void on_channel_connected(const ITEM_ID &) override { ADD_FAILURE(); }
    };
}

TEST(IpcConfigurationCoverageTest, named_and_enum_configurations_share_updates)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration named(areg::String("router"), areg::String("tcpip"));
    areg::ConnectionConfiguration typed(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    EXPECT_EQ(named.remote_service_name(), typed.remote_service_name());
    EXPECT_EQ(named.connection_type(), typed.connection_type());
    EXPECT_TRUE(named.is_configured());
    named.set_connection_data(areg::String("192.0.2.17"), 12345u);
    EXPECT_EQ(typed.connection_address(), areg::String("192.0.2.17"));
    EXPECT_EQ(typed.connection_port(), 12345u);
    typed.set_connection_address(areg::String("127.0.0.1"));
    typed.set_connection_port(65535u);
    EXPECT_EQ(named.connection_address(), areg::String("127.0.0.1"));
    EXPECT_EQ(named.connection_port(), 65535u);
    named.set_connection_enable(false);
    EXPECT_FALSE(typed.connection_enable_flag());
    typed.set_connection_enable(true);
    EXPECT_TRUE(named.connection_enable_flag());
}

TEST(IpcConfigurationCoverageTest, missing_network_keys_use_documented_defaults)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    EXPECT_EQ(config.socket_send_buffer(), areg::SOCKET_SEND_BUFFER_SIZE);
    EXPECT_EQ(config.socket_recv_buffer(), areg::SOCKET_RECV_BUFFER_SIZE);
    EXPECT_EQ(config.drain_limit(), areg::DEFAULT_DRAIN_LIMIT);
    EXPECT_EQ(config.pool_pairs(), areg::DEFAULT_POOL_PAIRS);
    EXPECT_EQ(config.socket_send_timeout(), 0u);
    areg::Application::config_manager().release_properties();
    EXPECT_FALSE(config.is_configured());
}

TEST(IpcConfigurationCoverageTest, network_keys_reach_both_remote_service_kinds)
{
    ConfigurationScope saved;
    saved.set({ "net::*::tcpip::sndbuf = 16", "net::*::tcpip::rcvbuf = 32"
              , "net::*::tcpip::drain = 5", "net::*::tcpip::pairs = 3"
              , "net::*::tcpip::timeout = 1500", "net::*::tcpip::keepalive = 40" });
    for (const auto kind : { areg::RemoteServiceKind::Router, areg::RemoteServiceKind::Logger })
    {
        areg::ConnectionConfiguration config(kind, areg::ConnectionType::Tcpip);
        EXPECT_EQ(config.socket_send_buffer(), 16u * 1024u);
        EXPECT_EQ(config.socket_recv_buffer(), 32u * 1024u);
        EXPECT_EQ(config.drain_limit(), 5u);
        EXPECT_EQ(config.pool_pairs(), 3u);
        EXPECT_EQ(config.socket_send_timeout(), 1500u);
        EXPECT_EQ(config.socket_refusal_timeout(), 1500u);
    }
}

TEST(IpcConfigurationCoverageTest, zero_timeout_uses_build_policy_and_keepalive_is_clamped)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    saved.set({ "net::*::tcpip::timeout = 0", "net::*::tcpip::keepalive = 40" });
    const uint32_t ordinary{ config.socket_refusal_timeout() };
    EXPECT_EQ(config.socket_send_timeout(), 0u);
    saved.set({ "net::*::tcpip::timeout = 0", "net::*::tcpip::keepalive = 7200" });
    const uint32_t clamped{ config.socket_refusal_timeout() };
#if defined(DEBUG)
    EXPECT_EQ(ordinary, clamped);
    EXPECT_GT(ordinary, areg::SOCKET_KEEPALIVE_MAX_SEC * 1000u);
#else
    EXPECT_EQ(ordinary, 40'000u);
    EXPECT_EQ(clamped, areg::SOCKET_KEEPALIVE_MAX_SEC * 1000u);
#endif
}

TEST(IpcConfigurationCoverageTest, ipv4_octets_include_zero_and_byte_boundaries)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    const std::array<std::array<uint8_t, 4>, 3> expected
    {
          std::array<uint8_t, 4>{ 0u, 0u, 0u, 0u }
        , std::array<uint8_t, 4>{ 1u, 127u, 128u, 255u }
        , std::array<uint8_t, 4>{ 255u, 255u, 255u, 255u }
    };
    uint32_t index{ 0u };
    for (const char * text : { "0.0.0.0", "1.127.128.255", "255.255.255.255" })
    {
        SCOPED_TRACE(text);
        config.set_connection_address(areg::String(text));
        std::array<uint8_t, 4> octets{};
        ASSERT_TRUE(config.connection_ip_address(octets[0], octets[1], octets[2], octets[3]));
        EXPECT_EQ(octets, expected[index++]);
    }
}

TEST(IpcConfigurationCoverageTest, invalid_ipv4_resets_every_output_octet)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    for (const char * text : { "", "localhost", ".1.2.3", "1..2.3", "1.2..3", "1.2.3."
                            , "1.2.3", "256.1.2.3", "1.256.2.3", "1.2.256.3", "1.2.3.256"
                            , "1x.2.3.4", "1.2x.3.4", "1.2.3x.4" })
    {
        SCOPED_TRACE(text);
        config.set_connection_address(areg::String(text));
        std::array<uint8_t, 4> octets{ 9u, 8u, 7u, 6u };
        EXPECT_FALSE(config.connection_ip_address(octets[0], octets[1], octets[2], octets[3]));
        EXPECT_EQ(octets, (std::array<uint8_t, 4>{}));
    }
}

// Reproduces the defect: an IPv4 address with extra text is accepted as its four-octet prefix.
TEST(IpcConfigurationCoverageTest, ipv4_rejects_suffix_after_fourth_octet)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    for (const char * text : { "1.2.3.4junk", "1.2.3.4.5", "1.2.3.4:80", "1.2.3.4." })
    {
        SCOPED_TRACE(text);
        config.set_connection_address(areg::String(text));
        std::array<uint8_t, 4> octets{ 9u, 8u, 7u, 6u };
        EXPECT_FALSE(config.connection_ip_address(octets[0], octets[1], octets[2], octets[3]));
        EXPECT_EQ(octets, (std::array<uint8_t, 4>{}));
    }
}

// Reproduces the defect: overflowing, signed, spaced or over-long octets were parsed as numbers.
TEST(IpcConfigurationCoverageTest, ipv4_rejects_non_decimal_and_overlong_octets)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    for (const char * text : { "4294967297.2.3.4", "1.2.3.4294967297", "0001.2.3.4", "1.2.3.0004"
                            , "+1.2.3.4", "-1.2.3.4", "1. 2.3.4", "1.2.3.-4", "1.2.3.+4" })
    {
        SCOPED_TRACE(text);
        config.set_connection_address(areg::String(text));
        std::array<uint8_t, 4> octets{ 9u, 8u, 7u, 6u };
        EXPECT_FALSE(config.connection_ip_address(octets[0], octets[1], octets[2], octets[3]));
        EXPECT_EQ(octets, (std::array<uint8_t, 4>{}));
    }
}

TEST(IpcConfigurationCoverageTest, ipv4_accepts_boundary_and_leading_zero_octets)
{
    ConfigurationScope saved;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    const std::pair<const char *, std::array<uint8_t, 4>> cases[]{
          { "0.0.0.0",         { 0u, 0u, 0u, 0u } }
        , { "255.255.255.255", { 255u, 255u, 255u, 255u } }
        , { "127.0.0.1",       { 127u, 0u, 0u, 1u } }
        , { "010.002.000.099", { 10u, 2u, 0u, 99u } }
    };
    for (const auto & entry : cases)
    {
        SCOPED_TRACE(entry.first);
        config.set_connection_address(areg::String(entry.first));
        std::array<uint8_t, 4> octets{ 9u, 8u, 7u, 6u };
        EXPECT_TRUE(config.connection_ip_address(octets[0], octets[1], octets[2], octets[3]));
        EXPECT_EQ(octets, entry.second);
    }
}

TEST(IpcStatisticsCoverageTest, enable_transitions_reset_but_repeated_enable_preserves_interval)
{
    areg::DataRateStats stats;
    EXPECT_FALSE(stats.is_enabled());
    stats.accumulate(99u, 9u);
    EXPECT_EQ(stats.extract_bytes(), 0u);
    EXPECT_EQ(stats.extract_msgs(), 0u);
    stats.set_enabled(true);
    EXPECT_TRUE(stats.is_enabled());
    stats.accumulate(uint64_t{ 1u } << 33u, 2u);
    stats.set_enabled(true);
    stats.accumulate(7u, 3u);
    EXPECT_EQ(stats.extract_bytes(), (uint64_t{ 1u } << 33u) + 7u);
    EXPECT_EQ(stats.extract_msgs(), 5u);
    EXPECT_EQ(stats.extract_bytes(), 0u);
    EXPECT_EQ(stats.extract_msgs(), 0u);
    stats.accumulate(12u, 1u);
    stats.set_enabled(false);
    EXPECT_FALSE(stats.is_enabled());
    stats.set_enabled(true);
    EXPECT_EQ(stats.extract_bytes(), 0u);
    EXPECT_EQ(stats.extract_msgs(), 0u);
}

// Reproduces the defect: the flag was a plain bool that one thread wrote while network threads read it.
TEST(IpcStatisticsCoverageTest, enable_toggled_while_another_thread_accumulates)
{
    areg::DataRateStats stats;
    std::atomic<bool> stop{ false };
    std::thread worker([&stats, &stop]
    {
        while (stop.load(std::memory_order_relaxed) == false)
        {
            stats.accumulate(1u, 1u);
        }
    });

    for (uint32_t i = 0u; i < 1'000u; ++i)
    {
        stats.set_enabled((i % 2u) == 0u);
    }

    stop.store(true, std::memory_order_relaxed);
    worker.join();
    EXPECT_FALSE(stats.is_enabled());
}

// Reproduces the defect: a new priority was written into a block the send thread could be using.
TEST(IpcStatisticsCoverageTest, send_priority_leaves_a_shared_block_unchanged)
{
    constexpr uint8_t NORMAL{ static_cast<uint8_t>(areg::EventPriority::NormalPrio) };
    constexpr uint8_t HIGH{ static_cast<uint8_t>(areg::EventPriority::HighPrio) };
    const uint8_t data[]{ 1u, 2u, 3u, 4u };
    areg::MessageEnvelope original(areg::message_bye_server(), static_cast<uint32_t>(sizeof(data)));
    ASSERT_EQ(original.write(data, static_cast<uint32_t>(sizeof(data))), sizeof(data));

    areg::MessageEnvelope same{ original };
    ASSERT_TRUE(areg::set_send_priority(same, areg::EventPriority::NormalPrio));
    EXPECT_EQ(same.buffer(), original.buffer());

    areg::MessageEnvelope copy{ original };
    ASSERT_TRUE(areg::set_send_priority(copy, areg::EventPriority::HighPrio));
    EXPECT_EQ(original.priority(), NORMAL);
    EXPECT_EQ(copy.priority(), HIGH);
    EXPECT_NE(copy.buffer(), original.buffer());
    ASSERT_EQ(copy.size_used(), original.size_used());
    EXPECT_EQ(std::memcmp(copy.buffer(), original.buffer(), original.size_used()), 0);

    uint8_t * const block{ copy.buffer() };
    ASSERT_TRUE(areg::set_send_priority(copy, areg::EventPriority::CriticalPrio));
    EXPECT_EQ(copy.buffer(), block);
}

TEST(IpcServiceEventCoverageTest, every_client_and_server_command_dispatches_exactly_once)
{
    CommandRecorder recorder;
    areg::ServiceClientConsumer client(recorder);
    areg::ServiceServerConsumer server(recorder);
    using Command = areg::ServiceEventData::ServiceCommand;
    const std::array<Command, 7> commands{ Command::CMD_StartService, Command::CMD_StopService
                                      , Command::CMD_RestartService, Command::CMD_ServiceStarted
                                      , Command::CMD_ServiceStopped, Command::CMD_ServiceLost
                                      , Command::CMD_ServiceExit };
    for (const Command command : commands)
    {
        areg::ServiceClientEvent event{ areg::ServiceEventData(command) };
        event.dispatch_self(&client);
    }
    EXPECT_EQ(recorder.mCommands, (std::vector<uint32_t>{ 0u, 1u, 2u, 3u, 4u, 5u, 6u }));
    recorder.mCommands.clear();
    for (const Command command : commands)
    {
        areg::ServiceServerEvent event{ areg::ServiceEventData(command) };
        event.dispatch_self(&server);
    }
    EXPECT_EQ(recorder.mCommands, (std::vector<uint32_t>{ 0u, 1u, 2u, 3u, 4u, 5u, 6u }));
}

TEST(IpcServiceEventCoverageTest, remote_consumer_preserves_envelope_and_payload)
{
    CommandRecorder recorder;
    areg::RemoteMessageConsumer consumer(recorder);
    const auto message{ areg::create_disconnect_request(1234u, areg::COOKIE_ROUTER) };
    areg::Event event(message);
    event.dispatch_self(&consumer);
    expect_same_message(message, recorder.mMessage);
    EXPECT_EQ(recorder.mMessage.header(), message.header());
    EXPECT_TRUE(recorder.mCommands.empty());
}

TEST(IpcServiceEventCoverageTest, remote_connection_state_stream_round_trip)
{
    areg::SharedBuffer buffer;
    buffer << areg::RemoteConnectionState::Connected << areg::RemoteConnectionState::Disconnected;
    buffer.move_to_begin();
    areg::RemoteConnectionState first{ areg::RemoteConnectionState::Disconnected };
    areg::RemoteConnectionState second{ areg::RemoteConnectionState::Connected };
    buffer >> first >> second;
    EXPECT_EQ(first, areg::RemoteConnectionState::Connected);
    EXPECT_EQ(second, areg::RemoteConnectionState::Disconnected);
}

TEST_F(IpcSocketCoverageTest, client_overloads_connect_exchange_frames_and_reset_cookie)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    for (uint32_t mode = 0u; mode != 3u; ++mode)
    {
        SCOPED_TRACE(mode);
        areg::ClientConnection named(listener.address().host_address(), listener.address().host_port());
        areg::ClientConnection addressed(listener.address());
        areg::ClientConnection explicitAddress;
        areg::ClientConnection & client{ mode == 0u ? named : (mode == 1u ? addressed : explicitAddress) };
        client.set_socket_buffers(64u * 1024u, 64u * 1024u);
        client.set_socket_buffers(0u, 0u);
        client.set_send_timeout(IO_WAIT_MS);
        client.set_send_timeout(0u);
        ASSERT_TRUE(mode == 2u
            ? client.create_socket(listener.address().host_address(), listener.address().host_port())
            : client.create_socket());
        ASSERT_TRUE(set_io_deadline(client.live_handle()));
        auto peer{ listener.accept() };
        ASSERT_TRUE(peer.is_valid());
        EXPECT_TRUE(client.is_valid());
        EXPECT_EQ(client.cookie(), areg::COOKIE_LOCAL);
        EXPECT_EQ(client.live_handle_word().load(), client.socket().handle());
        EXPECT_GE(areg::max_send_size(client.live_handle()), 64u * 1024u);
        const auto request{ areg::create_connect_request(areg::COOKIE_UNKNOWN, areg::COOKIE_ROUTER
                                                      , areg::MessageSource::SourceTest) };
        EXPECT_EQ(client.send_message(areg::MessageEnvelope()), 0);
        ASSERT_EQ(client.send_message(request), static_cast<int32_t>(areg::wire_size(*request.header())));
        areg::MessageEnvelope received;
        ASSERT_TRUE(receive_wire(peer, received));
        expect_same_message(request, received);
        const auto reply{ areg::create_connect_notify(areg::COOKIE_ROUTER, areg::COOKIE_REMOTE_SERVICE + mode) };
        ASSERT_TRUE(send_wire(peer, reply));
        ASSERT_EQ(client.receive_message(received), static_cast<int32_t>(areg::wire_size(*reply.header())));
        expect_same_message(reply, received);
        client.set_cookie(areg::COOKIE_REMOTE_SERVICE + mode);
        client.close_socket();
        EXPECT_EQ(client.cookie(), areg::COOKIE_UNKNOWN);
        EXPECT_FALSE(client.is_valid());
        EXPECT_EQ(client.live_handle(), areg::InvalidSocketHandle);
    }
}

TEST_F(IpcSocketCoverageTest, failed_split_connect_invalidates_live_handle_and_cookie)
{
    areg::ClientConnection unconfigured;
    EXPECT_FALSE(unconfigured.create_socket());
    EXPECT_FALSE(unconfigured.create_socket_fd());
    EXPECT_FALSE(unconfigured.create_socket(areg::String("127.0.0.1"), areg::InvalidPort));
    EXPECT_EQ(unconfigured.cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_FALSE(unconfigured.is_valid());
    LoopbackListener reserved;
    ASSERT_TRUE(reserved.open(false));
    areg::ClientConnection client(reserved.address());
    ASSERT_TRUE(client.create_socket_fd());
    EXPECT_TRUE(client.is_valid());
    EXPECT_EQ(client.cookie(), areg::COOKIE_LOCAL);
    EXPECT_FALSE(client.connect_socket());
    EXPECT_FALSE(client.is_valid());
    EXPECT_EQ(client.cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_EQ(client.live_handle_word().load(), areg::InvalidSocketHandle);
    client.interrupt_socket();
    client.close_socket();
    EXPECT_FALSE(client.connect_socket());
}

namespace
{
    //! Gives a test the buffer size setters of a socket.
    class SizedSocket : public areg::SocketClient
    {
    public:
        using areg::SocketClient::SocketClient;
        using areg::Socket::set_send_size;
        using areg::Socket::set_recv_size;
    };
}

TEST_F(IpcSocketCoverageTest, socket_buffer_sizes_change_when_larger_or_forced_and_need_an_open_socket)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    SizedSocket socket(listener.address().host_address(), listener.address().host_port());
    EXPECT_EQ(socket.set_send_size(64u * 1024u), areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(socket.set_recv_size(64u * 1024u), areg::PACKET_INVALID_SIZE);
    ASSERT_TRUE(socket.create());
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());

    const uint32_t send{ socket.set_send_size(1u) };
    EXPECT_NE(send, areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(socket.set_send_size(1u), send);
    const uint32_t sendForced{ socket.set_send_size(32u * 1024u, true) };
    EXPECT_NE(sendForced, areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(socket.set_send_size(1u), sendForced);

    const uint32_t recv{ socket.set_recv_size(1u) };
    EXPECT_NE(recv, areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(socket.set_recv_size(1u), recv);
    const uint32_t recvForced{ socket.set_recv_size(32u * 1024u, true) };
    EXPECT_NE(recvForced, areg::PACKET_INVALID_SIZE);
    EXPECT_EQ(socket.set_recv_size(1u), recvForced);

    socket.close();
    EXPECT_EQ(socket.set_send_size(64u * 1024u), areg::PACKET_INVALID_SIZE);
}

// Reproduces the defect: a failure path closed the client socket while another thread still used its descriptor.
TEST_F(IpcSocketCoverageTest, interrupting_a_client_socket_ends_a_blocked_receive_and_keeps_the_descriptor_until_close)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    areg::ClientConnection client(listener.address());
    ASSERT_TRUE(client.create_socket());
    ASSERT_TRUE(set_io_deadline(client.live_handle()));
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());
    client.set_cookie(areg::COOKIE_REMOTE_SERVICE);
    const SOCKETHANDLE handle{ client.socket().handle() };

    std::promise<int32_t> received;
    std::future<int32_t> done{ received.get_future() };
    std::thread reader([handle, &received]()
    {
        uint8_t byte{ 0u };
        received.set_value(areg::receive_data(handle, &byte, 1u));
    });

    client.interrupt_socket();
    ASSERT_EQ(done.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    reader.join();
    EXPECT_LE(done.get(), 0);
    EXPECT_EQ(client.live_handle(), areg::InvalidSocketHandle);
    EXPECT_EQ(client.cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_FALSE(client.is_valid());
    EXPECT_TRUE(client.socket().is_valid());
    EXPECT_EQ(client.socket().handle(), handle);

    client.close_socket();
    EXPECT_FALSE(client.socket().is_valid());
}

#if defined(__linux__) && defined(TCP_USER_TIMEOUT)
// Reproduces the defect: with an unlimited send timeout a client never bounded unacknowledged data, so it missed a dead router.
TEST_F(IpcSocketCoverageTest, unlimited_send_timeout_bounds_unacknowledged_data_by_the_keepalive_time)
{
    ConfigurationScope saved;
    saved.set({ "net::*::tcpip::keepalive = 15" });
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    for (const auto & check : { std::make_pair(areg::SEND_REFUSAL_UNLIMITED, 15'000u), std::make_pair(2'500u, 2'500u) })
    {
        SCOPED_TRACE(check.first);
        areg::ClientConnection client(listener.address());
        client.set_send_timeout(check.first);
        ASSERT_TRUE(client.create_socket());
        auto peer{ listener.accept() };
        ASSERT_TRUE(peer.is_valid());
        unsigned int userTimeout{ 0u };
        socklen_t length{ static_cast<socklen_t>(sizeof(userTimeout)) };
        ASSERT_EQ(::getsockopt(client.socket().handle(), IPPROTO_TCP, TCP_USER_TIMEOUT, &userTimeout, &length), 0);
        EXPECT_EQ(userTimeout, check.second);
        client.close_socket();
    }
}
#endif

TEST_F(IpcSocketCoverageTest, split_connect_batch_preserves_frame_order_and_send_half_close)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    areg::ClientConnection client;
    client.set_address(listener.address());
    ASSERT_TRUE(client.create_socket_fd());
    ASSERT_TRUE(client.connect_socket());
    ASSERT_TRUE(set_io_deadline(client.live_handle()));
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());
    const std::array<areg::MessageEnvelope, 3> messages
    {
          areg::create_disconnect_request(1111u, areg::COOKIE_ROUTER)
        , areg::create_disconnect_request(2222u, areg::COOKIE_ROUTER)
        , areg::create_disconnect_request(3333u, areg::COOKIE_ROUTER)
    };
    std::array<areg::IoBuffer, 3> buffers{};
    uint32_t total{ 0u };
    for (uint32_t i = 0u; i != messages.size(); ++i)
    {
        buffers[i] = { areg::wire_data(*messages[i].header()), areg::wire_size(*messages[i].header()) };
        total += static_cast<uint32_t>(buffers[i].size);
    }
    ASSERT_EQ(client.send_messages_batch(buffers.data(), static_cast<uint32_t>(buffers.size()), total)
             , static_cast<int32_t>(total));
    for (const auto & message : messages)
    {
        areg::MessageEnvelope received;
        ASSERT_TRUE(receive_wire(peer, received));
        expect_same_message(message, received);
    }
    ASSERT_TRUE(client.disable_send());
    uint8_t byte{};
    EXPECT_EQ(peer.receive(&byte, 1u), 0);
    const auto response{ areg::create_connect_notify(areg::COOKIE_ROUTER, 1234u) };
    ASSERT_TRUE(send_wire(peer, response));
    areg::MessageEnvelope received;
    ASSERT_GT(client.receive_message(received), 0);
    expect_same_message(response, received);
}

TEST_F(IpcSocketCoverageTest, truncated_header_and_body_report_incomplete_message)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    for (const bool partialHeader : { true, false })
    {
        areg::ClientConnection client(listener.address());
        ASSERT_TRUE(client.create_socket());
        ASSERT_TRUE(set_io_deadline(client.live_handle()));
        auto peer{ listener.accept() };
        ASSERT_TRUE(peer.is_valid());
        const auto message{ areg::create_connect_notify(areg::COOKIE_ROUTER, 1234u) };
        const uint32_t size{ partialHeader ? areg::WIRE_HEADER_SIZE - 1u : areg::wire_size(*message.header()) - 1u };
        ASSERT_EQ(peer.send(areg::wire_data(*message.header()), static_cast<int32_t>(size)), static_cast<int32_t>(size));
        ASSERT_TRUE(peer.disable_send());
        areg::MessageEnvelope received;
        EXPECT_EQ(client.receive_message(received), 0);
    }
}

TEST_F(IpcSocketCoverageTest, server_constructors_preserve_addresses_and_reject_invalid_clients)
{
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    areg::ServerConnectionBase named(listener.address().host_address(), listener.address().host_port());
    areg::ServerConnectionBase addressed(listener.address());
    for (auto * server : { &named, &addressed })
    {
        EXPECT_EQ(server->address().host_address(), listener.address().host_address());
        EXPECT_EQ(server->address().host_port(), listener.address().host_port());
        EXPECT_FALSE(server->is_valid());
        areg::SocketAccepted invalid;
        EXPECT_FALSE(server->accept_connection(invalid));
        EXPECT_EQ(server->cookie(areg::InvalidSocketHandle), areg::COOKIE_UNKNOWN);
        EXPECT_FALSE(areg::is_valid_socket(server->hold_by_cookie(areg::COOKIE_UNKNOWN).value()));
        EXPECT_FALSE(server->client_by_cookie(areg::COOKIE_UNKNOWN).is_valid());
    }
    areg::ServerConnectionBase invalid;
    EXPECT_FALSE(invalid.create_socket());
    EXPECT_FALSE(invalid.create_socket(areg::String("127.0.0.1"), areg::InvalidPort));
}

namespace
{
    class ChannelRecorder final : public areg::ConnectionConsumer
    {
    public:
        explicit ChannelRecorder(std::function<void()> firstLossAction = {})
            : mMutex()
            , mChanged()
            , mConnected()
            , mLost()
            , mDisconnected(0u)
            , mFirstLossAction(std::move(firstLossAction))
        {
        }

        void on_service_channel_connected(const areg::Channel & channel) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mConnected.push_back(channel);
            mChanged.notify_all();
        }

        void on_service_channel_disconnected(const areg::Channel &) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            ++mDisconnected;
            mChanged.notify_all();
        }

        void on_service_channel_lost(const areg::Channel & channel) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            if (mLost.empty() && mFirstLossAction)
            {
                mFirstLossAction();
            }
            mLost.push_back(channel);
            mChanged.notify_all();
        }

        bool wait_counts(uint32_t connected, uint32_t lost, uint32_t disconnected = 0u)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, EVENT_TEST_WAIT, [&]()
                {
                    return mConnected.size() >= connected && mLost.size() >= lost && mDisconnected >= disconnected;
                });
        }

        void expect_channels(std::initializer_list<ITEM_ID> connected, std::initializer_list<ITEM_ID> lost)
        {
            std::lock_guard<std::mutex> guard(mMutex);
            ASSERT_EQ(mConnected.size(), connected.size());
            ASSERT_EQ(mLost.size(), lost.size());
            uint32_t index{ 0u };
            for (const ITEM_ID cookie : connected)
            {
                EXPECT_EQ(mConnected[index++].cookie(), cookie);
            }
            index = 0u;
            for (const ITEM_ID cookie : lost)
            {
                EXPECT_EQ(mLost[index++].cookie(), cookie);
            }
        }

    private:
        std::mutex mMutex;
        std::condition_variable mChanged;
        std::vector<areg::Channel> mConnected;
        std::vector<areg::Channel> mLost;
        uint32_t mDisconnected;
        std::function<void()> mFirstLossAction;
    };

    class LoopbackService final : private areg::RemoteMessageHandler, public areg::ServiceClientConnectionBase
    {
    public:
        LoopbackService(ChannelRecorder & observer, EventTestDispatcher & dispatcher, uint32_t transport = TCPIP)
            : areg::RemoteMessageHandler()
            , areg::ServiceClientConnectionBase(areg::COOKIE_ROUTER, areg::RemoteServiceKind::Router, transport
                                             , areg::MessageSource::SourceTest, observer, *this, dispatcher
                                             , areg::String("IpcCoverage"))
        {
        }

        bool queue_payload(const areg::MessageEnvelope & message)
        {
            return send_message(message);
        }

    private:
        bool is_connection_allowed() const override
        {
            return true;
        }

        void failed_send_message(const areg::MessageEnvelope &, areg::Socket &) override
        {
            notify_connection_lost();
        }

        void failed_receive_message(areg::Socket &) override
        {
            notify_connection_lost();
        }

        void failed_process_message(const areg::MessageEnvelope &) override
        {
            ADD_FAILURE() << "Unexpected message processing failure";
        }

        void process_received_message(areg::MessageEnvelope & message, areg::Socket &) override
        {
            EXPECT_TRUE(send_received_message(std::move(message)));
        }

        void on_message_received(const areg::MessageEnvelope & message) override
        {
            service_connection_event(message);
        }
    };

    class ServiceRun
    {
    public:
        ServiceRun(LoopbackService & service, EventTestDispatcher & dispatcher)
            : mService(service)
            , mDispatcher(dispatcher)
        {
        }

        ~ServiceRun()
        {
            if (mDispatcher.is_running())
            {
                static_cast<areg::ConnectionProvider &>(mService).disconnect_service_host();
                EXPECT_TRUE(mDispatcher.barrier());
                mService.unregister_consumer_commands();
                mDispatcher.shutdown(IO_WAIT_MS);
                EXPECT_FALSE(mDispatcher.is_running());
            }
        }

    private:
        LoopbackService & mService;
        EventTestDispatcher & mDispatcher;
    };
}

TEST_F(IpcSocketCoverageTest, service_setup_rejects_mismatches_disabled_remote_and_missing_configuration)
{
    ConfigurationScope saved;
    ChannelRecorder observer;
    EventTestDispatcher dispatcher("IpcSetupCoverage");
    LoopbackService service(observer, dispatcher);
    auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
    EXPECT_FALSE(service.is_connect_state());
    EXPECT_TRUE(service.is_disconnect_state());
    EXPECT_FALSE(service.is_connected_state());
    EXPECT_FALSE(provider.is_host_connected());
    EXPECT_FALSE(provider.is_host_pending());
    EXPECT_FALSE(provider.is_host_setup());
    EXPECT_EQ(service.connection_cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_FALSE(provider.setup_connection_data(areg::RemoteServiceKind::Logger, TCPIP));
    EXPECT_FALSE(provider.setup_connection_data(areg::RemoteServiceKind::Router, 0u));
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(false);
    EXPECT_FALSE(provider.setup_connection_data(areg::RemoteServiceKind::Router, TCPIP));
    EXPECT_FALSE(provider.connect_service_host());
    config.set_connection_enable(true);
    config.set_connection_data(areg::String("127.0.0.1"), 12345u);
    ASSERT_TRUE(provider.setup_connection_data(areg::RemoteServiceKind::Router, TCPIP));
    EXPECT_TRUE(provider.is_host_setup());
    provider.apply_connection_data(areg::String("127.0.0.1"), 23456u);
    EXPECT_TRUE(provider.is_host_setup());
    areg::Application::config_manager().release_properties();
    EXPECT_FALSE(provider.setup_connection_data(areg::RemoteServiceKind::Router, TCPIP));
    EXPECT_FALSE(provider.connect_service_host());
    EXPECT_FALSE(service.is_data_rate_enabled());
    service.enable_data_rate(true);
    EXPECT_TRUE(service.is_data_rate_enabled());
    uint64_t bytes{ 1u };
    uint32_t messages{ 1u };
    service.query_data_sent(bytes, messages);
    EXPECT_EQ(bytes, 0u);
    EXPECT_EQ(messages, 0u);
    service.query_data_received(bytes, messages);
    EXPECT_EQ(bytes, 0u);
    EXPECT_EQ(messages, 0u);
    service.enable_data_rate(false);
    EXPECT_FALSE(service.is_data_rate_enabled());
}

TEST_F(IpcSocketCoverageTest, unsupported_transport_never_starts_remote_communication)
{
    ConfigurationScope saved;
    ChannelRecorder observer;
    EventTestDispatcher dispatcher("IpcUnsupportedTransport");
    constexpr uint32_t UDP{ static_cast<uint32_t>(areg::ConnectionType::Udp) };
    LoopbackService service(observer, dispatcher, UDP);
    auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
    EXPECT_FALSE(provider.setup_connection_data(areg::RemoteServiceKind::Router, UDP));
    EXPECT_FALSE(provider.connect_service_host());
    EXPECT_FALSE(provider.is_host_pending());
    EXPECT_TRUE(service.is_disconnect_state());
}

TEST_F(IpcSocketCoverageTest, outage_reconnects_with_new_cookie_and_stop_interrupts_partial_receive)
{
    ConfigurationScope saved;
    LoopbackListener listener;
    ASSERT_TRUE(listener.open(false));
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    ChannelRecorder observer([&listener]() { EXPECT_TRUE(listener.start()); });
    EventTestDispatcher dispatcher("IpcOutageCoverage");
    LoopbackService service(observer, dispatcher);
    ServiceRun run(service, dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    service.register_consumer_commands();
    auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
    provider.apply_connection_data(listener.address().host_address(), listener.address().host_port());
    service.enable_data_rate(true);
    ASSERT_TRUE(provider.connect_service_host());
    ASSERT_TRUE(observer.wait_counts(0u, 1u));
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());
    areg::MessageEnvelope handshake;
    ASSERT_TRUE(receive_wire(peer, handshake));
    EXPECT_EQ(handshake.message_id(), areg::message_hello_server().messageId);
    EXPECT_EQ(handshake.target(), areg::COOKIE_ROUTER);
    areg::ConnectedInstance instance{};
    handshake >> instance;
    EXPECT_EQ(instance.ciCookie, areg::COOKIE_UNKNOWN);
    EXPECT_EQ(instance.ciSource, areg::MessageSource::SourceTest);
    EXPECT_FALSE(instance.ciInstance.empty());
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_TRUE(service.is_connect_state());
    EXPECT_FALSE(service.is_connected_state());
    EXPECT_FALSE(provider.is_host_connected());
    EXPECT_FALSE(provider.connect_service_host());
    constexpr ITEM_ID FIRST_COOKIE{ areg::COOKIE_REMOTE_SERVICE + 17u };
    constexpr ITEM_ID SECOND_COOKIE{ FIRST_COOKIE + 1u };
    ASSERT_TRUE(send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, FIRST_COOKIE)));
    ASSERT_TRUE(observer.wait_counts(1u, 1u));
    EXPECT_TRUE(provider.is_host_connected());
    EXPECT_TRUE(service.is_connected_state());
    EXPECT_FALSE(service.is_disconnect_state());
    EXPECT_EQ(service.connection_cookie(), FIRST_COOKIE);
    peer.close();
    ASSERT_TRUE(observer.wait_counts(1u, 2u));
    peer = listener.accept();
    ASSERT_TRUE(peer.is_valid());
    ASSERT_TRUE(receive_wire(peer, handshake));
    EXPECT_EQ(handshake.source(), areg::COOKIE_UNKNOWN);
    ASSERT_TRUE(send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, SECOND_COOKIE)));
    ASSERT_TRUE(observer.wait_counts(2u, 2u));
    EXPECT_EQ(service.connection_cookie(), SECOND_COOKIE);

    std::vector<uint8_t> payload(128u * 1024u);
    for (uint32_t i = 0u; i != payload.size(); ++i)
    {
        payload[i] = static_cast<uint8_t>(i * 37u);
    }
    areg::MessageEnvelope message(areg::message_bye_server(), static_cast<uint32_t>(payload.size()));
    ASSERT_EQ(message.write(payload.data(), static_cast<uint32_t>(payload.size())), payload.size());
    ASSERT_TRUE(service.queue_payload(message));
    areg::MessageEnvelope received;
    ASSERT_TRUE(receive_wire(peer, received));
    expect_same_message(message, received);
    const auto partial{ areg::create_connect_notify(areg::COOKIE_ROUTER, SECOND_COOKIE) };
    const uint32_t incomplete{ areg::wire_size(*partial.header()) - 1u };
    ASSERT_EQ(peer.send(areg::wire_data(*partial.header()), static_cast<int32_t>(incomplete)), static_cast<int32_t>(incomplete));
    provider.disconnect_service_host();
    ASSERT_TRUE(observer.wait_counts(2u, 2u, 1u));
    ASSERT_TRUE(dispatcher.barrier());
    EXPECT_FALSE(provider.is_host_connected());
    EXPECT_EQ(service.connection_cookie(), areg::COOKIE_UNKNOWN);
    EXPECT_TRUE(service.is_disconnect_state());
    observer.expect_channels({ FIRST_COOKIE, SECOND_COOKIE }, { areg::COOKIE_LOCAL, FIRST_COOKIE });
}

// Reproduces the defect: sending a message again wrote routing fields into the block the send
// thread was using, so a queued copy could lose its consumer and pointers could reach the wire.
TEST_F(IpcSocketCoverageTest, message_sent_again_keeps_its_header_and_every_copy_arrives)
{
    constexpr uint32_t COPIES{ 400u };
    constexpr uint32_t PAYLOAD_SIZE{ 128u * 1024u };
    ConfigurationScope saved;
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    ChannelRecorder observer;
    EventTestDispatcher dispatcher("IpcSentAgain");
    LoopbackService service(observer, dispatcher);
    ServiceRun run(service, dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    service.register_consumer_commands();
    auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
    provider.apply_connection_data(listener.address().host_address(), listener.address().host_port());
    ASSERT_TRUE(provider.connect_service_host());
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());
    areg::MessageEnvelope handshake;
    ASSERT_TRUE(receive_wire(peer, handshake));
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 37u };
    ASSERT_TRUE(send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(observer.wait_counts(1u, 0u));

    std::vector<uint8_t> payload(PAYLOAD_SIZE);
    for (uint32_t i = 0u; i != PAYLOAD_SIZE; ++i)
    {
        payload[i] = static_cast<uint8_t>(i * 41u);
    }

    areg::MessageEnvelope message(areg::message_bye_server(), PAYLOAD_SIZE);
    ASSERT_EQ(message.write(payload.data(), PAYLOAD_SIZE), PAYLOAD_SIZE);

    // The peer reads nothing yet, so the socket fills up and the send thread stops with copies queued.
    for (uint32_t i = 0u; i != COPIES; ++i)
    {
        ASSERT_TRUE(service.queue_payload(message));
    }

    EXPECT_EQ(message.header()->internal1, 0u);
    EXPECT_EQ(message.header()->internal2, 0u);
    for (uint32_t i = 0u; i != COPIES; ++i)
    {
        areg::MessageEnvelope received;
        ASSERT_TRUE(receive_wire(peer, received)) << "copy " << i;
        expect_same_message(message, received);
    }
}

// Reproduces the defect: explicit reconnect stops the service but cannot start the next connection.
TEST_F(IpcSocketCoverageTest, explicit_reconnect_establishes_a_second_channel)
{
    ConfigurationScope saved;
    LoopbackListener listener;
    ASSERT_TRUE(listener.open());
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    ChannelRecorder observer;
    EventTestDispatcher dispatcher("IpcExplicitReconnect");
    LoopbackService service(observer, dispatcher);
    ServiceRun run(service, dispatcher);
    ASSERT_TRUE(dispatcher.start_ready());
    service.register_consumer_commands();
    auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
    provider.apply_connection_data(listener.address().host_address(), listener.address().host_port());
    ASSERT_TRUE(provider.connect_service_host());
    auto peer{ listener.accept() };
    ASSERT_TRUE(peer.is_valid());
    areg::MessageEnvelope handshake;
    ASSERT_TRUE(receive_wire(peer, handshake));
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 27u };
    ASSERT_TRUE(send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(observer.wait_counts(1u, 0u));
    ASSERT_TRUE(provider.reconnect_service_host());
    ASSERT_TRUE(dispatcher.barrier());
    auto reconnected{ listener.accept() };
    ASSERT_TRUE(reconnected.is_valid());
    ASSERT_TRUE(receive_wire(reconnected, handshake));
    ASSERT_TRUE(send_wire(reconnected, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE + 1u)));
    ASSERT_TRUE(observer.wait_counts(2u, 0u, 1u));
    EXPECT_TRUE(provider.is_host_connected());
    EXPECT_EQ(service.connection_cookie(), COOKIE + 1u);
}

namespace
{
    //! A loopback service connected to a test router with the cookie, and the router side of it.
    struct RouterSession
    {
        ConfigurationScope      saved;
        LoopbackListener        listener;
        ChannelRecorder         observer;
        EventTestDispatcher     dispatcher{ "IpcRouterNotice" };
        LoopbackService         service{ observer, dispatcher };
        ServiceRun              run{ service, dispatcher };
        areg::SocketAccepted    peer;

        //! Connects the service and answers its handshake with the cookie.
        bool connect(ITEM_ID cookie)
        {
            if (!listener.open())
                return false;

            areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
            config.set_connection_enable(true);
            if (!dispatcher.start_ready())
                return false;

            service.register_consumer_commands();
            auto & provider{ static_cast<areg::ConnectionProvider &>(service) };
            provider.apply_connection_data(listener.address().host_address(), listener.address().host_port());
            return provider.connect_service_host() && answer(cookie) && observer.wait_counts(1u, 0u);
        }

        //! Accepts the next connection of the service and answers its handshake with the cookie.
        bool answer(ITEM_ID cookie)
        {
            peer = listener.accept();
            areg::MessageEnvelope handshake;
            return peer.is_valid() && receive_wire(peer, handshake)
                && (handshake.message_id() == areg::message_hello_server().messageId)
                && send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, cookie));
        }
    };
}

/**
 * \brief   The router telling the client that its connection is gone: the channel is reported
 *          lost, the client connects again and gets the new cookie.
 **/
TEST_F(IpcSocketCoverageTest, router_disconnect_notice_loses_the_channel_and_the_client_connects_again)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 51u };
    RouterSession session;
    ASSERT_TRUE(session.connect(COOKIE));
    ASSERT_TRUE(send_wire(session.peer, areg::create_disconnect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(session.observer.wait_counts(1u, 1u));
    EXPECT_NE(session.service.connection_cookie(), COOKIE);

    ASSERT_TRUE(session.answer(COOKIE + 1u));
    ASSERT_TRUE(session.observer.wait_counts(2u, 1u));
    EXPECT_EQ(session.service.connection_cookie(), COOKIE + 1u);
}

/**
 * \brief   The router rejecting the client: the channel is reported disconnected, not lost, and the
 *          client connects again after the retry time, and gets the new cookie.
 **/
TEST_F(IpcSocketCoverageTest, router_reject_notice_disconnects_the_channel_and_the_client_retries)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 61u };
    RouterSession session;
    ASSERT_TRUE(session.connect(COOKIE));
    ASSERT_TRUE(send_wire(session.peer, areg::create_reject_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(session.observer.wait_counts(1u, 0u, 1u));
    ASSERT_TRUE(session.dispatcher.barrier());
    EXPECT_FALSE(static_cast<areg::ConnectionProvider &>(session.service).is_host_connected());

    ASSERT_TRUE(session.answer(COOKIE + 1u));
    ASSERT_TRUE(session.observer.wait_counts(2u, 0u, 1u));
    EXPECT_EQ(session.service.connection_cookie(), COOKIE + 1u);
    session.observer.expect_channels({ COOKIE, COOKIE + 1u }, { });
}

/**
 * \brief   Reproduces the defect: a second start command queued while the first one is pending
 *          started a second connection over the live one. Now the client makes one connection.
 **/
/**
 * \brief   Reproduces the defect: the socket options were plain fields, which a second connect
 *          request wrote while the dispatcher applied them to the new socket.
 **/
TEST(IpcClientConnectionTest, socket_options_change_while_the_socket_is_created)
{
    constexpr uint32_t ROUNDS{ 200u };
    areg::ClientConnection client;
    ASSERT_TRUE(client.set_address(areg::String("127.0.0.1"), 8383u));
    std::thread writer([&client]()
    {
        for (uint32_t i = 1u; i <= ROUNDS; ++i)
        {
            client.set_socket_buffers(64u * 1024u + i, 64u * 1024u + i);
            client.set_send_timeout(1000u + i);
        }
    });

    for (uint32_t i = 0u; i < ROUNDS; ++i)
    {
        EXPECT_TRUE(client.create_socket_fd());
        client.close_socket();
    }

    writer.join();
}

TEST_F(IpcSocketCoverageTest, start_requested_twice_makes_one_connection)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 66u };
    RouterSession session;
    ASSERT_TRUE(session.listener.open());
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    ASSERT_TRUE(session.dispatcher.start_ready());
    session.service.register_consumer_commands();
    auto & provider{ static_cast<areg::ConnectionProvider &>(session.service) };
    provider.apply_connection_data(session.listener.address().host_address(), session.listener.address().host_port());

    std::promise<void> release;
    std::shared_future<void> released{ release.get_future().share() };
    ASSERT_TRUE(session.dispatcher.post_action([released]() { released.wait(); }));
    ASSERT_TRUE(provider.connect_service_host());
    ASSERT_TRUE(provider.connect_service_host());
    release.set_value();

    ASSERT_TRUE(session.answer(COOKIE));
    ASSERT_TRUE(session.observer.wait_counts(1u, 0u));
    ASSERT_TRUE(session.dispatcher.barrier());
    EXPECT_TRUE(provider.is_host_connected());
    EXPECT_EQ(session.service.connection_cookie(), COOKIE);
    session.observer.expect_channels({ COOKIE }, { });
}

/**
 * \brief   Reproduces the defect: a message queued while another thread was still starting the
 *          send thread of a new connection was refused. Now every producer of the first
 *          messages of a connection is accepted, and the router reads every message.
 **/
TEST_F(IpcSocketCoverageTest, first_messages_of_many_producers_are_all_accepted_while_the_send_thread_starts)
{
    constexpr uint32_t ROUNDS{ 20u };
    constexpr uint32_t PRODUCERS{ 6u };
    constexpr uint32_t MESSAGES{ 40u };
    constexpr uint32_t LARGE{ 80u * 1024u };
    constexpr uint32_t SMALL{ 64u };
    std::vector<uint8_t> payload(LARGE, 0x5Au);
    for (uint32_t round = 0u; round < ROUNDS; ++ round)
    {
        RouterSession session;
        ASSERT_TRUE(session.connect(areg::COOKIE_REMOTE_SERVICE + 121u + round));
        std::atomic<uint32_t> ready{ 0u };
        std::atomic<uint32_t> refused{ 0u };
        std::vector<std::thread> producers;
        for (uint32_t i = 0u; i < PRODUCERS; ++ i)
        {
            producers.emplace_back([&, i]()
                {
                    ready.fetch_add(1u);
                    while (ready.load() < PRODUCERS)
                    {
                        std::this_thread::yield();
                    }

                    // The first message of the first producer is too large to be written inline.
                    for (uint32_t n = 0u; n < MESSAGES; ++ n)
                    {
                        const uint32_t size{ ((i == 0u) && (n == 0u)) ? LARGE : SMALL };
                        areg::MessageEnvelope message(areg::message_bye_server(), size);
                        static_cast<void>(message.write(payload.data(), size));
                        if (session.service.queue_payload(message) == false)
                        {
                            refused.fetch_add(1u);
                        }
                    }
                });
        }

        for (std::thread & producer : producers)
        {
            producer.join();
        }

        EXPECT_EQ(refused.load(), 0u) << "round " << round;
        for (uint32_t i = refused.load(); i < PRODUCERS * MESSAGES; ++ i)
        {
            areg::MessageEnvelope received;
            ASSERT_TRUE(receive_wire(session.peer, received)) << "round " << round << ", message " << i;
        }
    }
}

/**
 * \brief   A connect notice with a failed result is a lost connection: the client keeps no
 *          cookie, connects again and takes the cookie of the next successful notice.
 **/
TEST_F(IpcSocketCoverageTest, connect_notice_with_a_failed_result_loses_the_connection_and_the_client_connects_again)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 71u };
    RouterSession session;
    ASSERT_TRUE(session.connect(COOKIE));
    areg::MessageEnvelope failed{ areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE + 1u) };
    failed.set_result(areg::MESSAGE_SUCCESS + 1u);
    ASSERT_TRUE(send_wire(session.peer, failed));
    ASSERT_TRUE(session.observer.wait_counts(1u, 1u));

    ASSERT_TRUE(session.answer(COOKIE + 2u));
    ASSERT_TRUE(session.observer.wait_counts(2u, 1u));
    EXPECT_EQ(session.service.connection_cookie(), COOKIE + 2u);
}

namespace
{
    //! The router side of the router client of a running application.
    struct FakeRouter
    {
        ConfigurationScope      saved;
        LoopbackListener        listener;
        areg::SocketAccepted    peer;
        bool                    started{ false };

        ~FakeRouter()
        {
            if (started)
            {
                areg::Application::release();
            }
        }

        //! Starts the application and connects its router client with the cookie.
        bool connect(ITEM_ID cookie)
        {
            if (!listener.open())
                return false;

            areg::Application::setup(false, true, false, true, false);
            started = true;
            areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
            config.set_connection_enable(true);
            config.set_connection_data(listener.address().host_address(), listener.address().host_port());
            return areg::Application::connect_router() && answer(cookie);
        }

        //! Accepts the next connection and answers its handshake with the cookie.
        bool answer(ITEM_ID cookie)
        {
            peer = listener.accept();
            areg::MessageEnvelope handshake;
            return peer.is_valid() && receive_wire(peer, handshake)
                && (handshake.message_id() == areg::message_hello_server().messageId)
                && send_wire(peer, areg::create_connect_notify(areg::COOKIE_ROUTER, cookie));
        }
    };

    //! A message of the router with the header and no data.
    areg::MessageEnvelope router_message(const areg::EventHeader & header, ITEM_ID target)
    {
        areg::EventHeader hdr{ header };
        hdr.target = static_cast<uint32_t>(target);
        areg::MessageEnvelope message;
        EXPECT_NE(message.init_envelope(hdr, 0u), nullptr);
        return message;
    }
}

// Reproduces the defect: an executable message of a connection event type that no provider takes
// reached an assertion in the failure reply, and the receiving process aborted.
/**
 * \brief   The router client of an application takes, in order, a duplicate role notice, a
 *          message the client does not handle, an unknown message that is not executable, an
 *          executable message of a connection event type and a request for a provider that does
 *          not exist, and stays connected: the disconnect notice after them makes it connect again.
 **/
TEST_F(IpcSocketCoverageTest, router_client_ignores_notices_it_cannot_serve_and_reconnects_after_a_disconnect_notice)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 81u };
    FakeRouter router;
    ASSERT_TRUE(router.connect(COOKIE));

    areg::MessageEnvelope duplicate{ router_message(areg::message_duplicate_notify(), COOKIE) };
    areg::ConnectedInstance other{};
    other.ciCookie = COOKIE + 5u;
    other.ciInstance = "other";
    other.ciLocation = "elsewhere";
    duplicate << other;
    ASSERT_TRUE(send_wire(router.peer, duplicate));

    areg::EventHeader ignored{ areg::message_duplicate_notify() };
    ignored.messageId = static_cast<uint32_t>(areg::FuncIdRange::ServiceLastId);
    ASSERT_TRUE(send_wire(router.peer, router_message(ignored, COOKIE)));

    areg::EventHeader unknown{ areg::message_duplicate_notify() };
    unknown.messageId = static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyDuplicate) + 1u;
    ASSERT_FALSE(areg::is_executable_id(unknown.messageId));
    ASSERT_TRUE(send_wire(router.peer, router_message(unknown, COOKIE)));

    areg::EventHeader foreign{ areg::message_duplicate_notify() };
    foreign.messageId = static_cast<uint32_t>(areg::FuncIdRange::EmptyFunctionId);
    ASSERT_TRUE(areg::is_executable_id(foreign.messageId));
    ASSERT_TRUE(send_wire(router.peer, router_message(foreign, COOKIE)));

    areg::EventHeader request{};
    request.source = static_cast<uint32_t>(COOKIE + 5u);
    request.messageId = static_cast<uint32_t>(areg::FuncIdRange::RequestFirstId);
    request.eventType = static_cast<uint16_t>(areg::EventType::EventRemoteRequest);
    request.provider.number = 0x613AFFFFu;
    request.consumer.number = 0x613AFFFEu;
    ASSERT_TRUE(send_wire(router.peer, router_message(request, COOKIE)));

    ASSERT_TRUE(send_wire(router.peer, areg::create_disconnect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(router.answer(COOKIE + 1u));
}

namespace
{
    //! Waits until the router client reports the connection confirmed. Returns false on timeout.
    bool wait_router_connected()
    {
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(IO_WAIT_MS) };
        while (areg::Application::router_state() != areg::RouterState::Connected)
        {
            if (std::chrono::steady_clock::now() > deadline)
                return false;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        return true;
    }

    //! Posts a remote request for the stub of the cookie to the dispatcher thread of the router client.
    bool post_remote_request(ITEM_ID stubCookie, uint32_t requestId)
    {
        areg::Thread * thread{ areg::Thread::find_by_address(areg::ThreadAddress(areg::String("router_CLIENT_DISPATCH_MESSAGE_THREAD"))) };
        areg::DispatcherThread * dispatcher{ AREG_RUNTIME_CAST(thread, areg::DispatcherThread) };
        if (dispatcher == nullptr)
            return false;

        areg::ProxyAddress proxy(0x613B2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public, 0x613B3001u, 0x613B4001u, 0x613B1001u);
        areg::StubAddress stub(0x613B2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public, 0x613B3002u, 0x613B4002u, 0x613B1002u);
        stub.set_cookie(stubCookie);
        const uint8_t data[]{ 1u, 2u, 3u, 4u };
        areg::RemoteRequestEvent event(areg::SharedBuffer(data, static_cast<uint32_t>(sizeof(data))), proxy, stub, requestId);
        event.set_target_dispatcher(dispatcher);
        return event.deliver_event();
    }

    //! Posts a remote attribute subscription and a remote response to the dispatcher thread of the router client.
    bool post_remote_notify_and_response(ITEM_ID stubCookie, uint32_t attributeId, uint32_t responseId)
    {
        areg::Thread * thread{ areg::Thread::find_by_address(areg::ThreadAddress(areg::String("router_CLIENT_DISPATCH_MESSAGE_THREAD"))) };
        areg::DispatcherThread * dispatcher{ AREG_RUNTIME_CAST(thread, areg::DispatcherThread) };
        if (dispatcher == nullptr)
            return false;

        areg::ProxyAddress proxy(0x613B2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public, 0x613B3001u, 0x613B4001u, 0x613B1001u);
        areg::StubAddress stub(0x613B2001u, areg::Version(1u, 0u, 0u), areg::ServiceType::Public, 0x613B3002u, 0x613B4002u, 0x613B1002u);
        stub.set_cookie(stubCookie);
        proxy.set_cookie(stubCookie);
        areg::RemoteNotifyRequestEvent notify(proxy, stub, attributeId, areg::RequestType::StartNotify);
        notify.set_target_dispatcher(dispatcher);
        areg::RemoteResponseEvent response(proxy, areg::ResultType::RequestOK, responseId);
        response.set_target_dispatcher(dispatcher);
        return notify.deliver_event() && response.deliver_event();
    }
}

/**
 * \brief   Reproduces the defect: a remote request posted to the router client before the
 *          router confirmed the connection went out with no cookie and the router dropped it.
 *          Now nothing goes out without the cookie: a request, a subscription or a response
 *          handled before the confirmation fails or is dropped, and one handled after it goes
 *          out with the cookie of the connection as its source.
 **/
TEST_F(IpcSocketCoverageTest, remote_request_before_the_connection_is_confirmed_fails_and_is_not_sent)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 91u };
    constexpr ITEM_ID STUB_COOKIE{ areg::COOKIE_REMOTE_SERVICE + 92u };
    constexpr uint32_t REQUEST{ areg::REQUEST_ID_FIRST + 5u };
    FakeRouter router;
    ASSERT_TRUE(router.listener.open());
    areg::Application::setup(false, true, false, true, false);
    router.started = true;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    config.set_connection_data(router.listener.address().host_address(), router.listener.address().host_port());
    ASSERT_TRUE(areg::Application::connect_router());
    router.peer = router.listener.accept();
    ASSERT_TRUE(router.peer.is_valid());
    areg::MessageEnvelope handshake;
    ASSERT_TRUE(receive_wire(router.peer, handshake));

    ASSERT_TRUE(post_remote_request(STUB_COOKIE, REQUEST));
    ASSERT_TRUE(post_remote_notify_and_response(STUB_COOKIE, areg::ATTRIBUTE_ID_FIRST + 2u, areg::RESPONSE_ID_FIRST + 2u));
    ASSERT_TRUE(send_wire(router.peer, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(wait_router_connected());
    ASSERT_TRUE(post_remote_request(STUB_COOKIE, REQUEST + 1u));

    // A message handled after the confirmation may go out; none goes out without the cookie.
    areg::MessageEnvelope later;
    do
    {
        ASSERT_TRUE(receive_wire(router.peer, later));
        EXPECT_EQ(later.source(), COOKIE);
    } while (later.message_id() != REQUEST + 1u);

    EXPECT_EQ(later.target(), STUB_COOKIE);
}

// Reproduces the defect: the dispatcher thread read the channel to stamp an outgoing message
// while the receive thread wrote that channel on a connection notification.
TEST_F(IpcSocketCoverageTest, outgoing_messages_keep_the_cookie_while_the_router_confirms_the_connection)
{
    constexpr ITEM_ID COOKIE{ areg::COOKIE_REMOTE_SERVICE + 93u };
    constexpr ITEM_ID STUB_COOKIE{ areg::COOKIE_REMOTE_SERVICE + 94u };
    constexpr uint32_t LAST_REQUEST{ areg::REQUEST_ID_FIRST + 9u };
    constexpr int ROUNDS{ 200 };
    FakeRouter router;
    ASSERT_TRUE(router.listener.open());
    areg::Application::setup(false, true, false, true, false);
    router.started = true;
    areg::ConnectionConfiguration config(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip);
    config.set_connection_enable(true);
    config.set_connection_data(router.listener.address().host_address(), router.listener.address().host_port());
    ASSERT_TRUE(areg::Application::connect_router());
    router.peer = router.listener.accept();
    ASSERT_TRUE(router.peer.is_valid());
    areg::MessageEnvelope handshake;
    ASSERT_TRUE(receive_wire(router.peer, handshake));
    ASSERT_TRUE(send_wire(router.peer, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE)));
    ASSERT_TRUE(wait_router_connected());

    for (int round{ 0 }; round < ROUNDS; ++round)
    {
        ASSERT_TRUE(send_wire(router.peer, areg::create_connect_notify(areg::COOKIE_ROUTER, COOKIE)));
        ASSERT_TRUE(post_remote_notify_and_response(STUB_COOKIE, areg::ATTRIBUTE_ID_FIRST + 3u, areg::RESPONSE_ID_FIRST + 3u));
    }

    ASSERT_TRUE(post_remote_request(STUB_COOKIE, LAST_REQUEST));
    areg::MessageEnvelope outgoing;
    int count{ 0 };
    do
    {
        ASSERT_TRUE(receive_wire(router.peer, outgoing));
        if (areg::is_executable_id(static_cast<uint32_t>(outgoing.message_id())))
        {
            EXPECT_EQ(outgoing.source(), COOKIE);
            ++count;
        }
    } while (outgoing.message_id() != LAST_REQUEST);

    EXPECT_EQ(count, 2 * ROUNDS + 1);
}
