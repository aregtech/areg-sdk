//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:34 GMT+02:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 *
 * \file            generated/src/private/SimpleTrafficSwitchEvents.hpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch
 *                  Service Interface event classes implementation.
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
}

SimpleTrafficSwitchRequestEvent::SimpleTrafficSwitchRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NESimpleTrafficSwitch::getString( static_cast<NESimpleTrafficSwitch::eMessageIDs>(reqId) ) )
{
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
}

SimpleTrafficSwitchResponseEvent::SimpleTrafficSwitchResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NESimpleTrafficSwitch::getString(static_cast<NESimpleTrafficSwitch::eMessageIDs>(responseId)) )
{
}

SimpleTrafficSwitchResponseEvent::SimpleTrafficSwitchResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
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
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficSwitchEvents.cpp file
//////////////////////////////////////////////////////////////////////////
