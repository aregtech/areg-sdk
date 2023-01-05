//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/TrafficControllerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:40 GMT+01:00
 *                  Create by AREG SDK code generator tool from source TrafficController.
 *
 * \file            generated/src/private/TrafficControllerEvents.hpp
 * \ingroup         TrafficController Service Interface
 * \brief           This is an automatic generated code of TrafficController
 *                  Service Interface event classes implementation.
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
}

TrafficControllerRequestEvent::TrafficControllerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NETrafficController::getString( static_cast<NETrafficController::eMessageIDs>(reqId) ) )
{
}

TrafficControllerRequestEvent::TrafficControllerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
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
}

TrafficControllerNotifyRequestEvent::TrafficControllerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
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
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NETrafficController::getString(static_cast<NETrafficController::eMessageIDs>(responseId)) )
{
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

TrafficControllerResponseEvent::TrafficControllerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
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
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/TrafficControllerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
