//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PublicHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.12.2022  16:19:17 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/private/PublicHelloWorldEvents.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/PublicHelloWorldEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PublicHelloWorldRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( PublicHelloWorldRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PublicHelloWorldRequestEvent::PublicHelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

PublicHelloWorldRequestEvent::PublicHelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEPublicHelloWorld::getString( static_cast<NEPublicHelloWorld::eMessageIDs>(reqId) ) )
{
}

PublicHelloWorldRequestEvent::PublicHelloWorldRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PublicHelloWorldNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PublicHelloWorldNotifyRequestEvent::PublicHelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

PublicHelloWorldNotifyRequestEvent::PublicHelloWorldNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PublicHelloWorldResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PublicHelloWorldResponseEvent::PublicHelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

PublicHelloWorldResponseEvent::PublicHelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPublicHelloWorld::getString(static_cast<NEPublicHelloWorld::eMessageIDs>(responseId)) )
{
}

PublicHelloWorldResponseEvent::PublicHelloWorldResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

PublicHelloWorldResponseEvent::PublicHelloWorldResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* PublicHelloWorldResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW PublicHelloWorldResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PublicHelloWorldNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PublicHelloWorldNotificationEvent::PublicHelloWorldNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PublicHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////
