/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/StubEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Thread.
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
AREG_IMPLEMENT_RUNTIME_EVENT(StubEvent, StreamableEvent)

//////////////////////////////////////////////////////////////////////////
// StubEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
StubEvent::StubEvent( const StubAddress& toTarget, Event::EventType eventType )
    : StreamableEvent   (eventType)
    , mTargetStubAddress(toTarget)
{
}

StubEvent::StubEvent( const InStream & stream  )
    : StreamableEvent   (stream)
    , mTargetStubAddress(stream)
{
}

//////////////////////////////////////////////////////////////////////////
// StubEvent class, methods
//////////////////////////////////////////////////////////////////////////
const InStream & StubEvent::readStream( const InStream & stream )
{
    StreamableEvent::readStream(stream);
    stream >> mTargetStubAddress;
    return stream;
}

OutStream & StubEvent::writeStream( OutStream & stream ) const
{
    StreamableEvent::writeStream(stream);
    stream << mTargetStubAddress;
    return stream;
}

void StubEvent::deliverEvent()
{
    if ( mTargetThread == nullptr )
    {
        Thread * thread = Thread::findThreadByName( mTargetStubAddress.getThread() );
        registerForThread( thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr );
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
// StubEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
StubEventConsumer::StubEventConsumer( const StubAddress & stubAddress )
    : EventConsumer   ( )
    , mStubAddress      ( stubAddress )
    , mCurEvent         ( nullptr )
{
}

inline void StubEventConsumer::_localProcessRequestEvent( RequestEvent & requestEvent )
{
    Component *curComponent   = Component::findComponentByName(requestEvent.getTargetStub().getRoleName());
    ComponentThread::setCurrentComponent(curComponent);

    if (areg::isRequestId(requestEvent.getRequestId()))
    {
        processRequestEvent(requestEvent);
    }
    else
    {
        processStubEvent(static_cast<StubEvent&>(requestEvent));
    }

    ComponentThread::setCurrentComponent(nullptr);
}

inline void StubEventConsumer::_localProcessNotifyRequestEvent( NotifyRequestEvent & notifyRequest )
{
    Component *curComponent   = Component::findComponentByName(notifyRequest.getTargetStub().getRoleName());
    ComponentThread::setCurrentComponent(curComponent);

    uint32_t reqId = notifyRequest.getRequestId();
    if (areg::isAttributeId(reqId) || areg::isResponseId(reqId))
    {
        processAttributeEvent(notifyRequest);
    }
    else
    {
        processStubEvent( static_cast<StubEvent &>(notifyRequest) );
    }

    ComponentThread::setCurrentComponent(nullptr);
}

inline void StubEventConsumer::_localProcessConnectEvent( StubConnectEvent & notifyConnect )
{
    if ( notifyConnect.getRequestId() == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) )
    {
        if (notifyConnect.getRequestType() == areg::RequestType::ServiceConnection)
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
// StubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void StubEventConsumer::startEventProcessing( Event & eventElem )
{
    mCurEvent = &eventElem;
    StubEvent* stubEvent = AREG_RUNTIME_CAST(&eventElem, StubEvent);
    if ( stubEvent != nullptr )
    {
        if ( stubEvent->getTargetStub() == mStubAddress )
        {
            RequestEvent* reqEvent = AREG_RUNTIME_CAST(stubEvent, RequestEvent);
            if (reqEvent != nullptr)
            {
                _localProcessRequestEvent(*reqEvent);
            }
            else
            {
                NotifyRequestEvent * notifyRequest = AREG_RUNTIME_CAST(stubEvent, NotifyRequestEvent);
                if ( notifyRequest != nullptr )
                {
                    _localProcessNotifyRequestEvent(*notifyRequest);
                }
                else
                {
                    StubConnectEvent * stubConnectEvent = AREG_RUNTIME_CAST(stubEvent, StubConnectEvent);
                    if (stubConnectEvent != nullptr)
                    {
                        _localProcessConnectEvent(*stubConnectEvent);
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
            // ignore, wrong stub address
        }
    }
    else
    {
        processGenericEvent(eventElem);
    }

    mCurEvent = nullptr;
}
