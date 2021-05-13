//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     12.05.2021  16:41:24 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectMessagerStub.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/DirectMessagerStub.hpp"
#include "shared/generated/private/DirectMessagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectMessagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectMessagerStub::DirectMessagerStub( Component & masterComp )
    : StubBase    ( masterComp, NEDirectMessager::createInterfaceData() )
    
    , mChatParticipants       (  )
    , mChatParticipantsState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

DirectMessagerStub::~DirectMessagerStub( void )
{
    ; // do nothing
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

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_startupServiceInterface);
void DirectMessagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(shared_generated_DirectMessagerStub_startupServiceInterface);
    
    DirectMessagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    DirectMessagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_shutdownServiceIntrface);
void DirectMessagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(shared_generated_DirectMessagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    DirectMessagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    DirectMessagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int DirectMessagerStub::getNumberOfRequests( void ) const
{
    return NEDirectMessager::NumberofRequests;
}

unsigned int DirectMessagerStub::getNumberOfResponses( void ) const
{
    return NEDirectMessager::NumberofResponses;
}

unsigned int DirectMessagerStub::getNumberOfAttributes( void ) const
{
    return NEDirectMessager::NumberofAttributes;
}

const unsigned int * DirectMessagerStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::RequestIds);
}

const unsigned int * DirectMessagerStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::ResponseIds);
}

const unsigned int * DirectMessagerStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::AttributeIds);
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

const Version & DirectMessagerStub::getImplVersion( void ) const
{
    return NEDirectMessager::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_sendNotification);
void DirectMessagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
    {
    case NEDirectMessager::MSG_ID_ChatParticipants:
        mChatParticipantsState = NEService::DATA_OK;
        stream << mChatParticipants;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_DirectMessagerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_errorRequest);
void DirectMessagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_ChatParticipants:
        mChatParticipantsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_responseChatJoin:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_broadcastMessageSent:
    case NEDirectMessager::MSG_ID_broadcastMessageTyped:
    case NEDirectMessager::MSG_ID_broadcastParticipantJoined:
    case NEDirectMessager::MSG_ID_broadcastParticipantLeft:
    case NEDirectMessager::MSG_ID_broadcastChatClosed:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_requestChatJoin:
    case NEDirectMessager::MSG_ID_requestMessageSend:
    case NEDirectMessager::MSG_ID_requestMessageType:
    case NEDirectMessager::MSG_ID_requestChatLeave:
        listenerId = NEDirectMessager::getResponseId(static_cast< NEDirectMessager::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_DirectMessagerStub_errorRequest);
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
    if ( (mChatParticipantsState != NEService::DATA_OK) || (mChatParticipants != newValue) )
    {
        mChatParticipants = newValue;
        sendNotification( NEDirectMessager::MSG_ID_ChatParticipants );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void DirectMessagerStub::responseChatJoin( bool succeed, const NEDirectMessager::ListParticipants & listParticipant, const DateTime & timeConnect, const DateTime & timeConnected )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_responseChatJoin;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeed;
    stream << listParticipant;
    stream << timeConnect;
    stream << timeConnected;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void DirectMessagerStub::broadcastMessageSent( const NEDirectMessager::sParticipant & sender, const String & msgText, const DateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_broadcastMessageSent;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    sendResponseEvent( msgId, args );
}

void DirectMessagerStub::broadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const String & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_broadcastMessageTyped;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << msgText;
    sendResponseEvent( msgId, args );
}

void DirectMessagerStub::broadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const DateTime & timeJoined )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_broadcastParticipantJoined;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeJoined;
    sendResponseEvent( msgId, args );
}

void DirectMessagerStub::broadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const DateTime & timeLeft )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_broadcastParticipantLeft;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << participant;
    stream << timeLeft;
    sendResponseEvent( msgId, args );
}

void DirectMessagerStub::broadcastChatClosed( void )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_broadcastChatClosed;
    sendResponseEvent( msgId, EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_processRequestEvent);
void DirectMessagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    DirectMessagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, DirectMessagerRequestEvent);

    if ( (reqEvent != static_cast<DirectMessagerRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const DirectMessagerRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEDirectMessager::eMessageIDs respId = NEDirectMessager::getResponseId(static_cast<NEDirectMessager::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectMessager::eMessageIDs>(reqId) )
        {
        case NEDirectMessager::MSG_ID_requestChatJoin:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                NEDirectMessager::sParticipant  participant;
                DateTime                        timeConnect;
                stream >> participant;                
                stream >> timeConnect;                
                requestChatJoin( participant, timeConnect );
            }
            break;
            
        case NEDirectMessager::MSG_ID_requestMessageSend:
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
            
        case NEDirectMessager::MSG_ID_requestMessageType:
            if ( true )
            {
                NEDirectMessager::sParticipant  participant;
                String                          msgText;
                stream >> participant;                
                stream >> msgText;                
                requestMessageType( participant, msgText );
            }
            break;
            
        case NEDirectMessager::MSG_ID_requestChatLeave:
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
                TRACE_SCOPE(shared_generated_DirectMessagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerStub_processAttributeEvent);
void DirectMessagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEDirectMessager::eMessageIDs updId  = static_cast<NEDirectMessager::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_DirectMessagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectMessager::getString(updId)
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
            case NEDirectMessager::MSG_ID_ChatParticipants:
                if ( mChatParticipantsState == NEService::DATA_OK )
                    stream << mChatParticipants;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_DirectMessagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEDirectMessager::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEDirectMessager::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/DirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
