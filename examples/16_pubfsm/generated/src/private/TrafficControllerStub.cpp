//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficController.
 * Generated at     15.08.2021  00:03:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/TrafficControllerStub.cpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/TrafficControllerStub.hpp"
#include "generated/src/private/TrafficControllerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
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
    , mTrafficSouthNorthState ( NEService::DATA_UNAVAILABLE )
    
    , mTrafficEastWest        (  )
    , mTrafficEastWestState   ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

TrafficControllerStub::~TrafficControllerStub( void )
{
    ; // do nothing
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
    
    TrafficControllerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    TrafficControllerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_shutdownServiceIntrface);
void TrafficControllerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_TrafficControllerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    TrafficControllerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    TrafficControllerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
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
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NETrafficController::eMessageIDs>(msgId) )
    {
    case NETrafficController::MSG_ID_TrafficSouthNorth:
        mTrafficSouthNorthState = NEService::DATA_OK;
        stream << mTrafficSouthNorth;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NETrafficController::MSG_ID_TrafficEastWest:
        mTrafficEastWestState = NEService::DATA_OK;
        stream << mTrafficEastWest;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
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
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NETrafficController::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NETrafficController::MSG_ID_TrafficSouthNorth:
        mTrafficSouthNorthState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NETrafficController::MSG_ID_TrafficEastWest:
        mTrafficEastWestState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NETrafficController::MSG_ID_broadcastSouthNorth:
    case NETrafficController::MSG_ID_broadcastEastWest:
        result = NEService::RESULT_INVALID;
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
    if ( (mTrafficSouthNorthState != NEService::DATA_OK) || (mTrafficSouthNorth != newValue) )
    {
        mTrafficSouthNorth = newValue;
        sendNotification( NETrafficController::MSG_ID_TrafficSouthNorth );
    }
}

void TrafficControllerStub::setTrafficEastWest( const NETrafficController::sTrafficLight & newValue )
{
    if ( (mTrafficEastWestState != NEService::DATA_OK) || (mTrafficEastWest != newValue) )
    {
        mTrafficEastWest = newValue;
        sendNotification( NETrafficController::MSG_ID_TrafficEastWest );
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
    static const NETrafficController::eMessageIDs msgId = NETrafficController::MSG_ID_broadcastSouthNorth;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << LightVehicle;
    stream << LightPedestrian;
    sendResponseEvent( msgId, args );
}

void TrafficControllerStub::broadcastEastWest( NETrafficController::eVehicleTrafficLight LightVehicle, NETrafficController::ePedestrianTrafficLight LightPedestrian )
{
    static const NETrafficController::eMessageIDs msgId = NETrafficController::MSG_ID_broadcastEastWest;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << LightVehicle;
    stream << LightPedestrian;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_TrafficControllerStub_processRequestEvent);
void TrafficControllerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    TrafficControllerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, TrafficControllerRequestEvent);

    if ( (reqEvent != static_cast<TrafficControllerRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
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
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NETrafficController::eMessageIDs updId  = static_cast<NETrafficController::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_TrafficControllerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NETrafficController::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataExternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NETrafficController::MSG_ID_TrafficSouthNorth:
                if ( mTrafficSouthNorthState == NEService::DATA_OK )
                    stream << mTrafficSouthNorth;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NETrafficController::MSG_ID_TrafficEastWest:
                if ( mTrafficEastWestState == NEService::DATA_OK )
                    stream << mTrafficEastWest;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_TrafficControllerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NETrafficController::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NETrafficController::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/TrafficControllerStub.cpp file
//////////////////////////////////////////////////////////////////////////
