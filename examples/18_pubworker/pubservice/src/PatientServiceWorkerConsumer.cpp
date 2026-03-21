/************************************************************************
 * \file        pubservice/src/PatientServiceWorkerConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a patient service worker thread to read out data from console.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubservice/src/PatientServiceWorkerConsumer.hpp"

#include "areg/base/UtilityDefs.hpp"
#include "common/WorkerDefs.hpp"
#include "examples/18_pubworker/services/PatientInformationProviderBase.hpp"
#include "areg/appbase/Application.hpp"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
    #define MACRO_SCANFX(fmt, data)         scanf_s(fmt, data)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
    #define MACRO_SCANFX(fmt, data)         scanf(fmt, data)
#endif  // _WINDOWS


PatientServiceWorkerConsumer::PatientServiceWorkerConsumer(const char * consumerName, PatientInformationProviderBase & infoPatient)
    : areg::WorkerThreadConsumer( areg::create_component_item_name( worker::ServiceNamePatientInfo, consumerName) )

    , mPatienInfo       ( infoPatient )
{
}

void PatientServiceWorkerConsumer::register_event_consumers(areg::WorkerThread & /* workThread */, areg::ComponentThread & /* masterThread */ )
{
    bool quitApp = false;

    do 
    {
        /******************************************
         * Read Patient information from console
         ******************************************/

        // Patient information
        char firstName[128] = {0};
        char lastName[128]  = {0};
        float weight        = 0.0f;
        uint32_t age        = 0u;

        printf("\n===============================");
        printf("\nEnter Patient information .....\n");

        printf("\tFirst Name ...........: ");
        if (MACRO_SCANF( "%127s", firstName, 128 ) != 1)
        {
            printf("\nERROR: Unexpected choice, quit application ...\n");
            break;
        }

        printf( "\tLast Name ............: " );
        if ( MACRO_SCANF( "%127s", lastName, 128 ) != 1 )
        {
            printf( "\nERROR: Unexpected choice, quit application ...\n" );
            break;
        }

        printf( "\tPatient weight (kg) ..: " );
        if ( MACRO_SCANFX( "%f", &weight ) != 1 )
        {
            printf( "\nERROR: Unexpected choice, quit application ...\n" );
            break;
        }

        printf("\tPatient age ..........: ");
        if ( MACRO_SCANFX( "%u", &age ) != 1 )
        {
            printf( "\nERROR: Unexpected choice, quit application ...\n" );
            break;
        }

        printf("===============================\n");

        PatientInformation::PatientInfo infoPatient;
        infoPatient.firstName   = firstName;
        infoPatient.lastName    = lastName;
        infoPatient.weight      = weight;
        infoPatient.age         = age;

        mPatienInfo.set_patient(infoPatient);

        /******************************************
         * Do you want to continue or exit application?
         ******************************************/
        char cmd[8] = {0};
        printf("Do you want to continue? (y/n): ");
        if ( MACRO_SCANF( "%1s", cmd, 2 ) != 1 )
        {
            printf( "\nERROR: Unexpected choice, quit application ...\n" );
            break;
        }

        quitApp = (*cmd == 'n') || (*cmd == 'N');

    } while (!quitApp);

    mPatienInfo.invalidate_patient();
    areg::Application::signal_quit();
}

void PatientServiceWorkerConsumer::unregister_event_consumers(areg::WorkerThread & /* workThread */)
{
}
