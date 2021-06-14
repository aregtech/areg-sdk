//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/LocalHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source LocalHelloWorld.
 * Generated at     11.06.2021  21:11:04 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/LocalHelloWorldClientBase.cpp
 * \ingroup         LocalHelloWorld Service Interface
 * \brief           This is an automatic generated code of LocalHelloWorld Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shareipcmix/src/LocalHelloWorldClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NELocalHelloWorld
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (LocalHelloWorldClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static FuncRequestFailure failureFunctions[] = 
    {
        &LocalHelloWorldClientBase::requestHelloWorldFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// LocalHelloWorldClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::LocalHelloWorldClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LocalHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

LocalHelloWorldClientBase::~LocalHelloWorldClientBase( void )
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

bool LocalHelloWorldClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        LocalHelloWorldProxy * newProxy = LocalHelloWorldProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * LocalHelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<LocalHelloWorldProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_serviceConnected);
bool LocalHelloWorldClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_serviceConnected);
    
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

void LocalHelloWorldClientBase::notifyOn( NELocalHelloWorld::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_processNotificationEvent);
void LocalHelloWorldClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NELocalHelloWorld::eMessageIDs msgId   = static_cast<NELocalHelloWorld::eMessageIDs>(data.getNotifyId());
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
            case NELocalHelloWorld::MSG_ID_ConnectedClients:
                {
                    const NELocalHelloWorld::ConnectionList & ConnectedClients = mProxy->getConnectedClients(dataState);
                    onConnectedClientsUpdate(ConnectedClients, dataState);
                }
                break;

            case NELocalHelloWorld::MSG_ID_RemainOutput:
                {
                    short RemainOutput = mProxy->getRemainOutput(dataState);
                    onRemainOutputUpdate(RemainOutput, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NELocalHelloWorld::MSG_ID_responseHelloWorld:
                {
                    const NELocalHelloWorld::sConnectedClient & clientInfo = mProxy->getParamclientInfo();
                    responseHelloWorld( clientInfo );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_processNotificationEvent);
            TRACE_ERR("Client object LocalHelloWorldClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_invalidResponse);
void LocalHelloWorldClientBase::invalidResponse( NELocalHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client LocalHelloWorldClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NELocalHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("LocalHelloWorldClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_invalidRequest);
void LocalHelloWorldClientBase::invalidRequest( NELocalHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client LocalHelloWorldClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NELocalHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_requestFailed);
void LocalHelloWorldClientBase::requestFailed( NELocalHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shareipcmix_src_LocalHelloWorldClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of LocalHelloWorldClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NELocalHelloWorld::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NELocalHelloWorld::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_INVALID) )
    {
        (this->*NELocalHelloWorld::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

void LocalHelloWorldClientBase::onConnectedClientsUpdate( const NELocalHelloWorld::ConnectionList & /* ConnectedClients */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "LocalHelloWorldClientBase", static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_ConnectedClients) );
}

void LocalHelloWorldClientBase::onRemainOutputUpdate( short /* RemainOutput */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "LocalHelloWorldClientBase", static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_RemainOutput) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void LocalHelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "LocalHelloWorldClientBase", static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_requestHelloWorld) );
}

void LocalHelloWorldClientBase::responseHelloWorld( const NELocalHelloWorld::sConnectedClient & /* clientInfo */ )
{
    ClientBase::responseNotImplemented( "LocalHelloWorldClientBase", static_cast<unsigned int>(NELocalHelloWorld::MSG_ID_responseHelloWorld) );
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/LocalHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
