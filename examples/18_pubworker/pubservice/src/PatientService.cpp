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
    , PatientInformationProviderBase( static_cast<areg::Component &>(self()) )

    , mWorkerConsumer       ( PatientService::PatienServiceConsumerName.data(), static_cast<PatientInformationProviderBase &>(self()) )
{
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
