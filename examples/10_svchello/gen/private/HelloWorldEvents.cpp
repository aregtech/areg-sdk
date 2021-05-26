//////////////////////////////////////////////////////////////////////////
// Begin generate gen/private/HelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     25.05.2021  23:09:10 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/private/HelloWorldEvents.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "gen/private/HelloWorldEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// HelloWorldRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWorldRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWorldRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( HelloWorldRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWorldRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWorldRequestEvent::HelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

HelloWorldRequestEvent::HelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NEHelloWorld::getString( static_cast<NEHelloWorld::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


HelloWorldRequestEvent::~HelloWorldRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWorldNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWorldNotifyRequestEvent::HelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


HelloWorldNotifyRequestEvent::~HelloWorldNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWorldResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWorldResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWorldResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWorldResponseEvent::HelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

HelloWorldResponseEvent::HelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEHelloWorld::getString(static_cast<NEHelloWorld::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

HelloWorldResponseEvent::HelloWorldResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

HelloWorldResponseEvent::~HelloWorldResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* HelloWorldResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW HelloWorldResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWorldNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWorldNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWorldNotificationEvent::HelloWorldNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

HelloWorldNotificationEvent::~HelloWorldNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate gen/private/HelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////
