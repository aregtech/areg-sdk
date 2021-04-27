//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectConnectionStub.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/CEDirectConnectionStub.hpp"
#include "shared/generated/private/CEDirectConnectionEvents.hpp"

#include "areg/component/CEServiceResponseEvent.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEDirectConnectionStub::CEDirectConnectionStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NEDirectConnection::CreateInterfaceData() )
    
    , mInitiatedConnections       (  )
    , mInitiatedConnectionsState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

CEDirectConnectionStub::~CEDirectConnectionStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CEDirectConnectionStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CEDirectConnectionStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CEDirectConnectionStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_StartupServiceInterface);
void CEDirectConnectionStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionStub_StartupServiceInterface);
    
    CEDirectConnectionRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEDirectConnectionNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ShutdownServiceIntrface);
void CEDirectConnectionStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CEDirectConnectionRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEDirectConnectionNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

unsigned int CEDirectConnectionStub::GetNumberOfRequests( void ) const
{
    return NEDirectConnection::NumberofRequests;
}

unsigned int CEDirectConnectionStub::GetNumberOfResponses( void ) const
{
    return NEDirectConnection::NumberofResponses;
}

unsigned int CEDirectConnectionStub::GetNumberOfAttributes( void ) const
{
    return NEDirectConnection::NumberofAttributes;
}

const unsigned int * CEDirectConnectionStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::RequestIds);
}

const unsigned int * CEDirectConnectionStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::ResponseIds);
}

const unsigned int * CEDirectConnectionStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::AttributeIds);
}

CEResponseEvent * CEDirectConnectionStub::CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CEDirectConnectionResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CEDirectConnectionResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CEDirectConnectionStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteRequestEvent *>( DEBUG_NEW CEDirectConnectionRequestEvent(stream) );
}

CERemoteNotifyRequestEvent * CEDirectConnectionStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteNotifyRequestEvent *>( DEBUG_NEW CEDirectConnectionNotifyRequestEvent(stream) );
}

const CEVersion & CEDirectConnectionStub::GetImplVersion( void ) const
{
    return NEDirectConnection::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_SendNotification);
void CEDirectConnectionStub::SendNotification( unsigned int msgId )
{
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        mInitiatedConnectionsState = NEService::DATA_OK;
        stream << mInitiatedConnections;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_CEDirectConnectionStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ErrorRequest);
void CEDirectConnectionStub::ErrorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        mInitiatedConnectionsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_ResponseConnectoinSetup:
    case NEDirectConnection::MSG_ID_ResponseAddParticipant:
    case NEDirectConnection::MSG_ID_ResponseRemoveParticipant:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_RequestConnectoinSetup:
    case NEDirectConnection::MSG_ID_RequestAddParticipant:
    case NEDirectConnection::MSG_ID_RequestRemoveParticipant:
    case NEDirectConnection::MSG_ID_RequestCloseConnection:
        listenerId = NEDirectConnection::GetResponseId(static_cast< NEDirectConnection::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEDirectConnection::GetString( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NEDirectConnection::GetString(static_cast<NEDirectConnection::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CEDirectConnectionResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void CEDirectConnectionStub::SetInitiatedConnections( const NEDirectConnection::MapParticipants & newValue )
{
    if ( (mInitiatedConnectionsState != NEService::DATA_OK) || (mInitiatedConnections != newValue) )
    {
        mInitiatedConnections = newValue;
        SendNotification( NEDirectConnection::MSG_ID_InitiatedConnections );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void CEDirectConnectionStub::ResponseConnectoinSetup( const bool & succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_ResponseConnectoinSetup;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << succeeded;
    stream << target;
    stream << initiator;
    stream << listParticipants;
    SendResponseEvent( msgId, args );
}

void CEDirectConnectionStub::ResponseAddParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_ResponseAddParticipant;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    SendResponseEvent( msgId, args );
}

void CEDirectConnectionStub::ResponseRemoveParticipant( const bool & succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_ResponseRemoveParticipant;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ProcessRequestEvent);
void CEDirectConnectionStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CEDirectConnectionRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CEDirectConnectionRequestEvent);

    if ( (reqEvent != static_cast<CEDirectConnectionRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CEDirectConnectionRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );
        NEDirectConnection::eMessageIDs respId = NEDirectConnection::GetResponseId(static_cast<NEDirectConnection::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectConnection::eMessageIDs>(reqId) )
        {
        case NEDirectConnection::MSG_ID_RequestConnectoinSetup:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                RequestConnectoinSetup( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_RequestAddParticipant:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                RequestAddParticipant( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_RequestRemoveParticipant:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                RequestRemoveParticipant( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_RequestCloseConnection:
            if ( true )
            {
                NEDirectConnection::sInitiator  initiator;
                stream >> initiator;                
                RequestCloseConnection( initiator );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ProcessAttributeEvent);
void CEDirectConnectionStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NEDirectConnection::eMessageIDs updId  = static_cast<NEDirectConnection::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectConnection::GetString(updId)
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
            case NEDirectConnection::MSG_ID_InitiatedConnections:
                if ( mInitiatedConnectionsState == NEService::DATA_OK )
                    stream << mInitiatedConnections;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_CEDirectConnectionStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEDirectConnection::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEDirectConnection::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
