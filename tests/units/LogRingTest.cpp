/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogRingTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the remote log queue of a log source that is not
 *              connected to the log collector yet. The test plays the log collector.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/persist/ConfigListener.hpp"
#include "areg/persist/ConfigManager.hpp"

#if AREG_LOGGING

#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else   // !defined(_WIN32)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
#endif  // defined(_WIN32)

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace
{
    //!< Milliseconds the test waits for the log source before it fails.
    constexpr int32_t   WAIT_MS     { 5'000 };

    //!< The size of the remote log queue of the source.
    constexpr uint32_t  QUEUE_SIZE  { 100u };

    //!< The log rows the source writes before it is connected.
    constexpr uint32_t  ROWS        { 250u };

    //!< The cookie the test assigns to the source.
    constexpr ITEM_ID   COOKIE      { 300u };

    //!< The port of the listening socket of the test.
    uint16_t            _port       { 0u };

    //!< Points the logger of the process at the test and sets the size of its remote log queue.
    class RingConfigListener final : public areg::ConfigListener
    {
    protected:
        void prepare_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void post_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void prepare_read_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void on_setup_configuration(const areg::ListProperties& /*listReadonly*/, const areg::ListProperties& /*listWritable*/, areg::ConfigManager& /*config*/) final
        {
        }

        void post_read_configuration(areg::ConfigManager& config) final
        {
            config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true);
            config.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, true, true);
            config.set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, areg::String("127.0.0.1"), true);
            config.set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, _port, true);
            config.set_log_enabled(areg::LogTarget::Remote, true, true);
            config.set_log_enabled(areg::LogTarget::File, false, true);
            config.set_log_enabled(areg::LogTarget::Database, false, true);
            config.set_remote_queue_size(QUEUE_SIZE, true);
        }
    };

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
    bool read_exact(SOCKETHANDLE hSocket, uint8_t * buffer, uint32_t length)
    {
        uint32_t done{ 0u };
        while (done < length)
        {
            if (wait_readable(hSocket, WAIT_MS) == false)
                return false;

            const int received{ static_cast<int>(::recv(hSocket, reinterpret_cast<char *>(buffer + done), static_cast<int>(length - done), 0)) };
            if (received <= 0)
                return false;

            done += static_cast<uint32_t>(received);
        }

        return true;
    }

    //!< Reads one message, its header and its payload.
    bool read_message(SOCKETHANDLE hSocket, areg::EventHeader & hdr, std::vector<uint8_t> & payload)
    {
        if (read_exact(hSocket, areg::wire_header(hdr), areg::WIRE_HEADER_SIZE) == false)
            return false;

        payload.resize(hdr.bufHeader.biUsed);
        return payload.empty() || read_exact(hSocket, payload.data(), hdr.bufHeader.biUsed);
    }

    //!< Writes the wire bytes of the message. Returns false on error.
    bool write_message(SOCKETHANDLE hSocket, areg::MessageEnvelope & message)
    {
        const areg::EventHeader * hdr{ message.header() };
        const uint8_t * bytes{ areg::wire_data(*hdr) };
        const uint32_t length{ areg::wire_size(*hdr) };
        uint32_t done{ 0u };
        while (done < length)
        {
            const int sent{ static_cast<int>(::send(hSocket, reinterpret_cast<const char *>(bytes + done), static_cast<int>(length - done), 0)) };
            if (sent <= 0)
                return false;

            done += static_cast<uint32_t>(sent);
        }

        return true;
    }

    //!< Returns the text of a log row, or an empty string if the payload is not a log row.
    std::string row_text(const areg::EventHeader & hdr, const std::vector<uint8_t> & payload)
    {
        constexpr std::size_t textAt{ offsetof(areg::LogEntry, logMessage) };
        if ((hdr.messageId != static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage)) || (payload.size() <= textAt))
            return std::string();

        const char * text{ reinterpret_cast<const char *>(payload.data() + textAt) };
        return std::string(text, ::strnlen(text, payload.size() - textAt));
    }

    //!< A listening socket of the loopback interface on a free port.
    struct Listener
    {
        SOCKETHANDLE    socket  { areg::InvalidSocketHandle };
        SOCKETHANDLE    client  { areg::InvalidSocketHandle };

        Listener()
        {
            socket = areg::socket_create();
            sockaddr_in bound{ };
            bound.sin_family = AF_INET;
            bound.sin_port   = 0u;
            ::inet_pton(AF_INET, "127.0.0.1", &bound.sin_addr);
            socklen_t length{ sizeof(bound) };
            if ( areg::is_valid_socket(socket)
              && (::bind(socket, reinterpret_cast<sockaddr *>(&bound), sizeof(bound)) == 0)
              && (::listen(socket, 4) == 0)
              && (::getsockname(socket, reinterpret_cast<sockaddr *>(&bound), &length) == 0) )
            {
                _port = ntohs(bound.sin_port);
            }
        }

        ~Listener()
        {
            if (areg::is_valid_socket(client))
                areg::socket_close(client);

            if (areg::is_valid_socket(socket))
                areg::socket_close(socket);
        }

        //!< Accepts the connection of the log source.
        bool accept()
        {
            if (wait_readable(socket, WAIT_MS) == false)
                return false;

            client = static_cast<SOCKETHANDLE>(::accept(socket, nullptr, nullptr));
            return areg::is_valid_socket(client);
        }
    };
}

