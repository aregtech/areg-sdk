//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project contains service client and the worker thread 
//               described in the model.The worker thread imitates the output
//               device.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"

#include "common/NECommon.hpp"
#include "pubclient/src/PatientClient.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "15_generated")
#endif // _MSC_VER



constexpr char const _modelName[]  { "TheModel" };   // The name of model

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( "TestPatientMonitoring", NECommon::WATCHDOG_IGNORE)
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceNameHwManager, PatientClient )
            // register service dependencies
            REGISTER_DEPENDENCY( NECommon::ServiceNamePatientInfo )
            // register HW worker thread
            REGISTER_WORKER_THREAD( PatientClient::HwWorkerThreadName.data(), PatientClient::HwWorkerThreadConsumer.data(), NECommon::WATCHDOG_IGNORE)
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
    // Initialize application, enable servicing, routing, timer and watchdog.
    Application::initApplication(false, true, true, true, true, nullptr );


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
