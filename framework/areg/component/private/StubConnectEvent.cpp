/************************************************************************
 * \file        areg/component/private/StubConnectEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Connection event class declaration.
 *
 ************************************************************************/
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/base/IEIOStream.hpp"

IMPLEMENT_RUNTIME_EVENT(StubConnectEvent, ServiceRequestEvent)

StubConnectEvent::StubConnectEvent(const StubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : ServiceRequestEvent   ( ProxyAddress::INVALID_PROXY_ADDRESS, stubTarget, NEService::SI_SERVICE_CONNECTION_NOTIFY, NEService::REQUEST_CONNECTION, Event::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

StubConnectEvent::StubConnectEvent(const ProxyAddress & proxyClient, const StubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : ServiceRequestEvent   ( proxyClient, stubTarget, NEService::SI_SERVICE_CONNECTION_NOTIFY, NEService::REQUEST_CLIENT_CONNECTION, Event::EventLocalStubConnect)
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

StubConnectEvent::StubConnectEvent( const IEInStream & stream )
    : ServiceRequestEvent   ( stream )
    ,  mConnectionStatus    ( NEService::ServiceConnectionUnknown )
{
    stream >> mConnectionStatus;
}

StubConnectEvent::~StubConnectEvent(void)
{
    ; // do nothing
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
