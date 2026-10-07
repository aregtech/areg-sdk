/**
 * \file    main.cpp
 * \brief   A state machine that hosts another state machine twice.
 **/
#include <iostream>

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"

#include "src/services/LampActionHandler.hpp"
#include "src/services/BlinkActionHandler.hpp"
#include "src/services/LampFSM.hpp"

//! The actions of one hosted Blink. Each hosting state gets its own, so the two
//! instances of the same machine do different work.
class Flasher final : public BlinkActionHandler
{
public:
    explicit Flasher(const char * name)
        : BlinkActionHandler()
        , mName(name)
    { }

protected:
    void action_on_lit() final
    {
        std::cout << "blink: " << mName << " lit" << std::endl;
    }

    void action_on_dark() final
    {
        std::cout << "blink: " << mName << " dark" << std::endl;
    }

private:
    const char * const  mName;  //!< Which hosting state this instance serves.
};

//! Owns the hosting machine and hears it finish.
class LampHost final    : public    areg::Component
                        , protected LampActionHandler
                        , private   LampFSM::FinalObserver
{
public:
    LampHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , LampActionHandler()
        , LampFSM::FinalObserver()
        , mWarmup("warmup")
        , mSignal("signal")
        // One BlinkActionHandler per hosting state, in the order the states are
        // written in the document.
        , mFsm(static_cast<LampActionHandler &>(self()), mWarmup, mSignal)
    { }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        mFsm.set_final_observer(this);
        mFsm.init_fsm(&comThread);
        mFsm.switch_on();
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        mFsm.release_fsm();
        areg::Component::shutdown_component(comThread);
    }

    void action_on_idle() final
    {
        std::cout << "lamp: idle" << std::endl;
    }

    void on_fsm_final(LampFSM & /*machine*/, const char * const finalState) final
    {
        std::cout << "lamp: finished in " << finalState << std::endl;
        areg::Application::signal_quit();
    }

private:
    inline LampHost & self()
    {   return (*this); }

    Flasher     mWarmup;    //!< Actions of the instance LAMP_WARMUP hosts.
    Flasher     mSignal;    //!< Actions of the instance LAMP_SIGNAL hosts.
    LampFSM     mFsm;       //!< The hosting machine.
};

constexpr char const _modelName[]{ "LampModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("LampThread")
        BEGIN_REGISTER_COMPONENT("LampHost", LampHost)
        END_REGISTER_COMPONENT("LampHost")
    END_REGISTER_THREAD("LampThread")

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
