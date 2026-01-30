/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyConnectEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Connection event class implementation.
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
    : ServiceResponseEvent  ( target
                            , NEService::eResultType::DataOK
                            , static_cast<unsigned int>(NEService::eFuncIdRange::ResponseServiceProviderConnection)
                            , Event::eEventType::EventLocalProxyConnect )
    , mStubAddress          ( implAddress )
    , mConnectionStatus     ( connectStatus )
{
}

ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target, const ProxyConnectEvent & src )
    : ServiceResponseEvent  ( target, static_cast<const ServiceResponseEvent &>(src) )
    , mStubAddress          ( src.mStubAddress )
    , mConnectionStatus     ( src.mConnectionStatus )
{
}

ProxyConnectEvent::ProxyConnectEvent(const IEInStream & stream)
    : ServiceResponseEvent  ( stream )
    , mStubAddress          ( stream )
    , mConnectionStatus     ( NEService::eServiceConnection::ServiceConnectionUnknown )
{
     stream >> mConnectionStatus;
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
