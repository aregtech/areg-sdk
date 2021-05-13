//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/TrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/TrafficLightStub.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/TrafficLightStub.hpp"
#include "src/gen/private/TrafficLightEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// TrafficLightStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
TrafficLightStub::TrafficLightStub( Component & masterComp )
    : StubBase    ( masterComp, NETrafficLight::createInterfaceData() )
    
    , mTrafficLight       (  )
    , mTrafficLightState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

TrafficLightStub::~TrafficLightStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void TrafficLightStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_startupServiceInterface);
void TrafficLightStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(src_gen_TrafficLightStub_startupServiceInterface);
    
    TrafficLightRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    TrafficLightNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_shutdownServiceIntrface);
void TrafficLightStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(src_gen_TrafficLightStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    TrafficLightRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    TrafficLightNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int TrafficLightStub::getNumberOfRequests( void ) const
{
    return NETrafficLight::NumberofRequests;
}

unsigned int TrafficLightStub::getNumberOfResponses( void ) const
{
    return NETrafficLight::NumberofResponses;
}

unsigned int TrafficLightStub::getNumberOfAttributes( void ) const
{
    return NETrafficLight::NumberofAttributes;
}

const unsigned int * TrafficLightStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::RequestIds);
}

const unsigned int * TrafficLightStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::ResponseIds);
}

const unsigned int * TrafficLightStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NETrafficLight::AttributeIds);
}

ResponseEvent * TrafficLightStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW TrafficLightResponseEvent(data, proxy, result, msgId) : DEBUG_NEW TrafficLightResponseEvent(proxy, result, msgId));
}

const Version & TrafficLightStub::getImplVersion( void ) const
{
    return NETrafficLight::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_sendNotification);
void TrafficLightStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NETrafficLight::eMessageIDs>(msgId) )
    {
    case NETrafficLight::MSG_ID_TrafficLight:
        mTrafficLightState = NEService::DATA_OK;
        stream << mTrafficLight;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_TrafficLightStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_errorRequest);
void TrafficLightStub::errorRequest( unsigned int msgId, bool msgCancel )
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
    case NETrafficLight::MSG_ID_responseChangeLight:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NETrafficLight::MSG_ID_requestChangeLight:
        listenerId = NETrafficLight::getResponseId(static_cast< NETrafficLight::eMessageIDs>(msgId));
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
        TRACE_SCOPE(src_gen_TrafficLightStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NETrafficLight::getString( static_cast<NETrafficLight::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NETrafficLight::getString(static_cast<NETrafficLight::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, TrafficLightResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void TrafficLightStub::setTrafficLight( const NETrafficLight::eLight & newValue )
{
    if ( (mTrafficLightState != NEService::DATA_OK) || (mTrafficLight != newValue) )
    {
        mTrafficLight = newValue;
        sendNotification( NETrafficLight::MSG_ID_TrafficLight );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void TrafficLightStub::responseChangeLight( NETrafficLight::eLight lightColor )
{
    static const NETrafficLight::eMessageIDs msgId = NETrafficLight::MSG_ID_responseChangeLight;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << lightColor;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_processRequestEvent);
void TrafficLightStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    TrafficLightRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, TrafficLightRequestEvent);

    if ( (reqEvent != static_cast<TrafficLightRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const TrafficLightRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NETrafficLight::eMessageIDs respId = NETrafficLight::getResponseId(static_cast<NETrafficLight::eMessageIDs>(reqId));

        switch ( static_cast<NETrafficLight::eMessageIDs>(reqId) )
        {
        case NETrafficLight::MSG_ID_requestChangeLight:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                NETrafficLight::eLight  lightColor;
                bool                    holdon;
                stream >> lightColor;                
                stream >> holdon;                
                requestChangeLight( lightColor, holdon );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_TrafficLightStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_TrafficLightStub_processAttributeEvent);
void TrafficLightStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NETrafficLight::eMessageIDs updId  = static_cast<NETrafficLight::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_TrafficLightStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NETrafficLight::getString(updId)
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
            case NETrafficLight::MSG_ID_TrafficLight:
                if ( mTrafficLightState == NEService::DATA_OK )
                    stream << mTrafficLight;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_TrafficLightStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NETrafficLight::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NETrafficLight::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/TrafficLightStub.cpp file
//////////////////////////////////////////////////////////////////////////
