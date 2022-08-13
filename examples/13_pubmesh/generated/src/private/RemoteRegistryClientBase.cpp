//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/RemoteRegistryClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     13.08.2022  02:45:11 GMT+02:00
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/RemoteRegistryClientBase.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/RemoteRegistryClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NERemoteRegistry
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (RemoteRegistryClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &RemoteRegistryClientBase::requestRegisterFailed
        , &RemoteRegistryClientBase::requestUnregisterFailed
        , &RemoteRegistryClientBase::requestHelloWorldFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

RemoteRegistryClientBase::RemoteRegistryClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

RemoteRegistryClientBase::RemoteRegistryClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

RemoteRegistryClientBase::RemoteRegistryClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

RemoteRegistryClientBase::~RemoteRegistryClientBase( void )
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

bool RemoteRegistryClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            RemoteRegistryProxy * newProxy = RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * RemoteRegistryClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_serviceConnected);
bool RemoteRegistryClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_serviceConnected);

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

void RemoteRegistryClientBase::notifyOn( NERemoteRegistry::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_processNotificationEvent);
void RemoteRegistryClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NERemoteRegistry::eMessageIDs msgId   = static_cast<NERemoteRegistry::eMessageIDs>(data.getNotifyId());
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
            RemoteRegistryClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            RemoteRegistryClientBase::invalidResponse(msgId);
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
            case NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
                {
                    const NERemoteRegistry::ListRegistry & RegistryList = mProxy->getRegistryList(dataState);
                    onRegistryListUpdate(RegistryList, dataState);
                }
                break;

            case NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
                {
                    unsigned int RemainOutputs = mProxy->getRemainOutputs(dataState);
                    onRemainOutputsUpdate(RemainOutputs, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_processNotificationEvent);
                    TRACE_ERR("Client object RemoteRegistryClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NERemoteRegistry::eMessageIDs::MsgId_responseRegister:
                {
                    const NERemoteRegistry::sClientRegister & client = mProxy->getParamclient();
                    responseRegister( client );
                }
                break;

            case NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld:
                {
                    unsigned int clientID = mProxy->getParamclientID();
                    responseHelloWorld( clientID );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_processNotificationEvent);
                    TRACE_ERR("Client object RemoteRegistryClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_RemoteRegistryClientBase_processNotificationEvent);
            TRACE_ERR("Client object RemoteRegistryClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_invalidResponse);
void RemoteRegistryClientBase::invalidResponse( NERemoteRegistry::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client RemoteRegistryClientBase is not implemented! Make error handling!"
                    , NERemoteRegistry::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_invalidRequest);
void RemoteRegistryClientBase::invalidRequest( NERemoteRegistry::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client RemoteRegistryClientBase is not implemented! Make error handling!"
                    , NERemoteRegistry::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestFailed);
void RemoteRegistryClientBase::requestFailed( NERemoteRegistry::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client RemoteRegistryClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NERemoteRegistry::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NERemoteRegistry::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NERemoteRegistry::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NERemoteRegistry::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NERemoteRegistry::eMessageIDs::MsgId_Invalid) && (index < NERemoteRegistry::getInterfaceData().idRequestCount) )
    {
        (this->*NERemoteRegistry::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_onRegistryListUpdate);
void RemoteRegistryClientBase::onRegistryListUpdate( const NERemoteRegistry::ListRegistry & /* RegistryList */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_onRegistryListUpdate);
    TRACE_WARN("The attribute RegistryList (value = %u) update method of proxy [ %s ] client RemoteRegistryClientBase is not implemented!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_RegistryList)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_onRemainOutputsUpdate);
void RemoteRegistryClientBase::onRemainOutputsUpdate( unsigned int /* RemainOutputs */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_onRemainOutputsUpdate);
    TRACE_WARN("The attribute RemainOutputs (value = %u) update method of proxy [ %s ] client RemoteRegistryClientBase is not implemented!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestRegisterFailed);
void RemoteRegistryClientBase::requestRegisterFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestRegisterFailed);
    TRACE_WARN("The request requestRegister (value = %u) method of proxy [ %s ] client RemoteRegistryClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_requestRegister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestUnregisterFailed);
void RemoteRegistryClientBase::requestUnregisterFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestUnregisterFailed);
    TRACE_WARN("The request requestUnregister (value = %u) method of proxy [ %s ] client RemoteRegistryClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_requestUnregister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestHelloWorldFailed);
void RemoteRegistryClientBase::requestHelloWorldFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_requestHelloWorldFailed);
    TRACE_WARN("The request requestHelloWorld (value = %u) method of proxy [ %s ] client RemoteRegistryClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_responseRegister);
void RemoteRegistryClientBase::responseRegister( const NERemoteRegistry::sClientRegister & /* client */ )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_responseRegister);
    TRACE_WARN("The response responseRegister (value = %u) method of proxy [ %s ] client RemoteRegistryClientBase is not implemented!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_responseRegister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryClientBase_responseHelloWorld);
void RemoteRegistryClientBase::responseHelloWorld( unsigned int /* clientID */ )
{
    TRACE_SCOPE(generated_src_RemoteRegistryClientBase_responseHelloWorld);
    TRACE_WARN("The response responseHelloWorld (value = %u) method of proxy [ %s ] client RemoteRegistryClientBase is not implemented!"
                    , static_cast<unsigned int>(NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/RemoteRegistryClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
