//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWatchdogStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:31 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/HelloWatchdogStub.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/HelloWatchdogStub.hpp"
#include "generated/src/private/HelloWatchdogEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloWatchdogStub::HelloWatchdogStub( Component & masterComp )
    : StubBase    ( masterComp, NEHelloWatchdog::getInterfaceData() )

    , mServiceState       (  )
    , mServiceStateState  ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void HelloWatchdogStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_startupServiceInterface);
void HelloWatchdogStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWatchdogStub_startupServiceInterface);

    HelloWatchdogRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloWatchdogNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_shutdownServiceIntrface);
void HelloWatchdogStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloWatchdogStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    HelloWatchdogRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloWatchdogNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * HelloWatchdogStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW HelloWatchdogResponseEvent(data, proxy, result, msgId) : DEBUG_NEW HelloWatchdogResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * HelloWatchdogStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW HelloWatchdogRequestEvent(stream) );
}

RemoteNotifyRequestEvent * HelloWatchdogStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW HelloWatchdogNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_sendNotification);
void HelloWatchdogStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEHelloWatchdog::eMessageIDs>(msgId) )
    {
    case NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
        mServiceStateState = NEService::eDataStateType::DataIsOK;
        stream << mServiceState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_HelloWatchdogStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_errorRequest);
void HelloWatchdogStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEHelloWatchdog::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
        mServiceStateState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep:
    case NEHelloWatchdog::eMessageIDs::MsgId_requestStopService:
    case NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService:
        listenerId = static_cast<msg_id>(NEHelloWatchdog::getResponseId(static_cast< NEHelloWatchdog::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_HelloWatchdogStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEHelloWatchdog::getString( static_cast<NEHelloWatchdog::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEHelloWatchdog::getString(static_cast<NEHelloWatchdog::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, HelloWatchdogResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void HelloWatchdogStub::setServiceState( const NEHelloWatchdog::eState & newValue )
{
    if ( (mServiceStateState != NEService::eDataStateType::DataIsOK) || (mServiceState != newValue) )
    {
        mServiceState = newValue;
        sendNotification( static_cast<msg_id>(NEHelloWatchdog::eMessageIDs::MsgId_ServiceState) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloWatchdogStub::responseStartSleep( unsigned int timeoutSleep )
{
    static const NEHelloWatchdog::eMessageIDs msgId = NEHelloWatchdog::eMessageIDs::MsgId_responseStartSleep;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << timeoutSleep;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_processRequestEvent);
void HelloWatchdogStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    HelloWatchdogRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, HelloWatchdogRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const HelloWatchdogRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEHelloWatchdog::eMessageIDs respId = NEHelloWatchdog::getResponseId(static_cast<NEHelloWatchdog::eMessageIDs>(reqId));

        switch ( static_cast<NEHelloWatchdog::eMessageIDs>(reqId) )
        {
        case NEHelloWatchdog::eMessageIDs::MsgId_requestStartSleep:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                unsigned int    timeoutSleep;
                stream >> timeoutSleep;
                requestStartSleep( timeoutSleep );
            }
            break;

        case NEHelloWatchdog::eMessageIDs::MsgId_requestStopService:
            if ( true )
            {
                requestStopService(  );
            }
            break;

        case NEHelloWatchdog::eMessageIDs::MsgId_requestShutdownService:
            if ( true )
            {
                requestShutdownService(  );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_HelloWatchdogStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_HelloWatchdogStub_processAttributeEvent);
void HelloWatchdogStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEHelloWatchdog::eMessageIDs updId  = static_cast<NEHelloWatchdog::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_src_HelloWatchdogStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloWatchdog::getString(updId)
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
            case NEHelloWatchdog::eMessageIDs::MsgId_ServiceState:
                if ( mServiceStateState == NEService::eDataStateType::DataIsOK )
                    stream << mServiceState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_HelloWatchdogStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEHelloWatchdog::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWatchdogStub.cpp file
//////////////////////////////////////////////////////////////////////////
