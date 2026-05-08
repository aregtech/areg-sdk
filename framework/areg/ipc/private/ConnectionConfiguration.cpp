/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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

uint32_t ConnectionConfiguration::batch_size() const noexcept
{
    return Application::config_manager().network_batch(areg::EmptyStringA, mConnectType);
}

bool ConnectionConfiguration::zerocopy_enabled() const noexcept
{
    return Application::config_manager().network_zerocopy(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::zerocopy_ring_size() const noexcept
{
    return Application::config_manager().network_ring(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::pool_pairs() const noexcept
{
    return Application::config_manager().network_pool_pairs(areg::EmptyStringA, mConnectType);
}

uint32_t ConnectionConfiguration::socket_send_timeout() const noexcept
{
    return Application::config_manager().network_timeout(areg::EmptyStringA, mConnectType);
}

bool ConnectionConfiguration::connection_ip_address( uint8_t & field0
                                                   , uint8_t & field1
                                                   , uint8_t & field2
                                                   , uint8_t & field3 )
{
    bool result = false;
    field0 = field1 = field2 = field3 = 0u;

    String addr{ connection_address() };
    if ( addr.is_empty() == false )
    {
        const char * buffer = addr.as_string( );
        const char * next   = nullptr;

        uint32_t f0 = String::make_uint32(buffer, areg::Radix::Decimal, &next);
        if ( (buffer != next) && (f0 <= 0xFFu) && (*next == areg::IP_SEPARATOR) )
        {
            buffer = next + 1;
            uint32_t f1 = String::make_uint32( buffer, areg::Radix::Decimal, &next );
            if ( (buffer != next) && (f1 <= 0xFFu) && (*next == areg::IP_SEPARATOR) )
            {
                buffer = next + 1;
                uint32_t f2 = String::make_uint32( buffer, areg::Radix::Decimal, &next );
                if ( (buffer != next) && (f2 <= 0xFFu) && (*next == areg::IP_SEPARATOR) )
                {
                    buffer = next + 1;
                    uint32_t f3 = String::make_uint32( buffer, areg::Radix::Decimal, &next );
                    if ( (buffer != next) && (f3 <= 0xFFu) )
                    {
                        field0 = static_cast<uint8_t>(f0);
                        field1 = static_cast<uint8_t>(f1);
                        field2 = static_cast<uint8_t>(f2);
                        field3 = static_cast<uint8_t>(f3);
                        result = true;
                    }
                }
            }
        }
    }

    return result;
}

} // namespace areg
