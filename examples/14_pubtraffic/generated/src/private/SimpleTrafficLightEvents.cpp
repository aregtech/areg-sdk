//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     15.11.2023  14:51:50 GMT+01:00
 *                  Create by AREG SDK code generator tool from source SimpleTrafficLight.
 *
 * \file            generated/src/private/SimpleTrafficLightEvents.hpp
 * \ingroup         SimpleTrafficLight Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficLight
 *                  Service Interface event classes implementation.
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
}

SimpleTrafficLightRequestEvent::SimpleTrafficLightRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NESimpleTrafficLight::getString( static_cast<NESimpleTrafficLight::eMessageIDs>(reqId) ) )
{
}

SimpleTrafficLightRequestEvent::SimpleTrafficLightRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
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
}

SimpleTrafficLightNotifyRequestEvent::SimpleTrafficLightNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
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

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NESimpleTrafficLight::getString(static_cast<NESimpleTrafficLight::eMessageIDs>(responseId)) )
{
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

SimpleTrafficLightResponseEvent::SimpleTrafficLightResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
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
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////
