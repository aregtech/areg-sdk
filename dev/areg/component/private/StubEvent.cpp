/************************************************************************
 * \file        areg/component/private/StubEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component Thread.
 *
 ************************************************************************/
#include "areg/component/StubEvent.hpp"

#include "areg/component/StubBase.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/private/StubConnectEvent.hpp"


//////////////////////////////////////////////////////////////////////////
// StubEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEvent class, implement runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(StubEvent, StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// StubEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
StubEvent::StubEvent( const StubAddress& toTarget, Event::eEventType eventType )
    : StreamableEvent (eventType)
    , mTargetStubAddress(toTarget)
{
    ; // do nothing
}

StubEvent::StubEvent( const IEInStream & stream  )
    : StreamableEvent (stream)
    , mTargetStubAddress(stream)
{
    ; // do nothing
}

StubEvent::~StubEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// StubEvent class, methods
//////////////////////////////////////////////////////////////////////////
const IEInStream & StubEvent::readStream( const IEInStream & stream )
{
    StreamableEvent::readStream(stream);
    stream >> mTargetStubAddress;
    return stream;
}

IEOutStream & StubEvent::writeStream( IEOutStream & stream ) const
{
    StreamableEvent::writeStream(stream);
    stream << mTargetStubAddress;
    return stream;
}

void StubEvent::sendEvent( void )
{
    if ( mTargetThread == NULL )
    {
        Thread * thread = Thread::findThreadByName( mTargetStubAddress.getThread() );
        registerForThread( thread != NULL ? RUNTIME_CAST(thread, DispatcherThread) : NULL );
    }

    if ( mTargetThread != NULL )
        StreamableEvent::sendEvent();
    else
        this->destroy();
}

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IEStubEventConsumer::IEStubEventConsumer( const StubAddress & stubAddress )
    : ThreadEventConsumerBase ( )
    , mStubAddress              ( stubAddress )
{
    ; // do nothing
}

IEStubEventConsumer::~IEStubEventConsumer( void )
{
    ; // do nothing
}

inline void IEStubEventConsumer::localProcessRequestEvent( RequestEvent & requestEvent )
{
    Component *curComponent   = Component::findComponentByName(requestEvent.getTargetStub().getRoleName());
    ComponentThread::setCurrentComponent(curComponent);

    if (NEService::isRequestId(requestEvent.getRequestId()))
        processRequestEvent(requestEvent);
    else
        processStubEvent( static_cast<StubEvent &>(requestEvent));

    ComponentThread::setCurrentComponent(NULL);
}

inline void IEStubEventConsumer::localProcessNotifyRequestEvent( NotifyRequestEvent & notifyRequest )
{
    Component *curComponent   = Component::findComponentByName(notifyRequest.getTargetStub().getRoleName());
    ComponentThread::setCurrentComponent(curComponent);

    unsigned int reqId = notifyRequest.getRequestId();
    if (NEService::isAttributeId(reqId) || NEService::isResponseId(reqId))
    {
        processAttributeEvent(notifyRequest);
    }
    else
    {
        processStubEvent( static_cast<StubEvent &>(notifyRequest) );
    }

    ComponentThread::setCurrentComponent(NULL);
}

inline void IEStubEventConsumer::localProcessConnectEvent( StubConnectEvent & notifyConnect )
{
    if ( notifyConnect.getRequestId() == static_cast<unsigned int>(NEService::SI_SERVICE_CONNECTION_NOTIFY) )
    {
        if ( notifyConnect.getRequestType() == NEService::REQUEST_CONNECTION)
            processStubRegisteredEvent( notifyConnect.getTargetStub(), notifyConnect.getConnectionStatus() );
        else
            processClientConnectEvent( notifyConnect.getEventSource(), notifyConnect.getConnectionStatus() );
    }
    else
    {
        processStubEvent( static_cast<StubEvent &>(notifyConnect));
    }
}

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void IEStubEventConsumer::startEventProcessing( Event & eventElem )
{
    StubEvent* stubEvent = RUNTIME_CAST(&eventElem, StubEvent);
    if ( stubEvent != NULL )
    {
        if ( stubEvent->getTargetStub() == mStubAddress )
        {
            RequestEvent* reqEvent = RUNTIME_CAST(stubEvent, RequestEvent);
            if (reqEvent != NULL)
            {
                localProcessRequestEvent(*reqEvent);
            }
            else
            {
                NotifyRequestEvent * notifyRequest = RUNTIME_CAST(stubEvent, NotifyRequestEvent);
                if ( notifyRequest != NULL )
                {
                    localProcessNotifyRequestEvent(*notifyRequest);
                }
                else
                {
                    StubConnectEvent * stubConnectEvent = RUNTIME_CAST(stubEvent, StubConnectEvent);
                    if ( stubConnectEvent != NULL )
                        localProcessConnectEvent(*stubConnectEvent);
                    else
                        processStubEvent(*stubEvent);
                }
            }
        }
        else
        {
            ; // ignore, wrong stub address
        }
    }
    else
    {
        processGenericEvent(eventElem);
    }
}

void IEStubEventConsumer::consumerRegistered( bool isRegistered )
{
    ThreadEventConsumerBase::consumerRegistered(isRegistered);
}
