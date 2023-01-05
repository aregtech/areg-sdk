//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectConnectionEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/private/DirectConnectionEvents.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/private/DirectConnectionEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// DirectConnectionRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectConnectionRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   DirectConnectionRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( DirectConnectionRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// DirectConnectionRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectConnectionRequestEvent::DirectConnectionRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

DirectConnectionRequestEvent::DirectConnectionRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEDirectConnection::getString( static_cast<NEDirectConnection::eMessageIDs>(reqId) ) )
{
}

DirectConnectionRequestEvent::DirectConnectionRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectConnectionNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectConnectionNotifyRequestEvent::DirectConnectionNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

DirectConnectionNotifyRequestEvent::DirectConnectionNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectConnectionResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectConnectionResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// DirectConnectionResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectConnectionResponseEvent::DirectConnectionResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

DirectConnectionResponseEvent::DirectConnectionResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEDirectConnection::getString(static_cast<NEDirectConnection::eMessageIDs>(responseId)) )
{
}

DirectConnectionResponseEvent::DirectConnectionResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

DirectConnectionResponseEvent::DirectConnectionResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* DirectConnectionResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW DirectConnectionResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( DirectConnectionNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// DirectConnectionNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

DirectConnectionNotificationEvent::DirectConnectionNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectConnectionEvents.cpp file
//////////////////////////////////////////////////////////////////////////
