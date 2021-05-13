//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/MessageOutputStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source MessageOutput.
 * Generated at     12.05.2021  16:41:12 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/MessageOutputStub.cpp
 * \ingroup         MessageOutput Service Interface
 * \brief           This is an automatic generated code of MessageOutput Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/MessageOutputStub.hpp"
#include "src/gen/private/MessageOutputEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// MessageOutputStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
MessageOutputStub::MessageOutputStub( Component & masterComp )
    : StubBase    ( masterComp, NEMessageOutput::createInterfaceData() )
    
    , mLastMessage        (  )
    , mLastMessageState   ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

MessageOutputStub::~MessageOutputStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void MessageOutputStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_startupServiceInterface);
void MessageOutputStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(src_gen_MessageOutputStub_startupServiceInterface);
    
    MessageOutputRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    MessageOutputNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_shutdownServiceIntrface);
void MessageOutputStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(src_gen_MessageOutputStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    MessageOutputRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    MessageOutputNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int MessageOutputStub::getNumberOfRequests( void ) const
{
    return NEMessageOutput::NumberofRequests;
}

unsigned int MessageOutputStub::getNumberOfResponses( void ) const
{
    return NEMessageOutput::NumberofResponses;
}

unsigned int MessageOutputStub::getNumberOfAttributes( void ) const
{
    return NEMessageOutput::NumberofAttributes;
}

const unsigned int * MessageOutputStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::RequestIds);
}

const unsigned int * MessageOutputStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::ResponseIds);
}

const unsigned int * MessageOutputStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NEMessageOutput::AttributeIds);
}

ResponseEvent * MessageOutputStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW MessageOutputResponseEvent(data, proxy, result, msgId) : DEBUG_NEW MessageOutputResponseEvent(proxy, result, msgId));
}

const Version & MessageOutputStub::getImplVersion( void ) const
{
    return NEMessageOutput::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_sendNotification);
void MessageOutputStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
    {
    case NEMessageOutput::MSG_ID_LastMessage:
        mLastMessageState = NEService::DATA_OK;
        stream << mLastMessage;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_MessageOutputStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_errorRequest);
void MessageOutputStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_LastMessage:
        mLastMessageState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_broadcastActionRequest:
    case NEMessageOutput::MSG_ID_broadcastInputMessage:
        result = NEService::RESULT_INVALID;
        break;

/************************************************************************
 * Request errors
 ************************************************************************/
    case NEMessageOutput::MSG_ID_requestOutputMessage:
        listenerId = NEMessageOutput::getResponseId(static_cast< NEMessageOutput::eMessageIDs>(msgId));
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
        TRACE_SCOPE(src_gen_MessageOutputStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NEMessageOutput::getString( static_cast<NEMessageOutput::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NEMessageOutput::getString(static_cast<NEMessageOutput::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, MessageOutputResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void MessageOutputStub::setLastMessage( const String & newValue )
{
    if ( (mLastMessageState != NEService::DATA_OK) || (mLastMessage != newValue) )
    {
        mLastMessage = newValue;
        sendNotification( NEMessageOutput::MSG_ID_LastMessage );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

void MessageOutputStub::broadcastActionRequest( NEMessageOutput::eRequestedActons userAction )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_broadcastActionRequest;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << userAction;
    sendResponseEvent( msgId, args );
}

void MessageOutputStub::broadcastInputMessage( const String & msgInput )
{
    static const NEMessageOutput::eMessageIDs msgId = NEMessageOutput::MSG_ID_broadcastInputMessage;
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();
    stream << msgInput;
    sendResponseEvent( msgId, args );
}

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_processRequestEvent);
void MessageOutputStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    MessageOutputRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, MessageOutputRequestEvent);

    if ( (reqEvent != static_cast<MessageOutputRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        const IEInStream & stream  = static_cast<const MessageOutputRequestEvent *>(reqEvent)->getData().getReadStream();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        switch ( static_cast<NEMessageOutput::eMessageIDs>(reqId) )
        {
        case NEMessageOutput::MSG_ID_requestOutputMessage:
            if ( true )
            {
                String      msgOutput;
                DateTime    dateTime;
                stream >> msgOutput;                
                stream >> dateTime;                
                requestOutputMessage( msgOutput, dateTime );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_MessageOutputStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_MessageOutputStub_processAttributeEvent);
void MessageOutputStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NEMessageOutput::eMessageIDs updId  = static_cast<NEMessageOutput::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_MessageOutputStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NEMessageOutput::getString(updId)
                            , ProxyAddress::convAddressToPath(eventElem.getEventSource()).getString());
            }
#else   // _DEBUG
            addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
#endif  // _DEBUG
            EventDataStream args(EventDataStream::EventDataInternal);
            NEService::eResultType validUpdate = NEService::RESULT_DATA_OK;
            IEOutStream & stream               = args.getStreamForWrite();

            switch (updId)
            {
            case NEMessageOutput::MSG_ID_LastMessage:
                if ( mLastMessageState == NEService::DATA_OK )
                    stream << mLastMessage;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_MessageOutputStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NEMessageOutput::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NEMessageOutput::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/MessageOutputStub.cpp file
//////////////////////////////////////////////////////////////////////////
