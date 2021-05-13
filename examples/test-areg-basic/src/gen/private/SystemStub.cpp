//////////////////////////////////////////////////////////////////////////
// Begin generate src/gen/private/SystemStub.cpp file
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * (c) copyright    2021
 *                  Create by AREG SDK code generator tool from source System.
 * Generated at     12.05.2021  16:41:13 GMT+02:00 
 ************************************************************************/

/************************************************************************
 * \file            src/gen/private/SystemStub.cpp
 * \ingroup         System Service Interface
 * \brief           This is an automatic generated code of System Service Interface Stub class implementation.
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "src/gen/SystemStub.hpp"
#include "src/gen/private/SystemEvents.hpp"

#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/base/Thread.hpp"
#include "areg/trace/GETrace.h"

//////////////////////////////////////////////////////////////////////////
// SystemStub class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
SystemStub::SystemStub( Component & masterComp )
    : StubBase    ( masterComp, NESystem::createInterfaceData() )
    
    , mSystemState        (  )
    , mSystemStateState   ( NEService::DATA_UNAVAILABLE )
    
{
    ; // do nothing
}

SystemStub::~SystemStub( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////

void SystemStub::unlockAllRequests( void )
{
    StubBase::cancelAllRequests( );
}

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(src_gen_SystemStub_startupServiceInterface);
void SystemStub::startupServiceInterface( Component & holder )
{
    TRACE_SCOPE(src_gen_SystemStub_startupServiceInterface);
    
    SystemRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SystemNotifyRequestEvent::addListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::startupServiceInterface( holder );

    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] has been started and is available ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
}

DEF_TRACE_SCOPE(src_gen_SystemStub_shutdownServiceIntrface);
void SystemStub::shutdownServiceIntrface( Component & holder )
{
    TRACE_SCOPE(src_gen_SystemStub_shutdownServiceIntrface);
    TRACE_DBG("The Stub Service [ %s ] of component with role name [ %s ] is shutting down and not available anymore ...", mAddress.getServiceName().getString(), mAddress.getRoleName().getString());
    
    SystemRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    SystemNotifyRequestEvent::removeListener( static_cast<IEEventConsumer &>(self()), Thread::getCurrentThreadName() );
    StubBase::shutdownServiceIntrface( holder );
}

unsigned int SystemStub::getNumberOfRequests( void ) const
{
    return NESystem::NumberofRequests;
}

unsigned int SystemStub::getNumberOfResponses( void ) const
{
    return NESystem::NumberofResponses;
}

unsigned int SystemStub::getNumberOfAttributes( void ) const
{
    return NESystem::NumberofAttributes;
}

const unsigned int * SystemStub::getRequestIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::RequestIds);
}

const unsigned int * SystemStub::getResponseIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::ResponseIds);
}

const unsigned int * SystemStub::getAttributeIds( void ) const
{
    return reinterpret_cast<const unsigned int *>(NESystem::AttributeIds);
}

ResponseEvent * SystemStub::createResponseEvent( const ProxyAddress & proxy, unsigned int msgId, NEService::eResultType result, const EventDataStream & data ) const
{
    return (data.isEmpty() == false ? DEBUG_NEW SystemResponseEvent(data, proxy, result, msgId) : DEBUG_NEW SystemResponseEvent(proxy, result, msgId));
}

const Version & SystemStub::getImplVersion( void ) const
{
    return NESystem::InterfaceVersion;
}

DEF_TRACE_SCOPE(src_gen_SystemStub_sendNotification);
void SystemStub::sendNotification( unsigned int msgId )
{
    EventDataStream args(EventDataStream::EventDataInternal);
    IEOutStream & stream = args.getStreamForWrite();

    switch ( static_cast<NESystem::eMessageIDs>(msgId) )
    {
    case NESystem::MSG_ID_SystemState:
        mSystemStateState = NEService::DATA_OK;
        stream << mSystemState;
        sendUpdateEvent(msgId, args, NEService::RESULT_DATA_OK);
        break;

    default:
        {
            TRACE_SCOPE(src_gen_SystemStub_sendNotification);
            TRACE_ERR("Unexpected attribute ID [ %d ] requested to send by Stub [ %s ].", msgId, StubAddress::convAddressToPath(mAddress).getString());
            ASSERT(false);
        }
        break; // do nothing
    }
}

DEF_TRACE_SCOPE(src_gen_SystemStub_errorRequest);
void SystemStub::errorRequest( unsigned int msgId, bool msgCancel )
{
    NEService::eResultType result = NEService::RESULT_NOT_PROCESSED;
    unsigned int listenerId = static_cast<unsigned int>(msgId);
    
    switch ( static_cast<NESystem::eMessageIDs>(msgId) )
    {
/************************************************************************
 * Attribute errors
 ************************************************************************/
    case NESystem::MSG_ID_SystemState:
        mSystemStateState = NEService::DATA_INVALID;
        result = NEService::RESULT_DATA_INVALID;
        break;

/************************************************************************
 * Response errors
 ************************************************************************/
/************************************************************************
 * Broadcast errors
 ************************************************************************/
