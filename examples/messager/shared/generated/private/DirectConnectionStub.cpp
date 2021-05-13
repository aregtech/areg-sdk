//////////////////////////////////////////////////////////////////////////
// Begin generate shared/generated/private/DirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source DirectConnection.
 * Generated at     12.05.2021  16:41:23 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            shared/generated/private/DirectConnectionStub.cpp
 * \ingroup         DirectConnection Service Interface
 * \brief           This is an automatic generated code of DirectConnection Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "shared/generated/DirectConnectionStub.hpp"
#include "shared/generated/private/DirectConnectionEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// DirectConnectionStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
DirectConnectionStub::DirectConnectionStub( Component & masterComp )
    : StubBase    ( masterComp, NEDirectConnection::createInterfaceData() )
    
    , mInitiatedConnections       (  )
    , mInitiatedConnectionsState  ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

DirectConnectionStub::~DirectConnectionStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void DirectConnectionStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_startupServiceInterface);
void DirectConnectionStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(shared_generated_DirectConnectionStub_startupServiceInterface);
    
    DirectConnectionRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    DirectConnectionNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_shutdownServiceIntrface);
void DirectConnectionStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(shared_generated_DirectConnectionStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    DirectConnectionRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    DirectConnectionNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int DirectConnectionStub::getNumberOfRequests( void ) const
{
    return NEDirectConnection::NumberofRequests;
}

unsigned int DirectConnectionStub::getNumberOfResponses( void ) const
{
    return NEDirectConnection::NumberofResponses;
}

unsigned int DirectConnectionStub::getNumberOfAttributes( void ) const
{
    return NEDirectConnection::NumberofAttributes;
}

const unsigned int * DirectConnectionStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::RequestIds);
}

const unsigned int * DirectConnectionStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::ResponseIds);
}

const unsigned int * DirectConnectionStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEDirectConnection::AttributeIds);
}

ResponseEvent * DirectConnectionStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW DirectConnectionResponseEvent(data, proxy, result, msgId) : DEBUG_NEW DirectConnectionResponseEvent(proxy, result, msgId));
}

RemoteRequestEvent * DirectConnectionStub::createRemoteRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteRequestEvent *>( DEBUG_NEW DirectConnectionRequestEvent(stream) );
}

RemoteNotifyRequestEvent * DirectConnectionStub::createRemoteNotifyRequestEvent( const IEInStream & stream ) const
{
    return static_cast<RemoteNotifyRequestEvent *>( DEBUG_NEW DirectConnectionNotifyRequestEvent(stream) );
}

const Version & DirectConnectionStub::getImplVersion( void ) const
{
    return NEDirectConnection::InterfaceVersion;
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_sendNotification);
void DirectConnectionStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        mInitiatedConnectionsState = NEService::DATA_OK;
        stream << mInitiatedConnections;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(shared_generated_DirectConnectionStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_errorRequest);
void DirectConnectionStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_InitiatedConnections:
        mInitiatedConnectionsState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_responseConnectoinSetup:
    case NEDirectConnection::MSG_ID_responseAddParticipant:
    case NEDirectConnection::MSG_ID_responseRemoveParticipant:
/************************************************************************
 * Broadcast errors
 ************************************************************************/
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEDirectConnection::MSG_ID_requestConnectoinSetup:
    case NEDirectConnection::MSG_ID_requestAddParticipant:
    case NEDirectConnection::MSG_ID_requestRemoveParticipant:
    case NEDirectConnection::MSG_ID_requestCloseConnection:
        listenerId = NEDirectConnection::getResponseId(static_cast< NEDirectConnection::eMessageIDs>(msgId));
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
        TRACE_SCOPE(shared_generated_DirectConnectionStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEDirectConnection::getString( static_cast<NEDirectConnection::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEDirectConnection::getString(static_cast<NEDirectConnection::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, DirectConnectionResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void DirectConnectionStub::setInitiatedConnections( const NEDirectConnection::MapParticipants & newValue )
{
    if ( (mInitiatedConnectionsState != NEService::DATA_OK) || (mInitiatedConnections != newValue) )
    {
        mInitiatedConnections = newValue;
        sendNotification( NEDirectConnection::MSG_ID_InitiatedConnections );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

void DirectConnectionStub::responseConnectoinSetup( bool succeeded, const NEDirectConnection::sParticipant & target, const NEDirectConnection::sInitiator & initiator, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_responseConnectoinSetup;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << target;
    stream << initiator;
    stream << listParticipants;
    sendResponseEvent( msgId, args );
}

void DirectConnectionStub::responseAddParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_responseAddParticipant;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    sendResponseEvent( msgId, args );
}

void DirectConnectionStub::responseRemoveParticipant( bool succeeded, const NEDirectConnection::ListParticipants & listParticipants )
{
    static const NEDirectConnection::eMessageIDs msgId = NEDirectConnection::MSG_ID_responseRemoveParticipant;
    EventDataStream args(EventDataStream::EventDataExternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << succeeded;
    stream << listParticipants;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_processRequestEvent);
void DirectConnectionStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    DirectConnectionRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, DirectConnectionRequestEvent);

    if ( (reqEvent != static_cast<DirectConnectionRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const DirectConnectionRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );
        NEDirectConnection::eMessageIDs respId = NEDirectConnection::getResponseId(static_cast<NEDirectConnection::eMessageIDs>(reqId));

        switch ( static_cast<NEDirectConnection::eMessageIDs>(reqId) )
        {
        case NEDirectConnection::MSG_ID_requestConnectoinSetup:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                requestConnectoinSetup( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_requestAddParticipant:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                requestAddParticipant( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_requestRemoveParticipant:
            if ( canExecuteRequest(listener, respId, reqEvent->getSequenceNumber()) )
            {
                NEDirectConnection::sInitiator          initiator;
                NEDirectConnection::ListParticipants    listParticipants;
                stream >> initiator;                
                stream >> listParticipants;                
                requestRemoveParticipant( initiator, listParticipants );
            }
            break;
            
        case NEDirectConnection::MSG_ID_requestCloseConnection:
            if ( true )
            {
                NEDirectConnection::sInitiator  initiator;
                stream >> initiator;                
                requestCloseConnection( initiator );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(shared_generated_DirectConnectionStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(shared_generated_DirectConnectionStub_processAttributeEvent);
void DirectConnectionStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEDirectConnection::eMessageIDs updId  = static_cast<NEDirectConnection::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(shared_generated_DirectConnectionStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEDirectConnection::getString(updId)
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
            case NEDirectConnection::MSG_ID_InitiatedConnections:
                if ( mInitiatedConnectionsState == NEService::DATA_OK )
                    stream << mInitiatedConnections;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(shared_generated_DirectConnectionStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEDirectConnection::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEDirectConnection::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate shared/generated/private/DirectConnectionStub.cpp file
//////////////////////////////////////////////////////////////////////////
