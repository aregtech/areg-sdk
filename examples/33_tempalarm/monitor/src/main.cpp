/************************************************************************
 * \file        monitor/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the monitor process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/areg_log.h"

#include "monitor/src/MonitorComponent.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

constexpr char const _modelName[]{ "MonitorModel" };
constexpr char const _roleName[] { "Monitor" };

BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD("MonitorThread")
        BEGIN_REGISTER_COMPONENT(_roleName, MonitorComponent)
            REGISTER_IMPLEMENT_SERVICE(TempAlarm::ServiceName, TempAlarm::InterfaceVersion)
        END_REGISTER_COMPONENT(_roleName)
    END_REGISTER_THREAD("MonitorThread")

END_MODEL(_modelName)

DEF_LOG_SCOPE(examples_33_tempalarm_monitor_main, main);

int main(void)
{
    std::cout << "Temperature monitor: publishes a reading every 200 ms and raises the alarm ..." << std::endl;

    areg::Application::setup();
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do
    {
        LOG_SCOPE(examples_33_tempalarm_monitor_main, main);
        LOG_DBG("Loading model [ %s ]", _modelName);

        areg::Application::load_model(_modelName);
        areg::Application::wait_quit(areg::WAIT_INFINITE);
        areg::Application::unload_model(_modelName);
        areg::Application::release();

    } while (false);

    std::cout << "Exit monitor." << std::endl;
    return 0;
}
