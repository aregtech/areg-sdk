/************************************************************************
 * \file        areg/src/component/CERemoteEventFactory.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Factory class. Creates event
 *              from stream and converts event to stream.
 ************************************************************************/
#include "areg/src/component/CERemoteEventFactory.hpp"
#include "areg/src/base/CERemoteMessage.hpp"
#include "areg/src/component/CERequestEvents.hpp"
#include "areg/src/component/CEResponseEvents.hpp"
#include "areg/src/base/CEProcess.hpp"
#include "areg/src/component/private/CEProxyConnectEvent.hpp"
#include "areg/src/component/private/CEStubConnectEvent.hpp"
#include "areg/src/component/CEStubBase.hpp"
#include "areg/src/component/CEProxyBase.hpp"
#include "areg/src/component/CEChannel.hpp"

#include "areg/src/trace/GETrace.h"
DEF_TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateEventFromStream);
DEF_TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateStreamFromEvent);
DEF_TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateRequestFailedEvent);

CERemoteEventFactory::CERemoteEventFactory(void)
{
    ; // do nothing
}

CERemoteEventFactory::~CERemoteEventFactory(void)
{
    ; // do nothing
}

CEStreamableEvent * CERemoteEventFactory::CreateEventFromStream( const CERemoteMessage & stream, const CEChannel & comChannel )
{
    TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateEventFromStream);

    CEStreamableEvent * result = NULL;
    CEEvent::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Going to create event [ %s ] from remote message object", CEEvent::GetString(eventType));

    switch ( eventType )
    {
    case CEEvent::EventRemoteServiceRequest:
        {
            CEStubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.GetCookie() == addrStub.GetCookie() )
                addrStub.SetCookie( NEService::COOKIE_LOCAL );
            const CEStubBase * stub = CEStubBase::FindStubByAddress(addrStub);
            if ( stub != NULL )
            {
                stream.MoveToBeginOfData();
                CERemoteRequestEvent * eventRequest = stub->CreateRemoteServiceRequestEvent(stream);
                if ( eventRequest != NULL )
                {
                    CEChannel chTarget( stub->GetAddress().GetChannel() );
                    CEChannel chSource( comChannel.GetSource(), chTarget.GetSource(), stream.GetSource() );
                    eventRequest->SetTargetChannel(chTarget);
                    eventRequest->SetSourceChannel(chSource);

                    TRACE_DBG("Created event CEEvent::EventRemoteServiceRequest for target stub [ %s ] from source proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ], Source [ ch = %p, src = %p, trg = %p ]"
                                , CEStubAddress::ConvertAddressToPath(eventRequest->GetTargetStub()).GetBuffer()
                                , CEProxyAddress::ConvertAddressToPath(eventRequest->GetEventSource()).GetBuffer()
                                , chTarget.GetCookie(), chTarget.GetSource(), chTarget.GetTarget()
                                , chSource.GetCookie(), chSource.GetSource(), chSource.GetTarget() );
                }

                result = static_cast<CEStreamableEvent *>(eventRequest);
            }
        }        
        break;

    case CEEvent::EventRemoteNotifyRequest:
        {
            CEStubAddress addrStub;
            stream >> addrStub;
            if ( comChannel.GetCookie() == addrStub.GetCookie() )
                addrStub.SetCookie( NEService::COOKIE_LOCAL );
            const CEStubBase * stub = CEStubBase::FindStubByAddress(addrStub);
            if ( stub != NULL )
            {
                stream.MoveToBeginOfData();
                CERemoteNotifyRequestEvent * eventNotify = stub->CreateRemoteNotifyRequestEvent(stream);
                if ( eventNotify != NULL )
                {
                    CEChannel chTarget( stub->GetAddress().GetChannel() );
                    CEChannel chSource( comChannel.GetSource(), chTarget.GetSource(), stream.GetSource() );
                    eventNotify->SetTargetChannel(chTarget);
                    eventNotify->SetSourceChannel(chSource);

                    TRACE_DBG("Created event CEEvent::EventRemoteNotifyRequest for target stub [ %s ] from source proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ], Source [ ch = %p, src = %p, trg = %p ]"
                                , CEStubAddress::ConvertAddressToPath(eventNotify->GetTargetStub()).GetBuffer()
                                , CEProxyAddress::ConvertAddressToPath(eventNotify->GetEventSource()).GetBuffer()
                                , chTarget.GetCookie(), chTarget.GetSource(), chTarget.GetTarget()
                                , chSource.GetCookie(), chSource.GetSource(), chSource.GetTarget() );
                }
                result = static_cast<CEStreamableEvent *>(eventNotify);
            }
        }
        break;

    case CEEvent::EventRemoteServiceResponse:
        {
            CEProxyAddress addrProxy;
            stream >> addrProxy;
            if ( comChannel.GetCookie() == addrProxy.GetCookie() )
                addrProxy.SetCookie( NEService::COOKIE_LOCAL );
            CEProxyBase * proxy = CEProxyBase::FindProxyByAddress(addrProxy);
            if ( proxy != NULL )
            {
                stream.MoveToBeginOfData();
                CERemoteResponseEvent * eventResponse = proxy->CreateRemoteResponseEvent(stream);
                if ( eventResponse != NULL )
                {
                    CEChannel chTarget( proxy->GetProxyAddress().GetChannel() );
                    eventResponse->SetTargetChannel(chTarget);

                    TRACE_DBG("Created event CEEvent::EventRemoteServiceResponse for target proxy [ %s ]. Target: [ ch = %p, src = %p, trg = %p ]"
                                , CEProxyAddress::ConvertAddressToPath(eventResponse->GetTargetProxy()).GetBuffer()
                                , chTarget.GetCookie(), chTarget.GetSource(), chTarget.GetTarget() );
                }
                result = static_cast<CEStreamableEvent *>(eventResponse);
            }
        }
        break;

    case CEEvent::EventStubConnect:
    case CEEvent::EventProxyConnect:
    case CEEvent::EventLocalServiceRequest:
    case CEEvent::EventLocalNotifyRequest:
    case CEEvent::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    default:
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT(result == NULL || result->GetEventType() == eventType);
    return result;
}

