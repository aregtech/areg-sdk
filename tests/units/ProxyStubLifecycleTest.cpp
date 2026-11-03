/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan).
 * Areg SDK is dual-licensed under Apache 2.0 and commercial licenses.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/SharedBuffer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ProxyListener.hpp"
#include "areg/component/RequestEvents.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/component/StubBase.hpp"

#include <algorithm>
#include <any>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iterator>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace
{
    using namespace areg;

    constexpr char CONSUMER_MODEL[]{ "ContractConsumerModel" };
    constexpr char PROVIDER_MODEL[]{ "ContractProviderModel" };
    constexpr char CONSUMER_ROLE[]{ "ContractConsumerRole" };
    constexpr char PROVIDER_ROLE[]{ "ContractProviderRole" };
    constexpr char CONSUMER_THREAD[]{ "ContractConsumerThread" };
    constexpr char PROVIDER_THREAD[]{ "ContractProviderThread" };
    constexpr uint32_t REQUEST = REQUEST_ID_FIRST;
    constexpr uint32_t RESPONSE = RESPONSE_ID_FIRST;
    constexpr uint32_t ATTRIBUTE = ATTRIBUTE_ID_FIRST;
    constexpr uint32_t REQUESTS[]{ REQUEST };
    constexpr uint32_t RESPONSES[]{ RESPONSE };
    constexpr uint32_t ATTRIBUTES[]{ ATTRIBUTE };
    constexpr uint32_t PARAM_COUNTS[]{ 1 };
    const InterfaceData INTERFACE
    {
          "LifecycleContractService", Version(1, 0, 0), ServiceType::Local
        , 1, 1, 1, REQUESTS, RESPONSES, ATTRIBUTES, RESPONSES, PARAM_COUNTS
    };

    enum class ObservationKind
    {
          Connected
        , Disconnected
        , Request
        , Response
        , Attribute
        , Failure
        , Deleted
        , Drained
        , UnloadReturned
    };

    struct Observation
    {
        ObservationKind kind;
        uint32_t value;
        ResultType result;
        SequenceNumber sequence;
    };

    class LifecycleObservations
    {
    public:
        void record(ObservationKind kind, uint32_t value = 0, ResultType result = ResultType::RequestOK
                    , SequenceNumber sequence = 0)
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                mRows.push_back(Observation{ kind, value, result, sequence });
            }
            mChanged.notify_all();
        }

        bool wait_for(ObservationKind kind, size_t count)
        {
            std::unique_lock<std::mutex> lock(mLock);
            return mChanged.wait_for(lock, std::chrono::seconds(30), [&]
            {
                return static_cast<size_t>(std::count_if(mRows.begin(), mRows.end(), [kind](const Observation& row)
                {
                    return row.kind == kind;
                })) >= count;
            });
        }

        std::vector<Observation> rows(ObservationKind kind)
        {
            std::lock_guard<std::mutex> lock(mLock);
            std::vector<Observation> result;
            std::copy_if(mRows.begin(), mRows.end(), std::back_inserter(result)
                         , [kind](const Observation& row) { return row.kind == kind; });
            return result;
        }

        std::vector<Observation> all()
        {
            std::lock_guard<std::mutex> lock(mLock);
            return mRows;
        }

    private:
        std::mutex mLock;
        std::condition_variable mChanged;
        std::vector<Observation> mRows;
    };

    struct LifecycleCommand
    {
        std::function<void()> action;
        std::promise<void>* done;
    };

    AREG_DECLARE_EVENT(LifecycleCommand, LifecycleCommandEvent, LifecycleCommandConsumer);
}

class ProxyStubLifecycleRequest final : public areg::LocalRequestEvent
{
    AREG_DECLARE_RUNTIME_EVENT(ProxyStubLifecycleRequest)
public:
    ProxyStubLifecycleRequest(const areg::ProxyAddress& source, const areg::StubAddress& target, uint32_t request, uint32_t reserve)
        : LocalRequestEvent(source, target, request, reserve)
    {
        header()->eventId = CLASS_ID;
    }

