//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/CEMessageOutputEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREGtech code generator tool from source MessageOutput.
 * Generated at     21.04.2021  18:33:55 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/CEMessageOutputEvents.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "src/gen/private/CEMessageOutputEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// CEMessageOutputRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   CEMessageOutputRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( CEMessageOutputRequestEvent, CELocalRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEMessageOutputRequestEvent::CEMessageOutputRequestEvent( const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

CEMessageOutputRequestEvent::CEMessageOutputRequestEvent( const CEEventDataStream & args, const CEProxyAddress & fromSource, const CEStubAddress & toTarget, unsigned int reqId )
    : CELocalRequestEvent( args, fromSource, toTarget, reqId, NEMessageOutput::GetString( static_cast<NEMessageOutput::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}


CEMessageOutputRequestEvent::~CEMessageOutputRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEMessageOutputNotifyRequestEvent, CELocalNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEMessageOutputNotifyRequestEvent::CEMessageOutputNotifyRequestEvent( const CEProxyAddress & fromProxy, const CEStubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : CELocalNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}


CEMessageOutputNotifyRequestEvent::~CEMessageOutputNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEMessageOutputResponseEvent, CELocalResponseEvent )

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEMessageOutputResponseEvent::CEMessageOutputResponseEvent( const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

CEMessageOutputResponseEvent::CEMessageOutputResponseEvent( const CEEventDataStream & args, const CEProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : CELocalResponseEvent( args, proxyTarget, result, responseId, seqNr, NEMessageOutput::GetString(static_cast<NEMessageOutput::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

CEMessageOutputResponseEvent::CEMessageOutputResponseEvent( const CEProxyAddress & proxyTarget, const CELocalResponseEvent & source )
    : CELocalResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

CEMessageOutputResponseEvent::~CEMessageOutputResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

CEServiceResponseEvent* CEMessageOutputResponseEvent::CloneEventForTarget( const CEProxyAddress & target ) const
{
    return static_cast<CEServiceResponseEvent *>( DEBUG_NEW CEMessageOutputResponseEvent(target, static_cast<const CELocalResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( CEMessageOutputNotificationEvent, CENotificationEvent )

//////////////////////////////////////////////////////////////////////////
// CEMessageOutputNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEMessageOutputNotificationEvent::CEMessageOutputNotificationEvent( const CENotificationEventData & data )
    : CENotificationEvent   ( data )
{
    ; // do nothing
}

CEMessageOutputNotificationEvent::~CEMessageOutputNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/CEMessageOutputEvents.cpp file
//////////////////////////////////////////////////////////////////////////
