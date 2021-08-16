//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     04.07.2021  04:29:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/private/CentralMessagerProxy.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/private/CentralMessagerProxy.hpp"
#include "generated/private/CentralMessagerEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy::CentralMessagerServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CentralMessagerProxy::CentralMessagerServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

CentralMessagerProxy::CentralMessagerServiceAvailableEvent::CentralMessagerServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CentralMessagerProxy::CentralMessagerServiceAvailableEvent::~CentralMessagerServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * CentralMessagerProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CentralMessagerProxy(roleName, ownerThread);
}

CentralMessagerProxy * CentralMessagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<CentralMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NECentralMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &CentralMessagerProxy::_createProxy
                                                                      , ownerThread) );
}

CentralMessagerProxy * CentralMessagerProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CentralMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NECentralMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &CentralMessagerProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CentralMessagerProxy::CentralMessagerProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NECentralMessager::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamnickName      (  )
    , mParamcookie        (  )
    , mParamnewMessage    (  )
    , mParamdateTime      (  )
    , mParamserverMessage (  )
{
    ; // do nothing
}

CentralMessagerProxy::~CentralMessagerProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * CentralMessagerProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW CentralMessagerNotificationEvent(data);
}

ServiceRequestEvent * CentralMessagerProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CentralMessagerRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* CentralMessagerProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CentralMessagerNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * CentralMessagerProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW CentralMessagerResponseEvent(stream) );
}

RemoteResponseEvent * CentralMessagerProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW CentralMessagerResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * CentralMessagerProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW CentralMessagerProxy::CentralMessagerServiceAvailableEvent(consumer) );
}

void CentralMessagerProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    CentralMessagerResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CentralMessagerProxy::unregisterServiceListeners( void )
{
    CentralMessagerResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CentralMessagerProxy::CentralMessagerServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void CentralMessagerProxy::requestSendMessage( const String & nickName, unsigned int cookie, const String & newMessage, const DateTime & dateTime )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_requestSendMessage;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
void CentralMessagerProxy::requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::MSG_ID_requestKeyTyping;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CentralMessagerProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    CentralMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CentralMessagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void CentralMessagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    CentralMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CentralMessagerResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(GENERATED_private_CentralMessagerProxy_updateData);
void CentralMessagerProxy::updateData( CentralMessagerResponseEvent & eventElem, NECentralMessager::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CentralMessagerResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NECentralMessager::MSG_ID_broadcastSendMessage:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        stream >> mParamdateTime;
        break;

    case NECentralMessager::MSG_ID_broadcastKeyTyping:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        break;

    case NECentralMessager::MSG_ID_broadcastBroadcastMessage:
        stream >> mParamserverMessage;
        stream >> mParamdateTime;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(GENERATED_private_CentralMessagerProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void CentralMessagerProxy::processResponse( CentralMessagerResponseEvent & evenElem )
{
    NECentralMessager::eMessageIDs respId  = static_cast<NECentralMessager::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NECentralMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NECentralMessager::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NECentralMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NECentralMessager::MSG_ID_NO_PROCEED;            
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
// End generate generated/private/CentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 