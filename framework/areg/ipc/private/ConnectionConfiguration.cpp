/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/ConnectionConfiguration.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "areg/base/private/SocketLiveness.hpp"

namespace areg {

ConnectionConfiguration::ConnectionConfiguration(const String& service, const String& connectType)
    : mServiceName  ( service )
    , mConnectType  ( connectType )
{
}

ConnectionConfiguration::ConnectionConfiguration(areg::RemoteServiceKind service, areg::ConnectionType connectType)
    : mServiceName  (Identifier::to_string(static_cast<uint32_t>(service), areg::RemoteServiceIdentifiers, static_cast<uint32_t>(areg::RemoteServiceKind::Unknown)))
    , mConnectType  (Identifier::to_string(static_cast<uint32_t>(connectType), areg::ConnectionIdentifiers, static_cast<uint32_t>(areg::ConnectionType::Undefined)))
{
}

String ConnectionConfiguration::connection_address() const
{
    return Application::config_manager().remote_service_address(mServiceName, mConnectType);
}

void ConnectionConfiguration::set_connection_address(const String& address)
{
    Application::config_manager().set_service_address(mServiceName, mConnectType, address);
}

void ConnectionConfiguration::set_connection_data(const String& address, uint16_t portNr)
{
    Application::config_manager().set_service_address(mServiceName, mConnectType, address);
    Application::config_manager().set_service_port(mServiceName, mConnectType, portNr);
}

bool ConnectionConfiguration::is_configured() const noexcept
{
    return Application::is_configured();
}

bool ConnectionConfiguration::connection_enable_flag() const
{
    return Application::config_manager().remote_service_enable(mServiceName, mConnectType);
}

void ConnectionConfiguration::set_connection_enable(bool is_enabled)
{
    Application::config_manager().set_service_enable(mServiceName, mConnectType, is_enabled);
}

uint16_t ConnectionConfiguration::connection_port() const noexcept
{
    return Application::config_manager().remote_service_port(mServiceName, mConnectType);
}

void ConnectionConfiguration::set_connection_port(uint16_t portNr)
{
    Application::config_manager().set_service_port(mServiceName, mConnectType, portNr);
}

uint32_t ConnectionConfiguration::socket_send_buffer() const noexcept
{
    return Application::config_manager().network_sndbuf(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::socket_recv_buffer() const noexcept
{
    return Application::config_manager().network_rcvbuf(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::drain_limit() const noexcept
{
    return Application::config_manager().network_drain_limit(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::pool_pairs() const noexcept
{
    return Application::config_manager().network_pool_pairs(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::socket_send_timeout() const noexcept
{
    return Application::config_manager().network_timeout(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::socket_refusal_timeout() const noexcept
{
    const ConfigManager & config{ Application::config_manager() };
    const uint32_t keepalive{ config.network_keepalive(areg::EmptyStringA, mConnectType) };
    const uint32_t keepaliveSec{ keepalive < areg::SOCKET_KEEPALIVE_MAX_SEC ? keepalive : areg::SOCKET_KEEPALIVE_MAX_SEC };
    return areg::send_refusal_ms(config.network_timeout(areg::EmptyStringA, mConnectType), keepaliveSec);
}

namespace
{
    /**
     * \brief   Parses a decimal octet of 1 to 3 digits and returns the position after it,
     *          or nullptr if the text does not start with a valid octet.
     **/
    const char * _parse_ip_octet(const char * text, uint8_t & octet) noexcept
    {
        constexpr int32_t MAX_DIGITS{ 3 };
        uint32_t value{ 0u };
        const char * pos{ text };
        while (((pos - text) < MAX_DIGITS) && (*pos >= '0') && (*pos <= '9'))
        {
            value = (value * 10u) + static_cast<uint32_t>(*pos - '0');
            ++ pos;
        }

        if ((pos == text) || (value > 0xFFu) || ((*pos >= '0') && (*pos <= '9')))
            return nullptr;

        octet = static_cast<uint8_t>(value);
        return pos;
    }
}

bool ConnectionConfiguration::connection_ip_address( uint8_t & field0
                                                   , uint8_t & field1
                                                   , uint8_t & field2
                                                   , uint8_t & field3 )
{
    field0 = field1 = field2 = field3 = 0u;

    const String addr{ connection_address() };
    uint8_t octets[4]{ 0u, 0u, 0u, 0u };
    const char * pos{ addr.as_string() };
    for (uint32_t i = 0u; (i < 4u) && (pos != nullptr); ++ i)
    {
        pos = _parse_ip_octet(pos, octets[i]);
        if ((pos != nullptr) && (i < 3u))
        {
            pos = (*pos == areg::IP_SEPARATOR) ? pos + 1 : nullptr;
        }
    }

    if ((pos == nullptr) || (*pos != areg::EndOfString))
        return false;

    field0 = octets[0];
    field1 = octets[1];
    field2 = octets[2];
    field3 = octets[3];
    return true;
}

} // namespace areg
