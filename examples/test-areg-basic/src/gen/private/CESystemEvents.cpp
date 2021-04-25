//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CESystemEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source System.
 * Generated at     21.04.2021  18:33:57 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CESystemEvents.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/CESystemEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CESystemRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESystemRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CESystemRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CESystemRequestEvent, CELocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CESystemRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CESystemRequestEvent::CESystemRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CESystemRequestEvent::CESystemRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( args, fromSource, toTarget, reqId, NESystem::GetString( static_cast<NESystem::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


CESystemRequestEvent::~CESystemRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESystemNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESystemNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CESystemNotifyRequestEvent, CELocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CESystemNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CESystemNotifyRequestEvent::CESystemNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : CELocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


CESystemNotifyRequestEvent::~CESystemNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESystemResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESystemResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CESystemResponseEvent, CELocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CESystemResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CESystemResponseEvent::CESystemResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CESystemResponseEvent::CESystemResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NESystem::GetString(static_cast<NESystem::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CESystemResponseEvent::CESystemResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source )
    : CELocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CESystemResponseEvent::~CESystemResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CESystemResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CESystemResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CESystemResponseEvent(target, static_cast<const CELocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CESystemNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CESystemNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CESystemNotificationEvent, CENotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CESystemNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CESystemNotificationEvent::CESystemNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CESystemNotificationEvent::~CESystemNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CESystemEvents.cpp file
//////////////////////////////////////////////////////////////////////////
