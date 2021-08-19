//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PowerManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source PowerManager.
 * Generated at     15.08.2021  00:03:03 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/PowerManagerClientBase.cpp
 * \ingroup         PowerManager Service Interface
 * \brief           This is an automatic generated code of PowerManager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/PowerManagerClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEPowerManager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (PowerManagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &PowerManagerClientBase::requestPowerOnFailed
        , &PowerManagerClientBase::requestPowerOffFailed
        , &PowerManagerClientBase::requestStartTrafficLightFailed
        , &PowerManagerClientBase::requestStopTrafficLightFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// PowerManagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

PowerManagerClientBase::PowerManagerClientBase( const char * roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PowerManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

PowerManagerClientBase::PowerManagerClientBase( const char * roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PowerManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

PowerManagerClientBase::PowerManagerClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PowerManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

PowerManagerClientBase::~PowerManagerClientBase( void )
{
    if (mProxy != NULL)
    {
        mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
        mProxy->freeProxy( static_cast<IEProxyListener &>(self()) );
        mProxy  = NULL;
    }
    
    mIsConnected= false;
}

/************************************************************************
 * Operations
 ************************************************************************/

bool PowerManagerClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != NULL)
    {
        String roleName   = mProxy->getProxyAddress().getRoleName();
        String threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            PowerManagerProxy * newProxy = PowerManagerProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
            if (newProxy != NULL)
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

DispatcherThread * PowerManagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<PowerManagerProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_serviceConnected);
bool PowerManagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_serviceConnected);
    
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

void PowerManagerClientBase::notifyOn( NEPowerManager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_processNotificationEvent);
void PowerManagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEPowerManager::eMessageIDs msgId   = static_cast<NEPowerManager::eMessageIDs>(data.getNotifyId());
    mCurrSequenceNr = data.getSequenceNr();

    switch (result)
    {
    case NEService::RESULT_REQUEST_BUSY:
    case NEService::RESULT_REQUEST_ERROR:
    case NEService::RESULT_REQUEST_CANCELED:
        {
        /************************************************************************
         * Trigger request error handling if request failed.
         ************************************************************************/
            PowerManagerClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            PowerManagerClientBase::invalidResponse(msgId);
        }
        break;

    case NEService::RESULT_DATA_OK:
    case NEService::RESULT_DATA_INVALID:
        {
            NEService::eDataStateType dataState;
            switch (msgId)
            {
        /************************************************************************
         * Trigger attribute update processing
         ************************************************************************/
            case NEPowerManager::MSG_ID_LightsPowerState:
                {
                    NEPowerManager::ePoweredState LightsPowerState = mProxy->getLightsPowerState(dataState);
                    onLightsPowerStateUpdate(LightsPowerState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_PowerManagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PowerManagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::RESULT_OK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
            case NEPowerManager::MSG_ID_responseStartTrafficLight:
                {
                    bool Success = mProxy->getParamSuccess();
                    responseStartTrafficLight( Success );
                }
                break;

            case NEPowerManager::MSG_ID_responseStopTrafficLight:
                {
                    bool Success = mProxy->getParamSuccess();
                    responseStopTrafficLight( Success );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_PowerManagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PowerManagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }        
        break;

    default:
        {
            TRACE_SCOPE(generated_src_PowerManagerClientBase_processNotificationEvent);
            TRACE_ERR("Client object PowerManagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_invalidResponse);
void PowerManagerClientBase::invalidResponse( NEPowerManager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client PowerManagerClientBase is not implemented! Make error handling!"
                    , NEPowerManager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_invalidRequest);
void PowerManagerClientBase::invalidRequest( NEPowerManager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PowerManagerClientBase is not implemented! Make error handling!"
                    , NEPowerManager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_requestFailed);
void PowerManagerClientBase::requestFailed( NEPowerManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PowerManagerClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEPowerManager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEPowerManager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEPowerManager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEPowerManager::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEPowerManager::MSG_ID_INVALID) )
    {
        (this->*NEPowerManager::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_onLightsPowerStateUpdate);
void PowerManagerClientBase::onLightsPowerStateUpdate( NEPowerManager::ePoweredState /* LightsPowerState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_onLightsPowerStateUpdate);
    TRACE_WARN("The attribute LightsPowerState (value = %u) update method of proxy [ %s ] client PowerManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_LightsPowerState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_requestPowerOnFailed);
void PowerManagerClientBase::requestPowerOnFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_requestPowerOnFailed);
    TRACE_WARN("The request requestPowerOn (value = %u) method of proxy [ %s ] client PowerManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_requestPowerOn)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_requestPowerOffFailed);
void PowerManagerClientBase::requestPowerOffFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_requestPowerOffFailed);
    TRACE_WARN("The request requestPowerOff (value = %u) method of proxy [ %s ] client PowerManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_requestPowerOff)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_requestStartTrafficLightFailed);
void PowerManagerClientBase::requestStartTrafficLightFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_requestStartTrafficLightFailed);
    TRACE_WARN("The request requestStartTrafficLight (value = %u) method of proxy [ %s ] client PowerManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_requestStartTrafficLight)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_requestStopTrafficLightFailed);
void PowerManagerClientBase::requestStopTrafficLightFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_requestStopTrafficLightFailed);
    TRACE_WARN("The request requestStopTrafficLight (value = %u) method of proxy [ %s ] client PowerManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_requestStopTrafficLight)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_responseStartTrafficLight);
void PowerManagerClientBase::responseStartTrafficLight( bool /* Success */ )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_responseStartTrafficLight);
    TRACE_WARN("The response responseStartTrafficLight (value = %u) method of proxy [ %s ] client PowerManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_responseStartTrafficLight)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PowerManagerClientBase_responseStopTrafficLight);
void PowerManagerClientBase::responseStopTrafficLight( bool /* Success */ )
{
    TRACE_SCOPE(generated_src_PowerManagerClientBase_responseStopTrafficLight);
    TRACE_WARN("The response responseStopTrafficLight (value = %u) method of proxy [ %s ] client PowerManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPowerManager::MSG_ID_responseStopTrafficLight)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PowerManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
