//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectMessager.
 * Generated at     12.05.2021  16:41:24 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectMessagerClientBase.cpp
 * \ingroup         DirectMessager Service Interface
 * \brief           This is an automatic generated code of DirectMessager Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/DirectMessagerClientBase.hpp"
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
    static FuncRequestFailure failureFunctions[] = 
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

DirectMessagerClientBase::DirectMessagerClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

DirectMessagerClientBase::DirectMessagerClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

DirectMessagerClientBase::DirectMessagerClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectMessagerProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

DirectMessagerClientBase::~DirectMessagerClientBase( void )
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

bool DirectMessagerClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        DirectMessagerProxy * newProxy = DirectMessagerProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * DirectMessagerClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<DirectMessagerProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void DirectMessagerClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerClientBase_ServiceConnected);
bool DirectMessagerClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_ServiceConnected);
    
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

DEF_TRACE_SCOPE(shared_generated_DirectMessagerClientBase_processNotificationEvent);
void DirectMessagerClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEDirectMessager::eMessageIDs msgId   = static_cast<NEDirectMessager::eMessageIDs>(data.getNotifyId());
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
            case NEDirectMessager::MSG_ID_ChatParticipants:
                {
                    const NEDirectMessager::ListParticipants & ChatParticipants = mProxy->getChatParticipants(dataState);
                    onChatParticipantsUpdate(ChatParticipants, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectMessagerClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEDirectMessager::MSG_ID_responseChatJoin:
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
            case NEDirectMessager::MSG_ID_broadcastMessageSent:
                {
                    const NEDirectMessager::sParticipant & sender = mProxy->getParamsender();
                    const String & msgText = mProxy->getParammsgText();
                    const DateTime & timeSent = mProxy->getParamtimeSent();
                    broadcastMessageSent( sender, msgText, timeSent );
                }
                break;

            case NEDirectMessager::MSG_ID_broadcastMessageTyped:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const String & msgText = mProxy->getParammsgText();
                    broadcastMessageTyped( participant, msgText );
                }
                break;

            case NEDirectMessager::MSG_ID_broadcastParticipantJoined:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const DateTime & timeJoined = mProxy->getParamtimeJoined();
                    broadcastParticipantJoined( participant, timeJoined );
                }
                break;

            case NEDirectMessager::MSG_ID_broadcastParticipantLeft:
                {
                    const NEDirectMessager::sParticipant & participant = mProxy->getParamparticipant();
                    const DateTime & timeLeft = mProxy->getParamtimeLeft();
                    broadcastParticipantLeft( participant, timeLeft );
                }
                break;

            case NEDirectMessager::MSG_ID_broadcastChatClosed:
                {
                    broadcastChatClosed(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_processNotificationEvent);
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
            TRACE_SCOPE(shared_generated_DirectMessagerClientBase_processNotificationEvent);
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
DEF_TRACE_SCOPE(shared_generated_DirectMessagerClientBase_invalidResponse);
void DirectMessagerClientBase::invalidResponse( NEDirectMessager::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client DirectMessagerClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEDirectMessager::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("DirectMessagerClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerClientBase_invalidRequest);
void DirectMessagerClientBase::invalidRequest( NEDirectMessager::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client DirectMessagerClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEDirectMessager::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_DirectMessagerClientBase_requestFailed);
void DirectMessagerClientBase::requestFailed( NEDirectMessager::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_DirectMessagerClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of DirectMessagerClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEDirectMessager::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectMessager::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEDirectMessager::MSG_ID_INVALID) )
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

void DirectMessagerClientBase::onChatParticipantsUpdate( const NEDirectMessager::ListParticipants & /* ChatParticipants */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_ChatParticipants) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void DirectMessagerClientBase::requestChatJoinFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_requestChatJoin) );
}

void DirectMessagerClientBase::requestMessageSendFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_requestMessageSend) );
}

void DirectMessagerClientBase::requestMessageTypeFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_requestMessageType) );
}

void DirectMessagerClientBase::requestChatLeaveFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_requestChatLeave) );
}

void DirectMessagerClientBase::responseChatJoin( bool /* succeed */, const NEDirectMessager::ListParticipants & /* listParticipant */, const DateTime & /* timeConnect */, const DateTime & /* timeConnected */ )
{
    ClientBase::responseNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_responseChatJoin) );
}

void DirectMessagerClientBase::broadcastMessageSent( const NEDirectMessager::sParticipant & /* sender */, const String & /* msgText */, const DateTime & /* timeSent */ )
{
    ClientBase::broadcastNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_broadcastMessageSent) );
}

void DirectMessagerClientBase::broadcastMessageTyped( const NEDirectMessager::sParticipant & /* participant */, const String & /* msgText */ )
{
    ClientBase::broadcastNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_broadcastMessageTyped) );
}

void DirectMessagerClientBase::broadcastParticipantJoined( const NEDirectMessager::sParticipant & /* participant */, const DateTime & /* timeJoined */ )
{
    ClientBase::broadcastNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_broadcastParticipantJoined) );
}

void DirectMessagerClientBase::broadcastParticipantLeft( const NEDirectMessager::sParticipant & /* participant */, const DateTime & /* timeLeft */ )
{
    ClientBase::broadcastNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_broadcastParticipantLeft) );
}

void DirectMessagerClientBase::broadcastChatClosed( void )
{
    ClientBase::broadcastNotImplemented( "DirectMessagerClientBase", static_cast<unsigned int>(NEDirectMessager::MSG_ID_broadcastChatClosed) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/DirectMessagerClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
