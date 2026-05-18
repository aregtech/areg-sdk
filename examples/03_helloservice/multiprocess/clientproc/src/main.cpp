/**
 * \file    multiprocess/clientproc/src/main.hpp
 * \brief   Runs service and the client in one thread.
 **/

#include "areg/base/areg_global.h"
#include "areg/base/UtilityDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ClientComponent.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "03_generated")
#endif // _MSC_VER

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };
//!< Client component name. Let's generate the name for client service, we'll use it later.
const areg::String _client(areg::generate_name("ServiceClient"));

// Describe model, register the service consumer (client)
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1" )
        BEGIN_REGISTER_COMPONENT( _client, ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( _client )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main()
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    areg::Application::setup( );

    // load model to initialize components
    areg::Application::load_model(_modelName);

    // wait until Application quit signal is set.
    areg::Application::wait_quit(areg::WAIT_INFINITE);

    // stop and unload components
    areg::Application::unload_model(_modelName);

    // release and cleanup resources of application.
    areg::Application::release();

    return 0;
}
