//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/ConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     04.07.2021  04:30:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/ConnectionManagerEvents.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/ConnectionManagerEvents.hpp"
 
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
    ; // do nothing
}

ConnectionManagerRequestEvent::ConnectionManagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEConnectionManager::getString( static_cast<NEConnectionManager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

ConnectionManagerRequestEvent::ConnectionManagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

ConnectionManagerRequestEvent::~ConnectionManagerRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

ConnectionManagerNotifyRequestEvent::ConnectionManagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

ConnectionManagerNotifyRequestEvent::~ConnectionManagerNotifyRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEConnectionManager::getString(static_cast<NEConnectionManager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

ConnectionManagerResponseEvent::ConnectionManagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
ConnectionManagerResponseEvent::~ConnectionManagerResponseEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

ConnectionManagerNotificationEvent::~ConnectionManagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/ConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
