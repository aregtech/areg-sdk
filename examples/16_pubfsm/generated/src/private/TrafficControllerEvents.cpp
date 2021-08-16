//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficController.
 * Generated at     15.08.2021  00:03:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/TrafficControllerEvents.cpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/TrafficControllerEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// TrafficControllerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficControllerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   TrafficControllerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( TrafficControllerRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficControllerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficControllerRequestEvent::TrafficControllerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

TrafficControllerRequestEvent::TrafficControllerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NETrafficController::getString( static_cast<NETrafficController::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

TrafficControllerRequestEvent::TrafficControllerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

TrafficControllerRequestEvent::~TrafficControllerRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficControllerNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficControllerNotifyRequestEvent::TrafficControllerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

TrafficControllerNotifyRequestEvent::TrafficControllerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

TrafficControllerNotifyRequestEvent::~TrafficControllerNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficControllerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficControllerResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficControllerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NETrafficController::getString(static_cast<NETrafficController::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
TrafficControllerResponseEvent::~TrafficControllerResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* TrafficControllerResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW TrafficControllerResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( TrafficControllerNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// TrafficControllerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

TrafficControllerNotificationEvent::TrafficControllerNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

TrafficControllerNotificationEvent::~TrafficControllerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/TrafficControllerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
