//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : Example 32 provider. Runs a bulk stream and answers small ping
//               requests on the same connection, so that the cost a bulk stream
//               puts on small-message latency can be measured.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include <iostream>
#include <string>

#include "common/MixedDefs.hpp"
#include "pubservice/src/ProviderComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "32_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "MixedProviderModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MixedProviderThread")
        BEGIN_REGISTER_COMPONENT(mixed::ServiceRoleName, ProviderComponent)
            REGISTER_IMPLEMENT_SERVICE(MixedTraffic::ServiceName, MixedTraffic::InterfaceVersion)
        END_REGISTER_COMPONENT(mixed::ServiceRoleName)
    END_REGISTER_THREAD("MixedProviderThread")

END_MODEL(_modelName)

int main(int argc, char* argv[])
{
    if (ProviderComponent::options().parse(argc, argv) == false)
    {
        mixed::print_provider_help();
        return 0;
    }

    LOGGING_CONFIGURE_AND_START(nullptr, false);
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