    ProxyStubLifecycleRequest(const areg::SharedBuffer& data, const areg::ProxyAddress& source
                              , const areg::StubAddress& target, uint32_t request)
        : LocalRequestEvent(data, source, target, request)
    {
        header()->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT(ProxyStubLifecycleRequest, areg::LocalRequestEvent)

class ProxyStubLifecycleNotify final : public areg::LocalNotifyRequestEvent
{
    AREG_DECLARE_RUNTIME_EVENT(ProxyStubLifecycleNotify)
public:
    ProxyStubLifecycleNotify(const areg::ProxyAddress& source, const areg::StubAddress& target
                             , uint32_t message, areg::RequestType type)
        : LocalNotifyRequestEvent(source, target, message, type)
    {
        header()->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT(ProxyStubLifecycleNotify, areg::LocalNotifyRequestEvent)

class ProxyStubLifecycleResponse final : public areg::LocalResponseEvent
{
    AREG_DECLARE_RUNTIME_EVENT(ProxyStubLifecycleResponse)
public:
    ProxyStubLifecycleResponse(const areg::ProxyAddress& target, uint32_t message, areg::ResultType result, uint32_t reserve = 0)
        : LocalResponseEvent(target, result, message, areg::SEQUENCE_NUMBER_NOTIFY, reserve)
    {
        header()->eventId = CLASS_ID;
    }

    ProxyStubLifecycleResponse(const areg::SharedBuffer& data, const areg::ProxyAddress& target, uint32_t message, areg::ResultType result)
        : LocalResponseEvent(data, target, result, message)
    {
        header()->eventId = CLASS_ID;
    }
};
AREG_IMPLEMENT_RUNTIME_EVENT(ProxyStubLifecycleResponse, areg::LocalResponseEvent)

namespace
{
    class LifecycleProxy final : public ProxyBase
    {
    public:
        LifecycleProxy(const String& role, DispatcherThread* owner)
            : ProxyBase(role, INTERFACE, owner)
            , mResponseValue(0)
            , mAttributeValue(0)
        {
        }

        static ProxyBase* create(const String& role, DispatcherThread* owner)
        {
            return new LifecycleProxy(role, owner);
        }

        SequenceNumber request(uint32_t value, NotificationConsumer& listener)
        {
            auto event = create_request_event(REQUEST, sizeof(value));
            event.write_stream() << value;
            return send_request_event(event, &listener);
        }

        uint32_t response_value() const { return mResponseValue; }
        uint32_t attribute_value() const { return mAttributeValue; }

    protected:
        void process_response_event(ServiceResponseEvent& event) override
        {
            const ResultType result = static_cast<ResultType>(event.envelope().result());
            uint32_t message = event.response_id();
            if (message == REQUEST)
                message = RESPONSE;
            if ((result == ResultType::DataOK) || (result == ResultType::RequestOK))
            {
                if (message == ATTRIBUTE)
                    event.read_stream() >> mAttributeValue;
                else
                    event.read_stream() >> mResponseValue;
            }
            set_state(message, ((result == ResultType::DataOK) || (result == ResultType::RequestOK))
                              ? DataState::DataIsOK : DataState::DataIsInvalid);
            notify_listeners(message, result, event.sequence_number());
        }

        void process_custom_event(Event&) override
        {
        }

        void register_service_listeners() override
        {
            ProxyBase::register_service_listeners();
            ProxyStubLifecycleResponse::add_listener(*this, proxy_dispatcher_thread());
        }

        void unregister_service_listeners() noexcept override
        {
            ProxyStubLifecycleResponse::remove_listener(*this, proxy_dispatcher_thread());
            ProxyBase::unregister_service_listeners();
        }

        ServiceAvailableEvent create_service_available(NotificationConsumer& listener) override
        {
            return ServiceAvailableEvent(listener);
        }

        NotificationEvent create_client_notification(const NotificationEventData& data) const override
        {
            return NotificationEvent(data);
        }

        ServiceRequestEvent create_request(const SharedBuffer& data, uint32_t request) override
        {
            return ProxyStubLifecycleRequest(data, proxy_address(), stub_address(), request);
        }

        ServiceRequestEvent create_request(uint32_t request, uint32_t reserve) override
        {
            return ProxyStubLifecycleRequest(proxy_address(), stub_address(), request, reserve);
        }

        ServiceRequestEvent create_notification_request(uint32_t message, RequestType type) override
        {
            return ProxyStubLifecycleNotify(proxy_address(), stub_address(), message, type);
        }

        ServiceResponseEvent create_request_failed(const ProxyAddress& target, uint32_t message, ResultType reason
                                                  , const SequenceNumber& sequence) const override
        {
            ProxyStubLifecycleResponse result(target, message, reason);
            result.set_sequence_number(sequence);
            return result;
        }

    private:
        uint32_t mResponseValue;
        uint32_t mAttributeValue;
    };

    class LifecycleComponent : public Component, public LifecycleCommandConsumer
    {
    public:
        LifecycleComponent(const ComponentEntry& entry, ComponentThread& owner)
            : Component(entry, owner)
            , mObservations(*std::any_cast<LifecycleObservations*>(entry.data()))
        {
        }

        ~LifecycleComponent() override
        {
            mObservations.record(ObservationKind::Deleted);
        }

        void startup_component(ComponentThread& owner) override
        {
            Component::startup_component(owner);
            LifecycleCommandEvent::add_listener(*this, owner);
        }

        void shutdown_component(ComponentThread& owner) override
        {
            LifecycleCommandEvent::remove_listener(*this, owner);
            Component::shutdown_component(owner);
        }

    protected:
        void process_event(const LifecycleCommand& command) override
        {
            command.action();
            if (command.done != nullptr)
                command.done->set_value();
        }

        LifecycleObservations& mObservations;
    };

    class LifecycleProvider final : public LifecycleComponent, public StubBase
    {
    public:
        LifecycleProvider(const ComponentEntry& entry, ComponentThread& owner)
            : LifecycleComponent(entry, owner)
            , StubBase(static_cast<Component&>(*this), INTERFACE)
            , mValue(7)
            , mSessions()
        {
        }

        void complete(uint32_t value, ResultType result = ResultType::RequestOK)
        {
            StubListenerList listeners;
            auto event = prepare_response_event(RESPONSE, result, sizeof(value), listeners);
            if (event.is_valid())
            {
                if (result == ResultType::RequestOK)
                    event.write_stream() << value;
                send_response_notification(listeners, event);
            }
        }

        void complete_deferred()
        {
            for (auto pos = mSessions.rbegin(); pos != mSessions.rend(); ++pos)
            {
                EXPECT_TRUE(prepare_response(pos->first));
                complete(pos->second);
                EXPECT_FALSE(prepare_response(pos->first));
            }
            mSessions.clear();
        }

        void publish(uint32_t value, uint32_t message = ATTRIBUTE)
        {
            mValue = value;
            send_notification(message);
        }

        void startup_service_interface(Component& holder) override
        {
            StubBase::startup_service_interface(holder);
            ProxyStubLifecycleRequest::add_listener(static_cast<StubBase&>(*this), holder.master_thread());
            ProxyStubLifecycleNotify::add_listener(static_cast<StubBase&>(*this), holder.master_thread());
        }

        void shutdown_service_interface(Component& holder) noexcept override
        {
            ProxyStubLifecycleRequest::remove_listener(static_cast<StubBase&>(*this), holder.master_thread());
            ProxyStubLifecycleNotify::remove_listener(static_cast<StubBase&>(*this), holder.master_thread());
            StubBase::shutdown_service_interface(holder);
        }

        void send_notification(uint32_t message) override
        {
            SharedBuffer data;
            data << mValue;
            send_update_event(message, data, message == ATTRIBUTE ? ResultType::DataOK : ResultType::RequestOK);
        }

        void error_request(uint32_t message, bool canceled) override
        {
            if ((message == REQUEST) || (message == RESPONSE))
                complete(0, canceled ? ResultType::RequestCanceled : ResultType::RequestError);
        }

    protected:
        ServiceResponseEvent create_response(const ProxyAddress& target, uint32_t message, ResultType result
                                            , const SharedBuffer& data) const override
        {
            return ProxyStubLifecycleResponse(data, target, message, result);
        }

        ServiceResponseEvent create_response_event(const ProxyAddress& target, uint32_t message
                                                  , ResultType result, uint32_t reserve) const override
        {
            return ProxyStubLifecycleResponse(target, message, result, reserve);
        }

        void process_request_event(ServiceRequestEvent& event) override
        {
            uint32_t value = 0;
            event.read_stream() >> value;
            mObservations.record(ObservationKind::Request, value, ResultType::RequestOK, event.sequence_number());
            Listener listener(event.request_id(), event.sequence_number(), event.event_source());
            if (!can_execute_request(listener, RESPONSE, event.sequence_number()))
                return;
            if (value >= 100)
                mSessions.emplace_back(unblock_current_request(), value);
            else if (value != 2)
                complete(value * 10);
        }

        void process_attribute_event(ServiceRequestEvent& event) override
        {
            const uint32_t message = event.request_id();
            const ProxyAddress source = event.event_source();
            if (event.request_type() == RequestType::StartNotify)
            {
                if (add_notification_listener(message, source) && (message == ATTRIBUTE))
                    send_notification(message);
            }
            else if (event.request_type() == RequestType::StopNotify)
                remove_notification_listener(message, source);
            else if (event.request_type() == RequestType::RemoveAllNotify)
                clear_all_listeners(source);
        }

    private:
        uint32_t mValue;
        std::vector<std::pair<SessionID, uint32_t>> mSessions;
    };

    class LifecycleConsumer final : public LifecycleComponent, public ProxyListener
    {
    public:
        LifecycleConsumer(const ComponentEntry& entry, ComponentThread& owner)
            : LifecycleComponent(entry, owner)
            , mProxy(ProxyBase::acquire_proxy(String(PROVIDER_ROLE), INTERFACE, *this, LifecycleProxy::create, owner))
        {
        }

        ~LifecycleConsumer() override
        {
            mProxy->free_proxy(*this);
        }

        LifecycleProxy& proxy()
        {
            return *static_cast<LifecycleProxy*>(mProxy.get());
        }

        bool service_connected(ServiceConnectionState status, ProxyBase&) override
        {
            mObservations.record(is_service_connected(status) ? ObservationKind::Connected : ObservationKind::Disconnected);
            return true;
        }

        void process_notification_event(NotificationEvent& event) override
        {
            const NotificationEventData& data = std::as_const(event).data();
            process_notification(data.notify_id(), data.notify_type(), data.sequence());
        }

        void process_notification(uint32_t message, ResultType result, SequenceNumber sequence) override
        {
            if (message == ATTRIBUTE)
                mObservations.record(ObservationKind::Attribute, proxy().attribute_value(), result, sequence);
            else if (result == ResultType::RequestOK)
                mObservations.record(ObservationKind::Response, proxy().response_value(), result, sequence);
            else
                mObservations.record(ObservationKind::Failure, 0, result, sequence);
        }

    private:
        std::shared_ptr<ProxyBase> mProxy;
    };

    bool invoke_component(const char* role, std::function<void()> action)
    {
        Component* component = Component::find_by_name(String(role));
        if (component == nullptr)
            return false;
        std::promise<void> done;
        auto result = done.get_future();
        if (!LifecycleCommandEvent::send_event(LifecycleCommand{ std::move(action), &done }, component->master_thread()))
            return false;
        // The completion object remains alive until the accepted command has returned.
        const bool ready = result.wait_for(std::chrono::seconds(30)) == std::future_status::ready;
        EXPECT_TRUE(ready);
        if (!ready)
            result.wait();
        return ready;
    }

    template<typename ComponentType>
    Model lifecycle_model(const char* name, const char* thread, const char* role, LifecycleObservations& observations)
    {
        ComponentEntry entry(String(thread), String(role)
            , [](const ComponentEntry& item, ComponentThread& owner) -> Component* { return new ComponentType(item, owner); }
            , [](Component& component, const ComponentEntry&) { delete &component; });
        entry.set_data(&observations);
        ComponentThreadEntry owner{ String(thread) };
        owner.mMaxQueue = 512;
        owner.mDropOnFull = Bool::False;
        owner.add_component(entry);
        Model model{ String(name) };
        model.add_thread(owner);
        return model;
    }

    class LifecycleModels
    {
    public:
        ~LifecycleModels()
        {
            ComponentLoader::remove_component_model(String(CONSUMER_MODEL));
            ComponentLoader::remove_component_model(String(PROVIDER_MODEL));
            ComponentLoader::remove_component_model(String("ContractInvalidModel"));
        }
    };

    class LifecycleApplication
    {
    public:
        LifecycleApplication()
        {
            Application::setup(false, true, false, true, false);
        }

        ~LifecycleApplication()
        {
            Application::release();
        }
    };
}

TEST(ProxyStubLifecycleTest, late_provider_requests_subscriptions_restart_and_queued_unload)
{
    LifecycleObservations observations;
    LifecycleApplication application;
    LifecycleModels models;
    ASSERT_TRUE(ComponentLoader::add_model_unique(
        lifecycle_model<LifecycleConsumer>(CONSUMER_MODEL, CONSUMER_THREAD, CONSUMER_ROLE, observations)));
    ASSERT_TRUE(ComponentLoader::add_model_unique(
        lifecycle_model<LifecycleProvider>(PROVIDER_MODEL, PROVIDER_THREAD, PROVIDER_ROLE, observations)));
    ASSERT_TRUE(ComponentLoader::load_component_model(String(CONSUMER_MODEL), true));
    auto* consumer = static_cast<LifecycleConsumer*>(Component::find_by_name(String(CONSUMER_ROLE)));
    ASSERT_NE(nullptr, consumer);
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(CONSUMER_MODEL)));
    EXPECT_EQ(ThreadState::Running, Application::thread_state(CONSUMER_THREAD));
    EXPECT_TRUE(ComponentLoader::find_component_list(String(CONSUMER_THREAD)).is_valid());
    EXPECT_TRUE(ComponentLoader::find_component_entry(String(CONSUMER_ROLE), String(CONSUMER_THREAD)).is_valid());
    EXPECT_FALSE(ComponentLoader::find_component_entry(String(PROVIDER_ROLE), String(CONSUMER_THREAD)).is_valid());
    ProxyAddress consumerAddress;
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&]
    {
        EXPECT_FALSE(consumer->proxy().is_connected());
        EXPECT_FALSE(consumer->proxy().stub_address().is_valid());
        EXPECT_EQ(&consumer->master_thread(), consumer->find_event_consumer(LifecycleCommandEvent::CLASS_ID));
        consumerAddress = consumer->proxy().proxy_address();
    }));

    ASSERT_TRUE(ComponentLoader::load_component_model(String(PROVIDER_MODEL), true));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Connected, 1));
    auto* provider = static_cast<LifecycleProvider*>(Component::find_by_name(String(PROVIDER_ROLE)));
    ASSERT_NE(nullptr, provider);
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(CONSUMER_MODEL)));
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(PROVIDER_MODEL)));
    EXPECT_FALSE(ComponentLoader::load_component_model(String("ContractMissingModel"), true));
    ComponentLoader::wait_model_unload(String("ContractMissingModel"));
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(CONSUMER_MODEL)));
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(1, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Response, 1));
    ASSERT_EQ(1u, observations.rows(ObservationKind::Response).size());
    EXPECT_EQ(10u, observations.rows(ObservationKind::Response)[0].value);

    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(2, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Request, 2));
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(1, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Failure, 1));
    EXPECT_EQ(ResultType::RequestBusy, observations.rows(ObservationKind::Failure)[0].result);
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->complete(22); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Response, 2));
    EXPECT_EQ(22u, observations.rows(ObservationKind::Response)[1].value);

    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&]
    {
        consumer->proxy().request(301, *consumer);
        consumer->proxy().request(302, *consumer);
    }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Request, 5));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->complete_deferred(); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Response, 4));
    const auto responses = observations.rows(ObservationKind::Response);
    EXPECT_EQ(302u, responses[2].value);
    EXPECT_EQ(301u, responses[3].value);
    EXPECT_GT(responses[2].sequence, responses[3].sequence);

    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(2, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Request, 6));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->cancel_all_requests(); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Failure, 2));
    EXPECT_EQ(ResultType::RequestCanceled, observations.rows(ObservationKind::Failure)[1].result);
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(2, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Request, 7));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->error_all_requests(); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Failure, 3));
    EXPECT_EQ(ResultType::RequestError, observations.rows(ObservationKind::Failure)[2].result);

    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().set_notification(ATTRIBUTE, consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Attribute, 2));
    auto attributes = observations.rows(ObservationKind::Attribute);
    ASSERT_EQ(2u, attributes.size());
    EXPECT_EQ(ResultType::DataInvalid, attributes[0].result);
    EXPECT_EQ(0u, attributes[0].value);
    EXPECT_EQ(ResultType::DataOK, attributes[1].result);
    EXPECT_EQ(7u, attributes[1].value);
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->publish(8); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Attribute, 3));
    EXPECT_EQ(8u, observations.rows(ObservationKind::Attribute)[2].value);
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().clear_notification(ATTRIBUTE, consumer); }));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&] { provider->publish(9); }));
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [] {}));
    EXPECT_EQ(3u, observations.rows(ObservationKind::Attribute).size());
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().set_notification(ATTRIBUTE, consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Attribute, 5));
    attributes = observations.rows(ObservationKind::Attribute);
    EXPECT_EQ(ResultType::DataInvalid, attributes[3].result);
    EXPECT_EQ(8u, attributes[3].value);
    EXPECT_EQ(ResultType::DataOK, attributes[4].result);
    EXPECT_EQ(9u, attributes[4].value);

    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().set_notification(RESPONSE, consumer); }));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [] {}));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [&]
    {
        for (uint32_t value = 0; value < 128; ++value)
            provider->publish(1'000 + value, RESPONSE);
    }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Response, 132));
    const auto broadcast = observations.rows(ObservationKind::Response);
    ASSERT_EQ(132u, broadcast.size());
    for (uint32_t index = 0; index < 128; ++index)
    {
        EXPECT_EQ(1'000 + index, broadcast[4 + index].value);
        EXPECT_EQ(SEQUENCE_NUMBER_NOTIFY, broadcast[4 + index].sequence);
    }
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().clear_notification(RESPONSE, consumer); }));
    ASSERT_TRUE(invoke_component(PROVIDER_ROLE, [] {}));
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(2, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Request, 8));
    StubAddress oldProvider = StubBase::registered_address(
        StubAddress(INTERFACE, String(PROVIDER_ROLE), String(PROVIDER_THREAD)));
    ComponentLoader::unload_component_model(true, String(PROVIDER_MODEL));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Failure, 4));
    EXPECT_EQ(ResultType::ServiceUnavailable, observations.rows(ObservationKind::Failure)[3].result);
    EXPECT_EQ(nullptr, StubBase::find_stub(oldProvider));
    EXPECT_EQ(nullptr, Component::find_by_name(String(PROVIDER_ROLE)));
    const auto order = observations.all();
    const auto disconnected = std::find_if(order.begin(), order.end()
        , [](const Observation& row) { return row.kind == ObservationKind::Disconnected; });
    const auto failed = std::find_if(order.begin(), order.end(), [](const Observation& row)
    {
        return (row.kind == ObservationKind::Failure) && (row.result == ResultType::ServiceUnavailable);
    });
    ASSERT_NE(order.end(), disconnected);
    ASSERT_NE(order.end(), failed);
    EXPECT_LT(disconnected - order.begin(), failed - order.begin());
    ASSERT_TRUE(ComponentLoader::load_component_model(String(PROVIDER_MODEL), true));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Connected, 2));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Attribute, 6));
    EXPECT_EQ(7u, observations.rows(ObservationKind::Attribute).back().value);
    EXPECT_EQ(ResultType::DataOK, observations.rows(ObservationKind::Attribute).back().result);
    ASSERT_TRUE(invoke_component(CONSUMER_ROLE, [&] { consumer->proxy().request(1, *consumer); }));
    ASSERT_TRUE(observations.wait_for(ObservationKind::Response, 133));
    EXPECT_EQ(10u, observations.rows(ObservationKind::Response).back().value);

    ComponentThreadList invalidThreads;
    invalidThreads.mListThreads.add(ComponentThreadEntry{});
    ASSERT_TRUE(ComponentLoader::add_model_unique(Model(String("ContractInvalidModel"), invalidThreads)));
    EXPECT_FALSE(ComponentLoader::load_component_model(String("ContractInvalidModel"), true));
    EXPECT_EQ(ModelState::Unloaded, ComponentLoader::model_state(String("ContractInvalidModel")));
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(CONSUMER_MODEL)));
    EXPECT_TRUE(ComponentLoader::is_model_loaded(String(PROVIDER_MODEL)));

    auto entered = std::make_shared<std::promise<void>>();
    auto held = entered->get_future();
    std::promise<void> release;
    auto released = release.get_future().share();
    auto* consumerThread = &consumer->master_thread();
    ASSERT_TRUE(LifecycleCommandEvent::send_event(LifecycleCommand
    {
        [&, entered, released]
        {
            entered->set_value();
            released.wait();
            ComponentLoader::unload_component_model(true, String(CONSUMER_MODEL));
            observations.record(ObservationKind::UnloadReturned);
        }, nullptr
    }, *consumerThread));
    const bool holding = held.wait_for(std::chrono::seconds(30)) == std::future_status::ready;
    EXPECT_TRUE(holding);
    if (!holding)
    {
        release.set_value();
        return;
    }
    for (uint32_t value = 0; value < 64; ++value)
    {
        EXPECT_TRUE(LifecycleCommandEvent::send_event(LifecycleCommand
        {
            [&, value] { observations.record(ObservationKind::Drained, value); }, nullptr
        }, *consumerThread));
    }
    release.set_value();
    ASSERT_TRUE(observations.wait_for(ObservationKind::UnloadReturned, 1));
    ComponentLoader::wait_model_unload(String(CONSUMER_MODEL));
    const auto drained = observations.rows(ObservationKind::Drained);
    ASSERT_EQ(64u, drained.size());
    for (uint32_t index = 0; index < 64; ++index)
        EXPECT_EQ(index, drained[index].value);
    EXPECT_EQ(ModelState::Unloaded, ComponentLoader::model_state(String(CONSUMER_MODEL)));
    EXPECT_EQ(nullptr, Component::find_by_name(String(CONSUMER_ROLE)));
    EXPECT_EQ(nullptr, ProxyBase::find_proxy(consumerAddress));
    EXPECT_FALSE(ComponentLoader::find_component_list(String(CONSUMER_THREAD)).is_valid());
    ComponentLoader::unload_component_model(true, String(PROVIDER_MODEL));
    EXPECT_EQ(3u, observations.rows(ObservationKind::Deleted).size());
}
