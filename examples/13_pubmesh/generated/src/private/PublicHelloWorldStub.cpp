//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PublicHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:54 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/PublicHelloWorldStub.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PublicHelloWorldStub.hpp"
#include "generated/src/private/PublicHelloWorldEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PublicHelloWorldStub::PublicHelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NEPublicHelloWorld::getInterfaceData() )

{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void PublicHelloWorldStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_startupServiceInterface);
void PublicHelloWorldStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldStub_startupServiceInterface);

    PublicHelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PublicHelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_shutdownServiceIntrface);
void PublicHelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    PublicHelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PublicHelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PublicHelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PublicHelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PublicHelloWorldResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * PublicHelloWorldStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW PublicHelloWorldRequestEvent(stream) );
}

RemoteNotifyRequestEvent * PublicHelloWorldStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW PublicHelloWorldNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_sendNotification);
void PublicHelloWorldStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_errorRequest);
void PublicHelloWorldStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEPublicHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
    case NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister:
    case NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister:
    case NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister:
    case NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        listenerId = static_cast<msg_id>(NEPublicHelloWorld::getResponseId(static_cast< NEPublicHelloWorld::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_PublicHelloWorldStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEPublicHelloWorld::getString( static_cast<NEPublicHelloWorld::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEPublicHelloWorld::getString(static_cast<NEPublicHelloWorld::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, PublicHelloWorldResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

void PublicHelloWorldStub::responseRegister( const NEPublicHelloWorld::sClientRegister & client )
{
    static const NEPublicHelloWorld::eMessageIDs msgId = NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void PublicHelloWorldStub::responseHelloWorld( unsigned int clientID )
{
    static const NEPublicHelloWorld::eMessageIDs msgId = NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_processRequestEvent);
void PublicHelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    PublicHelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, PublicHelloWorldRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const PublicHelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEPublicHelloWorld::eMessageIDs respId = NEPublicHelloWorld::getResponseId(static_cast<NEPublicHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NEPublicHelloWorld::eMessageIDs>(reqId) )
        {
        case NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String          name;
                ServiceAddress  service;
                String          thread;
                String          process;
                stream >> name;
                stream >> service;
                stream >> thread;
                stream >> process;
                requestRegister( name, service, thread, process );
            }
            break;

        case NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister:
            if ( true )
            {
                NEPublicHelloWorld::sClientRegister client;
                stream >> client;
                requestUnregister( client );
            }
            break;

        case NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                unsigned int    clientID;
                stream >> clientID;
                requestHelloWorld( clientID );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_PublicHelloWorldStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldStub_processAttributeEvent);
void PublicHelloWorldStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEPublicHelloWorld::eMessageIDs updId  = static_cast<NEPublicHelloWorld::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_src_PublicHelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPublicHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_src_PublicHelloWorldStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PublicHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
