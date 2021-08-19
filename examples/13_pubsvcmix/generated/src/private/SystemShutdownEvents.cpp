//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SystemShutdownEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SystemShutdown.
 * Generated at     11.08.2021  13:17:34 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SystemShutdownEvents.cpp
 * \ingroup         SystemShutdown Service Interface
 * \brief           This is an automatic generated code of SystemShutdown Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/SystemShutdownEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// SystemShutdownRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemShutdownRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemShutdownRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( SystemShutdownRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SystemShutdownRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemShutdownRequestEvent::SystemShutdownRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

SystemShutdownRequestEvent::SystemShutdownRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NESystemShutdown::getString( static_cast<NESystemShutdown::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

SystemShutdownRequestEvent::SystemShutdownRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

SystemShutdownRequestEvent::~SystemShutdownRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemShutdownNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemShutdownNotifyRequestEvent::SystemShutdownNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

SystemShutdownNotifyRequestEvent::SystemShutdownNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

SystemShutdownNotifyRequestEvent::~SystemShutdownNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemShutdownResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemShutdownResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// SystemShutdownResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemShutdownResponseEvent::SystemShutdownResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

SystemShutdownResponseEvent::SystemShutdownResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NESystemShutdown::getString(static_cast<NESystemShutdown::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

SystemShutdownResponseEvent::SystemShutdownResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

SystemShutdownResponseEvent::SystemShutdownResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
SystemShutdownResponseEvent::~SystemShutdownResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* SystemShutdownResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW SystemShutdownResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemShutdownNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// SystemShutdownNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemShutdownNotificationEvent::SystemShutdownNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

SystemShutdownNotificationEvent::~SystemShutdownNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SystemShutdownEvents.cpp file
//////////////////////////////////////////////////////////////////////////
