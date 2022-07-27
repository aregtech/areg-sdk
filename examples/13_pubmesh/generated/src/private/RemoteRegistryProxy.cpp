//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/RemoteRegistryProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:13 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/private/RemoteRegistryProxy.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/RemoteRegistryProxy.hpp"
#include "generated/src/private/RemoteRegistryEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent::RemoteRegistryServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * RemoteRegistryProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW RemoteRegistryProxy(roleName, ownerThread);
}

RemoteRegistryProxy * RemoteRegistryProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<RemoteRegistryProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NERemoteRegistry::getInterfaceData()
                                                                      , connectListener
                                                                      , &RemoteRegistryProxy::_createProxy
                                                                      , ownerThread) );
}

RemoteRegistryProxy * RemoteRegistryProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<RemoteRegistryProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NERemoteRegistry::getInterfaceData()
                                                                      , connectListener
                                                                      , &RemoteRegistryProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteRegistryProxy::RemoteRegistryProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NERemoteRegistry::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mRegistryList   (  )
    , mRemainOutputs  (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclient    (  )
    , mParamclientID  (  )
{
}

RemoteRegistryProxy::~RemoteRegistryProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * RemoteRegistryProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW RemoteRegistryNotificationEvent(data);
}

ServiceRequestEvent * RemoteRegistryProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW RemoteRegistryRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* RemoteRegistryProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW RemoteRegistryNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * RemoteRegistryProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW RemoteRegistryResponseEvent(stream) );
}

RemoteResponseEvent * RemoteRegistryProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW RemoteRegistryResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * RemoteRegistryProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent(consumer) );
}

void RemoteRegistryProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    RemoteRegistryResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void RemoteRegistryProxy::unregisterServiceListeners( void )
{
    RemoteRegistryResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int RemoteRegistryProxy::requestRegister( IENotificationEventConsumer & caller, const String & name, const ServiceAddress & service, const String & thread, const String & process )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::eMessageIDs::MsgId_requestRegister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << name;
    stream << service;
    stream << thread;
    stream << process;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void RemoteRegistryProxy::requestUnregister( const NERemoteRegistry::sClientRegister & client )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::eMessageIDs::MsgId_requestUnregister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
unsigned int RemoteRegistryProxy::requestHelloWorld( IENotificationEventConsumer & caller, unsigned int clientID, const String & addMessage )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    stream << addMessage;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void RemoteRegistryProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    RemoteRegistryResponseEvent * eventResp = RUNTIME_CAST( &eventElem, RemoteRegistryResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void RemoteRegistryProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    RemoteRegistryResponseEvent * eventResp = RUNTIME_CAST( &eventElem, RemoteRegistryResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_RemoteRegistryProxy_updateData);
void RemoteRegistryProxy::updateData( RemoteRegistryResponseEvent & eventElem, NERemoteRegistry::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const RemoteRegistryResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NERemoteRegistry::eMessageIDs::MsgId_responseRegister:
        stream >> mParamclient;
        break;

    case NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld:
        stream >> mParamclientID;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
        stream >> mRegistryList;
        break;

    case NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
        stream >> mRemainOutputs;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_RemoteRegistryProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void RemoteRegistryProxy::processResponse( RemoteRegistryResponseEvent & evenElem )
{
    NERemoteRegistry::eMessageIDs respId  = static_cast<NERemoteRegistry::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NERemoteRegistry::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NERemoteRegistry::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NERemoteRegistry::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }
        
        setStates   = respId != NERemoteRegistry::eMessageIDs::MsgId_NotProcessed;            
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
// End generate generated/src/private/RemoteRegistryProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 