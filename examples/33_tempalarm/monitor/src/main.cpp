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

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <thread>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

namespace
{
    void start_quit_listener(void)
    {
        // When there is no interactive stdin, the thread exits immediately and the
        // monitor keeps running until it is stopped by the framework or the process.
        std::thread([]()
        {
            std::string line;
            while (std::getline(std::cin, line))
            {
                const std::string::size_type first{ line.find_first_not_of(" \t\r\n") };
                if (first == std::string::npos)
                {
                    continue;
                }

                const std::string::size_type last{ line.find_last_not_of(" \t\r\n") };
                std::string command{ line.substr(first, last - first + 1u) };
                std::transform(command.begin(), command.end(), command.begin(),
                    [](unsigned char ch) -> char { return static_cast<char>(std::tolower(ch)); });

                if ((command == "-q") || (command == "--quit"))
                {
                    std::cout << "Quit requested, stopping monitor ..." << std::endl;
                    areg::Application::signal_quit();
                    break;
                }
            }
        }).detach();
    }
}

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
    std::cout << "Type -q or --quit to stop the monitor." << std::endl;

    areg::Application::setup();
    LOGGING_CONFIGURE_AND_START(nullptr, false);

    do
    {
        LOG_SCOPE(examples_33_tempalarm_monitor_main, main);
        LOG_DBG("Loading model [ %s ]", _modelName);

        areg::Application::load_model(_modelName);
        start_quit_listener();
        areg::Application::wait_quit(areg::WAIT_INFINITE);
        areg::Application::unload_model(_modelName);
        areg::Application::release();

    } while (false);

    std::cout << "Exit monitor." << std::endl;
    return 0;
}
