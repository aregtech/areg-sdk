//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/ConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/private/ConnectionManagerEvents.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/private/ConnectionManagerEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   ConnectionManagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( ConnectionManagerRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ConnectionManagerRequestEvent::ConnectionManagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

ConnectionManagerRequestEvent::ConnectionManagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEConnectionManager::getString( static_cast<NEConnectionManager::eMessageIDs>(reqId) ) )
{
}

ConnectionManagerRequestEvent::ConnectionManagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( ConnectionManagerNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ConnectionManagerNotifyRequestEvent::ConnectionManagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

ConnectionManagerNotifyRequestEvent::ConnectionManagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( ConnectionManagerResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEConnectionManager::getString(static_cast<NEConnectionManager::eMessageIDs>(responseId)) )
{
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* ConnectionManagerResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW ConnectionManagerResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( ConnectionManagerNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

ConnectionManagerNotificationEvent::ConnectionManagerNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/ConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
