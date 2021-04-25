//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CECentralMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source CentralMessager.
 * Generated at     03.09.2019  02:48:07 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CECentralMessagerStub.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/CECentralMessagerStub.hpp"
#include "shared/generated/private/CECentralMessagerEvents.hpp"

#include "areg/component/CEServiceResponseEvent.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CECentralMessagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CECentralMessagerStub::CECentralMessagerStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NECentralMessager::CreateInterfaceData() )
    
{
    ; // do nothing
}

CECentralMessagerStub::~CECentralMessagerStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CECentralMessagerStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CECentralMessagerStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CECentralMessagerStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_StartupServiceInterface);
void CECentralMessagerStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerStub_StartupServiceInterface);
    
    CECentralMessagerRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CECentralMessagerNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_ShutdownServiceIntrface);
void CECentralMessagerStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CECentralMessagerRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CECentralMessagerNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

const unsigned int CECentralMessagerStub::GetNumberOfRequests( void ) const
{
    return NECentralMessager::NumberofRequests;
}

const unsigned int CECentralMessagerStub::GetNumberOfResponses( void ) const
{
    return NECentralMessager::NumberofResponses;
}

const unsigned int CECentralMessagerStub::GetNumberOfAttributes( void ) const
{
    return NECentralMessager::NumberofAttributes;
}

const unsigned int * CECentralMessagerStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NECentralMessager::RequestIds);
}

const unsigned int * CECentralMessagerStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NECentralMessager::ResponseIds);
}

const unsigned int * CECentralMessagerStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NECentralMessager::AttributeIds);
}

CEResponseEvent * CECentralMessagerStub::CreateResponseEvent( const CEProxyAddress & proxy, const unsigned int msgId, const NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CECentralMessagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CECentralMessagerResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CECentralMessagerStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteRequestEvent *>( DEBUG_NEW CECentralMessagerRequestEvent(stream) );
}

CERemoteNotifyRequestEvent * CECentralMessagerStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<CERemoteNotifyRequestEvent *>( DEBUG_NEW CECentralMessagerNotifyRequestEvent(stream) );
}

const CEVersion & CECentralMessagerStub::GetImplVersion( void ) const
{
    return NECentralMessager::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_SendNotification);
void CECentralMessagerStub::SendNotification( const unsigned int msgId )
{
    TRACE_SCOPE(shared_generated_CECentralMessagerStub_SendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_ErrorRequest);
void CECentralMessagerStub::ErrorRequest( const unsigned int msgId, const bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<const NECentralMessager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NECentralMessager::MSG_ID_BroadcastSendMessage:
    case NECentralMessager::MSG_ID_BroadcastKeyTyping:
    case NECentralMessager::MSG_ID_BroadcastBroadcastMessage:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NECentralMessager::MSG_ID_RequestSendMessage:
    case NECentralMessager::MSG_ID_RequestKeyTyping:
        listenerId = NECentralMessager::GetResponseId(static_cast<const NECentralMessager::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_CECentralMessagerStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NECentralMessager::GetString( static_cast<NECentralMessager::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NECentralMessager::GetString(static_cast<NECentralMessager::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CECentralMessagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void CECentralMessagerStub::BroadcastSendMessage( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage, const CEDateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_BroadcastSendMessage;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    stream << dateTime;
    SendResponseEvent( msgId, args );
}

void CECentralMessagerStub::BroadcastKeyTyping( const CEString & nickName, const unsigned int & cookie, const CEString & newMessage )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_BroadcastKeyTyping;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    SendResponseEvent( msgId, args );
}

void CECentralMessagerStub::BroadcastBroadcastMessage( const CEString & serverMessage, const CEDateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_BroadcastBroadcastMessage;
    CEEventDataStream args(CEEventDataStream::EventDataExternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << serverMessage;
    stream << dateTime;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_ProcessRequestEvent);
void CECentralMessagerStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CECentralMessagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CECentralMessagerRequestEvent);

    if ( (reqEvent != static_cast<CECentralMessagerRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CECentralMessagerRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );

        switch ( static_cast<NECentralMessager::eMessageIDs>(reqId) )
        {
        case NECentralMessager::MSG_ID_RequestSendMessage:
            if ( true )
            {
                CEString        nickName;
                unsigned int    cookie;
                CEString        newMessage;
                CEDateTime      dateTime;
                stream >> nickName;                
                stream >> cookie;                
                stream >> newMessage;                
                stream >> dateTime;                
                RequestSendMessage( nickName, cookie, newMessage, dateTime );
            }
            break;
            
        case NECentralMessager::MSG_ID_RequestKeyTyping:
            if ( true )
            {
                CEString        nickName;
                unsigned int    cookie;
                CEString        newMessage;
                stream >> nickName;                
                stream >> cookie;                
                stream >> newMessage;                
                RequestKeyTyping( nickName, cookie, newMessage );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_CECentralMessagerStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_CECentralMessagerStub_ProcessAttributeEvent);
void CECentralMessagerStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NECentralMessager::eMessageIDs updId  = static_cast<NECentralMessager::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_CECentralMessagerStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NECentralMessager::GetString(updId)
                            , CEProxyAddress::ConvertAddressToPath(eventElem.GetEventSource()).GetBuffer());
            }
#else   // _DEBUG
            AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
            {
                TRACE_SCOPE(shared_generated_CECentralMessagerStub_ProcessAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CECentralMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
