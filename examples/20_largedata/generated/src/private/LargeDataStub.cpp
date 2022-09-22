//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LargeDataStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:51 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/LargeDataStub.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/LargeDataStub.hpp"
#include "generated/src/private/LargeDataEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// LargeDataStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LargeDataStub::LargeDataStub( Component & masterComp )
    : StubBase    ( masterComp, NELargeData::getInterfaceData() )

{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void LargeDataStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_LargeDataStub_startupServiceInterface);
void LargeDataStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_LargeDataStub_startupServiceInterface);

    LargeDataRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    LargeDataNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_LargeDataStub_shutdownServiceIntrface);
void LargeDataStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_LargeDataStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    LargeDataRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    LargeDataNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * LargeDataStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW LargeDataResponseEvent(data, proxy, result, msgId) : DEBUG_NEW LargeDataResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * LargeDataStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW LargeDataRequestEvent(stream) );
}

RemoteNotifyRequestEvent * LargeDataStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW LargeDataNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_LargeDataStub_sendNotification);
void LargeDataStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_src_LargeDataStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_src_LargeDataStub_errorRequest);
void LargeDataStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NELargeData::eMessageIDs>(msgId) )
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
    case NELargeData::eMessageIDs::MsgId_broadcastWriteImageBlock:
    case NELargeData::eMessageIDs::MsgId_broadcastServiceStopping:
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
        TRACE_SCOPE(generated_src_LargeDataStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NELargeData::getString( static_cast<NELargeData::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NELargeData::getString(static_cast<NELargeData::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, LargeDataResponseEvent(listeners[0].mProxy, result, msgId));
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

void LargeDataStub::broadcastWriteImageBlock( void )
{
    static const NELargeData::eMessageIDs msgId = NELargeData::eMessageIDs::MsgId_broadcastWriteImageBlock;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
}

void LargeDataStub::broadcastServiceStopping( void )
{
    static const NELargeData::eMessageIDs msgId = NELargeData::eMessageIDs::MsgId_broadcastServiceStopping;
    sendResponseEvent( static_cast<msg_id>(msgId), EventDataStream::EmptyData );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LargeDataStub_processRequestEvent);
void LargeDataStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    LargeDataRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, LargeDataRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

    TRACE_SCOPE(generated_src_LargeDataStub_processRequestEvent);
    TRACE_ERR("The service LargeData has no request. Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_LargeDataStub_processAttributeEvent);
void LargeDataStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NELargeData::eMessageIDs updId  = static_cast<NELargeData::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_LargeDataStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NELargeData::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_src_LargeDataStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LargeDataStub.cpp file
//////////////////////////////////////////////////////////////////////////
