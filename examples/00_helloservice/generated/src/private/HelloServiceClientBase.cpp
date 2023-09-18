//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloServiceClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:47 GMT+02:00
 *                  Create by AREG SDK code generator tool from source HelloService.
 *
 * \file            generated/src/HelloServiceClientBase.hpp
 * \ingroup         HelloService Service Interface
 * \brief           This is an automatic generated code of HelloService
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/HelloServiceClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEHelloService
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (HelloServiceClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
        &HelloServiceClientBase::requestHelloServiceFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// HelloServiceClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

HelloServiceClientBase::HelloServiceClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloServiceProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloServiceClientBase::HelloServiceClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloServiceProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloServiceClientBase::HelloServiceClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloServiceProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloServiceClientBase::HelloServiceClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloServiceProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloServiceClientBase::~HelloServiceClientBase( void )
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

bool HelloServiceClientBase::recreateProxy( void )
{
    bool result { false };
    if (mProxy != nullptr)
    {
        const String & roleName   { mProxy->getProxyAddress().getRoleName() };
        const String & threadName { mProxy->getProxyAddress().getThread()   };
        if ( roleName.isEmpty() == false )
        {
            HelloServiceProxy * newProxy = HelloServiceProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * HelloServiceClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_serviceConnected);
bool HelloServiceClientBase::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_serviceConnected);

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

void HelloServiceClientBase::notifyOn( NEHelloService::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_processNotificationEvent);
void HelloServiceClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  { static_cast<const NotificationEvent &>(eventElem).getData() };
    NEService::eResultType result       { data.getNotifyType() };
    NEHelloService::eMessageIDs msgId   { static_cast<NEHelloService::eMessageIDs>(data.getNotifyId()) };
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
            HelloServiceClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            HelloServiceClientBase::invalidResponse(msgId);
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
            case NEHelloService::eMessageIDs::MsgId_responseHelloService:
                {
                    bool success = mProxy->getParamsuccess();
                    responseHelloService( success );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_HelloServiceClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloServiceClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_HelloServiceClientBase_processNotificationEvent);
            TRACE_ERR("Client object HelloServiceClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_invalidResponse);
void HelloServiceClientBase::invalidResponse( NEHelloService::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloServiceClientBase is not implemented! Make error handling!"
                    , NEHelloService::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_invalidRequest);
void HelloServiceClientBase::invalidRequest( NEHelloService::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloServiceClientBase is not implemented! Make error handling!"
                    , NEHelloService::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_requestFailed);
void HelloServiceClientBase::requestFailed( NEHelloService::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloServiceClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEHelloService::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    msg_id index { static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEHelloService::getRequestId(FailureMsgId) : FailureMsgId) };
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEHelloService::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEHelloService::eMessageIDs::MsgId_Invalid) && (index < NEHelloService::getInterfaceData().idRequestCount) )
    {
        (this->*NEHelloService::failureFunctions[index])( FailureReason );
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

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_requestHelloServiceFailed);
void HelloServiceClientBase::requestHelloServiceFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_requestHelloServiceFailed);
    TRACE_WARN("The request requestHelloService (value = %u) method of proxy [ %s ] client HelloServiceClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloService::eMessageIDs::MsgId_requestHelloService)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloServiceClientBase_responseHelloService);
void HelloServiceClientBase::responseHelloService( bool /* success */ )
{
    TRACE_SCOPE(generated_src_HelloServiceClientBase_responseHelloService);
    TRACE_WARN("The response responseHelloService (value = %u) method of proxy [ %s ] client HelloServiceClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloService::eMessageIDs::MsgId_responseHelloService)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloServiceClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
