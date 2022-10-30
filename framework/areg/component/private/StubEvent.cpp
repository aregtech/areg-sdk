/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
    : StreamableEvent   (eventType)
    , mTargetStubAddress(toTarget)
{
}

StubEvent::StubEvent( const IEInStream & stream  )
    : StreamableEvent   (stream)
    , mTargetStubAddress(stream)
{
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

void StubEvent::deliverEvent( void )
{
    if ( mTargetThread == nullptr )
    {
        Thread * thread = Thread::findThreadByName( mTargetStubAddress.getThread() );
        registerForThread( thread != nullptr ? RUNTIME_CAST(thread, DispatcherThread) : nullptr );
    }

    if ( mTargetThread != nullptr )
    {
        StreamableEvent::deliverEvent();
    }
    else
    {
        this->destroy();
    }
}

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IEStubEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IEStubEventConsumer::IEStubEventConsumer( const StubAddress & stubAddress )
    : ThreadEventConsumerBase   ( )
    , mStubAddress              ( stubAddress )
{
}

inline void IEStubEventConsumer::localProcessRequestEvent( RequestEvent & requestEvent )
{
    Component *curComponent   = Component::findComponentByName(requestEvent.getTargetStub().getRoleName());
    ComponentThread::setCurrentComponent(curComponent);

    if (NEService::isRequestId(requestEvent.getRequestId()))
    {
        processRequestEvent(requestEvent);
    }
    else
    {
        processStubEvent(static_cast<StubEvent&>(requestEvent));
    }

    ComponentThread::setCurrentComponent(nullptr);
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

    ComponentThread::setCurrentComponent(nullptr);
}

inline void IEStubEventConsumer::localProcessConnectEvent( StubConnectEvent & notifyConnect )
{
    if ( notifyConnect.getRequestId() == static_cast<unsigned int>(NEService::eFuncIdRange::ServiceNotifyConnection) )
    {
        if (notifyConnect.getRequestType() == NEService::eRequestType::ServiceConnection)
        {
            processStubRegisteredEvent(notifyConnect.getTargetStub(), notifyConnect.getConnectionStatus());
        }
        else
        {
            processClientConnectEvent(notifyConnect.getEventSource(), notifyConnect.getConnectionStatus());
        }
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
    if ( stubEvent != nullptr )
    {
        if ( stubEvent->getTargetStub() == mStubAddress )
        {
            RequestEvent* reqEvent = RUNTIME_CAST(stubEvent, RequestEvent);
            if (reqEvent != nullptr)
            {
                localProcessRequestEvent(*reqEvent);
            }
            else
            {
                NotifyRequestEvent * notifyRequest = RUNTIME_CAST(stubEvent, NotifyRequestEvent);
                if ( notifyRequest != nullptr )
                {
                    localProcessNotifyRequestEvent(*notifyRequest);
                }
                else
                {
                    StubConnectEvent * stubConnectEvent = RUNTIME_CAST(stubEvent, StubConnectEvent);
                    if (stubConnectEvent != nullptr)
                    {
                        localProcessConnectEvent(*stubConnectEvent);
                    }
                    else
                    {
                        processStubEvent(*stubEvent);
                    }
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
