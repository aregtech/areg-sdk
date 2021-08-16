//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PowerManagerEvents.cpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/PowerManagerEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// PowerManagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PowerManagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PowerManagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( PowerManagerRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PowerManagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PowerManagerRequestEvent::PowerManagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

PowerManagerRequestEvent::PowerManagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NEPowerManager::getString( static_cast<NEPowerManager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


PowerManagerRequestEvent::~PowerManagerRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PowerManagerNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PowerManagerNotifyRequestEvent::PowerManagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


PowerManagerNotifyRequestEvent::~PowerManagerNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PowerManagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PowerManagerResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// PowerManagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PowerManagerResponseEvent::PowerManagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

PowerManagerResponseEvent::PowerManagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPowerManager::getString(static_cast<NEPowerManager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

PowerManagerResponseEvent::PowerManagerResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

PowerManagerResponseEvent::~PowerManagerResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* PowerManagerResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW PowerManagerResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PowerManagerNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// PowerManagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PowerManagerNotificationEvent::PowerManagerNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

PowerManagerNotificationEvent::~PowerManagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PowerManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
