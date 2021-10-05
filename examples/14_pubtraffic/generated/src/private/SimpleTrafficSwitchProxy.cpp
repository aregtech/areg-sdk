//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:13 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/private/SimpleTrafficSwitchProxy.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/SimpleTrafficSwitchProxy.hpp"
#include "generated/src/private/SimpleTrafficSwitchEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchProxy::SimpleTrafficSwitchServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(SimpleTrafficSwitchProxy::SimpleTrafficSwitchServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

SimpleTrafficSwitchProxy::SimpleTrafficSwitchServiceAvailableEvent::SimpleTrafficSwitchServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * SimpleTrafficSwitchProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW SimpleTrafficSwitchProxy(roleName, ownerThread);
}

SimpleTrafficSwitchProxy * SimpleTrafficSwitchProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<SimpleTrafficSwitchProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESimpleTrafficSwitch::getInterfaceData()
                                                                      , connectListener
                                                                      , &SimpleTrafficSwitchProxy::_createProxy
                                                                      , ownerThread) );
}

SimpleTrafficSwitchProxy * SimpleTrafficSwitchProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= nullptr*/ )
{
    return static_cast<SimpleTrafficSwitchProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NESimpleTrafficSwitch::getInterfaceData()
                                                                      , connectListener
                                                                      , &SimpleTrafficSwitchProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SimpleTrafficSwitchProxy::SimpleTrafficSwitchProxy( const char * roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NESimpleTrafficSwitch::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mIsSwitchedOn   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
}

SimpleTrafficSwitchProxy::~SimpleTrafficSwitchProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * SimpleTrafficSwitchProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW SimpleTrafficSwitchNotificationEvent(data);
}

ServiceRequestEvent * SimpleTrafficSwitchProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW SimpleTrafficSwitchRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* SimpleTrafficSwitchProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW SimpleTrafficSwitchNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * SimpleTrafficSwitchProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW SimpleTrafficSwitchProxy::SimpleTrafficSwitchServiceAvailableEvent(consumer) );
}

void SimpleTrafficSwitchProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    SimpleTrafficSwitchResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void SimpleTrafficSwitchProxy::unregisterServiceListeners( void )
{
    SimpleTrafficSwitchResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    SimpleTrafficSwitchProxy::SimpleTrafficSwitchServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void SimpleTrafficSwitchProxy::requestSwitchLight( bool switchOn )
{
    static const NESimpleTrafficSwitch::eMessageIDs msgId = NESimpleTrafficSwitch::eMessageIDs::MsgId_requestSwitchLight;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << switchOn;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void SimpleTrafficSwitchProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    SimpleTrafficSwitchResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SimpleTrafficSwitchResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void SimpleTrafficSwitchProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    SimpleTrafficSwitchResponseEvent * eventResp = RUNTIME_CAST( &eventElem, SimpleTrafficSwitchResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_SimpleTrafficSwitchProxy_updateData);
void SimpleTrafficSwitchProxy::updateData( SimpleTrafficSwitchResponseEvent & eventElem, NESimpleTrafficSwitch::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const SimpleTrafficSwitchResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NESimpleTrafficSwitch::eMessageIDs::MsgId_IsSwitchedOn:
        stream >> mIsSwitchedOn;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_SimpleTrafficSwitchProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void SimpleTrafficSwitchProxy::processResponse( SimpleTrafficSwitchResponseEvent & evenElem )
{
    NESimpleTrafficSwitch::eMessageIDs respId  = static_cast<NESimpleTrafficSwitch::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NESimpleTrafficSwitch::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NESimpleTrafficSwitch::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NESimpleTrafficSwitch::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }
        
        setStates   = respId != NESimpleTrafficSwitch::eMessageIDs::MsgId_NotProcessed;            
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
// End generate generated/src/private/SimpleTrafficSwitchProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 