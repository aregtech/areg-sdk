/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubConnectEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Stub Connection event class declaration.
 *
 ************************************************************************/
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/IEIOStream.hpp"

IMPLEMENT_RUNTIME_EVENT(StubConnectEvent, ServiceRequestEvent)

StubConnectEvent::StubConnectEvent(const StubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : ServiceRequestEvent   ( ProxyAddress::INVALID_PROXY_ADDRESS
                            , stubTarget
                            , static_cast<unsigned int>(NEService::eFuncIdRange::ServiceNotifyConnection)
                            , NEService::eRequestType::ServiceConnection
                            , Event::eEventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent(const ProxyAddress & proxyClient, const StubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : ServiceRequestEvent   ( proxyClient
                            , stubTarget
                            , static_cast<unsigned int>(NEService::eFuncIdRange::ServiceNotifyConnection)
                            , NEService::eRequestType::ClientConnection
                            , Event::eEventType::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
}

StubConnectEvent::StubConnectEvent( const IEInStream & stream )
    : ServiceRequestEvent   ( stream )
    ,  mConnectionStatus    ( NEService::eServiceConnection::ServiceConnectionUnknown )
{
    stream >> mConnectionStatus;
}

const IEInStream & StubConnectEvent::readStream(const IEInStream & stream)
{
    ServiceRequestEvent::readStream(stream);
    stream >> mConnectionStatus;
    return stream;
}

IEOutStream & StubConnectEvent::writeStream(IEOutStream & stream) const
{
    ServiceRequestEvent::writeStream(stream);
    stream << mConnectionStatus;
    return stream;
}
