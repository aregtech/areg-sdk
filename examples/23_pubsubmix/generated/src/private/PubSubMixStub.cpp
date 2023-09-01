//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     31.08.2023  12:34:15 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/PubSubMixStub.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PubSubMixStub.hpp"
#include "generated/src/private/PubSubMixEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PubSubMixStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PubSubMixStub::PubSubMixStub( Component & masterComp )
    : StubBase    ( masterComp, NEPubSubMix::getInterfaceData() )

    , mStringOnChange             (  )
    , mStringOnChangeState        ( NEService::eDataStateType::DataIsUnavailable )

    , mIntegerAlways              (  )
    , mIntegerAlwaysState         ( NEService::eDataStateType::DataIsUnavailable )

    , mServiceProviderState       (  )
    , mServiceProviderStateState  ( NEService::eDataStateType::DataIsUnavailable )
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void PubSubMixStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_startupServiceInterface);
void PubSubMixStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_PubSubMixStub_startupServiceInterface);

    PubSubMixRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PubSubMixNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_shutdownServiceIntrface);
void PubSubMixStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PubSubMixStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    PubSubMixRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PubSubMixNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PubSubMixStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PubSubMixResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PubSubMixResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * PubSubMixStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW PubSubMixRequestEvent(stream) );
}

RemoteNotifyRequestEvent * PubSubMixStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW PubSubMixNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_sendNotification);
void PubSubMixStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream { args.getStreamForWrite() };

    switch ( static_cast<NEPubSubMix::eMessageIDs>(msgId) )
    {
    case NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
        mStringOnChangeState = NEService::eDataStateType::DataIsOK;
        stream << mStringOnChange;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
        mIntegerAlwaysState = NEService::eDataStateType::DataIsOK;
        stream << mIntegerAlways;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
        mServiceProviderStateState = NEService::eDataStateType::DataIsOK;
        stream << mServiceProviderState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PubSubMixStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_errorRequest);
void PubSubMixStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result { NEService::eResultType::NotProcessed };
    msg_id listenerId { msgId };

    switch ( static_cast<NEPubSubMix::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
        mStringOnChangeState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
        mIntegerAlwaysState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
        mServiceProviderStateState = NEService::eDataStateType::DataIsInvalid;
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
        TRACE_SCOPE(generated_src_PubSubMixStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEPubSubMix::getString( static_cast<NEPubSubMix::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEPubSubMix::getString(static_cast<NEPubSubMix::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, PubSubMixResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void PubSubMixStub::setStringOnChange( const NEPubSubMix::sString & newValue )
{
    if ( (mStringOnChangeState != NEService::eDataStateType::DataIsOK) || (mStringOnChange != newValue) )
    {
        mStringOnChange = newValue;
        sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_StringOnChange) );
    }
}

void PubSubMixStub::setIntegerAlways( const NEPubSubMix::sInteger & newValue )
{
    if ( (mIntegerAlwaysState != NEService::eDataStateType::DataIsOK) || (true) )
    {
        mIntegerAlways = newValue;
        sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_IntegerAlways) );
    }
}

void PubSubMixStub::setServiceProviderState( const NEPubSubMix::eServiceState & newValue )
{
    if ( (mServiceProviderStateState != NEService::eDataStateType::DataIsOK) || (mServiceProviderState != newValue) )
    {
        mServiceProviderState = newValue;
        sendNotification( static_cast<msg_id>(NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState) );
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

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_processRequestEvent);
void PubSubMixStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    PubSubMixRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, PubSubMixRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        TRACE_SCOPE(generated_src_PubSubMixStub_processRequestEvent);
        TRACE_ERR("The service PubSubMix has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
        ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_PubSubMixStub_processAttributeEvent);
void PubSubMixStub::processAttributeEvent( ServiceRequestEvent & eventElem )
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
        NEPubSubMix::eMessageIDs updId  { static_cast<NEPubSubMix::eMessageIDs>(eventElem.getRequestId()) };
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_PubSubMixStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPubSubMix::getString(updId)
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
            case NEPubSubMix::eMessageIDs::MsgId_StringOnChange:
                if ( mStringOnChangeState == NEService::eDataStateType::DataIsOK )
                    stream << mStringOnChange;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NEPubSubMix::eMessageIDs::MsgId_IntegerAlways:
                if ( mIntegerAlwaysState == NEService::eDataStateType::DataIsOK )
                    stream << mIntegerAlways;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NEPubSubMix::eMessageIDs::MsgId_ServiceProviderState:
                if ( mServiceProviderStateState == NEService::eDataStateType::DataIsOK )
                    stream << mServiceProviderState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_PubSubMixStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEPubSubMix::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEPubSubMix::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubMixStub.cpp file
//////////////////////////////////////////////////////////////////////////
