//////////////////////////////////////////////////////////////////////////
// Begin generate generated/src/private/RemoteRegistryStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *
 * Generated at     30.09.2021  01:22:13 GMT+02:00 
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 *
 * \file            generated/src/RemoteRegistryStub.hpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry
 *                  Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "generated/src/RemoteRegistryStub.hpp"
#include "generated/src/private/RemoteRegistryEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// RemoteRegistryStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
RemoteRegistryStub::RemoteRegistryStub( Component & masterComp )
    : StubBase    ( masterComp, NERemoteRegistry::getInterfaceData() )
    
    , mRegistryList       (  )
    , mRegistryListState  ( NEService::eDataStateType::DataIsUnavailable )
    
    , mRemainOutputs      (  )
    , mRemainOutputsState ( NEService::eDataStateType::DataIsUnavailable )
    
{
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void RemoteRegistryStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_startupServiceInterface);
void RemoteRegistryStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(generated_src_RemoteRegistryStub_startupServiceInterface);
    
    RemoteRegistryRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    RemoteRegistryNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_shutdownServiceIntrface);
void RemoteRegistryStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(generated_src_RemoteRegistryStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    RemoteRegistryRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    RemoteRegistryNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName().c_str() );
    StubBase::shutdownServiceIntrface( holder );
}

ResponseEvent * RemoteRegistryStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW RemoteRegistryResponseEvent(data, proxy, result, msgId) : DEBUG_NEW RemoteRegistryResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * RemoteRegistryStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW RemoteRegistryRequestEvent(stream) );
}

RemoteNotifyRequestEvent * RemoteRegistryStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW RemoteRegistryNotifyRequestEvent(stream) );
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_sendNotification);
void RemoteRegistryStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NERemoteRegistry::eMessageIDs>(msgId) )
    {
    case NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
        mRegistryListState = NEService::eDataStateType::DataIsOK;
        stream << mRegistryList;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    case NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
        mRemainOutputsState = NEService::eDataStateType::DataIsOK;
        stream << mRemainOutputs;
        sendUpdateEvent(msgId, args, NEService::eResultType::DataOK);
        break;

    default:
        {
            TRACE_SCOPE(generated_src_RemoteRegistryStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_errorRequest);
void RemoteRegistryStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::eResultType::NotProcessed;
    msg_id listenerId = msgId;
    
    switch ( static_cast<NERemoteRegistry::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
        mRegistryListState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

    case NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
        mRemainOutputsState = NEService::eDataStateType::DataIsInvalid;
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NERemoteRegistry::eMessageIDs::MsgId_responseRegister:
    case NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::eResultType::DataInvalid;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NERemoteRegistry::eMessageIDs::MsgId_requestRegister:
    case NERemoteRegistry::eMessageIDs::MsgId_requestUnregister:
    case NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld:
        listenerId = static_cast<msg_id>(NERemoteRegistry::getResponseId(static_cast< NERemoteRegistry::eMessageIDs>(msgId)));
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
        TRACE_SCOPE(generated_src_RemoteRegistryStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NERemoteRegistry::getString( static_cast<NERemoteRegistry::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NERemoteRegistry::getString(static_cast<NERemoteRegistry::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, RemoteRegistryResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void RemoteRegistryStub::setRegistryList( const NERemoteRegistry::ListRegistry & newValue )
{
    if ( (mRegistryListState != NEService::eDataStateType::DataIsOK) || (mRegistryList != newValue) )
    {
        mRegistryList = newValue;
        sendNotification( static_cast<msg_id>(NERemoteRegistry::eMessageIDs::MsgId_RegistryList) );
    }
}

void RemoteRegistryStub::setRemainOutputs( const unsigned int & newValue )
{
    if ( (mRemainOutputsState != NEService::eDataStateType::DataIsOK) || (mRemainOutputs != newValue) )
    {
        mRemainOutputs = newValue;
        sendNotification( static_cast<msg_id>(NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs) );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void RemoteRegistryStub::responseRegister( const NERemoteRegistry::sClientRegister & client )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::eMessageIDs::MsgId_responseRegister;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

void RemoteRegistryStub::responseHelloWorld( unsigned int clientID )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::eMessageIDs::MsgId_responseHelloWorld;
    EventDataStream args(EventDataStream::eEventData::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    sendResponseEvent( static_cast<msg_id>(msgId), args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_processRequestEvent);
void RemoteRegistryStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    RemoteRegistryRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, RemoteRegistryRequestEvent);

    if ( (reqEvent != nullptr) && (reqEvent->getRequestType() == NEService::eRequestType::CallFunction) )
    {
        msg_id reqId = static_cast<msg_id>(reqEvent->getRequestId());
        const IEInStream & stream  = static_cast<const RemoteRegistryRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NERemoteRegistry::eMessageIDs respId = NERemoteRegistry::getResponseId(static_cast<NERemoteRegistry::eMessageIDs>(reqId));

        switch ( static_cast<NERemoteRegistry::eMessageIDs>(reqId) )
        {
        case NERemoteRegistry::eMessageIDs::MsgId_requestRegister:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                String          name;
                ServiceAddress  service;
                String          thread;
                String          process;
                stream >> name;                
                stream >> service;                
                stream >> thread;                
                stream >> process;                
                requestRegister( name, service, thread, process );
            }
            break;
            
        case NERemoteRegistry::eMessageIDs::MsgId_requestUnregister:
            if ( true )
            {
                NERemoteRegistry::sClientRegister   client;
                stream >> client;                
                requestUnregister( client );
            }
            break;
            
        case NERemoteRegistry::eMessageIDs::MsgId_requestHelloWorld:
            if ( canExecuteRequest(listener, static_cast<msg_id>(respId), reqEvent->getSequenceNumber()) )
            {
                unsigned int    clientID;
                String          addMessage;
                stream >> clientID;                
                stream >> addMessage;                
                requestHelloWorld( clientID, addMessage );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(generated_src_RemoteRegistryStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(generated_src_RemoteRegistryStub_processAttributeEvent);
void RemoteRegistryStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::eRequestType::RemoveAllNotify)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NERemoteRegistry::eMessageIDs updId  = static_cast<NERemoteRegistry::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::eRequestType::StopNotify)
        {
            removeNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::eRequestType::StartNotify)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(generated_src_RemoteRegistryStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NERemoteRegistry::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<msg_id>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::eEventData::EventDataExternal);
            NEService::eResultType validUpdate = NEService::eResultType::DataOK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NERemoteRegistry::eMessageIDs::MsgId_RegistryList:
                if ( mRegistryListState == NEService::eDataStateType::DataIsOK )
                    stream << mRegistryList;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            case NERemoteRegistry::eMessageIDs::MsgId_RemainOutputs:
                if ( mRemainOutputsState == NEService::eDataStateType::DataIsOK )
                    stream << mRemainOutputs;
                else
                    validUpdate = NEService::eResultType::DataInvalid;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<msg_id>(updId)) == false )
                {
                    TRACE_SCOPE(generated_src_RemoteRegistryStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::eResultType::DataInvalid;
                updId       = NERemoteRegistry::eMessageIDs::MsgId_NotProcessed;
                break;
            }

            if (updId != NERemoteRegistry::eMessageIDs::MsgId_NotProcessed)
                sendUpdateEvent( static_cast<msg_id>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate generated/src/private/RemoteRegistryStub.cpp file
//////////////////////////////////////////////////////////////////////////
