//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:15 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 *
 * \file            generated/src/SystemShutdownStub.hpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/SystemShutdownStub.hpp"
#include "generated/src/private/SystemShutdownEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SystemShutdownStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemShutdownStub::SystemShutdownStub( Component & masterComp )
    : StubBase    ( masterComp, NESystemShutdown::getInterfaceData() )
    
    , mServiceState       (  )
    , mServiceStateState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void SystemShutdownStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_startupServiceInterface);
void SystemShutdownStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_SystemShutdownStub_startupServiceInterface);
    
    SystemShutdownRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SystemShutdownNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_shutdownServiceIntrface);
void SystemShutdownStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_SystemShutdownStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    SystemShutdownRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SystemShutdownNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * SystemShutdownStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW SystemShutdownResponseEvent(data, proxy, result, msgId) : DEBUG_NEW SystemShutdownResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * SystemShutdownStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW SystemShutdownRequestEvent(stream) );
}

RemoteNotifyRequestEvent * SystemShutdownStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW SystemShutdownNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_sendNotification);
void SystemShutdownStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NESystemShutdown::eMessageIDs>(msgId) )
    {
    case NESystemShutdown::eMessageIDs::MsgId_ServiceState:
        mServiceStateState = NEService::eDataStateType::DataIsOK;
        stream << mServiceState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_SystemShutdownStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_errorRequest);
void SystemShutdownStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;
    
    switch ( static_cast<NESystemShutdown::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESystemShutdown::eMessageIDs::MsgId_ServiceState:
        mServiceStateState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NESystemShutdown::eMessageIDs::MsgId_broadcastServiceUnavailable:
    case NESystemShutdown::eMessageIDs::MsgId_broadcastServiceShutdown:
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
        TRACE_SCOPE(generated_src_SystemShutdownStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NESystemShutdown::getString( static_cast<NESystemShutdown::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NESystemShutdown::getString(static_cast<NESystemShutdown::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, SystemShutdownResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void SystemShutdownStub::setServiceState( const NESystemShutdown::eServiceState & newValue )
{
    if ( (mServiceStateState != NEService::eDataStateType::DataIsOK) || (mServiceState != newValue) )
    {
        mServiceState = newValue;
        sendNotification( static_cast<msg_id>(NESystemShutdown::eMessageIDs::MsgId_ServiceState) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void SystemShutdownStub::broadcastServiceUnavailable( void )
{
    static const NESystemShutdown::eMessageIDs msgId = NESystemShutdown::eMessageIDs::MsgId_broadcastServiceUnavailable;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
}

void SystemShutdownStub::broadcastServiceShutdown( void )
{
    static const NESystemShutdown::eMessageIDs msgId = NESystemShutdown::eMessageIDs::MsgId_broadcastServiceShutdown;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_processRequestEvent);
void SystemShutdownStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    SystemShutdownRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SystemShutdownRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

    TRACE_SCOPE(generated_src_SystemShutdownStub_processRequestEvent);
    TRACE_ERR("The service SystemShutdown has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_SystemShutdownStub_processAttributeEvent);
void SystemShutdownStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NESystemShutdown::eMessageIDs updId  = static_cast<NESystemShutdown::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_SystemShutdownStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESystemShutdown::getString(updId)
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
            case NESystemShutdown::eMessageIDs::MsgId_ServiceState:
                if ( mServiceStateState == NEService::eDataStateType::DataIsOK )
                    stream << mServiceState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_SystemShutdownStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NESystemShutdown::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NESystemShutdown::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SystemShutdownStub.cpp file
//////////////////////////////////////////////////////////////////////////
