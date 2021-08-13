/************************************************************************
 * \file        ipchwmgr/src/HardwareWorker.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager worker thread to communication with hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ipchwmgr/src/HardwareWorker.hpp"


HardwareWorker::HardwareWorker(const char * consumerName)
    : IEWorkerThreadConsumer    ( consumerName )
    , IEPatientInfoEventConsumer( )
{

}

HardwareWorker::~HardwareWorker( void )
{

}

inline HardwareWorker & HardwareWorker::self(void)
{
    return (*this);
}

void HardwareWorker::registerEventConsumers(WorkerThread & workThread, ComponentThread & masterThread)
{
    PatientInfoEvent::addListener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<DispatcherThread &>(workThread) );

    printf("Example Hardware is initialized .....\n\n");
}

void HardwareWorker::unregisterEventConsumers(WorkerThread & workThread)
{
    PatientInfoEvent::removeListener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<DispatcherThread &>(workThread) );

    printf("Example Hardware is uninitialized .....\n\n");
}

void HardwareWorker::processEvent(const PatientInfoEventData & data)
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

void HardwareWorker::updateInfoPatient(const SharedBuffer & data)
{
    NEPatientInformation::PatientInfo infoPatient;
    data >> infoPatient;

    printf("Update patient info ...\n");
    printf("\tFirst Name ......: %s\n", infoPatient.firstName.getString());
    printf("\tLast Name .......: %s\n", infoPatient.lastName.getString());
    printf("\tPatient weight ..: %.02f kg\n", static_cast<float>(infoPatient.weight));
    printf("\tPatient age .....: %d y.\n", infoPatient.age);
    printf("-----------------------------\n\n");
}