// Logs written before the connection to the log collector is started are kept up to the size of the
// remote log queue. The older ones are counted and reported in one row, sent after the scopes and
// before the kept logs. The cookie of the source is reported only once the logs are sent directly.
TEST(LogRingTest, logs_before_the_start_are_counted_and_reported)
{
    ASSERT_TRUE(areg::socket_initialize());
    {
        Listener listener;
        ASSERT_NE(_port, 0u);

        RingConfigListener config;
        areg::Application::setup(true, true, false, true, false, nullptr, &config);

        // The source connected and sent its handshake: it is not started until the test answers.
        areg::EventHeader hdr{ };
        std::vector<uint8_t> payload;
        const bool connected{ listener.accept() && read_message(listener.client, hdr, payload) };
        EXPECT_TRUE(connected);
        EXPECT_EQ(areg::cookie(), areg::COOKIE_UNKNOWN);

        for (uint32_t i = 0u; i < ROWS; ++i)
        {
            char text[64]{ };
            const int32_t len{ areg::String::format_string(text, static_cast<int32_t>(sizeof(text)), "ring row %u", i) };
            const areg::LogEntry row(areg::LogMessageType::MessageText, areg::LOG_SCOPE_ID_NONE, 0u, 0u, areg::LogPriority::PrioDebug, text, static_cast<uint32_t>(len));
            areg::log_any_message(row);
        }

        // The answer is queued behind the rows in the logging thread, so all of them meet the queue first.
        areg::MessageEnvelope answer{ areg::create_connect_notify(areg::COOKIE_LOGGER, COOKIE) };
        const bool answered{ connected && write_message(listener.client, answer) };
        EXPECT_TRUE(answered);

        bool scopes{ false };
        bool scopesFirst{ false };
        uint64_t dropped{ 0u };
        std::vector<uint32_t> kept;
        while (answered && (kept.size() < QUEUE_SIZE) && read_message(listener.client, hdr, payload))
        {
            if (hdr.messageId == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes))
            {
                scopes = true;
                continue;
            }

            const std::string text{ row_text(hdr, payload) };
            unsigned long long count{ 0u };
            uint32_t number{ 0u };
            if (std::sscanf(text.c_str(), "Dropped [ %llu ]", &count) == 1)
            {
                scopesFirst = scopes && kept.empty();
                dropped = count;
            }
            else if (std::sscanf(text.c_str(), "ring row %u", &number) == 1)
            {
                kept.push_back(number);
            }
        }

        EXPECT_TRUE(scopesFirst);
        EXPECT_GE(dropped, static_cast<uint64_t>(ROWS - QUEUE_SIZE));
        ASSERT_EQ(kept.size(), static_cast<std::size_t>(QUEUE_SIZE));
        for (uint32_t i = 0u; i < QUEUE_SIZE; ++i)
        {
            EXPECT_EQ(kept[i], ROWS - QUEUE_SIZE + i);
        }

        EXPECT_EQ(areg::cookie(), COOKIE);
        areg::Application::release();
    }

    areg::socket_release();
}

#endif  // AREG_LOGGING
