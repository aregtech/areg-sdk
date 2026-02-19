/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubConnectEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Stub Connection event class declaration.
 *
 ************************************************************************/
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/IOStream.hpp"

AREG_IMPLEMENT_RUNTIME_EVENT(StubConnectEvent, ServiceRequestEvent)

StubConnectEvent::StubConnectEvent(const StubAddress & stubTarget, NEService::ServiceConnectionState connectStatus)
    : ServiceRequestEvent   ( ProxyAddress::getInvalidProxyAddress()
                            , stubTarget
                            , static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderConnection)
                            , NEService::RequestType::ServiceConnection
                            , Event::EventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent(const ProxyAddress & proxyClient, const StubAddress & stubTarget, NEService::ServiceConnectionState connectStatus)
    : ServiceRequestEvent   ( proxyClient
                            , stubTarget
                            , static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderConnection)
                            , NEService::RequestType::ClientConnection
                            , Event::EventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent( const InStream & stream )
    : ServiceRequestEvent   ( stream )
    , mConnectionStatus    ( NEService::ServiceConnectionState::Unknown )
{
    stream >> mConnectionStatus;
}

const InStream & StubConnectEvent::readStream(const InStream & stream)
{
    ServiceRequestEvent::readStream(stream);
    stream >> mConnectionStatus;
    return stream;
}

OutStream & StubConnectEvent::writeStream(OutStream & stream) const
{
    ServiceRequestEvent::writeStream(stream);
    stream << mConnectionStatus;
    return stream;
}
