//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CEMessageOutputStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CEMessageOutputStub.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/CEMessageOutputStub.hpp"
#include "src/gen/private/CEMessageOutputEvents.hpp"

#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEMessageOutputStub::CEMessageOutputStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NEMessageOutput::CreateInterfaceData() )
    
    , mLastMessage        (  )
    , mLastMessageState   ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

CEMessageOutputStub::~CEMessageOutputStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CEMessageOutputStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CEMessageOutputStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CEMessageOutputStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_StartupServiceInterface);
void CEMessageOutputStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(src_gen_CEMessageOutputStub_StartupServiceInterface);
    
    CEMessageOutputRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEMessageOutputNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_ShutdownServiceIntrface);
void CEMessageOutputStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(src_gen_CEMessageOutputStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CEMessageOutputRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEMessageOutputNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

unsigned int CEMessageOutputStub::GetNumberOfRequests( void ) const
{
    return NEMessageOutput::NumberofRequests;
}

unsigned int CEMessageOutputStub::GetNumberOfResponses( void ) const
{
    return NEMessageOutput::NumberofResponses;
}

unsigned int CEMessageOutputStub::GetNumberOfAttributes( void ) const
{
    return NEMessageOutput::NumberofAttributes;
}

const unsigned int * CEMessageOutputStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::RequestIds);
}

const unsigned int * CEMessageOutputStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::ResponseIds);
}

const unsigned int * CEMessageOutputStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::AttributeIds);
}

CEResponseEvent * CEMessageOutputStub::CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CEMessageOutputResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CEMessageOutputResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CEMessageOutputStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<CERemoteRequestEvent *>(NULL);
}

CERemoteNotifyRequestEvent * CEMessageOutputStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<CERemoteNotifyRequestEvent *>(NULL);
}

const CEVersion & CEMessageOutputStub::GetImplVersion( void ) const
{
    return NEMessageOutput::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_SendNotification);
void CEMessageOutputStub::SendNotification( unsigned int msgId )
{
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
    {
    case NEMessageOutput::MSG_ID_LastMessage:
        mLastMessageState = NEService::DATA_OK;
        stream << mLastMessage;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_CEMessageOutputStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_ErrorRequest);
void CEMessageOutputStub::ErrorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_LastMessage:
        mLastMessageState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_BroadcastActionRequest:
    case NEMessageOutput::MSG_ID_BroadcastInputMessage:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_RequestOutputMessage:
        listenerId = NEMessageOutput::GetResponseId(static_cast< NEMessageOutput::eMessageIDs>(msgId));
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
        TRACE_SCOPE(src_gen_CEMessageOutputStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEMessageOutput::GetString( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NEMessageOutput::GetString(static_cast<NEMessageOutput::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CEMessageOutputResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void CEMessageOutputStub::SetLastMessage( const CEString & newValue )
{
    if ( (mLastMessageState != NEService::DATA_OK) || (mLastMessage != newValue) )
    {
        mLastMessage = newValue;
        SendNotification( NEMessageOutput::MSG_ID_LastMessage );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void CEMessageOutputStub::BroadcastActionRequest( const NEMessageOutput::eRequestedActons & userAction )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_BroadcastActionRequest;
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << userAction;
    SendResponseEvent( msgId, args );
}

void CEMessageOutputStub::BroadcastInputMessage( const CEString & msgInput )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_BroadcastInputMessage;
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << msgInput;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_ProcessRequestEvent);
void CEMessageOutputStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CEMessageOutputRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CEMessageOutputRequestEvent);

    if ( (reqEvent != static_cast<CEMessageOutputRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CEMessageOutputRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );

        switch ( static_cast<NEMessageOutput::eMessageIDs>(reqId) )
        {
        case NEMessageOutput::MSG_ID_RequestOutputMessage:
            if ( true )
            {
                CEString    msgOutput;
                CEDateTime  dateTime;
                stream >> msgOutput;                
                stream >> dateTime;                
                RequestOutputMessage( msgOutput, dateTime );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_CEMessageOutputStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_CEMessageOutputStub_ProcessAttributeEvent);
void CEMessageOutputStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NEMessageOutput::eMessageIDs updId  = static_cast<NEMessageOutput::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_CEMessageOutputStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEMessageOutput::GetString(updId)
                            , CEProxyAddress::ConvertAddressToPath(eventElem.GetEventSource()).GetBuffer());
            }
#else   // _DEBUG
            AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
#endif  // _DEBUG
            CEEventDataStream args(CEEventDataStream::EventDataInternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.GetStreamForWrite();

            switch (updId)
            {
            case NEMessageOutput::MSG_ID_LastMessage:
                if ( mLastMessageState == NEService::DATA_OK )
                    stream << mLastMessage;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_CEMessageOutputStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEMessageOutput::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEMessageOutput::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CEMessageOutputStub.cpp file
//////////////////////////////////////////////////////////////////////////
