//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     18.12.2022  15:17:30 GMT+01:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/LocalHelloWorldStub.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/LocalHelloWorldStub.hpp"
#include "generated/src/private/LocalHelloWorldEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LocalHelloWorldStub::LocalHelloWorldStub( Component & masterComp )
    : StubBase    ( masterComp, NELocalHelloWorld::getInterfaceData() )

{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void LocalHelloWorldStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_startupServiceInterface);
void LocalHelloWorldStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldStub_startupServiceInterface);

    LocalHelloWorldRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    LocalHelloWorldNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_shutdownServiceIntrface);
void LocalHelloWorldStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    LocalHelloWorldRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    LocalHelloWorldNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * LocalHelloWorldStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW LocalHelloWorldResponseEvent(data, proxy, result, msgId) : DEBUG_NEW LocalHelloWorldResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_sendNotification);
void LocalHelloWorldStub::sendNotification( unsigned int msgId )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldStub_sendNotification);
    TRACE_ERR("The Service Interface has no attribute. Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
    ASSERT(false);
}


DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_errorRequest);
void LocalHelloWorldStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
/************************************************************************
 * Response errors
 ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
        listenerId = static_cast<msg_id>(NELocalHelloWorld::getResponseId(static_cast< NELocalHelloWorld::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_LocalHelloWorldStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NELocalHelloWorld::getString( static_cast<NELocalHelloWorld::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NELocalHelloWorld::getString(static_cast<NELocalHelloWorld::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, LocalHelloWorldResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

/************************************************************************
 * Send responses
 ************************************************************************/

void LocalHelloWorldStub::responseHelloWorld( const NELocalHelloWorld::sConnectedClient & clientInfo )
{
    static const NELocalHelloWorld::eMessageIDs msgId = NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientInfo;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_processRequestEvent);
void LocalHelloWorldStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    LocalHelloWorldRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, LocalHelloWorldRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const LocalHelloWorldRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        switch ( static_cast<NELocalHelloWorld::eMessageIDs>(reqId) )
        {
        case NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld:
            if ( true )
            {
                String  roleName;
                stream >> roleName;
                requestHelloWorld( roleName );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
void LocalHelloWorldStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NELocalHelloWorld::eMessageIDs updId  = static_cast<NELocalHelloWorld::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NELocalHelloWorld::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
#ifdef  _DEBUG
            if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
            {
                TRACE_SCOPE(generated_src_LocalHelloWorldStub_processAttributeEvent);
                TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                ASSERT(false);
            }
#endif // _DEBUG
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldStub.cpp file
//////////////////////////////////////////////////////////////////////////
