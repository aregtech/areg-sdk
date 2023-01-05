//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:08:52 GMT+01:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/private/LocalHelloWorldProxy.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/LocalHelloWorldProxy.hpp"
#include "generated/src/private/LocalHelloWorldEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent::LocalHelloWorldServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * LocalHelloWorldProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW LocalHelloWorldProxy(roleName, ownerThread);
}

LocalHelloWorldProxy * LocalHelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<LocalHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELocalHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &LocalHelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

LocalHelloWorldProxy * LocalHelloWorldProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<LocalHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELocalHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &LocalHelloWorldProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LocalHelloWorldProxy::LocalHelloWorldProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NELocalHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclientInfo    (  )
{
}

LocalHelloWorldProxy::~LocalHelloWorldProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * LocalHelloWorldProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW LocalHelloWorldNotificationEvent(data);
}

ServiceRequestEvent * LocalHelloWorldProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW LocalHelloWorldRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* LocalHelloWorldProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW LocalHelloWorldNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * LocalHelloWorldProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent(consumer) );
}

void LocalHelloWorldProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    LocalHelloWorldResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void LocalHelloWorldProxy::unregisterServiceListeners( void )
{
    LocalHelloWorldResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

void LocalHelloWorldProxy::requestHelloWorld( const String & roleName )
{
    static const NELocalHelloWorld::eMessageIDs msgId = NELocalHelloWorld::eMessageIDs::MsgId_requestHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << roleName;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void LocalHelloWorldProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    LocalHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LocalHelloWorldResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void LocalHelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    LocalHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LocalHelloWorldResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_LocalHelloWorldProxy_updateData);
void LocalHelloWorldProxy::updateData( LocalHelloWorldResponseEvent & eventElem, NELocalHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const LocalHelloWorldResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NELocalHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
        stream >> mParamclientInfo;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_src_private_LocalHelloWorldProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void LocalHelloWorldProxy::processResponse( LocalHelloWorldResponseEvent & evenElem )
{
    NELocalHelloWorld::eMessageIDs respId  = static_cast<NELocalHelloWorld::eMessageIDs>(evenElem.getResponseId());
    NEService::eResultType resultType  = evenElem.getResult();

    bool dataValid  = false;
    bool setStates  = true;

    switch (resultType)
    {
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NELocalHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NELocalHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NELocalHelloWorld::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::RequestOK:     // fall through
    case NEService::eResultType::DataOK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
    {
        updateData(evenElem, respId);
    }

    if (setStates == true)
    {
        setState(static_cast<msg_id>(respId), dataValid ? NEService::eDataStateType::DataIsOK : NEService::eDataStateType::DataIsInvalid);
    }

    notifyListeners(static_cast<msg_id>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
