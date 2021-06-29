/************************************************************************
 * \file        areg/component/private/ProxyConnectEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Proxy Connection event class implementation.
 *
 ************************************************************************/
#include "areg/component/private/ProxyConnectEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(ProxyConnectEvent, ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target, const StubAddress & implAddress, NEService::eServiceConnection connectStatus )
    : ServiceResponseEvent  ( target, NEService::RESULT_DATA_OK, NEService::SI_SERVICE_CONNECTION_NOTIFY, Event::EventLocalProxyConnect )
    , mStubAddress          ( implAddress )
    , mConnectionStatus     ( connectStatus )
{
    ; // do nothing
}

ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target, const ProxyConnectEvent & src )
    : ServiceResponseEvent  ( target, static_cast<const ServiceResponseEvent &>(src) )
    , mStubAddress          ( src.mStubAddress )
    , mConnectionStatus     ( src.mConnectionStatus )
{
    ; // do nothing
}

ProxyConnectEvent::ProxyConnectEvent(const IEInStream & stream)
    : ServiceResponseEvent  ( stream )
    , mStubAddress          ( stream )
    , mConnectionStatus     ( NEService::ServiceConnectionUnknown )
{
     stream >> mConnectionStatus;
}

ProxyConnectEvent::~ProxyConnectEvent( void )
{
    ; // do nothing
}

const IEInStream & ProxyConnectEvent::readStream(const IEInStream & stream)
{
    ServiceResponseEvent::readStream(stream);
    stream >> mStubAddress;
    stream >> mConnectionStatus;
    return stream;
}

IEOutStream & ProxyConnectEvent::writeStream(IEOutStream & stream) const
{
    ServiceResponseEvent::writeStream(stream);
    stream << mStubAddress;
    stream << mConnectionStatus;
    return stream;
}
