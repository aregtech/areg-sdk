//////////////////////////////////////////////////////////////////////////
// Begin generate shareipcmix/src/private/RemoteRegistryStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source RemoteRegistry.
 * Generated at     11.06.2021  21:11:05 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shareipcmix/src/private/RemoteRegistryStub.cpp
 * \ingroup         RemoteRegistry Service Interface
 * \brief           This is an automatic generated code of RemoteRegistry Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shareipcmix/src/RemoteRegistryStub.hpp"
#include "shareipcmix/src/private/RemoteRegistryEvents.hpp"

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
    , mRegistryListState  ( NEService::DATA_UNAVAILABLE )
    
    , mRemainOutputs      (  )
    , mRemainOutputsState ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

RemoteRegistryStub::~RemoteRegistryStub( void )
{
    ; // do nothing
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

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_startupServiceInterface);
void RemoteRegistryStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_startupServiceInterface);
    
    RemoteRegistryRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    RemoteRegistryNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_shutdownServiceIntrface);
void RemoteRegistryStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    RemoteRegistryRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    RemoteRegistryNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
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

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_sendNotification);
void RemoteRegistryStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NERemoteRegistry::eMessageIDs>(msgId) )
    {
    case NERemoteRegistry::MSG_ID_RegistryList:
        mRegistryListState = NEService::DATA_OK;
        stream << mRegistryList;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    case NERemoteRegistry::MSG_ID_RemainOutputs:
        mRemainOutputsState = NEService::DATA_OK;
        stream << mRemainOutputs;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_errorRequest);
void RemoteRegistryStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NERemoteRegistry::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NERemoteRegistry::MSG_ID_RegistryList:
        mRegistryListState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

    case NERemoteRegistry::MSG_ID_RemainOutputs:
        mRemainOutputsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NERemoteRegistry::MSG_ID_responseRegister:
    case NERemoteRegistry::MSG_ID_responseHelloWorld:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NERemoteRegistry::MSG_ID_requestRegister:
    case NERemoteRegistry::MSG_ID_requestUnregister:
    case NERemoteRegistry::MSG_ID_requestHelloWorld:
        listenerId = NERemoteRegistry::getResponseId(static_cast< NERemoteRegistry::eMessageIDs>(msgId));
        result = msgCancel ? NEService::RESULT_REQUEST_CANCELED : NEService::RESULT_REQUEST_ERROR;
        break;

    default:
        OUTPUT_ERR("Unexpected message ID [ %d ] received!", msgId);
        ASSERT(false);
        break;
    }
    
    StubBase::StubListenerList listeners;
    if ( findListeners(listenerId, listeners) > 0 )
    {
        TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_errorRequest);
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
    if ( (mRegistryListState != NEService::DATA_OK) || (mRegistryList != newValue) )
    {
        mRegistryList = newValue;
        sendNotification( NERemoteRegistry::MSG_ID_RegistryList );
    }
}

void RemoteRegistryStub::setRemainOutputs( const unsigned int & newValue )
{
    if ( (mRemainOutputsState != NEService::DATA_OK) || (mRemainOutputs != newValue) )
    {
        mRemainOutputs = newValue;
        sendNotification( NERemoteRegistry::MSG_ID_RemainOutputs );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void RemoteRegistryStub::responseRegister( const NERemoteRegistry::sClientRegister & client )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::MSG_ID_responseRegister;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << client;
    sendResponseEvent( msgId, args );
}

void RemoteRegistryStub::responseHelloWorld( unsigned int clientID )
{
    static const NERemoteRegistry::eMessageIDs msgId = NERemoteRegistry::MSG_ID_responseHelloWorld;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << clientID;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_processRequestEvent);
void RemoteRegistryStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    RemoteRegistryRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, RemoteRegistryRequestEvent);

    if ( (reqEvent != static_cast<RemoteRegistryRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const RemoteRegistryRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NERemoteRegistry::eMessageIDs respId = NERemoteRegistry::getResponseId(static_cast<NERemoteRegistry::eMessageIDs>(reqId));

        switch ( static_cast<NERemoteRegistry::eMessageIDs>(reqId) )
        {
        case NERemoteRegistry::MSG_ID_requestRegister:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
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
            
        case NERemoteRegistry::MSG_ID_requestUnregister:
            if ( true )
            {
                NERemoteRegistry::sClientRegister   client;
                stream >> client;                
                requestUnregister( client );
            }
            break;
            
        case NERemoteRegistry::MSG_ID_requestHelloWorld:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
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
                TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_processAttributeEvent);
void RemoteRegistryStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NERemoteRegistry::eMessageIDs updId  = static_cast<NERemoteRegistry::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NERemoteRegistry::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataExternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NERemoteRegistry::MSG_ID_RegistryList:
                if ( mRegistryListState == NEService::DATA_OK )
                    stream << mRegistryList;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            case NERemoteRegistry::MSG_ID_RemainOutputs:
                if ( mRemainOutputsState == NEService::DATA_OK )
                    stream << mRemainOutputs;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shareipcmix_src_RemoteRegistryStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NERemoteRegistry::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NERemoteRegistry::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shareipcmix/src/private/RemoteRegistryStub.cpp file
//////////////////////////////////////////////////////////////////////////
