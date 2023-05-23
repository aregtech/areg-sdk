//////////////////////////////////////////////////////////////////////////
// Begin generate generated/private/DirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2023
 *
 * Generated at     05.01.2023  11:10:04 GMT+01:00
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 *
 * \file            generated/DirectConnectionClientBase.hpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection
 *                  Service Interface Client base class implementation.
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "generated/DirectConnectionClientBase.hpp"
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
    static constexpr FuncRequestFailure failureFunctions[]
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

DirectConnectionClientBase::DirectConnectionClientBase( const String & roleName, const String & ownerThread /* = String::getEmptyString() */ )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

DirectConnectionClientBase::DirectConnectionClientBase( const String & roleName, DispatcherThread & ownerThread )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), ownerThread) )
{
}

DirectConnectionClientBase::DirectConnectionClientBase( const String & roleName, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

DirectConnectionClientBase::DirectConnectionClientBase( const NERegistry::DependencyEntry & dependency, Component & owner )
    : IEProxyListener   ( )

    , mIsConnected      ( false )
    , mCurrSequenceNr   ( 0 )
    , mProxy            ( DirectConnectionProxy::createProxy(dependency.mRoleName, static_cast<IEProxyListener &>(self()), owner.getMasterThread()) )
{
}

DirectConnectionClientBase::~DirectConnectionClientBase( void )
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

bool DirectConnectionClientBase::recreateProxy( void )
{
    bool result         = false;
    if (mProxy != nullptr)
    {
        const String & roleName   = mProxy->getProxyAddress().getRoleName();
        const String & threadName = mProxy->getProxyAddress().getThread();
        if ( roleName.isEmpty() == false )
        {
            DirectConnectionProxy * newProxy = DirectConnectionProxy::createProxy(roleName, static_cast<IEProxyListener &>(self()), threadName);
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

DispatcherThread * DirectConnectionClientBase::getDispatcherThread( void )
{
    return ( mProxy != nullptr ? &(mProxy->getProxyDispatcherThread()) : nullptr );
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_serviceConnected);
bool DirectConnectionClientBase::serviceConnected( bool isConnected, ProxyBase & proxy )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_serviceConnected);

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

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_processNotificationEvent);
void DirectConnectionClientBase::processNotificationEvent( NotificationEvent & eventElem )
{
    const NotificationEventData & data  = static_cast<const NotificationEvent &>(eventElem).getData();
    NEService::eResultType result       = data.getNotifyType();
    NEDirectConnection::eMessageIDs msgId   = static_cast<NEDirectConnection::eMessageIDs>(data.getNotifyId());
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
            DirectConnectionClientBase::requestFailed(msgId, result);
        }
        break;

    case NEService::eResultType::RequestInvalid:
        {
        /************************************************************************
         * Trigger invalid response / broadcast handling. May happen when remove notification
         ************************************************************************/
            DirectConnectionClientBase::invalidResponse(msgId);
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
            case NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections:
                {
                    const NEDirectConnection::MapParticipants & InitiatedConnections = mProxy->getInitiatedConnections(dataState);
                    onInitiatedConnectionsUpdate(InitiatedConnections, dataState);
                }
                break;

            default:
                {
                    TRACE_SCOPE(generated_DirectConnectionClientBase_processNotificationEvent);
                    TRACE_ERR("Client object DirectConnectionClientBase of proxy [ %s ] received unexpected Attribute update notification message ID [ %d ]."
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
            case NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup:
                {
                    bool succeeded = mProxy->getParamsucceeded();
                    const NEDirectConnection::sParticipant & target = mProxy->getParamtarget();
                    const NEDirectConnection::sInitiator & initiator = mProxy->getParaminitiator();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->getParamlistParticipants();
                    responseConnectoinSetup( succeeded, target, initiator, listParticipants );
                }
                break;

            case NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant:
                {
                    bool succeeded = mProxy->getParamsucceeded();
                    const NEDirectConnection::ListParticipants & listParticipants = mProxy->getParamlistParticipants();
                    responseAddParticipant( succeeded, listParticipants );
                }
                break;

            case NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant:
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
                    TRACE_SCOPE(generated_DirectConnectionClientBase_processNotificationEvent);
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
            TRACE_SCOPE(generated_DirectConnectionClientBase_processNotificationEvent);
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
DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_invalidResponse);
void DirectConnectionClientBase::invalidResponse( NEDirectConnection::eMessageIDs InvalidRespId )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_invalidResponse);
    TRACE_ERR("The invalid response [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectConnectionClientBase is not implemented! Make error handling!"
                    , NEDirectConnection::getString(InvalidRespId)
                    , static_cast<unsigned int>(InvalidRespId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_invalidRequest);
void DirectConnectionClientBase::invalidRequest( NEDirectConnection::eMessageIDs InvalidReqId )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_invalidRequest);
    TRACE_ERR("The invalid request [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectConnectionClientBase is not implemented! Make error handling!"
                    , NEDirectConnection::getString(InvalidReqId)
                    , static_cast<unsigned int>(InvalidReqId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString() );

    ASSERT(false);
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_requestFailed);
void DirectConnectionClientBase::requestFailed( NEDirectConnection::eMessageIDs FailureMsgId, NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_requestFailed);
    TRACE_WARN("The request [ %s ] (value = [ %d ]) method of proxy [ %s ] client DirectConnectionClientBase failed with reason [ %s ]! Implemented error handling!"
                    , NEDirectConnection::getString(FailureMsgId)
                    , static_cast<unsigned int>(FailureMsgId)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason) );

    unsigned int index = static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_Invalid);
    index = static_cast<msg_id>( NEService::isResponseId(static_cast<unsigned int>(FailureMsgId)) ? NEDirectConnection::getRequestId(FailureMsgId) : FailureMsgId);
    index = NEService::isRequestId(index)  ? GET_REQ_INDEX(index) : static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_Invalid);
    if ( index != static_cast<msg_id>(NEDirectConnection::eMessageIDs::MsgId_Invalid) && (index < NEDirectConnection::getInterfaceData().idRequestCount) )
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

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_onInitiatedConnectionsUpdate);
void DirectConnectionClientBase::onInitiatedConnectionsUpdate( const NEDirectConnection::MapParticipants & /* InitiatedConnections */, NEService::eDataStateType /* state */ )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_onInitiatedConnectionsUpdate);
    TRACE_WARN("The attribute InitiatedConnections (value = %u) update method of proxy [ %s ] client DirectConnectionClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_InitiatedConnections)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

/************************************************************************
 * Request failure / Response and Broadcast notifications
 ************************************************************************/

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_requestConnectoinSetupFailed);
void DirectConnectionClientBase::requestConnectoinSetupFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_requestConnectoinSetupFailed);
    TRACE_WARN("The request requestConnectoinSetup (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_requestConnectoinSetup)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_requestAddParticipantFailed);
void DirectConnectionClientBase::requestAddParticipantFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_requestAddParticipantFailed);
    TRACE_WARN("The request requestAddParticipant (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_requestAddParticipant)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_requestRemoveParticipantFailed);
void DirectConnectionClientBase::requestRemoveParticipantFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_requestRemoveParticipantFailed);
    TRACE_WARN("The request requestRemoveParticipant (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_requestRemoveParticipant)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_requestCloseConnectionFailed);
void DirectConnectionClientBase::requestCloseConnectionFailed( NEService::eResultType FailureReason )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_requestCloseConnectionFailed);
    TRACE_WARN("The request requestCloseConnection (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is failed with reason [ %s ]! Make error handling!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_requestCloseConnection)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString()
                    , NEService::getString(FailureReason));
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_responseConnectoinSetup);
void DirectConnectionClientBase::responseConnectoinSetup( bool /* succeeded */, const NEDirectConnection::sParticipant & /* target */, const NEDirectConnection::sInitiator & /* initiator */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_responseConnectoinSetup);
    TRACE_WARN("The response responseConnectoinSetup (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_responseConnectoinSetup)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_responseAddParticipant);
void DirectConnectionClientBase::responseAddParticipant( bool /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_responseAddParticipant);
    TRACE_WARN("The response responseAddParticipant (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_responseAddParticipant)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

DEF_TRACE_SCOPE(generated_DirectConnectionClientBase_responseRemoveParticipant);
void DirectConnectionClientBase::responseRemoveParticipant( bool /* succeeded */, const NEDirectConnection::ListParticipants & /* listParticipants */ )
{
    TRACE_SCOPE(generated_DirectConnectionClientBase_responseRemoveParticipant);
    TRACE_WARN("The response responseRemoveParticipant (value = %u) method of proxy [ %s ] client DirectConnectionClientBase is not implemented!"
                    , static_cast<unsigned int>(NEDirectConnection::eMessageIDs::MsgId_responseRemoveParticipant)
                    , ProxyAddress::convAddressToPath(mProxy->getProxyAddress()).getString());
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/private/DirectConnectionClientBase.cpp file
//////////////////////////////////////////////////////////////////////////
