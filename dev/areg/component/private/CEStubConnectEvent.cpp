/************************************************************************
 * \file        areg/component/private/CEStubConnectEvent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Stub Connection event class declaration.
 *
 ************************************************************************/
#include "areg/component/private/CEStubConnectEvent.hpp"
#include "areg/component/CEStubAddress.hpp"
#include "areg/component/CEProxyAddress.hpp"
#include "areg/base/IEIOStream.hpp"

IMPLEMENT_RUNTIME_EVENT(CEStubConnectEvent, CEServiceRequestEvent)

CEStubConnectEvent::CEStubConnectEvent(const CEStubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : CEServiceRequestEvent ( CEProxyAddress::INVALID_PROXY_ADDRESS, stubTarget, NEService::SI_SERVICE_CONNECTION_NOTIFY, NEService::REQUEST_CONNECTION, CEEvent::EventStubConnect)
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

CEStubConnectEvent::CEStubConnectEvent(const CEProxyAddress & proxyClient, const CEStubAddress & stubTarget, NEService::eServiceConnection connectStatus)
    : CEServiceRequestEvent ( proxyClient, stubTarget, NEService::SI_SERVICE_CONNECTION_NOTIFY, NEService::REQUEST_CLIENT_CONNECTION, CEEvent::EventStubConnect)
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

CEStubConnectEvent::CEStubConnectEvent( const IEInStream & stream )
    : CEServiceRequestEvent ( stream )
    ,  mConnectionStatus    ( NEService::ServiceConnectionUnknown )
{
    stream >> mConnectionStatus;
}

CEStubConnectEvent::~CEStubConnectEvent(void)
{
    ; // do nothing
}

const IEInStream & CEStubConnectEvent::ReadFromStream(const IEInStream & stream)
{
    CEServiceRequestEvent::ReadFromStream(stream);
    stream >> mConnectionStatus;
    return stream;
}

IEOutStream & CEStubConnectEvent::WriteToStream(IEOutStream & stream) const
{
    CEServiceRequestEvent::WriteToStream(stream);
    stream << mConnectionStatus;
    return stream;
}
