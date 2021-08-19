//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 * Generated at     11.08.2021  13:18:21 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SimpleTrafficSwitchEvents.cpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/SimpleTrafficSwitchEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SimpleTrafficSwitchRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( SimpleTrafficSwitchRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficSwitchRequestEvent::SimpleTrafficSwitchRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

SimpleTrafficSwitchRequestEvent::SimpleTrafficSwitchRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NESimpleTrafficSwitch::getString( static_cast<NESimpleTrafficSwitch::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


SimpleTrafficSwitchRequestEvent::~SimpleTrafficSwitchRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficSwitchNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficSwitchNotifyRequestEvent::SimpleTrafficSwitchNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


SimpleTrafficSwitchNotifyRequestEvent::~SimpleTrafficSwitchNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficSwitchResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficSwitchResponseEvent::SimpleTrafficSwitchResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

SimpleTrafficSwitchResponseEvent::SimpleTrafficSwitchResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NESimpleTrafficSwitch::getString(static_cast<NESimpleTrafficSwitch::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

SimpleTrafficSwitchResponseEvent::SimpleTrafficSwitchResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

SimpleTrafficSwitchResponseEvent::~SimpleTrafficSwitchResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* SimpleTrafficSwitchResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW SimpleTrafficSwitchResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SimpleTrafficSwitchNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SimpleTrafficSwitchNotificationEvent::SimpleTrafficSwitchNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

SimpleTrafficSwitchNotificationEvent::~SimpleTrafficSwitchNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficSwitchEvents.cpp file
//////////////////////////////////////////////////////////////////////////
