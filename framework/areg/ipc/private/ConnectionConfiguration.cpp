/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/ConnectionConfiguration.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Remote service connection configuration
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/ipc/ConnectionConfiguration.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/NEApplication.hpp"
#include "areg/base/Identifier.hpp"
#include "areg/persist/ConfigManager.hpp"

ConnectionConfiguration::ConnectionConfiguration(const String& service, const String& connectType)
    : mServiceName  ( service )
    , mConnectType  ( connectType )
{
}

ConnectionConfiguration::ConnectionConfiguration(NERemoteService::eRemoteServices service, NERemoteService::eConnectionTypes connectType)
    : mServiceName  (Identifier::convToString(static_cast<unsigned int>(service), NEApplication::RemoteServiceIdentifiers, static_cast<unsigned int>(NERemoteService::eRemoteServices::ServiceUnknown)))
    , mConnectType  (Identifier::convToString(static_cast<unsigned int>(connectType), NEApplication::ConnectionIdentifiers, static_cast<unsigned int>(NERemoteService::eConnectionTypes::ConnectUndefined)))
{
}

String ConnectionConfiguration::getConnectionAddress( void ) const
{
    return Application::getConfigManager().getRemoteServiceAddress(mServiceName, mConnectType);
}

void ConnectionConfiguration::setConnectionAddress(const String& address)
{
    Application::getConfigManager().setRemoteServiceAddress(mServiceName, mConnectType, address);
}

void ConnectionConfiguration::setConnectionData(const String& address, unsigned short portNr)
{
    Application::getConfigManager().setRemoteServiceAddress(mServiceName, mConnectType, address);
    Application::getConfigManager().setRemoteServicePort(mServiceName, mConnectType, portNr);
}

bool ConnectionConfiguration::isConfigured(void) const
{
    return Application::isConfigured();
}

bool ConnectionConfiguration::getConnectionEnableFlag( void ) const
{
    return Application::getConfigManager().getRemoteServiceEnable(mServiceName, mConnectType);
}

void ConnectionConfiguration::setConnectionEnableFlag(bool isEnabled)
{
    Application::getConfigManager().setRemoteServiceEnable(mServiceName, mConnectType, isEnabled);
}

unsigned short ConnectionConfiguration::getConnectionPort( void ) const
{
    return Application::getConfigManager().getRemoteServicePort(mServiceName, mConnectType);
}

void ConnectionConfiguration::setConnectionPort(unsigned short portNr)
{
    Application::getConfigManager().setRemoteServicePort(mServiceName, mConnectType, portNr);
}

bool ConnectionConfiguration::getConnectionIpAddress( unsigned char & OUT field0
                                                    , unsigned char & OUT field1
                                                    , unsigned char & OUT field2
                                                    , unsigned char & OUT field3 )
{
    bool result = false;
    field0 = field1 = field2 = field3 = 0u;

    String addr{ getConnectionAddress() };
    if ( addr.isEmpty() == false )
    {
        const char * buffer = addr.getString( );
        const char * next   = nullptr;

        uint32_t f0 = String::makeUInt32(buffer, NEString::eRadix::RadixDecimal, &next);
        if ( (buffer != next) && (f0 <= 0xFFu) && (*next == NESocket::IP_SEPARATOR) )
        {
            buffer = next + 1;
            uint32_t f1 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
            if ( (buffer != next) && (f1 <= 0xFFu) && (*next == NESocket::IP_SEPARATOR) )
            {
                buffer = next + 1;
                uint32_t f2 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
                if ( (buffer != next) && (f2 <= 0xFFu) && (*next == NESocket::IP_SEPARATOR) )
                {
                    buffer = next + 1;
                    uint32_t f3 = String::makeUInt32( buffer, NEString::eRadix::RadixDecimal, &next );
                    if ( (buffer != next) && (f3 <= 0xFFu) )
                    {
                        field0 = static_cast<unsigned char>(f0);
                        field1 = static_cast<unsigned char>(f1);
                        field2 = static_cast<unsigned char>(f2);
                        field3 = static_cast<unsigned char>(f3);
                        result = true;
                    }
                }
            }
        }
    }

    return result;
}
