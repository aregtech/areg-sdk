//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteHelloWorld.
 * Generated at     29.05.2021  12:43:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteHelloWorldStub.cpp
 * \ingroup         RemoteHelloWorld Service Interface
 * \brief           This is an automatic generated code of RemoteHelloWorld Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shareipcmix/src/RemoteHelloWorldStub.hpp"
#include "shareipcmix/src/private/RemoteHelloWorldEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteHelloWorldStub::RemoteHelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NERemoteHelloWorld::getInterfaceData() )
    
    , mConnectedClients       (  )
    , mConnectedClientsState  ( NEService::DATA_UNAVAILABLE )
    
    , mRemainOutput           (  )
    , mRemainOutputState      ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

RemoteHelloWorldStub::~RemoteHelloWorldStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void RemoteHelloWorldStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_startupServiceInterface);
void RemoteHelloWorldStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_startupServiceInterface);
    
    RemoteHelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    RemoteHelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_shutdownServiceIntrface);
void RemoteHelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    RemoteHelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    RemoteHelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * RemoteHelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW RemoteHelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW RemoteHelloWorldResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * RemoteHelloWorldStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW RemoteHelloWorldRequestEvent(stream) );
}

RemoteNotifyRequestEvent * RemoteHelloWorldStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW RemoteHelloWorldNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_sendNotification);
void RemoteHelloWorldStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NERemoteHelloWorld::eMessageIDs>(msgId) )
    {
    case NERemoteHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_OK;
        stream << mConnectedClients;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NERemoteHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_OK;
        stream << mRemainOutput;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_errorRequest);
void RemoteHelloWorldStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NERemoteHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NERemoteHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_broadcastHelloClients:
    case NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NERemoteHelloWorld::MSG_ID_requestHelloWorld:
    case NERemoteHelloWorld::MSG_ID_requestClientShutdown:
        listenerId = NERemoteHelloWorld::getResponseId(static_cast< NERemoteHelloWorld::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NERemoteHelloWorld::getString( static_cast<NERemoteHelloWorld::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NERemoteHelloWorld::getString(static_cast<NERemoteHelloWorld::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, RemoteHelloWorldResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void RemoteHelloWorldStub::setConnectedClients( const NERemoteHelloWorld::ConnectionList & newValue )
{
    if ( (mConnectedClientsState != NEService::DATA_OK) || (mConnectedClients != newValue) )
    {
        mConnectedClients = newValue;
        sendNotification( NERemoteHelloWorld::MSG_ID_ConnectedClients );
    }
}

void RemoteHelloWorldStub::setRemainOutput( const short & newValue )
{
    if ( (mRemainOutputState != NEService::DATA_OK) || (mRemainOutput != newValue) )
    {
        mRemainOutput = newValue;
        sendNotification( NERemoteHelloWorld::MSG_ID_RemainOutput );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void RemoteHelloWorldStub::responseHelloWorld( const NERemoteHelloWorld::sConnectedClient & clientInfo )
{
    static const NERemoteHelloWorld::eMessageIDs msgId = NERemoteHelloWorld::MSG_ID_responseHelloWorld;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void RemoteHelloWorldStub::broadcastHelloClients( const NERemoteHelloWorld::ConnectionList & clientList )
{
    static const NERemoteHelloWorld::eMessageIDs msgId = NERemoteHelloWorld::MSG_ID_broadcastHelloClients;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientList;
    sendResponseEvent( msgId, args );
}

void RemoteHelloWorldStub::broadcastServiceUnavailable( void )
{
    static const NERemoteHelloWorld::eMessageIDs msgId = NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable;
    sendResponseEvent( msgId, EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_processRequestEvent);
void RemoteHelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    RemoteHelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, RemoteHelloWorldRequestEvent);

    if ( (reqEvent != static_cast<RemoteHelloWorldRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const RemoteHelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NERemoteHelloWorld::eMessageIDs respId = NERemoteHelloWorld::getResponseId(static_cast<NERemoteHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NERemoteHelloWorld::eMessageIDs>(reqId) )
        {
        case NERemoteHelloWorld::MSG_ID_requestHelloWorld:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                String  roleName;
                String  addMessage  = "";
                stream >> roleName;                
                stream >> addMessage;                
                requestHelloWorld( roleName, addMessage );
            }
            break;
            
        case NERemoteHelloWorld::MSG_ID_requestClientShutdown:
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
                TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_processAttributeEvent);
void RemoteHelloWorldStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NERemoteHelloWorld::eMessageIDs updId  = static_cast<NERemoteHelloWorld::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NERemoteHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataExternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NERemoteHelloWorld::MSG_ID_ConnectedClients:
                if ( mConnectedClientsState == NEService::DATA_OK )
                    stream << mConnectedClients;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NERemoteHelloWorld::MSG_ID_RemainOutput:
                if ( mRemainOutputState == NEService::DATA_OK )
                    stream << mRemainOutput;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NERemoteHelloWorld::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NERemoteHelloWorld::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
