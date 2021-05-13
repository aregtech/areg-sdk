//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/SystemStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     09.04.2021  13:21:49 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/SystemStub.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/SystemStub.hpp"
#include "src/gen/private/SystemEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SystemStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemStub::SystemStub( Component & masterComp )
    : StubBase    ( masterComp, NESystem::CreateInterfaceData() )
    
    , mSystemState        (  )
    , mSystemStateState   ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

SystemStub::~SystemStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void SystemStub::UnlockAllRequests( void )
{
    StubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void SystemStub::ProcessStubEvent( StubEvent & eventElem )
{
    ; // do nothing
}

void SystemStub::ProcessGenericEvent( Event & eventElem )
{
    ; // do nothing
}

DEF_TRA_SCOPE(src_gen_SystemStub_StartupServiceInterface);
void SystemStub::StartupServiceInterface( Component & holder )
{
    TRACE_SCOPE(src_gen_SystemStub_StartupServiceInterface);
    
    SystemRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), Thread::GetCurrentThreadName() );
    SystemNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), Thread::GetCurrentThreadName() );
    StubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(src_gen_SystemStub_ShutdownServiceIntrface);
void SystemStub::ShutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(src_gen_SystemStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    SystemRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), Thread::GetCurrentThreadName() );
    SystemNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), Thread::GetCurrentThreadName() );
    StubBase::ShutdownServiceIntrface( holder );
}

const unsigned int SystemStub::GetNumberOfRequests( void ) const
{
    return NESystem::NumberofRequests;
}

const unsigned int SystemStub::GetNumberOfResponses( void ) const
{
    return NESystem::NumberofResponses;
}

const unsigned int SystemStub::GetNumberOfAttributes( void ) const
{
    return NESystem::NumberofAttributes;
}

const unsigned int * SystemStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::RequestIds);
}

const unsigned int * SystemStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::ResponseIds);
}

const unsigned int * SystemStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::AttributeIds);
}

ResponseEvent * SystemStub::CreateResponseEvent( const ProxyAddress & proxy, const unsigned int msgId, const NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW SystemResponseEvent(data, proxy, result, msgId) : DEBUG_NEW SystemResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * SystemStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<RemoteRequestEvent *>(NULL);
}

RemoteNotifyRequestEvent * SystemStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<RemoteNotifyRequestEvent *>(NULL);
}

const Version & SystemStub::GetImplVersion( void ) const
{
    return NESystem::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_SystemStub_SendNotification);
void SystemStub::SendNotification( const unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<const NESystem::eMessageIDs>(msgId) )
    {
    case NESystem::MSG_ID_SystemState:
        mSystemStateState = NEService::DATA_OK;
        stream << mSystemState;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_SystemStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(src_gen_SystemStub_ErrorRequest);
void SystemStub::ErrorRequest( const unsigned int msgId, const bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<const NESystem::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESystem::MSG_ID_SystemState:
        mSystemStateState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
/************************************************************************
 * Request errors
 ************************************************************************/
    case NESystem::MSG_ID_RequestPowerOff:
    case NESystem::MSG_ID_RequestPowerOn:
    case NESystem::MSG_ID_RequestShutdown:
    case NESystem::MSG_ID_RequestStart:
        listenerId = NESystem::GetResponseId(static_cast<const NESystem::eMessageIDs>(msgId));
        result = msgCancel ? NEService::RESULT_REQUEST_CANCELED : NEService::RESULT_REQUEST_ERROR;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }
    
    StubBase::StubListenerList listeners;
    if ( FindListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(src_gen_SystemStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NESystem::GetString( static_cast<NESystem::eMessageIDs>(msgId) )
                        , StubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NESystem::GetString(static_cast<NESystem::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        StubBase::SendErrorNotification(listeners, SystemResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void SystemStub::SetSystemState( const NESystem::eSystemState & newValue )
{
    if ( (mSystemStateState != NEService::DATA_OK) || (mSystemState != newValue) )
    {
        mSystemState = newValue;
        SendNotification( NESystem::MSG_ID_SystemState );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_SystemStub_ProcessRequestEvent);
void SystemStub::ProcessRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    SystemRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SystemRequestEvent);

    if ( (reqEvent != static_cast<SystemRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        StubBase::Listener listener( reqId, 0, reqEvent->GetEventSource() );

        switch ( static_cast<NESystem::eMessageIDs>(reqId) )
        {
        case NESystem::MSG_ID_RequestPowerOff:
            if ( true )
            {
                RequestPowerOff(  );
            }
            break;
            
        case NESystem::MSG_ID_RequestPowerOn:
            if ( true )
            {
                RequestPowerOn(  );
            }
            break;
            
        case NESystem::MSG_ID_RequestShutdown:
            if ( true )
            {
                RequestShutdown(  );
            }
            break;
            
        case NESystem::MSG_ID_RequestStart:
            if ( true )
            {
                RequestStart(  );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_SystemStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_SystemStub_ProcessAttributeEvent);
void SystemStub::ProcessAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NESystem::eMessageIDs updId  = static_cast<NESystem::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_SystemStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESystem::GetString(updId)
                            , ProxyAddress::ConvertAddressToPath(eventElem.GetEventSource()).GetBuffer());
            }
#else   // _DEBUG
            AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataInternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.GetStreamForWrite();

            switch (updId)
            {
            case NESystem::MSG_ID_SystemState:
                if ( mSystemStateState == NEService::DATA_OK )
                    stream << mSystemState;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_SystemStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NESystem::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NESystem::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<const unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/SystemStub.cpp file
//////////////////////////////////////////////////////////////////////////
