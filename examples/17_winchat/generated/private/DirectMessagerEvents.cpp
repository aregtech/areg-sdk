//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/private/DirectMessagerEvents.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/private/DirectMessagerEvents.hpp"

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
}

DirectMessagerRequestEvent::DirectMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEDirectMessager::getString( static_cast<NEDirectMessager::eMessageIDs>(reqId) ) )
{
}

DirectMessagerRequestEvent::DirectMessagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
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
}

DirectMessagerNotifyRequestEvent::DirectMessagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
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
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEDirectMessager::getString(static_cast<NEDirectMessager::eMessageIDs>(responseId)) )
{
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

DirectMessagerResponseEvent::DirectMessagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
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
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
