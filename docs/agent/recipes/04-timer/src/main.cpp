/**
 * \file    main.cpp
 * \brief   A provider that broadcasts on every period of a timer.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "src/services/TickServiceProviderBase.hpp"
#include "src/services/TickServiceConsumerBase.hpp"

class TickProvider final    : public    areg::Component
                            , protected TickServiceProviderBase
                            , private   areg::TimerConsumer
{
public:
    TickProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , TickServiceProviderBase(static_cast<areg::Component &>(self()))
        , areg::TimerConsumer()
        , mTimer(static_cast<areg::TimerConsumer &>(self()), "TickTimer")
        , mCount(0)
    { }

protected:
    //! The timer runs on the component's own dispatcher, so it must be started
    //! after that thread exists, not in the constructor.
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        mTimer.start_timer(200, static_cast<areg::DispatcherThread &>(comThread)
                          , areg::TimerBase::CONTINUOUSLY);
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        mTimer.stop_timer();
        areg::Component::shutdown_component(comThread);
    }

    void process_timer(areg::Timer & /*timer*/) final
    {
        broadcast_tick(++mCount);
    }

private:
    inline TickProvider & self()
    {   return (*this); }

    areg::Timer mTimer; //!< Fires every 200 milliseconds.
    uint32_t    mCount; //!< Number of ticks sent so far.
};

class TickConsumer final    : public    areg::Component
                            , protected TickServiceConsumerBase
{
public:
    TickConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , TickServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (TickServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_broadcast_tick(true);
            }
        }

        return result;
    }

    void broadcast_tick(uint32_t count) final
    {
        std::cout << "consumer: tick " << count << std::endl;
        if (count >= 3)
        {
            areg::Application::signal_quit();
        }
    }
};

constexpr char const _modelName[]{ "TickModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("TickProvider", TickProvider)
            REGISTER_IMPLEMENT_SERVICE(TickService::ServiceName, TickService::InterfaceVersion)
        END_REGISTER_COMPONENT("TickProvider")
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT("TickConsumer", TickConsumer)
            REGISTER_DEPENDENCY("TickProvider")
        END_REGISTER_COMPONENT("TickConsumer")
    END_REGISTER_THREAD("ConsumerThread")

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
