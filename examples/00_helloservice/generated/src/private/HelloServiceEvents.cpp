//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloServiceEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     20.10.2021  21:36:19 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/private/HelloServiceEvents.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/HelloServiceEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// HelloServiceRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloServiceRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloServiceRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( HelloServiceRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloServiceRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloServiceRequestEvent::HelloServiceRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

HelloServiceRequestEvent::HelloServiceRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEHelloService::getString( static_cast<NEHelloService::eMessageIDs>(reqId) ) )
{
}

HelloServiceRequestEvent::HelloServiceRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}    

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloServiceNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloServiceNotifyRequestEvent::HelloServiceNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

HelloServiceNotifyRequestEvent::HelloServiceNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}    

//////////////////////////////////////////////////////////////////////////
// HelloServiceResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloServiceResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloServiceResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// HelloServiceResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloServiceResponseEvent::HelloServiceResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

HelloServiceResponseEvent::HelloServiceResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEHelloService::getString(static_cast<NEHelloService::eMessageIDs>(responseId)) )
{
}

HelloServiceResponseEvent::HelloServiceResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

HelloServiceResponseEvent::HelloServiceResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}    

//////////////////////////////////////////////////////////////////////////
// HelloServiceResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* HelloServiceResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW HelloServiceResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloServiceNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// HelloServiceNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloServiceNotificationEvent::HelloServiceNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloServiceEvents.cpp file
//////////////////////////////////////////////////////////////////////////
