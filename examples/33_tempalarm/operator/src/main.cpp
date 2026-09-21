/************************************************************************
 * \file        operator/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the simulated operator process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "operator/src/OperatorComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

constexpr char const _modelName[] { "OperatorModel" };
constexpr char const _roleName[]  { "Operator" };
constexpr char const _monitorRole[]{ "Monitor" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("OperatorThread")
        BEGIN_REGISTER_COMPONENT(_roleName, OperatorComponent)
            REGISTER_DEPENDENCY(_monitorRole)
        END_REGISTER_COMPONENT(_roleName)
    END_REGISTER_THREAD("OperatorThread")

END_MODEL(_modelName)

DEF_LOG_SCOPE(examples_33_tempalarm_operator_main, main);

int main(void)
{
    std::cout << "Simulated operator: connects to the monitor and runs the alarm scenario ..." << std::endl;

    areg::Application::setup();
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do
    {
        LOG_SCOPE(examples_33_tempalarm_operator_main, main);
        LOG_DBG("Loading model [ %s ]", _modelName);

        areg::Application::load_model(_modelName);
        areg::Application::wait_quit(areg::WAIT_INFINITE);
        areg::Application::unload_model(_modelName);
        areg::Application::release();

    } while (false);

    const int exitCode{ OperatorComponent::get_exit_code() };
    std::cout << "Exit operator, exit code " << exitCode << "." << std::endl;
    return exitCode;
}
