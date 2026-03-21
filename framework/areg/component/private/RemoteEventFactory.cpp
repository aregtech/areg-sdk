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

#include "areg/base/RemoteMessage.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/base/Process.hpp"
#include "areg/component/private/ProxyConnectEvent.hpp"
#include "areg/component/private/StubConnectEvent.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/Channel.hpp"

#include "areg/logging/areg_log.h"
namespace areg {
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createStreamFromEvent);
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

StreamableEvent * RemoteEventFactory::event_from_stream( const RemoteMessage & stream, const Channel & comChannel )
{
    LOG_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);

    StreamableEvent * result = nullptr;
    areg::EventType eventType;
    stream >> eventType;

    LOG_DBG("Going to create event [ %s ] from remote message object", areg::as_string(eventType));

    switch ( eventType )
    {
    case areg::EventType::EventRemoteServiceRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.cookie() == addrStub.cookie() )
            {
                addrStub.set_cookie( areg::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::find_stub(addrStub);
            if ( stub != nullptr )
            {
                stream.move_to_begin();
                RemoteRequestEvent * eventRequest = stub->create_remote_request(stream);
                if ( eventRequest != nullptr )
                {
                    Channel chTarget( stub->address().channel() );
                    Channel chSource( comChannel.source(), chTarget.source(), stream.source() );
                    eventRequest->set_target_channel(chTarget);
                    eventRequest->set_source_channel(chSource);

                    LOG_DBG("Created areg::EventType::EventRemoteServiceRequest for target stub [ %s ] from source proxy [ %s ]."
                                , StubAddress::to_path(eventRequest->target_stub()).as_string()
                                , ProxyAddress::to_path(eventRequest->event_source()).as_string());
                }

                result = static_cast<StreamableEvent *>(eventRequest);
            }
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.cookie() == addrStub.cookie() )
            {
                addrStub.set_cookie( areg::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::find_stub(addrStub);
            if ( stub != nullptr )
            {
                stream.move_to_begin();
                RemoteNotifyRequestEvent * eventNotify = stub->create_notify_request(stream);
                if ( eventNotify != nullptr )
                {
                    Channel chTarget( stub->address().channel() );
                    Channel chSource( comChannel.source(), chTarget.source(), stream.source() );
                    eventNotify->set_target_channel(chTarget);
                    eventNotify->set_source_channel(chSource);

                    LOG_DBG("Created areg::EventType::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]."
                                , StubAddress::to_path(eventNotify->target_stub()).as_string()
                                , ProxyAddress::to_path(eventNotify->event_source()).as_string());
                }

                result = static_cast<StreamableEvent *>(eventNotify);
            }
        }
        break;

    case areg::EventType::EventRemoteServiceResponse:
        {
            ProxyBase::lock_resource();
            ProxyAddress addrProxy;
            stream >> addrProxy;
            if ( comChannel.cookie() == addrProxy.cookie() )
                addrProxy.set_cookie( areg::COOKIE_LOCAL );
            std::shared_ptr<ProxyBase> proxy = ProxyBase::find_proxy(addrProxy);
            if ( proxy != nullptr )
            {
                stream.move_to_begin();
                RemoteResponseEvent * eventResponse = proxy->create_remote_response(stream);
                if ( eventResponse != nullptr )
                {
                    Channel chTarget( proxy->proxy_address().channel() );
                    eventResponse->set_target_channel(chTarget);

                    LOG_DBG("Created areg::EventType::EventRemoteServiceResponse for target proxy [ %s ]."
                                , ProxyAddress::to_path(eventResponse->target_proxy()).as_string());
                }

                result = static_cast<StreamableEvent *>(eventResponse);
            }

            ProxyBase::unlock_resource();
        }
        break;

    case areg::EventType::EventRemoteProviderConnect: // fall through
    case areg::EventType::EventRemoteProxyConnect:
        break;

    case areg::EventType::EventLocalProviderConnect:  // fall through
    case areg::EventType::EventLocalProxyConnect:     // fall through
    case areg::EventType::EventLocalServiceRequest:   // fall through
    case areg::EventType::EventLocalNotifyRequest:    // fall through
    case areg::EventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::EventType::EventCustom:                // fall through
    case areg::EventType::EventCustomInternal:        // fall through
    case areg::EventType::EventCustomExternal:        // fall through
    case areg::EventType::EventNotifyClient:          // fall through
    case areg::EventType::EventConnect:               // fall through
    case areg::EventType::EventToProxy:               // fall through
    case areg::EventType::EventToProvider:            // fall through
    case areg::EventType::EventNotify:                // fall through
    case areg::EventType::EventRemote:                // fall through
    case areg::EventType::EventLocal:                 // fall through
    case areg::EventType::EventExternal:              // fall through
    case areg::EventType::EventInternal:              // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT((result == nullptr) || (result->event_type() == eventType));
    return result;
}

bool RemoteEventFactory::stream_from_event( RemoteMessage & stream, const StreamableEvent & eventStreamable, const Channel & comChannel )
{
    bool result = false;
    stream.invalidate();

    switch ( eventStreamable.event_type() )
    {
    case areg::EventType::EventRemoteServiceRequest:
        {
            const ServiceRequestEvent * stubEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.write_stream(stream);
                if (stream.is_valid())
                {
                    result = true;
                    stream.set_source( comChannel.cookie() );
                    stream.set_target( stubEvent->target_stub().cookie() );
                    stream.set_message_id( stubEvent->request_id() );
                    stream.set_result( areg::MESSAGE_SUCCESS );
                    stream.set_sequence( stubEvent->sequence_number() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            const ServiceRequestEvent * stubEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.write_stream(stream);
                if (stream.is_valid())
                {
                    result = true;
                    stream.set_source( comChannel.cookie() );
                    stream.set_target( stubEvent->target_stub().cookie() );
                    stream.set_message_id( stubEvent->request_id() );
                    stream.set_result( areg::MESSAGE_SUCCESS );
                    stream.set_sequence( stubEvent->sequence_number() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::EventType::EventRemoteServiceResponse:
        {
            const ServiceResponseEvent * proxyEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, ServiceResponseEvent);
            if ( proxyEvent != nullptr )
            {
                eventStreamable.write_stream(stream);
                if ( stream.is_valid() )
                {
                    result = true;
                    stream.set_source( comChannel.cookie() );
                    stream.set_target( proxyEvent->target_proxy().cookie() );
                    stream.set_message_id( proxyEvent->response_id() );
                    stream.set_result( areg::MESSAGE_SUCCESS );
                    stream.set_sequence( proxyEvent->sequence_number() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteProxyConnect:
        break;

    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventLocalProxyConnect:       // fall through
    case areg::EventType::EventLocalServiceRequest:     // fall through
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::EventType::EventCustom:                  // fall through
    case areg::EventType::EventCustomInternal:          // fall through
    case areg::EventType::EventCustomExternal:          // fall through
    case areg::EventType::EventNotifyClient:            // fall through
    case areg::EventType::EventConnect:                 // fall through
    case areg::EventType::EventToProxy:                 // fall through
    case areg::EventType::EventToProvider:              // fall through
    case areg::EventType::EventNotify:                  // fall through
    case areg::EventType::EventRemote:                  // fall through
    case areg::EventType::EventLocal:                   // fall through
    case areg::EventType::EventExternal:                // fall through
    case areg::EventType::EventInternal:                // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        {
            LOG_SCOPE( areg_component_RemoteEventFactory_createStreamFromEvent );
            LOG_ERR( "Unexpected event value [ %d ]", static_cast<int32_t>(eventStreamable.event_type( )) );
            ASSERT( false );  // unsupported remote streaming events
        }
        break;
    }

    return result;
}

StreamableEvent * RemoteEventFactory::request_failed_event( const RemoteMessage & stream, const Channel & /* comChannel */ )
{
    LOG_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

    StreamableEvent * result = nullptr;
    areg::EventType eventType;
    stream >> eventType;

    LOG_DBG("Creating request failed event of type [ %s ] from remote message stream", areg::as_string(eventType));

    switch ( eventType )
    {
    case areg::EventType::EventRemoteServiceRequest:
        {
            stream.move_to_begin();
            RemoteRequestEvent eventRequest(stream);
            const ProxyAddress & addrProxy = eventRequest.event_source();
            result = static_cast<StreamableEvent *>( ProxyBase::request_failure_event( addrProxy
                                                                                     , eventRequest.request_id()
                                                                                     , areg::ResultType::MessageUndelivered
                                                                                     , eventRequest.sequence_number()) );
        }
        break;

    case areg::EventType::EventRemoteNotifyRequest:
        {
            stream.move_to_begin();
            RemoteNotifyRequestEvent eventNotify(stream);
            const ProxyAddress & addrProxy = eventNotify.event_source();
            result = static_cast<StreamableEvent *>( ProxyBase::request_failure_event( addrProxy
                                                                                         , eventNotify.request_id()
                                                                                         , areg::ResultType::MessageUndelivered
                                                                                         , eventNotify.sequence_number()) );
        }
        break;

    case areg::EventType::EventRemoteServiceResponse:
        break;  // not required

    case areg::EventType::EventRemoteProviderConnect:   // fall through
    case areg::EventType::EventRemoteProxyConnect:
        break;

    case areg::EventType::EventLocalServiceRequest:     // fall through
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventLocalServiceResponse:    // fall through
    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventLocalProxyConnect:
        ASSERT(false);
        break;

    case areg::EventType::EventCustom:                  // fall through
    case areg::EventType::EventCustomInternal:          // fall through
    case areg::EventType::EventCustomExternal:          // fall through
    case areg::EventType::EventNotifyClient:            // fall through
    case areg::EventType::EventConnect:                 // fall through
    case areg::EventType::EventToProxy:                 // fall through
    case areg::EventType::EventToProvider:              // fall through
    case areg::EventType::EventNotify:                  // fall through
    case areg::EventType::EventRemote:                  // fall through
    case areg::EventType::EventLocal:                   // fall through
    case areg::EventType::EventExternal:                // fall through
    case areg::EventType::EventInternal:                // fall through
    case areg::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}

} // namespace areg
