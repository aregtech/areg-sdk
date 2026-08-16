/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ServiceResponseEvent.hpp"
#include "areg/logging/areg_log.h"

namespace areg {

DEF_LOG_SCOPE(areg_component_StubEventConsumer, refuse_request);

//////////////////////////////////////////////////////////////////////////
// StubEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////

StubEventConsumer::StubEventConsumer( const StubAddress & stubAddress )
    : EventConsumer ( )
    , mStubAddress  ( stubAddress )
    , mCurEvent     ( nullptr )
{
}

inline void StubEventConsumer::_refuse_request(ServiceRequestEvent& reqEvent)
{
    LOG_SCOPE( areg_component_StubEventConsumer, refuse_request );

    LOG_WARN("Provider [ %s ] is not ready, refusing request [ %u ]"
                , StubAddress::to_path(mStubAddress).as_string(), reqEvent.request_id());

    ServiceResponseEvent failure{ ProxyBase::request_failure_event( reqEvent.event_source()
                                                                 , reqEvent.request_id()
                                                                 , areg::ResultType::MessageUndelivered
                                                                 , reqEvent.sequence_number()) };
    if (failure.is_valid())
    {
        failure.deliver_event();
    }
}

inline void StubEventConsumer::_local_request(ServiceRequestEvent& reqEvent )
{
    if (can_process_requests() == false)
    {
        _refuse_request(reqEvent);
        return;
    }

    Component* curComponent = Component::find_by_name(mStubAddress.role_name());
    ComponentThread::set_current_component(curComponent);

    if (areg::is_request_id(reqEvent.request_id()))
        process_request_event(reqEvent);
    else
        process_stub_event(static_cast<StubEvent&>(reqEvent));

    ComponentThread::set_current_component(nullptr);
}

inline void StubEventConsumer::_local_notify_request(ServiceRequestEvent& notifyRequest )
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

void StubEventConsumer::start_event_processing( Event & eventElem )
{
    const areg::EventType eventType{ eventElem.event_type() };
    const MessageEnvelope& envelope{ eventElem.envelope() };

    if (!areg::is_to_provider(eventType))
    {
        process_generic_event(eventElem);
        return;
    }

    if (envelope.provider_number() != static_cast<uint32_t>(mStubAddress))
        return;

    mCurEvent = &eventElem;

    // A dispatched areg::Event is type erased -- EventQueue holds it by value, so its dynamic
    // type is areg::Event and nothing else. Casting the reference down to a typed event is
    // undefined behaviour, and every member call made through such a reference is undefined
    // too, here and in the generated code this hands it to. The typed events add no state --
    // all of them are exactly sizeof(Event) and read the EventHeader -- so a real object is
    // built over the same envelope instead. The envelope shares the payload through a shared
    // pointer: one reference count, no copy, and the temporary destructor is a no-op because
    // Event::~Event() returns early while the original still holds a reference.
    switch (eventType)
    {
    case areg::EventType::EventLocalProviderConnect:    // fall through
    case areg::EventType::EventRemoteProviderConnect:
    {
        StubConnectEvent connectEvent{ eventElem.envelope() };
        _local_connect(connectEvent);
    }
    break;
    case areg::EventType::EventLocalRequest:            // fall through
    case areg::EventType::EventRemoteRequest:
    {
        ServiceRequestEvent reqEvent{ eventElem.envelope() };
        _local_request(reqEvent);
    }
    break;
    case areg::EventType::EventLocalNotifyRequest:      // fall through
    case areg::EventType::EventRemoteNotifyRequest:
    {
        ServiceRequestEvent notifyRequest{ eventElem.envelope() };
        _local_notify_request(notifyRequest);
    }
    break;
    default:
    {
        StubEvent stubEvent{ eventElem.envelope() };
        process_stub_event(stubEvent);
    }
    break;
    }

    mCurEvent = nullptr;
}

} // namespace areg
