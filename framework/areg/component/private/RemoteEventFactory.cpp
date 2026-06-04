/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Factory class. Creates event
 *              from stream and converts event to stream.
 ************************************************************************/
#include "areg/component/RemoteEventFactory.hpp"

#include "areg/base/EventEnvelope.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyAddress.hpp"

#include "areg/logging/areg_log.h"
namespace areg {
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, stream_from_event);
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, request_failed_event);

bool RemoteEventFactory::stream_from_event( EventEnvelope & stream, const Event & eventStreamable, const Channel & comChannel )
{
    bool result = false;

    switch ( static_cast<areg::EventType>(eventStreamable.event_type()) )
    {
    case areg::EventType::EventRemoteRequest:
        {
            const ServiceRequestEvent & reqEvent = static_cast<const ServiceRequestEvent&>(eventStreamable);
            stream = static_cast<const EventEnvelope&>(eventStreamable);  // O(1) shared-ptr copy
            if (stream.is_valid())
            {
                result = true;
                stream.set_source( comChannel.cookie() );
                stream.set_target( eventStreamable.header()->provider.id );  // stub's process cookie
                stream.set_message_id( reqEvent.request_id() );
                stream.set_result( areg::MESSAGE_SUCCESS );
                stream.set_sequence( reqEvent.sequence_number() );
            }
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            const ServiceRequestEvent & stubEvent = static_cast<const ServiceRequestEvent&>(eventStreamable);
            stream = static_cast<const EventEnvelope&>(eventStreamable);  // O(1) shared-ptr copy
            if (stream.is_valid())
            {
                result = true;
                stream.set_source( comChannel.cookie() );
                stream.set_target( eventStreamable.header()->provider.id );  // stub's process cookie
                stream.set_message_id( stubEvent.request_id() );
                stream.set_result( areg::MESSAGE_SUCCESS );
                stream.set_sequence( stubEvent.sequence_number() );
            }
        }
        break;

    case areg::EventType::EventRemoteResponse:
        {
            const ServiceResponseEvent & respEvent = static_cast<const ServiceResponseEvent&>(eventStreamable);
            stream = static_cast<const EventEnvelope&>(eventStreamable);  // O(1) shared-ptr copy
            if ( stream.is_valid() )
            {
                result = true;
                stream.set_source( comChannel.cookie() );
                stream.set_target( respEvent.target_cookie() );
                stream.set_message_id( respEvent.response_id() );
                // result field carries ResultType (RequestOK, DataOK, etc.) — preserve it from the copy.
                stream.set_sequence( respEvent.sequence_number() );
            }
        }
        break;

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteConsumerConnect:
        break;

    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventLocalConsumerConnect:    // fall through
    case areg::EventType::EventLocalRequest:            // fall through
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventLocalResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::EventType::EventCustomInternal:          // fall through
    case areg::EventType::EventCustomExternal:          // fall through
    case areg::EventType::EventNotifyClient:            // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        {
            DEBUG_LOG_SCOPE( areg_component_RemoteEventFactory, stream_from_event);
            DEBUG_LOG_ERR( "Unexpected event value [ %d ]", static_cast<int32_t>(eventStreamable.event_type( )) );
            ASSERT( false );  // unsupported remote streaming events
        }
        break;
    }

    return result;
}

Event RemoteEventFactory::request_failed_event( const EventEnvelope & stream, const Channel & /* comChannel */ )
{
    DEBUG_LOG_SCOPE( areg_component_RemoteEventFactory, request_failed_event);

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
