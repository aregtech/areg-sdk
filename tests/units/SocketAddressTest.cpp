/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketAddressTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of SocketAddress: an address that is given or accepted
 *              as an IP keeps the IP as its host name, and a host name stays as given.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"

namespace
{
    //!< The loopback address of every test.
    constexpr std::string_view  LOOPBACK    { "127.0.0.1" };

    //!< The port given to the addresses that are not connected.
    constexpr uint16_t          ANY_PORT    { 41'800u };

    //!< Accepts one loopback connection and returns the address the accept filled in.
    bool accept_one(areg::SocketAddress & accepted)
    {
        for (uint16_t port = 41'801u; port < 41'901u; ++port)
        {
            const SOCKETHANDLE listener{ areg::server_connect(areg::String(LOOPBACK), port) };
            if (areg::is_valid_socket(listener) == false)
                continue;

            SOCKETHANDLE client{ areg::InvalidSocketHandle };
            SOCKETHANDLE server{ areg::InvalidSocketHandle };
            if (areg::server_listen(listener))
            {
                areg::SocketAddress address{ };
                client = address.resolve_address(areg::String(LOOPBACK), port, false) ? areg::socket_create() : areg::InvalidSocketHandle;
                if (areg::is_valid_socket(client) && areg::client_connect_fd(client, address))
                {
                    const SOCKETHANDLE none{ areg::InvalidSocketHandle };
                    server = areg::server_accept(listener, &none, 0, &accepted);
                }
            }

            const bool result{ areg::is_valid_socket(server) };
            if (result)
                areg::socket_close(server);
            if (areg::is_valid_socket(client))
                areg::socket_close(client);
            areg::socket_close(listener);
            if (result)
                return true;
        }

        return false;
    }
}

/**
 * \brief   An accepted connection has the IP of the peer as its host name.
 **/
TEST(SocketAddressTest, AcceptedAddressNamesTheIp)
{
    areg::SocketAddress accepted{ };
    ASSERT_TRUE(accept_one(accepted));
    EXPECT_EQ(accepted.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(accepted.host_name(), accepted.host_address());
}

/**
 * \brief   An address resolved from an IP has that IP as its host name.
 **/
TEST(SocketAddressTest, ResolvedIpNamesTheIp)
{
    areg::SocketAddress address{ };
    ASSERT_TRUE(address.resolve_address(areg::String(LOOPBACK), ANY_PORT, false));
    EXPECT_EQ(address.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(address.host_name(), areg::String(LOOPBACK));
}

/**
 * \brief   An address constructed from an IP has that IP as its host name.
 **/
TEST(SocketAddressTest, ConstructedIpNamesTheIp)
{
    const areg::SocketAddress address{ areg::String(LOOPBACK), ANY_PORT };
    EXPECT_EQ(address.host_address(), areg::String(LOOPBACK));
    EXPECT_EQ(address.host_name(), areg::String(LOOPBACK));
}

/**
 * \brief   An address resolved from a host name keeps that name and gets its IP.
 **/
TEST(SocketAddressTest, ResolvedNameStaysTheName)
{
    areg::SocketAddress address{ };
    ASSERT_TRUE(address.resolve_address(areg::String("localhost"), ANY_PORT, false));
    EXPECT_EQ(address.host_name(), areg::String("localhost"));
    EXPECT_FALSE(address.host_address().is_empty());
    EXPECT_TRUE(address.is_equal(areg::String("localhost"), ANY_PORT));
    EXPECT_TRUE(address.is_equal(address.host_address(), ANY_PORT));
}
