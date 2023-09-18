//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/PublicHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     17.09.2023  00:33:59 GMT+02:00
 *                  Create by AREG SDK code generator tool from source PublicHelloWorld.
 *
 * \file            generated/src/PublicHelloWorldClientBase.hpp
 * \ingroup         PublicHelloWorld Service Interface
 * \brief           This is an automatic generated code of PublicHelloWorld
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/PublicHelloWorldClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEPublicHelloWorld
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (PublicHelloWorldClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &PublicHelloWorldClientBase::requestRegisterFailed
        , &PublicHelloWorldClientBase::requestUnregisterFailed
        , &PublicHelloWorldClientBase::requestHelloWorldFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// PublicHelloWorldClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

PublicHelloWorldClientBase::PublicHelloWorldClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PublicHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PublicHelloWorldClientBase::PublicHelloWorldClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PublicHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

PublicHelloWorldClientBase::PublicHelloWorldClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PublicHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PublicHelloWorldClientBase::PublicHelloWorldClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( PublicHelloWorldProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

PublicHelloWorldClientBase::~PublicHelloWorldClientBase( void )
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

bool PublicHelloWorldClientBase::recreateProxy( void )
{
    bool result { false };
    if (mProxy != nullptr)
    {
        const String & roleName   { mProxy->getProxyAddress().getRoleName() };
        const String & threadName { mProxy->getProxyAddress().getThread()   };
        if ( roleName.isEmpty() == false )
        {
            PublicHelloWorldProxy * newProxy = PublicHelloWorldProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * PublicHelloWorldClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_serviceConnected);
bool PublicHelloWorldClientBase::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_serviceConnected);

    bool result { false };
    if(mProxy == &proxy)
    {
        mIsConnected= NEService::isServiceConnected(status);
        result      = true;

        TRACE_DBG("Client [ %p ]: The Service [ %s ] with Role Name [ %s ] is [ %s ]"
                 , this
                 , proxy.getProxyAddress().getServiceName().getString()
                 , proxy.getProxyAddress().getRoleName().getString()
                 , mIsConnected ? "CONNECTED" : "DISCONNECTED");
    }

    return result;
}

void PublicHelloWorldClientBase::notifyOn( NEPublicHelloWorld::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_processNotificationEvent);
void PublicHelloWorldClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  { static_cast<const NotificationEvent &>(eventElem).getData() };
    NEService::eResultType result       { data.getNotifyType() };
    NEPublicHelloWorld::eMessageIDs msgId   { static_cast<NEPublicHelloWorld::eMessageIDs>(data.getNotifyId()) };
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
            PublicHelloWorldClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            PublicHelloWorldClientBase::invalidResponse(msgId);
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
            case NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister:
                {
                    const NEPublicHelloWorld::sClientRegister & client = mProxy->getParamclient();
                    responseRegister( client );
                }
                break;

            case NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld:
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
                    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_processNotificationEvent);
                    TRACE_ERR("Client object PublicHelloWorldClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_processNotificationEvent);
            TRACE_ERR("Client object PublicHelloWorldClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_invalidResponse);
void PublicHelloWorldClientBase::invalidResponse( NEPublicHelloWorld::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client PublicHelloWorldClientBase is not implemented! Make error handling!"
                    , NEPublicHelloWorld::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_invalidRequest);
void PublicHelloWorldClientBase::invalidRequest( NEPublicHelloWorld::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PublicHelloWorldClientBase is not implemented! Make error handling!"
                    , NEPublicHelloWorld::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestFailed);
void PublicHelloWorldClientBase::requestFailed( NEPublicHelloWorld::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client PublicHelloWorldClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEPublicHelloWorld::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    msg_id index { static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEPublicHelloWorld::getRequestId(FailureMsgId) : FailureMsgId) };
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEPublicHelloWorld::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEPublicHelloWorld::eMessageIDs::MsgId_Invalid) && (index < NEPublicHelloWorld::getInterfaceData().idRequestCount) )
    {
        (this->*NEPublicHelloWorld::failureFunctions[index])( FailureReason );
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

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestRegisterFailed);
void PublicHelloWorldClientBase::requestRegisterFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestRegisterFailed);
    TRACE_WARN("The request requestRegister (value = %u) method of proxy [ %s ] client PublicHelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPublicHelloWorld::eMessageIDs::MsgId_requestRegister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestUnregisterFailed);
void PublicHelloWorldClientBase::requestUnregisterFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestUnregisterFailed);
    TRACE_WARN("The request requestUnregister (value = %u) method of proxy [ %s ] client PublicHelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPublicHelloWorld::eMessageIDs::MsgId_requestUnregister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestHelloWorldFailed);
void PublicHelloWorldClientBase::requestHelloWorldFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_requestHelloWorldFailed);
    TRACE_WARN("The request requestHelloWorld (value = %u) method of proxy [ %s ] client PublicHelloWorldClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEPublicHelloWorld::eMessageIDs::MsgId_requestHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_responseRegister);
void PublicHelloWorldClientBase::responseRegister( const NEPublicHelloWorld::sClientRegister & /* client */ )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_responseRegister);
    TRACE_WARN("The response responseRegister (value = %u) method of proxy [ %s ] client PublicHelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPublicHelloWorld::eMessageIDs::MsgId_responseRegister)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_responseHelloWorld);
void PublicHelloWorldClientBase::responseHelloWorld( unsigned int /* clientID */ )
{
    TRACE_SCOPE(generated_src_PublicHelloWorldClientBase_responseHelloWorld);
    TRACE_WARN("The response responseHelloWorld (value = %u) method of proxy [ %s ] client PublicHelloWorldClientBase is not implemented!"
                    , static_cast<unsigned int>(NEPublicHelloWorld::eMessageIDs::MsgId_responseHelloWorld)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/PublicHelloWorldClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
