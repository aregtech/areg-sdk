//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project contains instance of public service and the 
//               worker thread to receive input data from the console and 
//               update the attribute of the service. The service and the 
//               worker threads are described in the static model to load on 
//               startup and unload on exit.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/logging/GELog.h"

#include "pubservice/src/PatientService.hpp"
#include "common/NECommon.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "18_generated")
#endif // _MSC_VER


constexpr char const _modelName[]      { "TheModel" };           // The name of model
constexpr char const _threadPatient[]  { "TestPatientThread" };  // The name of patient thread

// Describe mode, set model name
BEGIN_MODEL(_modelName)

    // define component thread
    BEGIN_REGISTER_THREAD( _threadPatient )
        // define component, set role name. This will trigger default 'create' and 'delete' methods of component
        BEGIN_REGISTER_COMPONENT( NECommon::ServiceNamePatientInfo, PatientService )
            // register Patient service
            REGISTER_IMPLEMENT_SERVICE( NEPatientInformation::ServiceName, NEPatientInformation::InterfaceVersion )
            // register HW worker thread
            REGISTER_WORKER_THREAD( PatientService::PatientServiceWorkerThread.data(), PatientService::PatienServiceConsumerName.data(), NECommon::WATCHDOG_IGNORE)
        // end of component description
        END_REGISTER_COMPONENT( NECommon::ServiceNamePatientInfo )
    // end of thread description
    END_REGISTER_THREAD( _threadPatient )

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

    Application::waitAppQuit(NECommon::WAIT_INFINITE);

    // stop and unload components
    Application::unloadModel(_modelName);

    // release and cleanup resources of application.
    Application::releaseApplication();

	return 0;
}
