/************************************************************************
 * \file        operator/src/main.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \brief       Temperature monitor with a threshold alarm: the simulated operator process.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "operator/src/TempMonitorServiceConsumer.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

constexpr char const _exitCode[]{ "exitCode" };

//! Ends the application with this exit code, in storage that outlives
//! the components.
void quit_with(int code)
{
    areg::Primitive value{};
    value.valInt.mElement = code;
    areg::Application::store_element(_exitCode, value);
    areg::Application::signal_quit();
}

//! True once quit_with() has run, so a disconnect that follows is
//! this process shutting down rather than a lost provider.
bool is_quitting()
{
    return areg::Application::is_element_stored(_exitCode);
}

constexpr char const _modelName[]{ "ConsumerModel" };

// A unique role name lets several consumer processes run at the same time.
const areg::String _consumer(areg::generate_name("TempMonitorServiceConsumer"));

BEGIN_MODEL(_modelName)
    BEGIN_REGISTER_THREAD("ConsumerThread")
        BEGIN_REGISTER_COMPONENT(_consumer, TempMonitorServiceConsumer)
            REGISTER_DEPENDENCY("TempMonitorServiceProvider")
        END_REGISTER_COMPONENT(_consumer)
    END_REGISTER_THREAD("ConsumerThread")
END_MODEL(_modelName)

int main()
{
    areg::Application::setup();
    areg::Application::load_model(_modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(_modelName);
    areg::Application::release();
    return areg::Application::stored_element(_exitCode).valInt.mElement;
}
