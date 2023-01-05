//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/TrafficControllerStub.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/TrafficControllerStub.hpp"
#include "generated/src/private/TrafficControllerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// TrafficControllerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
TrafficControllerStub::TrafficControllerStub( Component & masterComp )
    : StubBase    ( masterComp, NETrafficController::getInterfaceData() )

    , mTrafficSouthNorth      (  )
    , mTrafficSouthNorthState ( NEService::eDataStateType::DataIsUnavailable )
    
    , mTrafficEastWest        (  )
    , mTrafficEastWestState   ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void TrafficControllerStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_startupServiceInterface);
void TrafficControllerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_TrafficControllerStub_startupServiceInterface);

    TrafficControllerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    TrafficControllerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_shutdownServiceIntrface);
void TrafficControllerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_TrafficControllerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    TrafficControllerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    TrafficControllerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * TrafficControllerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW TrafficControllerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW TrafficControllerResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * TrafficControllerStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW TrafficControllerRequestEvent(stream) );
}

RemoteNotifyRequestEvent * TrafficControllerStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW TrafficControllerNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_sendNotification);
void TrafficControllerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NETrafficController::eMessageIDs>(msgId) )
    {
    case NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
        mTrafficSouthNorthState = NEService::eDataStateType::DataIsOK;
        stream << mTrafficSouthNorth;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
        mTrafficEastWestState = NEService::eDataStateType::DataIsOK;
        stream << mTrafficEastWest;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_TrafficControllerStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_errorRequest);
void TrafficControllerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NETrafficController::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
        mTrafficSouthNorthState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
        mTrafficEastWestState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth:
    case NETrafficController::eMessageIDs::MsgId_broadcastEastWest:
        result = NEService::eResultType::DataInvalid;
        break;

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
        TRACE_SCOPE(generated_src_TrafficControllerStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NETrafficController::getString( static_cast<NETrafficController::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NETrafficController::getString(static_cast<NETrafficController::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, TrafficControllerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void TrafficControllerStub::setTrafficSouthNorth( const NETrafficController::sTrafficLight & newValue )
{
    if ( (mTrafficSouthNorthState != NEService::eDataStateType::DataIsOK) || (mTrafficSouthNorth != newValue) )
    {
        mTrafficSouthNorth = newValue;
        sendNotification( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth) );
    }
}

void TrafficControllerStub::setTrafficEastWest( const NETrafficController::sTrafficLight & newValue )
{
    if ( (mTrafficEastWestState != NEService::eDataStateType::DataIsOK) || (mTrafficEastWest != newValue) )
    {
        mTrafficEastWest = newValue;
        sendNotification( static_cast<msg_id>(NETrafficController::eMessageIDs::MsgId_TrafficEastWest) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void TrafficControllerStub::broadcastSouthNorth( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
{
    static const NETrafficController::eMessageIDs msgId = NETrafficController::eMessageIDs::MsgId_broadcastSouthNorth;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << LightVehicle;
    stream << LightPedestrian;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void TrafficControllerStub::broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
{
    static const NETrafficController::eMessageIDs msgId = NETrafficController::eMessageIDs::MsgId_broadcastEastWest;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << LightVehicle;
    stream << LightPedestrian;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_processRequestEvent);
void TrafficControllerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    TrafficControllerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, TrafficControllerRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

    TRACE_SCOPE(generated_src_TrafficControllerStub_processRequestEvent);
    TRACE_ERR("The service TrafficController has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_processAttributeEvent);
void TrafficControllerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NETrafficController::eMessageIDs updId  = static_cast<NETrafficController::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_src_TrafficControllerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NETrafficController::getString(updId)
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
            case NETrafficController::eMessageIDs::MsgId_TrafficSouthNorth:
                if ( mTrafficSouthNorthState == NEService::eDataStateType::DataIsOK )
                    stream << mTrafficSouthNorth;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NETrafficController::eMessageIDs::MsgId_TrafficEastWest:
                if ( mTrafficEastWestState == NEService::eDataStateType::DataIsOK )
                    stream << mTrafficEastWest;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_TrafficControllerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NETrafficController::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NETrafficController::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/TrafficControllerStub.cpp file
//////////////////////////////////////////////////////////////////////////
