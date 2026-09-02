/**
 * \file    main.cpp
 * \brief   A worked answer to a design question: three workers, a monitor that
 *          watches all of them, and an operator that asks for a summary.
 *
 *          The decomposition is the point. Two services, not one: what a worker
 *          publishes about itself is a different contract from what the monitor
 *          tells an operator. State is an attribute because it has a current
 *          value; the alarm is a broadcast because it is an event with no value
 *          to read back; the summary is a request because it has an answer.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "src/services/WorkerServiceProviderBase.hpp"
#include "src/services/WorkerServiceConsumerBase.hpp"
#include "src/services/MonitorServiceProviderBase.hpp"
#include "src/services/MonitorServiceConsumerBase.hpp"

namespace
{
    constexpr std::string_view  Workers[]   { "Worker1", "Worker2", "Worker3" };
    constexpr uint32_t          WorkerCount { 3 };
    constexpr uint32_t          Working     { 1 };
    constexpr uint32_t          Stalled     { 0 };
    //! The one that stops working, so that the alarm path is exercised.
    constexpr std::string_view  Stalling    { "Worker3" };
}

//! Publishes its own state. One instance per worker, three in the model.
class Worker final  : public    areg::Component
                    , protected WorkerServiceProviderBase
                    , private   areg::TimerConsumer
{
public:
    Worker(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , WorkerServiceProviderBase(static_cast<areg::Component &>(self()))
        , areg::TimerConsumer()
        , mTimer(static_cast<areg::TimerConsumer &>(self()), "WorkerTimer")
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        set_state(Working);
        if (role_name() == Stalling.data())
        {
            mTimer.start_timer(300, static_cast<areg::DispatcherThread &>(comThread),
                               areg::TimerBase::ONE_TIME);
        }
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        mTimer.stop_timer();
        areg::Component::shutdown_component(comThread);
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        set_state(Stalled);
    }

private:
    areg::Timer mTimer;

    inline Worker & self()
    {   return (*this); }
};

class Monitor;

//! One proxy per worker. A component that consumes several instances of one
//! service holds one of these for each, rather than deriving three times.
class Watcher final : protected WorkerServiceConsumerBase
{
public:
    Watcher(const areg::String & roleName, areg::Component & owner, Monitor & monitor)
        : WorkerServiceConsumerBase(roleName, owner)
        , mMonitor(monitor)
    { }

    //! The role name of the worker this one watches: the consumer base knows it.
    inline const areg::String & watched() const
    {   return service_name(); }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final;
    void on_state_update(uint32_t State, areg::DataState state) final;

private:
    Monitor & mMonitor;
};

//! Watches every worker and answers the operator.
class Monitor final : public    areg::Component
                    , protected MonitorServiceProviderBase
{
public:
    Monitor(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MonitorServiceProviderBase(static_cast<areg::Component &>(self()))
    {
        for (const std::string_view & name : Workers)
        {
            mWatchers.push_back(std::make_unique<Watcher>(
                areg::String(name.data()), static_cast<areg::Component &>(self()), self()));
        }
    }

    void worker_changed(const areg::String & worker, uint32_t state)
    {
        if (state == Stalled)
        {
            std::cout << "monitor: alarm from " << worker << std::endl;
            broadcast_alarm(worker);
        }
    }

    uint32_t alive() const
    {
        uint32_t count{ 0 };
        for (const auto & watcher : mWatchers)
        {
            count += mStalled.contains(watcher->watched()) ? 0u : 1u;
        }
        return count;
    }

    void mark_stalled(const areg::String & worker)
    {   mStalled.add(worker); }

protected:
    void request_summary() final
    {
        response_summary(alive(), WorkerCount);
    }

private:
    std::vector<std::unique_ptr<Watcher>>   mWatchers;
    areg::StringArray                       mStalled;

    inline Monitor & self()
    {   return (*this); }
};

bool Watcher::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result{ false };
    if (WorkerServiceConsumerBase::service_connected(status, proxy))
    {
        result = true;
        if (areg::is_service_connected(status))
        {
            notify_on_state_update(true);
        }
    }

    return result;
}

void Watcher::on_state_update(uint32_t State, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        if (State == Stalled)
        {
            mMonitor.mark_stalled(watched());
        }
        mMonitor.worker_changed(watched(), State);
    }
}

//! Asks for the summary once an alarm says something is wrong.
class Operator final    : public    areg::Component
                        , protected MonitorServiceConsumerBase
{
public:
    Operator(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , MonitorServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (MonitorServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_broadcast_alarm(true);
            }
        }

        return result;
    }

    void broadcast_alarm(const areg::String & worker) final
    {
        std::cout << "operator: " << worker << " raised an alarm" << std::endl;
        request_summary();
    }

    void response_summary(unsigned int alive, unsigned int total) final
    {
        std::cout << "operator: " << alive << " of " << total << " alive" << std::endl;
        areg::Application::signal_quit();
    }
};

constexpr char const _modelName[]{ "MonitorModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("WorkerThread")
        BEGIN_REGISTER_COMPONENT("Worker1", Worker)
            REGISTER_IMPLEMENT_SERVICE(WorkerService::ServiceName, WorkerService::InterfaceVersion)
        END_REGISTER_COMPONENT("Worker1")
        BEGIN_REGISTER_COMPONENT("Worker2", Worker)
            REGISTER_IMPLEMENT_SERVICE(WorkerService::ServiceName, WorkerService::InterfaceVersion)
        END_REGISTER_COMPONENT("Worker2")
        BEGIN_REGISTER_COMPONENT("Worker3", Worker)
            REGISTER_IMPLEMENT_SERVICE(WorkerService::ServiceName, WorkerService::InterfaceVersion)
        END_REGISTER_COMPONENT("Worker3")
    END_REGISTER_THREAD("WorkerThread")

    BEGIN_REGISTER_THREAD("MonitorThread")
        BEGIN_REGISTER_COMPONENT("Monitor", Monitor)
            REGISTER_IMPLEMENT_SERVICE(MonitorService::ServiceName, MonitorService::InterfaceVersion)
            REGISTER_DEPENDENCY("Worker1")
            REGISTER_DEPENDENCY("Worker2")
            REGISTER_DEPENDENCY("Worker3")
        END_REGISTER_COMPONENT("Monitor")
    END_REGISTER_THREAD("MonitorThread")

    BEGIN_REGISTER_THREAD("OperatorThread")
        BEGIN_REGISTER_COMPONENT("Operator", Operator)
            REGISTER_DEPENDENCY("Monitor")
        END_REGISTER_COMPONENT("Operator")
    END_REGISTER_THREAD("OperatorThread")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
