//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     04.07.2021  04:21:25 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryEvents.cpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shareipcmix/src/private/RemoteRegistryEvents.hpp"
 
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
    ; // do nothing
}

RemoteRegistryRequestEvent::RemoteRegistryRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NERemoteRegistry::getString( static_cast<NERemoteRegistry::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

RemoteRegistryRequestEvent::RemoteRegistryRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

RemoteRegistryRequestEvent::~RemoteRegistryRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

RemoteRegistryNotifyRequestEvent::RemoteRegistryNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

RemoteRegistryNotifyRequestEvent::~RemoteRegistryNotifyRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NERemoteRegistry::getString(static_cast<NERemoteRegistry::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

RemoteRegistryResponseEvent::RemoteRegistryResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
RemoteRegistryResponseEvent::~RemoteRegistryResponseEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

RemoteRegistryNotificationEvent::~RemoteRegistryNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryEvents.cpp file
//////////////////////////////////////////////////////////////////////////
