//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/RemoteRegistryEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:11 GMT+02:00
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/private/RemoteRegistryEvents.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/RemoteRegistryEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteRegistryRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( RemoteRegistryRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteRegistryRequestEvent::RemoteRegistryRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

RemoteRegistryRequestEvent::RemoteRegistryRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NERemoteRegistry::getString( static_cast<NERemoteRegistry::eMessageIDs>(reqId) ) )
{
}

RemoteRegistryRequestEvent::RemoteRegistryRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteRegistryNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteRegistryNotifyRequestEvent::RemoteRegistryNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

RemoteRegistryNotifyRequestEvent::RemoteRegistryNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteRegistryResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NERemoteRegistry::getString(static_cast<NERemoteRegistry::eMessageIDs>(responseId)) )
{
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* RemoteRegistryResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW RemoteRegistryResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteRegistryNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteRegistryNotificationEvent::RemoteRegistryNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/RemoteRegistryEvents.cpp file
//////////////////////////////////////////////////////////////////////////
