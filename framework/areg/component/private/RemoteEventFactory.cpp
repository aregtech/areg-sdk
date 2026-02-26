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

#include "areg/logging/GELog.h"
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createStreamFromEvent);
DEF_LOG_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

areg::StreamableEvent * RemoteEventFactory::createEventFromStream( const areg::RemoteMessage & stream, const areg::Channel & comChannel )
{
    LOG_SCOPE(areg_component_RemoteEventFactory_createEventFromStream);

    areg::StreamableEvent * result = nullptr;
    areg::Event::EventType eventType;
    stream >> eventType;

    LOG_DBG("Going to create event [ %s ] from remote message object", areg::Event::getString(eventType));

    switch ( eventType )
    {
    case areg::Event::EventType::EventRemoteServiceRequest:
        {
            areg::StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( areg::COOKIE_LOCAL );
            }

            const areg::StubBase * stub = areg::StubBase::findStubByAddress(addrStub);
            if ( stub != nullptr )
            {
                stream.moveToBegin();
                RemoteRequestEvent * eventRequest = stub->createRemoteRequestEvent(stream);
                if ( eventRequest != nullptr )
                {
                    areg::Channel chTarget( stub->getAddress().getChannel() );
                    areg::Channel chSource( comChannel.getSource(), chTarget.getSource(), stream.getSource() );
                    eventRequest->setTargetChannel(chTarget);
                    eventRequest->setSourceChannel(chSource);

                    LOG_DBG("Created Event::EventType::EventRemoteServiceRequest for target stub [ %s ] from source proxy [ %s ]."
                                , areg::StubAddress::convAddressToPath(eventRequest->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventRequest->getEventSource()).getString());
                }

                result = static_cast<areg::StreamableEvent *>(eventRequest);
            }
        }
        break;

    case areg::Event::EventType::EventRemoteNotifyRequest:
        {
            areg::StubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.getCookie() == addrStub.getCookie() )
            {
                addrStub.setCookie( areg::COOKIE_LOCAL );
            }

            const areg::StubBase * stub = areg::StubBase::findStubByAddress(addrStub);
            if ( stub != nullptr )
            {
                stream.moveToBegin();
                RemoteNotifyRequestEvent * eventNotify = stub->createRemoteNotifyRequestEvent(stream);
                if ( eventNotify != nullptr )
                {
                    areg::Channel chTarget( stub->getAddress().getChannel() );
                    areg::Channel chSource( comChannel.getSource(), chTarget.getSource(), stream.getSource() );
                    eventNotify->setTargetChannel(chTarget);
                    eventNotify->setSourceChannel(chSource);

                    LOG_DBG("Created Event::EventType::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]."
                                , areg::StubAddress::convAddressToPath(eventNotify->getTargetStub()).getString()
                                , ProxyAddress::convAddressToPath(eventNotify->getEventSource()).getString());
                }

                result = static_cast<areg::StreamableEvent *>(eventNotify);
            }
        }
        break;

    case areg::Event::EventType::EventRemoteServiceResponse:
        {
            ProxyBase::lockProxyResource();
            ProxyAddress addrProxy;
            stream >> addrProxy;
            if ( comChannel.getCookie() == addrProxy.getCookie() )
                addrProxy.setCookie( areg::COOKIE_LOCAL );
            std::shared_ptr<ProxyBase> proxy = ProxyBase::findProxyByAddress(addrProxy);
            if ( proxy != nullptr )
            {
                stream.moveToBegin();
                areg::RemoteResponseEvent * eventResponse = proxy->createRemoteResponseEvent(stream);
                if ( eventResponse != nullptr )
                {
                    areg::Channel chTarget( proxy->getProxyAddress().getChannel() );
                    eventResponse->setTargetChannel(chTarget);

                    LOG_DBG("Created Event::EventType::EventRemoteServiceResponse for target proxy [ %s ]."
                                , ProxyAddress::convAddressToPath(eventResponse->getTargetProxy()).getString());
                }

                result = static_cast<areg::StreamableEvent *>(eventResponse);
            }

            ProxyBase::unlockProxyResource();
        }
        break;

    case areg::Event::EventType::EventRemoteStubConnect:     // fall through
    case areg::Event::EventType::EventRemoteProxyConnect:
        break;

    case areg::Event::EventType::EventLocalStubConnect:      // fall through
    case areg::Event::EventType::EventLocalProxyConnect:     // fall through
    case areg::Event::EventType::EventLocalServiceRequest:   // fall through
    case areg::Event::EventType::EventLocalNotifyRequest:    // fall through
    case areg::Event::EventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::Event::EventType::EventCustom:                // fall through
    case areg::Event::EventType::EventCustomInternal:        // fall through
    case areg::Event::EventType::EventCustomExternal:        // fall through
    case areg::Event::EventType::EventNotifyClient:          // fall through
    case areg::Event::EventType::EventConnect:               // fall through
    case areg::Event::EventType::EventToProxy:               // fall through
    case areg::Event::EventType::EventToStub:                // fall through
    case areg::Event::EventType::EventNotify:                // fall through
    case areg::Event::EventType::EventRemote:                // fall through
    case areg::Event::EventType::EventLocal:                 // fall through
    case areg::Event::EventType::EventExternal:              // fall through
    case areg::Event::EventType::EventInternal:              // fall through
    case areg::Event::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT((result == nullptr) || (result->getEventType() == eventType));
    return result;
}

