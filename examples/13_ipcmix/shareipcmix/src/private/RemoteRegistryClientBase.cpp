//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.06.2021  21:11:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryClientBase.cpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "shareipcmix/src/RemoteRegistryClientBase.hpp"
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
    static FuncRequestFailure failureFunctions[] = 
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

RemoteRegistryClientBase::RemoteRegistryClientBase( const char* roleName, const char * ownerThread /*= static_cast<const char *>(NULL)*/ )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

RemoteRegistryClientBase::RemoteRegistryClientBase( const char* roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
    ; // do nothing
}

RemoteRegistryClientBase::RemoteRegistryClientBase( const char* roleName, Component & owner )
    : IEProxyListener   ( )
    , ClientBase        ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( RemoteRegistryProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
    ; // do nothing
}

RemoteRegistryClientBase::~RemoteRegistryClientBase( void )
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

bool RemoteRegistryClientBase::recreateProxy( void )
{
    bool result         = false;
    String roleName   = mProxy != NULL ? mProxy->getProxyAddress().getRoleName() : "";
    String threadName = mProxy != NULL ? mProxy->getProxyAddress().getThread()   : "";
    if ( roleName.isEmpty() == false )
    {
        RemoteRegistryProxy * newProxy = RemoteRegistryProxy::createProxy(roleName.getString(), static_cast<IEProxyListener &>(self()), threadName.getString());
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

DispatcherThread * RemoteRegistryClientBase::getDispatcherThread( void )
{
    return ( mProxy != static_cast<RemoteRegistryProxy *>(NULL) ? &(mProxy->getProxyDispatcherThread()) : static_cast<DispatcherThread *>(NULL) );
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_serviceConnected);
bool RemoteRegistryClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_serviceConnected);
    
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

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_processNotificationEvent);
void RemoteRegistryClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NERemoteRegistry::eMessageIDs msgId   = static_cast<NERemoteRegistry::eMessageIDs>(data.getNotifyId());
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
            case NERemoteRegistry::MSG_ID_RegistryList:
                {
                    const NERemoteRegistry::ListRegistry & RegistryList = mProxy->getRegistryList(dataState);
                    onRegistryListUpdate(RegistryList, dataState);
                }
                break;

            case NERemoteRegistry::MSG_ID_RemainOutputs:
                {
                    unsigned int RemainOutputs = mProxy->getRemainOutputs(dataState);
                    onRemainOutputsUpdate(RemainOutputs, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_processNotificationEvent);
                    TRACE_ERR("Client object RemoteRegistryClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NERemoteRegistry::MSG_ID_responseRegister:
                {
                    const NERemoteRegistry::sClientRegister & client = mProxy->getParamclient();
                    responseRegister( client );
                }
                break;

            case NERemoteRegistry::MSG_ID_responseHelloWorld:
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
                    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_processNotificationEvent);
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
            TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_processNotificationEvent);
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
DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_invalidResponse);
void RemoteRegistryClientBase::invalidResponse( NERemoteRegistry::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_invalidResponse);
    TRACE_WARN(">>> There is an invalid response [ %s ] (value = [ %d ]) in client RemoteRegistryClientBase with path [ %s ], which cannot be processed! Make error handling! <<<"
                    , NERemoteRegistry::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ClientBase::responseInvalidNotImpelemnted("RemoteRegistryClientBase", static_cast<unsigned int>(InvalidRespId));
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_invalidRequest);
void RemoteRegistryClientBase::invalidRequest( NERemoteRegistry::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_invalidRequest);
    TRACE_WARN(">>> There is an invalid request [ %s ] (value = [ %d ]) in client RemoteRegistryClientBase with path [ %s ], which was not able to process! Make error handling! <<<"
                    , NERemoteRegistry::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );
                    
    ASSERT(false);
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_requestFailed);
void RemoteRegistryClientBase::requestFailed( NERemoteRegistry::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryClientBase_requestFailed);
    TRACE_WARN(">>> The request [ %s ] (value = [ %d ]) in Proxy [ %s ] of RemoteRegistryClientBase failed with reason [ %s ]! Triggering appropriate request failed function! <<<"
                    , NERemoteRegistry::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );
                    
    unsigned int index = static_cast<unsigned int>(NERemoteRegistry::MSG_ID_INVALID);
    index = static_cast<unsigned int>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NERemoteRegistry::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<unsigned int>(NERemoteRegistry::MSG_ID_INVALID);
    if ( index != static_cast<unsigned int>(NERemoteRegistry::MSG_ID_INVALID) )
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

void RemoteRegistryClientBase::onRegistryListUpdate( const NERemoteRegistry::ListRegistry & /* RegistryList */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_RegistryList) );
}

void RemoteRegistryClientBase::onRemainOutputsUpdate( unsigned int /* RemainOutputs */, NEService::eDataStateType /* state */ )
{
    ClientBase::onUpdateNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_RemainOutputs) );
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/
 
void RemoteRegistryClientBase::requestRegisterFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_requestRegister) );
}

void RemoteRegistryClientBase::requestUnregisterFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_requestUnregister) );
}

void RemoteRegistryClientBase::requestHelloWorldFailed( NEService::eResultType /* FailureReason */ )
{
    ClientBase::requestFailedNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_requestHelloWorld) );
}

void RemoteRegistryClientBase::responseRegister( const NERemoteRegistry::sClientRegister & /* client */ )
{
    ClientBase::responseNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_responseRegister) );
}

void RemoteRegistryClientBase::responseHelloWorld( unsigned int /* clientID */ )
{
    ClientBase::responseNotImplemented( "RemoteRegistryClientBase", static_cast<unsigned int>(NERemoteRegistry::MSG_ID_responseHelloWorld) );
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
