//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LargeDataEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     07.10.2022  01:35:30 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/private/LargeDataEvents.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/LargeDataEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// LargeDataRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LargeDataRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LargeDataRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( LargeDataRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// LargeDataRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LargeDataRequestEvent::LargeDataRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

LargeDataRequestEvent::LargeDataRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NELargeData::getString( static_cast<NELargeData::eMessageIDs>(reqId) ) )
{
}

LargeDataRequestEvent::LargeDataRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// LargeDataNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LargeDataNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LargeDataNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// LargeDataNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LargeDataNotifyRequestEvent::LargeDataNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

LargeDataNotifyRequestEvent::LargeDataNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// LargeDataResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LargeDataResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LargeDataResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// LargeDataResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LargeDataResponseEvent::LargeDataResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

LargeDataResponseEvent::LargeDataResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NELargeData::getString(static_cast<NELargeData::eMessageIDs>(responseId)) )
{
}

LargeDataResponseEvent::LargeDataResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

LargeDataResponseEvent::LargeDataResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// LargeDataResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* LargeDataResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW LargeDataResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// LargeDataNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LargeDataNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LargeDataNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// LargeDataNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LargeDataNotificationEvent::LargeDataNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LargeDataEvents.cpp file
//////////////////////////////////////////////////////////////////////////
