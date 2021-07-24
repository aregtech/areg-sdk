/************************************************************************
 * \file        areg/component/RemoteEventFactory.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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

RemoteEventFactory::RemoteEventFactory(void)
{
    ; // do nothing
}

RemoteEventFactory::~RemoteEventFactory(void)
{
    ; // do nothing
}

StreamableEvent * RemoteEventFactory::createEventFromStream( const RemoteMessage & stream, const Channel & comChannel )
{
    TRACE_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);

    StreamableEvent * result = NULL;
    Event::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Going to create event [ %s ] from remote message object", Event::getString(eventType));

    switch ( eventType )
    {
    case Event::EventRemoteServiceRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( NEService::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::findStubByAddress(addrStub);
            if ( stub != NULL )
            {
                stream.moveToBegin();
                RemoteRequestEvent * eventRequest = stub->createRemoteRequestEvent(stream);
                if ( eventRequest != NULL )
                {
                    Channel chTarget( stub->getAddress().getChannel() );
                    Channel chSource( static_cast<ITEM_ID>(comChannel.getSource()), static_cast<ITEM_ID>(chTarget.getSource()), static_cast<ITEM_ID>(stream.getSource()) );
                    eventRequest->setTargetChannel(chTarget);
                    eventRequest->setSourceChannel(chSource);

                    TRACE_DBG("Created event Event::EventRemoteServiceRequest for target stub [ %s ] from source proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ], Source [ ch = %p, src = %p, trg = %p ]"
                                , StubAddress::convAddressToPath(eventRequest->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventRequest->getEventSource()).getString()
                                , static_cast<id_type>(chTarget.getCookie())
                                , static_cast<id_type>(chTarget.getSource())
                                , static_cast<id_type>(chTarget.getTarget())
                                , static_cast<id_type>(chSource.getCookie())
                                , static_cast<id_type>(chSource.getSource())
                                , static_cast<id_type>(chSource.getTarget()) );
                }

                result = static_cast<StreamableEvent *>(eventRequest);
            }
        }        
        break;

    case Event::EventRemoteNotifyRequest:
        {
            StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( NEService::COOKIE_LOCAL );
            }

            const StubBase * stub = StubBase::findStubByAddress(addrStub);
            if ( stub != NULL )
            {
                stream.moveToBegin();
                RemoteNotifyRequestEvent * eventNotify = stub->createRemoteNotifyRequestEvent(stream);
                if ( eventNotify != NULL )
                {
                    Channel chTarget( stub->getAddress().getChannel() );
                    Channel chSource( static_cast<ITEM_ID>(comChannel.getSource()), static_cast<ITEM_ID>(chTarget.getSource()), static_cast<ITEM_ID>(stream.getSource()) );
                    eventNotify->setTargetChannel(chTarget);
                    eventNotify->setSourceChannel(chSource);

                    TRACE_DBG("Created event Event::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ], Source [ ch = %p, src = %p, trg = %p ]"
                                , StubAddress::convAddressToPath(eventNotify->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventNotify->getEventSource()).getString()
                                , static_cast<id_type>(chTarget.getCookie())
                                , static_cast<id_type>(chTarget.getSource())
                                , static_cast<id_type>(chTarget.getTarget())
                                , static_cast<id_type>(chSource.getCookie())
                                , static_cast<id_type>(chSource.getSource())
                                , static_cast<id_type>(chSource.getTarget()) );
                }
                result = static_cast<StreamableEvent *>(eventNotify);
            }
        }
        break;

    case Event::EventRemoteServiceResponse:
        {
            ProxyAddress addrProxy;
            stream >> addrProxy;
            if ( comChannel.getCookie() == addrProxy.getCookie() )
                addrProxy.setCookie( NEService::COOKIE_LOCAL );
            ProxyBase * proxy = ProxyBase::findProxyByAddress(addrProxy);
            if ( proxy != NULL )
            {
                stream.moveToBegin();
                RemoteResponseEvent * eventResponse = proxy->createRemoteResponseEvent(stream);
                if ( eventResponse != NULL )
                {
                    Channel chTarget( proxy->getProxyAddress().getChannel() );
                    eventResponse->setTargetChannel(chTarget);

                    TRACE_DBG("Created event Event::EventRemoteServiceResponse for target proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ]"
                                , ProxyAddress::convAddressToPath(eventResponse->getTargetProxy()).getString()
                                , static_cast<id_type>(chTarget.getCookie())
                                , static_cast<id_type>(chTarget.getSource())
                                , static_cast<id_type>(chTarget.getTarget()) );
                }
                result = static_cast<StreamableEvent *>(eventResponse);
            }
        }
        break;

    case Event::EventRemoteStubConnect:     // fall through
    case Event::EventRemoteProxyConnect:    // fall through
        break;

    case Event::EventLocalStubConnect:      // fall through
    case Event::EventLocalProxyConnect:     // fall through
    case Event::EventLocalServiceRequest:   // fall through
    case Event::EventLocalNotifyRequest:    // fall through
    case Event::EventLocalServiceResponse:  // fall through
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case Event::EventCustom:                // fall through
    case Event::EventCustomInternal:        // fall through
    case Event::EventCustomExternal:        // fall through
    case Event::EventNotifyClient:          // fall through
    case Event::EventConnect:               // fall through
    case Event::EventToProxy:               // fall through
    case Event::EventToStub:                // fall through
    case Event::EventNotify:                // fall through
    case Event::EventRemote:                // fall through
    case Event::EventLocal:                 // fall through
    case Event::EventExternal:              // fall through
    case Event::EventInternal:              // fall through
    case Event::EventUnknown:               // fall through
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        TRACE_ERR("Unexpected event value [ %d ]", static_cast<int>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT((result == NULL) || (result->getEventType() == eventType));
    return result;
}

bool RemoteEventFactory::createStreamFromEvent( RemoteMessage & stream, const StreamableEvent & eventStreamable, const Channel & comChannel )
{
    TRACE_SCOPE(areg_component_RemoteEventFactory_createStreamFromEvent);

    bool result = false;
    stream.invalidate();

    TRACE_DBG("Going to stream event [ %s ] for remote messaging", Event::getString( eventStreamable.getEventType()) );

    switch ( eventStreamable.getEventType() )
    {
    case Event::EventRemoteServiceRequest:
        {
            const ServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != NULL )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( NEMemory::ResultSucceed );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();

                    TRACE_DBG("Created message [ %p ] from event Event::EventRemoteServiceRequest. Target [ %p ], Source [ %p ]"
                                , static_cast<id_type>(stream.getMessageId())
                                , static_cast<id_type>(stream.getTarget())
                                , static_cast<id_type>(stream.getSource()));
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case Event::EventRemoteNotifyRequest:
        {
            const ServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceRequestEvent);
            if ( stubEvent != NULL )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( NEMemory::ResultSucceed );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();

                    TRACE_DBG("Created message [ %p ] from event Event::EventRemoteNotifyRequest. Target [ %p ], Source [ %p ]"
                                , static_cast<id_type>(stream.getMessageId())
                                , static_cast<id_type>(stream.getTarget())
                                , static_cast<id_type>(stream.getSource()));
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case Event::EventRemoteServiceResponse:
        {
            const ServiceResponseEvent * proxyEvent = RUNTIME_CONST_CAST(&eventStreamable, ServiceResponseEvent);
            if ( proxyEvent != NULL )
            {
                eventStreamable.writeStream(stream);
                if ( stream.isValid() )
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( proxyEvent->getTargetProxy().getCookie() );
                    stream.setMessageId( proxyEvent->getResponseId() );
                    stream.setResult( NEMemory::ResultSucceed );
                    stream.setSequenceNr( proxyEvent->getSequenceNumber() );
                    stream.bufferCompletionFix();

                    TRACE_DBG("Created message [ %p ] from event Event::EventRemoteServiceResponse Target [ %p ], Source [ %p ]"
                                , static_cast<id_type>(stream.getMessageId())
                                , static_cast<id_type>(stream.getTarget())
                                , static_cast<id_type>(stream.getSource()));
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case Event::EventRemoteStubConnect:     // fall through
    case Event::EventRemoteProxyConnect:    // fall through
        break;

    case Event::EventLocalStubConnect:      // fall through
    case Event::EventLocalProxyConnect:     // fall through
    case Event::EventLocalServiceRequest:   // fall through
    case Event::EventLocalNotifyRequest:    // fall through
    case Event::EventLocalServiceResponse:  // fall through
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case Event::EventCustom:                // fall through
    case Event::EventCustomInternal:        // fall through
    case Event::EventCustomExternal:        // fall through
    case Event::EventNotifyClient:          // fall through
    case Event::EventConnect:               // fall through
    case Event::EventToProxy:               // fall through
    case Event::EventToStub:                // fall through
    case Event::EventNotify:                // fall through
    case Event::EventRemote:                // fall through
    case Event::EventLocal:                 // fall through
    case Event::EventExternal:              // fall through
    case Event::EventInternal:              // fall through
    case Event::EventUnknown:               // fall through
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        TRACE_ERR("Unexpected event value [ %d ]", static_cast<int>(eventStreamable.getEventType()));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}

StreamableEvent * RemoteEventFactory::createRequestFailedEvent( const RemoteMessage & stream, const Channel & /* comChannel */ )
{
    TRACE_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

    StreamableEvent * result = NULL;
    Event::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Creating request failed event of type [ %s ] from remote message stream", Event::getString(eventType));

    switch ( eventType )
    {
    case Event::EventRemoteServiceRequest:
        {
            stream.moveToBegin();
            RemoteRequestEvent eventRequest(stream);
            const ProxyAddress & addrProxy = eventRequest.getEventSource();
            result = static_cast<StreamableEvent *>( ProxyBase::createRequestFailureEvent(addrProxy, eventRequest.getRequestId(), NEService::RESULT_MESSAGE_UNDELIVERED, eventRequest.getSequenceNumber()) );
        }        
        break;

    case Event::EventRemoteNotifyRequest:
        {
            stream.moveToBegin();
            RemoteNotifyRequestEvent eventNotify(stream);
            const ProxyAddress & addrProxy = eventNotify.getEventSource();
            result = static_cast<StreamableEvent *>( ProxyBase::createRequestFailureEvent(addrProxy, eventNotify.getRequestId(), NEService::RESULT_MESSAGE_UNDELIVERED, eventNotify.getSequenceNumber()) );
        }
        break;

    case Event::EventRemoteServiceResponse:
        break;  // not required

    case Event::EventRemoteStubConnect:     // fall through
    case Event::EventRemoteProxyConnect:
        break;

    case Event::EventLocalServiceRequest:   // fall through
    case Event::EventLocalNotifyRequest:    // fall through
    case Event::EventLocalServiceResponse:  // fall through
    case Event::EventLocalStubConnect:      // fall through
    case Event::EventLocalProxyConnect:     // fall through
        ASSERT(false);
        break;

    case Event::EventCustom:                // fall through
    case Event::EventCustomInternal:        // fall through
    case Event::EventCustomExternal:        // fall through
    case Event::EventNotifyClient:          // fall through
    case Event::EventConnect:               // fall through
    case Event::EventToProxy:               // fall through
    case Event::EventToStub:                // fall through
    case Event::EventNotify:                // fall through
    case Event::EventRemote:                // fall through
    case Event::EventLocal:                 // fall through
    case Event::EventExternal:              // fall through
    case Event::EventInternal:              // fall through
    case Event::EventUnknown:               // fall through
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        TRACE_ERR("Unexpected event value [ %d ]", static_cast<int>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}
