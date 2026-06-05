/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/RemoteEventFactory.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Remote Event Factory implementation.
 ************************************************************************/
#include "areg/component/RemoteEventFactory.hpp"

#include "areg/base/EventEnvelope.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/StubAddress.hpp"
#include "areg/component/ComponentThread.hpp"

#include "areg/logging/areg_log.h"
namespace areg {
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, route_outgoing_message);
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, route_incoming_message);
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, create_request_failed_event);

bool RemoteEventFactory::route_outgoing_message( Event & srcWire, const Channel & comChannel )
{
    // carry request_id()/response_id() and sequence_number(). Only the process-routing
    // cookies need to be stamped, in place, on the event's own buffer.
    areg::EventHeader * hdr = srcWire.header();
    if ( hdr == nullptr )
        return false;

    bool result = false;

    switch ( static_cast<areg::EventType>(hdr->eventType) )
    {
    case areg::EventType::EventRemoteRequest:           // fall through
    case areg::EventType::EventRemoteNotifyRequest:
        hdr->source = comChannel.cookie();
        hdr->target = hdr->provider.id;     // stub's process cookie
        // NOTE: do NOT touch hdr->result here. On the request path the `result` field carries the
        // RequestType (CallFunction / StartNotify / StopNotify / RemoveAllNotify) — see
        // ServiceRequestEvent::request_type(). Overwriting it broke IPC notify subscriptions
        // (StartNotify decoded as 0 on the stub side), so broadcasts/attribute updates were never sent.
        result = true;
        break;

    case areg::EventType::EventRemoteResponse:
        hdr->source = comChannel.cookie();
        hdr->target = hdr->consumer.id;     // ServiceResponseEvent::target_cookie(); result preserved
        result = true;
        break;

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteConsumerConnect:
        break;

    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventLocalConsumerConnect:    // fall through
    case areg::EventType::EventLocalRequest:            // fall through
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventLocalResponse:
        ASSERT(false);  // local events must not be serialized for remote transmission
        break;

    case areg::EventType::EventCustomInternal:          // fall through
    case areg::EventType::EventCustomExternal:          // fall through
    case areg::EventType::EventNotifyClient:            // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event serialization
        break;

    default:
        {
            DEBUG_LOG_SCOPE( areg_component_RemoteEventFactory, route_outgoing_message);
            DEBUG_LOG_ERR( "Unexpected event value [ %d ]", static_cast<int32_t>(hdr->eventType) );
            ASSERT( false );
        }
        break;
    }

    return result;
}

bool RemoteEventFactory::route_incoming_message( EventEnvelope & src, const Channel & comChannel )
{
    const areg::EventHeader * hdrPtr = src.header();
    if ((hdrPtr == nullptr) || !src.is_valid())
        return false;

    const areg::EventType eventType{ static_cast<areg::EventType>(hdrPtr->eventType) };

    switch ( eventType )
    {
    case areg::EventType::EventRemoteRequest:       // fall through
    case areg::EventType::EventRemoteNotifyRequest:
    {
        UniqueNumber stubId = hdrPtr->provider.number;
        const StubBase * stub = StubBase::find_stub(stubId);
        if ( stub == nullptr )
            return false;

        const Channel chTarget(stub->address().channel());
        // chSource encodes the response routing path back through this RouterClient
        const Channel chSource(comChannel.source(), chTarget.source(), src.source());

        // Move the receive buffer into the delivered event (no shared_ptr refcount bump); all reads
        // from src/hdrPtr above are complete, so src may be consumed here.
        Event evt(std::move(src));
        areg::EventHeader * hdr = evt.header();
        ASSERT(hdr != nullptr);
        // Route to stub's local thread
        hdr->target          = chTarget.cookie();
        hdr->channel         = chTarget.target();
        hdr->provider.id     = chTarget.cookie();
        hdr->provider.thread = chTarget.source();
        // Response routing key back through RouterClient
        hdr->source          = chSource.target();
        hdr->consumer.id     = chSource.cookie();
        hdr->consumer.thread = chSource.source();
        hdr->internal2       = 0;

        evt.register_for_thread(&stub->component_thread());
        evt.deliver_event();
        return true;
    }

    case areg::EventType::EventRemoteResponse:
    {
        UniqueNumber proxyId = hdrPtr->consumer.number;
        const std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy(proxyId);
        if ( !proxy )
            return false;

        const Channel chTarget(proxy->proxy_address().channel());

        // Move the receive buffer into the delivered event (no shared_ptr refcount bump); all reads
        // from src/hdrPtr above are complete, so src may be consumed here.
        Event evt(std::move(src));
        areg::EventHeader * hdr = evt.header();
        ASSERT(hdr != nullptr);
        // Route response to proxy's local thread
        hdr->target          = chTarget.cookie();
        hdr->channel         = chTarget.target();
        hdr->consumer.id     = chTarget.cookie();
        hdr->consumer.thread = chTarget.source();
        hdr->internal2       = 0;

        evt.register_for_thread(&proxy->proxy_dispatcher_thread());
        evt.deliver_event();
        return true;
    }

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteConsumerConnect:
        return true;    // handled by service_connection_event, not the executable path

    default:
        return false;
    }
}

Event RemoteEventFactory::create_request_failed_event( const EventEnvelope & stream, const Channel & /* comChannel */ )
{
    DEBUG_LOG_SCOPE( areg_component_RemoteEventFactory, create_request_failed_event);

    const areg::EventHeader* hdr = stream.header();
    if (hdr == nullptr)
        return Event{};

    const areg::EventType eventType{ static_cast<areg::EventType>(hdr->eventType) };

    DEBUG_LOG_DBG("Creating request failed event of type [ %s ] from remote message stream", areg::as_string(eventType));

    switch ( eventType )
    {
    case areg::EventType::EventRemoteRequest:       // fall through
    case areg::EventType::EventRemoteNotifyRequest:
        return ProxyBase::request_failure_event( ProxyAddress(*hdr)
                                               , hdr->messageId
                                               , areg::ResultType::MessageUndelivered
                                               , hdr->sequenceNr );

    case areg::EventType::EventRemoteResponse:      // fall through
    case areg::EventType::EventRemoteProviderConnect:// fall through
    case areg::EventType::EventRemoteConsumerConnect:
        break;

    default:
        DEBUG_LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);
        break;
    }

    return Event{};
}

} // namespace areg
