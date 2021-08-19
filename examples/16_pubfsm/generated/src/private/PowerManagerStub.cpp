//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PowerManagerStub.cpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/PowerManagerStub.hpp"
#include "generated/src/private/PowerManagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
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
    , mLightsPowerStateState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

PowerManagerStub::~PowerManagerStub( void )
{
    ; // do nothing
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
    
    PowerManagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    PowerManagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_shutdownServiceIntrface);
void PowerManagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_PowerManagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    PowerManagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    PowerManagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * PowerManagerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW PowerManagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW PowerManagerResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerStub_sendNotification);
void PowerManagerStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEPowerManager::eMessageIDs>(msgId) )
    {
    case NEPowerManager::MSG_ID_LightsPowerState:
        mLightsPowerStateState = NEService::DATA_OK;
        stream << mLightsPowerState;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
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
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEPowerManager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEPowerManager::MSG_ID_LightsPowerState:
        mLightsPowerStateState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEPowerManager::MSG_ID_responseStartTrafficLight:
    case NEPowerManager::MSG_ID_responseStopTrafficLight:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEPowerManager::MSG_ID_requestPowerOn:
    case NEPowerManager::MSG_ID_requestPowerOff:
    case NEPowerManager::MSG_ID_requestStartTrafficLight:
    case NEPowerManager::MSG_ID_requestStopTrafficLight:
        listenerId = NEPowerManager::getResponseId(static_cast< NEPowerManager::eMessageIDs>(msgId));
        result = msgCancel ? NEService::RESULT_REQUEST_CANCELED : NEService::RESULT_REQUEST_ERROR;
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
    if ( (mLightsPowerStateState != NEService::DATA_OK) || (mLightsPowerState != newValue) )
    {
        mLightsPowerState = newValue;
        sendNotification( NEPowerManager::MSG_ID_LightsPowerState );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void PowerManagerStub::responseStartTrafficLight( bool Success )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_responseStartTrafficLight;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << Success;
    sendResponseEvent( msgId, args );
}

void PowerManagerStub::responseStopTrafficLight( bool Success )
{
    static const NEPowerManager::eMessageIDs msgId = NEPowerManager::MSG_ID_responseStopTrafficLight;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << Success;
    sendResponseEvent( msgId, args );
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

    if ( (reqEvent != static_cast<PowerManagerRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEPowerManager::eMessageIDs respId = NEPowerManager::getResponseId(static_cast<NEPowerManager::eMessageIDs>(reqId));

        switch ( static_cast<NEPowerManager::eMessageIDs>(reqId) )
        {
        case NEPowerManager::MSG_ID_requestPowerOn:
            if ( true )
            {
                requestPowerOn(  );
            }
            break;
            
        case NEPowerManager::MSG_ID_requestPowerOff:
            if ( true )
            {
                requestPowerOff(  );
            }
            break;
            
        case NEPowerManager::MSG_ID_requestStartTrafficLight:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                requestStartTrafficLight(  );
            }
            break;
            
        case NEPowerManager::MSG_ID_requestStopTrafficLight:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
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
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEPowerManager::eMessageIDs updId  = static_cast<NEPowerManager::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_PowerManagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEPowerManager::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataInternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEPowerManager::MSG_ID_LightsPowerState:
                if ( mLightsPowerStateState == NEService::DATA_OK )
                    stream << mLightsPowerState;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_PowerManagerStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEPowerManager::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEPowerManager::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PowerManagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
