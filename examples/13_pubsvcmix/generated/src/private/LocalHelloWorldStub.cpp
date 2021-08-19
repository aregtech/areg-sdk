//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     11.08.2021  13:17:31 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/LocalHelloWorldStub.cpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Stub class implementation.
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
    , mConnectedClientsState  ( NEService::DATA_UNAVAILABLE )
    
    , mRemainOutput           (  )
    , mRemainOutputState      ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

LocalHelloWorldStub::~LocalHelloWorldStub( void )
{
    ; // do nothing
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
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
    {
    case NELocalHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_OK;
        stream << mConnectedClients;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NELocalHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_OK;
        stream << mRemainOutput;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
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
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NELocalHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NELocalHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NELocalHelloWorld::MSG_ID_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NELocalHelloWorld::MSG_ID_requestHelloWorld:
        listenerId = NELocalHelloWorld::getResponseId(static_cast< NELocalHelloWorld::eMessageIDs>(msgId));
        result = msgCancel ? NEService::RESULT_REQUEST_CANCELED : NEService::RESULT_REQUEST_ERROR;
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
    if ( (mConnectedClientsState != NEService::DATA_OK) || (mConnectedClients != newValue) )
    {
        mConnectedClients = newValue;
        sendNotification( NELocalHelloWorld::MSG_ID_ConnectedClients );
    }
}

void LocalHelloWorldStub::setRemainOutput( const short & newValue )
{
    if ( (mRemainOutputState != NEService::DATA_OK) || (mRemainOutput != newValue) )
    {
        mRemainOutput = newValue;
        sendNotification( NELocalHelloWorld::MSG_ID_RemainOutput );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void LocalHelloWorldStub::responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )
{
    static const NELocalHelloWorld::eMessageIDs msgId = NELocalHelloWorld::MSG_ID_responseHelloWorld;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( msgId, args );
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

    if ( (reqEvent != static_cast<LocalHelloWorldRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const LocalHelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NELocalHelloWorld::eMessageIDs respId = NELocalHelloWorld::getResponseId(static_cast<NELocalHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NELocalHelloWorld::eMessageIDs>(reqId) )
        {
        case NELocalHelloWorld::MSG_ID_requestHelloWorld:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
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
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NELocalHelloWorld::eMessageIDs updId  = static_cast<NELocalHelloWorld::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NELocalHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataInternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NELocalHelloWorld::MSG_ID_ConnectedClients:
                if ( mConnectedClientsState == NEService::DATA_OK )
                    stream << mConnectedClients;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NELocalHelloWorld::MSG_ID_RemainOutput:
                if ( mRemainOutputState == NEService::DATA_OK )
                    stream << mRemainOutput;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NELocalHelloWorld::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NELocalHelloWorld::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
