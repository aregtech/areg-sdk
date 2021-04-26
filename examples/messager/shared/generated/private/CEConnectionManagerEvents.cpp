//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/CEConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source ConnectionManager.
 * Generated at     25.04.2021  23:50:44 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/CEConnectionManagerEvents.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "shared/generated/private/CEConnectionManagerEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEConnectionManagerRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CEConnectionManagerRequestEvent, CERemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEConnectionManagerRequestEvent::CEConnectionManagerRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CEConnectionManagerRequestEvent::CEConnectionManagerRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CERemoteRequestEvent( args, fromSource, toTarget, reqId, NEConnectionManager::GetString( static_cast<NEConnectionManager::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

CEConnectionManagerRequestEvent::CEConnectionManagerRequestEvent( const IEInStream & stream )
    : CERemoteRequestEvent( stream )
{
    ; // do nothing
}    

CEConnectionManagerRequestEvent::~CEConnectionManagerRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEConnectionManagerNotifyRequestEvent, CERemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEConnectionManagerNotifyRequestEvent::CEConnectionManagerNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : CERemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

CEConnectionManagerNotifyRequestEvent::CEConnectionManagerNotifyRequestEvent( const IEInStream & stream )
    : CERemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

CEConnectionManagerNotifyRequestEvent::~CEConnectionManagerNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEConnectionManagerResponseEvent, CERemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEConnectionManagerResponseEvent::CEConnectionManagerResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CEConnectionManagerResponseEvent::CEConnectionManagerResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CERemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEConnectionManager::GetString(static_cast<NEConnectionManager::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CEConnectionManagerResponseEvent::CEConnectionManagerResponseEvent( const CEProxyAddress & proxyTarget, const CERemoteResponseEvent & source )
    : CERemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CEConnectionManagerResponseEvent::CEConnectionManagerResponseEvent( const IEInStream & stream )
    : CERemoteResponseEvent( stream )
{
    ; // do nothing
}    
CEConnectionManagerResponseEvent::~CEConnectionManagerResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CEConnectionManagerResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CEConnectionManagerResponseEvent(target, static_cast<const CERemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEConnectionManagerNotificationEvent, CENotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CEConnectionManagerNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEConnectionManagerNotificationEvent::CEConnectionManagerNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CEConnectionManagerNotificationEvent::~CEConnectionManagerNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/CEConnectionManagerEvents.cpp file
//////////////////////////////////////////////////////////////////////////
