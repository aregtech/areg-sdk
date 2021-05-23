//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     23.05.2021  00:18:58 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectConnectionClientBase.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shared/generated/DirectConnectionClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEDirectConnection
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (DirectConnectionClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
          &DirectConnectionClientBase::requestConnectoinSetupFailed
        , &DirectConnectionClientBase::requestAddParticipantFailed
        , &DirectConnectionClientBase::requestRemoveParticipantFailed
        , &DirectConnectionClientBase::requestCloseConnectionFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// DirectConnectionClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

DirectConnectionClientBase::DirectConnectionClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

DirectConnectionClientBase::DirectConnectionClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

DirectConnectionClientBase::DirectConnectionClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

DirectConnectionClientBase::~DirectConnectionClientBase( void )
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

bool DirectConnectionClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        DirectConnectionProxy * newProxy = DirectConnectionProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * DirectConnectionClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<DirectConnectionProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

void DirectConnectionClientBase::clearAllNotifications( void )
{
    mProxy->clearAllNotifications(static_cast<IENotificationEventConsumer &>(self()));
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionClientBase_ServiceConnected);
bool DirectConnectionClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_ServiceConnected);
    
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

void DirectConnectionClientBase::notifyOn( NEDirectConnection::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_DirectConnectionClientBase_processNotificationEvent);
void DirectConnectionClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEDirectConnection::eMessageIDs msgId   = static_cast<NEDirectConnection::eMessageIDs>(data.getNotifyId());
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
            case NEDirectConnection::MSG_ID_InitiatedConnections:
                {
                    const NEDirectConnection::MapParticipants & InitiatedConnections = mProxy->getInitiatedConnections(dataState);
                    onInitiatedConnectionsUpdate(InitiatedConnections, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectConnectionClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEDirectConnection::MSG_ID_responseConnectoinSetup:
                {
                    bool succeeded = mProxy->getParamsucceeded();
                    const NEDirectConnection::sParticipant & target = mProxy->getParamtarget();
                    const NEDirectConnection::sInitiator & initiator = mProxy->getParaminitiator();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->getParamlistParticipants();
                    responseConnectoinSetup( succeeded, target, initiator, listParticipants );
                }
                break;

            case NEDirectConnection::MSG_ID_responseAddParticipant:
                {
                    bool succeeded = mProxy->getParamsucceeded();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->getParamlistParticipants();
                    responseAddParticipant( succeeded, listParticipants );
                }
                break;

            case NEDirectConnection::MSG_ID_responseRemoveParticipant:
                {
                    bool succeeded = mProxy->getParamsucceeded();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->getParamlistParticipants();
                    responseRemoveParticipant( succeeded, listParticipants );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectConnectionClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shared_generated_DirectConnectionClientBase_processNotificationEvent);
            TRACE_ERR("Client object DirectConnectionClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shared_generated_DirectConnectionClientBase_invalidResponse);
void DirectConnectionClientBase::invalidResponse( NEDirectConnection::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client DirectConnectionClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NEDirectConnection::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("DirectConnectionClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionClientBase_invalidRequest);
void DirectConnectionClientBase::invalidRequest( NEDirectConnection::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client DirectConnectionClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NEDirectConnection::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionClientBase_requestFailed);
void DirectConnectionClientBase::requestFailed( NEDirectConnection::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shared_generated_DirectConnectionClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of DirectConnectionClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NEDirectConnection::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectConnection::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NEDirectConnection::MSG_ID_INVALID) )
    {
        (this->*NEDirectConnection::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void DirectConnectionClientBase::onInitiatedConnectionsUpdate( const NEDirectConnection::MapParticipants & /* InitiatedConnections */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_InitiatedConnections) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void DirectConnectionClientBase::requestConnectoinSetupFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_requestConnectoinSetup) );
}

void DirectConnectionClientBase::requestAddParticipantFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_requestAddParticipant) );
}

void DirectConnectionClientBase::requestRemoveParticipantFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_requestRemoveParticipant) );
}

void DirectConnectionClientBase::requestCloseConnectionFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_requestCloseConnection) );
}

void DirectConnectionClientBase::responseConnectoinSetup( bool /* succeeded */, const NEDirectConnection::sParticipant & /* target */, const NEDirectConnection::sInitiator & /* initiator */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    ClientBase::responseNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_responseConnectoinSetup) );
}

void DirectConnectionClientBase::responseAddParticipant( bool /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    ClientBase::responseNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_responseAddParticipant) );
}

void DirectConnectionClientBase::responseRemoveParticipant( bool /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    ClientBase::responseNotImplemented( "DirectConnectionClientBase", static_cast<unsigned int>(NEDirectConnection::MSG_ID_responseRemoveParticipant) );
}


//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/DirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
