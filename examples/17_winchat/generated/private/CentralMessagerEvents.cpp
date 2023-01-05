//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/private/CentralMessagerEvents.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/private/CentralMessagerEvents.hpp"

//////////////////////////////////////////////////////////////////////////
// CentralMessagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CentralMessagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CentralMessagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CentralMessagerRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CentralMessagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CentralMessagerRequestEvent::CentralMessagerRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
}

CentralMessagerRequestEvent::CentralMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NECentralMessager::getString( static_cast<NECentralMessager::eMessageIDs>(reqId) ) )
{
}

CentralMessagerRequestEvent::CentralMessagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CentralMessagerNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CentralMessagerNotifyRequestEvent::CentralMessagerNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
}

CentralMessagerNotifyRequestEvent::CentralMessagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CentralMessagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CentralMessagerResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CentralMessagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NECentralMessager::getString(static_cast<NECentralMessager::eMessageIDs>(responseId)) )
{
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* CentralMessagerResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW CentralMessagerResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CentralMessagerNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CentralMessagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CentralMessagerNotificationEvent::CentralMessagerNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/CentralMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
