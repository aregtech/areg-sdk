//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PatientInformationClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:09:24 GMT+01:00
 *                  Create by AREG SDK code generator tool from source PatientInformation.
 *
 * \file            generated/src/PatientInformationClientBase.hpp
 * \ingroup         PatientInformation Service Interface
 * \brief           This is an automatic generated code of PatientInformation
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/PatientInformationClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEPatientInformation
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (PatientInformationClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// PatientInformationClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

PatientInformationClientBase::PatientInformationClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PatientInformationProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PatientInformationClientBase::PatientInformationClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PatientInformationProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PatientInformationClientBase::PatientInformationClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PatientInformationProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PatientInformationClientBase::PatientInformationClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PatientInformationProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PatientInformationClientBase::~PatientInformationClientBase( void )
{
    if (mProxy != nullptr)
    {
        mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
        mProxy->freeProxy( static_cast<IEProxyListener &>(self()) );
        mProxy  = nullptr;
    }

    mIsConnected= false;
}

/************************************************************************
 * Operations
 ************************************************************************/

bool PatientInformationClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            PatientInformationProxy * newProxy = PatientInformationProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
            if (newProxy != nullptr)
            {
                mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
                mProxy->freeProxy (static_cast<IEProxyListener &>(self()) );
                mProxy = newProxy;
                result = true;
            }
        }
    }

    return result;
}

DispatcherThread * PatientInformationClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_serviceConnected);
bool PatientInformationClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_PatientInformationClientBase_serviceConnected);

    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("Client [ %p ]: The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , this
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , isConnected ? "CONNECTED" : "DISCONNECTED");

        mIsConnected= isConnected;
        result      = true;
    }

    return result;
}

void PatientInformationClientBase::notifyOn( NEPatientInformation::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_processNotificationEvent);
void PatientInformationClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEPatientInformation::eMessageIDs msgId   = static_cast<NEPatientInformation::eMessageIDs>(data.getNotifyId());
    mCurrSequenceNr = data.getSequenceNr();

    switch (result)
    {
    case NEService::eResultType::RequestBusy:       // fall through
    case NEService::eResultType::RequestError:      // fall through
    case NEService::eResultType::RequestCanceled:   // fall through
        {
        /************************************************************************
         * Trigger request error handling if request failed.
         ************************************************************************/
            PatientInformationClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            PatientInformationClientBase::invalidResponse(msgId);
        }
        break;

    case NEService::eResultType::DataOK:            // fall through
    case NEService::eResultType::DataInvalid:
        {
            NEService::eDataStateType dataState;
            switch (msgId)
            {
        /************************************************************************
         * Trigger attribute update processing
         ************************************************************************/
            case NEPatientInformation::eMessageIDs::MsgId_Patient:
                {
                    const NEPatientInformation::PatientInfo & Patient = mProxy->getPatient(dataState);
                    onPatientUpdate(Patient, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_PatientInformationClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PatientInformationClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::eResultType::RequestOK:        
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PatientInformationClientBase_processNotificationEvent);
            TRACE_ERR("Client object PatientInformationClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
                        , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                        , static_cast<int>(result));
            ASSERT(false);
        }
        break;
    }
}

/************************************************************************
 * Error handling calls
 ************************************************************************/
DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_invalidResponse);
void PatientInformationClientBase::invalidResponse( NEPatientInformation::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_PatientInformationClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client PatientInformationClientBase is not implemented! Make error handling!"
                    , NEPatientInformation::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_invalidRequest);
void PatientInformationClientBase::invalidRequest( NEPatientInformation::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_PatientInformationClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PatientInformationClientBase is not implemented! Make error handling!"
                    , NEPatientInformation::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_requestFailed);
void PatientInformationClientBase::requestFailed( NEPatientInformation::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PatientInformationClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PatientInformationClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEPatientInformation::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NEPatientInformation::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PatientInformationClientBase_onPatientUpdate);
void PatientInformationClientBase::onPatientUpdate( const NEPatientInformation::PatientInfo & /* Patient */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PatientInformationClientBase_onPatientUpdate);
    TRACE_WARN("The attribute Patient (value = %u) update method of proxy [ %s ] client PatientInformationClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPatientInformation::eMessageIDs::MsgId_Patient)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PatientInformationClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
