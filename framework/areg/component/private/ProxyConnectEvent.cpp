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
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyConnectEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(ProxyConnectEvent, ServiceResponseEvent)

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target, const StubAddress & implAddress, areg::ServiceConnectionState connectStatus )
    : ServiceResponseEvent  ( target
                            , areg::ResultType::DataOK
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , Event::EventType::EventLocalProxyConnect )
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

ProxyConnectEvent::ProxyConnectEvent(const InStream & stream)
    : ServiceResponseEvent  ( stream )
    , mStubAddress          ( stream )
    , mConnectionStatus     ( areg::ServiceConnectionState::Unknown )
{
     stream >> mConnectionStatus;
}

const InStream & ProxyConnectEvent::read_stream(const InStream & stream)
{
    ServiceResponseEvent::read_stream(stream);
    stream >> mStubAddress;
    stream >> mConnectionStatus;
    return stream;
}

OutStream & ProxyConnectEvent::write_stream(OutStream & stream) const
{
    ServiceResponseEvent::write_stream(stream);
    stream << mStubAddress;
    stream << mConnectionStatus;
    return stream;
}

} // namespace areg
