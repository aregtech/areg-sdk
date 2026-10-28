/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        router-io/router_io_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Runs the message router service in this process and drives it with raw TCP
 *              clients: a client that misbehaves must never block or damage another one.
 *
 *              Usage: areg-router-io-test <case> <pairs> <bin-dir>
 *              Cases: stale-cache, wrong-client, partial-header, partial-body, invalid-type, oversize,
 *              slow-reader, stop-while-flooding.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/EventDefs.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "mtrouter/service/RouterServerService.hpp"

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else   // !defined(_WIN32)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/resource.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif  // defined(_WIN32)

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace
{
    //!< Milliseconds a client waits for an answer of the router before the case fails.
    constexpr int32_t   ANSWER_MS       { 3'000 };

    //!< Milliseconds the router is given to start listening.
    constexpr int32_t   START_MS        { 5'000 };

    //!< The message ID of the data messages the clients exchange: a request, forwarded as is.
    constexpr uint32_t  DATA_MESSAGE    { static_cast<uint32_t>(areg::ServiceCallType::RequestFunction) | 1u };

    //!< The port of the router of this run.
    uint16_t            _port           { 0u };

    //!< The send buffer limit of the router in the slow-reader case, in kilobytes.
    constexpr uint32_t  SLOW_SNDBUF_KB  { 256u };

    //!< Writes the result of a check and returns it.
    bool check(bool condition, const char * what)
    {
        std::printf("%s: %s\n", condition ? "ok  " : "FAIL", what);
        std::fflush(stdout);
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

    //!< Writes the configuration of the router of this run beside the installed one.
    std::string write_config(const std::string & binDir, const std::string & name, uint32_t pairs)
    {
        std::ifstream source(binDir + "/config/areg.init");
        std::ostringstream out;
        std::string line;
        while (std::getline(source, line))
        {
            if (line.rfind("router::*::address::tcpip", 0) == 0)
                line = "router::*::address::tcpip = 127.0.0.1";
            else if (line.rfind("router::*::port::tcpip", 0) == 0)
                line = "router::*::port::tcpip = " + std::to_string(_port);
            else if (line.rfind("net::*::tcpip::pairs", 0) == 0)
                line = "net::*::tcpip::pairs = " + std::to_string(pairs);
            else if ((name == "slow-reader") && (line.rfind("net::*::tcpip::sndbuf", 0) == 0))
                line = "net::*::tcpip::sndbuf = " + std::to_string(SLOW_SNDBUF_KB);

            out << line << "\n";
        }

        const std::string path{ binDir + "/config/router-io-" + name + "-" + std::to_string(pairs) + ".init" };
        std::ofstream target(path, std::ios::trunc);
        target << out.str();
        return (source.is_open() && target.good()) ? path : std::string();
    }

    //!< Waits until the socket is readable. Returns false on timeout or error.
    bool wait_readable(SOCKETHANDLE hSocket, int32_t timeoutMs)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(hSocket, &reads);
        timeval timeout{ timeoutMs / 1'000, (timeoutMs % 1'000) * 1'000 };
        return (::select(static_cast<int>(hSocket) + 1, &reads, nullptr, nullptr, &timeout) > 0);
    }

    //!< Reads exactly length bytes. Returns false on timeout, error or end of stream.
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

    //!< Writes all bytes. Returns false on error.
    bool write_all(SOCKETHANDLE hSocket, const uint8_t * buffer, uint32_t length)
    {
        uint32_t done{ 0u };
        while (done < length)
        {
            const int sent{ static_cast<int>(::send(hSocket, reinterpret_cast<const char *>(buffer + done), static_cast<int>(length - done), 0)) };
            if (sent <= 0)
                return false;

            done += static_cast<uint32_t>(sent);
        }

        return true;
    }

    //!< Returns true when the peer closed the connection within the timeout.
    bool sees_end(SOCKETHANDLE hSocket, int32_t timeoutMs)
    {
        uint8_t buffer[4096];
        while (wait_readable(hSocket, timeoutMs))
        {
            if (::recv(hSocket, reinterpret_cast<char *>(buffer), sizeof(buffer), 0) <= 0)
                return true;
        }

        return false;
    }

    //!< The wire bytes of a message: the header and its payload.
    std::vector<uint8_t> wire_of(areg::MessageEnvelope & message)
    {
        const areg::EventHeader * hdr{ message.header() };
        const uint8_t * bytes{ areg::wire_data(*hdr) };
        return std::vector<uint8_t>(bytes, bytes + areg::wire_size(*hdr));
    }

    //!< The wire bytes of a data message from source to target with a payload of the given size.
    std::vector<uint8_t> data_message(ITEM_ID source, ITEM_ID target, uint32_t size, uint8_t fill)
    {
        areg::EventHeader hdr{ };
        hdr.bufHeader.biUsed    = size;
        hdr.source              = static_cast<uint32_t>(source);
        hdr.target              = static_cast<uint32_t>(target);
        hdr.messageId           = DATA_MESSAGE;
        hdr.eventType           = static_cast<uint16_t>(areg::EventType::EventRemoteRequest);
        std::vector<uint8_t> result(areg::wire_size(hdr), fill);
        std::memcpy(result.data(), areg::wire_data(hdr), areg::WIRE_HEADER_SIZE);
        return result;
    }

    //!< A data message carrying its sequence number in the first 4 bytes of the payload.
    std::vector<uint8_t> numbered_message(ITEM_ID source, ITEM_ID target, uint32_t size, uint32_t sequence)
    {
        std::vector<uint8_t> result{ data_message(source, target, size, 0x3Cu) };
        std::memcpy(result.data() + areg::WIRE_HEADER_SIZE, &sequence, sizeof(sequence));
        return result;
    }

    //!< A raw TCP client of the router.
    struct Client
    {
        SOCKETHANDLE    socket  { areg::InvalidSocketHandle };
        ITEM_ID         cookie  { areg::COOKIE_UNKNOWN };

        Client() = default;
        Client(const Client &) = delete;
        Client & operator = (const Client &) = delete;

        ~Client()
        {
            close();
        }

        //!< Creates the socket, without connecting it.
        bool create()
        {
            socket = areg::socket_create();
            return areg::is_valid_socket(socket);
        }

        //!< Connects the created socket to the router.
        bool connect()
        {
            areg::SocketAddress address{ };
            return address.resolve_address(areg::String("127.0.0.1"), _port, false) && areg::client_connect_fd(socket, address);
        }

        //!< Sends the connect request and reads the cookie the router assigned.
        bool handshake()
        {
            areg::MessageEnvelope request{ areg::create_connect_request(areg::COOKIE_UNKNOWN, areg::COOKIE_ROUTER, areg::MessageSource::SourceClient) };
            const std::vector<uint8_t> bytes{ wire_of(request) };
            if (write_all(socket, bytes.data(), static_cast<uint32_t>(bytes.size())) == false)
                return false;

            areg::EventHeader hdr{ };
            if (read_message(hdr, ANSWER_MS) == false)
                return false;

            cookie = static_cast<ITEM_ID>(hdr.target);
            return (hdr.messageId == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection)) && (cookie >= areg::COOKIE_REMOTE_SERVICE);
        }

        //!< Reads one message and keeps its header. The payload is read and dropped.
        bool read_message(areg::EventHeader & hdr, int32_t timeoutMs)
        {
            std::vector<uint8_t> payload;
            return read_message(hdr, payload, timeoutMs);
        }

        //!< Reads one message, its header and its payload.
        bool read_message(areg::EventHeader & hdr, std::vector<uint8_t> & payload, int32_t timeoutMs)
        {
            if (read_exact(socket, areg::wire_header(hdr), areg::WIRE_HEADER_SIZE, timeoutMs) == false)
                return false;

            payload.resize(hdr.bufHeader.biUsed);
            return payload.empty() || read_exact(socket, payload.data(), hdr.bufHeader.biUsed, timeoutMs);
        }

        //!< Reads data messages and checks that their numbers run from 0 to count - 1. Counts the
        //!< bytes read in total. While slow is true, it reads one batch, then sleeps.
        bool read_numbered(uint32_t count, std::atomic<uint64_t> & bytesRead, const std::atomic<bool> & slow)
        {
            constexpr uint32_t SLOW_BATCH   { 16u };
            constexpr int32_t  SLOW_PAUSE_MS{ 20 };
            areg::EventHeader hdr{ };
            std::vector<uint8_t> payload;
            for (uint32_t next = 0u; next < count; ++next)
            {
                if ((read_message(hdr, payload, 10 * ANSWER_MS) == false) || (payload.size() < sizeof(uint32_t)))
                    return false;

                uint32_t sequence{ 0u };
                std::memcpy(&sequence, payload.data(), sizeof(sequence));
                if ((hdr.messageId != DATA_MESSAGE) || (sequence != next))
                {
                    std::printf("      expected message [ %u ], got [ %u ] of id [ %u ]\n", next, sequence, hdr.messageId);
                    return false;
                }

                bytesRead.fetch_add(areg::WIRE_HEADER_SIZE + payload.size(), std::memory_order_relaxed);
                if (slow.load(std::memory_order_relaxed) && (((next + 1u) % SLOW_BATCH) == 0u))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(SLOW_PAUSE_MS));
                }
            }

            return true;
        }

        //!< Sends data messages numbered 0 to count - 1 to the target.
        bool send_numbered(ITEM_ID target, uint32_t count, uint32_t size)
        {
            constexpr uint32_t PER_WRITE{ 64u };
            std::vector<uint8_t> bytes;
            for (uint32_t i = 0u; i < count; )
            {
                bytes.clear();
                for (const uint32_t end{ (std::min)(count, i + PER_WRITE) }; i < end; ++i)
                {
                    const std::vector<uint8_t> one{ numbered_message(cookie, target, size, i) };
                    bytes.insert(bytes.end(), one.begin(), one.end());
                }

                if (send(bytes) == false)
                    return false;
            }

            return true;
        }

        //!< Sends batches of data messages to the target until stop is set or a write fails.
        void send_until(ITEM_ID target, uint32_t size, const std::atomic<bool> & stop)
        {
            constexpr uint32_t PER_WRITE{ 64u };
            std::vector<uint8_t> bytes;
            for (uint32_t i = 0u; i < PER_WRITE; ++i)
            {
                const std::vector<uint8_t> one{ numbered_message(cookie, target, size, i) };
                bytes.insert(bytes.end(), one.begin(), one.end());
            }

            while ((stop.load(std::memory_order_relaxed) == false) && send(bytes))
            {
            }
        }

        //!< Reads messages until the connection ends or nothing arrives within the timeout. Returns the count.
        uint32_t read_until_end()
        {
            uint32_t result{ 0u };
            areg::EventHeader hdr{ };
            std::vector<uint8_t> payload;
            while (read_message(hdr, payload, ANSWER_MS))
            {
                ++result;
            }

            return result;
        }

        bool send(const std::vector<uint8_t> & bytes)
        {
            return write_all(socket, bytes.data(), static_cast<uint32_t>(bytes.size()));
        }

        void close()
        {
            if (areg::is_valid_socket(socket))
                areg::socket_close(socket);

            socket = areg::InvalidSocketHandle;
        }
    };

    //!< Connects a client to the router of this run, waiting until the router listens.
    bool connect_when_listening(Client & client)
    {
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(START_MS) };
        do
        {
            if (client.create() && client.connect())
                return true;

            client.close();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (std::chrono::steady_clock::now() < until);

        return false;
    }

    //!< Connects a client and reads its cookie, retrying until the router listens and answers.
    bool join_when_listening(Client & client)
    {
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(START_MS) };
        do
        {
            if (client.create() && client.connect() && client.handshake())
                return true;

            client.close();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (std::chrono::steady_clock::now() < until);

        return false;
    }

    //!< A connection closed by the router leaves no received bytes for the next client.
    bool case_stale_cache()
    {
        Client first;
        bool result{ check(connect_when_listening(first) && first.handshake(), "first client connected") };

        Client second;
        result = result && check(second.create(), "second client socket created before the first one leaves");

        // One write: the disconnect, then half of a message the router reads with it and keeps.
        areg::MessageEnvelope bye{ areg::create_disconnect_request(first.cookie, areg::COOKIE_ROUTER) };
        std::vector<uint8_t> bytes{ wire_of(bye) };
        const std::vector<uint8_t> half{ data_message(first.cookie, areg::TARGET_UNKNOWN, 64u, 0x5Au) };
        bytes.insert(bytes.end(), half.begin(), half.begin() + areg::WIRE_HEADER_SIZE + 20u);
        result = result && check(first.send(bytes), "first client sent its disconnect and half of a message");
        result = result && check(sees_end(first.socket, ANSWER_MS), "the router closed the first client");

        result = result && check(second.connect() && second.handshake(), "second client connected and got its cookie");
        return result;
    }

    //!< A client connected right after another one left never gets the messages of that one.
    bool case_wrong_client()
    {
        constexpr uint32_t ROUNDS   { 100u };
        constexpr uint32_t FLOOD    { 400u };
        constexpr uint32_t SIZE     { 1'024u };

        Client sender;
        bool result{ check(connect_when_listening(sender) && sender.handshake(), "sender connected") };
        uint32_t wrong{ 0u };
        for (uint32_t round = 0u; result && (round < ROUNDS); ++round)
        {
            Client leaving;
            Client next;
            result = leaving.create() && leaving.connect() && leaving.handshake() && next.create();
            if (result == false)
                break;

            std::vector<uint8_t> flood;
            for (uint32_t i = 0u; i < FLOOD; ++i)
            {
                const std::vector<uint8_t> one{ data_message(sender.cookie, leaving.cookie, SIZE, static_cast<uint8_t>(i)) };
                flood.insert(flood.end(), one.begin(), one.end());
            }

            std::thread writer([&sender, &flood]() { static_cast<void>(sender.send(flood)); });
            leaving.close();
            result = next.connect() && next.handshake();
            writer.join();
            if (result == false)
                break;

            result = sender.send(data_message(sender.cookie, next.cookie, 8u, 0xEEu));
            areg::EventHeader hdr{ };
            while (result && (result = next.read_message(hdr, ANSWER_MS)) && (hdr.bufHeader.biUsed != 8u))
            {
                wrong += (static_cast<ITEM_ID>(hdr.target) != next.cookie) ? 1u : 0u;
            }

            wrong += (result && (static_cast<ITEM_ID>(hdr.target) != next.cookie)) ? 1u : 0u;
        }

        result = check(result, "every round connected, sent and got its marker") && result;
        std::printf("      messages of a closed client received by the next one: %u\n", wrong);
        return check(wrong == 0u, "no client received a message of another client") && result;
    }

    //!< A message without an event type closes the connection of its sender, not the router.
    bool case_invalid_type()
    {
        Client sender;
        Client target;
        bool result{ check(connect_when_listening(sender) && sender.handshake(), "sender connected") };
        result = result && check(target.create() && target.connect() && target.handshake(), "target connected");

        std::vector<uint8_t> bytes{ data_message(sender.cookie, target.cookie, 16u, 0x44u) };
        areg::EventHeader hdr{ };
        std::memcpy(areg::wire_header(hdr), bytes.data(), areg::WIRE_HEADER_SIZE);
        hdr.eventType = 0u;
        std::memcpy(bytes.data(), areg::wire_data(hdr), areg::WIRE_HEADER_SIZE);
        result = result && check(sender.send(bytes), "sender sent a message without an event type");
        result = result && check(sees_end(sender.socket, ANSWER_MS), "the router closed the sender");

        Client other;
        result = result && check(other.create() && other.connect() && other.handshake(), "another client connected and got its cookie");
        return result;
    }

    //!< A client that stops in the middle of a message does not stop the router serving another one.
    bool case_partial(uint32_t pairs, bool inBody)
    {
        Client stalled;
        bool result{ check(connect_when_listening(stalled), "stalled client connected") };
        if (inBody)
        {
            result = result && check(stalled.handshake(), "stalled client got its cookie");
        }

        // In pool mode a client is served by the slot cookie % pairs: fill the slots in between.
        std::vector<Client> fillers(pairs > 1u ? pairs - 1u : 0u);
        for (Client & filler : fillers)
        {
            result = result && filler.create() && filler.connect();
        }

        const std::vector<uint8_t> message{ inBody ? data_message(stalled.cookie, areg::TARGET_UNKNOWN, 1'000u, 0x11u) : data_message(0u, 0u, 0u, 0u) };
        const uint32_t cut{ inBody ? areg::WIRE_HEADER_SIZE + 10u : 10u };
        result = result && check(write_all(stalled.socket, message.data(), cut), inBody ? "stalled client sent a header and 10 bytes of its body" : "stalled client sent 10 bytes of a header");

        Client other;
        result = result && check(other.create() && other.connect() && other.handshake(), "another client connected and got its cookie");
        return result;
    }

    //!< A client that reads slower than its sender writes holds back that sender only. Another
    //!< client of the same send thread gets its messages at once, and nothing is lost.
    bool case_slow_reader(uint32_t pairs)
    {
        constexpr uint32_t SIZE         { 1'024u };
        constexpr uint32_t SLOW_COUNT   { 8'192u };
        constexpr uint32_t FAST_COUNT   { 4'096u };
        constexpr uint64_t SLOW_LIMIT   { 1024u * 1024u };

        // A small receive window makes the slow reader take only what it reads.
        Client slow;
        bool result{ check(connect_when_listening(slow), "slow reader connected") };
        static_cast<void>(areg::set_recv_size(slow.socket, 64u * 1024u));
        result = result && check(slow.handshake(), "slow reader got its cookie");

        // In pool mode both readers are served by the slot cookie % pairs: fill the slots in between.
        std::vector<Client> fillers(pairs > 1u ? pairs - 1u : 0u);
        for (Client & filler : fillers)
        {
            result = result && filler.create() && filler.connect() && filler.handshake();
        }

        Client fast;
        Client slowSender;
        Client fastSender;
        result = result && check(fast.create() && fast.connect() && fast.handshake(), "fast reader connected");
        result = result && check(slowSender.create() && slowSender.connect() && slowSender.handshake(), "sender of the slow reader connected");
        result = result && check(fastSender.create() && fastSender.connect() && fastSender.handshake(), "sender of the fast reader connected");
        result = result && check((pairs == 0u) || ((slow.cookie % pairs) == (fast.cookie % pairs)), "both readers share one send thread");
        if (result == false)
            return false;

        std::atomic<bool> slowMode{ true };
        std::atomic<bool> neverSlow{ false };
        std::atomic<uint64_t> slowRead{ 0u };
        std::atomic<uint64_t> fastRead{ 0u };
        bool slowOk{ false };
        bool slowSent{ false };
        std::thread slowReader([&]() { slowOk = slow.read_numbered(SLOW_COUNT, slowRead, slowMode); });
        std::thread slowWriter([&]() { slowSent = slowSender.send_numbered(slow.cookie, SLOW_COUNT, SIZE); });

        // The slow reader is held once its sender wrote past the cap of the router.
        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        const uint64_t slowBefore{ slowRead.load() };
        bool fastOk{ false };
        bool fastSent{ false };
        const auto started{ std::chrono::steady_clock::now() };
        std::thread fastReader([&]() { fastOk = fast.read_numbered(FAST_COUNT, fastRead, neverSlow); });
        std::thread fastWriter([&]() { fastSent = fastSender.send_numbered(fast.cookie, FAST_COUNT, SIZE); });
        fastReader.join();
        const uint64_t slowDuring{ slowRead.load() - slowBefore };
        const auto fastMs{ std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - started).count() };

        // The slow reader reads at full speed now: everything its sender wrote arrives.
        slowMode.store(false);
        fastWriter.join();
        slowReader.join();
        slowWriter.join();

        std::printf("      fast reader: %u messages in %lld ms; the slow reader read %llu bytes meanwhile\n"
                    , FAST_COUNT
                    , static_cast<long long>(fastMs)
                    , static_cast<unsigned long long>(slowDuring));
        result = check(fastSent && fastOk, "the fast reader got all its messages, in order") && result;
        result = check(slowDuring < SLOW_LIMIT, "the fast reader did not wait for the slow one") && result;
        result = check(slowSent && slowOk, "the slow reader got all its messages, in order, none lost") && result;
        return result;
    }

    //!< The router restarts while its clients flood each other, and serves new clients after each restart.
    bool case_stop_while_flooding(RouterServerService & router, uint32_t pairs)
    {
        constexpr uint32_t ROUNDS   { 20u };
        constexpr uint32_t SIZE     { 256u };
        const uint32_t clients{ (pairs > 1u ? pairs : 2u) * 2u };

        bool result{ true };
        uint64_t forwarded{ 0u };
        for (uint32_t round = 0u; result && (round < ROUNDS); ++round)
        {
            std::vector<Client> ring(clients);
            for (Client & client : ring)
            {
                result = result && join_when_listening(client);
            }

            if (check(result, "every client of the round connected") == false)
                break;

            std::atomic<bool> stop{ false };
            std::vector<uint32_t> received(clients, 0u);
            std::vector<std::thread> threads;
            for (uint32_t i = 0u; i < clients; ++i)
            {
                Client & client{ ring[i] };
                const ITEM_ID target{ ring[(i + 1u) % clients].cookie };
                threads.emplace_back([&client, target, &stop]() { client.send_until(target, SIZE, stop); });
                threads.emplace_back([&client, &received, i]() { received[i] = client.read_until_end(); });
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20 + 7 * (round % 5u)));
            result = router.reconnect_service_host();
            stop.store(true);
            for (std::thread & thread : threads)
            {
                thread.join();
            }

            for (uint32_t count : received)
            {
                forwarded += count;
            }
        }

        std::printf("      messages forwarded before the restarts: %llu\n", static_cast<unsigned long long>(forwarded));
        result = check(result, "the router restarted every round") && result;
        result = check(forwarded != 0u, "the clients exchanged messages before the restarts") && result;

        Client last;
        return check(join_when_listening(last), "a client connected after the last restart") && result;
    }

    //!< A message the router has no memory for closes that one connection, not the router.
    bool case_oversize()
    {
#if defined(__linux__) && !defined(__SANITIZE_ADDRESS__) && !defined(__SANITIZE_THREAD__)
        Client first;
        bool result{ check(connect_when_listening(first) && first.handshake(), "first client connected") };

        long pages{ 0 };
        std::ifstream statm("/proc/self/statm");
        statm >> pages;
        rlimit previous{ };
        ::getrlimit(RLIMIT_AS, &previous);
        rlimit limited{ previous };
        limited.rlim_cur = static_cast<rlim_t>(pages) * static_cast<rlim_t>(::sysconf(_SC_PAGESIZE)) + (256ull << 20);
        result = result && check(::setrlimit(RLIMIT_AS, &limited) == 0, "address space limited to the present use + 256 MB");

        Client huge;
        result = result && huge.create() && huge.connect() && huge.handshake();
        const std::vector<uint8_t> header{ data_message(huge.cookie, first.cookie, 0u, 0u) };
        areg::EventHeader hdr{ };
        std::memcpy(areg::wire_header(hdr), header.data(), areg::WIRE_HEADER_SIZE);
        hdr.bufHeader.biUsed = 768u << 20;
        result = result && check(write_all(huge.socket, areg::wire_data(hdr), areg::WIRE_HEADER_SIZE), "a client sent a header of a 768 MB message");
        result = result && check(sees_end(huge.socket, ANSWER_MS), "the router closed that client");

        static_cast<void>(::setrlimit(RLIMIT_AS, &previous));
        Client other;
        result = result && check(other.create() && other.connect() && other.handshake(), "another client connected and got its cookie");
        return result;
#else
        std::printf("skip: needs Linux without a sanitizer (address space limit)\n");
        return true;
#endif
    }
}

int main(int argc, char * argv[])
{
    if (argc < 4)
    {
        std::printf("usage: areg-router-io-test <stale-cache|wrong-client|partial-header|partial-body|invalid-type|oversize|slow-reader|stop-while-flooding> <pairs> <bin-dir>\n");
        return 2;
    }

    const std::string name{ argv[1] };
    const uint32_t pairs{ static_cast<uint32_t>(std::atoi(argv[2])) };
    static_cast<void>(areg::socket_initialize());
    _port = free_port();
    const std::string config{ write_config(argv[3], name, pairs) };
    if ((_port == 0u) || config.empty())
    {
        std::printf("FAIL: no free port or no configuration\n");
        return 1;
    }

    areg::Application::setup(false, true, false, true, false, config.c_str());
    bool result{ false };
    {
        RouterServerService router;
        if (router.setup_connection_data(areg::RemoteServiceKind::Router, static_cast<uint32_t>(areg::ConnectionType::Tcpip)) && router.connect_service_host())
        {
            if (name == "stale-cache")
                result = case_stale_cache();
            else if (name == "wrong-client")
                result = case_wrong_client();
            else if (name == "partial-header")
                result = case_partial(pairs, false);
            else if (name == "partial-body")
                result = case_partial(pairs, true);
            else if (name == "invalid-type")
                result = case_invalid_type();
            else if (name == "oversize")
                result = case_oversize();
            else if (name == "slow-reader")
                result = case_slow_reader(pairs);
            else if (name == "stop-while-flooding")
                result = case_stop_while_flooding(router, pairs);
            else
                std::printf("FAIL: unknown case [ %s ]\n", name.c_str());

            router.disconnect_service_host();
            router.wait_to_complete();
        }
        else
        {
            std::printf("FAIL: the router did not start\n");
        }
    }

    areg::Application::release();
    areg::socket_release();
    std::remove(config.c_str());
    std::printf("%s %s pairs=%u\n", result ? "PASSED" : "FAILED", name.c_str(), pairs);
    return (result ? 0 : 1);
}
