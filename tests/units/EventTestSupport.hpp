#ifndef AREG_TESTS_UNITS_EVENTTESTSUPPORT_HPP
#define AREG_TESTS_UNITS_EVENTTESTSUPPORT_HPP

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventTemplate.hpp"

#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <utility>

inline constexpr uint32_t EVENT_TEST_WAIT_MS{ 30'000u };
inline constexpr std::chrono::seconds EVENT_TEST_WAIT{ 30 };

struct LifecycleActionData
{
    std::function<void()> action;
};

AREG_DECLARE_EVENT(LifecycleActionData, LifecycleActionEvent, LifecycleActionConsumer);

class EventTestGate
{
public:
    EventTestGate()
        : mMutex()
        , mChanged()
        , mOpen(false)
    {
    }

    void open()
    {
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mOpen = true;
        }
        mChanged.notify_all();
    }

    bool wait()
    {
        std::unique_lock<std::mutex> guard(mMutex);
        return mChanged.wait_for(guard, EVENT_TEST_WAIT, [this]() { return mOpen; });
    }

private:
    std::mutex mMutex;
    std::condition_variable mChanged;
    bool mOpen;
};

class EventTestDispatcher : public areg::DispatcherThread, private LifecycleActionConsumer
{
public:
    explicit EventTestDispatcher(const areg::String & name, uint32_t capacity = 128u)
        : areg::DispatcherThread(name, areg::DEFAULT_STACK_SIZE, capacity, areg::Bool::True, 0u)
        , LifecycleActionConsumer()
    {
    }

    ~EventTestDispatcher() override
    {
        shutdown(areg::WAIT_INFINITE);
    }

    bool start_ready()
    {
        return start(EVENT_TEST_WAIT_MS) && wait_start(EVENT_TEST_WAIT_MS);
    }

    bool post_event(areg::Event & event) override
    {
        return areg::EventDispatcher::post_event(event);
    }

    bool post_action(std::function<void()> action)
    {
        return LifecycleActionEvent::send_event(LifecycleActionData{ std::move(action) }, *this, *this);
    }

    bool invoke(std::function<void()> action)
    {
        // JUSTIFICATION: caller and queued action share completion ownership across the deadline.
        const auto done{ std::make_shared<std::promise<void>>() };
        std::future<void> completed{ done->get_future() };
        if (!post_action([action = std::move(action), done]()
            {
                action();
                done->set_value();
            }))
        {
            return false;
        }
        return completed.wait_for(EVENT_TEST_WAIT) == std::future_status::ready;
    }

    bool barrier()
    {
        return invoke([]() {});
    }

private:
    void process_event(const LifecycleActionData & data) override
    {
        data.action();
    }
};

// Releases a held callback and joins the dispatcher before its consumers leave scope.
class EventTestRun
{
public:
    explicit EventTestRun(EventTestDispatcher & dispatcher, EventTestGate * gate = nullptr)
        : mDispatcher(dispatcher)
        , mGate(gate)
    {
    }

    ~EventTestRun()
    {
        if (mGate != nullptr)
        {
            mGate->open();
        }
        mDispatcher.shutdown(areg::WAIT_INFINITE);
    }

private:
    EventTestDispatcher & mDispatcher;
    EventTestGate * mGate;
};

#endif // AREG_TESTS_UNITS_EVENTTESTSUPPORT_HPP
