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

StubEvent::StubEvent(const EventEnvelope& envelope) noexcept
    : Event (envelope)
{
}

StubEvent::StubEvent(EventEnvelope&& envelope) noexcept
    : Event (std::move(envelope))
{
}

StubEvent::StubEvent( const StubAddress& toTarget, areg::EventType eventType, uint32_t initSize /*= 0u*/ )
    : Event (eventType, initSize)
{
    areg::EventHeader* hdr{ header() };
    if (hdr != nullptr)
        toTarget.to_event(*hdr);
}

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
StubEventConsumer::StubEventConsumer( const StubAddress & stubAddress )
    : EventConsumer ( )
    , mStubAddress  ( stubAddress )
    , mCurEvent     ( nullptr )
{
}

inline void StubEventConsumer::_local_request( RequestEvent & reqEvent )
{
    Component* curComponent = Component::find_by_name(mStubAddress.role_name());
    ComponentThread::set_current_component(curComponent);

    if (areg::is_request_id(reqEvent.request_id()))
        process_request_event(reqEvent);
    else
        process_stub_event(static_cast<StubEvent&>(reqEvent));

    ComponentThread::set_current_component(nullptr);
}

inline void StubEventConsumer::_local_notify_request( NotifyRequestEvent & notifyRequest )
{
    Component* curComponent = Component::find_by_name(mStubAddress.role_name());
    ComponentThread::set_current_component(curComponent);

    const uint32_t reqId{ notifyRequest.request_id() };
    if (areg::is_attribute_id(reqId) || areg::is_response_id(reqId))
        process_attribute_event(notifyRequest);
    else
        process_stub_event(static_cast<StubEvent&>(notifyRequest));

    ComponentThread::set_current_component(nullptr);
}

inline void StubEventConsumer::_local_connect( StubConnectEvent & notifyConnect )
{
    if ( notifyConnect.request_id() == static_cast<uint32_t>(areg::FuncIdRange::ResponseServiceProviderConnection) )
    {
        ProxyAddress proxyAddr = notifyConnect.event_source();
        if (proxyAddr.is_valid())
            process_connect_event(proxyAddr, notifyConnect.connection_status());
        else
            process_registered_event(mStubAddress, notifyConnect.connection_status());
    }
    else
    {
        process_stub_event(static_cast<StubEvent&>(notifyConnect));
    }
}

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void StubEventConsumer::start_event_processing( Event & eventElem )
{
    const areg::EventType eventType{ eventElem.event_type() };
    const EventEnvelope& envelope{ eventElem.envelope() };

    if (!areg::is_to_provider(eventType))
    {
        process_generic_event(eventElem);
        return;
    }

    if (envelope.provider_number() != static_cast<uint32_t>(mStubAddress))
        return;

    mCurEvent = &eventElem;
    switch (eventType)
    {
    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventRemoteProviderConnect:
        _local_connect(static_cast<StubConnectEvent&>(eventElem));
        break;
    case areg::EventType::EventLocalRequest:            // fall through
    case areg::EventType::EventRemoteRequest:
        _local_request(static_cast<RequestEvent&>(eventElem));
        break;
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventRemoteNotifyRequest:
        _local_notify_request(static_cast<NotifyRequestEvent&>(eventElem));
        break;
    default:
        process_stub_event(static_cast<StubEvent&>(eventElem));
        break;
    }
    mCurEvent = nullptr;
}

} // namespace areg
