//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/OptionsProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/private/OptionsProxy.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/OptionsProxy.hpp"
#include "generated/src/private/OptionsEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// OptionsProxy::OptionsServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(OptionsProxy::OptionsServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

OptionsProxy::OptionsServiceAvailableEvent::OptionsServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
}

//////////////////////////////////////////////////////////////////////////
// OptionsProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * OptionsProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW OptionsProxy(roleName, ownerThread);
}

OptionsProxy * OptionsProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<OptionsProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEOptions::getInterfaceData()
                                                                      , connectListener
                                                                      , &OptionsProxy::_createProxy
                                                                      , ownerThread) );
}

OptionsProxy * OptionsProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<OptionsProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEOptions::getInterfaceData()
                                                                      , connectListener
                                                                      , &OptionsProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
OptionsProxy::OptionsProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEOptions::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mOptionData         (  )

/************************************************************************
 * Parameters
 ************************************************************************/
    , mParamsentData      (  )
    , mParammissedData    (  )
    , mParamgenPackets    (  )
    , mParamavSize        (  )
{
}

OptionsProxy::~OptionsProxy( void )
{
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * OptionsProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW OptionsNotificationEvent(data);
}

ServiceRequestEvent * OptionsProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW OptionsRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* OptionsProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW OptionsNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

ProxyBase::ServiceAvailableEvent * OptionsProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW OptionsProxy::OptionsServiceAvailableEvent(consumer) );
}

void OptionsProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    OptionsResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void OptionsProxy::unregisterServiceListeners( void )
{
    OptionsResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    OptionsProxy::OptionsServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/

unsigned int OptionsProxy::requestDataRate( IENotificationEventConsumer & caller )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_requestDataRate;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
unsigned int OptionsProxy::requestPacketRate( IENotificationEventConsumer & caller )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_requestPacketRate;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, &caller );
    return mSequenceCount;
}
    
void OptionsProxy::requestChangeOptions( const NEOptions::sOptionData & Options )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_requestChangeOptions;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << Options;
    sendRequestEvent( static_cast<unsigned int>(msgId), args, nullptr );
}
void OptionsProxy::requestQuitApplication( void )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_requestQuitApplication;
    sendRequestEvent( static_cast<unsigned int>(msgId), EventDataStream::EmptyData, nullptr );
}
/************************************************************************
 * Event processing.
 ************************************************************************/

/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void OptionsProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    OptionsResponseEvent * eventResp = RUNTIME_CAST( &eventElem, OptionsResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void OptionsProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    OptionsResponseEvent * eventResp = RUNTIME_CAST( &eventElem, OptionsResponseEvent );
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_OptionsProxy_updateData);
void OptionsProxy::updateData( OptionsResponseEvent & eventElem, NEOptions::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const OptionsResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    case NEOptions::eMessageIDs::MsgId_responseDataRate:
        stream >> mParamsentData;
        stream >> mParammissedData;
        break;

    case NEOptions::eMessageIDs::MsgId_responsePacketRate:
        stream >> mParamgenPackets;
        stream >> mParamavSize;
        break;

    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEOptions::eMessageIDs::MsgId_OptionData:
        stream >> mOptionData;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_OptionsProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}

    void OptionsProxy::processResponse( OptionsResponseEvent & evenElem )
{
    NEOptions::eMessageIDs respId  = static_cast<NEOptions::eMessageIDs>(evenElem.getResponseId());
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
        respId      = static_cast<NEOptions::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEOptions::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEOptions::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEOptions::eMessageIDs::MsgId_NotProcessed;
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
// End generate generated/src/private/OptionsProxy.cpp file
//////////////////////////////////////////////////////////////////////////
