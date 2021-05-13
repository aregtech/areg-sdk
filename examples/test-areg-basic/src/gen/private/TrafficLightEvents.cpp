//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/TrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/TrafficLightEvents.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/TrafficLightEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// TrafficLightRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLightRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficLightRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( TrafficLightRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficLightRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficLightRequestEvent::TrafficLightRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

TrafficLightRequestEvent::TrafficLightRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NETrafficLight::getString( static_cast<NETrafficLight::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


TrafficLightRequestEvent::~TrafficLightRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficLightNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficLightNotifyRequestEvent::TrafficLightNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


TrafficLightNotifyRequestEvent::~TrafficLightNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLightResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficLightResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficLightResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficLightResponseEvent::TrafficLightResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

TrafficLightResponseEvent::TrafficLightResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NETrafficLight::getString(static_cast<NETrafficLight::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

TrafficLightResponseEvent::TrafficLightResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

TrafficLightResponseEvent::~TrafficLightResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* TrafficLightResponseEvent::cloneEventForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW TrafficLightResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficLightNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficLightNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficLightNotificationEvent::TrafficLightNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

TrafficLightNotificationEvent::~TrafficLightNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/TrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////
