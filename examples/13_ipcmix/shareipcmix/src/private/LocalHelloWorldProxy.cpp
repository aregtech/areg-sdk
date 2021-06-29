//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/LocalHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     11.06.2021  21:11:04 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/LocalHelloWorldProxy.cpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shareipcmix/src/private/LocalHelloWorldProxy.hpp"
#include "shareipcmix/src/private/LocalHelloWorldEvents.hpp"
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
    ; // do nothing
}

LocalHelloWorldProxy::LocalHelloWorldServiceAvailableEvent::~LocalHelloWorldServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * LocalHelloWorldProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW LocalHelloWorldProxy(roleName, ownerThread);
}

LocalHelloWorldProxy * LocalHelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<LocalHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELocalHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &LocalHelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

LocalHelloWorldProxy * LocalHelloWorldProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<LocalHelloWorldProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NELocalHelloWorld::getInterfaceData()
                                                                      , connectListener
                                                                      , &LocalHelloWorldProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
LocalHelloWorldProxy::LocalHelloWorldProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NELocalHelloWorld::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mConnectedClients   (  )
    , mRemainOutput       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamclientInfo    (  )
{
    ; // do nothing
}

LocalHelloWorldProxy::~LocalHelloWorldProxy( void )
{
    ; // do nothing
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
 
unsigned int LocalHelloWorldProxy::requestHelloWorld( IENotificationEventConsumer & caller, const String & roleName, const String & addMessage/* = "" */ )
{
    static const NELocalHelloWorld::eMessageIDs msgId = NELocalHelloWorld::MSG_ID_requestHelloWorld;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << roleName;
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

void LocalHelloWorldProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    LocalHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LocalHelloWorldResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void LocalHelloWorldProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    LocalHelloWorldResponseEvent * eventResp = RUNTIME_CAST( &eventElem, LocalHelloWorldResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_private_LocalHelloWorldProxy_updateData);
void LocalHelloWorldProxy::updateData( LocalHelloWorldResponseEvent & eventElem, NELocalHelloWorld::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const LocalHelloWorldResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NELocalHelloWorld::MSG_ID_responseHelloWorld:
        stream >> mParamclientInfo;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NELocalHelloWorld::MSG_ID_ConnectedClients:
        stream >> mConnectedClients;
        break;

    case NELocalHelloWorld::MSG_ID_RemainOutput:
        stream >> mRemainOutput;
        break;

    default:
        {
            TRACE_SCOPE(shareipcmix_src_private_LocalHelloWorldProxy_updateData);
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
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NELocalHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NELocalHelloWorld::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NELocalHelloWorld::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NELocalHelloWorld::MSG_ID_NO_PROCEED;            
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
// End generate shareipcmix/src/private/LocalHelloWorldProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 