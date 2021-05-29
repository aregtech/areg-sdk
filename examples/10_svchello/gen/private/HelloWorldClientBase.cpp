//////////////////////////////////////////////////////////////////////////
// Begin generate gen/private/HelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source HelloWorld.
 * Generated at     27.05.2021  03:25:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            gen/private/HelloWorldClientBase.cpp
 * \ingroup         HelloWorld Service Interface
 * \brief           This is an automatic generated code of HelloWorld Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "gen/HelloWorldClientBase.hpp"
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

HelloWorldClientBase::HelloWorldClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

HelloWorldClientBase::HelloWorldClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

HelloWorldClientBase::HelloWorldClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

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
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
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

DispatcherThread * HelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<HelloWorldProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(gen_HelloWorldClientBase_serviceConnected);
bool HelloWorldClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(gen_HelloWorldClientBase_serviceConnected);
    
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

DEF_TRACE_SCOPE(gen_HelloWorldClientBase_processNotificationEvent);
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
                    TRACE_SCOPE(gen_HelloWorldClientBase_processNotificationEvent);
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
                    TRACE_SCOPE(gen_HelloWorldClientBase_processNotificationEvent);
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
            TRACE_SCOPE(gen_HelloWorldClientBase_processNotificationEvent);
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
DEF_TRACE_SCOPE(gen_HelloWorldClientBase_invalidResponse);
void HelloWorldClientBase::invalidResponse( NEHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(gen_HelloWorldClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client HelloWorldClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("HelloWorldClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(gen_HelloWorldClientBase_invalidRequest);
void HelloWorldClientBase::invalidRequest( NEHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(gen_HelloWorldClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client HelloWorldClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(gen_HelloWorldClientBase_requestFailed);
void HelloWorldClientBase::requestFailed( NEHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(gen_HelloWorldClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of HelloWorldClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
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

void HelloWorldClientBase::onConnectedClientsUpdate( const NEHelloWorld::ConnectionList & /* ConnectedClients */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_ConnectedClients) );
}

void HelloWorldClientBase::onRemainOutputUpdate( short /* RemainOutput */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_RemainOutput) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void HelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_requestHelloWorld) );
}

void HelloWorldClientBase::requestClientShutdownFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_requestClientShutdown) );
}

void HelloWorldClientBase::responseHelloWorld( const NEHelloWorld::sConnectedClient & /* clientInfo */ )
{
    ClientBase::responseNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_responseHelloWorld) );
}

void HelloWorldClientBase::broadcastHelloClients( const NEHelloWorld::ConnectionList & /* clientList */ )
{
    ClientBase::broadcastNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_broadcastHelloClients) );
}

void HelloWorldClientBase::broadcastServiceUnavailable( void )
{
    ClientBase::broadcastNotImplemented( "HelloWorldClientBase", static_cast<unsigned int>(NEHelloWorld::MSG_ID_broadcastServiceUnavailable) );
}

//////////////////////////////////////////////////////////////////////////
// End generate gen/private/HelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
