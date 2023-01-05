//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:38 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PowerManager.
 *
 * \file            generated/src/PowerManagerStub.hpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PowerManagerStub.hpp"
#include "generated/src/private/PowerManagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PowerManagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PowerManagerStub::PowerManagerStub( Component & masterComp )
    : StubBase    ( masterComp, NEPowerManager::getInterfaceData() )

    , mLightsPowerState       (  )
    , mLightsPowerStateState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void PowerManagerStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_startupServiceInterface);
void PowerManagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_PowerManagerStub_startupServiceInterface);

    PowerManagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PowerManagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_shutdownServiceIntrface);
void PowerManagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PowerManagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    PowerManagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    PowerManagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PowerManagerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PowerManagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PowerManagerResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_sendNotification);
void PowerManagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEPowerManager::eMessageIDs>(msgId) )
    {
    case NEPowerManager::eMessageIDs::MsgId_LightsPowerState:
        mLightsPowerStateState = NEService::eDataStateType::DataIsOK;
        stream << mLightsPowerState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PowerManagerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_errorRequest);
void PowerManagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEPowerManager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEPowerManager::eMessageIDs::MsgId_LightsPowerState:
        mLightsPowerStateState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight:
    case NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEPowerManager::eMessageIDs::MsgId_requestPowerOn:
    case NEPowerManager::eMessageIDs::MsgId_requestPowerOff:
    case NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight:
    case NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight:
        listenerId = static_cast<msg_id>(NEPowerManager::getResponseId(static_cast< NEPowerManager::eMessageIDs>(msgId)));
        result = msgCancel ? NEService::eResultType::RequestCanceled : NEService::eResultType::RequestError;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }

    StubBase::StubListenerList listeners;
    if ( findListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(generated_src_PowerManagerStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEPowerManager::getString( static_cast<NEPowerManager::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEPowerManager::getString(static_cast<NEPowerManager::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, PowerManagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void PowerManagerStub::setLightsPowerState( const NEPowerManager::ePoweredState & newValue )
{
    if ( (mLightsPowerStateState != NEService::eDataStateType::DataIsOK) || (mLightsPowerState != newValue) )
    {
        mLightsPowerState = newValue;
        sendNotification( static_cast<msg_id>(NEPowerManager::eMessageIDs::MsgId_LightsPowerState) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void PowerManagerStub::responseStartTrafficLight( bool Success )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_responseStartTrafficLight;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << Success;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void PowerManagerStub::responseStopTrafficLight( bool Success )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::eMessageIDs::MsgId_responseStopTrafficLight;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << Success;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_processRequestEvent);
void PowerManagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    PowerManagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, PowerManagerRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEPowerManager::eMessageIDs respId = NEPowerManager::getResponseId(static_cast<NEPowerManager::eMessageIDs>(reqId));

        switch ( static_cast<NEPowerManager::eMessageIDs>(reqId) )
        {
        case NEPowerManager::eMessageIDs::MsgId_requestPowerOn:
            if ( true )
            {
                requestPowerOn(  );
            }
            break;

        case NEPowerManager::eMessageIDs::MsgId_requestPowerOff:
            if ( true )
            {
                requestPowerOff(  );
            }
            break;

        case NEPowerManager::eMessageIDs::MsgId_requestStartTrafficLight:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                requestStartTrafficLight(  );
            }
            break;

        case NEPowerManager::eMessageIDs::MsgId_requestStopTrafficLight:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                requestStopTrafficLight(  );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_PowerManagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_processAttributeEvent);
void PowerManagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEPowerManager::eMessageIDs updId  = static_cast<NEPowerManager::eMessageIDs>(eventElem.getRequestId());
        const ProxyAddress & source = eventElem.getEventSource( );
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), source );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), source ) == false )
            {
                TRACE_SCOPE(generated_src_PowerManagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPowerManager::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataInternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEPowerManager::eMessageIDs::MsgId_LightsPowerState:
                if ( mLightsPowerStateState == NEService::eDataStateType::DataIsOK )
                    stream << mLightsPowerState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_PowerManagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEPowerManager::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEPowerManager::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PowerManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
