#ifndef AREG_TESTS_UNITS_LOGCOLLECTORFAKE_HPP
#define AREG_TESTS_UNITS_LOGCOLLECTORFAKE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogCollectorFake.hpp
 * \brief       A log collector on a loopback socket that makes the handshake of a log source,
 *              reads its messages and records the log rows. For tests of the remote log target.
 ************************************************************************/
#include "areg/base/areg_global.h"

#if AREG_LOGGING

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#if defined(_WIN32)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

namespace log_fake
{
    //!< The longest wait of the collector, in milliseconds.
    constexpr int32_t WAIT_MS{ 30'000 };
    //!< The cookie the collector gives the source.
    constexpr ITEM_ID SOURCE_COOKIE{ 380u };

    class RuntimeCollector
    {
    public:
        RuntimeCollector()
            : mInitialized(areg::socket_initialize())
            , mListener(areg::InvalidSocketHandle)
            , mPeer(areg::InvalidSocketHandle)
            , mPoll()
            , mPort(0u)
            , mLogs()
            , mMalformed(0u)
        {
        }

        ~RuntimeCollector()
        {
            close_peer();
            if (areg::is_valid_socket(mListener))
            {
                mPoll->unregister_socket(mListener);
                areg::socket_close(mListener);
            }
            mPoll.reset();
            if (mInitialized)
                areg::socket_release();
        }

        RuntimeCollector(const RuntimeCollector&) = delete;
        RuntimeCollector& operator = (const RuntimeCollector&) = delete;

        bool open(bool listenNow = true)
        {
            if (!mInitialized)
                return false;

            mPoll = std::make_unique<areg::SocketMultiplexer>();
            mListener = areg::socket_create();
            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_port = 0u;
            ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
            socklen_t length{ sizeof(address) };
            if (!areg::is_valid_socket(mListener)
                || (::bind(mListener, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
                || (::getsockname(mListener, reinterpret_cast<sockaddr*>(&address), &length) != 0))
                return false;

            mPort = ntohs(address.sin_port);
            return !listenNow || listen();
        }

        uint16_t port() const { return mPort; }
        const std::vector<std::string>& logs() const { return mLogs; }
        //!< The number of received log messages that are too short to hold a log row.
        uint32_t malformed_rows() const { return mMalformed; }

        bool listen()
        {
            return (::listen(mListener, 4) == 0) && mPoll->register_socket(mListener, true);
        }

        void close_peer()
        {
            if (areg::is_valid_socket(mPeer))
            {
                mPoll->unregister_socket(mPeer);
                areg::socket_interrupt(mPeer);
                areg::socket_close(mPeer);
                mPeer = areg::InvalidSocketHandle;
                mPoll->register_socket(mListener, true);
            }
        }

        bool accept_source()
        {
            if (mPoll->wait(WAIT_MS) != mListener)
                return false;

            mPeer = static_cast<SOCKETHANDLE>(::accept(mListener, nullptr, nullptr));
            if (!areg::is_valid_socket(mPeer))
                return false;

            mPoll->unregister_socket(mListener);
            return mPoll->register_socket(mPeer, true)
                && areg::set_send_timeout(mPeer, static_cast<uint32_t>(WAIT_MS));
        }

        bool send(const areg::MessageEnvelope& message)
        {
            if (!message.is_valid())
                return false;

            const auto& header{ *message.header() };
            return areg::send_data(mPeer, areg::wire_data(header), areg::wire_size(header))
                == static_cast<int32_t>(areg::wire_size(header));
        }

        bool read(areg::MessageEnvelope& message, std::chrono::steady_clock::time_point deadline
            = std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS))
        {
            areg::EventHeader header{};
            if (!read_exact(areg::wire_header(header), areg::WIRE_HEADER_SIZE, deadline))
                return false;

            uint8_t* body{ areg::SocketConnectionBase::prepare_message(message, header) };
            if ((body == nullptr) || !read_exact(body, header.bufHeader.biUsed, deadline))
                return false;

            message.set_size_used(header.bufHeader.biUsed);
            message.move_to_begin();
            if (message.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage))
            {
                if (message.size_used() >= areg::log_entry_head() + 1u)
                {
                    areg::LogEntry entry;
                    message >> entry;
                    mLogs.emplace_back(entry.logMessage, areg::log_message_size(entry));
                    message.move_to_begin();
                }
                else
                {
                    ++mMalformed;
                }
            }
            return true;
        }

        bool next(areg::FuncIdRange id, areg::MessageEnvelope& message)
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            for (uint32_t count = 0u; count < 256u; ++count)
            {
                if (!read(message, deadline))
                    return false;
                if (message.message_id() == static_cast<uint32_t>(id))
                    return true;
            }
            return false;
        }

        //!< Reads the next log row of the source; true if one arrived with the given text.
        bool next_row(const char* text)
        {
            areg::MessageEnvelope message;
            if (!next(areg::FuncIdRange::ServiceLogMessage, message) || (message.size_used() < areg::log_entry_head() + 1u))
                return false;

            areg::LogEntry entry;
            message >> entry;
            return (std::string(entry.logMessage, areg::log_message_size(entry)) == text);
        }

        //!< Reads the messages of the source up to the log row with the given text; true if it arrived.
        bool read_until_row(const char* text)
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            areg::MessageEnvelope message;
            while (mLogs.empty() || (mLogs.back() != text))
            {
                if (!read(message, deadline))
                    return false;
            }
            return true;
        }

