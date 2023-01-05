//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/DirectConnectionStub.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/DirectConnectionStub.hpp"
#include "generated/private/DirectConnectionEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectConnectionStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectConnectionStub::DirectConnectionStub( Component & masterComp )
    : StubBase    ( masterComp, NEDirectConnection::getInterfaceData() )

    , mInitiatedConnections       (  )
    , mInitiatedConnectionsState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void DirectConnectionStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_DirectConnectionStub_startupServiceInterface);
void DirectConnectionStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_DirectConnectionStub_startupServiceInterface);

    DirectConnectionRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    DirectConnectionNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_DirectConnectionStub_shutdownServiceIntrface);
void DirectConnectionStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_DirectConnectionStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    DirectConnectionRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    DirectConnectionNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * DirectConnectionStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW DirectConnectionResponseEvent(data, proxy, result, msgId) : DEBUG_NEW DirectConnectionResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * DirectConnectionStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW DirectConnectionRequestEvent(stream) );
}

RemoteNotifyRequestEvent * DirectConnectionStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW DirectConnectionNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_DirectConnectionStub_sendNotification);
void DirectConnectionStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
    case NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
        mInitiatedConnectionsState = NEService::eDataStateType::DataIsOK;
        stream << mInitiatedConnections;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_DirectConnectionStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_DirectConnectionStub_errorRequest);
void DirectConnectionStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
        mInitiatedConnectionsState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup:
    case NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant:
    case NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup:
    case NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant:
    case NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant:
    case NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection:
        listenerId = static_cast<msg_id>(NEDirectConnection::getResponseId(static_cast< NEDirectConnection::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_DirectConnectionStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEDirectConnection::getString( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEDirectConnection::getString(static_cast<NEDirectConnection::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, DirectConnectionResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void DirectConnectionStub::setInitiatedConnections( const NEDirectConnection::MapParticipants & newValue )
{
    if ( (mInitiatedConnectionsState != NEService::eDataStateType::DataIsOK) || (mInitiatedConnections != newValue) )
    {
        mInitiatedConnections = newValue;
        sendNotification( static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void DirectConnectionStub::responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << target;
    stream << initiator;
    stream << listParticipants;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectConnectionStub::responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectConnectionStub::responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectConnectionStub_processRequestEvent);
void DirectConnectionStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    DirectConnectionRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, DirectConnectionRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const DirectConnectionRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEDirectConnection::eMessageIDs respId = NEDirectConnection::getResponseId(static_cast<NEDirectConnection::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectConnection::eMessageIDs>(reqId) )
        {
        case NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;
                stream >> listParticipants;
                requestConnectoinSetup( initiator, listParticipants );
            }
            break;

        case NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;
                stream >> listParticipants;
                requestAddParticipant( initiator, listParticipants );
            }
            break;

        case NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;
                stream >> listParticipants;
                requestRemoveParticipant( initiator, listParticipants );
            }
            break;

        case NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection:
            if ( true )
            {
                NEDirectConnection::sInitiator  initiator;
                stream >> initiator;
                requestCloseConnection( initiator );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_DirectConnectionStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_DirectConnectionStub_processAttributeEvent);
void DirectConnectionStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEDirectConnection::eMessageIDs updId  = static_cast<NEDirectConnection::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_DirectConnectionStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectConnection::getString(updId)
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
            case NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
                if ( mInitiatedConnectionsState == NEService::eDataStateType::DataIsOK )
                    stream << mInitiatedConnections;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_DirectConnectionStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEDirectConnection::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEDirectConnection::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
