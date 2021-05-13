//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/MessageOutputEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/MessageOutputEvents.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/MessageOutputEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// MessageOutputRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutputRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   MessageOutputRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( MessageOutputRequestEvent, LocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// MessageOutputRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

MessageOutputRequestEvent::MessageOutputRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

MessageOutputRequestEvent::MessageOutputRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : LocalRequestEvent( args, fromSource, toTarget, reqId, NEMessageOutput::getString( static_cast<NEMessageOutput::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


MessageOutputRequestEvent::~MessageOutputRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( MessageOutputNotifyRequestEvent, LocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

MessageOutputNotifyRequestEvent::MessageOutputNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : LocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


MessageOutputNotifyRequestEvent::~MessageOutputNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutputResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( MessageOutputResponseEvent, LocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// MessageOutputResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

MessageOutputResponseEvent::MessageOutputResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

MessageOutputResponseEvent::MessageOutputResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : LocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEMessageOutput::getString(static_cast<NEMessageOutput::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

MessageOutputResponseEvent::MessageOutputResponseEvent( const ProxyAddress & proxyTarget, const LocalResponseEvent & source )
    : LocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

MessageOutputResponseEvent::~MessageOutputResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* MessageOutputResponseEvent::cloneEventForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW MessageOutputResponseEvent(target, static_cast<const LocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( MessageOutputNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// MessageOutputNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

MessageOutputNotificationEvent::MessageOutputNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

MessageOutputNotificationEvent::~MessageOutputNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/MessageOutputEvents.cpp file
//////////////////////////////////////////////////////////////////////////
