//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationEvents.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:12 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/private/PatientInformationEvents.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface event classes implementation.
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
}

PatientInformationRequestEvent::PatientInformationRequestEvent( const EventDataStream & args, const ProxyAddress & fromSource, const StubAddress & toTarget, unsigned int reqId )
    : RemoteRequestEvent( args, fromSource, toTarget, reqId, NEPatientInformation::getString( static_cast<NEPatientInformation::eMessageIDs>(reqId) ) )
{
}

PatientInformationRequestEvent::PatientInformationRequestEvent( const IEInStream & stream )
    : RemoteRequestEvent( stream )
{
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
}

PatientInformationNotifyRequestEvent::PatientInformationNotifyRequestEvent( const IEInStream & stream )
    : RemoteNotifyRequestEvent( stream )
{
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
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const EventDataStream & args, const ProxyAddress & proxyTarget, NEService::eResultType result, unsigned int responseId, unsigned int seqNr /* = NEService::SEQUENCE_NUMBER_NOTIFY */ )
    : RemoteResponseEvent( args, proxyTarget, result, responseId, seqNr, NEPatientInformation::getString(static_cast<NEPatientInformation::eMessageIDs>(responseId)) )
{
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const ProxyAddress & proxyTarget, const RemoteResponseEvent & source )
    : RemoteResponseEvent( proxyTarget, source )
{
}

PatientInformationResponseEvent::PatientInformationResponseEvent( const IEInStream & stream )
    : RemoteResponseEvent( stream )
{
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
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationEvents.cpp file
//////////////////////////////////////////////////////////////////////////
