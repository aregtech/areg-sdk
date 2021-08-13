//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 * Generated at     11.08.2021  15:53:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PatientInformationEvents.cpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation Service Interface Event classes implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
 #include "generated/src/private/PatientInformationEvents.hpp"
 
//////////////////////////////////////////////////////////////////////////
// PatientInformationRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInformationRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   PatientInformationRequestEvent Event runtime implementation
 **/
IMPLEMENT_RUNTIME_EVENT( PatientInformationRequestEvent, RemoteRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PatientInformationRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PatientInformationRequestEvent::PatientInformationRequestEvent( const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( fromSource, toTarget, reqId )
{
    ; // do nothing
}

PatientInformationRequestEvent::PatientInformationRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEPatientInformation::getString( static_cast<NEPatientInformation::eMessageIDs>(reqId) ) )
{
    ; // do nothing
}

PatientInformationRequestEvent::PatientInformationRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
    ; // do nothing
}    

PatientInformationRequestEvent::~PatientInformationRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotifyRequestEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotifyRequestEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PatientInformationNotifyRequestEvent, RemoteNotifyRequestEvent )

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotifyRequestEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PatientInformationNotifyRequestEvent::PatientInformationNotifyRequestEvent( const ProxyAddress & fromProxy, const StubAddress & toStub, unsigned int msgId, NEService::eRequestType reqType )
    : RemoteNotifyRequestEvent( fromProxy, toStub, msgId, reqType )
{
    ; // do nothing
}

PatientInformationNotifyRequestEvent::PatientInformationNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
    ; // do nothing
}    

PatientInformationNotifyRequestEvent::~PatientInformationNotifyRequestEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationResponseEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInformationResponseEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PatientInformationResponseEvent, RemoteResponseEvent )

//////////////////////////////////////////////////////////////////////////
// PatientInformationResponseEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PatientInformationResponseEvent::PatientInformationResponseEvent( const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( proxyTarget, result, responseId, seqNr )
{
    ; // do nothing
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPatientInformation::getString(static_cast<NEPatientInformation::eMessageIDs>(responseId)) )
{
    ; // do nothing
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
    ; // do nothing
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
    ; // do nothing
}    
PatientInformationResponseEvent::~PatientInformationResponseEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationResponseEvent class, Methods
//////////////////////////////////////////////////////////////////////////

ServiceResponseEvent* PatientInformationResponseEvent::cloneForTarget( const ProxyAddress & target ) const
{
    return static_cast<ServiceResponseEvent *>( DEBUG_NEW PatientInformationResponseEvent(target, static_cast<const RemoteResponseEvent &>(*this)) );
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotificationEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT( PatientInformationNotificationEvent, NotificationEvent )

//////////////////////////////////////////////////////////////////////////
// PatientInformationNotificationEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

PatientInformationNotificationEvent::PatientInformationNotificationEvent( const NotificationEventData & data )
    : NotificationEvent   ( data )
{
    ; // do nothing
}

PatientInformationNotificationEvent::~PatientInformationNotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationEvents.cpp file
//////////////////////////////////////////////////////////////////////////
