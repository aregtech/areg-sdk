//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloUnblockEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/private/HelloUnblockEvents.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/HelloUnblockEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// HelloUnblockRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloUnblockRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloUnblockRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( HelloUnblockRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloUnblockRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloUnblockRequestEvent::HelloUnblockRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

HelloUnblockRequestEvent::HelloUnblockRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEHelloUnblock::getString( static_cast<NEHelloUnblock::eMessageIDs>(reqId) ) )
{
}

HelloUnblockRequestEvent::HelloUnblockRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloUnblockNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloUnblockNotifyRequestEvent::HelloUnblockNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

HelloUnblockNotifyRequestEvent::HelloUnblockNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloUnblockResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloUnblockResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// HelloUnblockResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloUnblockResponseEvent::HelloUnblockResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

HelloUnblockResponseEvent::HelloUnblockResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEHelloUnblock::getString(static_cast<NEHelloUnblock::eMessageIDs>(responseId)) )
{
}

HelloUnblockResponseEvent::HelloUnblockResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

HelloUnblockResponseEvent::HelloUnblockResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* HelloUnblockResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW HelloUnblockResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloUnblockNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// HelloUnblockNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloUnblockNotificationEvent::HelloUnblockNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloUnblockEvents.cpp file
//////////////////////////////////////////////////////////////////////////
