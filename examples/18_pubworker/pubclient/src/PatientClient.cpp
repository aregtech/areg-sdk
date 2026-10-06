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
#include "areg/component/WorkerThread.hpp"

PatientClient::PatientClient(const areg::ComponentEntry & entry, areg::ComponentThread & /* owner */)
    : areg::Component                     ( entry.mRoleName )
    , PatientInformationConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )

    , mHwWorker ( entry.mWorkerThreads[0].mConsumerName )
    , mHwThread ( nullptr )
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

void PatientClient::notify_thread_started(areg::WorkerThreadConsumer & consumer, areg::WorkerThread & workerThread)
{
    if (&consumer == &mHwWorker)
    {
        mHwThread = &workerThread;
        PatientInfoEvent::add_listener( static_cast<IEPatientInfoEventConsumer &>(mHwWorker), static_cast<areg::DispatcherThread &>(workerThread) );
    }
}

void PatientClient::shutdown_component(areg::ComponentThread & comThread)
{
    if (mHwThread != nullptr)
    {
        PatientInfoEvent::remove_listener( static_cast<IEPatientInfoEventConsumer &>(mHwWorker), static_cast<areg::DispatcherThread &>(*mHwThread) );
        mHwThread = nullptr;
    }

    areg::Component::shutdown_component(comThread);
}

bool PatientClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    bool result = PatientInformationConsumerBase::service_connected( status, proxy );
    if ( is_connected( ) )
    {
        notify_on_patient_update( true );
    }
    else
    {
        notify_on_patient_update( false );
        areg::Application::signal_quit( );
    }

    return result;
}

void PatientClient::on_patient_update(const PatientInformation::PatientInfo & Patient, areg::DataState state)
{
    if (state == areg::DataState::DataIsOK)
    {
        PatientInfoEvent::send_event( PatientInfoEventData(Patient) );
    }
}
