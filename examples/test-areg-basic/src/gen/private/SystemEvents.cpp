//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/SystemEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/SystemEvents.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/SystemEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// SystemRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   SystemRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( SystemRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SystemRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemRequestEvent::SystemRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

SystemRequestEvent::SystemRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NESystem::getString( static_cast<NESystem::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


SystemRequestEvent::~SystemRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// SystemNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemNotifyRequestEvent::SystemNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


SystemNotifyRequestEvent::~SystemNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// SystemResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemResponseEvent::SystemResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

SystemResponseEvent::SystemResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NESystem::getString(static_cast<NESystem::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

SystemResponseEvent::SystemResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

SystemResponseEvent::~SystemResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// SystemResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* SystemResponseEvent::cloneEventForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW SystemResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// SystemNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// SystemNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( SystemNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// SystemNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

SystemNotificationEvent::SystemNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

SystemNotificationEvent::~SystemNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/SystemEvents.cpp file
//////////////////////////////////////////////////////////////////////////
