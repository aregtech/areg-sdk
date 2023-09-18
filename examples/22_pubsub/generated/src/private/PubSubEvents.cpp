//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSub.
 *
 * \file            generated/src/private/PubSubEvents.hpp
 * \ingroup         PubSub Service Interface
 * \brief           This is an automatic generated code of PubSub
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/PubSubEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// PubSubRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( PubSubRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubRequestEvent::PubSubRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

PubSubRequestEvent::PubSubRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEPubSub::getString( static_cast<NEPubSub::eMessageIDs>(reqId) ) )
{
}

PubSubRequestEvent::PubSubRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubNotifyRequestEvent::PubSubNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

PubSubNotifyRequestEvent::PubSubNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubResponseEvent::PubSubResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

PubSubResponseEvent::PubSubResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPubSub::getString(static_cast<NEPubSub::eMessageIDs>(responseId)) )
{
}

PubSubResponseEvent::PubSubResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

PubSubResponseEvent::PubSubResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* PubSubResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW PubSubResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// PubSubNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubNotificationEvent::PubSubNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubEvents.cpp file
//////////////////////////////////////////////////////////////////////////
