/**
 * \file    multiprocess/clientproc/src/main.hpp
 * \brief   Runs service and the client in one thread.
 **/

#include "areg/base/GEGlobal.h"
#include "areg/base/NEUtilities.hpp"
#include "areg/base/String.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "common/src/ClientComponent.hpp"

// Use these options if compile for Windows with MSVC
// It links with areg library (dynamic or static) and generated static library
#ifdef MS_VISUAL_CPP
    #pragma comment(lib, "areg")
    #pragma comment(lib, "00_generated.lib")
#endif // MS_VISUAL_CPP

//!< The name of model
constexpr char const _modelName[]{ "ServiceModel" };
//!< Client component name. Let's generate the name for client service, we'll use it later.
const String _client(NEUtilities::generateName("ServiceClient"));

// Describe model, register the service consumer (client)
BEGIN_MODEL(_modelName)

    BEGIN_REGISTER_THREAD( "Thread1", NECommon::WATCHDOG_IGNORE)
        BEGIN_REGISTER_COMPONENT( _client, ClientComponent )
            REGISTER_DEPENDENCY( "ServiceComponent" ) /* reference to the service*/
        END_REGISTER_COMPONENT( _client )
    END_REGISTER_THREAD( "Thread1" )

// end of model description
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////////

int main( void )
{
    // Initialize application, enable logging, servicing, routing, timer and watchdog.
    // Use default settings.
    Application::initApplication( );

    // load model to initialize components
    Application::loadModel(_modelName);

    // wait until Application quit signal is set.
    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

    return 0;
}
