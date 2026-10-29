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
 *              receive mode, after a release and after a mode change, and the
 *              calls that read and fill a cache given by the caller.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/EventDefs.hpp"
#include "aregextend/service/private/ServiceThreadHelper.hpp"

#include <chrono>
#include <cstring>
#include <thread>
#include <vector>

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

    //!< Appends one message on the wire: the header with \a used bytes of data, then the data.
    void append_message(std::vector<uint8_t> & wire, areg::EventType type, uint32_t used, uint8_t fill)
    {
        areg::EventHeader header{ };
        header.eventType        = static_cast<uint16_t>(type);
        header.bufHeader.biUsed = used;
        const uint8_t * const head{ areg::wire_data(header) };
        wire.insert(wire.end(), head, head + areg::WIRE_HEADER_SIZE);
        wire.insert(wire.end(), used, fill);
    }

    //!< Copies \a wire into the cache as unread bytes from the start.
    void load_cache(areg::ThreadCache & cache, const std::vector<uint8_t> & wire)
    {
        uint8_t * const buffer{ cache.cache() };
        ASSERT_NE(buffer, nullptr);
        ASSERT_LE(wire.size(), static_cast<size_t>(cache.space));
        std::memcpy(buffer, wire.data(), wire.size());
        cache.head   = 0u;
        cache.unread = static_cast<uint32_t>(wire.size());
    }

    //!< Opens a connected TCP loopback pair; returns false if no port in the range is free.
    bool open_pair(SOCKETHANDLE & client, SOCKETHANDLE & server)
    {
        for (uint16_t port = 42'331u; port < 42'431u; ++port)
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

TEST( ReceiveCacheTest, AvailableCountsOnlyWholeMessages )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        areg::ThreadCache& cache = areg::thread_rx_cache( SOCKET_ONE );
        EXPECT_EQ( areg::recv_data_available( cache ), 0u );
        EXPECT_FALSE( areg::recv_burst_pending( SOCKET_ONE ) );

        std::vector<uint8_t> wire;
        append_message( wire, areg::EventType::EventRemoteRequest, 10u, 0x5Au );
        load_cache( cache, wire );

        cache.unread = areg::WIRE_HEADER_SIZE - 1u;
        EXPECT_EQ( areg::recv_data_available( cache ), 0u );
        cache.unread = static_cast<uint32_t>(wire.size()) - 1u;
        EXPECT_EQ( areg::recv_data_available( cache ), 0u );
        EXPECT_EQ( areg::recv_data_available( SOCKET_ONE ), 0u );
        EXPECT_FALSE( areg::recv_burst_pending( SOCKET_ONE ) );

        cache.unread = static_cast<uint32_t>(wire.size());
        EXPECT_EQ( areg::recv_data_available( cache ), cache.unread );
        EXPECT_EQ( areg::recv_data_available( SOCKET_ONE ), cache.unread );
        EXPECT_TRUE( areg::recv_burst_pending( SOCKET_ONE ) );

        areg::thread_rx_cache_release( SOCKET_ONE );
    });

    run_in_new_thread([]()
    {
        EXPECT_EQ( areg::recv_data_available( SOCKET_ONE ), 0u );
        EXPECT_TRUE( areg::recv_burst_pending( SOCKET_ONE ) );
    });
}

