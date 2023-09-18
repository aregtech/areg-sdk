//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PubSubMixEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:34:02 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PubSubMix.
 *
 * \file            generated/src/private/PubSubMixEvents.hpp
 * \ingroup         PubSubMix Service Interface
 * \brief           This is an automatic generated code of PubSubMix
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/PubSubMixEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// PubSubMixRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubMixRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PubSubMixRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( PubSubMixRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubMixRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubMixRequestEvent::PubSubMixRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

PubSubMixRequestEvent::PubSubMixRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEPubSubMix::getString( static_cast<NEPubSubMix::eMessageIDs>(reqId) ) )
{
}

PubSubMixRequestEvent::PubSubMixRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubMixNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubMixNotifyRequestEvent::PubSubMixNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

PubSubMixNotifyRequestEvent::PubSubMixNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubMixResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubMixResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubMixResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubMixResponseEvent::PubSubMixResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

PubSubMixResponseEvent::PubSubMixResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, SequenceNumber seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPubSubMix::getString(static_cast<NEPubSubMix::eMessageIDs>(responseId)) )
{
}

PubSubMixResponseEvent::PubSubMixResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

PubSubMixResponseEvent::PubSubMixResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* PubSubMixResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW PubSubMixResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PubSubMixNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// PubSubMixNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PubSubMixNotificationEvent::PubSubMixNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PubSubMixEvents.cpp file
//////////////////////////////////////////////////////////////////////////
