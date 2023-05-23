//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/HelloUnblockClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:56 GMT+01:00
 *                  Create by AREG SDK code generator tool from source HelloUnblock.
 *
 * \file            generated/src/HelloUnblockClientBase.hpp
 * \ingroup         HelloUnblock Service Interface
 * \brief           This is an automatic generated code of HelloUnblock
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/HelloUnblockClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEHelloUnblock
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (HelloUnblockClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &HelloUnblockClientBase::requestIdentifierFailed
        , &HelloUnblockClientBase::requestHelloUblockFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// HelloUnblockClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

HelloUnblockClientBase::HelloUnblockClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloUnblockProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloUnblockClientBase::HelloUnblockClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloUnblockProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

HelloUnblockClientBase::HelloUnblockClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloUnblockProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloUnblockClientBase::HelloUnblockClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( HelloUnblockProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

HelloUnblockClientBase::~HelloUnblockClientBase( void )
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

bool HelloUnblockClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            HelloUnblockProxy * newProxy = HelloUnblockProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * HelloUnblockClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_serviceConnected);
bool HelloUnblockClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_serviceConnected);

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

void HelloUnblockClientBase::notifyOn( NEHelloUnblock::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_processNotificationEvent);
void HelloUnblockClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEHelloUnblock::eMessageIDs msgId   = static_cast<NEHelloUnblock::eMessageIDs>(data.getNotifyId());
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
            HelloUnblockClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            HelloUnblockClientBase::invalidResponse(msgId);
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
            case NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState:
                {
                    NEHelloUnblock::eServiceState HelloServiceState = mProxy->getHelloServiceState(dataState);
                    onHelloServiceStateUpdate(HelloServiceState, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_HelloUnblockClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloUnblockClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier:
                {
                    unsigned int clientId = mProxy->getParamclientId();
                    responseIdentifier( clientId );
                }
                break;

            case NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock:
                {
                    unsigned int clientId = mProxy->getParamclientId();
                    unsigned int seqNr = mProxy->getParamseqNr();
                    responseHelloUnblock( clientId, seqNr );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_HelloUnblockClientBase_processNotificationEvent);
                    TRACE_ERR("Client object HelloUnblockClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_HelloUnblockClientBase_processNotificationEvent);
            TRACE_ERR("Client object HelloUnblockClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_invalidResponse);
void HelloUnblockClientBase::invalidResponse( NEHelloUnblock::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloUnblockClientBase is not implemented! Make error handling!"
                    , NEHelloUnblock::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_invalidRequest);
void HelloUnblockClientBase::invalidRequest( NEHelloUnblock::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloUnblockClientBase is not implemented! Make error handling!"
                    , NEHelloUnblock::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestFailed);
void HelloUnblockClientBase::requestFailed( NEHelloUnblock::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client HelloUnblockClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEHelloUnblock::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEHelloUnblock::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEHelloUnblock::eMessageIDs::MsgId_Invalid) && (index < NEHelloUnblock::getInterfaceData().idRequestCount) )
    {
        (this->*NEHelloUnblock::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_onHelloServiceStateUpdate);
void HelloUnblockClientBase::onHelloServiceStateUpdate( NEHelloUnblock::eServiceState /* HelloServiceState */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_onHelloServiceStateUpdate);
    TRACE_WARN("The attribute HelloServiceState (value = %u) update method of proxy [ %s ] client HelloUnblockClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloUnblock::eMessageIDs::MsgId_HelloServiceState)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestIdentifierFailed);
void HelloUnblockClientBase::requestIdentifierFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestIdentifierFailed);
    TRACE_WARN("The request requestIdentifier (value = %u) method of proxy [ %s ] client HelloUnblockClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloUnblock::eMessageIDs::MsgId_requestIdentifier)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestHelloUblockFailed);
void HelloUnblockClientBase::requestHelloUblockFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_requestHelloUblockFailed);
    TRACE_WARN("The request requestHelloUblock (value = %u) method of proxy [ %s ] client HelloUnblockClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEHelloUnblock::eMessageIDs::MsgId_requestHelloUblock)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_responseIdentifier);
void HelloUnblockClientBase::responseIdentifier( unsigned int /* clientId */ )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_responseIdentifier);
    TRACE_WARN("The response responseIdentifier (value = %u) method of proxy [ %s ] client HelloUnblockClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloUnblock::eMessageIDs::MsgId_responseIdentifier)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_HelloUnblockClientBase_responseHelloUnblock);
void HelloUnblockClientBase::responseHelloUnblock( unsigned int /* clientId */, unsigned int /* seqNr */ )
{
    TRACE_SCOPE(generated_src_HelloUnblockClientBase_responseHelloUnblock);
    TRACE_WARN("The response responseHelloUnblock (value = %u) method of proxy [ %s ] client HelloUnblockClientBase is not implemented!"
                    , static_cast<unsigned int>(NEHelloUnblock::eMessageIDs::MsgId_responseHelloUnblock)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/HelloUnblockClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
