/************************************************************************
 * \file        pubservice/src/PatientService.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a patient info service.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/PatientService.hpp"
#include "areg/component/ComponentThread.hpp"

PatientService::PatientService( const areg::ComponentEntry & entry, areg::ComponentThread & owner )
    : areg::Component             ( entry, owner )
    , PatientInformationProviderBase( static_cast<areg::Component &>(*this) )

    , mWorkerConsumer       ( PatientService::PatienServiceConsumerName.data(), static_cast<IEPatientInfoEventConsumer &>(*this) )
{
}

void PatientService::process_event(const PatientInfoEventData & data)
{
    const areg::SharedBuffer & buf = data.data();
    PatientInfoEventData::UpdateCommands cmd = PatientInfoEventData::UpdateCommands::CMD_Undefined;
    buf >> cmd;
    if (cmd == PatientInfoEventData::UpdateCommands::CMD_PatientInfo)
    {
        PatientInformation::PatientInfo infoPatient;
        buf >> infoPatient;
        set_patient(infoPatient);
    }
    else
    {
        invalidate_patient();
    }
}

inline PatientService & PatientService::self()
{
    return (*this);
}

areg::WorkerThreadConsumer * PatientService::worker_thread_consumer(const areg::String & consumerName, const areg::String & workerThreadName)
{
    if ( mWorkerConsumer.consumer_name() == consumerName)
    {
        return &mWorkerConsumer;
    }
    else
    {
        return areg::Component::worker_thread_consumer(consumerName, workerThreadName);
    }
}
