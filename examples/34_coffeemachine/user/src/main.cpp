/**
 * \file    main.cpp
 * \brief   The simulated user process: drives the coffee machine through the
 *          acceptance scenario and exits 0 only if every expectation held.
 **/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "user/src/UserComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "34_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "CoffeeMachineUserModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("UserThread")
        BEGIN_REGISTER_COMPONENT("CoffeeMachineUser", UserComponent)
            REGISTER_DEPENDENCY("CoffeeMachine")
        END_REGISTER_COMPONENT("CoffeeMachineUser")
    END_REGISTER_THREAD("UserThread")

END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return g_scenarioExitCode;
}
