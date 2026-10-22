/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketInterruptTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of socket_interrupt(): a receive or a send blocked on a
 *              connected socket whose peer does nothing returns at once.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< Milliseconds the blocked call has to return in after the interrupt.
    constexpr int64_t   PROMPT_MS   { 1'000 };

    //!< Milliseconds the blocked call is given to enter the wait.
    constexpr uint32_t  ENTER_MS    { 200u };

    //!< Milliseconds after which the peer is closed, so that a call the interrupt did not end returns.
    constexpr int64_t   RESCUE_MS   { 5'000 };

    //!< A connected TCP loopback pair whose server side never reads nor writes.
    struct Pair
    {
        SOCKETHANDLE    client{ areg::InvalidSocketHandle };
        SOCKETHANDLE    server{ areg::InvalidSocketHandle };

        bool open()
        {
            for (uint16_t port = 41'931u; port < 42'031u; ++port)
            {
                const SOCKETHANDLE listener{ areg::server_connect(areg::String("127.0.0.1"), port) };
                if (areg::is_valid_socket(listener) == false)
                    continue;

                if (areg::server_listen(listener))
                {
                    areg::SocketAddress address{ };
                    client = address.resolve_address(areg::String("127.0.0.1"), port, false) ? areg::socket_create() : areg::InvalidSocketHandle;
                    if (areg::is_valid_socket(client))
                    {
                        static_cast<void>(areg::set_send_size(client, 64u * 1024u));
                        if (areg::client_connect_fd(client, address) == false)
                        {
                            areg::socket_close(client);
                            client = areg::InvalidSocketHandle;
                        }
                    }

                    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                    server = areg::is_valid_socket(client) ? areg::server_accept(listener, &none, 0) : areg::InvalidSocketHandle;
                }

                areg::socket_close(listener);
                if (areg::is_valid_socket(server))
                {
                    static_cast<void>(areg::set_recv_size(server, 64u * 1024u));
                    return true;
                }

                close();
            }

            return false;
        }

        void close_server()
        {
            if (areg::is_valid_socket(server))
                areg::socket_close(server);
            server = areg::InvalidSocketHandle;
        }

        void close()
        {
            if (areg::is_valid_socket(client))
                areg::socket_close(client);
            client = areg::InvalidSocketHandle;
            close_server();
        }
    };

    //!< Runs the blocking call on a thread, interrupts the client and returns the milliseconds until the call returned.
    template<typename Call>
    int64_t interrupt_blocked(Pair & pair, Call call)
    {
        std::atomic_bool returned{ false };
        std::thread worker([&returned, call]() { call(); returned.store(true); });
        std::this_thread::sleep_for(std::chrono::milliseconds(ENTER_MS));

        const Clock::time_point start{ Clock::now() };
        areg::socket_interrupt(pair.client);
        while ((returned.load() == false) && (Clock::now() - start < std::chrono::milliseconds(RESCUE_MS)))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        const int64_t elapsed{ std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - start).count() };
        pair.close_server();
        worker.join();
        return elapsed;
    }
}

/**
 * \brief   A receive blocked on a socket whose peer sends nothing returns after the interrupt.
 **/
TEST(SocketInterruptTest, BlockedReceiveReturns)
{
    ASSERT_TRUE(areg::socket_initialize());
    Pair pair;
    ASSERT_TRUE(pair.open());

    std::atomic<int32_t> received{ 1 };
    const SOCKETHANDLE client{ pair.client };
    const int64_t elapsed{ interrupt_blocked(pair, [client, &received]()
        {
            uint8_t buffer[64]{ };
            received.store(areg::receive_data(client, buffer, sizeof(buffer)));
        }) };

    pair.close();
    EXPECT_LT(elapsed, PROMPT_MS);
    EXPECT_LE(received.load(), 0);
}

/**
 * \brief   A send blocked on a full socket whose peer does not read returns after the interrupt.
 **/
TEST(SocketInterruptTest, BlockedSendReturns)
{
    ASSERT_TRUE(areg::socket_initialize());
    Pair pair;
    ASSERT_TRUE(pair.open());

    const SOCKETHANDLE client{ pair.client };
    const int64_t elapsed{ interrupt_blocked(pair, [client]()
        {
            std::vector<uint8_t> data(16u * 1024u * 1024u, static_cast<uint8_t>(0x5A));
            static_cast<void>(areg::send_data(client, data.data(), static_cast<uint32_t>(data.size())));
        }) };

    pair.close();
    EXPECT_LT(elapsed, PROMPT_MS);
}
