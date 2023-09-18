//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:48 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 *
 * \file            generated/src/private/LocalHelloWorldEvents.hpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/LocalHelloWorldEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   LocalHelloWorldRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( LocalHelloWorldRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LocalHelloWorldRequestEvent::LocalHelloWorldRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
}

LocalHelloWorldRequestEvent::LocalHelloWorldRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NELocalHelloWorld::getString( static_cast<NELocalHelloWorld::eMessageIDs>(reqId) ) )
{
}


//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LocalHelloWorldNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LocalHelloWorldNotifyRequestEvent::LocalHelloWorldNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}


//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LocalHelloWorldResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LocalHelloWorldResponseEvent::LocalHelloWorldResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

LocalHelloWorldResponseEvent::LocalHelloWorldResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, const SequenceNumber &  seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NELocalHelloWorld::getString(static_cast<NELocalHelloWorld::eMessageIDs>(responseId)) )
{
}

LocalHelloWorldResponseEvent::LocalHelloWorldResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
}


//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* LocalHelloWorldResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW LocalHelloWorldResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( LocalHelloWorldNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

LocalHelloWorldNotificationEvent::LocalHelloWorldNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldEvents.cpp file
//////////////////////////////////////////////////////////////////////////
