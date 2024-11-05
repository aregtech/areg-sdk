/************************************************************************
 * \file        pubclient/src/HardwareWorkerConsumer.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager worker thread to communication with hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/HardwareWorkerConsumer.hpp"


HardwareWorkerConsumer::HardwareWorkerConsumer(const char * consumerName)
    : IEWorkerThreadConsumer    ( consumerName )
    , IEPatientInfoEventConsumer( )
{
}

inline HardwareWorkerConsumer & HardwareWorkerConsumer::self(void)
{
    return (*this);
}

void HardwareWorkerConsumer::registerEventConsumers(WorkerThread & workThread, ComponentThread & /* masterThread */ )
{
    PatientInfoEvent::addListener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<DispatcherThread &>(workThread) );

    printf("Example Hardware is initialized .....\n\n");
}

void HardwareWorkerConsumer::unregisterEventConsumers(WorkerThread & workThread)
{
    PatientInfoEvent::removeListener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<DispatcherThread &>(workThread) );

    printf("Example Hardware is uninitialized .....\n\n");
}

void HardwareWorkerConsumer::processEvent(const PatientInfoEventData & data)
{
    const SharedBuffer & buf = data.getData();
    PatientInfoEventData::eUpdateCommands cmd = PatientInfoEventData::CMD_Undefined;
    buf >> cmd;

    switch (cmd)
    {
    case PatientInfoEventData::CMD_PatientInfo:
        updateInfoPatient(buf);
        break;

    case PatientInfoEventData::CMD_Undefined:
    default:
        break;
    }
}

void HardwareWorkerConsumer::updateInfoPatient(const SharedBuffer & data)
{
    NEPatientInformation::PatientInfo infoPatient;
    data >> infoPatient;

    printf("Update patient info ...\n");
    printf("\tFirst Name ......: %s\n", infoPatient.firstName.getString());
    printf("\tLast Name .......: %s\n", infoPatient.lastName.getString());
    printf("\tPatient weight ..: %.02f kg\n", static_cast<double>(infoPatient.weight));
    printf("\tPatient age .....: %d y.\n", infoPatient.age);
    printf("-----------------------------\n\n");
}
