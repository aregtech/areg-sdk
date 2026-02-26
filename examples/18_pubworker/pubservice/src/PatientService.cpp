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
    : Component             ( entry, owner )
    , PatientInformationStub( static_cast<Component &>(self()) )

    , mWorkerConsumer       ( PatientService::PatienServiceConsumerName.data(), static_cast<PatientInformationStub &>(self()) )
{
}

inline PatientService & PatientService::self()
{
    return (*this);
}

areg::WorkerThreadConsumer * PatientService::workerThreadConsumer(const areg::String & consumerName, const areg::String & workerThreadName)
{
    if ( mWorkerConsumer.getConsumerName() == consumerName)
    {
        return &mWorkerConsumer;
    }
    else
    {
        return Component::workerThreadConsumer(consumerName, workerThreadName);
    }
}
