//////////////////////////////////////////////////////////////////////////
// Begin generate shripchello/src/private/HelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     04.07.2021  04:21:09 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shripchello/src/private/HelloWorldClientBase.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shripchello/src/HelloWorldClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEHelloWorld
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (HelloWorldClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &HelloWorldClientBase::requestHelloWorldFailed
        , &HelloWorldClientBase::requestClientShutdownFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// HelloWorldClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

HelloWorldClientBase::HelloWorldClientBase( const char * roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

HelloWorldClientBase::HelloWorldClientBase( const char * roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

HelloWorldClientBase::HelloWorldClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

HelloWorldClientBase::~HelloWorldClientBase( void )
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

bool HelloWorldClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != NULL)
    {
        String roleName   = mProxy->getProxyAddress().getRoleName();
        String threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            HelloWorldProxy * newProxy = HelloWorldProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * HelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<HelloWorldProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_serviceConnected);
bool HelloWorldClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_serviceConnected);
    
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

void HelloWorldClientBase::notifyOn( NEHelloWorld::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_processNotificationEvent);
void HelloWorldClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEHelloWorld::eMessageIDs msgId   = static_cast<NEHelloWorld::eMessageIDs>(data.getNotifyId());
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
            case NEHelloWorld::MSG_ID_ConnectedClients:
                {
                    const NEHelloWorld::ConnectionList & ConnectedClients = mProxy->getConnectedClients(dataState);
                    onConnectedClientsUpdate(ConnectedClients, dataState);
                }
                break;

            case NEHelloWorld::MSG_ID_RemainOutput:
                {
                    short RemainOutput = mProxy->getRemainOutput(dataState);
                    onRemainOutputUpdate(RemainOutput, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloWorldClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEHelloWorld::MSG_ID_responseHelloWorld:
                {
                    const NEHelloWorld::sConnectedClient & clientInfo = mProxy->getParamclientInfo();
                    responseHelloWorld( clientInfo );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEHelloWorld::MSG_ID_broadcastHelloClients:
                {
                    const NEHelloWorld::ConnectionList & clientList = mProxy->getParamclientList();
                    broadcastHelloClients( clientList );
                }
                break;

            case NEHelloWorld::MSG_ID_broadcastServiceUnavailable:
                {
                    broadcastServiceUnavailable(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloWorldClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shripchello_src_HelloWorldClientBase_processNotificationEvent);
            TRACE_ERR("Client object HelloWorldClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_invalidResponse);
void HelloWorldClientBase::invalidResponse( NEHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWorldClientBase is not implemented! Make error handling!"
                    , NEHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_invalidRequest);
void HelloWorldClientBase::invalidRequest( NEHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWorldClientBase is not implemented! Make error handling!"
                    , NEHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestFailed);
void HelloWorldClientBase::requestFailed( NEHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloWorldClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEHelloWorld::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEHelloWorld::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEHelloWorld::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEHelloWorld::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEHelloWorld::MSG_ID_INVALID) )
    {
        (this->*NEHelloWorld::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_onConnectedClientsUpdate);
void HelloWorldClientBase::onConnectedClientsUpdate( const NEHelloWorld::ConnectionList & /* ConnectedClients */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_onConnectedClientsUpdate);
    TRACE_WARN("The attribute ConnectedClients (value = %u) update method of proxy [ %s ] client HelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_ConnectedClients)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_onRemainOutputUpdate);
void HelloWorldClientBase::onRemainOutputUpdate( short /* RemainOutput */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_onRemainOutputUpdate);
    TRACE_WARN("The attribute RemainOutput (value = %u) update method of proxy [ %s ] client HelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_RemainOutput)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestHelloWorldFailed);
void HelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestHelloWorldFailed);
    TRACE_WARN("The request requestHelloWorld (value = %u) method of proxy [ %s ] client HelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_requestHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestClientShutdownFailed);
void HelloWorldClientBase::requestClientShutdownFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_requestClientShutdownFailed);
    TRACE_WARN("The request requestClientShutdown (value = %u) method of proxy [ %s ] client HelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_requestClientShutdown)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_responseHelloWorld);
void HelloWorldClientBase::responseHelloWorld( const NEHelloWorld::sConnectedClient & /* clientInfo */ )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_responseHelloWorld);
    TRACE_WARN("The response responseHelloWorld (value = %u) method of proxy [ %s ] client HelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_responseHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_broadcastHelloClients);
void HelloWorldClientBase::broadcastHelloClients( const NEHelloWorld::ConnectionList & /* clientList */ )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_broadcastHelloClients);
    TRACE_WARN("The broadcast broadcastHelloClients (value = %u) method of proxy [ %s ] client HelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_broadcastHelloClients)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(shripchello_src_HelloWorldClientBase_broadcastServiceUnavailable);
void HelloWorldClientBase::broadcastServiceUnavailable( void )
{
    TRACE_SCOPE(shripchello_src_HelloWorldClientBase_broadcastServiceUnavailable);
    TRACE_WARN("The broadcast broadcastServiceUnavailable (value = %u) method of proxy [ %s ] client HelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloWorld::MSG_ID_broadcastServiceUnavailable)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate shripchello/src/private/HelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
