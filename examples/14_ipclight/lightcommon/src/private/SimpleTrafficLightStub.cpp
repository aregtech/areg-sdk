//////////////////////////////////////////////////////////////////////////
// Begin generate lightcommon/src/private/SimpleTrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     03.08.2021  11:01:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            lightcommon/src/private/SimpleTrafficLightStub.cpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "lightcommon/src/SimpleTrafficLightStub.hpp"
#include "lightcommon/src/private/SimpleTrafficLightEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SimpleTrafficLightStub::SimpleTrafficLightStub( Component & masterComp )
    : StubBase    ( masterComp, NESimpleTrafficLight::getInterfaceData() )
    
    , mSouthNorth         (  )
    , mSouthNorthState    ( NEService::DATA_UNAVAILABLE )
    
    , mEastWest           (  )
    , mEastWestState      ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

SimpleTrafficLightStub::~SimpleTrafficLightStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void SimpleTrafficLightStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_startupServiceInterface);
void SimpleTrafficLightStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_startupServiceInterface);
    
    SimpleTrafficLightRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SimpleTrafficLightNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_shutdownServiceIntrface);
void SimpleTrafficLightStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    SimpleTrafficLightRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SimpleTrafficLightNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * SimpleTrafficLightStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW SimpleTrafficLightResponseEvent(data, proxy, result, msgId) : DEBUG_NEW SimpleTrafficLightResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * SimpleTrafficLightStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW SimpleTrafficLightRequestEvent(stream) );
}

RemoteNotifyRequestEvent * SimpleTrafficLightStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW SimpleTrafficLightNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_sendNotification);
void SimpleTrafficLightStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NESimpleTrafficLight::eMessageIDs>(msgId) )
    {
    case NESimpleTrafficLight::MSG_ID_SouthNorth:
        mSouthNorthState = NEService::DATA_OK;
        stream << mSouthNorth;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NESimpleTrafficLight::MSG_ID_EastWest:
        mEastWestState = NEService::DATA_OK;
        stream << mEastWest;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_errorRequest);
void SimpleTrafficLightStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NESimpleTrafficLight::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESimpleTrafficLight::MSG_ID_SouthNorth:
        mSouthNorthState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NESimpleTrafficLight::MSG_ID_EastWest:
        mEastWestState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NESimpleTrafficLight::MSG_ID_broadcastLightChanged:
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
        TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NESimpleTrafficLight::getString( static_cast<NESimpleTrafficLight::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NESimpleTrafficLight::getString(static_cast<NESimpleTrafficLight::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, SimpleTrafficLightResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void SimpleTrafficLightStub::setSouthNorth( const NESimpleTrafficLight::eTrafficLight & newValue )
{
    if ( (mSouthNorthState != NEService::DATA_OK) || (mSouthNorth != newValue) )
    {
        mSouthNorth = newValue;
        sendNotification( NESimpleTrafficLight::MSG_ID_SouthNorth );
    }
}

void SimpleTrafficLightStub::setEastWest( const NESimpleTrafficLight::eTrafficLight & newValue )
{
    if ( (mEastWestState != NEService::DATA_OK) || (mEastWest != newValue) )
    {
        mEastWest = newValue;
        sendNotification( NESimpleTrafficLight::MSG_ID_EastWest );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void SimpleTrafficLightStub::broadcastLightChanged( NESimpleTrafficLight::eTrafficLight SouthNorth, NESimpleTrafficLight::eTrafficLight EastWest )
{
    static const NESimpleTrafficLight::eMessageIDs msgId = NESimpleTrafficLight::MSG_ID_broadcastLightChanged;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << SouthNorth;
    stream << EastWest;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_processRequestEvent);
void SimpleTrafficLightStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    SimpleTrafficLightRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SimpleTrafficLightRequestEvent);

    if ( (reqEvent != static_cast<SimpleTrafficLightRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

    TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_processRequestEvent);
    TRACE_ERR("The service SimpleTrafficLight has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_processAttributeEvent);
void SimpleTrafficLightStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NESimpleTrafficLight::eMessageIDs updId  = static_cast<NESimpleTrafficLight::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESimpleTrafficLight::getString(updId)
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
            case NESimpleTrafficLight::MSG_ID_SouthNorth:
                if ( mSouthNorthState == NEService::DATA_OK )
                    stream << mSouthNorth;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NESimpleTrafficLight::MSG_ID_EastWest:
                if ( mEastWestState == NEService::DATA_OK )
                    stream << mEastWest;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(lightcommon_src_SimpleTrafficLightStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NESimpleTrafficLight::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NESimpleTrafficLight::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate lightcommon/src/private/SimpleTrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
