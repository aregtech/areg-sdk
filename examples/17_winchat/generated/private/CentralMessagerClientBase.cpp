//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/CentralMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:01 GMT+01:00
 *                  Create by AREG SDK code generator tool from source CentralMessager.
 *
 * \file            generated/CentralMessagerClientBase.hpp
 * \ingroup         CentralMessager Service Interface
 * \brief           This is an automatic generated code of CentralMessager
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/CentralMessagerClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NECentralMessager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (CentralMessagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &CentralMessagerClientBase::requestSendMessageFailed
        , &CentralMessagerClientBase::requestKeyTypingFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// CentralMessagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

CentralMessagerClientBase::CentralMessagerClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CentralMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

CentralMessagerClientBase::CentralMessagerClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CentralMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

CentralMessagerClientBase::CentralMessagerClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CentralMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

CentralMessagerClientBase::CentralMessagerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( CentralMessagerProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

CentralMessagerClientBase::~CentralMessagerClientBase( void )
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

bool CentralMessagerClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            CentralMessagerProxy * newProxy = CentralMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * CentralMessagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_serviceConnected);
bool CentralMessagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_serviceConnected);

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

void CentralMessagerClientBase::notifyOn( NECentralMessager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_processNotificationEvent);
void CentralMessagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NECentralMessager::eMessageIDs msgId   = static_cast<NECentralMessager::eMessageIDs>(data.getNotifyId());
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
            CentralMessagerClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            CentralMessagerClientBase::invalidResponse(msgId);
        }
        break;

    case NEService::eResultType::DataOK:            // fall through
    case NEService::eResultType::DataInvalid:
        break;

    case NEService::eResultType::RequestOK:
        {
            switch (msgId)
            {
        /************************************************************************
         * Trigger response processing
         ************************************************************************/
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage:
                {
                    const String & nickName = mProxy->getParamnickName();
                    unsigned int cookie = mProxy->getParamcookie();
                    const String & newMessage = mProxy->getParamnewMessage();
                    const DateTime & dateTime = mProxy->getParamdateTime();
                    broadcastSendMessage( nickName, cookie, newMessage, dateTime );
                }
                break;

            case NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping:
                {
                    const String & nickName = mProxy->getParamnickName();
                    unsigned int cookie = mProxy->getParamcookie();
                    const String & newMessage = mProxy->getParamnewMessage();
                    broadcastKeyTyping( nickName, cookie, newMessage );
                }
                break;

            case NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage:
                {
                    const String & serverMessage = mProxy->getParamserverMessage();
                    const DateTime & dateTime = mProxy->getParamdateTime();
                    broadcastBroadcastMessage( serverMessage, dateTime );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_CentralMessagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object CentralMessagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_CentralMessagerClientBase_processNotificationEvent);
            TRACE_ERR("Client object CentralMessagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_invalidResponse);
void CentralMessagerClientBase::invalidResponse( NECentralMessager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client CentralMessagerClientBase is not implemented! Make error handling!"
                    , NECentralMessager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_invalidRequest);
void CentralMessagerClientBase::invalidRequest( NECentralMessager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client CentralMessagerClientBase is not implemented! Make error handling!"
                    , NECentralMessager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_requestFailed);
void CentralMessagerClientBase::requestFailed( NECentralMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client CentralMessagerClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NECentralMessager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NECentralMessager::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NECentralMessager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NECentralMessager::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NECentralMessager::eMessageIDs::MsgId_Invalid) && (index < NECentralMessager::getInterfaceData().idRequestCount) )
    {
        (this->*NECentralMessager::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_requestSendMessageFailed);
void CentralMessagerClientBase::requestSendMessageFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_requestSendMessageFailed);
    TRACE_WARN("The request requestSendMessage (value = %u) method of proxy [ %s ] client CentralMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NECentralMessager::eMessageIDs::MsgId_requestSendMessage)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_requestKeyTypingFailed);
void CentralMessagerClientBase::requestKeyTypingFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_requestKeyTypingFailed);
    TRACE_WARN("The request requestKeyTyping (value = %u) method of proxy [ %s ] client CentralMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NECentralMessager::eMessageIDs::MsgId_requestKeyTyping)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastSendMessage);
void CentralMessagerClientBase::broadcastSendMessage( const String & /* nickName */, unsigned int /* cookie */, const String & /* newMessage */, const DateTime & /* dateTime */ )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastSendMessage);
    TRACE_WARN("The broadcast broadcastSendMessage (value = %u) method of proxy [ %s ] client CentralMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NECentralMessager::eMessageIDs::MsgId_broadcastSendMessage)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastKeyTyping);
void CentralMessagerClientBase::broadcastKeyTyping( const String & /* nickName */, unsigned int /* cookie */, const String & /* newMessage */ )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastKeyTyping);
    TRACE_WARN("The broadcast broadcastKeyTyping (value = %u) method of proxy [ %s ] client CentralMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NECentralMessager::eMessageIDs::MsgId_broadcastKeyTyping)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastBroadcastMessage);
void CentralMessagerClientBase::broadcastBroadcastMessage( const String & /* serverMessage */, const DateTime & /* dateTime */ )
{
    TRACE_SCOPE(generated_CentralMessagerClientBase_broadcastBroadcastMessage);
    TRACE_WARN("The broadcast broadcastBroadcastMessage (value = %u) method of proxy [ %s ] client CentralMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NECentralMessager::eMessageIDs::MsgId_broadcastBroadcastMessage)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/CentralMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
