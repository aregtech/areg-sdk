//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:05 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 *
 * \file            generated/DirectMessagerClientBase.hpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/DirectMessagerClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEDirectMessager
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (DirectMessagerClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &DirectMessagerClientBase::requestChatJoinFailed
        , &DirectMessagerClientBase::requestMessageSendFailed
        , &DirectMessagerClientBase::requestMessageTypeFailed
        , &DirectMessagerClientBase::requestChatLeaveFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// DirectMessagerClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

DirectMessagerClientBase::DirectMessagerClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

DirectMessagerClientBase::DirectMessagerClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

DirectMessagerClientBase::DirectMessagerClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

DirectMessagerClientBase::DirectMessagerClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

DirectMessagerClientBase::~DirectMessagerClientBase( void )
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

bool DirectMessagerClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            DirectMessagerProxy * newProxy = DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * DirectMessagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_serviceConnected);
bool DirectMessagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_serviceConnected);

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

void DirectMessagerClientBase::notifyOn( NEDirectMessager::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_processNotificationEvent);
void DirectMessagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEDirectMessager::eMessageIDs msgId   = static_cast<NEDirectMessager::eMessageIDs>(data.getNotifyId());
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
            DirectMessagerClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            DirectMessagerClientBase::invalidResponse(msgId);
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
            case NEDirectMessager::eMessageIDs::MsgId_ChatParticipants:
                {
                    const NEDirectMessager::ListParticipants & ChatParticipants = mProxy->getChatParticipants(dataState);
                    onChatParticipantsUpdate(ChatParticipants, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_DirectMessagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectMessagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEDirectMessager::eMessageIDs::MsgId_responseChatJoin:
                {
                    bool succeed = mProxy->getParamsucceed();
                    const NEDirectMessager::ListParticipants & listParticipant = mProxy->getParamlistParticipant();
                    const DateTime & timeConnect = mProxy->getParamtimeConnect();
                    const DateTime & timeConnected = mProxy->getParamtimeConnected();
                    responseChatJoin( succeed, listParticipant, timeConnect, timeConnected );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent:
                {
                    const NEDirectMessager::sParticipant & sender = mProxy->getParamsender();
                    const String & msgText = mProxy->getParammsgText();
                    const DateTime & timeSent = mProxy->getParamtimeSent();
                    broadcastMessageSent( sender, msgText, timeSent );
                }
                break;

            case NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const String & msgText = mProxy->getParammsgText();
                    broadcastMessageTyped( participant, msgText );
                }
                break;

            case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const DateTime & timeJoined = mProxy->getParamtimeJoined();
                    broadcastParticipantJoined( participant, timeJoined );
                }
                break;

            case NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const DateTime & timeLeft = mProxy->getParamtimeLeft();
                    broadcastParticipantLeft( participant, timeLeft );
                }
                break;

            case NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed:
                {
                    broadcastChatClosed(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_DirectMessagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectMessagerClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_DirectMessagerClientBase_processNotificationEvent);
            TRACE_ERR("Client object DirectMessagerClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_invalidResponse);
void DirectMessagerClientBase::invalidResponse( NEDirectMessager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectMessagerClientBase is not implemented! Make error handling!"
                    , NEDirectMessager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_invalidRequest);
void DirectMessagerClientBase::invalidRequest( NEDirectMessager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectMessagerClientBase is not implemented! Make error handling!"
                    , NEDirectMessager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_requestFailed);
void DirectMessagerClientBase::requestFailed( NEDirectMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectMessagerClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEDirectMessager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectMessager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEDirectMessager::eMessageIDs::MsgId_Invalid) && (index < NEDirectMessager::getInterfaceData().idRequestCount) )
    {
        (this->*NEDirectMessager::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_onChatParticipantsUpdate);
void DirectMessagerClientBase::onChatParticipantsUpdate( const NEDirectMessager::ListParticipants & /* ChatParticipants */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_onChatParticipantsUpdate);
    TRACE_WARN("The attribute ChatParticipants (value = %u) update method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_ChatParticipants)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_requestChatJoinFailed);
void DirectMessagerClientBase::requestChatJoinFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_requestChatJoinFailed);
    TRACE_WARN("The request requestChatJoin (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_requestChatJoin)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_requestMessageSendFailed);
void DirectMessagerClientBase::requestMessageSendFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_requestMessageSendFailed);
    TRACE_WARN("The request requestMessageSend (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_requestMessageSend)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_requestMessageTypeFailed);
void DirectMessagerClientBase::requestMessageTypeFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_requestMessageTypeFailed);
    TRACE_WARN("The request requestMessageType (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_requestMessageType)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_requestChatLeaveFailed);
void DirectMessagerClientBase::requestChatLeaveFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_requestChatLeaveFailed);
    TRACE_WARN("The request requestChatLeave (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_requestChatLeave)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_responseChatJoin);
void DirectMessagerClientBase::responseChatJoin( bool /* succeed */, const NEDirectMessager::ListParticipants & /* listParticipant */, const DateTime & /* timeConnect */, const DateTime & /* timeConnected */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_responseChatJoin);
    TRACE_WARN("The response responseChatJoin (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_responseChatJoin)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastMessageSent);
void DirectMessagerClientBase::broadcastMessageSent( const NEDirectMessager::sParticipant & /* sender */, const String & /* msgText */, const DateTime & /* timeSent */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastMessageSent);
    TRACE_WARN("The broadcast broadcastMessageSent (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_broadcastMessageSent)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastMessageTyped);
void DirectMessagerClientBase::broadcastMessageTyped( const NEDirectMessager::sParticipant & /* participant */, const String & /* msgText */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastMessageTyped);
    TRACE_WARN("The broadcast broadcastMessageTyped (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_broadcastMessageTyped)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastParticipantJoined);
void DirectMessagerClientBase::broadcastParticipantJoined( const NEDirectMessager::sParticipant & /* participant */, const DateTime & /* timeJoined */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastParticipantJoined);
    TRACE_WARN("The broadcast broadcastParticipantJoined (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantJoined)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastParticipantLeft);
void DirectMessagerClientBase::broadcastParticipantLeft( const NEDirectMessager::sParticipant & /* participant */, const DateTime & /* timeLeft */ )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastParticipantLeft);
    TRACE_WARN("The broadcast broadcastParticipantLeft (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_broadcastParticipantLeft)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastChatClosed);
void DirectMessagerClientBase::broadcastChatClosed( void )
{
    TRACE_SCOPE(generated_DirectMessagerClientBase_broadcastChatClosed);
    TRACE_WARN("The broadcast broadcastChatClosed (value = %u) method of proxy [ %s ] client DirectMessagerClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectMessager::eMessageIDs::MsgId_broadcastChatClosed)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
