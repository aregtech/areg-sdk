//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CETrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CETrafficLightProxy.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/CETrafficLightProxy.hpp"
#include "src/gen/private/CETrafficLightEvents.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CETrafficLightProxy::CETrafficLightServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CETrafficLightProxy::CETrafficLightServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CETrafficLightProxy::CETrafficLightServiceAvailableEvent::CETrafficLightServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CETrafficLightProxy::CETrafficLightServiceAvailableEvent::~CETrafficLightServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CETrafficLightProxy::_createInterfaceData( void )
{
    return NETrafficLight::CreateInterfaceData( );
}

CEProxyBase * CETrafficLightProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CETrafficLightProxy(roleName, ownerThread);
}

CETrafficLightProxy * CETrafficLightProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CETrafficLightProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NETrafficLight::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CETrafficLightProxy::_createProxy
                                                                        , ownerThread) );
}

CETrafficLightProxy * CETrafficLightProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CETrafficLightProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NETrafficLight::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CETrafficLightProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CETrafficLightProxy::CETrafficLightProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CETrafficLightProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mTrafficLight       (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamlightColor    (  )
{
    ; // do nothing
}

CETrafficLightProxy::~CETrafficLightProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CETrafficLightProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CETrafficLightNotificationEvent(data);
}

CEServiceRequestEvent * CETrafficLightProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CETrafficLightRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CETrafficLightProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CETrafficLightNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CETrafficLightProxy::CreateRemoteResponseEvent(const IEInStream & /* stream */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CERemoteResponseEvent * CETrafficLightProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & /* addrProxy */, unsigned int /* msgId */, NEService::eResultType /* reason */, unsigned int /* seqNr */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CEProxyBase::CEServiceAvailableEvent * CETrafficLightProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CETrafficLightProxy::CETrafficLightServiceAvailableEvent(consumer) );
}

void CETrafficLightProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CETrafficLightResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CETrafficLightProxy::UnregisterServiceListeners( void )
{
    CETrafficLightResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CETrafficLightProxy::CETrafficLightServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
unsigned int CETrafficLightProxy::RequestChangeLight( IENotificationEventConsumer & caller, const NETrafficLight::eLight & lightColor, const bool & holdon )
{
    static const NETrafficLight::eMessageIDs msgId = NETrafficLight::MSG_ID_RequestChangeLight;
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << lightColor;
    stream << holdon;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, &caller );
    return mSequenceCount;
}
    
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CETrafficLightProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CETrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CETrafficLightResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CETrafficLightProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CETrafficLightResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CETrafficLightResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_CETrafficLightProxy_UpdateData);
void CETrafficLightProxy::UpdateData( CETrafficLightResponseEvent & eventElem, NETrafficLight::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CETrafficLightResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NETrafficLight::MSG_ID_ResponseChangeLight:
        stream >> mParamlightColor;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NETrafficLight::MSG_ID_TrafficLight:
        stream >> mTrafficLight;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_CETrafficLightProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CETrafficLightProxy::ProcessResponse( CETrafficLightResponseEvent & evenElem )
{
    NETrafficLight::eMessageIDs respId  = static_cast<NETrafficLight::eMessageIDs>(evenElem.GetResponseId());
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
        respId      = static_cast<NETrafficLight::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NETrafficLight::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NETrafficLight::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NETrafficLight::MSG_ID_NO_PROCEED;            
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
// End generate src/gen/private/CETrafficLightProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 