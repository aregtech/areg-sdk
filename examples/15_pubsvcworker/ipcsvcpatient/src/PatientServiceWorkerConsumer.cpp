/************************************************************************
 * \file        ipcsvcpatient/src/PatientServiceWorkerConsumer.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a patient service worker thread to read out data from console.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "ipcsvcpatient/src/PatientServiceWorkerConsumer.hpp"

#include "areg/base/NEUtilities.hpp"
#include "generated/src/NECommon.hpp"
#include "generated/src/PatientInformationStub.hpp"
#include "areg/appbase/Application.hpp"

PatientServiceWorkerConsumer::PatientServiceWorkerConsumer(const char * consumerName, PatientInformationStub & infoPatient)
    : IEWorkerThreadConsumer( NEUtilities::createComponentItemName( NECommon::ServiceNamePatientInfo, consumerName) )

    , mStubPatienInfo       ( infoPatient )
{
}

PatientServiceWorkerConsumer::~PatientServiceWorkerConsumer(void)
{
}

void PatientServiceWorkerConsumer::registerEventConsumers(WorkerThread & workThread, ComponentThread & masterThread)
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
        int age             = 0;

        printf("\n===============================");
        printf("\nEnter Patient information .....\n");

        printf("\tFirst Name ...........: ");
#ifdef _WINDOWS
        scanf_s("%127s", firstName, 128);
#else   // _POSIX
        scanf("%127s", firstName);
#endif 	// _WINDOWS

        printf("\tLast Name ............: ");
#ifdef _WINDOWS
        scanf_s("%127s", lastName, 128);
#else   // _POSIX
        scanf("%127s", lastName);
#endif 	// _WINDOWS

        printf("\tPatient weight (kg) ..: ");
#ifdef _WINDOWS
        scanf_s("%f", &weight);
#else   // _POSIX
        scanf("%f", &weight);
#endif // _WINDOWS

        printf("\tPatient age ..........: ");
#ifdef _WINDOWS
        scanf_s("%u", &age);
#else   // _POSIX
        scanf("%u", &age);
#endif // _WINDOWS

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
        char cmd[2] = {0};
        printf("Do you want to continue? (y/n): ");
#ifdef _WINDOWS
        scanf_s("%1s", cmd, 2);
#else   // _POSIX
        scanf("%1s", cmd);
#endif 	// _WINDOWS

        quitApp = (*cmd == 'n') || (*cmd == 'N');

    } while (!quitApp);

    mStubPatienInfo.invalidatePatient();
    Application::signalAppQuit();
}

void PatientServiceWorkerConsumer::unregisterEventConsumers(WorkerThread & workThread)
{
}
