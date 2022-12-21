//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:32 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/SimpleTrafficLightStub.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/SimpleTrafficLightStub.hpp"
#include "generated/src/private/SimpleTrafficLightEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
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
    , mSouthNorthState    ( NEService::eDataStateType::DataIsUnavailable )
    
    , mEastWest           (  )
    , mEastWestState      ( NEService::eDataStateType::DataIsUnavailable )
    
{
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

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_startupServiceInterface);
void SimpleTrafficLightStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightStub_startupServiceInterface);

    SimpleTrafficLightRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    SimpleTrafficLightNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_shutdownServiceIntrface);
void SimpleTrafficLightStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_SimpleTrafficLightStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    SimpleTrafficLightRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    SimpleTrafficLightNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
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

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_sendNotification);
void SimpleTrafficLightStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NESimpleTrafficLight::eMessageIDs>(msgId) )
    {
    case NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
        mSouthNorthState = NEService::eDataStateType::DataIsOK;
        stream << mSouthNorth;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
        mEastWestState = NEService::eDataStateType::DataIsOK;
        stream << mEastWest;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_SimpleTrafficLightStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_errorRequest);
void SimpleTrafficLightStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NESimpleTrafficLight::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
        mSouthNorthState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
        mEastWestState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NESimpleTrafficLight::eMessageIDs::MsgId_broadcastLightChanged:
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
        TRACE_SCOPE(generated_src_SimpleTrafficLightStub_errorRequest);
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
    if ( (mSouthNorthState != NEService::eDataStateType::DataIsOK) || (mSouthNorth != newValue) )
    {
        mSouthNorth = newValue;
        sendNotification( static_cast<msg_id>(NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth) );
    }
}

void SimpleTrafficLightStub::setEastWest( const NESimpleTrafficLight::eTrafficLight & newValue )
{
    if ( (mEastWestState != NEService::eDataStateType::DataIsOK) || (mEastWest != newValue) )
    {
        mEastWest = newValue;
        sendNotification( static_cast<msg_id>(NESimpleTrafficLight::eMessageIDs::MsgId_EastWest) );
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
    static const NESimpleTrafficLight::eMessageIDs msgId = NESimpleTrafficLight::eMessageIDs::MsgId_broadcastLightChanged;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << SouthNorth;
    stream << EastWest;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_processRequestEvent);
void SimpleTrafficLightStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    SimpleTrafficLightRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SimpleTrafficLightRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

    TRACE_SCOPE(generated_src_SimpleTrafficLightStub_processRequestEvent);
    TRACE_ERR("The service SimpleTrafficLight has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficLightStub_processAttributeEvent);
void SimpleTrafficLightStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NESimpleTrafficLight::eMessageIDs updId  = static_cast<NESimpleTrafficLight::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_SimpleTrafficLightStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESimpleTrafficLight::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataExternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NESimpleTrafficLight::eMessageIDs::MsgId_SouthNorth:
                if ( mSouthNorthState == NEService::eDataStateType::DataIsOK )
                    stream << mSouthNorth;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NESimpleTrafficLight::eMessageIDs::MsgId_EastWest:
                if ( mEastWestState == NEService::eDataStateType::DataIsOK )
                    stream << mEastWest;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_SimpleTrafficLightStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NESimpleTrafficLight::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
