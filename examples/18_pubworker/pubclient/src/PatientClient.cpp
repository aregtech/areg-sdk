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
    : areg::Component                     ( entry.mRoleName )
    , PatientInformationClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )

    , mHwWorker ( entry.mWorkerThreads[0].mConsumerName )
{
}

PatientClient & PatientClient::self()
{
    return (*this);
}

areg::WorkerThreadConsumer * PatientClient::worker_thread_consumer(const areg::String & consumerName, const areg::String & workerThreadName)
{
    if ( mHwWorker.consumer_name() == consumerName)
    {
        return &mHwWorker;
    }
    else
    {
        return areg::Component::worker_thread_consumer(consumerName, workerThreadName);
    }
}

bool PatientClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result = PatientInformationClientBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        notifyOnPatientUpdate( true );
    }
    else
    {
        notifyOnPatientUpdate( false );
        areg::Application::signal_quit( );
    }

    return result;
}

void PatientClient::onPatientUpdate(const PatientInformation::PatientInfo & Patient, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        PatientInfoEvent::send_event( PatientInfoEventData(Patient) );
    }
}
