/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ProxyEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Proxy Event class implementation.
 *
 ************************************************************************/
#include "areg/component/ProxyEvent.hpp"

#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ResponseEvents.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyEvent class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ProxyEvent::ProxyEvent( const ProxyAddress & toTarget, areg::EventType eventType )
    : Event (eventType)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
        toTarget.to_event(*hdr);
}

ProxyEvent::ProxyEvent(const ProxyAddress& toTarget, const EventEnvelope& src)
    : Event (src)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
        toTarget.to_event(*hdr);
}

ProxyEvent::ProxyEvent(const ProxyAddress& toTarget, EventEnvelope&& src)
    : Event(std::move(src))
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
        toTarget.to_event(*hdr);
}

ProxyEvent::ProxyEvent(const EventEnvelope& envelope)
    : Event(envelope)
{
}

ProxyEvent::ProxyEvent(EventEnvelope&& envelope) noexcept
    : Event(std::move(envelope))
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
ProxyEventConsumer::ProxyEventConsumer( const ProxyAddress & proxy )
    : EventConsumer ( )
    , mProxyAddress ( proxy )
{
}

//////////////////////////////////////////////////////////////////////////
// ProxyEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////

void ProxyEventConsumer::start_event_processing( Event & eventElem )
{
    const areg::EventType eventType{ eventElem.event_type() };
    const EventEnvelope& envelope{ eventElem.envelope() };
    ASSERT(envelope.is_valid());

    switch (eventType)
    {
    case areg::EventType::EventLocalConsumerConnect:
    case areg::EventType::EventRemoteConsumerConnect:
    {
        const areg::EventHeader* hdr{ envelope.header() };
        ASSERT(hdr != nullptr);

        // Option A: local events route directly to the stub thread; remote events route
        // via the RouterClient thread (hdr->channel) so the proxy's Channel.mTarget is set
        // correctly for both cases, satisfying ProxyBase::service_connection_updated assertion.
        const uint32_t chTarget{ areg::is_local(eventType) ? hdr->provider.thread : hdr->channel };
        Channel ch{ hdr->consumer.thread, chTarget, hdr->consumer.id };
        areg::ServiceConnectionState status{ static_cast<areg::ServiceConnectionState>(hdr->result) };
        StubAddress prov(*hdr);
        service_connection_updated(prov, ch, status);
    }
    break;

    case areg::EventType::EventLocalResponse:
    case areg::EventType::EventRemoteResponse:
    {
        // consumer.number == static_cast<uint32_t>(ProxyAddress) is the correct identity
        // check: consumer.number = CRC32(service+type+role+thread) = ProxyAddress magic number.
        if (envelope.consumer_number() == static_cast<uint32_t>(mProxyAddress))
        {
            process_response_event(static_cast<ServiceResponseEvent&>(eventElem));
        }
    }
    break;

    case areg::EventType::EventLocalBroadcast:
    case areg::EventType::EventRemoteBroadcast:
    {
        if (envelope.consumer_number() == static_cast<uint32_t>(mProxyAddress))
        {
            process_broadcast_event(static_cast<ServiceResponseEvent&>(eventElem));
        }
    }
    break;

    case areg::EventType::EventLocalAttribute:
    case areg::EventType::EventRemoteAttribute:
    {
        if (envelope.consumer_number() == static_cast<uint32_t>(mProxyAddress))
        {
            process_attribute_event(static_cast<ServiceResponseEvent&>(eventElem));
        }
    }
    break;

    case areg::EventType::EventLocalRequestFailed:
    case areg::EventType::EventRemoteRequestFailed:
    {
        if (envelope.consumer_number() == static_cast<uint32_t>(mProxyAddress))
        {
            process_request_failed_event(static_cast<ServiceResponseEvent&>(eventElem));
        }
    }
    break;

    case areg::EventType::EventCustomExternal:
    case areg::EventType::EventCustomInternal:
    {
        process_custom_event(eventElem);
    }
    break;

    default:
    {
        if (is_to_consumer(eventType))
            process_proxy_event(static_cast<ProxyEvent&>(eventElem));
        else
            process_generic_event(eventElem);
    }
    break;
    }
}

} // namespace areg
