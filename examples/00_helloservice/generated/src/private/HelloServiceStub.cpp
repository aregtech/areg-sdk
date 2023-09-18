//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloServiceStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:47 GMT+02:00
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/HelloServiceStub.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/HelloServiceStub.hpp"
#include "generated/src/private/HelloServiceEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloServiceStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloServiceStub::HelloServiceStub( Component & masterComp )
    : StubBase    ( masterComp, NEHelloService::getInterfaceData() )
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void HelloServiceStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_HelloServiceStub_startupServiceInterface);
void HelloServiceStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloServiceStub_startupServiceInterface);

    HelloServiceRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloServiceNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloServiceStub_shutdownServiceIntrface);
void HelloServiceStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloServiceStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    HelloServiceRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloServiceNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * HelloServiceStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW HelloServiceResponseEvent(data, proxy, result, msgId) : DEBUG_NEW HelloServiceResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * HelloServiceStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW HelloServiceRequestEvent(stream) );
}

RemoteNotifyRequestEvent * HelloServiceStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW HelloServiceNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_HelloServiceStub_sendNotification);
void HelloServiceStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_src_HelloServiceStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_src_HelloServiceStub_errorRequest);
void HelloServiceStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result { NEService::eResultType::NotProcessed };
    msg_id listenerId { msgId };

    switch ( static_cast<NEHelloService::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloService::eMessageIDs::MsgId_responseHelloService:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloService::eMessageIDs::MsgId_requestHelloService:
        listenerId = static_cast<msg_id>(NEHelloService::getResponseId(static_cast< NEHelloService::eMessageIDs>(msgId)));
        result = msgCancel ? NEService::eResultType::RequestCanceled : NEService::eResultType::RequestError;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }

    StubBase::StubListenerList listeners;
    if ( findListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(generated_src_HelloServiceStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEHelloService::getString( static_cast<NEHelloService::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEHelloService::getString(static_cast<NEHelloService::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, HelloServiceResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloServiceStub::responseHelloService( bool success )
{
    constexpr NEHelloService::eMessageIDs msgId { NEHelloService::eMessageIDs::MsgId_responseHelloService };
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << success;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloServiceStub_processRequestEvent);
void HelloServiceStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    HelloServiceRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, HelloServiceRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const HelloServiceRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEHelloService::eMessageIDs respId = NEHelloService::getResponseId(static_cast<NEHelloService::eMessageIDs>(reqId));

        switch ( static_cast<NEHelloService::eMessageIDs>(reqId) )
        {
        case NEHelloService::eMessageIDs::MsgId_requestHelloService:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String  client;
                stream >> client;
                requestHelloService( client );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_HelloServiceStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_HelloServiceStub_processAttributeEvent);
void HelloServiceStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType { eventElem.getRequestType() };
    const ProxyAddress & source { eventElem.getEventSource( ) };
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(source, removedIds);
    }
    else
    {
        NEHelloService::eMessageIDs updId  { static_cast<NEHelloService::eMessageIDs>(eventElem.getRequestId()) };
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_HelloServiceStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloService::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_src_HelloServiceStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloServiceStub.cpp file
//////////////////////////////////////////////////////////////////////////
