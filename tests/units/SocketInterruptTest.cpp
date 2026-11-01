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
 *              connected socket whose peer does nothing returns, before the peer acts. Also the bounded receive
 *              of a client connection: it ends when the live handle changes, and it keeps waiting
 *              while the connection is live.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/private/SocketLiveness.hpp"

#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

namespace
{
    //!< Milliseconds the blocked call is given to enter the wait.
    constexpr uint32_t  ENTER_MS    { 200u };

    //!< Milliseconds a call is given to return before the test closes the peer to free it.
    constexpr int64_t   DEADLINE_MS { 30'000 };

    //!< Milliseconds of the receive timeout in the bounded receive cases.
    constexpr uint32_t  RECHECK_MS  { 50u };

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

    //!< Returns true if the call signaled 'returned' before the deadline.
    bool returns(const std::future<void> & returned)
    {
        return (returned.wait_for(std::chrono::milliseconds(DEADLINE_MS)) == std::future_status::ready);
    }

    //!< Returns true if the call has not signaled 'returned' yet.
    bool still_waits(const std::future<void> & returned)
    {
        return (returned.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready);
    }

    //!< Runs the blocking call on a thread, interrupts the client and returns true if the call
    //!< returned before the peer was closed.
    template<typename Call>
    bool interrupt_blocked(Pair & pair, Call call)
    {
        std::promise<void> done;
        const std::future<void> returned{ done.get_future() };
        std::thread worker([&done, call]() { call(); done.set_value(); });
        std::this_thread::sleep_for(std::chrono::milliseconds(ENTER_MS));

        areg::socket_interrupt(pair.client);
        const bool byInterrupt{ returns(returned) };
        pair.close_server();
        worker.join();
        return byInterrupt;
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
    const bool byInterrupt{ interrupt_blocked(pair, [client, &received]()
        {
            uint8_t buffer[64]{ };
            received.store(areg::receive_data(client, buffer, sizeof(buffer)));
        }) };

    pair.close();
    EXPECT_TRUE(byInterrupt);
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
    const bool byInterrupt{ interrupt_blocked(pair, [client]()
        {
            std::vector<uint8_t> data(16u * 1024u * 1024u, static_cast<uint8_t>(0x5A));
            static_cast<void>(areg::send_data(client, data.data(), static_cast<uint32_t>(data.size())));
        }) };

    pair.close();
    EXPECT_TRUE(byInterrupt);
}

/**
 * \brief   A receive blocked in the middle of a message ends once the live handle of its connection
 *          changes, without a shutdown. POSIX only: Windows cancels a blocked receive instead.
 **/
#if !defined(_WIN32)
TEST(SocketInterruptTest, BlockedReceiveEndsWhenLiveHandleChanges)
{
    ASSERT_TRUE(areg::socket_initialize());
    Pair pair;
    ASSERT_TRUE(pair.open());
    areg::set_receive_recheck(pair.client, RECHECK_MS);

    constexpr uint32_t MESSAGE{ 3u * 1024u * 1024u };
    const std::vector<uint8_t> part(32u * 1024u, static_cast<uint8_t>(0x5A));
    ASSERT_EQ(areg::send_data(pair.server, part.data(), static_cast<uint32_t>(part.size())), static_cast<int32_t>(part.size()));

    std::atomic<SOCKETHANDLE> live{ pair.client };
    std::atomic<int32_t> received{ 1 };
    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    const SOCKETHANDLE client{ pair.client };
    std::thread worker([client, &live, &received, &done]()
        {
            areg::set_receive_live_handle(&live);
            std::vector<uint8_t> buffer(MESSAGE);
            received.store(areg::receive_data(client, buffer.data(), MESSAGE));
            areg::set_receive_live_handle(nullptr);
            done.set_value();
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(6u * RECHECK_MS));
    const bool waitedAcrossRechecks{ still_waits(returned) };

    live.store(areg::InvalidSocketHandle);
    const bool byHandleChange{ returns(returned) };
    pair.close_server();
    worker.join();
    pair.close();

    EXPECT_TRUE(waitedAcrossRechecks);
    EXPECT_TRUE(byHandleChange);
    EXPECT_LE(received.load(), 0);
}
#endif  // !defined(_WIN32)

/**
 * \brief   A receive whose timeout expires while its connection is live keeps what it read and
 *          returns the whole message once the rest arrives.
 **/
TEST(SocketInterruptTest, ReceiveContinuesAcrossRechecks)
{
    ASSERT_TRUE(areg::socket_initialize());
    Pair pair;
    ASSERT_TRUE(pair.open());
    areg::set_receive_recheck(pair.client, RECHECK_MS);

    constexpr uint32_t MESSAGE{ 48u * 1024u };
    std::vector<uint8_t> data(MESSAGE);
    for (uint32_t i = 0u; i < MESSAGE; ++i)
    {
        data[i] = static_cast<uint8_t>(i * 7u);
    }

    std::atomic<SOCKETHANDLE> live{ pair.client };
    std::atomic<int32_t> received{ 0 };
    std::promise<void> done;
    const std::future<void> returned{ done.get_future() };
    std::vector<uint8_t> buffer(MESSAGE);
    const SOCKETHANDLE client{ pair.client };
    std::thread worker([client, &live, &received, &done, &buffer]()
        {
            areg::set_receive_live_handle(&live);
            received.store(areg::receive_data(client, buffer.data(), MESSAGE));
            areg::set_receive_live_handle(nullptr);
            done.set_value();
        });

    constexpr uint32_t FIRST{ 16u * 1024u };
    const bool firstSent{ areg::send_data(pair.server, data.data(), FIRST) == static_cast<int32_t>(FIRST) };
    std::this_thread::sleep_for(std::chrono::milliseconds(6u * RECHECK_MS));
    const bool waitedAcrossRechecks{ still_waits(returned) };
    const bool restSent{ areg::send_data(pair.server, data.data() + FIRST, MESSAGE - FIRST) == static_cast<int32_t>(MESSAGE - FIRST) };

    const bool completed{ returns(returned) };
    pair.close_server();
    worker.join();
    pair.close();

    EXPECT_TRUE(firstSent);
    EXPECT_TRUE(restSent);
    EXPECT_TRUE(waitedAcrossRechecks);
    EXPECT_TRUE(completed);
    EXPECT_EQ(received.load(), static_cast<int32_t>(MESSAGE));
    EXPECT_TRUE(buffer == data);
}
