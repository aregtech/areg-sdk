//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/ConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     12.05.2021  16:41:22 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/ConnectionManagerStub.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/ConnectionManagerStub.hpp"
#include "shared/generated/private/ConnectionManagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ConnectionManagerStub::ConnectionManagerStub( Component & masterComp )
    : StubBase    ( masterComp, NEConnectionManager::createInterfaceData() )
    
    , mConnectionList         (  )
    , mConnectionListState    ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

ConnectionManagerStub::~ConnectionManagerStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void ConnectionManagerStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_startupServiceInterface);
void ConnectionManagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerStub_startupServiceInterface);
    
    ConnectionManagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    ConnectionManagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_shutdownServiceIntrface);
void ConnectionManagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    ConnectionManagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    ConnectionManagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int ConnectionManagerStub::getNumberOfRequests( void ) const
{
    return NEConnectionManager::NumberofRequests;
}

unsigned int ConnectionManagerStub::getNumberOfResponses( void ) const
{
    return NEConnectionManager::NumberofResponses;
}

unsigned int ConnectionManagerStub::getNumberOfAttributes( void ) const
{
    return NEConnectionManager::NumberofAttributes;
}

const unsigned int * ConnectionManagerStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::RequestIds);
}

const unsigned int * ConnectionManagerStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::ResponseIds);
}

const unsigned int * ConnectionManagerStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::AttributeIds);
}

ResponseEvent * ConnectionManagerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW ConnectionManagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW ConnectionManagerResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * ConnectionManagerStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW ConnectionManagerRequestEvent(stream) );
}

RemoteNotifyRequestEvent * ConnectionManagerStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW ConnectionManagerNotifyRequestEvent(stream) );
}

const Version & ConnectionManagerStub::getImplVersion( void ) const
{
    return NEConnectionManager::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_sendNotification);
void ConnectionManagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
    {
    case NEConnectionManager::MSG_ID_ConnectionList:
        mConnectionListState = NEService::DATA_OK;
        stream << mConnectionList;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_ConnectionManagerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_errorRequest);
void ConnectionManagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_ConnectionList:
        mConnectionListState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_responseConnect:
    case NEConnectionManager::MSG_ID_responseRegisterConnection:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_broadcastConnectionUpdated:
    case NEConnectionManager::MSG_ID_broadcastClientConnected:
    case NEConnectionManager::MSG_ID_broadcastClientDisconnected:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_requestConnet:
    case NEConnectionManager::MSG_ID_requestRegisterConnection:
    case NEConnectionManager::MSG_ID_requestDiconnect:
        listenerId = NEConnectionManager::getResponseId(static_cast< NEConnectionManager::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_ConnectionManagerStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEConnectionManager::getString( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEConnectionManager::getString(static_cast<NEConnectionManager::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, ConnectionManagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void ConnectionManagerStub::setConnectionList( const NEConnectionManager::MapConnection & newValue )
{
    if ( (mConnectionListState != NEService::DATA_OK) || (mConnectionList != newValue) )
    {
        mConnectionList = newValue;
        sendNotification( NEConnectionManager::MSG_ID_ConnectionList );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void ConnectionManagerStub::responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_responseConnect;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    stream << result;
    sendResponseEvent( msgId, args );
}

void ConnectionManagerStub::responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_responseRegisterConnection;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << connection;
    stream << connectionList;
    stream << success;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void ConnectionManagerStub::broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_broadcastConnectionUpdated;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << updatedList;
    sendResponseEvent( msgId, args );
}

void ConnectionManagerStub::broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_broadcastClientConnected;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientConnected;
    sendResponseEvent( msgId, args );
}

void ConnectionManagerStub::broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_broadcastClientDisconnected;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientLeft;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_processRequestEvent);
void ConnectionManagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    ConnectionManagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, ConnectionManagerRequestEvent);

    if ( (reqEvent != static_cast<ConnectionManagerRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const ConnectionManagerRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEConnectionManager::eMessageIDs respId = NEConnectionManager::getResponseId(static_cast<NEConnectionManager::eMessageIDs>(reqId));

        switch ( static_cast<NEConnectionManager::eMessageIDs>(reqId) )
        {
        case NEConnectionManager::MSG_ID_requestConnet:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                String      nickName;
                DateTime    dateTime;
                stream >> nickName;                
                stream >> dateTime;                
                requestConnet( nickName, dateTime );
            }
            break;
            
        case NEConnectionManager::MSG_ID_requestRegisterConnection:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                String          nickName;
                unsigned int    cookie;
                unsigned int    connectCookie;
                DateTime        dateRegister;
                stream >> nickName;                
                stream >> cookie;                
                stream >> connectCookie;                
                stream >> dateRegister;                
                requestRegisterConnection( nickName, cookie, connectCookie, dateRegister );
            }
            break;
            
        case NEConnectionManager::MSG_ID_requestDiconnect:
            if ( true )
            {
                String          nickName;
                unsigned int    cookie;
                DateTime        dateTime;
                stream >> nickName;                
                stream >> cookie;                
                stream >> dateTime;                
                requestDiconnect( nickName, cookie, dateTime );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_ConnectionManagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerStub_processAttributeEvent);
void ConnectionManagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEConnectionManager::eMessageIDs updId  = static_cast<NEConnectionManager::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_ConnectionManagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEConnectionManager::getString(updId)
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
            case NEConnectionManager::MSG_ID_ConnectionList:
                if ( mConnectionListState == NEService::DATA_OK )
                    stream << mConnectionList;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_ConnectionManagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEConnectionManager::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEConnectionManager::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/ConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
