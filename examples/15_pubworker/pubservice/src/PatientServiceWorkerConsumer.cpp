/************************************************************************
 * \file        pubservice/src/PatientServiceWorkerConsumer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a patient service worker thread to read out data from console.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "pubservice/src/PatientServiceWorkerConsumer.hpp"

#include "areg/base/NEUtilities.hpp"
#include "common/NECommon.hpp"
#include "examples/15_pubworker/services/PatientInformationStub.hpp"
#include "areg/appbase/Application.hpp"

#ifdef _WINDOWS
    #define MACRO_SCANF(fmt, data, len)     scanf_s(fmt, data, len)
    #define MACRO_SCANFX(fmt, data)         scanf_s(fmt, data)
#else   // _POSIX
    #define MACRO_SCANF(fmt, data, len)     scanf(fmt, data)
    #define MACRO_SCANFX(fmt, data)         scanf(fmt, data)
#endif  // _WINDOWS


PatientServiceWorkerConsumer::PatientServiceWorkerConsumer(const char * consumerName, PatientInformationStub & infoPatient)
    : IEWorkerThreadConsumer( NEUtilities::createComponentItemName( NECommon::ServiceNamePatientInfo, consumerName) )

    , mStubPatienInfo       ( infoPatient )
{
}

void PatientServiceWorkerConsumer::registerEventConsumers(WorkerThread & /* workThread */, ComponentThread & /* masterThread */ )
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

        NEPatientInformation::PatientInfo infoPatient;
        infoPatient.firstName   = firstName;
        infoPatient.lastName    = lastName;
        infoPatient.weight      = weight;
        infoPatient.age         = age;

        mStubPatienInfo.setPatient(infoPatient);

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

    mStubPatienInfo.invalidatePatient();
    Application::signalAppQuit();
}

void PatientServiceWorkerConsumer::unregisterEventConsumers(WorkerThread & /* workThread */)
{
}