TEST( ReceiveCacheTest, CachedMessageTakesOneMessage )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        areg::ThreadCache& cache = areg::thread_rx_cache( SOCKET_ONE );

        std::vector<uint8_t> wire;
        append_message( wire, areg::EventType::EventRemoteRequest, 24u, 0x11u );
        const uint32_t first{ static_cast<uint32_t>(wire.size()) };
        append_message( wire, areg::EventType::EventRemoteResponse, 0u, 0x00u );
        const uint32_t second{ static_cast<uint32_t>(wire.size()) - first };
        load_cache( cache, wire );

        areg::MessageEnvelope received;
        EXPECT_EQ( areg::ext::receive_cached_message( received, cache ), static_cast<int32_t>(first) );
        ASSERT_TRUE( received.is_valid() );
        EXPECT_EQ( received.header()->bufHeader.biUsed, 24u );
        EXPECT_EQ( received.header()->eventType, static_cast<uint16_t>(areg::EventType::EventRemoteRequest) );
        const uint8_t * const data{ received.payload_ptr() };
        ASSERT_NE( data, nullptr );
        for (uint32_t i = 0u; i < 24u; ++i)
        {
            EXPECT_EQ( data[i], 0x11u );
        }

        EXPECT_EQ( cache.head, first );
        EXPECT_EQ( cache.unread, second );
        EXPECT_EQ( areg::recv_data_available( cache ), second );

        EXPECT_EQ( areg::ext::receive_cached_message( received, cache ), static_cast<int32_t>(second) );
        EXPECT_EQ( received.header()->eventType, static_cast<uint16_t>(areg::EventType::EventRemoteResponse) );
        EXPECT_EQ( cache.head, 0u );
        EXPECT_EQ( cache.unread, 0u );
        EXPECT_EQ( areg::ext::receive_cached_message( received, cache ), 0 );

        areg::thread_rx_cache_release( SOCKET_ONE );
    });
}

TEST( ReceiveCacheTest, CachedMessageRefusesBadHeader )
{
    run_in_new_thread([]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        areg::ThreadCache& cache = areg::thread_rx_cache( SOCKET_ONE );

        std::vector<uint8_t> wire;
        append_message( wire, areg::EventType::EventUnknown, 8u, 0x22u );
        load_cache( cache, wire );

        areg::MessageEnvelope received;
        EXPECT_EQ( areg::ext::receive_cached_message( received, cache ), 0 );
        EXPECT_EQ( cache.head, 0u );
        EXPECT_EQ( cache.unread, static_cast<uint32_t>(wire.size()) );

        wire.clear();
        append_message( wire, areg::EventType::EventRemoteRequest, 8u, 0x22u );
        wire.pop_back();
        load_cache( cache, wire );
        EXPECT_EQ( areg::ext::receive_cached_message( received, cache ), 0 );
        EXPECT_EQ( cache.unread, static_cast<uint32_t>(wire.size()) );

        areg::thread_rx_cache_release( SOCKET_ONE );
    });
}

TEST( ReceiveCacheTest, FillAndReceiveShareTheGivenCache )
{
    ASSERT_TRUE( areg::socket_initialize() );
    SOCKETHANDLE client{ areg::InvalidSocketHandle };
    SOCKETHANDLE server{ areg::InvalidSocketHandle };
    ASSERT_TRUE( open_pair(client, server) );

    std::vector<uint8_t> wire;
    append_message( wire, areg::EventType::EventRemoteRequest, 40u, 0x33u );
    append_message( wire, areg::EventType::EventRemoteRequest, 40u, 0x44u );
    ASSERT_EQ( areg::send_data(client, wire.data(), static_cast<uint32_t>(wire.size())), static_cast<int32_t>(wire.size()) );

    run_in_new_thread([server, &wire]()
    {
        areg::set_receive_mode( areg::ReceiveMode::MultiCache );
        areg::ThreadCache& cache = areg::thread_rx_cache( server );

        uint32_t filled{ 0u };
        for (int round = 0; (round < 1000) && (filled < wire.size()); ++round)
        {
            const int32_t added{ areg::recv_fill_nowait( server, cache ) };
            ASSERT_GE( added, 0 );
            filled += static_cast<uint32_t>(added);
            if (added == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        ASSERT_EQ( filled, static_cast<uint32_t>(wire.size()) );
        EXPECT_EQ( cache.unread, filled );
        EXPECT_EQ( areg::recv_data_available( cache ), filled );

        std::vector<uint8_t> read( wire.size(), 0u );
        EXPECT_EQ( areg::receive_data( server, read.data(), static_cast<uint32_t>(read.size()) ), static_cast<int32_t>(read.size()) );
        EXPECT_EQ( read, wire );
        EXPECT_EQ( cache.unread, 0u );

        areg::thread_rx_cache_release( server );
    });

    areg::socket_close( client );
    areg::socket_close( server );
}
