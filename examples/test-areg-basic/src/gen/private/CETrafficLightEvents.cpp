//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CETrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source TrafficLight.
 * Generated at     21.04.2021  18:33:59 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CETrafficLightEvents.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/CETrafficLightEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CETrafficLightRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETrafficLightRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CETrafficLightRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CETrafficLightRequestEvent, CELocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CETrafficLightRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CETrafficLightRequestEvent::CETrafficLightRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CETrafficLightRequestEvent::CETrafficLightRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( args, fromSource, toTarget, reqId, NETrafficLight::GetString( static_cast<NETrafficLight::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


CETrafficLightRequestEvent::~CETrafficLightRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CETrafficLightNotifyRequestEvent, CELocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CETrafficLightNotifyRequestEvent::CETrafficLightNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : CELocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


CETrafficLightNotifyRequestEvent::~CETrafficLightNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETrafficLightResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CETrafficLightResponseEvent, CELocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CETrafficLightResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CETrafficLightResponseEvent::CETrafficLightResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CETrafficLightResponseEvent::CETrafficLightResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NETrafficLight::GetString(static_cast<NETrafficLight::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CETrafficLightResponseEvent::CETrafficLightResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source )
    : CELocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CETrafficLightResponseEvent::~CETrafficLightResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CETrafficLightResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CETrafficLightResponseEvent(target, static_cast<const CELocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CETrafficLightNotificationEvent, CENotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CETrafficLightNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CETrafficLightNotificationEvent::CETrafficLightNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CETrafficLightNotificationEvent::~CETrafficLightNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CETrafficLightEvents.cpp file
//////////////////////////////////////////////////////////////////////////
