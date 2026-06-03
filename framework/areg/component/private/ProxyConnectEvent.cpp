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
ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target
                                    , const StubAddress & server
                                    , areg::ServiceConnectionState connectStatus )
    : ServiceResponseEvent  ( target
                            , areg::ResultType::DataOK
                            , static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection)
                            , areg::EventType::EventLocalConsumerConnect )
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
    {
        hdr->eventId = ProxyConnectEvent::CLASS_ID;
        // Overwrite DataOK with the actual connection status; result() reads this back.
        hdr->result = static_cast<uint32_t>(connectStatus);
        // Store stub address in the provider endpoint so stub_address() can reconstruct it.
        server.to_event(*hdr);
    }
}

ProxyConnectEvent::ProxyConnectEvent( const ProxyAddress & target, const ProxyConnectEvent & src )
    : ServiceResponseEvent  ( target, src.envelope().clone() )
{
}

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

StubAddress ProxyConnectEvent::stub_address() const
{
    const areg::EventHeader* hdr{ header() };
    return (hdr != nullptr ? StubAddress(*hdr) : StubAddress::invalid_stub_address());
}

} // namespace areg
