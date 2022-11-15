/************************************************************************
 * \file        pubclient/src/PatientClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager component runs as patient and temperature
 *              client to collect data and send to worker thread to update the hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/PatientClient.hpp"
#include "areg/appbase/Application.hpp"

Component * PatientClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW PatientClient(entry, owner);
}

void PatientClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

PatientClient::PatientClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry.mRoleName )
    , PatientInformationClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mHwWorker ( entry.mWorkerThreads[0].mConsumerName )
{
}

PatientClient & PatientClient::self(void)
{
    return (*this);
}

IEWorkerThreadConsumer * PatientClient::workerThreadConsumer(const String & consumerName, const String & workerThreadName)
{
    if ( mHwWorker.getConsumerName() == consumerName)
    {
        return &mHwWorker;
    }
    else
    {
        return Component::workerThreadConsumer(consumerName, workerThreadName);
    }
}

bool PatientClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    bool result = true;
    if (PatientInformationClientBase::serviceConnected(isConnected, proxy))
    {
        notifyOnPatientUpdate(isConnected);

        if (isConnected == false)
        {
            Application::signalAppQuit();
        }
    }
    else
    {
        result = false;
    }

    return result;
}

void PatientClient::onPatientUpdate(const NEPatientInformation::PatientInfo & Patient, NEService::eDataStateType state)
{
    if (state == NEService::eDataStateType::DataIsOK)
    {
        PatientInfoEvent::sendEvent( PatientInfoEventData(Patient) );
    }
}
