/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        keepalive-probe/keepalive_probe.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, measures how a connection configured by the framework
 *              notices a dead peer. Driven by keepalive_probe.py, which installs the
 *              packet drop rules. Not a ctest test: it needs root and takes minutes.
 *
 *              report              prints the effective keepalive values of a connected
 *                                  socket, the system defaults and the available options.
 *              deadpeer idle|busy  prints READY, waits for GO, then times until each
 *                                  side of the connection reports an error.
 *              blip <window>       prints READY, waits for GO, then reports whether the
 *                                  idle connection is still usable after <window> seconds.
 *              stall <seconds>     the receiver reads nothing for <seconds> while the sender
 *                                  sends at full rate, then reads again; reports whether the
 *                                  connection survived. Needs no packet drop.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"

#ifdef WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif  // NOMINMAX
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <Mstcpip.h>
#else   // WINDOWS
    #include <arpa/inet.h>
    #include <cerrno>
    #include <fcntl.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <poll.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #if defined(__APPLE__)
        #include <sys/sysctl.h>
    #endif  // __APPLE__
#endif  // WINDOWS

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

namespace
{
#ifdef WINDOWS
    using ProbeSocket = SOCKET;
    using ProbeLen    = int;
    constexpr ProbeSocket PROBE_INVALID { INVALID_SOCKET };
    void close_probe_socket(ProbeSocket s) { ::closesocket(s); }
    int last_error() { return ::WSAGetLastError(); }
#else   // WINDOWS
    using ProbeSocket = int;
    using ProbeLen    = socklen_t;
    constexpr ProbeSocket PROBE_INVALID { -1 };
    void close_probe_socket(ProbeSocket s) { ::close(s); }
    int last_error() { return errno; }
#endif  // WINDOWS

    using Clock = std::chrono::steady_clock;

#if defined(MSG_NOSIGNAL)
    constexpr int MSG_NOSIGNAL_PROBE { MSG_NOSIGNAL };
#else   // MSG_NOSIGNAL
    constexpr int MSG_NOSIGNAL_PROBE { 0 };
#endif  // MSG_NOSIGNAL

    //!< A connected loopback pair and the listener it was accepted from.
    struct Pair
    {
        ProbeSocket listener{ PROBE_INVALID };
        ProbeSocket client  { PROBE_INVALID };
        ProbeSocket server  { PROBE_INVALID };
        uint16_t    port    { 0u };
    };

    //!< Applies to the socket what the framework applies to every connected socket.
    void configure_like_areg(ProbeSocket s)
    {
        areg::set_send_timeout(static_cast<SOCKETHANDLE>(s), areg::SOCKET_SEND_TIMEOUT_MS);
        areg::socket_set_no_delay(static_cast<SOCKETHANDLE>(s));
    }

