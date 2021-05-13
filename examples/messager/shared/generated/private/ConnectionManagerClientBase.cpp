//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/ConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 * Generated at     12.05.2021  16:41:22 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/ConnectionManagerClientBase.cpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/ConnectionManagerClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEConnectionManager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (ConnectionManagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &ConnectionManagerClientBase::requestConnetFailed
        , &ConnectionManagerClientBase::requestRegisterConnectionFailed
        , &ConnectionManagerClientBase::requestDiconnectFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

ConnectionManagerClientBase::ConnectionManagerClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

ConnectionManagerClientBase::ConnectionManagerClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

ConnectionManagerClientBase::ConnectionManagerClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

ConnectionManagerClientBase::~ConnectionManagerClientBase( void )
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

bool ConnectionManagerClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        ConnectionManagerProxy * newProxy = ConnectionManagerProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * ConnectionManagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<ConnectionManagerProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void ConnectionManagerClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_ServiceConnected);
bool ConnectionManagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_ServiceConnected);
    
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

void ConnectionManagerClientBase::notifyOn( NEConnectionManager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_processNotificationEvent);
void ConnectionManagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEConnectionManager::eMessageIDs msgId   = static_cast<NEConnectionManager::eMessageIDs>(data.getNotifyId());
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
            case NEConnectionManager::MSG_ID_ConnectionList:
                {
                    const NEConnectionManager::MapConnection & ConnectionList = mProxy->getConnectionList(dataState);
                    onConnectionListUpdate(ConnectionList, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object ConnectionManagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEConnectionManager::MSG_ID_responseConnect:
                {
                    const String & nickName = mProxy->getParamnickName();
                    unsigned int cookie = mProxy->getParamcookie();
                    const DateTime & dateTime = mProxy->getParamdateTime();
                    NEConnectionManager::eConnectionResult result = mProxy->getParamresult();
                    responseConnect( nickName, cookie, dateTime, result );
                }
                break;

            case NEConnectionManager::MSG_ID_responseRegisterConnection:
                {
                    const NEConnectionManager::sConnection & connection = mProxy->getParamconnection();
                    const NEConnectionManager::ListConnection & connectionList = mProxy->getParamconnectionList();
                    bool success = mProxy->getParamsuccess();
                    responseRegisterConnection( connection, connectionList, success );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEConnectionManager::MSG_ID_broadcastConnectionUpdated:
                {
                    const NEConnectionManager::MapConnection & updatedList = mProxy->getParamupdatedList();
                    broadcastConnectionUpdated( updatedList );
                }
                break;

            case NEConnectionManager::MSG_ID_broadcastClientConnected:
                {
                    const NEConnectionManager::sConnection & clientConnected = mProxy->getParamclientConnected();
                    broadcastClientConnected( clientConnected );
                }
                break;

            case NEConnectionManager::MSG_ID_broadcastClientDisconnected:
                {
                    const NEConnectionManager::sConnection & clientLeft = mProxy->getParamclientLeft();
                    broadcastClientDisconnected( clientLeft );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object ConnectionManagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_processNotificationEvent);
            TRACE_ERR("Client object ConnectionManagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_invalidResponse);
void ConnectionManagerClientBase::invalidResponse( NEConnectionManager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client ConnectionManagerClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEConnectionManager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("ConnectionManagerClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_invalidRequest);
void ConnectionManagerClientBase::invalidRequest( NEConnectionManager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client ConnectionManagerClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEConnectionManager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_requestFailed);
void ConnectionManagerClientBase::requestFailed( NEConnectionManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_ConnectionManagerClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of ConnectionManagerClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEConnectionManager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEConnectionManager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEConnectionManager::MSG_ID_INVALID) )
    {
        (this->*NEConnectionManager::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void ConnectionManagerClientBase::onConnectionListUpdate( const NEConnectionManager::MapConnection & /* ConnectionList */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_ConnectionList) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void ConnectionManagerClientBase::requestConnetFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_requestConnet) );
}

void ConnectionManagerClientBase::requestRegisterConnectionFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_requestRegisterConnection) );
}

void ConnectionManagerClientBase::requestDiconnectFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_requestDiconnect) );
}

void ConnectionManagerClientBase::responseConnect( const String & /* nickName */, unsigned int /* cookie */, const DateTime & /* dateTime */, NEConnectionManager::eConnectionResult /* result */ )
{
    ClientBase::responseNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_responseConnect) );
}

void ConnectionManagerClientBase::responseRegisterConnection( const NEConnectionManager::sConnection & /* connection */, const NEConnectionManager::ListConnection & /* connectionList */, bool /* success */ )
{
    ClientBase::responseNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_responseRegisterConnection) );
}

void ConnectionManagerClientBase::broadcastConnectionUpdated( const NEConnectionManager::MapConnection & /* updatedList */ )
{
    ClientBase::broadcastNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_broadcastConnectionUpdated) );
}

void ConnectionManagerClientBase::broadcastClientConnected( const NEConnectionManager::sConnection & /* clientConnected */ )
{
    ClientBase::broadcastNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_broadcastClientConnected) );
}

void ConnectionManagerClientBase::broadcastClientDisconnected( const NEConnectionManager::sConnection & /* clientLeft */ )
{
    ClientBase::broadcastNotImplemented( "ConnectionManagerClientBase", static_cast<unsigned int>(NEConnectionManager::MSG_ID_broadcastClientDisconnected) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/ConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