bool RemoteEventFactory::createStreamFromEvent( areg::RemoteMessage & stream, const areg::StreamableEvent & eventStreamable, const areg::Channel & comChannel )
{
    bool result = false;
    stream.invalidate();

    switch ( eventStreamable.getEventType() )
    {
    case areg::Event::EventType::EventRemoteServiceRequest:
        {
            const areg::ServiceRequestEvent * stubEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, areg::ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( areg::MESSAGE_SUCCESS );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::Event::EventType::EventRemoteNotifyRequest:
        {
            const areg::ServiceRequestEvent * stubEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, areg::ServiceRequestEvent);
            if ( stubEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if (stream.isValid())
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( stubEvent->getTargetStub().getCookie() );
                    stream.setMessageId( stubEvent->getRequestId() );
                    stream.setResult( areg::MESSAGE_SUCCESS );
                    stream.setSequenceNr( stubEvent->getSequenceNumber() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::Event::EventType::EventRemoteServiceResponse:
        {
            const areg::ServiceResponseEvent * proxyEvent = AREG_RUNTIME_CONST_CAST(&eventStreamable, areg::ServiceResponseEvent);
            if ( proxyEvent != nullptr )
            {
                eventStreamable.writeStream(stream);
                if ( stream.isValid() )
                {
                    result = true;
                    stream.setSource( comChannel.getCookie() );
                    stream.setTarget( proxyEvent->getTargetProxy().getCookie() );
                    stream.setMessageId( proxyEvent->getResponseId() );
                    stream.setResult( areg::MESSAGE_SUCCESS );
                    stream.setSequenceNr( proxyEvent->getSequenceNumber() );
                }
            }
            else
            {
                // ignore, invalid event
            }
        }
        break;

    case areg::Event::EventType::EventRemoteStubConnect:     // fall through
    case areg::Event::EventType::EventRemoteProxyConnect:
        break;

    case areg::Event::EventType::EventLocalStubConnect:      // fall through
    case areg::Event::EventType::EventLocalProxyConnect:     // fall through
    case areg::Event::EventType::EventLocalServiceRequest:   // fall through
    case areg::Event::EventType::EventLocalNotifyRequest:    // fall through
    case areg::Event::EventType::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    case areg::Event::EventType::EventCustom:                // fall through
    case areg::Event::EventType::EventCustomInternal:        // fall through
    case areg::Event::EventType::EventCustomExternal:        // fall through
    case areg::Event::EventType::EventNotifyClient:          // fall through
    case areg::Event::EventType::EventConnect:               // fall through
    case areg::Event::EventType::EventToProxy:               // fall through
    case areg::Event::EventType::EventToStub:                // fall through
    case areg::Event::EventType::EventNotify:                // fall through
    case areg::Event::EventType::EventRemote:                // fall through
    case areg::Event::EventType::EventLocal:                 // fall through
    case areg::Event::EventType::EventExternal:              // fall through
    case areg::Event::EventType::EventInternal:              // fall through
    case areg::Event::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        {
            LOG_SCOPE( areg_component_RemoteEventFactory_createStreamFromEvent );
            LOG_ERR( "Unexpected event value [ %d ]", static_cast<int32_t>(eventStreamable.getEventType( )) );
            ASSERT( false );  // unsupported remote streaming events
        }
        break;
    }

    return result;
}

areg::StreamableEvent * RemoteEventFactory::createRequestFailedEvent( const areg::RemoteMessage & stream, const areg::Channel & /* comChannel */ )
{
    LOG_SCOPE(areg_component_RemoteEventFactory_createRequestFailedEvent);

    areg::StreamableEvent * result = nullptr;
    areg::Event::EventType eventType;
    stream >> eventType;

    LOG_DBG("Creating request failed event of type [ %s ] from remote message stream", areg::Event::getString(eventType));

    switch ( eventType )
    {
    case areg::Event::EventType::EventRemoteServiceRequest:
        {
            stream.moveToBegin();
            RemoteRequestEvent eventRequest(stream);
            const ProxyAddress & addrProxy = eventRequest.getEventSource();
            result = static_cast<areg::StreamableEvent *>( ProxyBase::createRequestFailureEvent( addrProxy
                                                                                         , eventRequest.getRequestId()
                                                                                         , areg::ResultType::MessageUndelivered
                                                                                         , eventRequest.getSequenceNumber()) );
        }
        break;

    case areg::Event::EventType::EventRemoteNotifyRequest:
        {
            stream.moveToBegin();
            RemoteNotifyRequestEvent eventNotify(stream);
            const ProxyAddress & addrProxy = eventNotify.getEventSource();
            result = static_cast<areg::StreamableEvent *>( ProxyBase::createRequestFailureEvent( addrProxy
                                                                                         , eventNotify.getRequestId()
                                                                                         , areg::ResultType::MessageUndelivered
                                                                                         , eventNotify.getSequenceNumber()) );
        }
        break;

    case areg::Event::EventType::EventRemoteServiceResponse:
        break;  // not required

    case areg::Event::EventType::EventRemoteStubConnect:     // fall through
    case areg::Event::EventType::EventRemoteProxyConnect:
        break;

    case areg::Event::EventType::EventLocalServiceRequest:   // fall through
    case areg::Event::EventType::EventLocalNotifyRequest:    // fall through
    case areg::Event::EventType::EventLocalServiceResponse:  // fall through
    case areg::Event::EventType::EventLocalStubConnect:      // fall through
    case areg::Event::EventType::EventLocalProxyConnect:
        ASSERT(false);
        break;

    case areg::Event::EventType::EventCustom:                // fall through
    case areg::Event::EventType::EventCustomInternal:        // fall through
    case areg::Event::EventType::EventCustomExternal:        // fall through
    case areg::Event::EventType::EventNotifyClient:          // fall through
    case areg::Event::EventType::EventConnect:               // fall through
    case areg::Event::EventType::EventToProxy:               // fall through
    case areg::Event::EventType::EventToStub:                // fall through
    case areg::Event::EventType::EventNotify:                // fall through
    case areg::Event::EventType::EventRemote:                // fall through
    case areg::Event::EventType::EventLocal:                 // fall through
    case areg::Event::EventType::EventExternal:              // fall through
    case areg::Event::EventType::EventInternal:              // fall through
    case areg::Event::EventType::EventUnknown:
        ASSERT(false);  // unexpected for remote event
        break;

    default:
        LOG_ERR("Unexpected event value [ %d ]", static_cast<int32_t>(eventType));
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    return result;
}
