//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteHelloWorld.
 * Generated at     29.05.2021  12:43:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteHelloWorldEvents.cpp
 * \ingroup         RemoteHelloWorld Service Interface
 * \brief           This is an automatic generated code of RemoteHelloWorld Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shareipcmix/src/private/RemoteHelloWorldEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   RemoteHelloWorldRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( RemoteHelloWorldRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteHelloWorldRequestEvent::RemoteHelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

RemoteHelloWorldRequestEvent::RemoteHelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NERemoteHelloWorld::getString( static_cast<NERemoteHelloWorld::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

RemoteHelloWorldRequestEvent::RemoteHelloWorldRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

RemoteHelloWorldRequestEvent::~RemoteHelloWorldRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteHelloWorldNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteHelloWorldNotifyRequestEvent::RemoteHelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

RemoteHelloWorldNotifyRequestEvent::RemoteHelloWorldNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

RemoteHelloWorldNotifyRequestEvent::~RemoteHelloWorldNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteHelloWorldResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteHelloWorldResponseEvent::RemoteHelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

RemoteHelloWorldResponseEvent::RemoteHelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NERemoteHelloWorld::getString(static_cast<NERemoteHelloWorld::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

RemoteHelloWorldResponseEvent::RemoteHelloWorldResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

RemoteHelloWorldResponseEvent::RemoteHelloWorldResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
RemoteHelloWorldResponseEvent::~RemoteHelloWorldResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* RemoteHelloWorldResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW RemoteHelloWorldResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( RemoteHelloWorldNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

RemoteHelloWorldNotificationEvent::RemoteHelloWorldNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

RemoteHelloWorldNotificationEvent::~RemoteHelloWorldNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////