bool CERemoteEventFactory::CreateStreamFromEvent( CERemoteMessage & stream, const CEStreamableEvent & eventStreamable, const CEChannel & comChannel )
{
    TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateStreamFromEvent);

    bool result = false;
    stream.InvalidateBuffer();

    TRACE_DBG("Going to stream event [ %s ] for remote messaging", CEEvent::GetString( eventStreamable.GetEventType() ));

    switch ( eventStreamable.GetEventType() )
    {
    case CEEvent::EventRemoteServiceRequest:
        {
            const CEServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, CEServiceRequestEvent);
            if ( stubEvent != NULL )
            {
                eventStreamable.WriteToStream(stream);
                if (stream.IsValid())
                {
                    result = true;
                    stream.SetSource( comChannel.GetCookie() );
                    stream.SetTarget( stubEvent->GetTargetStub().GetCookie() );
                    stream.SetMessageId( stubEvent->GetRequestId() );
                    stream.SetResult( NEMemory::ResultSucceed );
                    stream.SetSequenceNr( stubEvent->GetSequenceNumber() );
                    stream.BufferCompletionFix();

                    TRACE_DBG("Created message [ %p ] from event CEEvent::EventRemoteServiceRequest. Target [ %p ], Source [ %p ]"
                                , stream.GetMessageId()
                                , stream.GetTarget()
                                , stream.GetSource());
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case CEEvent::EventRemoteNotifyRequest:
        {
            const CEServiceRequestEvent * stubEvent = RUNTIME_CONST_CAST(&eventStreamable, CEServiceRequestEvent);
            if ( stubEvent != NULL )
            {
                eventStreamable.WriteToStream(stream);
                if (stream.IsValid())
                {
                    result = true;
                    stream.SetSource( comChannel.GetCookie() );
                    stream.SetTarget( stubEvent->GetTargetStub().GetCookie() );
                    stream.SetMessageId( stubEvent->GetRequestId() );
                    stream.SetResult( NEMemory::ResultSucceed );
                    stream.SetSequenceNr( stubEvent->GetSequenceNumber() );
                    stream.BufferCompletionFix();

                    TRACE_DBG("Created message [ 0x%08X ] from event CEEvent::EventRemoteNotifyRequest. Target [ %p ], Source [ %p ]"
                                , stream.GetMessageId()
                                , stream.GetTarget()
                                , stream.GetSource());
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case CEEvent::EventRemoteServiceResponse:
        {
            const CEServiceResponseEvent * proxyEvent = RUNTIME_CONST_CAST(&eventStreamable, CEServiceResponseEvent);
            if ( proxyEvent != NULL )
            {
                eventStreamable.WriteToStream(stream);
                if ( stream.IsValid() )
                {
                    result = true;
                    stream.SetSource( comChannel.GetCookie() );
                    stream.SetTarget( proxyEvent->GetTargetProxy().GetCookie() );
                    stream.SetMessageId( proxyEvent->GetResponseId() );
                    stream.SetResult( NEMemory::ResultSucceed );
                    stream.SetSequenceNr( proxyEvent->GetSequenceNumber() );
                    stream.BufferCompletionFix();

                    TRACE_DBG("Created message [ %p ] from event CEEvent::EventRemoteServiceResponse Target [ %p ], Source [ %p ]"
                                , stream.GetMessageId()
                                , stream.GetTarget()
                                , stream.GetSource());
                }
            }
            else
            {
                ; // ignore, invalid event
            }
        }
        break;

    case CEEvent::EventStubConnect:
    case CEEvent::EventProxyConnect:
        break;

    case CEEvent::EventLocalServiceRequest:
    case CEEvent::EventLocalNotifyRequest:
    case CEEvent::EventLocalServiceResponse:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    default:
        ASSERT(false);  // unsupported remote streaming events
        break;
    }
    return result;
}

CEStreamableEvent * CERemoteEventFactory::CreateRequestFailedEvent( const CERemoteMessage & stream
                                                                  , const CEChannel & /* comChannel */ )
{
    TRACE_SCOPE(areg_component_CERemoteEventFactory_CreateRequestFailedEvent);

    CEStreamableEvent * result = NULL;
    CEEvent::eEventType eventType;
    stream >> eventType;

    TRACE_DBG("Creating request failed event of type from remote message stream", CEEvent::GetString(eventType));

    switch ( eventType )
    {
    case CEEvent::EventRemoteServiceRequest:
        {
            stream.MoveToBeginOfData();
            CERemoteRequestEvent eventRequest(stream);
            const CEProxyAddress & addrProxy = eventRequest.GetEventSource();
            CEProxyBase * proxy = CEProxyBase::FindProxyByAddress(addrProxy);
            if ( proxy != NULL )
            {
                TRACE_DBG("Found Proxy by address [ %s ], creating request failed event", CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer());
                result = static_cast<CEStreamableEvent *>( proxy->CreateRemoteRequestFailedEvent(addrProxy, eventRequest.GetRequestId(), NEService::RESULT_MESSAGE_UNDELIVERED, eventRequest.GetSequenceNumber()) );
            }
        }        
        break;

    case CEEvent::EventRemoteNotifyRequest:
        {
            stream.MoveToBeginOfData();
            CERemoteNotifyRequestEvent eventNotify(stream);
            const CEProxyAddress & addrProxy = eventNotify.GetEventSource();
            CEProxyBase * proxy = CEProxyBase::FindProxyByAddress(addrProxy);
            if ( proxy != NULL )
            {
                TRACE_DBG("Found Proxy by address [ %s ], creating notification request failed event", CEProxyAddress::ConvertAddressToPath(addrProxy).GetBuffer());
                result = static_cast<CEStreamableEvent *>( proxy->CreateRemoteRequestFailedEvent(addrProxy, eventNotify.GetRequestId(), NEService::RESULT_MESSAGE_UNDELIVERED, eventNotify.GetSequenceNumber()) );
            }
        }
        break;

    case CEEvent::EventRemoteServiceResponse:
        break;  // not required

    case CEEvent::EventLocalServiceRequest:
    case CEEvent::EventLocalNotifyRequest:
    case CEEvent::EventLocalServiceResponse:
    case CEEvent::EventStubConnect:
    case CEEvent::EventProxyConnect:
        ASSERT(false);  // unexpected streaming for remote events
        break;

    default:
        ASSERT(false);  // unsupported remote streaming events
        break;
    }

    ASSERT(result == NULL || result->GetEventType() == eventType);
    return result;
}
