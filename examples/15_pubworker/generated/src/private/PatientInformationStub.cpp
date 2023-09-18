//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:33:58 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/PatientInformationStub.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PatientInformationStub.hpp"
#include "generated/src/private/PatientInformationEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PatientInformationStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PatientInformationStub::PatientInformationStub( Component & masterComp )
    : StubBase    ( masterComp, NEPatientInformation::getInterfaceData() )

    , mPatient        (  )
    , mPatientState   ( NEService::eDataStateType::DataIsUnavailable )
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void PatientInformationStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_startupServiceInterface);
void PatientInformationStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_PatientInformationStub_startupServiceInterface);

    PatientInformationRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PatientInformationNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_shutdownServiceIntrface);
void PatientInformationStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PatientInformationStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    PatientInformationRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PatientInformationNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PatientInformationStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PatientInformationResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PatientInformationResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * PatientInformationStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW PatientInformationRequestEvent(stream) );
}

RemoteNotifyRequestEvent * PatientInformationStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW PatientInformationNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_sendNotification);
void PatientInformationStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream { args.getStreamForWrite() };

    switch ( static_cast<NEPatientInformation::eMessageIDs>(msgId) )
    {
    case NEPatientInformation::eMessageIDs::MsgId_Patient:
        mPatientState = NEService::eDataStateType::DataIsOK;
        stream << mPatient;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PatientInformationStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_errorRequest);
void PatientInformationStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result { NEService::eResultType::NotProcessed };
    msg_id listenerId { msgId };

    switch ( static_cast<NEPatientInformation::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEPatientInformation::eMessageIDs::MsgId_Patient:
        mPatientState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
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
    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }

    StubBase::StubListenerList listeners;
    if ( findListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(generated_src_PatientInformationStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEPatientInformation::getString( static_cast<NEPatientInformation::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEPatientInformation::getString(static_cast<NEPatientInformation::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, PatientInformationResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void PatientInformationStub::setPatient( const NEPatientInformation::PatientInfo & newValue )
{
    if ( (mPatientState != NEService::eDataStateType::DataIsOK) || (mPatient != newValue) )
    {
        mPatient = newValue;
        sendNotification( static_cast<msg_id>(NEPatientInformation::eMessageIDs::MsgId_Patient) );
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

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_processRequestEvent);
void PatientInformationStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    PatientInformationRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, PatientInformationRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        TRACE_SCOPE(generated_src_PatientInformationStub_processRequestEvent);
        TRACE_ERR("The service PatientInformation has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
        ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_PatientInformationStub_processAttributeEvent);
void PatientInformationStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType { eventElem.getRequestType() };
    const ProxyAddress & source { eventElem.getEventSource( ) };
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(source, removedIds);
    }
    else
    {
        NEPatientInformation::eMessageIDs updId  { static_cast<NEPatientInformation::eMessageIDs>(eventElem.getRequestId()) };
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_PatientInformationStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPatientInformation::getString(updId)
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
            case NEPatientInformation::eMessageIDs::MsgId_Patient:
                if ( mPatientState == NEService::eDataStateType::DataIsOK )
                    stream << mPatient;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_PatientInformationStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEPatientInformation::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEPatientInformation::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationStub.cpp file
//////////////////////////////////////////////////////////////////////////
