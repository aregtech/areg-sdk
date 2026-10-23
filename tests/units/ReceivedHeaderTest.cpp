/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ReceivedHeaderTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of a received message header: the local-only fields
 *              (dispatcher, consumer and cleanup hook) never take the values sent by the peer.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Socket.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/component/Event.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"

#include <atomic>
#include <cstring>

namespace
{
    //!< The number of calls of the probe hook.
    std::atomic<uint32_t> gHookCalls{ 0u };

    //!< The cleanup hook a peer writes into the header it sends.
    void probe_hook(void * /*payload*/) noexcept
    {
        gHookCalls.fetch_add(1u);
    }

    //!< A socket that wraps an already connected handle.
    class ConnectedSocket : public areg::Socket
    {
    public:
        explicit ConnectedSocket(SOCKETHANDLE hSocket)
            : areg::Socket(hSocket, areg::SocketAddress())
        {
        }

        bool create(const areg::String & /*hostName*/, uint16_t /*portNr*/) override { return false; }
        bool create(const char * /*hostName*/, uint16_t /*portNr*/) override { return false; }
        bool create() override { return false; }
    };

    //!< Gives the test access to receive_message().
    class Receiver : public areg::SocketConnectionBase
    {
    public:
        using areg::SocketConnectionBase::receive_message;
    };

    //!< Opens a connected TCP loopback pair; returns false if no port in the range is free.
    bool open_pair(SOCKETHANDLE & client, SOCKETHANDLE & server)
    {
        for (uint16_t port = 42'131u; port < 42'231u; ++port)
        {
            const SOCKETHANDLE listener{ areg::server_connect(areg::String("127.0.0.1"), port) };
            if (areg::is_valid_socket(listener) == false)
                continue;

            if (areg::server_listen(listener))
            {
                areg::SocketAddress address{ };
                client = address.resolve_address(areg::String("127.0.0.1"), port, false) ? areg::socket_create() : areg::InvalidSocketHandle;
                if (areg::is_valid_socket(client) && (areg::client_connect_fd(client, address) == false))
                {
                    areg::socket_close(client);
                    client = areg::InvalidSocketHandle;
                }

                const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                server = areg::is_valid_socket(client) ? areg::server_accept(listener, &none, 0) : areg::InvalidSocketHandle;
            }

            areg::socket_close(listener);
            if (areg::is_valid_socket(server))
                return true;

            if (areg::is_valid_socket(client))
                areg::socket_close(client);
            client = areg::InvalidSocketHandle;
        }

        return false;
    }
}

/**
 * \brief   A header whose local-only fields a peer has set arrives with those fields cleared,
 *          and destroying the received event calls no hook.
 **/
TEST(ReceivedHeaderTest, LocalFieldsOfPeerAreCleared)
{
    ASSERT_TRUE(areg::socket_initialize());
    SOCKETHANDLE client{ areg::InvalidSocketHandle };
    SOCKETHANDLE server{ areg::InvalidSocketHandle };
    ASSERT_TRUE(open_pair(client, server));

    using HookFn = void(*)(void *) noexcept;
    const HookFn hook{ &probe_hook };
    areg::EventHeader sent{ };
    sent.eventType  = static_cast<uint16_t>(areg::EventType::EventRemoteRequest);
    sent.internal1  = 0x1111'1111u;
    sent.internal2  = 0x2222'2222u;
    std::memcpy(&sent.custom, &hook, sizeof(hook));
    ASSERT_EQ(areg::send_data(server, reinterpret_cast<const uint8_t *>(&sent), static_cast<uint32_t>(sizeof(sent))), static_cast<int32_t>(sizeof(sent)));

    gHookCalls.store(0u);
    {
        const ConnectedSocket socket(client);
        const Receiver receiver;
        areg::MessageEnvelope received;
        ASSERT_GT(receiver.receive_message(received, socket), 0);

        const areg::EventHeader * hdr{ received.header() };
        ASSERT_NE(hdr, nullptr);
        EXPECT_EQ(hdr->internal1, 0u);
        EXPECT_EQ(hdr->internal2, 0u);
        EXPECT_EQ(hdr->custom, 0u);

        areg::Event event(std::move(received));
    }

    EXPECT_EQ(gHookCalls.load(), 0u);
    areg::socket_close(server);
}

/**
 * \brief   The reserved header field carries no meaning: a header with any value in it is received.
 **/
TEST(ReceivedHeaderTest, ReservedFieldIsIgnored)
{
    ASSERT_TRUE(areg::socket_initialize());
    SOCKETHANDLE client{ areg::InvalidSocketHandle };
    SOCKETHANDLE server{ areg::InvalidSocketHandle };
    ASSERT_TRUE(open_pair(client, server));

    areg::EventHeader sent{ };
    sent.eventType  = static_cast<uint16_t>(areg::EventType::EventRemoteRequest);
    sent.reserved   = 0xFFFF'FFFFu;
    sent.messageId  = 0x1234u;
    ASSERT_EQ(areg::send_data(server, reinterpret_cast<const uint8_t *>(&sent), static_cast<uint32_t>(sizeof(sent))), static_cast<int32_t>(sizeof(sent)));

    {
        const ConnectedSocket socket(client);
        const Receiver receiver;
        areg::MessageEnvelope received;
        EXPECT_EQ(receiver.receive_message(received, socket), static_cast<int32_t>(sizeof(areg::EventHeader)));
        EXPECT_EQ(received.message_id(), 0x1234u);
    }

    areg::socket_close(server);
}

/**
 * \brief   A header without an event type is refused: nothing is received from it.
 **/
TEST(ReceivedHeaderTest, HeaderWithoutEventTypeIsRefused)
{
    ASSERT_TRUE(areg::socket_initialize());
    SOCKETHANDLE client{ areg::InvalidSocketHandle };
    SOCKETHANDLE server{ areg::InvalidSocketHandle };
    ASSERT_TRUE(open_pair(client, server));

    areg::EventHeader sent{ };
    sent.messageId  = 0x1234u;
    ASSERT_EQ(areg::send_data(server, reinterpret_cast<const uint8_t *>(&sent), static_cast<uint32_t>(sizeof(sent))), static_cast<int32_t>(sizeof(sent)));

    {
        const ConnectedSocket socket(client);
        const Receiver receiver;
        areg::MessageEnvelope received;
        EXPECT_EQ(receiver.receive_message(received, socket), 0);
        EXPECT_FALSE(received.is_valid());
    }

    areg::socket_close(server);
}
