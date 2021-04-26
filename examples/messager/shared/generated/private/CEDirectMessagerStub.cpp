//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     25.04.2021  23:50:46 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectMessagerStub.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/CEDirectMessagerStub.hpp"
#include "shared/generated/private/CEDirectMessagerEvents.hpp"

#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEDirectMessagerStub::CEDirectMessagerStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NEDirectMessager::CreateInterfaceData() )
    
    , mChatParticipants       (  )
    , mChatParticipantsState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

CEDirectMessagerStub::~CEDirectMessagerStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CEDirectMessagerStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CEDirectMessagerStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CEDirectMessagerStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_StartupServiceInterface);
void CEDirectMessagerStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerStub_StartupServiceInterface);
    
    CEDirectMessagerRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEDirectMessagerNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ShutdownServiceIntrface);
void CEDirectMessagerStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CEDirectMessagerRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEDirectMessagerNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

unsigned int CEDirectMessagerStub::GetNumberOfRequests( void ) const
{
    return NEDirectMessager::NumberofRequests;
}

unsigned int CEDirectMessagerStub::GetNumberOfResponses( void ) const
{
    return NEDirectMessager::NumberofResponses;
}

unsigned int CEDirectMessagerStub::GetNumberOfAttributes( void ) const
{
    return NEDirectMessager::NumberofAttributes;
}

const unsigned int * CEDirectMessagerStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::RequestIds);
}

const unsigned int * CEDirectMessagerStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::ResponseIds);
}

const unsigned int * CEDirectMessagerStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectMessager::AttributeIds);
}

CEResponseEvent * CEDirectMessagerStub::CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CEDirectMessagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CEDirectMessagerResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CEDirectMessagerStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteRequestEvent *>( DEBUG_NEW CEDirectMessagerRequestEvent(stream) );
}

CERemoteNotifyRequestEvent * CEDirectMessagerStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteNotifyRequestEvent *>( DEBUG_NEW CEDirectMessagerNotifyRequestEvent(stream) );
}

const CEVersion & CEDirectMessagerStub::GetImplVersion( void ) const
{
    return NEDirectMessager::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_SendNotification);
