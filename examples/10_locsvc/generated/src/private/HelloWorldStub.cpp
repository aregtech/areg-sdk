//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:11 GMT+02:00 
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
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWorldStub::HelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NEHelloWorld::getInterfaceData() )
    
    , mConnectedClients       (  )
    , mConnectedClientsState  ( NEService::eDataStateType::DataIsUnavailable )
    
    , mRemainOutput           (  )
    , mRemainOutputState      ( NEService::eDataStateType::DataIsUnavailable )
    
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
    
    HelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    HelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
void HelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    HelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    HelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * HelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW HelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW HelloWorldResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_sendNotification);
void HelloWorldStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEHelloWorld::eMessageIDs>(msgId) )
    {
    case NEHelloWorld::eMessageIDs::MsgId_ConnectedClients:
        mConnectedClientsState = NEService::eDataStateType::DataIsOK;
        stream << mConnectedClients;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NEHelloWorld::eMessageIDs::MsgId_RemainOutput:
        mRemainOutputState = NEService::eDataStateType::DataIsOK;
        stream << mRemainOutput;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_HelloWorldStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
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
    case NEHelloWorld::eMessageIDs::MsgId_ConnectedClients:
        mConnectedClientsState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NEHelloWorld::eMessageIDs::MsgId_RemainOutput:
        mRemainOutputState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_broadcastHelloClients:
    case NEHelloWorld::eMessageIDs::MsgId_broadcastServiceUnavailable:
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
    case NEHelloWorld::eMessageIDs::MsgId_requestClientShutdown:
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

void HelloWorldStub::setConnectedClients( const NEHelloWorld::ConnectionList & newValue )
{
    if ( (mConnectedClientsState != NEService::eDataStateType::DataIsOK) || (mConnectedClients != newValue) )
    {
        mConnectedClients = newValue;
        sendNotification( static_cast<msg_id>(NEHelloWorld::eMessageIDs::MsgId_ConnectedClients) );
    }
}

void HelloWorldStub::setRemainOutput( const short & newValue )
{
    if ( (mRemainOutputState != NEService::eDataStateType::DataIsOK) || (mRemainOutput != newValue) )
    {
        mRemainOutput = newValue;
        sendNotification( static_cast<msg_id>(NEHelloWorld::eMessageIDs::MsgId_RemainOutput) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloWorldStub::responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void HelloWorldStub::broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_broadcastHelloClients;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientList;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void HelloWorldStub::broadcastServiceUnavailable( void )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::eMessageIDs::MsgId_broadcastServiceUnavailable;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
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
                String  addMessage  = "";
                stream >> roleName;                
                stream >> addMessage;                
                requestHelloWorld( roleName, addMessage );
            }
            break;
            
        case NEHelloWorld::eMessageIDs::MsgId_requestClientShutdown:
            if ( true )
            {
                unsigned int    clientID;
                String          roleName;
                stream >> clientID;                
                stream >> roleName;                
                requestClientShutdown( clientID, roleName );
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
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataInternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEHelloWorld::eMessageIDs::MsgId_ConnectedClients:
                if ( mConnectedClientsState == NEService::eDataStateType::DataIsOK )
                    stream << mConnectedClients;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NEHelloWorld::eMessageIDs::MsgId_RemainOutput:
                if ( mRemainOutputState == NEService::eDataStateType::DataIsOK )
                    stream << mRemainOutput;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEHelloWorld::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEHelloWorld::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
