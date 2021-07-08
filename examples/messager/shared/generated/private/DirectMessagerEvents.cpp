//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     04.07.2021  04:30:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectMessagerEvents.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/DirectMessagerEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// DirectMessagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectMessagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( DirectMessagerRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// DirectMessagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectMessagerRequestEvent::DirectMessagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

DirectMessagerRequestEvent::DirectMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEDirectMessager::getString( static_cast<NEDirectMessager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

DirectMessagerRequestEvent::DirectMessagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

DirectMessagerRequestEvent::~DirectMessagerRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectMessagerNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectMessagerNotifyRequestEvent::DirectMessagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

DirectMessagerNotifyRequestEvent::DirectMessagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

DirectMessagerNotifyRequestEvent::~DirectMessagerNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectMessagerResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// DirectMessagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEDirectMessager::getString(static_cast<NEDirectMessager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
DirectMessagerResponseEvent::~DirectMessagerResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* DirectMessagerResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW DirectMessagerResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectMessagerNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// DirectMessagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectMessagerNotificationEvent::DirectMessagerNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

DirectMessagerNotificationEvent::~DirectMessagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/DirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
