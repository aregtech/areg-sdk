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

namespace areg
{
    //////////////////////////////////////////////////////////////////////////
    // StubEvent class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // StubEvent class, implement runtime
    //////////////////////////////////////////////////////////////////////////
    AREG_IMPLEMENT_RUNTIME_EVENT(StubEvent, areg::StreamableEvent)

    //////////////////////////////////////////////////////////////////////////
    // StubEvent class, constructor / destructor
    //////////////////////////////////////////////////////////////////////////
    StubEvent::StubEvent( const areg::StubAddress& toTarget, areg::Event::EventType eventType )
        : areg::StreamableEvent   (eventType)
        , mTargetStubAddress(toTarget)
    {
    }

    StubEvent::StubEvent( const areg::InStream & stream  )
        : areg::StreamableEvent   (stream)
        , mTargetStubAddress(stream)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // StubEvent class, methods
    //////////////////////////////////////////////////////////////////////////
    const areg::InStream & StubEvent::readStream( const areg::InStream & stream )
    {
        areg::StreamableEvent::readStream(stream);
        stream >> mTargetStubAddress;
        return stream;
    }

    areg::OutStream & StubEvent::writeStream( areg::OutStream & stream ) const
    {
        areg::StreamableEvent::writeStream(stream);
        stream << mTargetStubAddress;
        return stream;
    }

    void StubEvent::deliverEvent()
    {
        if ( mTargetThread == nullptr )
        {
            areg::Thread * thread = areg::Thread::findThreadByName( mTargetStubAddress.getThread() );
            registerForThread( thread != nullptr ? AREG_RUNTIME_CAST(thread, areg::DispatcherThread) : nullptr );
        }

        if ( mTargetThread != nullptr )
        {
            areg::StreamableEvent::deliverEvent();
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
    areg::StubEventConsumer::StubEventConsumer( const areg::StubAddress & stubAddress )
        : areg::EventConsumer   ( )
        , mStubAddress      ( stubAddress )
        , mCurEvent         ( nullptr )
    {
    }

    inline void areg::StubEventConsumer::_localProcessRequestEvent( areg::RequestEvent & requestEvent )
    {
        areg::Component *curComponent   = areg::Component::findComponentByName(requestEvent.getTargetStub().getRoleName());
        areg::ComponentThread::setCurrentComponent(curComponent);

        if (areg::isRequestId(requestEvent.getRequestId()))
        {
            processRequestEvent(requestEvent);
        }
        else
        {
            processStubEvent(static_cast<StubEvent&>(requestEvent));
        }

        areg::ComponentThread::setCurrentComponent(nullptr);
    }

    inline void areg::StubEventConsumer::_localProcessNotifyRequestEvent( areg::NotifyRequestEvent & notifyRequest )
    {
        areg::Component *curComponent   = areg::Component::findComponentByName(notifyRequest.getTargetStub().getRoleName());
        areg::ComponentThread::setCurrentComponent(curComponent);

        uint32_t reqId = notifyRequest.getRequestId();
        if (areg::isAttributeId(reqId) || areg::isResponseId(reqId))
        {
            processAttributeEvent(notifyRequest);
        }
        else
        {
            processStubEvent( static_cast<StubEvent &>(notifyRequest) );
        }

        areg::ComponentThread::setCurrentComponent(nullptr);
    }

    inline void areg::StubEventConsumer::_localProcessConnectEvent( areg::StubConnectEvent & notifyConnect )
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
    void areg::StubEventConsumer::startEventProcessing( areg::Event & eventElem )
    {
        mCurEvent = &eventElem;
        StubEvent* stubEvent = AREG_RUNTIME_CAST(&eventElem, StubEvent);
        if ( stubEvent != nullptr )
        {
            if ( stubEvent->getTargetStub() == mStubAddress )
            {
                areg::RequestEvent* reqEvent = AREG_RUNTIME_CAST(stubEvent, areg::RequestEvent);
                if (reqEvent != nullptr)
                {
                    _localProcessRequestEvent(*reqEvent);
                }
                else
                {
                    areg::NotifyRequestEvent * notifyRequest = AREG_RUNTIME_CAST(stubEvent, areg::NotifyRequestEvent);
                    if ( notifyRequest != nullptr )
                    {
                        _localProcessNotifyRequestEvent(*notifyRequest);
                    }
                    else
                    {
                        areg::StubConnectEvent * stubConnectEvent = AREG_RUNTIME_CAST(stubEvent, areg::StubConnectEvent);
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

} // namespace areg