//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/private/CentralMessagerProxy.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface Proxy class implementation.
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
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * CentralMessagerProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW CentralMessagerProxy(roleName, ownerThread);
}

CentralMessagerProxy * CentralMessagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<CentralMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NECentralMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &CentralMessagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

CentralMessagerProxy * CentralMessagerProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::getEmptyString()*/ )
{
    return static_cast<CentralMessagerProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NECentralMessager::getInterfaceData()
                                                                      , connectListener
                                                                      , &CentralMessagerProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CentralMessagerProxy::CentralMessagerProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
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
}

CentralMessagerProxy::~CentralMessagerProxy( void )
{
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
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::eMessageIDs::MsgId_requestSendMessage;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    stream << dateTime;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
void CentralMessagerProxy::requestKeyTyping( const String & nickName, unsigned int cookie, const String & newMessage )
{
    static const NECentralMessager::eMessageIDs msgId = NECentralMessager::eMessageIDs::MsgId_requestKeyTyping;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << nickName;
    stream << cookie;
    stream << newMessage;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
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
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void CentralMessagerProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    CentralMessagerResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CentralMessagerResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_private_CentralMessagerProxy_updateData);
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
    case NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        stream >> mParamdateTime;
        break;

    case NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping:
        stream >> mParamnickName;
        stream >> mParamcookie;
        stream >> mParamnewMessage;
        break;

    case NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage:
        stream >> mParamserverMessage;
        stream >> mParamdateTime;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    default:
        {
            TRACE_SCOPE(generated_private_CentralMessagerProxy_updateData);
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
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NECentralMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NECentralMessager::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NECentralMessager::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NECentralMessager::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/private/CentralMessagerProxy.cpp file
//////////////////////////////////////////////////////////////////////////
