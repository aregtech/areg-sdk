//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     04.07.2021  04:30:02 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/private/DirectConnectionProxy.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/private/DirectConnectionProxy.hpp"
#include "generated/private/DirectConnectionEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectConnectionProxy::DirectConnectionServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(DirectConnectionProxy::DirectConnectionServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

DirectConnectionProxy::DirectConnectionServiceAvailableEvent::DirectConnectionServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

DirectConnectionProxy::DirectConnectionServiceAvailableEvent::~DirectConnectionServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * DirectConnectionProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW DirectConnectionProxy(roleName, ownerThread);
}

DirectConnectionProxy * DirectConnectionProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<DirectConnectionProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectConnection::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectConnectionProxy::_createProxy
                                                                      , ownerThread) );
}

DirectConnectionProxy * DirectConnectionProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<DirectConnectionProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEDirectConnection::getInterfaceData()
                                                                      , connectListener
                                                                      , &DirectConnectionProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectConnectionProxy::DirectConnectionProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NEDirectConnection::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mInitiatedConnections   (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsucceeded         (  )
    , mParamtarget            (  )
    , mParaminitiator         (  )
    , mParamlistParticipants  (  )
{
    ; // do nothing
}

DirectConnectionProxy::~DirectConnectionProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * DirectConnectionProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW DirectConnectionNotificationEvent(data);
}

ServiceRequestEvent * DirectConnectionProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW DirectConnectionRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* DirectConnectionProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW DirectConnectionNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * DirectConnectionProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectConnectionResponseEvent(stream) );
}

RemoteResponseEvent * DirectConnectionProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW DirectConnectionResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * DirectConnectionProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW DirectConnectionProxy::DirectConnectionServiceAvailableEvent(consumer) );
}

void DirectConnectionProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    DirectConnectionResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void DirectConnectionProxy::unregisterServiceListeners( void )
{
    DirectConnectionResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    DirectConnectionProxy::DirectConnectionServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int DirectConnectionProxy::requestConnectoinSetup( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_requestConnectoinSetup;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int DirectConnectionProxy::requestAddParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_requestAddParticipant;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
unsigned int DirectConnectionProxy::requestRemoveParticipant( IENotificationEventConsumer & caller, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_requestRemoveParticipant;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    stream << listParticipants;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
void DirectConnectionProxy::requestCloseConnection( const NEDirectConnection::sInitiator & initiator )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_requestCloseConnection;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << initiator;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void DirectConnectionProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    DirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectConnectionResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void DirectConnectionProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    DirectConnectionResponseEvent * eventResp = RUNTIME_CAST( &eventElem, DirectConnectionResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(GENERATED_private_DirectConnectionProxy_updateData);
void DirectConnectionProxy::updateData( DirectConnectionResponseEvent & eventElem, NEDirectConnection::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const DirectConnectionResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEDirectConnection::MSG_ID_responseConnectoinSetup:
        stream >> mParamsucceeded;
        stream >> mParamtarget;
        stream >> mParaminitiator;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::MSG_ID_responseAddParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    case NEDirectConnection::MSG_ID_responseRemoveParticipant:
        stream >> mParamsucceeded;
        stream >> mParamlistParticipants;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        stream >> mInitiatedConnections;
        break;

    default:
        {
            TRACE_SCOPE(GENERATED_private_DirectConnectionProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void DirectConnectionProxy::processResponse( DirectConnectionResponseEvent & evenElem )
{
    NEDirectConnection::eMessageIDs respId  = static_cast<NEDirectConnection::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectConnection::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEDirectConnection::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEDirectConnection::MSG_ID_NO_PROCEED;            
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
// End generate generated/private/DirectConnectionProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 