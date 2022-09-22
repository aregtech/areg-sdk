//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/OptionsEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/private/OptionsEvents.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/private/OptionsEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// OptionsRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OptionsRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   OptionsRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( OptionsRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// OptionsRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

OptionsRequestEvent::OptionsRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
}

OptionsRequestEvent::OptionsRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NEOptions::getString( static_cast<NEOptions::eMessageIDs>(reqId) ) )
{
}


//////////////////////////////////////////////////////////////////////////
// OptionsNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OptionsNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( OptionsNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// OptionsNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

OptionsNotifyRequestEvent::OptionsNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}


//////////////////////////////////////////////////////////////////////////
// OptionsResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OptionsResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( OptionsResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// OptionsResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

OptionsResponseEvent::OptionsResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

OptionsResponseEvent::OptionsResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEOptions::getString(static_cast<NEOptions::eMessageIDs>(responseId)) )
{
}

OptionsResponseEvent::OptionsResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
}


//////////////////////////////////////////////////////////////////////////
// OptionsResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* OptionsResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW OptionsResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// OptionsNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OptionsNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( OptionsNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// OptionsNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

OptionsNotificationEvent::OptionsNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/OptionsEvents.cpp file
//////////////////////////////////////////////////////////////////////////
