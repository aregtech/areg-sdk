//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CETrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CETrafficLightStub.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/CETrafficLightStub.hpp"
#include "src/gen/private/CETrafficLightEvents.hpp"

#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CETrafficLightStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CETrafficLightStub::CETrafficLightStub( CEComponent & masterComp )
    : CEStubBase    ( masterComp, NETrafficLight::CreateInterfaceData() )
    
    , mTrafficLight       (  )
    , mTrafficLightState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

CETrafficLightStub::~CETrafficLightStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CETrafficLightStub::UnlockAllRequests( void )
{
    CEStubBase::CancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

void CETrafficLightStub::ProcessStubEvent( CEStubEvent & eventElem )
{
    ; // do nothing
}

void CETrafficLightStub::ProcessGenericEvent( CEEvent & eventElem )
{
    ; // do nothing
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_StartupServiceInterface);
void CETrafficLightStub::StartupServiceInterface( CEComponent & holder )
{
    TRACE_SCOPE(src_gen_CETrafficLightStub_StartupServiceInterface);
    
    CETrafficLightRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CETrafficLightNotifyRequestEvent::AddListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::StartupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_ShutdownServiceIntrface);
void CETrafficLightStub::ShutdownServiceIntrface( CEComponent & holder )
{
    TRACE_SCOPE(src_gen_CETrafficLightStub_ShutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.GetServiceName().GetBuffer(), mAddress.GetRoleName().GetBuffer());
    
    CETrafficLightRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CETrafficLightNotifyRequestEvent::RemoveListener( static_cast<IEEventConsumer &>(self()), CEThread::GetCurrentThreadName() );
    CEStubBase::ShutdownServiceIntrface( holder );
}

unsigned int CETrafficLightStub::GetNumberOfRequests( void ) const
{
    return NETrafficLight::NumberofRequests;
}

unsigned int CETrafficLightStub::GetNumberOfResponses( void ) const
{
    return NETrafficLight::NumberofResponses;
}

unsigned int CETrafficLightStub::GetNumberOfAttributes( void ) const
{
    return NETrafficLight::NumberofAttributes;
}

const unsigned int * CETrafficLightStub::GetRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::RequestIds);
}

const unsigned int * CETrafficLightStub::GetResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::ResponseIds);
}

const unsigned int * CETrafficLightStub::GetAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::AttributeIds);
}

CEResponseEvent * CETrafficLightStub::CreateResponseEvent( const CEProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const CEEventDataStream & data ) const
{
    return (data.IsEmpty() == false ? DEBUG_NEW CETrafficLightResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CETrafficLightResponseEvent(proxy, result, msgId));
}

CERemoteRequestEvent * CETrafficLightStub::CreateRemoteServiceRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<CERemoteRequestEvent *>(NULL);
}

CERemoteNotifyRequestEvent * CETrafficLightStub::CreateRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    // not relevant for local stub
    return static_cast<CERemoteNotifyRequestEvent *>(NULL);
}

const CEVersion & CETrafficLightStub::GetImplVersion( void ) const
{
    return NETrafficLight::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_SendNotification);
void CETrafficLightStub::SendNotification( unsigned int msgId )
{
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();

    switch ( static_cast<NETrafficLight::eMessageIDs>(msgId) )
    {
    case NETrafficLight::MSG_ID_TrafficLight:
        mTrafficLightState = NEService::DATA_OK;
        stream << mTrafficLight;
        SendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_CETrafficLightStub_SendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
            ASSERT(false);
        }
        break; // do nothing
    }
}
DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_ErrorRequest);
void CETrafficLightStub::ErrorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NETrafficLight::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NETrafficLight::MSG_ID_TrafficLight:
        mTrafficLightState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NETrafficLight::MSG_ID_ResponseChangeLight:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NETrafficLight::MSG_ID_RequestChangeLight:
        listenerId = NETrafficLight::GetResponseId(static_cast< NETrafficLight::eMessageIDs>(msgId));
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
        TRACE_SCOPE(src_gen_CETrafficLightStub_ErrorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NETrafficLight::GetString( static_cast<NETrafficLight::eMessageIDs>(msgId) )
                        , CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer()
                        , NEService::GetString( result )
                        , NETrafficLight::GetString(static_cast<NETrafficLight::eMessageIDs>(msgId))
                        , listeners.GetSize() );

        CEStubBase::SendErrorNotification(listeners, CETrafficLightResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void CETrafficLightStub::SetTrafficLight( const NETrafficLight::eLight & newValue )
{
    if ( (mTrafficLightState != NEService::DATA_OK) || (mTrafficLight != newValue) )
    {
        mTrafficLight = newValue;
        SendNotification( NETrafficLight::MSG_ID_TrafficLight );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void CETrafficLightStub::ResponseChangeLight( const NETrafficLight::eLight & lightColor )
{
    static const NETrafficLight::eMessageIDs msgId = NETrafficLight::MSG_ID_ResponseChangeLight;
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << lightColor;
    SendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_ProcessRequestEvent);
void CETrafficLightStub::ProcessRequestEvent( CEServiceRequestEvent & eventElem )
{
    ASSERT( NEService::IsRequestId(eventElem.GetRequestId()) );
    CETrafficLightRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CETrafficLightRequestEvent);

    if ( (reqEvent != static_cast<CETrafficLightRequestEvent *>(NULL)) && (reqEvent->GetRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->GetRequestId();
        const IEInStream & stream  = static_cast<const CETrafficLightRequestEvent *>(reqEvent)->GetData().GetReadStream();
        CEStubBase::CEListener listener( reqId, 0, reqEvent->GetEventSource() );
        NETrafficLight::eMessageIDs respId = NETrafficLight::GetResponseId(static_cast<NETrafficLight::eMessageIDs>(reqId));

        switch ( static_cast<NETrafficLight::eMessageIDs>(reqId) )
        {
        case NETrafficLight::MSG_ID_RequestChangeLight:
            if ( CanExecuteRequest(listener, respId, reqEvent->GetSequenceNumber()) )
            {
                NETrafficLight::eLight  lightColor;
                bool                    holdon;
                stream >> lightColor;                
                stream >> holdon;                
                RequestChangeLight( lightColor, holdon );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_CETrafficLightStub_ProcessRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, CEStubAddress::ConvertAddressToPath(mAddress).GetBuffer());
                ASSERT(false);
            }
            break;
        }
    }
    CancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_CETrafficLightStub_ProcessAttributeEvent);
void CETrafficLightStub::ProcessAttributeEvent( CEServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.GetRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        CEIntegerArray removedIds;
        CEStubBase::ClearAllListeners(eventElem.GetEventSource(), removedIds);
    }
    else
    {
        NETrafficLight::eMessageIDs updId  = static_cast<NETrafficLight::eMessageIDs>(eventElem.GetRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            RemoveNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (AddNotificationListener( static_cast<unsigned int>(updId), eventElem.GetEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_CETrafficLightStub_ProcessAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NETrafficLight::GetString(updId)
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
            case NETrafficLight::MSG_ID_TrafficLight:
                if ( mTrafficLightState == NEService::DATA_OK )
                    stream << mTrafficLight;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::IsResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_CETrafficLightStub_ProcessAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, CEStubAddress::ConvertAddressToPath(GetAddress()).GetBuffer());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NETrafficLight::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NETrafficLight::MSG_ID_NO_PROCEED)
                SendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CETrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
