//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/OptionsStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2022
 *
 * Generated at     20.09.2022  23:32:53 GMT+02:00
 *                  Create by AREG SDK code generator tool from source Options.
 *
 * \file            generated/src/OptionsStub.hpp
 * \ingroup         Options Service Interface
 * \brief           This is an automatic generated code of Options
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/OptionsStub.hpp"
#include "generated/src/private/OptionsEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// OptionsStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
OptionsStub::OptionsStub( Component & masterComp )
    : StubBase    ( masterComp, NEOptions::getInterfaceData() )

    , mOptionData         (  )
    , mOptionDataState    ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void OptionsStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_OptionsStub_startupServiceInterface);
void OptionsStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_OptionsStub_startupServiceInterface);

    OptionsRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    OptionsNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_OptionsStub_shutdownServiceIntrface);
void OptionsStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_OptionsStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());

    OptionsRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    OptionsNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), holder.getMasterThread() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * OptionsStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW OptionsResponseEvent(data, proxy, result, msgId) : DEBUG_NEW OptionsResponseEvent(proxy, result, msgId));
}

DEF_TRACE_SCOPE(generated_src_OptionsStub_sendNotification);
void OptionsStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEOptions::eMessageIDs>(msgId) )
    {
    case NEOptions::eMessageIDs::MsgId_OptionData:
        mOptionDataState = NEService::eDataStateType::DataIsOK;
        stream << mOptionData;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_OptionsStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_OptionsStub_errorRequest);
void OptionsStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;

    switch ( static_cast<NEOptions::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEOptions::eMessageIDs::MsgId_OptionData:
        mOptionDataState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEOptions::eMessageIDs::MsgId_responseDataRate:
    case NEOptions::eMessageIDs::MsgId_responsePacketRate:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEOptions::eMessageIDs::MsgId_requestDataRate:
    case NEOptions::eMessageIDs::MsgId_requestPacketRate:
    case NEOptions::eMessageIDs::MsgId_requestChangeOptions:
    case NEOptions::eMessageIDs::MsgId_requestQuitApplication:
        listenerId = static_cast<msg_id>(NEOptions::getResponseId(static_cast< NEOptions::eMessageIDs>(msgId)));
        result = msgCancel ? NEService::eResultType::RequestCanceled : NEService::eResultType::RequestError;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }

    StubBase::StubListenerList listeners;
    if ( findListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(generated_src_OptionsStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEOptions::getString( static_cast<NEOptions::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEOptions::getString(static_cast<NEOptions::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, OptionsResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void OptionsStub::setOptionData( const NEOptions::sOptionData & newValue )
{
    if ( (mOptionDataState != NEService::eDataStateType::DataIsOK) || (mOptionData != newValue) )
    {
        mOptionData = newValue;
        sendNotification( static_cast<msg_id>(NEOptions::eMessageIDs::MsgId_OptionData) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void OptionsStub::responseDataRate( const NEOptions::sDataRate & sentData, const NEOptions::sDataRate & missedData )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_responseDataRate;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << sentData;
    stream << missedData;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void OptionsStub::responsePacketRate( unsigned int genPackets, const NEOptions::sDataRate & avSize )
{
    static const NEOptions::eMessageIDs msgId = NEOptions::eMessageIDs::MsgId_responsePacketRate;
    EventDataStream args(EventDataStream::eEventData::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << genPackets;
    stream << avSize;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_OptionsStub_processRequestEvent);
void OptionsStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    OptionsRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, OptionsRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const OptionsRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEOptions::eMessageIDs respId = NEOptions::getResponseId(static_cast<NEOptions::eMessageIDs>(reqId));

        switch ( static_cast<NEOptions::eMessageIDs>(reqId) )
        {
        case NEOptions::eMessageIDs::MsgId_requestDataRate:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                requestDataRate(  );
            }
            break;

        case NEOptions::eMessageIDs::MsgId_requestPacketRate:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                requestPacketRate(  );
            }
            break;

        case NEOptions::eMessageIDs::MsgId_requestChangeOptions:
            if ( true )
            {
                NEOptions::sOptionData  Options;
                stream >> Options;
                requestChangeOptions( Options );
            }
            break;

        case NEOptions::eMessageIDs::MsgId_requestQuitApplication:
            if ( true )
            {
                requestQuitApplication(  );
            }
            break;

        default:
            {
                TRACE_SCOPE(generated_src_OptionsStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_OptionsStub_processAttributeEvent);
void OptionsStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEOptions::eMessageIDs updId  = static_cast<NEOptions::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_OptionsStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEOptions::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataInternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEOptions::eMessageIDs::MsgId_OptionData:
                if ( mOptionDataState == NEService::eDataStateType::DataIsOK )
                    stream << mOptionData;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_OptionsStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NEOptions::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NEOptions::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/OptionsStub.cpp file
//////////////////////////////////////////////////////////////////////////
