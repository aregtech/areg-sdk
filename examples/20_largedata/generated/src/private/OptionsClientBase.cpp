//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/OptionsClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/OptionsClientBase.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/OptionsClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NEOptions
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (OptionsClientBase::* FuncRequestFailure) ( NEService::eResultType );
    static constexpr FuncRequestFailure failureFunctions[]
    {
          &OptionsClientBase::requestDataRateFailed
        , &OptionsClientBase::requestPacketRateFailed
        , &OptionsClientBase::requestChangeOptionsFailed
        , &OptionsClientBase::requestQuitApplicationFailed
    };
}

//////////////////////////////////////////////////////////////////////////
// OptionsClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

OptionsClientBase::OptionsClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( OptionsProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

OptionsClientBase::OptionsClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( OptionsProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

OptionsClientBase::OptionsClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( OptionsProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

OptionsClientBase::~OptionsClientBase( void )
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

bool OptionsClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            OptionsProxy * newProxy = OptionsProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * OptionsClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_serviceConnected);
bool OptionsClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_serviceConnected);

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

void OptionsClientBase::notifyOn( NEOptions::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_processNotificationEvent);
void OptionsClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEOptions::eMessageIDs msgId   = static_cast<NEOptions::eMessageIDs>(data.getNotifyId());
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
            OptionsClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            OptionsClientBase::invalidResponse(msgId);
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
            case NEOptions::eMessageIDs::MsgId_OptionData:
                {
                    const NEOptions::sOptionData & OptionData = mProxy->getOptionData(dataState);
                    onOptionDataUpdate(OptionData, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_OptionsClientBase_processNotificationEvent);
                    TRACE_ERR("Client object OptionsClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEOptions::eMessageIDs::MsgId_responseDataRate:
                {
                    const NEOptions::sDataRate & sentData = mProxy->getParamsentData();
                    const NEOptions::sDataRate & missedData = mProxy->getParammissedData();
                    responseDataRate( sentData, missedData );
                }
                break;

            case NEOptions::eMessageIDs::MsgId_responsePacketRate:
                {
                    unsigned int genPackets = mProxy->getParamgenPackets();
                    const NEOptions::sDataRate & avSize = mProxy->getParamavSize();
                    responsePacketRate( genPackets, avSize );
                }
                break;

        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            default:
                {
                    TRACE_SCOPE(generated_src_OptionsClientBase_processNotificationEvent);
                    TRACE_ERR("Client object OptionsClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_OptionsClientBase_processNotificationEvent);
            TRACE_ERR("Client object OptionsClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_OptionsClientBase_invalidResponse);
void OptionsClientBase::invalidResponse( NEOptions::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client OptionsClientBase is not implemented! Make error handling!"
                    , NEOptions::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_invalidRequest);
void OptionsClientBase::invalidRequest( NEOptions::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client OptionsClientBase is not implemented! Make error handling!"
                    , NEOptions::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_requestFailed);
void OptionsClientBase::requestFailed( NEOptions::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client OptionsClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEOptions::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEOptions::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_Invalid) && (index < NEOptions::getInterfaceData().idRequestCount) )
    {
        (this->*NEOptions::failureFunctions[index])( FailureReason );
    }
    else
    {
        invalidRequest( FailureMsgId );
    }
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_onOptionDataUpdate);
void OptionsClientBase::onOptionDataUpdate( const NEOptions::sOptionData & /* OptionData */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_onOptionDataUpdate);
    TRACE_WARN("The attribute OptionData (value = %u) update method of proxy [ %s ] client OptionsClientBase is not implemented!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_OptionData)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_requestDataRateFailed);
void OptionsClientBase::requestDataRateFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_requestDataRateFailed);
    TRACE_WARN("The request requestDataRate (value = %u) method of proxy [ %s ] client OptionsClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_requestDataRate)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_requestPacketRateFailed);
void OptionsClientBase::requestPacketRateFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_requestPacketRateFailed);
    TRACE_WARN("The request requestPacketRate (value = %u) method of proxy [ %s ] client OptionsClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_requestPacketRate)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_requestChangeOptionsFailed);
void OptionsClientBase::requestChangeOptionsFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_requestChangeOptionsFailed);
    TRACE_WARN("The request requestChangeOptions (value = %u) method of proxy [ %s ] client OptionsClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_requestChangeOptions)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_requestQuitApplicationFailed);
void OptionsClientBase::requestQuitApplicationFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_requestQuitApplicationFailed);
    TRACE_WARN("The request requestQuitApplication (value = %u) method of proxy [ %s ] client OptionsClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_requestQuitApplication)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_responseDataRate);
void OptionsClientBase::responseDataRate( const NEOptions::sDataRate & /* sentData */, const NEOptions::sDataRate & /* missedData */ )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_responseDataRate);
    TRACE_WARN("The response responseDataRate (value = %u) method of proxy [ %s ] client OptionsClientBase is not implemented!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_responseDataRate)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_OptionsClientBase_responsePacketRate);
void OptionsClientBase::responsePacketRate( unsigned int /* genPackets */, const NEOptions::sDataRate & /* avSize */ )
{
    TRACE_SCOPE(generated_src_OptionsClientBase_responsePacketRate);
    TRACE_WARN("The response responsePacketRate (value = %u) method of proxy [ %s ] client OptionsClientBase is not implemented!"
                    , static_cast<unsigned int>(NEOptions::eMessageIDs::MsgId_responsePacketRate)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/OptionsClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
