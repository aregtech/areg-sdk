/**
 * \file    main.cpp
 * \brief   A service whose logic is a state machine, and its consumer.
 **/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/GateServiceProviderBase.hpp"
#include "src/services/GateServiceConsumerBase.hpp"
#include "src/services/GateActionHandler.hpp"
#include "src/services/GateFSM.hpp"
#include "areg/logging/areg_log.h"

GLOBAL_LOG_SCOPE(gate_main, log);

//! The provider owns the machine, implements the actions and hears it finish.
class GateProvider final    : public    areg::Component
                            , protected GateServiceProviderBase
                            , protected GateActionHandler
                            , private   GateFSM::FinalObserver
{
public:
    GateProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , GateServiceProviderBase(static_cast<areg::Component &>(*this))
        , GateActionHandler()
        , GateFSM::FinalObserver()
        , mFsm(static_cast<GateActionHandler &>(self()))
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        mFsm.set_final_observer(this);
        mFsm.init_fsm(&comThread);
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        mFsm.release_fsm();
        areg::Component::shutdown_component(comThread);
    }

    //! A request handler turns the call into a stimulus. It decides nothing itself.
    void request_open_gate(uint32_t width) final
    {
        mFsm.open(width);
    }

    void request_hold_gate() final
    {
        mFsm.hold();
    }

    void request_resume_gate() final
    {
        mFsm.resume();
    }

    //! An action performs an effect. It asks the machine nothing.
    void action_on_opening() final
    {
        GLOBAL_INFO("provider: gate opening");
        broadcast_gate_stage("opening");
    }

    void action_on_open() final
    {
        GLOBAL_INFO("provider: gate open to %u", static_cast<unsigned int>(mFsm.width()));
        broadcast_gate_stage("open");
    }

    void action_on_held() final
    {
        GLOBAL_INFO("provider: gate held");
        broadcast_gate_stage("held");
    }

    void action_on_closed() final
    {
        GLOBAL_INFO("provider: gate closed");
        broadcast_gate_stage("closed");
    }

    void action_on_refused(uint32_t asked) final
    {
        GLOBAL_INFO("provider: gate refused %u", static_cast<unsigned int>(asked));
        broadcast_gate_stage("refused");
    }

    void action_on_stopped() final
    {
        GLOBAL_INFO("provider: gate stopped");
        broadcast_gate_stage("stopped");
    }

    void on_fsm_final(GateFSM & /*machine*/, const char * const finalState) final
    {
        GLOBAL_INFO("provider: machine finished in %s", finalState);
    }

private:
    inline GateProvider & self()
    {   return (*this); }

    GateFSM     mFsm;   //!< The state machine driving the gate.
};

class GateConsumer final    : public    areg::Component
                            , protected GateServiceConsumerBase
{
public:
    GateConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , GateServiceConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
        , mOpenings(0)
        , mCycles(0)
        , mHeld(false)
    { }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (GateServiceConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_broadcast_gate_stage(true);
                request_open_gate(60);
            }
        }

        return result;
    }

    void broadcast_gate_stage(const areg::String & stage) final
    {
        GLOBAL_INFO("consumer: gate %s", stage.as_string());
        if (stage == "opening")
        {
            ++ mOpenings;
        }
        else if ((stage == "open") && (mHeld == false))
        {
            mHeld = true;
            request_hold_gate();
        }
        else if (stage == "held")
        {
            request_resume_gate();
        }
        else if (stage == "closed")
        {
            ++ mCycles;
            if (mCycles == 1)
            {
                // One opening only: resume named the history marker, so it re-entered
                // the stage the hold interrupted instead of starting the cycle again.
                GLOBAL_INFO("consumer: opening ran %u, so resume re-entered open",
                            static_cast<unsigned int>(mOpenings));
                request_open_gate(60);
            }
            else
            {
                // A second opening: this order named the composite and not its marker,
                // so it descended the Start chain and began a new cycle.
                GLOBAL_INFO("consumer: opening ran %u, so a fresh order started at the beginning",
                            static_cast<unsigned int>(mOpenings));
                request_open_gate(250);
            }
        }
        else if (stage == "stopped")
        {
            areg::Application::signal_quit();
        }
    }

private:
    uint32_t    mOpenings;  //!< How many times the opening stage was entered.
    uint32_t    mCycles;    //!< How many cycles have run to completion.
    bool        mHeld;      //!< True once the cycle has been interrupted.
};

constexpr char const _modelName[]{ "GateModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("GateProvider", GateProvider)
            REGISTER_IMPLEMENT_SERVICE(GateService::ServiceName, GateService::InterfaceVersion)
        END_REGISTER_COMPONENT("GateProvider")
    END_REGISTER_THREAD("ProviderThread")

    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT("GateConsumer", GateConsumer)
            REGISTER_DEPENDENCY("GateProvider")
        END_REGISTER_COMPONENT("GateConsumer")
    END_REGISTER_THREAD("ConsumerThread")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup(true, true, true, true, false, "./config/project.init");
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
