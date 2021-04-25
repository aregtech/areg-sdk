//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEDirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2019
 *                  Create by AREGtech code generator tool from source DirectMessager.
 * Generated at     03.09.2019  02:48:11 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEDirectMessagerEvents.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/CEDirectMessagerEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEDirectMessagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CEDirectMessagerRequestEvent, CERemoteRequestEvent );

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectMessagerRequestEvent::CEDirectMessagerRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId )
    : CERemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CEDirectMessagerRequestEvent::CEDirectMessagerRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, const unsigned int reqId )
    : CERemoteRequestEvent( args, fromSource, toTarget, reqId, NEDirectMessager::GetString( static_cast<const NEDirectMessager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

CEDirectMessagerRequestEvent::CEDirectMessagerRequestEvent( const IEInStream & stream )
    : CERemoteRequestEvent( stream )
{
    ; // do nothing
}    

CEDirectMessagerRequestEvent::~CEDirectMessagerRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectMessagerNotifyRequestEvent, CERemoteNotifyRequestEvent );

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectMessagerNotifyRequestEvent::CEDirectMessagerNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, const unsigned int msgId, const NEService::eRequestType & reqType )
    : CERemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

CEDirectMessagerNotifyRequestEvent::CEDirectMessagerNotifyRequestEvent( const IEInStream & stream )
    : CERemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

CEDirectMessagerNotifyRequestEvent::~CEDirectMessagerNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectMessagerResponseEvent, CERemoteResponseEvent );

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectMessagerResponseEvent::CEDirectMessagerResponseEvent( const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CEDirectMessagerResponseEvent::CEDirectMessagerResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, const NEService::eResultType & result, const unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEDirectMessager::GetString(static_cast<const NEDirectMessager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CEDirectMessagerResponseEvent::CEDirectMessagerResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source )
    : CERemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CEDirectMessagerResponseEvent::CEDirectMessagerResponseEvent( const IEInStream & stream )
    : CERemoteResponseEvent( stream )
{
    ; // do nothing
}    
CEDirectMessagerResponseEvent::~CEDirectMessagerResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CEDirectMessagerResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CEDirectMessagerResponseEvent(target, static_cast<const CERemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEDirectMessagerNotificationEvent, CENotificationEvent );

//////////////////////////////////////////////////////////////////////////
// CEDirectMessagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEDirectMessagerNotificationEvent::CEDirectMessagerNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CEDirectMessagerNotificationEvent::~CEDirectMessagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEDirectMessagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
