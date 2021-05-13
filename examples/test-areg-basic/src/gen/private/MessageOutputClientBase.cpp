//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/MessageOutputClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/MessageOutputClientBase.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "src/gen/MessageOutputClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEMessageOutput
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (MessageOutputClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &MessageOutputClientBase::requestOutputMessageFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// MessageOutputClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

MessageOutputClientBase::MessageOutputClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( MessageOutputProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

MessageOutputClientBase::MessageOutputClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( MessageOutputProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

MessageOutputClientBase::MessageOutputClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( MessageOutputProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

MessageOutputClientBase::~MessageOutputClientBase( void )
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

bool MessageOutputClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        MessageOutputProxy * newProxy = MessageOutputProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * MessageOutputClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<MessageOutputProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void MessageOutputClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(src_gen_MessageOutputClientBase_ServiceConnected);
bool MessageOutputClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(src_gen_MessageOutputClientBase_ServiceConnected);
    
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

void MessageOutputClientBase::notifyOn( NEMessageOutput::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_MessageOutputClientBase_processNotificationEvent);
void MessageOutputClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEMessageOutput::eMessageIDs msgId   = static_cast<NEMessageOutput::eMessageIDs>(data.getNotifyId());
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
            case NEMessageOutput::MSG_ID_LastMessage:
                {
                    const String & LastMessage = mProxy->getLastMessage(dataState);
                    onLastMessageUpdate(LastMessage, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_MessageOutputClientBase_processNotificationEvent);
                    TRACE_ERR("Client object MessageOutputClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NEMessageOutput::MSG_ID_broadcastActionRequest:
                {
                    NEMessageOutput::eRequestedActons userAction = mProxy->getParamuserAction();
                    broadcastActionRequest( userAction );
                }
                break;

            case NEMessageOutput::MSG_ID_broadcastInputMessage:
                {
                    const String & msgInput = mProxy->getParammsgInput();
                    broadcastInputMessage( msgInput );
                }
                break;

            default:
                {
                    TRACE_SCOPE(src_gen_MessageOutputClientBase_processNotificationEvent);
                    TRACE_ERR("Client object MessageOutputClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(src_gen_MessageOutputClientBase_processNotificationEvent);
            TRACE_ERR("Client object MessageOutputClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(src_gen_MessageOutputClientBase_invalidResponse);
void MessageOutputClientBase::invalidResponse( NEMessageOutput::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(src_gen_MessageOutputClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client MessageOutputClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEMessageOutput::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("MessageOutputClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(src_gen_MessageOutputClientBase_invalidRequest);
void MessageOutputClientBase::invalidRequest( NEMessageOutput::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(src_gen_MessageOutputClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client MessageOutputClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEMessageOutput::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(src_gen_MessageOutputClientBase_requestFailed);
void MessageOutputClientBase::requestFailed( NEMessageOutput::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(src_gen_MessageOutputClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of MessageOutputClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEMessageOutput::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEMessageOutput::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEMessageOutput::MSG_ID_INVALID) )
    {
        (this->*NEMessageOutput::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void MessageOutputClientBase::onLastMessageUpdate( const String & /* LastMessage */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "MessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_LastMessage) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void MessageOutputClientBase::requestOutputMessageFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "MessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_requestOutputMessage) );
}

void MessageOutputClientBase::broadcastActionRequest( NEMessageOutput::eRequestedActons /* userAction */ )
{
    ClientBase::broadcastNotImplemented( "MessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_broadcastActionRequest) );
}

void MessageOutputClientBase::broadcastInputMessage( const String & /* msgInput */ )
{
    ClientBase::broadcastNotImplemented( "MessageOutputClientBase", static_cast<unsigned int>(NEMessageOutput::MSG_ID_broadcastInputMessage) );
}


//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/MessageOutputClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
