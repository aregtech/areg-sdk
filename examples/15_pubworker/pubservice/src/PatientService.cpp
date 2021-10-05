/************************************************************************
 * \file        pubservice/src/PatientService.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a patient info service.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/PatientService.hpp"
#include "areg/component/ComponentThread.hpp"


Component * PatientService::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW PatientService(entry, owner);
}

void PatientService::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

PatientService::PatientService( const NERegistry::ComponentEntry & entry, ComponentThread & owner )
    : Component             ( owner, entry.mRoleName )
    , PatientInformationStub( static_cast<Component &>(self()) )

    , mWorkerConsumer       ( PatientService::PatienServiceConsumerName.data(), static_cast<PatientInformationStub &>(self()) )
{
}

inline PatientService & PatientService::self( void )
{
    return (*this);
}

IEWorkerThreadConsumer * PatientService::workerThreadConsumer(const char* consumerName, const char* workerThreadName)
{
    if ( NEString::compareFast<char, char>(mWorkerConsumer.getConsumerName().getString(), consumerName) == 0 )
    {
        return &mWorkerConsumer;
    }
    else
    {
        return Component::workerThreadConsumer(consumerName, workerThreadName);
    }
}
