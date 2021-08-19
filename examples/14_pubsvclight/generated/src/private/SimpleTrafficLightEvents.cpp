//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 * Generated at     11.08.2021  13:18:20 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SimpleTrafficLightEvents.cpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/SimpleTrafficLightEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficLightRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( SimpleTrafficLightRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficLightRequestEvent::SimpleTrafficLightRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

SimpleTrafficLightRequestEvent::SimpleTrafficLightRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NESimpleTrafficLight::getString( static_cast<NESimpleTrafficLight::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

SimpleTrafficLightRequestEvent::SimpleTrafficLightRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

SimpleTrafficLightRequestEvent::~SimpleTrafficLightRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficLightNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficLightNotifyRequestEvent::SimpleTrafficLightNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

SimpleTrafficLightNotifyRequestEvent::SimpleTrafficLightNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

SimpleTrafficLightNotifyRequestEvent::~SimpleTrafficLightNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficLightResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NESimpleTrafficLight::getString(static_cast<NESimpleTrafficLight::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
SimpleTrafficLightResponseEvent::~SimpleTrafficLightResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* SimpleTrafficLightResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW SimpleTrafficLightResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficLightNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficLightNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficLightNotificationEvent::SimpleTrafficLightNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

SimpleTrafficLightNotificationEvent::~SimpleTrafficLightNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////
