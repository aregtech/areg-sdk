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
namespace areg {


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
StubEvent::StubEvent( const StubAddress& toTarget, areg::EventType eventType )
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
const InStream & StubEvent::read_stream( const InStream & stream )
{
    StreamableEvent::read_stream(stream);
    stream >> mTargetStubAddress;
    return stream;
}

OutStream & StubEvent::write_stream( OutStream & stream ) const
{
    StreamableEvent::write_stream(stream);
    stream << mTargetStubAddress;
    return stream;
}

void StubEvent::deliver_event()
{
    if ( mTargetThread == nullptr )
    {
        Thread * thread = Thread::find_by_name( mTargetStubAddress.thread() );
        register_for_thread( thread != nullptr ? AREG_RUNTIME_CAST(thread, DispatcherThread) : nullptr );
    }

    if ( mTargetThread != nullptr )
    {
        StreamableEvent::deliver_event();
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

inline void StubEventConsumer::_local_request( RequestEvent & reqEvent )
{
    Component *curComponent   = Component::find_by_name(reqEvent.target_stub().role_name());
    ComponentThread::set_current_component(curComponent);

    if (areg::is_request_id(reqEvent.request_id()))
    {
        process_request_event(reqEvent);
    }
    else
    {
        process_stub_event(static_cast<StubEvent&>(reqEvent));
    }

    ComponentThread::set_current_component(nullptr);
}

inline void StubEventConsumer::_local_notify_request( NotifyRequestEvent & notifyRequest )
{
    Component *curComponent   = Component::find_by_name(notifyRequest.target_stub().role_name());
    ComponentThread::set_current_component(curComponent);

    uint32_t reqId = notifyRequest.request_id();
    if (areg::is_attribute_id(reqId) || areg::is_response_id(reqId))
    {
        process_attribute_event(notifyRequest);
    }
    else
    {
        process_stub_event( static_cast<StubEvent &>(notifyRequest) );
    }

    ComponentThread::set_current_component(nullptr);
}

inline void StubEventConsumer::_local_connect( StubConnectEvent & notifyConnect )
{
    if ( notifyConnect.request_id() == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) )
    {
        if (notifyConnect.request_type() == areg::RequestType::ServiceConnection)
        {
            process_registered_event(notifyConnect.target_stub(), notifyConnect.connection_status());
        }
        else
        {
            process_connect_event(notifyConnect.event_source(), notifyConnect.connection_status());
        }
    }
    else
    {
        process_stub_event( static_cast<StubEvent &>(notifyConnect));
    }
}

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void StubEventConsumer::start_event_processing( Event & eventElem )
{
    mCurEvent = &eventElem;
    StubEvent* stubEvent = AREG_RUNTIME_CAST(&eventElem, StubEvent);
    if ( stubEvent != nullptr )
    {
        if ( stubEvent->target_stub() == mStubAddress )
        {
            RequestEvent* reqEvent = AREG_RUNTIME_CAST(stubEvent, RequestEvent);
            if (reqEvent != nullptr)
            {
                _local_request(*reqEvent);
            }
            else
            {
                NotifyRequestEvent * notifyRequest = AREG_RUNTIME_CAST(stubEvent, NotifyRequestEvent);
                if ( notifyRequest != nullptr )
                {
                    _local_notify_request(*notifyRequest);
                }
                else
                {
                    StubConnectEvent * stubConnectEvent = AREG_RUNTIME_CAST(stubEvent, StubConnectEvent);
                    if (stubConnectEvent != nullptr)
                    {
                        _local_connect(*stubConnectEvent);
                    }
                    else
                    {
                        process_stub_event(*stubEvent);
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
        process_generic_event(eventElem);
    }

    mCurEvent = nullptr;
}

} // namespace areg
