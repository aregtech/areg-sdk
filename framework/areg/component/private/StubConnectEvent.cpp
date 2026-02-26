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

AREG_IMPLEMENT_RUNTIME_EVENT(StubConnectEvent, areg::ServiceRequestEvent)

StubConnectEvent::StubConnectEvent(const areg::StubAddress & stubTarget, areg::ServiceConnectionState connectStatus)
    : areg::ServiceRequestEvent   ( ProxyAddress::getInvalidProxyAddress()
                            , stubTarget
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , areg::RequestType::ServiceConnection
                            , Event::EventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent(const ProxyAddress & proxyClient, const areg::StubAddress & stubTarget, areg::ServiceConnectionState connectStatus)
    : areg::ServiceRequestEvent   ( proxyClient
                            , stubTarget
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , areg::RequestType::ClientConnection
                            , Event::EventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent( const areg::InStream & stream )
    : areg::ServiceRequestEvent   ( stream )
    , mConnectionStatus    ( areg::ServiceConnectionState::Unknown )
{
    stream >> mConnectionStatus;
}

const areg::InStream & StubConnectEvent::readStream(const areg::InStream & stream)
{
    areg::ServiceRequestEvent::readStream(stream);
    stream >> mConnectionStatus;
    return stream;
}

areg::OutStream & StubConnectEvent::writeStream(areg::OutStream & stream) const
{
    areg::ServiceRequestEvent::writeStream(stream);
    stream << mConnectionStatus;
    return stream;
}
