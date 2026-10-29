/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ReceiveCacheTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the thread-local receive cache.
 *              Covers which cache thread_rx_cache() returns for a socket in each
 *              receive mode, after a release and after a mode change.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"

#include <thread>

namespace
{
    //!< Socket handles that are never opened: the cache only keys on the value.
    const SOCKETHANDLE  SOCKET_ONE  { static_cast<SOCKETHANDLE>(1000001) };
    const SOCKETHANDLE  SOCKET_TWO  { static_cast<SOCKETHANDLE>(1000002) };

    //!< Runs the check in a new thread, which starts with a fresh receive state.
    template<typename Check>
    void run_in_new_thread(Check check)
    {
        std::thread worker(check);
        worker.join();
    }
}

TEST( ReceiveCacheTest, MultiCacheKeepsOneCachePerSocket )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );

        areg::ThreadCache* const first  = &areg::thread_rx_cache( SOCKET_ONE );
        areg::ThreadCache* const second = &areg::thread_rx_cache( SOCKET_TWO );
        EXPECT_NE( first, second );
        EXPECT_EQ( first , &areg::thread_rx_cache( SOCKET_ONE ) );
        EXPECT_EQ( second, &areg::thread_rx_cache( SOCKET_TWO ) );
        EXPECT_EQ( first->socket , SOCKET_ONE );
        EXPECT_EQ( second->socket, SOCKET_TWO );

        areg::thread_rx_cache_release( SOCKET_ONE );
        areg::thread_rx_cache_release( SOCKET_TWO );
    });
}

TEST( ReceiveCacheTest, ReleaseDropsTheCursors )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );

        areg::ThreadCache& cache = areg::thread_rx_cache( SOCKET_ONE );
        cache.head   = 5u;
        cache.unread = 7u;
        areg::thread_rx_cache_release( SOCKET_ONE );

        const areg::ThreadCache& fresh = areg::thread_rx_cache( SOCKET_ONE );
        EXPECT_EQ( fresh.socket, SOCKET_ONE );
        EXPECT_EQ( fresh.head  , 0u );
        EXPECT_EQ( fresh.unread, 0u );

        areg::thread_rx_cache_release( SOCKET_ONE );
    });
}

TEST( ReceiveCacheTest, ModeChangeSwitchesTheCache )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        areg::ThreadCache* const multi = &areg::thread_rx_cache( SOCKET_ONE );

        areg::set_receive_mode( areg::ReceiveMode::MonoCache );
        areg::ThreadCache* const mono = &areg::thread_rx_cache( SOCKET_ONE );
        EXPECT_NE( multi, mono );

        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        EXPECT_EQ( multi, &areg::thread_rx_cache( SOCKET_ONE ) );

        areg::thread_rx_cache_release( SOCKET_ONE );
    });
}

TEST( ReceiveCacheTest, MonoCacheServesEverySocket )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MonoCache );

        areg::ThreadCache* const first = &areg::thread_rx_cache( SOCKET_ONE );
        EXPECT_EQ( first->socket, SOCKET_ONE );

        areg::ThreadCache* const second = &areg::thread_rx_cache( SOCKET_TWO );
        EXPECT_EQ( first, second );
        EXPECT_EQ( second->socket, SOCKET_TWO );
        EXPECT_EQ( areg::thread_rx_cache( SOCKET_ONE ).socket, SOCKET_ONE );
    });
}

TEST( ReceiveCacheTest, ModeBelongsToTheThread )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        EXPECT_EQ( areg::receive_mode(), areg::ReceiveMode::MultiCache );

        areg::ReceiveMode other{ areg::ReceiveMode::MultiCache };
        run_in_new_thread([&other]() { other = areg::receive_mode(); });
        EXPECT_EQ( other, areg::ReceiveMode::NoCache );
        EXPECT_EQ( areg::receive_mode(), areg::ReceiveMode::MultiCache );
    });
}
