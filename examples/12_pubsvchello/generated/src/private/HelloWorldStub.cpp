//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     11.08.2021  13:16:40 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/HelloWorldStub.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Stub class implementation.
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
    , mConnectedClientsState  ( NEService::DATA_UNAVAILABLE )
    
    , mRemainOutput           (  )
    , mRemainOutputState      ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

HelloWorldStub::~HelloWorldStub( void )
{
    ; // do nothing
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
    
    HelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    HelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
void HelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    HelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    HelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
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
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEHelloWorld::eMessageIDs>(msgId) )
    {
    case NEHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_OK;
        stream << mConnectedClients;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NEHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_OK;
        stream << mRemainOutput;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
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
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEHelloWorld::MSG_ID_ConnectedClients:
        mConnectedClientsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NEHelloWorld::MSG_ID_RemainOutput:
        mRemainOutputState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloWorld::MSG_ID_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEHelloWorld::MSG_ID_broadcastHelloClients:
    case NEHelloWorld::MSG_ID_broadcastServiceUnavailable:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloWorld::MSG_ID_requestHelloWorld:
    case NEHelloWorld::MSG_ID_requestClientShutdown:
        listenerId = NEHelloWorld::getResponseId(static_cast< NEHelloWorld::eMessageIDs>(msgId));
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
    if ( (mConnectedClientsState != NEService::DATA_OK) || (mConnectedClients != newValue) )
    {
        mConnectedClients = newValue;
        sendNotification( NEHelloWorld::MSG_ID_ConnectedClients );
    }
}

void HelloWorldStub::setRemainOutput( const short & newValue )
{
    if ( (mRemainOutputState != NEService::DATA_OK) || (mRemainOutput != newValue) )
    {
        mRemainOutput = newValue;
        sendNotification( NEHelloWorld::MSG_ID_RemainOutput );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloWorldStub::responseHelloWorld( const NEHelloWorld::sConnectedClient & clientInfo )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::MSG_ID_responseHelloWorld;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void HelloWorldStub::broadcastHelloClients( const NEHelloWorld::ConnectionList & clientList )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::MSG_ID_broadcastHelloClients;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientList;
    sendResponseEvent( msgId, args );
}

void HelloWorldStub::broadcastServiceUnavailable( void )
{
    static const NEHelloWorld::eMessageIDs msgId = NEHelloWorld::MSG_ID_broadcastServiceUnavailable;
    sendResponseEvent( msgId, EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWorldStub_processRequestEvent);
void HelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    HelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, HelloWorldRequestEvent);

    if ( (reqEvent != static_cast<HelloWorldRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const HelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEHelloWorld::eMessageIDs respId = NEHelloWorld::getResponseId(static_cast<NEHelloWorld::eMessageIDs>(reqId));

        switch ( static_cast<NEHelloWorld::eMessageIDs>(reqId) )
        {
        case NEHelloWorld::MSG_ID_requestHelloWorld:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                String  roleName;
                String  addMessage  = "";
                stream >> roleName;                
                stream >> addMessage;                
                requestHelloWorld( roleName, addMessage );
            }
            break;
            
        case NEHelloWorld::MSG_ID_requestClientShutdown:
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
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEHelloWorld::eMessageIDs updId  = static_cast<NEHelloWorld::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloWorld::getString(updId)
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
            case NEHelloWorld::MSG_ID_ConnectedClients:
                if ( mConnectedClientsState == NEService::DATA_OK )
                    stream << mConnectedClients;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NEHelloWorld::MSG_ID_RemainOutput:
                if ( mRemainOutputState == NEService::DATA_OK )
                    stream << mRemainOutput;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_HelloWorldStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEHelloWorld::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEHelloWorld::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