        //!< Reads the source until it closes the connection; true if it closed within the wait.
        bool wait_peer_closed()
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            areg::MessageEnvelope message;
            while (read(message, deadline))
                ;

            const bool closed{ std::chrono::steady_clock::now() < deadline };
            close_peer();
            return closed;
        }

        bool handshake()
        {
            areg::MessageEnvelope message;
            return accept_source() && read(message) && register_source(message);
        }

        /**
         * \brief   Makes the handshake with the first source that stays connected. A source drops
         *          a connection that completes after its connect timeout, and connects again.
         **/
        bool handshake_next_source()
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            areg::MessageEnvelope message;
            while (std::chrono::steady_clock::now() < deadline)
            {
                if (accept_source() && read(message, deadline))
                    return register_source(message);

                close_peer();
            }
            return false;
        }

    private:
        bool register_source(areg::MessageEnvelope& message)
        {
            return (message.message_id() == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect))
                && (message.target() == static_cast<uint32_t>(areg::COOKIE_LOGGER))
                && send(areg::create_connect_notify(areg::COOKIE_LOGGER, SOURCE_COOKIE))
                && next(areg::FuncIdRange::ServiceLogRegisterScopes, message);
        }

        bool read_exact(uint8_t* bytes, uint32_t size, std::chrono::steady_clock::time_point deadline)
        {
            uint32_t done{ 0u };
            while (done != size)
            {
                const auto remaining{ std::chrono::duration_cast<std::chrono::milliseconds>(
                    deadline - std::chrono::steady_clock::now()).count() };
                if ((remaining <= 0) || (mPoll->wait(static_cast<int32_t>(remaining)) != mPeer))
                    return false;

                const int32_t received{ static_cast<int32_t>(::recv(mPeer,
                    reinterpret_cast<char*>(bytes + done), static_cast<int>(size - done), 0)) };
                if (received <= 0)
                    return false;
                done += static_cast<uint32_t>(received);
            }
            return true;
        }

        const bool mInitialized;
        SOCKETHANDLE mListener;
        SOCKETHANDLE mPeer;
        std::unique_ptr<areg::SocketMultiplexer> mPoll;
        uint16_t mPort;
        std::vector<std::string> mLogs;
        uint32_t mMalformed;
    };
}

#endif  // AREG_LOGGING

#endif  // AREG_TESTS_UNITS_LOGCOLLECTORFAKE_HPP
