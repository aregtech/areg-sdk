//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:52:18 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/PubSubStub.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PubSubStub.hpp"
#include "generated/src/private/PubSubEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PubSubStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PubSubStub::PubSubStub( Component & masterComp )
    : StubBase    ( masterComp, NEPubSub::getInterfaceData() )

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

void PubSubStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_PubSubStub_startupServiceInterface);
void PubSubStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_PubSubStub_startupServiceInterface);

    PubSubRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PubSubNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PubSubStub_shutdownServiceIntrface);
void PubSubStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PubSubStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    PubSubRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PubSubNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PubSubStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PubSubResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PubSubResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * PubSubStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW PubSubRequestEvent(stream) );
}

RemoteNotifyRequestEvent * PubSubStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW PubSubNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_PubSubStub_sendNotification);
void PubSubStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream { args.getStreamForWrite() };

    switch ( static_cast<NEPubSub::eMessageIDs>(msgId) )
    {
    case NEPubSub::eMessageIDs::MsgId_StringOnChange:
        mStringOnChangeState = NEService::eDataStateType::DataIsOK;
        stream << mStringOnChange;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NEPubSub::eMessageIDs::MsgId_IntegerAlways:
        mIntegerAlwaysState = NEService::eDataStateType::DataIsOK;
        stream << mIntegerAlways;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
        mServiceProviderStateState = NEService::eDataStateType::DataIsOK;
        stream << mServiceProviderState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PubSubStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_PubSubStub_errorRequest);
void PubSubStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result { NEService::eResultType::NotProcessed };
    msg_id listenerId { msgId };

    switch ( static_cast<NEPubSub::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEPubSub::eMessageIDs::MsgId_StringOnChange:
        mStringOnChangeState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NEPubSub::eMessageIDs::MsgId_IntegerAlways:
        mIntegerAlwaysState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
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
        TRACE_SCOPE(generated_src_PubSubStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEPubSub::getString( static_cast<NEPubSub::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEPubSub::getString(static_cast<NEPubSub::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, PubSubResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void PubSubStub::setStringOnChange( const String & newValue )
{
    if ( (mStringOnChangeState != NEService::eDataStateType::DataIsOK) || (mStringOnChange != newValue) )
    {
        mStringOnChange = newValue;
        sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_StringOnChange) );
    }
}

void PubSubStub::setIntegerAlways( const unsigned int & newValue )
{
    if ( (mIntegerAlwaysState != NEService::eDataStateType::DataIsOK) || (true) )
    {
        mIntegerAlways = newValue;
        sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_IntegerAlways) );
    }
}

void PubSubStub::setServiceProviderState( const NEPubSub::eServiceState & newValue )
{
    if ( (mServiceProviderStateState != NEService::eDataStateType::DataIsOK) || (mServiceProviderState != newValue) )
    {
        mServiceProviderState = newValue;
        sendNotification( static_cast<msg_id>(NEPubSub::eMessageIDs::MsgId_ServiceProviderState) );
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

DEF_TRACE_SCOPE(generated_src_PubSubStub_processRequestEvent);
void PubSubStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    PubSubRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, PubSubRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        TRACE_SCOPE(generated_src_PubSubStub_processRequestEvent);
        TRACE_ERR("The service PubSub has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
        ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_PubSubStub_processAttributeEvent);
void PubSubStub::processAttributeEvent( ServiceRequestEvent & eventElem )
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
        NEPubSub::eMessageIDs updId  { static_cast<NEPubSub::eMessageIDs>(eventElem.getRequestId()) };
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_PubSubStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPubSub::getString(updId)
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
            case NEPubSub::eMessageIDs::MsgId_StringOnChange:
                if ( mStringOnChangeState == NEService::eDataStateType::DataIsOK )
                    stream << mStringOnChange;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NEPubSub::eMessageIDs::MsgId_IntegerAlways:
                if ( mIntegerAlwaysState == NEService::eDataStateType::DataIsOK )
                    stream << mIntegerAlways;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NEPubSub::eMessageIDs::MsgId_ServiceProviderState:
                if ( mServiceProviderStateState == NEService::eDataStateType::DataIsOK )
                    stream << mServiceProviderState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_PubSubStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEPubSub::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEPubSub::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubStub.cpp file
//////////////////////////////////////////////////////////////////////////
