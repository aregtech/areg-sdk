/************************************************************************
 * \file        user/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine: the simulated user process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "user/src/UserComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "34_generated")
#endif // _MSC_VER

constexpr char const _modelName[]  { "UserModel" };
constexpr char const _roleName[]   { "User" };
constexpr char const _machineRole[]{ "CoffeeMachine" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("UserThread")
        BEGIN_REGISTER_COMPONENT(_roleName, UserComponent)
            REGISTER_DEPENDENCY(_machineRole)
        END_REGISTER_COMPONENT(_roleName)
    END_REGISTER_THREAD("UserThread")

END_MODEL(_modelName)

DEF_LOG_SCOPE(examples_34_coffeemachine_user_main, main);

int main(void)
{
    std::cout << "Simulated user: connects to the coffee machine and runs the scenario ..." << std::endl;

    areg::Application::setup();
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do
    {
        LOG_SCOPE(examples_34_coffeemachine_user_main, main);
        LOG_DBG("Loading model [ %s ]", _modelName);

        areg::Application::load_model(_modelName);
        areg::Application::wait_quit(areg::WAIT_INFINITE);
        areg::Application::unload_model(_modelName);
        areg::Application::release();

    } while (false);

    const int exitCode{ UserComponent::get_exit_code() };
    std::cout << "Exit user, exit code " << exitCode << "." << std::endl;
    return exitCode;
}
