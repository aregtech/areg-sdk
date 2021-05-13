//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/TrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source TrafficLight.
 * Generated at     12.05.2021  16:41:14 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/TrafficLightClientBase.cpp
 * \ingroup         TrafficLight Service Interface
 * \brief           This is an automatic generated code of TrafficLight Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/TrafficLightClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NETrafficLight
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (TrafficLightClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &TrafficLightClientBase::requestChangeLightFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// TrafficLightClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

TrafficLightClientBase::TrafficLightClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

TrafficLightClientBase::TrafficLightClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

TrafficLightClientBase::TrafficLightClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( TrafficLightProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

TrafficLightClientBase::~TrafficLightClientBase( void )
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

bool TrafficLightClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        TrafficLightProxy * newProxy = TrafficLightProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
        if (newProxy != NULL)
        {
            mProxy->clearAllNotifications( static_cast<IENotificationEventConsumer &>(self()) );
            mProxy->freeProxy (static_cast<IEProxyListener &>(self()) );
            mProxy = newProxy;
            result = true;
        }
        else
        {
            ; // do nothing
        }
    }
    else
    {
        ; // do nothing, no role name is assigned
    }
    
    return result;
}

DispatcherThread * TrafficLightClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<TrafficLightProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void TrafficLightClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_TrafficLightClientBase_ServiceConnected);
bool TrafficLightClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_TrafficLightClientBase_ServiceConnected);
    
    bool result = false;
    if(mProxy == &proxy)
    {
        TRACE_DBG("The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , isConnected ? "CONNECTED" : "DISCONNECTED");
        
        mIsConnected= isConnected;
        result      = true;
    }
    
    return result;
}

void TrafficLightClientBase::notifyOn( NETrafficLight::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_TrafficLightClientBase_processNotificationEvent);
void TrafficLightClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NETrafficLight::eMessageIDs msgId   = static_cast<NETrafficLight::eMessageIDs>(data.getNotifyId());
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
            requestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            invalidResponse(msgId);
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
            case NETrafficLight::MSG_ID_TrafficLight:
                {
                    NETrafficLight::eLight TrafficLight = mProxy->getTrafficLight(dataState);
                    onTrafficLightUpdate(TrafficLight, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_TrafficLightClientBase_processNotificationEvent);
                    TRACE_ERR("Client object TrafficLightClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NETrafficLight::MSG_ID_responseChangeLight:
                {
                    NETrafficLight::eLight lightColor = mProxy->getParamlightColor();
                    responseChangeLight( lightColor );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(src_gen_TrafficLightClientBase_processNotificationEvent);
                    TRACE_ERR("Client object TrafficLightClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(src_gen_TrafficLightClientBase_processNotificationEvent);
            TRACE_ERR("Client object TrafficLightClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(src_gen_TrafficLightClientBase_invalidResponse);
void TrafficLightClientBase::invalidResponse( NETrafficLight::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_TrafficLightClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client TrafficLightClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NETrafficLight::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("TrafficLightClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_TrafficLightClientBase_invalidRequest);
void TrafficLightClientBase::invalidRequest( NETrafficLight::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_TrafficLightClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client TrafficLightClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NETrafficLight::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_TrafficLightClientBase_requestFailed);
void TrafficLightClientBase::requestFailed( NETrafficLight::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_TrafficLightClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of TrafficLightClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NETrafficLight::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NETrafficLight::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NETrafficLight::MSG_ID_INVALID) )
    {
        (this->*NETrafficLight::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void TrafficLightClientBase::onTrafficLightUpdate( NETrafficLight::eLight /* TrafficLight */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "TrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_TrafficLight) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void TrafficLightClientBase::requestChangeLightFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "TrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_requestChangeLight) );
}

void TrafficLightClientBase::responseChangeLight( NETrafficLight::eLight /* lightColor */ )
{
    ClientBase::responseNotImplemented( "TrafficLightClientBase", static_cast<unsigned int>(NETrafficLight::MSG_ID_responseChangeLight) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/TrafficLightClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
