//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CESystemProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CESystemProxy.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/CESystemProxy.hpp"
#include "src/gen/private/CESystemEvents.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CESystemProxy::CESystemServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CESystemProxy::CESystemServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CESystemProxy::CESystemServiceAvailableEvent::CESystemServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CESystemProxy::CESystemServiceAvailableEvent::~CESystemServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESystemProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CESystemProxy::_createInterfaceData( void )
{
    return NESystem::CreateInterfaceData( );
}

CEProxyBase * CESystemProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CESystemProxy(roleName, ownerThread);
}

CESystemProxy * CESystemProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CESystemProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NESystem::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CESystemProxy::_createProxy
                                                                        , ownerThread) );
}

CESystemProxy * CESystemProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CESystemProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NESystem::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CESystemProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CESystemProxy::CESystemProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CESystemProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mSystemState    (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
    ; // do nothing
}

CESystemProxy::~CESystemProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CESystemProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CESystemNotificationEvent(data);
}

CEServiceRequestEvent * CESystemProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CESystemRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CESystemProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CESystemNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CESystemProxy::CreateRemoteResponseEvent(const IEInStream & /* stream */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CERemoteResponseEvent * CESystemProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & /* addrProxy */, unsigned int /* msgId */, NEService::eResultType /* reason */, unsigned int /* seqNr */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CEProxyBase::CEServiceAvailableEvent * CESystemProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CESystemProxy::CESystemServiceAvailableEvent(consumer) );
}

void CESystemProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CESystemResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CESystemProxy::UnregisterServiceListeners( void )
{
    CESystemResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CESystemProxy::CESystemServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void CESystemProxy::RequestPowerOff( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_RequestPowerOff;
    SendRequestEvent( static_cast<unsigned int>(msgId), CEEventDataStream::EmptyData, NULL );
}
void CESystemProxy::RequestPowerOn( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_RequestPowerOn;
    SendRequestEvent( static_cast<unsigned int>(msgId), CEEventDataStream::EmptyData, NULL );
}
void CESystemProxy::RequestShutdown( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_RequestShutdown;
    SendRequestEvent( static_cast<unsigned int>(msgId), CEEventDataStream::EmptyData, NULL );
}
void CESystemProxy::RequestStart( void )
{
    static const NESystem::eMessageIDs msgId = NESystem::MSG_ID_RequestStart;
    SendRequestEvent( static_cast<unsigned int>(msgId), CEEventDataStream::EmptyData, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CESystemProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CESystemResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CESystemResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CESystemProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CESystemResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CESystemResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_CESystemProxy_UpdateData);
void CESystemProxy::UpdateData( CESystemResponseEvent & eventElem, NESystem::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CESystemResponseEvent &>(eventElem).GetData().GetReadStream();

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
    case NESystem::MSG_ID_SystemState:
        stream >> mSystemState;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_CESystemProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CESystemProxy::ProcessResponse( CESystemResponseEvent & evenElem )
{
    NESystem::eMessageIDs respId  = static_cast<NESystem::eMessageIDs>(evenElem.GetResponseId());
    NEService::eResultType resultType  = evenElem.GetResult();

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
        respId      = static_cast<NESystem::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystem::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NESystem::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NESystem::MSG_ID_NO_PROCEED;            
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
        UpdateData(evenElem, respId);
    if (setStates == true)
        SetState(static_cast<unsigned int>(respId), dataValid ? NEService::DATA_OK : NEService::DATA_INVALID);

    NotifyListeners(static_cast<unsigned int>(respId), resultType, evenElem.GetSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CESystemProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 