/************************************************************************
 * Request errors
 ************************************************************************/
    case NESystem::MSG_ID_requestPowerOff:
    case NESystem::MSG_ID_requestPowerOn:
    case NESystem::MSG_ID_requestShutdown:
    case NESystem::MSG_ID_requestStart:
        listenerId = NESystem::getResponseId(static_cast< NESystem::eMessageIDs>(msgId));
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
        TRACE_SCOPE(src_gen_SystemStub_errorRequest);
        TRACE_WARN(" >>> The message [ %s ] of Stub [ %s ] is responding with error [ %s ], sending message [ %s ] to [ %d ] targets... <<<"
                        , NESystem::getString( static_cast<NESystem::eMessageIDs>(msgId) )
                        , StubAddress::convAddressToPath(mAddress).getString()
                        , NEService::getString( result )
                        , NESystem::getString(static_cast<NESystem::eMessageIDs>(msgId))
                        , listeners.getSize() );

        StubBase::sendErrorNotification(listeners, SystemResponseEvent(listeners[0].mProxy, result, msgId));
    }
}

/************************************************************************
 * Set attributes and send notifications
 ************************************************************************/

void SystemStub::setSystemState( const NESystem::eSystemState & newValue )
{
    if ( (mSystemStateState != NEService::DATA_OK) || (mSystemState != newValue) )
    {
        mSystemState = newValue;
        sendNotification( NESystem::MSG_ID_SystemState );
    }
}

/************************************************************************
 * Send responses
 ************************************************************************/

/************************************************************************
 * Send Broadcast
 ************************************************************************/

/************************************************************************
 * Process messages
 ************************************************************************/

DEF_TRACE_SCOPE(src_gen_SystemStub_processRequestEvent);
void SystemStub::processRequestEvent( ServiceRequestEvent & eventElem )
{
    ASSERT( NEService::isRequestId(eventElem.getRequestId()) );
    SystemRequestEvent * reqEvent = RUNTIME_CAST(&eventElem, SystemRequestEvent);

    if ( (reqEvent != static_cast<SystemRequestEvent *>(NULL)) && (reqEvent->getRequestType() == NEService::REQUEST_CALL) )
    {
        unsigned int reqId          = reqEvent->getRequestId();
        StubBase::Listener listener( reqId, 0, reqEvent->getEventSource() );

        switch ( static_cast<NESystem::eMessageIDs>(reqId) )
        {
        case NESystem::MSG_ID_requestPowerOff:
            if ( true )
            {
                requestPowerOff(  );
            }
            break;
            
        case NESystem::MSG_ID_requestPowerOn:
            if ( true )
            {
                requestPowerOn(  );
            }
            break;
            
        case NESystem::MSG_ID_requestShutdown:
            if ( true )
            {
                requestShutdown(  );
            }
            break;
            
        case NESystem::MSG_ID_requestStart:
            if ( true )
            {
                requestStart(  );
            }
            break;
            
        default:
            {
                TRACE_SCOPE(src_gen_SystemStub_processRequestEvent);
                TRACE_ERR("Unexpected request ID [ %d ] to process in Stub [ %s ]!", reqId, StubAddress::convAddressToPath(mAddress).getString());
                ASSERT(false);
            }
            break;
        }
    }

    cancelCurrentRequest();
}

DEF_TRACE_SCOPE(src_gen_SystemStub_processAttributeEvent);
void SystemStub::processAttributeEvent( ServiceRequestEvent & eventElem )
{
    const NEService::eRequestType reqType = eventElem.getRequestType();
    if (reqType == NEService::REQUEST_REMOVE_ALL_NOTIFY)
    {
        IntegerArray removedIds;
        StubBase::clearAllListeners(eventElem.getEventSource(), removedIds);
    }
    else
    {
        NESystem::eMessageIDs updId  = static_cast<NESystem::eMessageIDs>(eventElem.getRequestId());
        if (reqType == NEService::REQUEST_STOP_NOTIFY)
        {
            removeNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() );
        }
        else if (reqType == NEService::REQUEST_START_NOTIFY)
        {
#ifdef  _DEBUG
            if (addNotificationListener( static_cast<unsigned int>(updId), eventElem.getEventSource() ) == false )
            {
                TRACE_SCOPE(src_gen_SystemStub_processAttributeEvent);
                TRACE_WARN("The notification request of message ID [ %s ] of sources [ %s ] is already registered. Ignoring start notification registration request."
                            , NESystem::getString(updId)
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
            case NESystem::MSG_ID_SystemState:
                if ( mSystemStateState == NEService::DATA_OK )
                    stream << mSystemState;
                else
                    validUpdate = NEService::RESULT_DATA_INVALID;
                break;

            default:
#ifdef  _DEBUG
                if ( NEService::isResponseId(static_cast<unsigned int>(updId)) == false )
                {
                    TRACE_SCOPE(src_gen_SystemStub_processAttributeEvent);
                    TRACE_ERR("Unexpected notification request of attribute ID [ %d ] received in Stub [ %s ]!", updId, StubAddress::convAddressToPath(getAddress()).getString());
                    ASSERT(false);
                }
#endif // _DEBUG
                validUpdate = NEService::RESULT_DATA_INVALID;
                updId       = NESystem::MSG_ID_NO_PROCEED;
                break;
            }

            if (updId != NESystem::MSG_ID_NO_PROCEED)
                sendUpdateEvent( static_cast<unsigned int>(updId), args, validUpdate );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
// End generate src/gen/private/SystemStub.cpp file
//////////////////////////////////////////////////////////////////////////
