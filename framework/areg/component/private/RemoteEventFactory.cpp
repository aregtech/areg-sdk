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
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Channel.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"

#include "areg/logging/areg_log.h"
namespace areg {
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, event_from_stream);
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, stream_from_event);
DEBUG_DEF_LOG_SCOPE(areg_component_RemoteEventFactory, request_failed_event);

Event RemoteEventFactory::event_from_stream( const EventEnvelope & stream, const Channel & comChannel )
{
    DEBUG_LOG_SCOPE( areg_component_RemoteEventFactory, event_from_stream);

    Event result;

    areg::EventHeader hdr{ };
    stream.read(reinterpret_cast<uint8_t*>(&hdr), sizeof(areg::EventHeader));
    const areg::EventType eventType{ static_cast<areg::EventType>(hdr.eventType) };

    DEBUG_LOG_DBG("Going to create event [ %s ] from remote message object", areg::as_string(eventType));

    switch ( eventType )
    {
    case areg::EventType::EventRemoteRequest:
        {
            StubAddress addrStub(hdr);
            if ( comChannel.cookie() == addrStub.cookie() )
                addrStub.set_cookie( areg::COOKIE_LOCAL );

            const StubBase * stub = StubBase::find_stub(addrStub);
            if ( stub != nullptr )
            {
                Channel chTarget(stub->address().channel());
                Channel chSource(comChannel.source(), chTarget.source(), stream.source());
                RemoteRequestEvent evtRequest = stub->create_remote_request(stream);
                if ( evtRequest.is_valid() )
                {
                    evtRequest.set_target_channel(chTarget);
                    evtRequest.set_source_channel(chSource);
                    evtRequest.register_for_thread(&stub->component_thread());

                    DEBUG_LOG_DBG("Created areg::EventType::EventRemoteRequest for target stub [ %s ] from source proxy [ %s ]."
                                    , StubAddress::to_path(stub->address()).as_string()
                                    , ProxyAddress::to_path(evtRequest.event_source()).as_string());
                    result = evtRequest;
                }
            }
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            StubAddress addrStub(hdr);
            if ( comChannel.cookie() == addrStub.cookie() )
                addrStub.set_cookie( areg::COOKIE_LOCAL );

            const StubBase * stub = StubBase::find_stub(addrStub);
            if ( stub != nullptr )
            {
                Channel chTarget(stub->address().channel());
                Channel chSource(comChannel.source(), chTarget.source(), stream.source());
                RemoteNotifyRequestEvent evtNotify = stub->create_notify_request(stream);
                if ( evtNotify.is_valid() )
                {
                    evtNotify.set_target_channel(chTarget);
                    evtNotify.set_source_channel(chSource);
                    evtNotify.register_for_thread(&stub->component_thread());

                    DEBUG_LOG_DBG("Created areg::EventType::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]."
                                    , StubAddress::to_path(stub->address()).as_string()
                                    , ProxyAddress::to_path(evtNotify.event_source()).as_string());
                    result = evtNotify;
                }
            }
        }
        break;

    case areg::EventType::EventRemoteResponse:
        {
            ProxyAddress addrProxy(hdr);
            if ( comChannel.cookie() == addrProxy.cookie() )
                addrProxy.set_cookie( areg::COOKIE_LOCAL );

            std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy(addrProxy);
            if ( proxy != nullptr )
            {
                Channel chTarget(proxy->proxy_address().channel());
                RemoteResponseEvent evtResponse = proxy->create_remote_response(stream);
                if ( evtResponse.is_valid() )
                {
                    evtResponse.set_target_channel(chTarget);
                    evtResponse.register_for_thread(&proxy->proxy_dispatcher_thread());

                    DEBUG_LOG_DBG("Created areg::EventType::EventRemoteResponse for target proxy [ %s ]."
                                    , ProxyAddress::to_path(addrProxy).as_string());
                    result = evtResponse;
                }
            }
        }
        break;

    case areg::EventType::EventRemoteProviderConnect: // fall through
    case areg::EventType::EventRemoteConsumerConnect:
        break;

    case areg::EventType::EventLocalProviderConnect:  // fall through
    case areg::EventType::EventLocalConsumerConnect:  // fall through
    case areg::EventType::EventLocalRequest:          // fall through
    case areg::EventType::EventLocalNotifyRequest:    // fall through
    case areg::EventType::EventLocalResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::EventType::EventUnknown:
        break;

    case areg::EventType::EventCustomInternal:        // fall through
    case areg::EventType::EventCustomExternal:        // fall through
    case areg::EventType::EventNotifyClient:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        DEBUG_LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT(!result.is_valid() || (static_cast<areg::EventType>(result.event_type()) == eventType));
    return result;
}

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
                stream.set_result( areg::MESSAGE_SUCCESS );
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

    Event result;
    areg::EventHeader hdr{ };
    stream.read(reinterpret_cast<uint8_t*>(&hdr), sizeof(areg::EventHeader));
    const areg::EventType eventType{ static_cast<areg::EventType>(hdr.eventType) };

    DEBUG_LOG_DBG("Creating request failed event of type [ %s ] from remote message stream", areg::as_string(eventType));

    switch ( eventType )
    {
    case areg::EventType::EventRemoteRequest:
        {
            stream.move_to_begin();
            RemoteRequestEvent evtRequest(stream);
            result = ProxyBase::request_failure_event( evtRequest.event_source()
                                                     , evtRequest.request_id()
                                                     , areg::ResultType::MessageUndelivered
                                                     , evtRequest.sequence_number() );
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            stream.move_to_begin();
            RemoteNotifyRequestEvent evtNotify(stream);
            result = ProxyBase::request_failure_event( evtNotify.event_source()
                                                     , evtNotify.request_id()
                                                     , areg::ResultType::MessageUndelivered
                                                     , evtNotify.sequence_number() );
        }
        break;

    case areg::EventType::EventRemoteResponse:
        break;  // not required

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteConsumerConnect:
        break;

    case areg::EventType::EventLocalRequest:            // fall through
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventLocalResponse:           // fall through
    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventLocalConsumerConnect:
        ASSERT(false);
        break;

    case areg::EventType::EventCustomInternal:          // fall through
    case areg::EventType::EventCustomExternal:          // fall through
    case areg::EventType::EventNotifyClient:            // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        DEBUG_LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}

} // namespace areg
