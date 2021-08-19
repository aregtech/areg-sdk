/************************************************************************
 * \file        ipchwmgr/src/HardwareManager.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       Collection of AREG SDK examples.
 *              This is a hardware manager component runs as patient and temperature
 *              client to collect data and send to worker thread to update the hardware.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "ipchwmgr/src/HardwareManager.hpp"

#include "areg/appbase/Application.hpp"

const char * const  HardwareManager::HwWorkerThreadConsumer = "HwUpdateConsumer";

const char * const  HardwareManager::HwWorkerThreadName     = "HwUpdateThread";


Component * HardwareManager::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW HardwareManager(entry, owner);
}

void HardwareManager::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

HardwareManager::HardwareManager(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component                     ( entry.mRoleName )
    , PatientInformationClientBase  ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )

    , mHwWorker ( entry.mWorkerThreads[0].mConsumerName )
{

}

HardwareManager::~HardwareManager(void)
{

}

HardwareManager & HardwareManager::self(void)
{
    return (*this);
}

IEWorkerThreadConsumer * HardwareManager::workerThreadConsumer(const char* consumerName, const char* workerThreadName)
{
    if ( NEString::compareFast<char, char>(mHwWorker.getConsumerName(), consumerName) == 0)
    {
        return &mHwWorker;
    }
    else
    {
        return Component::workerThreadConsumer(consumerName, workerThreadName);
    }
}

bool HardwareManager::serviceConnected(bool isConnected, ProxyBase & proxy)
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

void HardwareManager::onPatientUpdate(const NEPatientInformation::PatientInfo & Patient, NEService::eDataStateType state)
{
    if (state == NEService::DATA_OK)
    {
        PatientInfoEvent::sendEvent( PatientInfoEventData(Patient) );
    }
}
