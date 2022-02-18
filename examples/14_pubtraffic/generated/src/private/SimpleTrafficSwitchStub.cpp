//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:13 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/SimpleTrafficSwitchStub.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/SimpleTrafficSwitchStub.hpp"
#include "generated/src/private/SimpleTrafficSwitchEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SimpleTrafficSwitchStub::SimpleTrafficSwitchStub( Component & masterComp )
    : StubBase    ( masterComp, NESimpleTrafficSwitch::getInterfaceData() )
    
    , mIsSwitchedOn       (  )
    , mIsSwitchedOnState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void SimpleTrafficSwitchStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_startupServiceInterface);
void SimpleTrafficSwitchStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_startupServiceInterface);
    
    SimpleTrafficSwitchRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    SimpleTrafficSwitchNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_shutdownServiceIntrface);
void SimpleTrafficSwitchStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    SimpleTrafficSwitchRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    SimpleTrafficSwitchNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * SimpleTrafficSwitchStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW SimpleTrafficSwitchResponseEvent(data, proxy, result, msgId) : DEBUG_NEW SimpleTrafficSwitchResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_sendNotification);
void SimpleTrafficSwitchStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NESimpleTrafficSwitch::eMessageIDs>(msgId) )
    {
    case NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn:
        mIsSwitchedOnState = NEService::eDataStateType::DataIsOK;
        stream << mIsSwitchedOn;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_errorRequest);
void SimpleTrafficSwitchStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;
    
    switch ( static_cast<NESimpleTrafficSwitch::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn:
        mIsSwitchedOnState = NEService::eDataStateType::DataIsInvalid;
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
    case NESimpleTrafficSwitch::eMessageIDs::MsgId_requestSwitchLight:
        listenerId = static_cast<msg_id>(NESimpleTrafficSwitch::getResponseId(static_cast< NESimpleTrafficSwitch::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NESimpleTrafficSwitch::getString( static_cast<NESimpleTrafficSwitch::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NESimpleTrafficSwitch::getString(static_cast<NESimpleTrafficSwitch::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, SimpleTrafficSwitchResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void SimpleTrafficSwitchStub::setIsSwitchedOn( const bool & newValue )
{
    if ( (mIsSwitchedOnState != NEService::eDataStateType::DataIsOK) || (mIsSwitchedOn != newValue) )
    {
        mIsSwitchedOn = newValue;
        sendNotification( static_cast<msg_id>(NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn) );
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

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_processRequestEvent);
void SimpleTrafficSwitchStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    SimpleTrafficSwitchRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SimpleTrafficSwitchRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const SimpleTrafficSwitchRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        switch ( static_cast<NESimpleTrafficSwitch::eMessageIDs>(reqId) )
        {
        case NESimpleTrafficSwitch::eMessageIDs::MsgId_requestSwitchLight:
            if ( true )
            {
                bool    switchOn;
                stream >> switchOn;                
                requestSwitchLight( switchOn );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_processAttributeEvent);
void SimpleTrafficSwitchStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NESimpleTrafficSwitch::eMessageIDs updId  = static_cast<NESimpleTrafficSwitch::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESimpleTrafficSwitch::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataInternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn:
                if ( mIsSwitchedOnState == NEService::eDataStateType::DataIsOK )
                    stream << mIsSwitchedOn;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_SimpleTrafficSwitchStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NESimpleTrafficSwitch::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NESimpleTrafficSwitch::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficSwitchStub.cpp file
//////////////////////////////////////////////////////////////////////////
