//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 * Generated at     11.08.2021  15:53:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PatientInformationProxy.cpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation Service Interface Proxy class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/private/PatientInformationProxy.hpp"
#include "generated/src/private/PatientInformationEvents.hpp"
#include "areg/component/IEProxyListener.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// PatientInformationProxy::PatientInformationServiceAvailableEvent class implementation
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME_EVENT(PatientInformationProxy::PatientInformationServiceAvailableEvent, ProxyBase::ServiceAvailableEvent)

PatientInformationProxy::PatientInformationServiceAvailableEvent::PatientInformationServiceAvailableEvent( IENotificationEventConsumer & consumer )
    : ProxyBase::ServiceAvailableEvent  ( consumer )
{
    ; // do nothing
}

PatientInformationProxy::PatientInformationServiceAvailableEvent::~PatientInformationServiceAvailableEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PatientInformationProxy::_createProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
{
    return DEBUG_NEW PatientInformationProxy(roleName, ownerThread);
}

PatientInformationProxy * PatientInformationProxy::createProxy( const char * roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PatientInformationProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPatientInformation::getInterfaceData()
                                                                      , connectListener
                                                                      , &PatientInformationProxy::_createProxy
                                                                      , ownerThread) );
}

PatientInformationProxy * PatientInformationProxy::createProxy( const char * roleName, IEProxyListener & connectListener, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
{
    return static_cast<PatientInformationProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPatientInformation::getInterfaceData()
                                                                      , connectListener
                                                                      , &PatientInformationProxy::_createProxy
                                                                      , ownerThread) );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PatientInformationProxy::PatientInformationProxy( const char * roleName, DispatcherThread * ownerThread /*= static_cast<DispatcherThread *>(NULL)*/ )
    : ProxyBase(roleName, NEPatientInformation::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mPatient    (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
    ; // do nothing
}

PatientInformationProxy::~PatientInformationProxy( void )
{
    ; // do nothing
}

/************************************************************************
 * Proxy base class overrides.
 ************************************************************************/

NotificationEvent * PatientInformationProxy::createNotificationEvent( const NotificationEventData & data ) const
{
    return DEBUG_NEW PatientInformationNotificationEvent(data);
}

ServiceRequestEvent * PatientInformationProxy::createRequestEvent( const EventDataStream & args, unsigned int reqId )
{
    return DEBUG_NEW PatientInformationRequestEvent(args, getProxyAddress(), getStubAddress(), reqId);
}

ServiceRequestEvent* PatientInformationProxy::createNotificationRequestEvent( unsigned int msgId, NEService::eRequestType reqType )
{
    return DEBUG_NEW PatientInformationNotifyRequestEvent(getProxyAddress(), getStubAddress(), msgId, reqType);
}

RemoteResponseEvent * PatientInformationProxy::createRemoteResponseEvent(const IEInStream & stream) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PatientInformationResponseEvent(stream) );
}

RemoteResponseEvent * PatientInformationProxy::createRemoteRequestFailedEvent(const ProxyAddress & addrProxy, unsigned int msgId, NEService::eResultType reason, unsigned int seqNr) const
{
    return static_cast<RemoteResponseEvent *>( DEBUG_NEW PatientInformationResponseEvent( addrProxy, reason, msgId, seqNr ) );
}

ProxyBase::ServiceAvailableEvent * PatientInformationProxy::createServiceAvailableEvent( IENotificationEventConsumer & consumer )
{
    return static_cast<ProxyBase::ServiceAvailableEvent *>( DEBUG_NEW PatientInformationProxy::PatientInformationServiceAvailableEvent(consumer) );
}

void PatientInformationProxy::registerServiceListeners( void )
{
    ProxyBase::registerServiceListeners( );
    PatientInformationResponseEvent::addListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
}

void PatientInformationProxy::unregisterServiceListeners( void )
{
    PatientInformationResponseEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    PatientInformationProxy::PatientInformationServiceAvailableEvent::removeListener(static_cast<IEEventConsumer &>(self()), mDispatcherThread );
    ProxyBase::unregisterServiceListeners( );
}

/************************************************************************
 * Requests.
 ************************************************************************/
 
/************************************************************************
 * Event processing.
 ************************************************************************/
 
/************************************************************************
 * IEProxyEventConsumer interface overrides.
 ************************************************************************/

void PatientInformationProxy::processResponseEvent( ServiceResponseEvent & eventElem )
{
    PatientInformationResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PatientInformationResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void PatientInformationProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PatientInformationResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PatientInformationResponseEvent );
    if (eventResp != NULL)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

/************************************************************************
 * Internal helper functions.
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_private_PatientInformationProxy_updateData);
void PatientInformationProxy::updateData( PatientInformationResponseEvent & eventElem, NEPatientInformation::eMessageIDs respId )
{
    const IEInStream & stream = static_cast<const PatientInformationResponseEvent &>(eventElem).getData().getReadStream();

    switch (respId)
    {
    /************************************************************************
     * Update Response parameters
     ************************************************************************/
    /************************************************************************
     * Update Broadcast parameters
     ************************************************************************/
    /************************************************************************
     * Update Attribute values
     ************************************************************************/
    case NEPatientInformation::MSG_ID_Patient:
        stream >> mPatient;
        break;

    default:
        {
            TRACE_SCOPE(generated_src_private_PatientInformationProxy_updateData);
            TRACE_ERR("Unexpected response message ID [ %d ] received in proxy [ %s ]!"
                        , static_cast<int>(respId)
                        , ProxyAddress::convAddressToPath(getProxyAddress()).getString());
            ASSERT(false);
        }
        break;
    }
}
 
void PatientInformationProxy::processResponse( PatientInformationResponseEvent & evenElem )
{
    NEPatientInformation::eMessageIDs respId  = static_cast<NEPatientInformation::eMessageIDs>(evenElem.getResponseId());
    NEService::eResultType resultType  = evenElem.getResult();

    bool dataValid  = false;
    bool setStates  = true;

    switch (resultType)
    {
    case NEService::RESULT_DATA_INVALID:
    case NEService::RESULT_INVALID:
        break;  // do nothing

    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_CANCELED:
        respId      = static_cast<NEPatientInformation::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEPatientInformation::MSG_ID_NO_PROCEED;
        break;

    case NEService::RESULT_MESSAGE_UNDELIVERED:
        if ( NEService::isRequestId(respId) )
            respId  = static_cast<NEPatientInformation::eMessageIDs>( mProxyData.getResponseId(static_cast<unsigned int>(respId)) );
        setStates   = respId != NEPatientInformation::MSG_ID_NO_PROCEED;            
        break;

    case NEService::RESULT_OK:
    case NEService::RESULT_DATA_OK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
        updateData(evenElem, respId);
    if (setStates == true)
        setState(static_cast<unsigned int>(respId), dataValid ? NEService::DATA_OK : NEService::DATA_INVALID);

    notifyListeners(static_cast<unsigned int>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationProxy.cpp file
//////////////////////////////////////////////////////////////////////////
 