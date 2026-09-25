/************************************************************************
 * \file        machine/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Coffee machine driven by a generated state machine: the machine process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ComponentLoader.hpp"

#include <iostream>
#include <string>

#include "machine/src/CoffeeMachineServiceProvider.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "34_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "ProviderModel" };

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD("ProviderThread")
        BEGIN_REGISTER_COMPONENT("CoffeeMachineServiceProvider", CoffeeMachineServiceProvider)
            REGISTER_IMPLEMENT_SERVICE(CoffeeMachineService::ServiceName, CoffeeMachineService::InterfaceVersion)
        END_REGISTER_COMPONENT("CoffeeMachineServiceProvider")
    END_REGISTER_THREAD("ProviderThread")
END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);

    // Quits on "-q" or "--quit" from the console. Any other input is
    // ignored, and end of input keeps the service running.
    bool quitRequested{ false };
    std::string line;
    while (std::getline(std::cin, line))
    {
        if ((line == "-q") || (line == "--quit"))
        {
            quitRequested = true;
            break;
        }
    }
    if (quitRequested == false)
    {
        areg::Application::wait_quit(areg::WAIT_INFINITE);
    }

    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return 0;
}
