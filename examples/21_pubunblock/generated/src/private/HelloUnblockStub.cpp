//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloUnblockStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/HelloUnblockStub.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/HelloUnblockStub.hpp"
#include "generated/src/private/HelloUnblockEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// HelloUnblockStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
HelloUnblockStub::HelloUnblockStub( Component & masterComp )
    : StubBase    ( masterComp, NEHelloUnblock::getInterfaceData() )

    , mHelloServiceState      (  )
    , mHelloServiceStateState ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void HelloUnblockStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_startupServiceInterface);
void HelloUnblockStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloUnblockStub_startupServiceInterface);

    HelloUnblockRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloUnblockNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_shutdownServiceIntrface);
void HelloUnblockStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_HelloUnblockStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    HelloUnblockRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    HelloUnblockNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * HelloUnblockStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW HelloUnblockResponseEvent(data, proxy, result, msgId) : DEBUG_NEW HelloUnblockResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * HelloUnblockStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW HelloUnblockRequestEvent(stream) );
}

RemoteNotifyRequestEvent * HelloUnblockStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW HelloUnblockNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_sendNotification);
void HelloUnblockStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEHelloUnblock::eMessageIDs>(msgId) )
    {
    case NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
        mHelloServiceStateState = NEService::eDataStateType::DataIsOK;
        stream << mHelloServiceState;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_HelloUnblockStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_errorRequest);
void HelloUnblockStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEHelloUnblock::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
        mHelloServiceStateState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier:
    case NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier:
    case NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock:
        listenerId = static_cast<msg_id>(NEHelloUnblock::getResponseId(static_cast< NEHelloUnblock::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_HelloUnblockStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEHelloUnblock::getString( static_cast<NEHelloUnblock::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEHelloUnblock::getString(static_cast<NEHelloUnblock::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, HelloUnblockResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void HelloUnblockStub::setHelloServiceState( const NEHelloUnblock::eServiceState & newValue )
{
    if ( (mHelloServiceStateState != NEService::eDataStateType::DataIsOK) || (mHelloServiceState != newValue) )
    {
        mHelloServiceState = newValue;
        sendNotification( static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void HelloUnblockStub::responseIdentifier( unsigned int clientId )
{
    static const NEHelloUnblock::eMessageIDs msgId = NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientId;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void HelloUnblockStub::responseHelloUnblock( unsigned int clientId, unsigned int seqNr )
{
    static const NEHelloUnblock::eMessageIDs msgId = NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientId;
    stream << seqNr;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_processRequestEvent);
void HelloUnblockStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    HelloUnblockRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, HelloUnblockRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const HelloUnblockRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEHelloUnblock::eMessageIDs respId = NEHelloUnblock::getResponseId(static_cast<NEHelloUnblock::eMessageIDs>(reqId));

        switch ( static_cast<NEHelloUnblock::eMessageIDs>(reqId) )
        {
        case NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                requestIdentifier(  );
            }
            break;

        case NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                unsigned int    clientId;
                unsigned int    seqNr;
                stream >> clientId;
                stream >> seqNr;
                requestHelloUblock( clientId, seqNr );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_HelloUnblockStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockStub_processAttributeEvent);
void HelloUnblockStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEHelloUnblock::eMessageIDs updId  = static_cast<NEHelloUnblock::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_src_HelloUnblockStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEHelloUnblock::getString(updId)
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
            case NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
                if ( mHelloServiceStateState == NEService::eDataStateType::DataIsOK )
                    stream << mHelloServiceState;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_HelloUnblockStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEHelloUnblock::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEHelloUnblock::eMessageIDs::MsgId_NotProcessed)
            {
                sendUpdateNotificationOnce( source, static_cast<msg_id>(updId), args, validUpdate );
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloUnblockStub.cpp file
//////////////////////////////////////////////////////////////////////////
