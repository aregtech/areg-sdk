//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     25.04.2021  23:50:44 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEConnectionManagerStub.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/CEConnectionManagerStub.hpp"
#include "shared/generated/private/CEConnectionManagerEvents.hpp"

#include "areg/component/CEServiceResponseEvent.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEConnectionManagerStub::CEConnectionManagerStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NEConnectionManager::CreateInterfaceData() )
    
    , mConnectionList         (  )
    , mConnectionListState    ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

CEConnectionManagerStub::~CEConnectionManagerStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CEConnectionManagerStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CEConnectionManagerStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CEConnectionManagerStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_StartupServiceInterface);
void CEConnectionManagerStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerStub_StartupServiceInterface);
    
    CEConnectionManagerRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEConnectionManagerNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ShutdownServiceIntrface);
void CEConnectionManagerStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CEConnectionManagerRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEConnectionManagerNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

unsigned int CEConnectionManagerStub::GetNumberOfRequests( void ) const
{
    return NEConnectionManager::NumberofRequests;
}

unsigned int CEConnectionManagerStub::GetNumberOfResponses( void ) const
{
    return NEConnectionManager::NumberofResponses;
}

unsigned int CEConnectionManagerStub::GetNumberOfAttributes( void ) const
{
    return NEConnectionManager::NumberofAttributes;
}

const unsigned int * CEConnectionManagerStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::RequestIds);
}

const unsigned int * CEConnectionManagerStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::ResponseIds);
}

const unsigned int * CEConnectionManagerStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEConnectionManager::AttributeIds);
}

CEResponseEvent * CEConnectionManagerStub::CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CEConnectionManagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CEConnectionManagerResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CEConnectionManagerStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteRequestEvent *>( DEBUG_NEW CEConnectionManagerRequestEvent(stream) );
}

CERemoteNotifyRequestEvent * CEConnectionManagerStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteNotifyRequestEvent *>( DEBUG_NEW CEConnectionManagerNotifyRequestEvent(stream) );
}

const CEVersion & CEConnectionManagerStub::GetImplVersion( void ) const
{
    return NEConnectionManager::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_SendNotification);
void CEConnectionManagerStub::SendNotification( unsigned int msgId )
{
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
    {
    case NEConnectionManager::MSG_ID_ConnectionList:
        mConnectionListState = NEService::DATA_OK;
        stream << mConnectionList;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_CEConnectionManagerStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ErrorRequest);
void CEConnectionManagerStub::ErrorRequest( unsigned int msgId, bool msgCancel )
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
    case NEConnectionManager::MSG_ID_ResponseConnect:
    case NEConnectionManager::MSG_ID_ResponseRegisterConnection:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_BroadcastConnectionUpdated:
    case NEConnectionManager::MSG_ID_BroadcastClientConnected:
    case NEConnectionManager::MSG_ID_BroadcastClientDisconnected:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEConnectionManager::MSG_ID_RequestConnet:
    case NEConnectionManager::MSG_ID_RequestRegisterConnection:
    case NEConnectionManager::MSG_ID_RequestDiconnect:
        listenerId = NEConnectionManager::GetResponseId(static_cast< NEConnectionManager::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEConnectionManager::GetString( static_cast<NEConnectionManager::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NEConnectionManager::GetString(static_cast<NEConnectionManager::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CEConnectionManagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void CEConnectionManagerStub::SetConnectionList( const NEConnectionManager::MapConnection & newValue )
{
    if ( (mConnectionListState != NEService::DATA_OK) || (mConnectionList != newValue) )
    {
        mConnectionList = newValue;
        SendNotification( NEConnectionManager::MSG_ID_ConnectionList );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void CEConnectionManagerStub::ResponseConnect( const CEString & nickName, const unsigned int & cookie, const CEDateTime & dateTime, const NEConnectionManager::eConnectionResult & result )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_ResponseConnect;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << dateTime;
    stream << result;
    SendResponseEvent( msgId, args );
}

void CEConnectionManagerStub::ResponseRegisterConnection( const NEConnectionManager::sConnection & connection, const NEConnectionManager::ListConnection & connectionList, const bool & success )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_ResponseRegisterConnection;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << connection;
    stream << connectionList;
    stream << success;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void CEConnectionManagerStub::BroadcastConnectionUpdated( const NEConnectionManager::MapConnection & updatedList )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_BroadcastConnectionUpdated;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << updatedList;
    SendResponseEvent( msgId, args );
}

void CEConnectionManagerStub::BroadcastClientConnected( const NEConnectionManager::sConnection & clientConnected )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_BroadcastClientConnected;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << clientConnected;
    SendResponseEvent( msgId, args );
}

void CEConnectionManagerStub::BroadcastClientDisconnected( const NEConnectionManager::sConnection & clientLeft )
{
    static const NEConnectionManager::eMessageIDs msgId = NEConnectionManager::MSG_ID_BroadcastClientDisconnected;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << clientLeft;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ProcessRequestEvent);
void CEConnectionManagerStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CEConnectionManagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CEConnectionManagerRequestEvent);

    if ( (reqEvent != static_cast<CEConnectionManagerRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CEConnectionManagerRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );
        NEConnectionManager::eMessageIDs respId = NEConnectionManager::GetResponseId(static_cast<NEConnectionManager::eMessageIDs>(reqId));

        switch ( static_cast<NEConnectionManager::eMessageIDs>(reqId) )
        {
        case NEConnectionManager::MSG_ID_RequestConnet:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                CEString    nickName;
                CEDateTime  dateTime;
                stream >> nickName;                
                stream >> dateTime;                
                RequestConnet( nickName, dateTime );
            }
            break;
            
        case NEConnectionManager::MSG_ID_RequestRegisterConnection:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                CEString        nickName;
                unsigned int    cookie;
                unsigned int    connectCookie;
                CEDateTime      dateRegister;
                stream >> nickName;                
                stream >> cookie;                
                stream >> connectCookie;                
                stream >> dateRegister;                
                RequestRegisterConnection( nickName, cookie, connectCookie, dateRegister );
            }
            break;
            
        case NEConnectionManager::MSG_ID_RequestDiconnect:
            if ( true )
            {
                CEString        nickName;
                unsigned int    cookie;
                CEDateTime      dateTime;
                stream >> nickName;                
                stream >> cookie;                
                stream >> dateTime;                
                RequestDiconnect( nickName, cookie, dateTime );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ProcessAttributeEvent);
void CEConnectionManagerStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NEConnectionManager::eMessageIDs updId  = static_cast<NEConnectionManager::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEConnectionManager::GetString(updId)
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
            case NEConnectionManager::MSG_ID_ConnectionList:
                if ( mConnectionListState == NEService::DATA_OK )
                    stream << mConnectionList;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_CEConnectionManagerStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEConnectionManager::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEConnectionManager::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEConnectionManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
