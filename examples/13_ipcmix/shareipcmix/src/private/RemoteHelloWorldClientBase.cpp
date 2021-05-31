//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteHelloWorld.
 * Generated at     29.05.2021  12:43:00 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteHelloWorldClientBase.cpp
 * \ingroup         RemoteHelloWorld Service Interface
 * \brief           This is an automatic generated code of RemoteHelloWorld Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shareipcmix/src/RemoteHelloWorldClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NERemoteHelloWorld
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (RemoteHelloWorldClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &RemoteHelloWorldClientBase::requestHelloWorldFailed
        , &RemoteHelloWorldClientBase::requestClientShutdownFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// RemoteHelloWorldClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

RemoteHelloWorldClientBase::RemoteHelloWorldClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

RemoteHelloWorldClientBase::RemoteHelloWorldClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

RemoteHelloWorldClientBase::RemoteHelloWorldClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

RemoteHelloWorldClientBase::~RemoteHelloWorldClientBase( void )
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

bool RemoteHelloWorldClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        RemoteHelloWorldProxy * newProxy = RemoteHelloWorldProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * RemoteHelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<RemoteHelloWorldProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_serviceConnected);
bool RemoteHelloWorldClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_serviceConnected);
    
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

void RemoteHelloWorldClientBase::notifyOn( NERemoteHelloWorld::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_processNotificationEvent);
void RemoteHelloWorldClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NERemoteHelloWorld::eMessageIDs msgId   = static_cast<NERemoteHelloWorld::eMessageIDs>(data.getNotifyId());
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
            case NERemoteHelloWorld::MSG_ID_ConnectedClients:
                {
                    const NERemoteHelloWorld::ConnectionList & ConnectedClients = mProxy->getConnectedClients(dataState);
                    onConnectedClientsUpdate(ConnectedClients, dataState);
                }
                break;

            case NERemoteHelloWorld::MSG_ID_RemainOutput:
                {
                    short RemainOutput = mProxy->getRemainOutput(dataState);
                    onRemainOutputUpdate(RemainOutput, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object RemoteHelloWorldClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NERemoteHelloWorld::MSG_ID_responseHelloWorld:
                {
                    const NERemoteHelloWorld::sConnectedClient & clientInfo = mProxy->getParamclientInfo();
                    responseHelloWorld( clientInfo );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NERemoteHelloWorld::MSG_ID_broadcastHelloClients:
                {
                    const NERemoteHelloWorld::ConnectionList & clientList = mProxy->getParamclientList();
                    broadcastHelloClients( clientList );
                }
                break;

            case NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable:
                {
                    broadcastServiceUnavailable(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object RemoteHelloWorldClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_processNotificationEvent);
            TRACE_ERR("Client object RemoteHelloWorldClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_invalidResponse);
void RemoteHelloWorldClientBase::invalidResponse( NERemoteHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client RemoteHelloWorldClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NERemoteHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("RemoteHelloWorldClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_invalidRequest);
void RemoteHelloWorldClientBase::invalidRequest( NERemoteHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client RemoteHelloWorldClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NERemoteHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_requestFailed);
void RemoteHelloWorldClientBase::requestFailed( NERemoteHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shareipcmix_src_RemoteHelloWorldClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of RemoteHelloWorldClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NERemoteHelloWorld::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NERemoteHelloWorld::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_INVALID) )
    {
        (this->*NERemoteHelloWorld::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void RemoteHelloWorldClientBase::onConnectedClientsUpdate( const NERemoteHelloWorld::ConnectionList & /* ConnectedClients */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_ConnectedClients) );
}

void RemoteHelloWorldClientBase::onRemainOutputUpdate( short /* RemainOutput */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_RemainOutput) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void RemoteHelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_requestHelloWorld) );
}

void RemoteHelloWorldClientBase::requestClientShutdownFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_requestClientShutdown) );
}

void RemoteHelloWorldClientBase::responseHelloWorld( const NERemoteHelloWorld::sConnectedClient & /* clientInfo */ )
{
    ClientBase::responseNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_responseHelloWorld) );
}

void RemoteHelloWorldClientBase::broadcastHelloClients( const NERemoteHelloWorld::ConnectionList & /* clientList */ )
{
    ClientBase::broadcastNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_broadcastHelloClients) );
}

void RemoteHelloWorldClientBase::broadcastServiceUnavailable( void )
{
    ClientBase::broadcastNotImplemented( "RemoteHelloWorldClientBase", static_cast<unsigned int>(NERemoteHelloWorld::MSG_ID_broadcastServiceUnavailable) );
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
