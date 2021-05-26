//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CentralMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 * Generated at     23.05.2021  00:18:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CentralMessagerEvents.cpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/CentralMessagerEvents.hpp"
 
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
    ; // do nothing
}

CentralMessagerRequestEvent::CentralMessagerRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NECentralMessager::getString( static_cast<NECentralMessager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

CentralMessagerRequestEvent::CentralMessagerRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

CentralMessagerRequestEvent::~CentralMessagerRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

CentralMessagerNotifyRequestEvent::CentralMessagerNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

CentralMessagerNotifyRequestEvent::~CentralMessagerNotifyRequestEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NECentralMessager::getString(static_cast<NECentralMessager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CentralMessagerResponseEvent::CentralMessagerResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
CentralMessagerResponseEvent::~CentralMessagerResponseEvent( void )
{
    ; // do nothing
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
    ; // do nothing
}

CentralMessagerNotificationEvent::~CentralMessagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CentralMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
