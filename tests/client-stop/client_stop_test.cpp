/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        client-stop/client_stop_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, stops the router client of an application connected to a fake
 *              router and checks that every stop ends in time and leaves the service manager free.
 *
 *              Usage: areg-client-stop-test <case>
 *              Cases: release-idle, release-flood, release-mid-message, disconnect-reconnect,
 *              disconnect-flood-load.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/UtilityDefs.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/persist/ConfigManager.hpp"

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else   // !defined(_WIN32)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif  // defined(_WIN32)

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    using Clock = std::chrono::steady_clock;

    constexpr char const _modelName[]   { "ClientStopModel" };
    constexpr char const _role[]        { "ClientStopProbe" };
    constexpr char const _thread[]      { "ClientStopThread" };

    //!< The cookie the fake router gives the client.
    constexpr ITEM_ID   CLIENT_COOKIE   { areg::COOKIE_REMOTE_SERVICE + 7u };

    //!< The cookie of the remote process whose providers the fake router announces.
    constexpr ITEM_ID   PEER_COOKIE     { areg::COOKIE_REMOTE_SERVICE + 8u };

    //!< Milliseconds the client is given to connect.
    constexpr int64_t   CONNECT_MS      { 5'000 };

    //!< The body size announced by the header of the cut message.
    constexpr uint32_t  CUT_BODY        { 3u * 1024u * 1024u };

    //!< Milliseconds since \a start.
    int64_t elapsed_ms(Clock::time_point start)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count();
    }

    //!< Writes the result of a check and returns it.
    bool check(bool condition, const char * what)
    {
        std::printf("%s: %s\n", condition ? "ok  " : "FAIL", what);
        return condition;
    }

    //!< Returns a free TCP port of the loopback interface.
    uint16_t free_port()
    {
        const SOCKETHANDLE probe{ areg::socket_create() };
        sockaddr_in bound{ };
        bound.sin_family = AF_INET;
        bound.sin_port   = 0u;
        ::inet_pton(AF_INET, "127.0.0.1", &bound.sin_addr);
        socklen_t length{ sizeof(bound) };
        uint16_t result{ 0u };
        if ( areg::is_valid_socket(probe)
          && (::bind(probe, reinterpret_cast<sockaddr *>(&bound), sizeof(bound)) == 0)
          && (::getsockname(probe, reinterpret_cast<sockaddr *>(&bound), &length) == 0) )
        {
            result = ntohs(bound.sin_port);
        }

        if (areg::is_valid_socket(probe))
            areg::socket_close(probe);

        return result;
    }

    //!< Waits until the socket is readable. Returns false on timeout or error.
    bool wait_readable(SOCKETHANDLE hSocket, int32_t timeoutMs)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(hSocket, &reads);
        timeval timeout{ };
        timeout.tv_sec  = timeoutMs / 1'000;
        timeout.tv_usec = (timeoutMs % 1'000) * 1'000;
        return (::select(static_cast<int>(hSocket) + 1, &reads, nullptr, nullptr, &timeout) > 0);
    }

    //!< Reads exactly \a length bytes. Returns false on error, end of stream or timeout.
    bool read_exact(SOCKETHANDLE hSocket, uint8_t * buffer, uint32_t length, int32_t timeoutMs)
    {
        uint32_t done{ 0u };
        while (done < length)
        {
            if (wait_readable(hSocket, timeoutMs) == false)
                return false;

            const int received{ static_cast<int>(::recv(hSocket, reinterpret_cast<char *>(buffer + done), static_cast<int>(length - done), 0)) };
            if (received <= 0)
                return false;

            done += static_cast<uint32_t>(received);
        }

        return true;
    }

    //!< Sends the wire bytes of a message. Returns false on error.
    bool send_envelope(SOCKETHANDLE hSocket, const areg::MessageEnvelope & msg)
    {
        const areg::EventHeader * hdr{ msg.header() };
        if (hdr == nullptr)
            return false;

        const uint32_t size{ areg::wire_size(*hdr) };
        return (areg::send_data(hSocket, areg::wire_data(*hdr), size) == static_cast<int32_t>(size));
    }

    //!< Returns the address of provider \a index of the remote peer, as a router announces it.
    areg::StubAddress peer_stub(uint32_t index)
    {
        constexpr UniqueNumber PEER_THREAD{ 0x100u };
        const UniqueNumber service{ areg::crc32_calculate("ClientStopFlood") };
        const UniqueNumber role{ static_cast<UniqueNumber>(areg::crc32_calculate("floodRole")) + index };
        areg::StubAddress stub(service, areg::Version(1, 0, 0), areg::ServiceType::Public, role, PEER_THREAD, 0x5A5A0000u + index);
        stub.set_channel(areg::Channel(static_cast<uint32_t>(PEER_THREAD), static_cast<uint32_t>(areg::COOKIE_ROUTER), static_cast<uint32_t>(PEER_COOKIE)));
        return stub;
    }

    //!< A router that accepts the client, answers its connect request and then acts by its mode.
    class FakeRouter
    {
    public:
        enum class Mode
        {
              Idle      //!< Sends nothing more.
            , Flood     //!< Sends registration notifications until the client goes.
            , Cut       //!< Sends the header and part of the body of a large message, then nothing.
        };

        explicit FakeRouter(Mode mode)
            : mMode     (mode)
            , mPort     (0u)
            , mListener (areg::InvalidSocketHandle)
            , mStop     (false)
            , mAccepted (0)
            , mClosed   (0)
            , mThread   ( )
        {
        }

        ~FakeRouter()
        {
            stop();
        }

        //!< Listens on a free port and starts serving. Returns false if it cannot listen.
        bool start()
        {
            for (int attempt = 0; (attempt < 20) && (areg::is_valid_socket(mListener) == false); ++attempt)
            {
                mPort = free_port();
                const SOCKETHANDLE listener{ areg::server_connect(areg::String("127.0.0.1"), mPort) };
                if (areg::is_valid_socket(listener) && areg::server_listen(listener))
                {
                    mListener = listener;
                }
                else if (areg::is_valid_socket(listener))
                {
                    areg::socket_close(listener);
                }
            }

            if (areg::is_valid_socket(mListener) == false)
                return false;

            mThread = std::thread([this]() { run(); });
            return true;
        }

        //!< Stops serving and closes every socket.
        void stop()
        {
            mStop.store(true);
            if (mThread.joinable())
            {
                mThread.join();
            }

            if (areg::is_valid_socket(mListener))
            {
                areg::socket_close(mListener);
                mListener = areg::InvalidSocketHandle;
            }
        }

        uint16_t port() const { return mPort; }

        //!< The number of connections accepted.
        int accepted() const { return mAccepted.load(); }

        //!< The number of connections the client ended.
        int closed() const { return mClosed.load(); }

    private:
        void run()
        {
            while (mStop.load() == false)
            {
                if (wait_readable(mListener, 50) == false)
                    continue;

                const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                const SOCKETHANDLE client{ areg::server_accept(mListener, &none, 0) };
                if (areg::is_valid_socket(client) == false)
                    continue;

                mAccepted.fetch_add(1);
                serve(client);
                areg::socket_close(client);
            }
        }

        //!< Serves one connection until the client ends it or the router stops.
        void serve(SOCKETHANDLE client)
        {
            areg::EventHeader header{ };
            if (read_exact(client, areg::wire_header(header), areg::WIRE_HEADER_SIZE, 3'000) == false)
                return;

            std::vector<uint8_t> body(header.bufHeader.biUsed);
            if ((body.empty() == false) && (read_exact(client, body.data(), static_cast<uint32_t>(body.size()), 3'000) == false))
                return;

            if (send_envelope(client, areg::create_connect_notify(areg::COOKIE_ROUTER, CLIENT_COOKIE)) == false)
                return;

            if (mMode == Mode::Cut)
            {
                send_cut(client);
            }

            uint32_t sequence{ 0u };
            std::vector<uint8_t> scratch(64u * 1024u);
            while (mStop.load() == false)
            {
                if ((mMode == Mode::Flood) && (send_flood(client, sequence) == false))
                {
                    mClosed.fetch_add(1);
                    return;
                }

                if (wait_readable(client, mMode == Mode::Flood ? 0 : 50))
                {
                    const int received{ static_cast<int>(::recv(client, reinterpret_cast<char *>(scratch.data()), static_cast<int>(scratch.size()), 0)) };
                    if (received <= 0)
                    {
                        mClosed.fetch_add(1);
                        return;
                    }
                }
            }
        }

        //!< Sends a batch of registration notifications. Returns false once the client is gone.
        bool send_flood(SOCKETHANDLE client, uint32_t & sequence)
        {
            for (int i = 0; i < 64; ++i, ++sequence)
            {
                const areg::StubAddress stub{ peer_stub(sequence % 32u) };
                const areg::MessageEnvelope msg{ ((sequence / 32u) % 2u) == 0u
                                                 ? areg::service_registered_event(stub, areg::COOKIE_ROUTER, CLIENT_COOKIE)
                                                 : areg::service_unregistered_event(stub, areg::DisconnectReason::ProviderDisconnected, areg::COOKIE_ROUTER, CLIENT_COOKIE) };
                if (send_envelope(client, msg) == false)
                    return false;
            }

            return true;
        }

        //!< Sends the header of a message whose body is CUT_BODY bytes, and only the first part of the body.
        void send_cut(SOCKETHANDLE client)
        {
            const areg::StubAddress stub{ peer_stub(0u) };
            const areg::MessageEnvelope msg{ areg::service_registered_event(stub, areg::COOKIE_ROUTER, CLIENT_COOKIE) };
            const areg::EventHeader * hdr{ msg.header() };
            if (hdr == nullptr)
                return;

            std::vector<uint8_t> wire(areg::WIRE_HEADER_SIZE + 64u * 1024u, static_cast<uint8_t>(0x5A));
            std::memcpy(wire.data(), areg::wire_data(*hdr), areg::WIRE_HEADER_SIZE);
            std::memcpy(wire.data(), &CUT_BODY, sizeof(CUT_BODY));
            static_cast<void>(areg::send_data(client, wire.data(), static_cast<uint32_t>(wire.size())));
        }

        const Mode              mMode;
        uint16_t                mPort;
        SOCKETHANDLE            mListener;
        std::atomic_bool        mStop;
        std::atomic_int         mAccepted;
        std::atomic_int         mClosed;
        std::thread             mThread;
    };

    //!< Waits until the router state is \a state. Returns false on timeout.
    bool wait_state(areg::RouterState state, int64_t timeoutMs)
    {
        const Clock::time_point start{ Clock::now() };
        while (areg::Application::router_state() != state)
        {
            if (elapsed_ms(start) > timeoutMs)
                return false;

            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        return true;
    }

    //!< Sets up the application and connects it to the fake router.
    bool setup_and_connect(const FakeRouter & router)
    {
        areg::Application::setup(false, true, false, true, false, nullptr);
        areg::ConfigManager & config{ areg::Application::config_manager() };
        config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, true, true);
        config.set_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, areg::String("127.0.0.1"), true);
        config.set_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, router.port(), true);
        areg::Application::connect_router();
        return check(wait_state(areg::RouterState::Connected, CONNECT_MS), "the client connects to the fake router");
    }

    //!< Releases the application and checks that it took less than \a limitMs.
    bool release_within(int64_t limitMs)
    {
        const Clock::time_point start{ Clock::now() };
        areg::Application::release();
        const int64_t took{ elapsed_ms(start) };
        std::printf("release took %lld ms\n", static_cast<long long>(took));
        return check(took < limitMs, "release() ends in time");
    }
}

class ProbeComponent : public areg::Component
{
public:
    static areg::Component * create(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        return new ProbeComponent(entry, owner);
    }

    static void destroy(areg::Component & item, const areg::ComponentEntry &)
    {
        delete (&item);
    }

    ProbeComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
    {
    }
};

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD(_thread)
        BEGIN_REGISTER_COMPONENT(_role, ProbeComponent)
        END_REGISTER_COMPONENT(_role)
    END_REGISTER_THREAD(_thread)
END_MODEL(_modelName)

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    const std::string name{ argc > 1 ? argv[1] : "" };

    FakeRouter::Mode mode{ FakeRouter::Mode::Idle };
    if ((name == "release-flood") || (name == "disconnect-flood-load"))
        mode = FakeRouter::Mode::Flood;
    else if (name == "release-mid-message")
        mode = FakeRouter::Mode::Cut;
    else if ((name != "release-idle") && (name != "disconnect-reconnect"))
    {
        std::printf("usage: %s release-idle | release-flood | release-mid-message | disconnect-reconnect | disconnect-flood-load\n", argv[0]);
        return 2;
    }

    areg::socket_initialize();
    FakeRouter router(mode);
    if (check(router.start(), "the fake router listens") == false)
        return 1;

    bool ok{ setup_and_connect(router) };
    if (name == "release-idle")
    {
        ok = release_within(2'000) && ok;
        const Clock::time_point start{ Clock::now() };
        while ((router.closed() == 0) && (elapsed_ms(start) < 2'000))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        ok = check(router.closed() == 1, "the fake router sees the client close") && ok;
    }
    else if (name == "release-flood")
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ok = release_within(3'000) && ok;
    }
    else if (name == "release-mid-message")
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        ok = release_within(2'000) && ok;
    }
    else if (name == "disconnect-reconnect")
    {
        areg::Application::disconnect_router();
        bool connecting{ false };
        const Clock::time_point start{ Clock::now() };
        while ((areg::Application::router_state() == areg::RouterState::Connected) && (elapsed_ms(start) < 3'000))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        connecting = (areg::Application::router_state() == areg::RouterState::Connecting);
        ok = check(connecting == false, "a stopping connection does not report Connecting") && ok;
        areg::Application::connect_router();
        ok = check(wait_state(areg::RouterState::Connected, CONNECT_MS), "the client connects again") && ok;
        ok = check(router.accepted() == 2, "the fake router accepted two connections") && ok;
        ok = release_within(2'000) && ok;
    }
    else
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        const Clock::time_point start{ Clock::now() };
        areg::Application::disconnect_router();
        const bool loaded{ areg::Application::load_model(_modelName, true) };
        const int64_t took{ elapsed_ms(start) };
        std::printf("model load during the stop took %lld ms\n", static_cast<long long>(took));
        ok = check(loaded, "the model loads during the stop") && ok;
        ok = check(took < 2'000, "the service manager serves the load during the stop") && ok;
        ok = release_within(3'000) && ok;
    }

    router.stop();
    std::printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
