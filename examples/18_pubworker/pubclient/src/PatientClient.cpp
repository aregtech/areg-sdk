/************************************************************************
 * \file        pubclient/src/PatientClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This is a hardware manager component runs as patient and temperature
 *              client to collect data and send to worker thread to update the hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/PatientClient.hpp"
#include "areg/appbase/Application.hpp"

PatientClient::PatientClient(const areg::ComponentEntry & entry, areg::ComponentThread & /* owner */)
    : Component                     ( entry.mRoleName )
    , PatientInformationClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mHwWorker ( entry.mWorkerThreads[0].mConsumerName )
{
}

PatientClient & PatientClient::self()
{
    return (*this);
}

areg::WorkerThreadConsumer * PatientClient::workerThreadConsumer(const areg::String & consumerName, const areg::String & workerThreadName)
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

bool PatientClient::serviceConnected( areg::ServiceConnectionState status, ProxyBase & proxy)
{
    bool result = PatientInformationClientBase::serviceConnected( status, proxy );
    if ( isConnected( ) )
    {
        notifyOnPatientUpdate( true );
    }
    else
    {
        notifyOnPatientUpdate( false );
        Application::signalAppQuit( );
    }

    return result;
}

void PatientClient::onPatientUpdate(const PatientInformation::PatientInfo & Patient, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        PatientInfoEvent::sendEvent( PatientInfoEventData(Patient) );
    }
}
