//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/ConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:02 GMT+01:00
 *                  Create by AREG SDK code generator tool from source ConnectionManager.
 *
 * \file            generated/ConnectionManagerClientBase.hpp
 * \ingroup         ConnectionManager Service Interface
 * \brief           This is an automatic generated code of ConnectionManager
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/ConnectionManagerClientBase.hpp"
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
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &ConnectionManagerClientBase::requestConnectFailed
        , &ConnectionManagerClientBase::requestRegisterConnectionFailed
        , &ConnectionManagerClientBase::requestDisconnectFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// ConnectionManagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

ConnectionManagerClientBase::ConnectionManagerClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

ConnectionManagerClientBase::ConnectionManagerClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

ConnectionManagerClientBase::ConnectionManagerClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

ConnectionManagerClientBase::ConnectionManagerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( ConnectionManagerProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

ConnectionManagerClientBase::~ConnectionManagerClientBase( void )
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

bool ConnectionManagerClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            ConnectionManagerProxy * newProxy = ConnectionManagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * ConnectionManagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_serviceConnected);
bool ConnectionManagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_serviceConnected);

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

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_processNotificationEvent);
void ConnectionManagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEConnectionManager::eMessageIDs msgId   = static_cast<NEConnectionManager::eMessageIDs>(data.getNotifyId());
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
            ConnectionManagerClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            ConnectionManagerClientBase::invalidResponse(msgId);
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
            case NEConnectionManager::eMessageIDs::MsgId_ConnectionList:
                {
                    const NEConnectionManager::MapConnection & ConnectionList = mProxy->getConnectionList(dataState);
                    onConnectionListUpdate(ConnectionList, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_ConnectionManagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object ConnectionManagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
                                , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                                , msgId);
                    ASSERT(false);
                }
                break;
            }
        }
        break;

    case NEService::eResultType::RequestOK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
            case NEConnectionManager::eMessageIDs::MsgId_responseConnect:
                {
                    const String & nickName = mProxy->getParamnickName();
                    unsigned int cookie = mProxy->getParamcookie();
                    const DateTime & dateTime = mProxy->getParamdateTime();
                    NEConnectionManager::eConnectionResult result = mProxy->getParamresult();
                    responseConnect( nickName, cookie, dateTime, result );
                }
                break;

            case NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection:
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
            case NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated:
                {
                    const NEConnectionManager::MapConnection & updatedList = mProxy->getParamupdatedList();
                    broadcastConnectionUpdated( updatedList );
                }
                break;

            case NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected:
                {
                    const NEConnectionManager::sConnection & clientConnected = mProxy->getParamclientConnected();
                    broadcastClientConnected( clientConnected );
                }
                break;

            case NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected:
                {
                    const NEConnectionManager::sConnection & clientLeft = mProxy->getParamclientLeft();
                    broadcastClientDisconnected( clientLeft );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_ConnectionManagerClientBase_processNotificationEvent);
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
            TRACE_SCOPE(generated_ConnectionManagerClientBase_processNotificationEvent);
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
DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_invalidResponse);
void ConnectionManagerClientBase::invalidResponse( NEConnectionManager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented! Make error handling!"
                    , NEConnectionManager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_invalidRequest);
void ConnectionManagerClientBase::invalidRequest( NEConnectionManager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented! Make error handling!"
                    , NEConnectionManager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_requestFailed);
void ConnectionManagerClientBase::requestFailed( NEConnectionManager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client ConnectionManagerClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEConnectionManager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEConnectionManager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEConnectionManager::eMessageIDs::MsgId_Invalid) && (index < NEConnectionManager::getInterfaceData().idRequestCount) )
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

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_onConnectionListUpdate);
void ConnectionManagerClientBase::onConnectionListUpdate( const NEConnectionManager::MapConnection & /* ConnectionList */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_onConnectionListUpdate);
    TRACE_WARN("The attribute ConnectionList (value = %u) update method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_ConnectionList)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_requestConnectFailed);
void ConnectionManagerClientBase::requestConnectFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_requestConnectFailed);
    TRACE_WARN("The request requestConnect (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_requestConnect)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_requestRegisterConnectionFailed);
void ConnectionManagerClientBase::requestRegisterConnectionFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_requestRegisterConnectionFailed);
    TRACE_WARN("The request requestRegisterConnection (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_requestRegisterConnection)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_requestDisconnectFailed);
void ConnectionManagerClientBase::requestDisconnectFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_requestDisconnectFailed);
    TRACE_WARN("The request requestDisconnect (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_requestDisconnect)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_responseConnect);
void ConnectionManagerClientBase::responseConnect( const String & /* nickName */, unsigned int /* cookie */, const DateTime & /* dateTime */, NEConnectionManager::eConnectionResult /* result */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_responseConnect);
    TRACE_WARN("The response responseConnect (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_responseConnect)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_responseRegisterConnection);
void ConnectionManagerClientBase::responseRegisterConnection( const NEConnectionManager::sConnection & /* connection */, const NEConnectionManager::ListConnection & /* connectionList */, bool /* success */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_responseRegisterConnection);
    TRACE_WARN("The response responseRegisterConnection (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_responseRegisterConnection)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastConnectionUpdated);
void ConnectionManagerClientBase::broadcastConnectionUpdated( const NEConnectionManager::MapConnection & /* updatedList */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastConnectionUpdated);
    TRACE_WARN("The broadcast broadcastConnectionUpdated (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_broadcastConnectionUpdated)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastClientConnected);
void ConnectionManagerClientBase::broadcastClientConnected( const NEConnectionManager::sConnection & /* clientConnected */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastClientConnected);
    TRACE_WARN("The broadcast broadcastClientConnected (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_broadcastClientConnected)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastClientDisconnected);
void ConnectionManagerClientBase::broadcastClientDisconnected( const NEConnectionManager::sConnection & /* clientLeft */ )
{
    TRACE_SCOPE(generated_ConnectionManagerClientBase_broadcastClientDisconnected);
    TRACE_WARN("The broadcast broadcastClientDisconnected (value = %u) method of proxy [ %s ] client ConnectionManagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEConnectionManager::eMessageIDs::MsgId_broadcastClientDisconnected)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/ConnectionManagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
