/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/SocketKeepAliveTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests for the TCP keepalive configuration.
 *              Covers the net::MODULE::tcpip::keepalive lookup and the keepalive
 *              values a connected socket carries.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/persist/ConfigManager.hpp"

#include <cstdio>
#include <fstream>
#include <string>

#if defined(_POSIX)
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>
#endif  // _POSIX

namespace
{
    constexpr const char * const TCPIP{ "tcpip" };

    //!< Returns the keepalive seconds read from a configuration file holding the given lines.
    uint32_t keepalive_from(const std::string & lines)
    {
        const std::string path{ std::string("./") + ::testing::UnitTest::GetInstance()->current_test_info()->name() + ".init" };
        {
            std::ofstream out(path, std::ios::trunc);
            out << lines;
        }

        areg::ConfigManager config;
        const bool loaded{ config.read_config(areg::String(path.c_str())) };
        const uint32_t result{ config.network_keepalive(areg::EmptyStringA, areg::String(TCPIP)) };
        std::remove(path.c_str());
        return (loaded ? result : 0u);
    }
}

/**
 * \brief   Without the key, the compile-time default applies.
 **/
TEST(SocketKeepAliveTest, DefaultWithoutKey)
{
    EXPECT_EQ(keepalive_from("config::*::version = 2.0.0\n"), areg::SOCKET_KEEPALIVE_SEC);
}

/**
 * \brief   The wildcard entry applies to every module, and zero means the default.
 **/
TEST(SocketKeepAliveTest, WildcardEntryAndZero)
{
    EXPECT_EQ(keepalive_from("net::*::tcpip::keepalive = 40\n"), 40u);
    EXPECT_EQ(keepalive_from("net::*::tcpip::keepalive = 0\n"), areg::SOCKET_KEEPALIVE_SEC);
}

/**
 * \brief   An entry of the running module overrides the wildcard entry.
 **/
TEST(SocketKeepAliveTest, ModuleEntryOverridesWildcard)
{
    const std::string module{ areg::Process::instance().app_name().as_string() };
    const std::string lines{ "net::*::tcpip::keepalive = 40\nnet::" + module + "::tcpip::keepalive = 25\n" };
    EXPECT_EQ(keepalive_from(lines), 25u);
}

#if defined(_POSIX)

/**
 * \brief   A socket configured by socket_set_no_delay() declares a silent peer lost after
 *          the configured seconds: 5 probes, and idle + 5 x interval equals the configured value.
 **/
TEST(SocketKeepAliveTest, SocketCarriesConfiguredValues)
{
    ASSERT_TRUE(areg::socket_initialize());
    const SOCKETHANDLE hSocket{ areg::socket_create() };
    ASSERT_TRUE(areg::is_valid_socket(hSocket));
    areg::socket_set_no_delay(hSocket);

    int keepIdle{ 0 };
    int keepInterval{ 0 };
    int keepCount{ 0 };
    socklen_t len{ sizeof(int) };
#if defined(__APPLE__)
    EXPECT_EQ(::getsockopt(hSocket, IPPROTO_TCP, TCP_KEEPALIVE, &keepIdle, &len), 0);
#else   // __APPLE__
    EXPECT_EQ(::getsockopt(hSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, &len), 0);
#endif  // __APPLE__
    len = sizeof(int);
    EXPECT_EQ(::getsockopt(hSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, &len), 0);
    len = sizeof(int);
    EXPECT_EQ(::getsockopt(hSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, &len), 0);
    areg::socket_close(hSocket);

    const int configured{ static_cast<int>(areg::Application::config_manager().network_keepalive(areg::EmptyStringA, areg::String(TCPIP))) };
    const int expected{ configured < 6 ? 6 : (configured > static_cast<int>(areg::SOCKET_KEEPALIVE_MAX_SEC) ? static_cast<int>(areg::SOCKET_KEEPALIVE_MAX_SEC) : configured) };
    EXPECT_EQ(keepCount, 5);
    EXPECT_GE(keepInterval, 1);
    EXPECT_GE(keepIdle, 1);
    EXPECT_EQ(keepIdle + keepInterval * keepCount, expected);
}

#endif  // _POSIX