    bool open_pair(Pair & pair)
    {
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port        = 0;

        pair.listener = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ((pair.listener == PROBE_INVALID) ||
            (::bind(pair.listener, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0) ||
            (::listen(pair.listener, 4) != 0))
        {
            return false;
        }

        ProbeLen len{ static_cast<ProbeLen>(sizeof(addr)) };
        ::getsockname(pair.listener, reinterpret_cast<sockaddr *>(&addr), &len);
        pair.port = ntohs(addr.sin_port);

        pair.client = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ((pair.client == PROBE_INVALID) ||
            (::connect(pair.client, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0))
        {
            return false;
        }

        pair.server = ::accept(pair.listener, nullptr, nullptr);
        if (pair.server == PROBE_INVALID)
        {
            return false;
        }

        configure_like_areg(pair.client);
        configure_like_areg(pair.server);
        return true;
    }

    void close_pair(Pair & pair)
    {
        for (ProbeSocket s : { pair.client, pair.server, pair.listener })
        {
            if (s != PROBE_INVALID)
            {
                close_probe_socket(s);
            }
        }
    }

    void print_int_option(const char * name, ProbeSocket s, int level, int option)
    {
        int value{ 0 };
        ProbeLen len{ static_cast<ProbeLen>(sizeof(value)) };
        if (::getsockopt(s, level, option, reinterpret_cast<char *>(&value), &len) == 0)
        {
            std::printf("socket.%s=%d\n", name, value);
        }
        else
        {
            std::printf("socket.%s=error %d\n", name, last_error());
        }
    }

    void print_defined(const char * name, bool defined)
    {
        std::printf("macro.%s=%s\n", name, defined ? "defined" : "undefined");
    }

#if defined(__linux__)
    void print_file_value(const char * name, const char * path)
    {
        std::ifstream file(path);
        std::string value;
        if (file && std::getline(file, value))
        {
            std::printf("system.%s=%s\n", name, value.c_str());
        }
    }
#endif  // __linux__

#if defined(__APPLE__)
    void print_sysctl(const char * name)
    {
        int value{ 0 };
        size_t len{ sizeof(value) };
        if (::sysctlbyname(name, &value, &len, nullptr, 0) == 0)
        {
            std::printf("system.%s=%d\n", name, value);
        }
    }
#endif  // __APPLE__

    int run_report()
    {
        Pair pair;
        if (!open_pair(pair))
        {
            std::printf("error=cannot open a loopback pair, %d\n", last_error());
            return 2;
        }

#if defined(WINDOWS)
        std::printf("platform=windows\n");
#elif defined(__linux__)
        std::printf("platform=linux\n");
#elif defined(__APPLE__)
        std::printf("platform=macos\n");
#else
        std::printf("platform=other-posix\n");
#endif

        print_int_option("SO_KEEPALIVE", pair.client, SOL_SOCKET, SO_KEEPALIVE);
#if defined(TCP_KEEPIDLE)
        print_int_option("TCP_KEEPIDLE", pair.client, IPPROTO_TCP, TCP_KEEPIDLE);
#endif
#if defined(TCP_KEEPALIVE)
        print_int_option("TCP_KEEPALIVE", pair.client, IPPROTO_TCP, TCP_KEEPALIVE);
#endif
#if defined(TCP_KEEPINTVL)
        print_int_option("TCP_KEEPINTVL", pair.client, IPPROTO_TCP, TCP_KEEPINTVL);
#endif
#if defined(TCP_KEEPCNT)
        print_int_option("TCP_KEEPCNT", pair.client, IPPROTO_TCP, TCP_KEEPCNT);
#endif
#if defined(TCP_USER_TIMEOUT)
        print_int_option("TCP_USER_TIMEOUT", pair.client, IPPROTO_TCP, TCP_USER_TIMEOUT);
#endif
#if defined(TCP_MAXRT)
        print_int_option("TCP_MAXRT", pair.client, IPPROTO_TCP, TCP_MAXRT);
#endif
#if defined(TCP_RXT_CONNDROPTIME)
        print_int_option("TCP_RXT_CONNDROPTIME", pair.client, IPPROTO_TCP, TCP_RXT_CONNDROPTIME);
#endif

#if defined(TCP_KEEPIDLE)
        print_defined("TCP_KEEPIDLE", true);
#else
        print_defined("TCP_KEEPIDLE", false);
#endif
#if defined(TCP_KEEPINTVL)
        print_defined("TCP_KEEPINTVL", true);
#else
        print_defined("TCP_KEEPINTVL", false);
#endif
#if defined(TCP_KEEPCNT)
        print_defined("TCP_KEEPCNT", true);
#else
        print_defined("TCP_KEEPCNT", false);
#endif
#if defined(TCP_USER_TIMEOUT)
        print_defined("TCP_USER_TIMEOUT", true);
#else
        print_defined("TCP_USER_TIMEOUT", false);
#endif
#if defined(TCP_RXT_CONNDROPTIME)
        print_defined("TCP_RXT_CONNDROPTIME", true);
#else
        print_defined("TCP_RXT_CONNDROPTIME", false);
#endif

#if defined(__linux__)
        print_file_value("tcp_keepalive_time", "/proc/sys/net/ipv4/tcp_keepalive_time");
        print_file_value("tcp_keepalive_intvl", "/proc/sys/net/ipv4/tcp_keepalive_intvl");
        print_file_value("tcp_keepalive_probes", "/proc/sys/net/ipv4/tcp_keepalive_probes");
        print_file_value("tcp_retries1", "/proc/sys/net/ipv4/tcp_retries1");
        print_file_value("tcp_retries2", "/proc/sys/net/ipv4/tcp_retries2");
#elif defined(__APPLE__)
        print_sysctl("net.inet.tcp.keepidle");
        print_sysctl("net.inet.tcp.keepintvl");
        print_sysctl("net.inet.tcp.keepcnt");
        print_sysctl("net.inet.tcp.keepinit");
        print_sysctl("net.inet.tcp.always_keepalive");
#endif

        close_pair(pair);
        return 0;
    }

#ifndef WINDOWS

    //!< Returns true if a new connection to the port cannot be established within the timeout.
    bool port_is_blocked(uint16_t port, int timeoutMs)
    {
        ProbeSocket s{ ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) };
        if (s == PROBE_INVALID)
        {
            return false;
        }

        ::fcntl(s, F_SETFL, ::fcntl(s, F_GETFL, 0) | O_NONBLOCK);
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port        = htons(port);

        bool blocked{ false };
        if (::connect(s, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) != 0)
        {
            pollfd pfd{ s, POLLOUT, 0 };
            blocked = (errno == EINPROGRESS) && (::poll(&pfd, 1, timeoutMs) == 0);
        }

        close_probe_socket(s);
        return blocked;
    }

    //!< The time and error with which one side of the pair noticed the dead peer.
    struct Outcome
    {
        std::atomic<bool>   done{ false };
        double              seconds{ 0.0 };
        int                 error{ 0 };
    };

    void report_outcome(const char * scenario, const char * side, const Outcome & outcome, int limitSec)
    {
        if (outcome.done.load())
        {
            std::printf("RESULT scenario=%s side=%s detected=1 seconds=%.2f error=%d (%s)\n",
                        scenario, side, outcome.seconds, outcome.error, std::strerror(outcome.error));
        }
        else
        {
            std::printf("RESULT scenario=%s side=%s detected=0 seconds=%d error=none\n", scenario, side, limitSec);
        }
    }

    //!< Blocks in recv() until the socket fails, which is what an idle side sees.
    void wait_idle(ProbeSocket s, Clock::time_point start, Outcome & outcome)
    {
        char buffer[64];
        for (;;)
        {
            const auto n{ ::recv(s, buffer, sizeof(buffer), 0) };
            if (n <= 0)
            {
                outcome.error   = (n == 0) ? 0 : errno;
                outcome.seconds = std::chrono::duration<double>(Clock::now() - start).count();
                outcome.done.store(true);
                return;
            }
        }
    }

    //!< Sends 64 bytes every 500 ms until a send fails, which is what a side with data in flight sees.
    void keep_sending(ProbeSocket s, Clock::time_point start, Outcome & outcome)
    {
        char buffer[64]{};
        for (;;)
        {
            const auto n{ ::send(s, buffer, sizeof(buffer), MSG_NOSIGNAL_PROBE) };
            if (n < 0)
            {
                outcome.error   = errno;
                outcome.seconds = std::chrono::duration<double>(Clock::now() - start).count();
                outcome.done.store(true);
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    bool wait_for_go()
    {
        std::string line;
        return static_cast<bool>(std::getline(std::cin, line)) && (line == "GO");
    }

    int run_deadpeer(const std::string & scenario, int limitSec)
    {
        const bool busy{ scenario == "busy" };
        if (!busy && (scenario != "idle"))
        {
            std::printf("error=unknown scenario %s\n", scenario.c_str());
            return 1;
        }

        Pair pair;
        if (!open_pair(pair))
        {
            std::printf("error=cannot open a loopback pair, %d\n", errno);
            return 2;
        }

        std::printf("READY port=%u\n", static_cast<unsigned>(pair.port));
        std::fflush(stdout);
        if (!wait_for_go())
        {
            return 1;
        }

        if (!port_is_blocked(pair.port, 1500))
        {
            std::printf("RESULT scenario=%s side=both detected=skipped error=drop-rule-does-not-block-the-port\n", scenario.c_str());
            std::fflush(stdout);
            return 3;
        }

        const auto start{ Clock::now() };
        Outcome client;
        Outcome server;
        std::thread clientThread(busy ? keep_sending : wait_idle, pair.client, start, std::ref(client));
        std::thread serverThread(wait_idle, pair.server, start, std::ref(server));
        clientThread.detach();
        serverThread.detach();

        const auto deadline{ start + std::chrono::seconds(limitSec) };
        while ((Clock::now() < deadline) && !(client.done.load() && server.done.load()))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        report_outcome(scenario.c_str(), busy ? "client-sending" : "client-idle", client, limitSec);
        report_outcome(scenario.c_str(), "server-idle", server, limitSec);
        std::fflush(stdout);
        std::_Exit(0);
    }

    //!< Returns the pending error of the socket, 0 if there is none.
    int pending_error(ProbeSocket s)
    {
        int value{ 0 };
        ProbeLen len{ static_cast<ProbeLen>(sizeof(value)) };
        return (::getsockopt(s, SOL_SOCKET, SO_ERROR, &value, &len) == 0) ? value : errno;
    }

    int run_blip(int windowSec)
    {
        Pair pair;
        if (!open_pair(pair))
        {
            std::printf("error=cannot open a loopback pair, %d\n", errno);
            return 2;
        }

        std::printf("READY port=%u\n", static_cast<unsigned>(pair.port));
        std::fflush(stdout);
        if (!wait_for_go())
        {
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::seconds(windowSec));

        const int clientError{ pending_error(pair.client) };
        const int serverError{ pending_error(pair.server) };
        bool survived{ (clientError == 0) && (serverError == 0) };
        if (survived)
        {
            const char ping{ 'p' };
            char pong{ 0 };
            pollfd pfd{ pair.client, POLLIN, 0 };
            survived = (::send(pair.server, &ping, 1, MSG_NOSIGNAL_PROBE) == 1) &&
                       (::poll(&pfd, 1, 3000) == 1) &&
                       (::recv(pair.client, &pong, 1, 0) == 1) && (pong == ping);
        }

        std::printf("RESULT scenario=blip window=%d survived=%d client_error=%d server_error=%d\n",
                    windowSec, survived ? 1 : 0, clientError, serverError);
        std::fflush(stdout);
        close_pair(pair);
        return 0;
    }

#endif  // !WINDOWS

    //!< Returns the pending error of the socket, 0 if there is none.
    int socket_error(ProbeSocket s)
    {
        int value{ 0 };
        ProbeLen len{ static_cast<ProbeLen>(sizeof(value)) };
        return (::getsockopt(s, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&value), &len) == 0) ? value : last_error();
    }

    //!< True if the error only says that a send waited longer than the send timeout.
    bool is_send_timeout(int error)
    {
#ifdef WINDOWS
        return (error == WSAETIMEDOUT) || (error == WSAEWOULDBLOCK);
#else   // WINDOWS
        return (error == EAGAIN) || (error == EWOULDBLOCK);
#endif  // WINDOWS
    }

    //!< The receiver reads nothing for stallSec seconds while the sender sends as fast as it can,
    //!< then reads again. Reports whether the connection is still usable afterwards.
    int run_stall(int stallSec)
    {
        Pair pair;
        if (!open_pair(pair))
        {
            std::printf("error=cannot open a loopback pair, %d\n", last_error());
            return 2;
        }

        std::atomic_bool stop{ false };
        std::atomic_int  sendError{ 0 };
        std::atomic_int  timeouts{ 0 };
        std::thread sender([&pair, &stop, &sendError, &timeouts]() {
            char buffer[16 * 1024]{};
            while (!stop.load())
            {
                if (::send(pair.client, buffer, static_cast<int>(sizeof(buffer)), MSG_NOSIGNAL_PROBE) < 0)
                {
                    const int error{ last_error() };
                    if (!is_send_timeout(error))
                    {
                        sendError.store(error);
                        return;
                    }

                    timeouts.fetch_add(1);
                }
            }
        });

        std::this_thread::sleep_for(std::chrono::seconds(stallSec));

        int readError{ 0 };
        char buffer[64 * 1024];
        const auto drainEnd{ Clock::now() + std::chrono::seconds(3) };
        while ((Clock::now() < drainEnd) && (sendError.load() == 0))
        {
            if (::recv(pair.server, buffer, static_cast<int>(sizeof(buffer)), 0) <= 0)
            {
                readError = last_error();
                break;
            }
        }

        stop.store(true);
        const int clientError{ sendError.load() != 0 ? sendError.load() : socket_error(pair.client) };
        const int serverError{ readError != 0 ? readError : socket_error(pair.server) };
        std::printf("RESULT scenario=stall seconds=%d survived=%d send_timeouts=%d client_error=%d server_error=%d\n",
                    stallSec, ((clientError == 0) && (serverError == 0)) ? 1 : 0, timeouts.load(), clientError, serverError);
        std::fflush(stdout);
        std::_Exit(0);
    }
}

int main(int argc, char ** argv)
{
    if (!areg::socket_initialize())
    {
        std::printf("error=socket_initialize failed\n");
        return 2;
    }

    const std::string mode{ argc > 1 ? argv[1] : "report" };
    int result{ 1 };
    if (mode == "report")
    {
        result = run_report();
    }
#ifndef WINDOWS
    else if ((mode == "deadpeer") && (argc > 2))
    {
        result = run_deadpeer(argv[2], argc > 3 ? std::atoi(argv[3]) : 1200);
    }
    else if ((mode == "blip") && (argc > 2))
    {
        result = run_blip(std::atoi(argv[2]));
    }
#endif  // !WINDOWS
    else if ((mode == "stall") && (argc > 2))
    {
        result = run_stall(std::atoi(argv[2]));
    }
    else
    {
        std::printf("usage: %s report | deadpeer idle|busy [limit_s] | blip <window_s> | stall <seconds>\n", argv[0]);
    }

    areg::socket_release();
    return result;
}
