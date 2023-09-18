//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/LargeDataClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     18.09.2023  09:14:52 GMT+02:00
 *                  Create by AREG SDK code generator tool from source LargeData.
 *
 * \file            generated/src/LargeDataClientBase.hpp
 * \ingroup         LargeData Service Interface
 * \brief           This is an automatic generated code of LargeData
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/src/LargeDataClientBase.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/trace/GETrace.h"

/************************************************************************
 * Request error handling initialization
 ************************************************************************/
namespace NELargeData
{
    /**
     * \brief   Initialize request failure function pointers to make error handling
     **/
    typedef void (LargeDataClientBase::* FuncRequestFailure) ( NEService::eResultType );
#ifdef  _DEBUG
    static FuncRequestFailure failureFunctions = nullptr;
#endif  // _DEBUG
}

//////////////////////////////////////////////////////////////////////////
// LargeDataClientBase class implementation
//////////////////////////////////////////////////////////////////////////

/************************************************************************
 * Constructor / Destructor
 ************************************************************************/

LargeDataClientBase::LargeDataClientBase( const String & roleName, const String & ownerThread /* = String::EmptyString */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LargeDataProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

LargeDataClientBase::LargeDataClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LargeDataProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

LargeDataClientBase::LargeDataClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LargeDataProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

LargeDataClientBase::LargeDataClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( LargeDataProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

LargeDataClientBase::~LargeDataClientBase( void )
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

bool LargeDataClientBase::recreateProxy( void )
{
    bool result { false };
    if (mProxy != nullptr)
    {
        const String & roleName   { mProxy->getProxyAddress().getRoleName() };
        const String & threadName { mProxy->getProxyAddress().getThread()   };
        if ( roleName.isEmpty() == false )
        {
            LargeDataProxy * newProxy = LargeDataProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * LargeDataClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_serviceConnected);
bool LargeDataClientBase::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_serviceConnected);

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

void LargeDataClientBase::notifyOn( NELargeData::eMessageIDs msgId, bool notify, bool always /* = false */ )
{
    if (notify)
        mProxy->setNotification(msgId, static_cast<IENotificationEventConsumer &>(self()), always);
    else
        mProxy->clearNotification(msgId, static_cast<IENotificationEventConsumer &>(self()));
}

/************************************************************************
 * Event processing
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_processNotificationEvent);
void LargeDataClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  { static_cast<const NotificationEvent &>(eventElem).getData() };
    NEService::eResultType result       { data.getNotifyType() };
    NELargeData::eMessageIDs msgId   { static_cast<NELargeData::eMessageIDs>(data.getNotifyId()) };
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
            LargeDataClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            LargeDataClientBase::invalidResponse(msgId);
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
        /************************************************************************
         * Trigger broadcast processing
         ************************************************************************/
            case NELargeData::eMessageIDs::MsgId_broadcastImageBlockAcquired:
                {
                    const NELargeData::ImageBlock & imageBlock = mProxy->getParamimageBlock();
                    broadcastImageBlockAcquired( imageBlock );
                }
                break;

            case NELargeData::eMessageIDs::MsgId_broadcastServiceStopping:
                {
                    broadcastServiceStopping(  );
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_src_LargeDataClientBase_processNotificationEvent);
                    TRACE_ERR("Client object LargeDataClientBase of proxy [ %s ] received unexpected Response message ID [ %d ]."
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
            TRACE_SCOPE(generated_src_LargeDataClientBase_processNotificationEvent);
            TRACE_ERR("Client object LargeDataClientBase of proxy [ %s ] received unexpected Response result type [ %d ]."
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
DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_invalidResponse);
void LargeDataClientBase::invalidResponse( NELargeData::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client LargeDataClientBase is not implemented! Make error handling!"
                    , NELargeData::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_invalidRequest);
void LargeDataClientBase::invalidRequest( NELargeData::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client LargeDataClientBase is not implemented! Make error handling!"
                    , NELargeData::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_requestFailed);
void LargeDataClientBase::requestFailed( NELargeData::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client LargeDataClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NELargeData::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    ASSERT(NELargeData::failureFunctions == nullptr);
    invalidRequest( FailureMsgId );
}

/************************************************************************
 * Attribute notifications
 ************************************************************************/

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_broadcastImageBlockAcquired);
void LargeDataClientBase::broadcastImageBlockAcquired( const NELargeData::ImageBlock & /* imageBlock */ )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_broadcastImageBlockAcquired);
    TRACE_WARN("The broadcast broadcastImageBlockAcquired (value = %u) method of proxy [ %s ] client LargeDataClientBase is not implemented!"
                    , static_cast<unsigned int>(NELargeData::eMessageIDs::MsgId_broadcastImageBlockAcquired)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_src_LargeDataClientBase_broadcastServiceStopping);
void LargeDataClientBase::broadcastServiceStopping( void )
{
    TRACE_SCOPE(generated_src_LargeDataClientBase_broadcastServiceStopping);
    TRACE_WARN("The broadcast broadcastServiceStopping (value = %u) method of proxy [ %s ] client LargeDataClientBase is not implemented!"
                    , static_cast<unsigned int>(NELargeData::eMessageIDs::MsgId_broadcastServiceStopping)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/LargeDataClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
