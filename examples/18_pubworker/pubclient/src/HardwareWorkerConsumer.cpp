/************************************************************************
 * \file        pubclient/src/HardwareWorkerConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a hardware manager worker thread to communication with hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/HardwareWorkerConsumer.hpp"


HardwareWorkerConsumer::HardwareWorkerConsumer(const char * consumerName)
    : areg::WorkerThreadConsumer    ( consumerName )
    , IEPatientInfoEventConsumer( )
{
}

inline HardwareWorkerConsumer & HardwareWorkerConsumer::self()
{
    return (*this);
}

void HardwareWorkerConsumer::register_event_consumers(areg::WorkerThread & workThread, areg::ComponentThread & /* masterThread */ )
{
    PatientInfoEvent::add_listener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<areg::DispatcherThread &>(workThread) );

    printf("Example Hardware is initialized .....\n\n");
}

void HardwareWorkerConsumer::unregister_event_consumers(areg::WorkerThread & workThread)
{
    PatientInfoEvent::remove_listener( static_cast<IEPatientInfoEventConsumer &>(self()), static_cast<areg::DispatcherThread &>(workThread) );

    printf("Example Hardware is uninitialized .....\n\n");
}

void HardwareWorkerConsumer::process_event(const PatientInfoEventData & data)
{
    const areg::SharedBuffer & buf = data.data();
    PatientInfoEventData::UpdateCommands cmd = PatientInfoEventData::UpdateCommands::CMD_Undefined;
    buf >> cmd;

    switch (cmd)
    {
    case PatientInfoEventData::UpdateCommands::CMD_PatientInfo:
        update_info_patient(buf);
        break;

    case PatientInfoEventData::UpdateCommands::CMD_Undefined:
    default:
        break;
    }
}

void HardwareWorkerConsumer::update_info_patient(const areg::SharedBuffer & data)
{
    PatientInformation::PatientInfo infoPatient;
    data >> infoPatient;

    printf("Update patient info ...\n");
    printf("\tFirst Name ......: %s\n", infoPatient.firstName.as_string());
    printf("\tLast Name .......: %s\n", infoPatient.lastName.as_string());
    printf("\tPatient weight ..: %.02f kg\n", static_cast<double>(infoPatient.weight));
    printf("\tPatient age .....: %u y.\n", infoPatient.age);
    printf("-----------------------------\n\n");
}
