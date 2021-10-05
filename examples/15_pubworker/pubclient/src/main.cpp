//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : This project contains service client and the worker thread 
//               described in the model.The worker thread imitates the output
//               device.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/trace/GETrace.h"

#include "generated/src/NECommon.hpp"
#include "pubclient/src/PatientClient.hpp"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
    #pragma comment(lib, "15_generated.lib")
#endif // WINDOWS



constexpr char const _modelName[]  { "TheModel" };   // The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestPatientMonitoring" )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceNameHwManager, PatientClient )
            // register service dependencies
            REGISTER_DEPENDENCY( NECommon::ServiceNamePatientInfo )
            // register HW worker thread
            REGISTER_WORKER_THREAD( PatientClient::HwWorkerThreadName.data(), PatientClient::HwWorkerThreadConsumer.data() )
        // end of component description
        END_REGISTER_COMPONENT( NECommon::ServiceNameHwManager )
    // end of thread description
    END_REGISTER_THREAD( "TestPatientMonitoring" )

// end of model
END_MODEL(_modelName)

//////////////////////////////////////////////////////////////////////////
// main method.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The main method does not start the logging, enables service manager and timer.
 *          it loads and unloads the services, releases application.
 **/
int main()
{
    // Initialize application, disable logging, enables servicing and the timer.
    Application::initApplication(false, true, true, true, nullptr, nullptr );


    // load model to initialize components
    Application::loadModel(_modelName);

    // wait application quit.
    Application::waitAppQuit();

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

	return 0;
}