void CEDirectMessagerStub::SendNotification( unsigned int msgId )
{
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
    {
    case NEDirectMessager::MSG_ID_ChatParticipants:
        mChatParticipantsState = NEService::DATA_OK;
        stream << mChatParticipants;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_CEDirectMessagerStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ErrorRequest);
void CEDirectMessagerStub::ErrorRequest( unsigned int msgId, bool msgCancel )
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
    case NEDirectMessager::MSG_ID_ResponseChatJoin:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_BroadcastMessageSent:
    case NEDirectMessager::MSG_ID_BroadcastMessageTyped:
    case NEDirectMessager::MSG_ID_BroadcastParticipantJoined:
    case NEDirectMessager::MSG_ID_BroadcastParticipantLeft:
    case NEDirectMessager::MSG_ID_BroadcastChatClosed:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectMessager::MSG_ID_RequestChatJoin:
    case NEDirectMessager::MSG_ID_RequestMessageSend:
    case NEDirectMessager::MSG_ID_RequestMessageType:
    case NEDirectMessager::MSG_ID_RequestChatLeave:
        listenerId = NEDirectMessager::GetResponseId(static_cast< NEDirectMessager::eMessageIDs>(msgId));
        result = msgCancel ? NEService::RESULT_REQUEST_CANCELED : NEService::RESULT_REQUEST_ERROR;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }
    
    CEStubBase::CEStubListenerList listeners;
    if ( FindListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEDirectMessager::GetString( static_cast<NEDirectMessager::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NEDirectMessager::GetString(static_cast<NEDirectMessager::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CEDirectMessagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void CEDirectMessagerStub::SetChatParticipants( const NEDirectMessager::ListParticipants & newValue )
{
    if ( (mChatParticipantsState != NEService::DATA_OK) || (mChatParticipants != newValue) )
    {
        mChatParticipants = newValue;
        SendNotification( NEDirectMessager::MSG_ID_ChatParticipants );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void CEDirectMessagerStub::ResponseChatJoin( const bool & succeed, const NEDirectMessager::ListParticipants & listParticipant, const CEDateTime & timeConnect, const CEDateTime & timeConnected )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_ResponseChatJoin;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << succeed;
    stream << listParticipant;
    stream << timeConnect;
    stream << timeConnected;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void CEDirectMessagerStub::BroadcastMessageSent( const NEDirectMessager::sParticipant & sender, const CEString & msgText, const CEDateTime & timeSent )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_BroadcastMessageSent;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << sender;
    stream << msgText;
    stream << timeSent;
    SendResponseEvent( msgId, args );
}

void CEDirectMessagerStub::BroadcastMessageTyped( const NEDirectMessager::sParticipant & participant, const CEString & msgText )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_BroadcastMessageTyped;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << msgText;
    SendResponseEvent( msgId, args );
}

void CEDirectMessagerStub::BroadcastParticipantJoined( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeJoined )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_BroadcastParticipantJoined;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << timeJoined;
    SendResponseEvent( msgId, args );
}

void CEDirectMessagerStub::BroadcastParticipantLeft( const NEDirectMessager::sParticipant & participant, const CEDateTime & timeLeft )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_BroadcastParticipantLeft;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << participant;
    stream << timeLeft;
    SendResponseEvent( msgId, args );
}

void CEDirectMessagerStub::BroadcastChatClosed( void )
{
    static const NEDirectMessager::eMessageIDs msgId = NEDirectMessager::MSG_ID_BroadcastChatClosed;
    SendResponseEvent( msgId, CEEventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ProcessRequestEvent);
void CEDirectMessagerStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CEDirectMessagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CEDirectMessagerRequestEvent);

    if ( (reqEvent != static_cast<CEDirectMessagerRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CEDirectMessagerRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );
        NEDirectMessager::eMessageIDs respId = NEDirectMessager::GetResponseId(static_cast<NEDirectMessager::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectMessager::eMessageIDs>(reqId) )
        {
        case NEDirectMessager::MSG_ID_RequestChatJoin:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                NEDirectMessager::sParticipant  participant;
                CEDateTime                      timeConnect;
                stream >> participant;                
                stream >> timeConnect;                
                RequestChatJoin( participant, timeConnect );
            }
            break;
            
        case NEDirectMessager::MSG_ID_RequestMessageSend:
            if ( true )
            {
                NEDirectMessager::sParticipant  sender;
                CEString                        msgText;
                CEDateTime                      timeSent;
                stream >> sender;                
                stream >> msgText;                
                stream >> timeSent;                
                RequestMessageSend( sender, msgText, timeSent );
            }
            break;
            
        case NEDirectMessager::MSG_ID_RequestMessageType:
            if ( true )
            {
                NEDirectMessager::sParticipant  participant;
                CEString                        msgText;
                stream >> participant;                
                stream >> msgText;                
                RequestMessageType( participant, msgText );
            }
            break;
            
        case NEDirectMessager::MSG_ID_RequestChatLeave:
            if ( true )
            {
                NEDirectMessager::sParticipant  participant;
                CEDateTime                      timeLeave;
                stream >> participant;                
                stream >> timeLeave;                
                RequestChatLeave( participant, timeLeave );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ProcessAttributeEvent);
void CEDirectMessagerStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NEDirectMessager::eMessageIDs updId  = static_cast<NEDirectMessager::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectMessager::GetString(updId)
                            , CEProxyAddress::ConvertAddressToPath(eventElem.GetEventSource()).GetBuffer());
            }
#else   // _DEBUG
            AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
#endif  // _DEBUG
            CEEventDataStream args(CEEventDataStream::EventDataExternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.GetStreamForWrite();

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
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_CEDirectMessagerStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEDirectMessager::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEDirectMessager::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
