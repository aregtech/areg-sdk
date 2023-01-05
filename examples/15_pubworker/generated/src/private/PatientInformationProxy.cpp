//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationProxy.cpp file
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:24 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/private/PatientInformationProxy.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface Proxy class implementation.
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
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationProxy class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// static function implementation
//////////////////////////////////////////////////////////////////////////

ProxyBase * PatientInformationProxy::_createProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
{
    return DEBUG_NEW PatientInformationProxy(roleName, ownerThread);
}

PatientInformationProxy * PatientInformationProxy::createProxy( const String & roleName, IEProxyListener & connectListener, DispatcherThread & ownerThread )
{
    return static_cast<PatientInformationProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPatientInformation::getInterfaceData()
                                                                      , connectListener
                                                                      , &PatientInformationProxy::_createProxy
                                                                      , ownerThread).get() );
}

PatientInformationProxy * PatientInformationProxy::createProxy( const String & roleName, IEProxyListener & connectListener, const String & ownerThread /*= String::EmptyString*/ )
{
    return static_cast<PatientInformationProxy *>(ProxyBase::findOrCreateProxy( roleName
                                                                      , NEPatientInformation::getInterfaceData()
                                                                      , connectListener
                                                                      , &PatientInformationProxy::_createProxy
                                                                      , ownerThread).get() );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
PatientInformationProxy::PatientInformationProxy( const String & roleName, DispatcherThread * ownerThread /*= nullptr*/ )
    : ProxyBase(roleName, NEPatientInformation::getInterfaceData(), ownerThread)

/************************************************************************
 * Attributes
 ************************************************************************/
    , mPatient    (  )

/************************************************************************
 * Parameters
 ************************************************************************/
{
}

PatientInformationProxy::~PatientInformationProxy( void )
{
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
    if (eventResp != nullptr)
        processResponse( *eventResp );
    else
        processProxyEvent( eventElem );
}

void PatientInformationProxy::processAttributeEvent( ServiceResponseEvent & eventElem )
{
    PatientInformationResponseEvent * eventResp = RUNTIME_CAST( &eventElem, PatientInformationResponseEvent );
    if (eventResp != nullptr)
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
    case NEPatientInformation::eMessageIDs::MsgId_Patient:
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
    case NEService::eResultType::DataInvalid:   // fall through
    case NEService::eResultType::RequestInvalid:
        break;  // do nothing

    case NEService::eResultType::RequestError:  // fall through
    case NEService::eResultType::RequestBusy:   // fall through
    case NEService::eResultType::RequestCanceled:
        respId      = static_cast<NEPatientInformation::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        setStates   = respId != NEPatientInformation::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::MessageUndelivered:
        if ( NEService::isRequestId( static_cast<msg_id>(respId) ) )
        {
            respId  = static_cast<NEPatientInformation::eMessageIDs>( mProxyData.getResponseId(static_cast<msg_id>(respId)) );
        }

        setStates   = respId != NEPatientInformation::eMessageIDs::MsgId_NotProcessed;
        break;

    case NEService::eResultType::RequestOK:     // fall through
    case NEService::eResultType::DataOK:
        dataValid   = true;
        break;

    default:
        setStates   = false;
        break;
    }

    if (dataValid == true)
    {
        updateData(evenElem, respId);
    }

    if (setStates == true)
    {
        setState(static_cast<msg_id>(respId), dataValid ? NEService::eDataStateType::DataIsOK : NEService::eDataStateType::DataIsInvalid);
    }

    notifyListeners(static_cast<msg_id>(respId), resultType, evenElem.getSequenceNumber());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationProxy.cpp file
//////////////////////////////////////////////////////////////////////////
