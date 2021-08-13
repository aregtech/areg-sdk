//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/SimpleTrafficSwitchClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source SimpleTrafficSwitch.
 * Generated at     11.08.2021  13:18:21 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/SimpleTrafficSwitchClientBase.cpp
 * \ingroup         SimpleTrafficSwitch Service Interface
 * \brief           This is an automatic generated code of SimpleTrafficSwitch Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/SimpleTrafficSwitchClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NESimpleTrafficSwitch
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (SimpleTrafficSwitchClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &SimpleTrafficSwitchClientBase::requestSwitchLightFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// SimpleTrafficSwitchClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

SimpleTrafficSwitchClientBase::SimpleTrafficSwitchClientBase( const char * roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficSwitchProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

SimpleTrafficSwitchClientBase::SimpleTrafficSwitchClientBase( const char * roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficSwitchProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

SimpleTrafficSwitchClientBase::SimpleTrafficSwitchClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( SimpleTrafficSwitchProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

SimpleTrafficSwitchClientBase::~SimpleTrafficSwitchClientBase( void )
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

bool SimpleTrafficSwitchClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != NULL)
    {
        String roleName   = mProxy->getProxyAddress().getRoleName();
        String threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            SimpleTrafficSwitchProxy * newProxy = SimpleTrafficSwitchProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * SimpleTrafficSwitchClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<SimpleTrafficSwitchProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_serviceConnected);
bool SimpleTrafficSwitchClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_serviceConnected);
    
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

void SimpleTrafficSwitchClientBase::notifyOn( NESimpleTrafficSwitch::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_processNotificationEvent);
void SimpleTrafficSwitchClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NESimpleTrafficSwitch::eMessageIDs msgId   = static_cast<NESimpleTrafficSwitch::eMessageIDs>(data.getNotifyId());
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
            SimpleTrafficSwitchClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            SimpleTrafficSwitchClientBase::invalidResponse(msgId);
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
            case NESimpleTrafficSwitch::MSG_ID_IsSwitchedOn:
                {
                    bool IsSwitchedOn = mProxy->getIsSwitchedOn(dataState);
                    onIsSwitchedOnUpdate(IsSwitchedOn, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_processNotificationEvent);
                    TRACE_ERR("Client object SimpleTrafficSwitchClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::RESULT_OK:        
        break;

    default:
        {
            TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_processNotificationEvent);
            TRACE_ERR("Client object SimpleTrafficSwitchClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_invalidResponse);
void SimpleTrafficSwitchClientBase::invalidResponse( NESimpleTrafficSwitch::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficSwitchClientBase is not implemented! Make error handling!"
                    , NESimpleTrafficSwitch::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_invalidRequest);
void SimpleTrafficSwitchClientBase::invalidRequest( NESimpleTrafficSwitch::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficSwitchClientBase is not implemented! Make error handling!"
                    , NESimpleTrafficSwitch::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_requestFailed);
void SimpleTrafficSwitchClientBase::requestFailed( NESimpleTrafficSwitch::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client SimpleTrafficSwitchClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NESimpleTrafficSwitch::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NESimpleTrafficSwitch::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NESimpleTrafficSwitch::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NESimpleTrafficSwitch::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NESimpleTrafficSwitch::MSG_ID_INVALID) )
    {
        (this->*NESimpleTrafficSwitch::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_onIsSwitchedOnUpdate);
void SimpleTrafficSwitchClientBase::onIsSwitchedOnUpdate( bool /* IsSwitchedOn */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_onIsSwitchedOnUpdate);
    TRACE_WARN("The attribute IsSwitchedOn (value = %u) update method of proxy [ %s ] client SimpleTrafficSwitchClientBase is not implemented!"
                    , static_cast<unsigned int>(NESimpleTrafficSwitch::MSG_ID_IsSwitchedOn)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
DEF_TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_requestSwitchLightFailed);
void SimpleTrafficSwitchClientBase::requestSwitchLightFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_SimpleTrafficSwitchClientBase_requestSwitchLightFailed);
    TRACE_WARN("The request requestSwitchLight (value = %u) method of proxy [ %s ] client SimpleTrafficSwitchClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NESimpleTrafficSwitch::MSG_ID_requestSwitchLight)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/SimpleTrafficSwitchClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
