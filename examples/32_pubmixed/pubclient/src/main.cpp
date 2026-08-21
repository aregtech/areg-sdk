//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : Example 32 consumer. Measures the round-trip time of small
//               requests while a bulk stream runs on the same connection.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "common/MixedDefs.hpp"
#include "pubclient/src/ConsumerComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "32_generated")
#endif // _MSC_VER

extern mixed::ConsumerOptions& consumer_options();

constexpr char const _modelName[]{ "MixedConsumerModel" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MixedConsumerThread")
        BEGIN_REGISTER_COMPONENT("MixedTrafficConsumer", ConsumerComponent)
            REGISTER_DEPENDENCY(mixed::ServiceRoleName)
        END_REGISTER_COMPONENT("MixedTrafficConsumer")
    END_REGISTER_THREAD("MixedConsumerThread")

END_MODEL(_modelName)

int main(int argc, char* argv[])
{
    if (consumer_options().parse(argc, argv) == false)
    {
        mixed::print_consumer_help();
        return 0;
    }

    LOGGING_CONFIGURE_AND_START(nullptr, false);
    areg::Application::setup();

    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();

    return 0;
}
