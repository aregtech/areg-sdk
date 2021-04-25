//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CEMessageOutputProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CEMessageOutputProxy.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/private/CEMessageOutputProxy.hpp"
#include "src/gen/private/CEMessageOutputEvents.hpp"
#include "areg/src/component/IEProxyListener.hpp"
#include "areg/src/base/CEThread.hpp"
#include "areg/src/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent, CEProxyBase::CEServiceAvailableEvent)

CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent::CEMessageOutputServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : CEProxyBase::CEServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent::~CEMessageOutputServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////
const NEService::SInterfaceData & CEMessageOutputProxy::_createInterfaceData( void )
{
    return NEMessageOutput::CreateInterfaceData( );
}

CEProxyBase * CEMessageOutputProxy::_createProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW CEMessageOutputProxy(roleName, ownerThread);
}

CEMessageOutputProxy * CEMessageOutputProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, CEDispatcherThread & ownerThread )
{
    return static_cast<CEMessageOutputProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEMessageOutput::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEMessageOutputProxy::_createProxy
                                                                        , ownerThread) );
}

CEMessageOutputProxy * CEMessageOutputProxy::CreateProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<CEMessageOutputProxy *>(CEProxyBase::FindOrCreateProxy( roleName
                                                                        , NEMessageOutput::CreateInterfaceData()
                                                                        , connectListener
                                                                        , &CEMessageOutputProxy::_createProxy
                                                                        , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEMessageOutputProxy::CEMessageOutputProxy( const char * roleName, CEDispatcherThread * ownerThread /*= static_cast<CEDispatcherThread *>(NULL)*/ )
    : CEProxyBase(roleName, CEMessageOutputProxy::_createInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mLastMessage        (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamuserAction    (  )
    , mParammsgInput      (  )
{
    ; // do nothing
}

CEMessageOutputProxy::~CEMessageOutputProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

CENotificationEvent * CEMessageOutputProxy::CreateNotificationEvent( const CENotificationEventData & data ) const
{
    return DEBUG_NEW CEMessageOutputNotificationEvent(data);
}

CEServiceRequestEvent * CEMessageOutputProxy::CreateRequestEvent( const CEEventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW CEMessageOutputRequestEvent(args, GetProxyAddress(), GetStubAddress(), reqId);
}

CEServiceRequestEvent* CEMessageOutputProxy::CreateNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW CEMessageOutputNotifyRequestEvent(GetProxyAddress(), GetStubAddress(), msgId, reqType);
}

CERemoteResponseEvent * CEMessageOutputProxy::CreateRemoteResponseEvent(const IEInStream & /* stream */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CERemoteResponseEvent * CEMessageOutputProxy::CreateRemoteRequestFailedEvent(const CEProxyAddress & /* addrProxy */, unsigned int /* msgId */, NEService::eResultType /* reason */, unsigned int /* seqNr */) const
{
    // Not relevant for local proxy
    return static_cast<CERemoteResponseEvent *>(NULL);
}

CEProxyBase::CEServiceAvailableEvent * CEMessageOutputProxy::CreateServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<CEProxyBase::CEServiceAvailableEvent *>( DEBUG_NEW CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent(consumer) );
}

void CEMessageOutputProxy::RegisterServiceListeners( void )
{
    CEProxyBase::RegisterServiceListeners( );
    CEMessageOutputResponseEvent::AddListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void CEMessageOutputProxy::UnregisterServiceListeners( void )
{
    CEMessageOutputResponseEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEMessageOutputProxy::CEMessageOutputServiceAvailableEvent::RemoveListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    CEProxyBase::UnregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
void CEMessageOutputProxy::RequestOutputMessage( const CEString & msgOutput, const CEDateTime & dateTime )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_RequestOutputMessage;
    CEEventDataStream args(CEEventDataStream::EventDataInternal);
    IEOutStream & stream = args.GetStreamForWrite();
    stream << msgOutput;
    stream << dateTime;
    SendRequestEvent( static_cast<unsigned int>(msgId), args, NULL );
}
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void CEMessageOutputProxy::ProcessResponseEvent( CEServiceResponseEvent & eventElem )
{
    CEMessageOutputResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEMessageOutputResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

void CEMessageOutputProxy::ProcessAttributeEvent( CEServiceResponseEvent & eventElem )
{
    CEMessageOutputResponseEvent * eventResp = RUNTIME_CAST( &eventElem, CEMessageOutputResponseEvent );
    if (eventResp != NULL)
        ProcessResponse( *eventResp );
    else
        ProcessProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_private_CEMessageOutputProxy_UpdateData);
void CEMessageOutputProxy::UpdateData( CEMessageOutputResponseEvent & eventElem, NEMessageOutput::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const CEMessageOutputResponseEvent &>(eventElem).GetData().GetReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    case NEMessageOutput::MSG_ID_BroadcastActionRequest:
        stream >> mParamuserAction;
        break;

    case NEMessageOutput::MSG_ID_BroadcastInputMessage:
        stream >> mParammsgInput;
        break;

    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEMessageOutput::MSG_ID_LastMessage:
        stream >> mLastMessage;
        break;

    default:
        {
            TRACE_SCOPE(src_gen_private_CEMessageOutputProxy_UpdateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , CEProxyAddress::ConvertAddressToPath(GetProxyAddress()).GetBuffer());
            ASSERT(false);
        }
        break;
    }
}
 
void CEMessageOutputProxy::ProcessResponse( CEMessageOutputResponseEvent & evenElem )
{
    NEMessageOutput::eMessageIDs respId  = static_cast<NEMessageOutput::eMessageIDs>(evenElem.GetResponseId());
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
        respId      = static_cast<NEMessageOutput::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEMessageOutput::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::IsRequestId(respId) )
            respId  = static_cast<NEMessageOutput::eMessageIDs>( mProxyData.GetResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEMessageOutput::MSG_ID_NO_PROCEED;            
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
// End generate src/gen/private/CEMessageOutputProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 