//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/CentralMessagerStub.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/CentralMessagerStub.hpp"
#include "generated/private/CentralMessagerEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CentralMessagerStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CentralMessagerStub::CentralMessagerStub( Component & masterComp )
    : StubBase    ( masterComp, NECentralMessager::getInterfaceData() )

{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void CentralMessagerStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_CentralMessagerStub_startupServiceInterface);
void CentralMessagerStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_CentralMessagerStub_startupServiceInterface);

    CentralMessagerRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    CentralMessagerNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_CentralMessagerStub_shutdownServiceIntrface);
void CentralMessagerStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_CentralMessagerStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    CentralMessagerRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    CentralMessagerNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * CentralMessagerStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW CentralMessagerResponseEvent(data, proxy, result, msgId) : DEBUG_NEW CentralMessagerResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * CentralMessagerStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW CentralMessagerRequestEvent(stream) );
}

RemoteNotifyRequestEvent * CentralMessagerStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW CentralMessagerNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_CentralMessagerStub_sendNotification);
void CentralMessagerStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_CentralMessagerStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_CentralMessagerStub_errorRequest);
void CentralMessagerStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NECentralMessager::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage:
    case NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping:
    case NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage:
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NECentralMessager::eMessageIDs::MsgId_requestSendMessage:
    case NECentralMessager::eMessageIDs::MsgId_requestKeyTyping:
        listenerId = static_cast<msg_id>(NECentralMessager::getResponseId(static_cast< NECentralMessager::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_CentralMessagerStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NECentralMessager::getString( static_cast<NECentralMessager::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NECentralMessager::getString(static_cast<NECentralMessager::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, CentralMessagerResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void CentralMessagerStub::broadcastSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    stream << dateTime;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void CentralMessagerStub::broadcastKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void CentralMessagerStub::broadcastBroadcastMessage( const String & serverMessage, const DateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << serverMessage;
    stream << dateTime;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_CentralMessagerStub_processRequestEvent);
void CentralMessagerStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    CentralMessagerRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, CentralMessagerRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const CentralMessagerRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        switch ( static_cast<NECentralMessager::eMessageIDs>(reqId) )
        {
        case NECentralMessager::eMessageIDs::MsgId_requestSendMessage:
            if ( true )
            {
                String          nickName;
                unsigned int    cookie;
                String          newMessage;
                DateTime        dateTime;
                stream >> nickName;
                stream >> cookie;
                stream >> newMessage;
                stream >> dateTime;
                requestSendMessage( nickName, cookie, newMessage, dateTime );
            }
            break;

        case NECentralMessager::eMessageIDs::MsgId_requestKeyTyping:
            if ( true )
            {
                String          nickName;
                unsigned int    cookie;
                String          newMessage;
                stream >> nickName;
                stream >> cookie;
                stream >> newMessage;
                requestKeyTyping( nickName, cookie, newMessage );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_CentralMessagerStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_CentralMessagerStub_processAttributeEvent);
void CentralMessagerStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NECentralMessager::eMessageIDs updId  = static_cast<NECentralMessager::eMessageIDs>(eventElem.getRequestId());
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
                TRACE_SCOPE(generated_CentralMessagerStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NECentralMessager::getString(updId)
                            , ProxyAddress::convAddressToPath(source).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), source );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_CentralMessagerStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/CentralMessagerStub.cpp file
//////////////////////////////////////////////////////////////////////////
