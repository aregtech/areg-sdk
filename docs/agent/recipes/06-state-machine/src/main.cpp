/**
 * \file    main.cpp
 * \brief   A service whose logic is a state machine, and its consumer.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/GateServiceProviderBase.hpp"
#include "src/services/GateServiceConsumerBase.hpp"
#include "src/services/GateActionHandler.hpp"
#include "src/services/GateFSM.hpp"

//! The provider owns the machine and implements the actions the machine calls.
class GateProvider final    : public    areg::Component
                            , protected GateServiceProviderBase
                            , protected GateActionHandler
{
public:
    GateProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , GateServiceProviderBase(static_cast<areg::Component &>(self()))
        , GateActionHandler()
        , mFsm(static_cast<GateActionHandler &>(self()))
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        mFsm.init_fsm(&comThread);
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        mFsm.release_fsm();
        areg::Component::shutdown_component(comThread);
    }

    //! A request handler turns the call into a stimulus. It decides nothing itself.
    void request_open_gate() final
    {
        mFsm.open();
    }

    void action_on_open() final
    {
        std::cout << "provider: gate open" << std::endl;
        broadcast_gate_changed(true);
    }

    void action_on_close() final
    {
        std::cout << "provider: gate closed" << std::endl;
        broadcast_gate_changed(false);
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
                notify_on_broadcast_gate_changed(true);
                request_open_gate();
            }
        }

        return result;
    }

    void broadcast_gate_changed(bool isOpen) final
    {
        std::cout << "consumer: gate is " << (isOpen ? "open" : "closed") << std::endl;
        if (isOpen == false)
        {
            areg::Application::signal_quit();
        }
    }
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
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
