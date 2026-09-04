/************************************************************************
 * \file        machine/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine: the provider process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "machine/src/MachineComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "34_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "MachineModel" };
constexpr char const _roleName[] { "CoffeeMachine" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MachineThread")
        BEGIN_REGISTER_COMPONENT(_roleName, MachineComponent)
            REGISTER_IMPLEMENT_SERVICE(CoffeeMachineService::ServiceName, CoffeeMachineService::InterfaceVersion)
        END_REGISTER_COMPONENT(_roleName)
    END_REGISTER_THREAD("MachineThread")

END_MODEL(_modelName)

DEF_LOG_SCOPE(examples_34_coffeemachine_machine_main, main);

int main(void)
{
    std::cout << "Coffee machine: accepts coins and orders, and makes the drinks ..." << std::endl;

    areg::Application::setup();
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do
    {
        LOG_SCOPE(examples_34_coffeemachine_machine_main, main);
        LOG_DBG("Loading model [ %s ]", _modelName);

        areg::Application::load_model(_modelName);
        areg::Application::wait_quit(areg::WAIT_INFINITE);
        areg::Application::unload_model(_modelName);
        areg::Application::release();

    } while (false);

    std::cout << "Exit machine." << std::endl;
    return 0;
}
