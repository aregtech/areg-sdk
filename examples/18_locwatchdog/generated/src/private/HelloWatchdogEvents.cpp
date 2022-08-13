//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloWatchdogEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:48:18 GMT+02:00
 *                  Create by AREG SDK code generator tool from source HelloWatchdog.
 *
 * \file            generated/src/private/HelloWatchdogEvents.hpp
 * \ingroup         HelloWatchdog Service Interface
 * \brief           This is an automatic generated code of HelloWatchdog
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/HelloWatchdogEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   HelloWatchdogRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( HelloWatchdogRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWatchdogRequestEvent::HelloWatchdogRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
}

HelloWatchdogRequestEvent::HelloWatchdogRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NEHelloWatchdog::getString( static_cast<NEHelloWatchdog::eMessageIDs>(reqId) ) )
{
}


//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWatchdogNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWatchdogNotifyRequestEvent::HelloWatchdogNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}


//////////////////////////////////////////////////////////////////////////
// HelloWatchdogResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWatchdogResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWatchdogResponseEvent::HelloWatchdogResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

HelloWatchdogResponseEvent::HelloWatchdogResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEHelloWatchdog::getString(static_cast<NEHelloWatchdog::eMessageIDs>(responseId)) )
{
}

HelloWatchdogResponseEvent::HelloWatchdogResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
}


//////////////////////////////////////////////////////////////////////////
// HelloWatchdogResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* HelloWatchdogResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW HelloWatchdogResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( HelloWatchdogNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// HelloWatchdogNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

HelloWatchdogNotificationEvent::HelloWatchdogNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloWatchdogEvents.cpp file
//////////////////////////////////////////////////////////////////////////
