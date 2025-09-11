/************************************************************************
 * \file        pubservice/src/PatientService.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a patient info service.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/PatientService.hpp"
#include "areg/component/ComponentThread.hpp"

PatientService::PatientService( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component             ( entry, owner )
    , PatientInformationStub( static_cast<Component &>(self()) )

    , mWorkerConsumer       ( PatientService::PatienServiceConsumerName.data(), static_cast<PatientInformationStub &>(self()) )
{
}

inline PatientService & PatientService::self( void )
{
    return (*this);
}

IEWorkerThreadConsumer * PatientService::workerThreadConsumer(const String & consumerName, const String & workerThreadName)
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
