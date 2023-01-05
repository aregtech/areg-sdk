//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:39 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 *
 * \file            generated/src/HelloWorldStub.hpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/HelloWorldStub.hpp"
#include "generated/src/private/HelloWorldEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWorldStub::HelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NEHelloWorld::getInterfaceData() )

{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void HelloWorldStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_startupServiceInterface);
void HelloWorldStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWorldStub_startupServiceInterface);

    HelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
void HelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    HelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * HelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW HelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW HelloWorldResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * HelloWorldStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW HelloWorldRequestEvent(stream) );
}

RemoteNotifyRequestEvent * HelloWorldStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW HelloWorldNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_sendNotification);
void HelloWorldStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_src_HelloWorldStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_src_HelloWorldStub_errorRequest);
void HelloWorldStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum:
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
    case NEHelloWorld::eMessageIDs::MsgId_requestShutdownService:
        listenerId = static_cast<msg_id>(NEHelloWorld::getResponseId(static_cast< NEHelloWorld::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_HelloWorldStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEHelloWorld::getString( static_cast<NEHelloWorld::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEHelloWorld::getString(static_cast<NEHelloWorld::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, HelloWorldResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloWorldStub::responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void HelloWorldStub::broadcastReachedMaximum( int maxNumber )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_broadcastReachedMaximum;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << maxNumber;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_processRequestEvent);
void HelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    HelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, HelloWorldRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const HelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEHelloWorld::eMessageIDs respId = NEHelloWorld::getResponseId(static_cast<NEHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NEHelloWorld::eMessageIDs>(reqId) )
        {
        case NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String  roleName;
                stream >> roleName;
                requestHelloWorld( roleName );
            }
            break;

        case NEHelloWorld::eMessageIDs::MsgId_requestShutdownService:
            if ( true )
            {
                unsigned int    clientID;
                String          roleName;
                stream >> clientID;
                stream >> roleName;
                requestShutdownService( clientID, roleName );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_HelloWorldStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
void HelloWorldStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEHelloWorld::eMessageIDs updId  = static_cast<NEHelloWorld::eMessageIDs>(eventElem.getRequestId());
        const ProxyAddress & source = eventElem.getEventSource( );
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
