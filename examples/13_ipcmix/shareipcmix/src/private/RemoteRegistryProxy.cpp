//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     04.07.2021  04:21:25 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryProxy.cpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shareipcmix/src/private/RemoteRegistryProxy.hpp"
#include "shareipcmix/src/private/RemoteRegistryEvents.hpp"
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
    ; // do nothing
}

RemoteRegistryProxy::RemoteRegistryServiceAvailableEvent::~RemoteRegistryServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * RemoteRegistryProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW RemoteRegistryProxy(roleName, ownerThread);
}

RemoteRegistryProxy * RemoteRegistryProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<RemoteRegistryProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NERemoteRegistry::getInterfaceData()
                                                                      , connectListener
                                                                      , &RemoteRegistryProxy::_createProxy
                                                                      , ownerThread) );
}

RemoteRegistryProxy * RemoteRegistryProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
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
RemoteRegistryProxy::RemoteRegistryProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
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
    ; // do nothing
}

RemoteRegistryProxy::~RemoteRegistryProxy( void )
{
    ; // do nothing
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
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::MSG_ID_requestRegister;
    EventDataStream args(EventDataStream::EventDataExternal);
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
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::MSG_ID_requestUnregister;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
unsigned int RemoteRegistryProxy::requestHelloWorld( IENotificationEventConsumer & caller, unsigned int clientID, const String & addMessage )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::MSG_ID_requestHelloWorld;
    EventDataStream args(EventDataStream::EventDataExternal);
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
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void RemoteRegistryProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    RemoteRegistryResponseEvent * eventResp = RUNTIME_CAST( &eventElem, RemoteRegistryResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_private_RemoteRegistryProxy_updateData);
void RemoteRegistryProxy::updateData( RemoteRegistryResponseEvent & eventElem, NERemoteRegistry::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const RemoteRegistryResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NERemoteRegistry::MSG_ID_responseRegister:
        stream >> mParamclient;
        break;

    case NERemoteRegistry::MSG_ID_responseHelloWorld:
        stream >> mParamclientID;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NERemoteRegistry::MSG_ID_RegistryList:
        stream >> mRegistryList;
        break;

    case NERemoteRegistry::MSG_ID_RemainOutputs:
        stream >> mRemainOutputs;
        break;

    default:
        {
            TRACE_SCOPE(shareipcmix_src_private_RemoteRegistryProxy_updateData);
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
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NERemoteRegistry::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NERemoteRegistry::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NERemoteRegistry::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NERemoteRegistry::MSG_ID_NO_PROCEED;            
        break;

    case NEService::RESULT_OK:
    case NEService::RESULT_DATA_OK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
        updateData(evenElem, respId);
    if (setStates == true)
        setState(static_cast<unsigned int>(respId), dataValid ? NEService::DATA_OK : NEService::DATA_INVALID);

    notifyListeners(static_cast<unsigned int>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 