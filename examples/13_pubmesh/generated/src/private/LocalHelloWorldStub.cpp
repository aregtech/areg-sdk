//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:12 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/LocalHelloWorldStub.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/LocalHelloWorldStub.hpp"
#include "generated/src/private/LocalHelloWorldEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LocalHelloWorldStub::LocalHelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NELocalHelloWorld::getInterfaceData() )
    
    , mConnectedClients       (  )
    , mConnectedClientsState  ( NEService::eDataStateType::DataIsUnavailable )
    
    , mRemainOutput           (  )
    , mRemainOutputState      ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void LocalHelloWorldStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_startupServiceInterface);
void LocalHelloWorldStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldStub_startupServiceInterface);
    
    LocalHelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    LocalHelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_shutdownServiceIntrface);
void LocalHelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    LocalHelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    LocalHelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * LocalHelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW LocalHelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW LocalHelloWorldResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_sendNotification);
void LocalHelloWorldStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
    {
    case NELocalHelloWorld::eMessageIDs::MsgId_ConnectedClients:
        mConnectedClientsState = NEService::eDataStateType::DataIsOK;
        stream << mConnectedClients;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NELocalHelloWorld::eMessageIDs::MsgId_RemainOutput:
        mRemainOutputState = NEService::eDataStateType::DataIsOK;
        stream << mRemainOutput;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_LocalHelloWorldStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_errorRequest);
void LocalHelloWorldStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;
    
    switch ( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_ConnectedClients:
        mConnectedClientsState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NELocalHelloWorld::eMessageIDs::MsgId_RemainOutput:
        mRemainOutputState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        listenerId = static_cast<msg_id>(NELocalHelloWorld::getResponseId(static_cast< NELocalHelloWorld::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_LocalHelloWorldStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NELocalHelloWorld::getString( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NELocalHelloWorld::getString(static_cast<NELocalHelloWorld::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, LocalHelloWorldResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void LocalHelloWorldStub::setConnectedClients( const NELocalHelloWorld::ConnectionList & newValue )
{
    if ( (mConnectedClientsState != NEService::eDataStateType::DataIsOK) || (mConnectedClients != newValue) )
    {
        mConnectedClients = newValue;
        sendNotification( static_cast<msg_id>(NELocalHelloWorld::eMessageIDs::MsgId_ConnectedClients) );
    }
}

void LocalHelloWorldStub::setRemainOutput( const short & newValue )
{
    if ( (mRemainOutputState != NEService::eDataStateType::DataIsOK) || (mRemainOutput != newValue) )
    {
        mRemainOutput = newValue;
        sendNotification( static_cast<msg_id>(NELocalHelloWorld::eMessageIDs::MsgId_RemainOutput) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void LocalHelloWorldStub::responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )
{
    static const NELocalHelloWorld::eMessageIDs msgId = NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_processRequestEvent);
void LocalHelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    LocalHelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, LocalHelloWorldRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const LocalHelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NELocalHelloWorld::eMessageIDs respId = NELocalHelloWorld::getResponseId(static_cast<NELocalHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NELocalHelloWorld::eMessageIDs>(reqId) )
        {
        case NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String  roleName;
                String  addMessage  = "";
                stream >> roleName;                
                stream >> addMessage;                
                requestHelloWorld( roleName, addMessage );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
void LocalHelloWorldStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NELocalHelloWorld::eMessageIDs updId  = static_cast<NELocalHelloWorld::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NELocalHelloWorld::getString(updId)
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
            case NELocalHelloWorld::eMessageIDs::MsgId_ConnectedClients:
                if ( mConnectedClientsState == NEService::eDataStateType::DataIsOK )
                    stream << mConnectedClients;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NELocalHelloWorld::eMessageIDs::MsgId_RemainOutput:
                if ( mRemainOutputState == NEService::eDataStateType::DataIsOK )
                    stream << mRemainOutput;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
