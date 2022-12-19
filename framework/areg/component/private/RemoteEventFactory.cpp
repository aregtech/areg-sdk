/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Factory class. Creates event
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

#include "areg/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);
DEF_TRACE_SCOPE(areg_component_RemoteEventFactory_createStreamFromEvent);
DEF_TRACE_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

StreamableEvent * RemoteEventFactory::createEventFromStream( const RemoteMessage & stream, const Channel & comChannel )
{
    TRACE_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);

    StreamableEvent * result = nullptr;
    Event::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Going to create event [ %s ] from remote message object", Event::getString(eventType));

    switch ( eventType )
    {
    case Event::eEventType::EventRemoteServiceRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( NEService::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::findStubByAddress(addrStub);
            if ( stub != nullptr )
            {
                stream.moveToBegin();
                RemoteRequestEvent * eventRequest = stub->createRemoteRequestEvent(stream);
                if ( eventRequest != nullptr )
                {
                    Channel chTarget( stub->getAddress().getChannel() );
                    Channel chSource( static_cast<ITEM_ID>(comChannel.getSource())
                                    , static_cast<ITEM_ID>(chTarget.getSource())
                                    , static_cast<ITEM_ID>(stream.getSource()) );
                    eventRequest->setTargetChannel(chTarget);
                    eventRequest->setSourceChannel(chSource);

                    TRACE_DBG("Created Event::eEventType::EventRemoteServiceRequest for target stub [ %s ] from source proxy [ %s ]."
                                , StubAddress::convAddressToPath(eventRequest->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventRequest->getEventSource()).getString());
                }

                result = static_cast<StreamableEvent *>(eventRequest);
            }
        }
        break;

    case Event::eEventType::EventRemoteNotifyRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( NEService::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::findStubByAddress(addrStub);
            if ( stub != nullptr )
            {
                stream.moveToBegin();
                RemoteNotifyRequestEvent * eventNotify = stub->createRemoteNotifyRequestEvent(stream);
                if ( eventNotify != nullptr )
                {
                    Channel chTarget( stub->getAddress().getChannel() );
                    Channel chSource( static_cast<ITEM_ID>(comChannel.getSource())
                                    , static_cast<ITEM_ID>(chTarget.getSource())
                                    , static_cast<ITEM_ID>(stream.getSource()) );
                    eventNotify->setTargetChannel(chTarget);
                    eventNotify->setSourceChannel(chSource);

                    TRACE_DBG("Created Event::eEventType::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]."
                                , StubAddress::convAddressToPath(eventNotify->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventNotify->getEventSource()).getString());
                }

                result = static_cast<StreamableEvent *>(eventNotify);
            }
        }
        break;

    case Event::eEventType::EventRemoteServiceResponse:
        {
            ProxyBase::lockProxyResource();
            ProxyAddress addrProxy;
            stream >> addrProxy;
            if ( comChannel.getCookie() == addrProxy.getCookie() )
                addrProxy.setCookie( NEService::COOKIE_LOCAL );
            ProxyBase * proxy = ProxyBase::findProxyByAddress(addrProxy);
            if ( proxy != nullptr )
            {
                stream.moveToBegin();
                RemoteResponseEvent * eventResponse = proxy->createRemoteResponseEvent(stream);
                if ( eventResponse != nullptr )
                {
                    Channel chTarget( proxy->getProxyAddress().getChannel() );
                    eventResponse->setTargetChannel(chTarget);

                    TRACE_DBG("Created Event::eEventType::EventRemoteServiceResponse for target proxy [ %s ]."
                                , ProxyAddress::convAddressToPath(eventResponse->getTargetProxy()).getString());
                }
                result = static_cast<StreamableEvent *>(eventResponse);
            }

            ProxyBase::unlockProxyResource();
        }
        break;

    case Event::eEventType::EventRemoteStubConnect:     // fall through
    case Event::eEventType::EventRemoteProxyConnect:
        break;

    case Event::eEventType::EventLocalStubConnect:      // fall through
    case Event::eEventType::EventLocalProxyConnect:     // fall through
    case Event::eEventType::EventLocalServiceRequest:   // fall through
    case Event::eEventType::EventLocalNotifyRequest:    // fall through
    case Event::eEventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case Event::eEventType::EventCustom:                // fall through
    case Event::eEventType::EventCustomInternal:        // fall through
    case Event::eEventType::EventCustomExternal:        // fall through
    case Event::eEventType::EventNotifyClient:          // fall through
    case Event::eEventType::EventConnect:               // fall through
    case Event::eEventType::EventToProxy:               // fall through
    case Event::eEventType::EventToStub:                // fall through
    case Event::eEventType::EventNotify:                // fall through
    case Event::eEventType::EventRemote:                // fall through
    case Event::eEventType::EventLocal:                 // fall through
    case Event::eEventType::EventExternal:              // fall through
    case Event::eEventType::EventInternal:              // fall through
    case Event::eEventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        TRACE_ERR("Unexpected event value [ %d ]", static_cast<int>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT((result == nullptr) || (result->getEventType() == eventType));
    return result;
}

bool RemoteEventFactory::createStreamFromEvent( RemoteMessage & stream, const StreamableEvent & eventStreamable, const Channel & comChannel )
{
    bool result = false;
    stream.invalidate();

    switch ( eventStreamable.getEventType() )
    {
    case Event::eEventType::EventRemoteServiceRequest:
        {
            const ServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( NEMemory::MESSAGE_SUCCESS );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case Event::eEventType::EventRemoteNotifyRequest:
        {
            const ServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( NEMemory::MESSAGE_SUCCESS );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case Event::eEventType::EventRemoteServiceResponse:
        {
            const ServiceResponseEvent * proxyEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceResponseEvent);
            if ( proxyEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if ( stream.isValid() )
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( proxyEvent->getTargetProxy().getCookie() );
                    stream.setMessageId( proxyEvent->getResponseId() );
                    stream.setResult( NEMemory::MESSAGE_SUCCESS );
                    stream.setSequenceNr( proxyEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case Event::eEventType::EventRemoteStubConnect:     // fall through
    case Event::eEventType::EventRemoteProxyConnect:
        break;

    case Event::eEventType::EventLocalStubConnect:      // fall through
    case Event::eEventType::EventLocalProxyConnect:     // fall through
    case Event::eEventType::EventLocalServiceRequest:   // fall through
    case Event::eEventType::EventLocalNotifyRequest:    // fall through
    case Event::eEventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case Event::eEventType::EventCustom:                // fall through
    case Event::eEventType::EventCustomInternal:        // fall through
    case Event::eEventType::EventCustomExternal:        // fall through
    case Event::eEventType::EventNotifyClient:          // fall through
    case Event::eEventType::EventConnect:               // fall through
    case Event::eEventType::EventToProxy:               // fall through
    case Event::eEventType::EventToStub:                // fall through
    case Event::eEventType::EventNotify:                // fall through
    case Event::eEventType::EventRemote:                // fall through
    case Event::eEventType::EventLocal:                 // fall through
    case Event::eEventType::EventExternal:              // fall through
    case Event::eEventType::EventInternal:              // fall through
    case Event::eEventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        {
            TRACE_SCOPE( areg_component_RemoteEventFactory_createStreamFromEvent );
            TRACE_ERR( "Unexpected event value [ %d ]", static_cast<int>(eventStreamable.getEventType( )) );
            ASSERT( false );  // unsupported remote streaming events
        }
        break;
    }

    return result;
}

StreamableEvent * RemoteEventFactory::createRequestFailedEvent( const RemoteMessage & stream, const Channel & /* comChannel */ )
{
    TRACE_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

    StreamableEvent * result = nullptr;
    Event::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Creating request failed event of type [ %s ] from remote message stream", Event::getString(eventType));

    switch ( eventType )
    {
    case Event::eEventType::EventRemoteServiceRequest:
        {
            stream.moveToBegin();
            RemoteRequestEvent eventRequest(stream);
            const ProxyAddress & addrProxy = eventRequest.getEventSource();
            result = static_cast<StreamableEvent *>( ProxyBase::createRequestFailureEvent( addrProxy
                                                                                         , eventRequest.getRequestId()
                                                                                         , NEService::eResultType::MessageUndelivered
                                                                                         , eventRequest.getSequenceNumber()) );
        }
        break;

    case Event::eEventType::EventRemoteNotifyRequest:
        {
            stream.moveToBegin();
            RemoteNotifyRequestEvent eventNotify(stream);
            const ProxyAddress & addrProxy = eventNotify.getEventSource();
            result = static_cast<StreamableEvent *>( ProxyBase::createRequestFailureEvent( addrProxy
                                                                                         , eventNotify.getRequestId()
                                                                                         , NEService::eResultType::MessageUndelivered
                                                                                         , eventNotify.getSequenceNumber()) );
        }
        break;

    case Event::eEventType::EventRemoteServiceResponse:
        break;  // not required

    case Event::eEventType::EventRemoteStubConnect:     // fall through
    case Event::eEventType::EventRemoteProxyConnect:
        break;

    case Event::eEventType::EventLocalServiceRequest:   // fall through
    case Event::eEventType::EventLocalNotifyRequest:    // fall through
    case Event::eEventType::EventLocalServiceResponse:  // fall through
    case Event::eEventType::EventLocalStubConnect:      // fall through
    case Event::eEventType::EventLocalProxyConnect:
        ASSERT(false);
        break;

    case Event::eEventType::EventCustom:                // fall through
    case Event::eEventType::EventCustomInternal:        // fall through
    case Event::eEventType::EventCustomExternal:        // fall through
    case Event::eEventType::EventNotifyClient:          // fall through
    case Event::eEventType::EventConnect:               // fall through
    case Event::eEventType::EventToProxy:               // fall through
    case Event::eEventType::EventToStub:                // fall through
    case Event::eEventType::EventNotify:                // fall through
    case Event::eEventType::EventRemote:                // fall through
    case Event::eEventType::EventLocal:                 // fall through
    case Event::eEventType::EventExternal:              // fall through
    case Event::eEventType::EventInternal:              // fall through
    case Event::eEventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        TRACE_ERR("Unexpected event value [ %d ]", static_cast<int>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}
