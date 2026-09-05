/**
 * \file    main.cpp
 * \brief   The coffee machine process: provides the CoffeeMachineService.
 **/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "examples/34_coffeemachine/services/CoffeeMachineService.hpp"
#include "machine/src/MachineComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "34_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "CoffeeMachineModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MachineThread")
        BEGIN_REGISTER_COMPONENT("CoffeeMachine", MachineComponent)
            REGISTER_IMPLEMENT_SERVICE(CoffeeMachineService::ServiceName, CoffeeMachineService::InterfaceVersion)
        END_REGISTER_COMPONENT("CoffeeMachine")
    END_REGISTER_THREAD("MachineThread")

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
