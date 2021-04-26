//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectConnectionEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source DirectConnection.
 * Generated at     25.04.2021  23:50:45 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectConnectionEvents.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/CEDirectConnectionEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectConnectionRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CEDirectConnectionRequestEvent, CERemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectConnectionRequestEvent::CEDirectConnectionRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CEDirectConnectionRequestEvent::CEDirectConnectionRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERemoteRequestEvent( args, fromSource, toTarget, reqId, NEDirectConnection::GetString( static_cast<NEDirectConnection::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

CEDirectConnectionRequestEvent::CEDirectConnectionRequestEvent( const IEInStream & stream )
    : CERemoteRequestEvent( stream )
{
    ; // do nothing
}    

CEDirectConnectionRequestEvent::~CEDirectConnectionRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectConnectionNotifyRequestEvent, CERemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectConnectionNotifyRequestEvent::CEDirectConnectionNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : CERemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

CEDirectConnectionNotifyRequestEvent::CEDirectConnectionNotifyRequestEvent( const IEInStream & stream )
    : CERemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

CEDirectConnectionNotifyRequestEvent::~CEDirectConnectionNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectConnectionResponseEvent, CERemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectConnectionResponseEvent::CEDirectConnectionResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CEDirectConnectionResponseEvent::CEDirectConnectionResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEDirectConnection::GetString(static_cast<NEDirectConnection::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CEDirectConnectionResponseEvent::CEDirectConnectionResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source )
    : CERemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CEDirectConnectionResponseEvent::CEDirectConnectionResponseEvent( const IEInStream & stream )
    : CERemoteResponseEvent( stream )
{
    ; // do nothing
}    
CEDirectConnectionResponseEvent::~CEDirectConnectionResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CEDirectConnectionResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CEDirectConnectionResponseEvent(target, static_cast<const CERemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectConnectionNotificationEvent, CENotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CEDirectConnectionNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectConnectionNotificationEvent::CEDirectConnectionNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CEDirectConnectionNotificationEvent::~CEDirectConnectionNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectConnectionEvents.cpp file
//////////////////////////////////////////////////////////////////////////
