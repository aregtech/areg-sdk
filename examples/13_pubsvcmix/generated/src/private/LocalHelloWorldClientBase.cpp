//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LocalHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     11.08.2021  13:17:31 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            generated/src/private/LocalHelloWorldClientBase.cpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/LocalHelloWorldClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NELocalHelloWorld
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (LocalHelloWorldClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &LocalHelloWorldClientBase::requestHelloWorldFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char * roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char * roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::~LocalHelloWorldClientBase( void )
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

bool LocalHelloWorldClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != NULL)
    {
        String roleName   = mProxy->getProxyAddress().getRoleName();
        String threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            LocalHelloWorldProxy * newProxy = LocalHelloWorldProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * LocalHelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<LocalHelloWorldProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_serviceConnected);
bool LocalHelloWorldClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_serviceConnected);
    
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

void LocalHelloWorldClientBase::notifyOn( NELocalHelloWorld::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_processNotificationEvent);
void LocalHelloWorldClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NELocalHelloWorld::eMessageIDs msgId   = static_cast<NELocalHelloWorld::eMessageIDs>(data.getNotifyId());
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
            LocalHelloWorldClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::RESULT_INVALID:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification 
         ************************************************************************/
            LocalHelloWorldClientBase::invalidResponse(msgId);
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
            case NELocalHelloWorld::MSG_ID_ConnectedClients:
                {
                    const NELocalHelloWorld::ConnectionList & ConnectedClients = mProxy->getConnectedClients(dataState);
                    onConnectedClientsUpdate(ConnectedClients, dataState);
                }
                break;

            case NELocalHelloWorld::MSG_ID_RemainOutput:
                {
                    short RemainOutput = mProxy->getRemainOutput(dataState);
                    onRemainOutputUpdate(RemainOutput, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NELocalHelloWorld::MSG_ID_responseHelloWorld:
                {
                    const NELocalHelloWorld::sConnectedClient & clientInfo = mProxy->getParamclientInfo();
                    responseHelloWorld( clientInfo );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_processNotificationEvent);
            TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_invalidResponse);
void LocalHelloWorldClientBase::invalidResponse( NELocalHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client LocalHelloWorldClientBase is not implemented! Make error handling!"
                    , NELocalHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_invalidRequest);
void LocalHelloWorldClientBase::invalidRequest( NELocalHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client LocalHelloWorldClientBase is not implemented! Make error handling!"
                    , NELocalHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_requestFailed);
void LocalHelloWorldClientBase::requestFailed( NELocalHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client LocalHelloWorldClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NELocalHelloWorld::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NELocalHelloWorld::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID) )
    {
        (this->*NELocalHelloWorld::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_onConnectedClientsUpdate);
void LocalHelloWorldClientBase::onConnectedClientsUpdate( const NELocalHelloWorld::ConnectionList & /* ConnectedClients */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_onConnectedClientsUpdate);
    TRACE_WARN("The attribute ConnectedClients (value = %u) update method of proxy [ %s ] client LocalHelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_ConnectedClients)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_onRemainOutputUpdate);
void LocalHelloWorldClientBase::onRemainOutputUpdate( short /* RemainOutput */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_onRemainOutputUpdate);
    TRACE_WARN("The attribute RemainOutput (value = %u) update method of proxy [ %s ] client LocalHelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_RemainOutput)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_requestHelloWorldFailed);
void LocalHelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_requestHelloWorldFailed);
    TRACE_WARN("The request requestHelloWorld (value = %u) method of proxy [ %s ] client LocalHelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_requestHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_responseHelloWorld);
void LocalHelloWorldClientBase::responseHelloWorld( const NELocalHelloWorld::sConnectedClient & /* clientInfo */ )
{
    TRACE_SCOPE(generated_src_LocalHelloWorldClientBase_responseHelloWorld);
    TRACE_WARN("The response responseHelloWorld (value = %u) method of proxy [ %s ] client LocalHelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_responseHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LocalHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
