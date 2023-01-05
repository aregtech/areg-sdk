//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/DirectMessagerStub.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/DirectMessagerStub.hpp"
#include "generated/private/DirectMessagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectMessagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectMessagerStub::DirectMessagerStub( Component & masterComp )
    : StubBase    ( masterComp, NEDirectMessager::getInterfaceData() )

    , mChatParticipants       (  )
    , mChatParticipantsState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void DirectMessagerStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_DirectMessagerStub_startupServiceInterface);
void DirectMessagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_DirectMessagerStub_startupServiceInterface);

    DirectMessagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    DirectMessagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerStub_shutdownServiceIntrface);
void DirectMessagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_DirectMessagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    DirectMessagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    DirectMessagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * DirectMessagerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW DirectMessagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW DirectMessagerResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * DirectMessagerStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW DirectMessagerRequestEvent(stream) );
}

RemoteNotifyRequestEvent * DirectMessagerStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW DirectMessagerNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_DirectMessagerStub_sendNotification);
void DirectMessagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
    {
    case NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
        mChatParticipantsState = NEService::eDataStateType::DataIsOK;
        stream << mChatParticipants;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_DirectMessagerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_DirectMessagerStub_errorRequest);
void DirectMessagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
        mChatParticipantsState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_responseChatJoin:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent:
    case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped:
    case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined:
    case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft:
    case NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed:
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectMessager::eMessageIDs::MsgId_requestChatJoin:
    case NEDirectMessager::eMessageIDs::MsgId_requestMessageSend:
    case NEDirectMessager::eMessageIDs::MsgId_requestMessageType:
    case NEDirectMessager::eMessageIDs::MsgId_requestChatLeave:
        listenerId = static_cast<msg_id>(NEDirectMessager::getResponseId(static_cast< NEDirectMessager::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_DirectMessagerStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEDirectMessager::getString( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEDirectMessager::getString(static_cast<NEDirectMessager::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, DirectMessagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void DirectMessagerStub::setChatParticipants( const NEDirectMessager::ListParticipants & newValue )
{
    if ( (mChatParticipantsState != NEService::eDataStateType::DataIsOK) || (mChatParticipants != newValue) )
    {
        mChatParticipants = newValue;
        sendNotification( static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void DirectMessagerStub::responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_responseChatJoin;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeed;
    stream << listParticipant;
    stream << timeConnect;
    stream << timeConnected;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void DirectMessagerStub::broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectMessagerStub::broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << msgText;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectMessagerStub::broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeJoined;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectMessagerStub::broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeLeft;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void DirectMessagerStub::broadcastChatClosed( void )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectMessagerStub_processRequestEvent);
void DirectMessagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    DirectMessagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, DirectMessagerRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const DirectMessagerRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEDirectMessager::eMessageIDs respId = NEDirectMessager::getResponseId(static_cast<NEDirectMessager::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectMessager::eMessageIDs>(reqId) )
        {
        case NEDirectMessager::eMessageIDs::MsgId_requestChatJoin:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                NEDirectMessager::sParticipant  participant;
                DateTime                        timeConnect;
                stream >> participant;
                stream >> timeConnect;
                requestChatJoin( participant, timeConnect );
            }
            break;

        case NEDirectMessager::eMessageIDs::MsgId_requestMessageSend:
            if ( true )
            {
                NEDirectMessager::sParticipant  sender;
                String                          msgText;
                DateTime                        timeSent;
                stream >> sender;
                stream >> msgText;
                stream >> timeSent;
                requestMessageSend( sender, msgText, timeSent );
            }
            break;

        case NEDirectMessager::eMessageIDs::MsgId_requestMessageType:
            if ( true )
            {
                NEDirectMessager::sParticipant  participant;
                String                          msgText;
                stream >> participant;
                stream >> msgText;
                requestMessageType( participant, msgText );
            }
            break;

        case NEDirectMessager::eMessageIDs::MsgId_requestChatLeave:
            if ( true )
            {
                NEDirectMessager::sParticipant  participant;
                DateTime                        timeLeave;
                stream >> participant;
                stream >> timeLeave;
                requestChatLeave( participant, timeLeave );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_DirectMessagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_DirectMessagerStub_processAttributeEvent);
void DirectMessagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEDirectMessager::eMessageIDs updId  = static_cast<NEDirectMessager::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_DirectMessagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectMessager::getString(updId)
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
            case NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
                if ( mChatParticipantsState == NEService::eDataStateType::DataIsOK )
                    stream << mChatParticipants;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_DirectMessagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEDirectMessager::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEDirectMessager::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
