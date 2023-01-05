//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/ConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/ConnectionManagerStub.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/ConnectionManagerStub.hpp"
#include "generated/private/ConnectionManagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ConnectionManagerStub::ConnectionManagerStub( Component & masterComp )
    : StubBase    ( masterComp, NEConnectionManager::getInterfaceData() )

    , mConnectionList         (  )
    , mConnectionListState    ( NEService::eDataStateType::DataIsUnavailable )
    
{
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

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_startupServiceInterface);
void ConnectionManagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_ConnectionManagerStub_startupServiceInterface);

    ConnectionManagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    ConnectionManagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_shutdownServiceIntrface);
void ConnectionManagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_ConnectionManagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    ConnectionManagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    ConnectionManagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
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

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_sendNotification);
void ConnectionManagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
    {
    case NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
        mConnectionListState = NEService::eDataStateType::DataIsOK;
        stream << mConnectionList;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_ConnectionManagerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_errorRequest);
void ConnectionManagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
        mConnectionListState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_responseConnect:
    case NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated:
    case NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected:
    case NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected:
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEConnectionManager::eMessageIDs::MsgId_requestConnect:
    case NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection:
    case NEConnectionManager::eMessageIDs::MsgId_requestDisconnect:
        listenerId = static_cast<msg_id>(NEConnectionManager::getResponseId(static_cast< NEConnectionManager::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_ConnectionManagerStub_errorRequest);
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
    if ( (mConnectionListState != NEService::eDataStateType::DataIsOK) || (mConnectionList != newValue) )
    {
        mConnectionList = newValue;
        sendNotification( static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void ConnectionManagerStub::responseConnect( const String & nickName, unsigned int cookie, const DateTime & dateTime, NEConnectionManager::eConnectionResult result )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_responseConnect;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    stream << result;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void ConnectionManagerStub::responseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, bool success )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << connection;
    stream << connectionList;
    stream << success;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void ConnectionManagerStub::broadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << updatedList;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void ConnectionManagerStub::broadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientConnected;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void ConnectionManagerStub::broadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientLeft;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_processRequestEvent);
void ConnectionManagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    ConnectionManagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, ConnectionManagerRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const ConnectionManagerRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEConnectionManager::eMessageIDs respId = NEConnectionManager::getResponseId(static_cast<NEConnectionManager::eMessageIDs>(reqId));

        switch ( static_cast<NEConnectionManager::eMessageIDs>(reqId) )
        {
        case NEConnectionManager::eMessageIDs::MsgId_requestConnect:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String      nickName;
                DateTime    dateTime;
                stream >> nickName;
                stream >> dateTime;
                requestConnect( nickName, dateTime );
            }
            break;

        case NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
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

        case NEConnectionManager::eMessageIDs::MsgId_requestDisconnect:
            if ( true )
            {
                String          nickName;
                unsigned int    cookie;
                DateTime        dateTime;
                stream >> nickName;
                stream >> cookie;
                stream >> dateTime;
                requestDisconnect( nickName, cookie, dateTime );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_ConnectionManagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_ConnectionManagerStub_processAttributeEvent);
void ConnectionManagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEConnectionManager::eMessageIDs updId  = static_cast<NEConnectionManager::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_ConnectionManagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEConnectionManager::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataExternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
                if ( mConnectionListState == NEService::eDataStateType::DataIsOK )
                    stream << mConnectionList;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_ConnectionManagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEConnectionManager::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEConnectionManager::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/ConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
