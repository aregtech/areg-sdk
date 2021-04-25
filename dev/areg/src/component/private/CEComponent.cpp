/************************************************************************
 * \file        areg/src/component/private/CEComponent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component class implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEComponent.hpp"

#include "areg/src/component/CEWorkerThread.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEStubBase.hpp"
#include "areg/src/component/private/CEServiceManager.hpp"
#include "areg/src/component/CEComponentLoader.hpp"

//////////////////////////////////////////////////////////////////////////
// CEComponent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CEComponent, CERuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
CEComponent::MapComponentResource       CEComponent::_mapComponentResource;

//////////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////////
CEComponent* CEComponent::LoadComponent(const NERegistry::CEComponentEntry &entry, CEComponentThread & componentThread )
{
    ASSERT(entry.mFuncCreate != NULL);

    CEComponent* component = entry.mFuncCreate(entry, componentThread);
    if (component != NULL)
    {
        const NERegistry::CEWorkerThreadList& wThreads = entry.GetWorkerThreads();
        for (int i = 0; i < wThreads.GetSize(); ++ i)
        {
            const NERegistry::CEWorkerThreadEntry& wtEntry = wThreads[i];
            IEWorkerThreadConsumer* consumer = static_cast<CEComponent *>(component)->GetWorkerThreadConsumer(wtEntry.mConsumerName.GetBuffer(), wtEntry.mThreadName.GetBuffer());
            if (consumer != NULL)
                component->CreateWorkerThread(wtEntry.mThreadName.GetBuffer(), *consumer, componentThread);
        }
    }
    return component;
}

void CEComponent::UnloadComponent( CEComponent& comItem, const NERegistry::CEComponentEntry& entry )
{
    ASSERT(entry.mFuncDelete != NULL);

    const NERegistry::CEWorkerThreadList& wThreads = entry.GetWorkerThreads();
    for (int i = 0; i < wThreads.GetSize(); ++ i)
        comItem.DeleteWorkerThread(wThreads[i].mThreadName.GetBuffer());

    entry.mFuncDelete(comItem, entry);
}

CEComponent* CEComponent::FindComponentByName( const char* roleName )
{
    ASSERT(NEString::isEmpty<char>(roleName) == false);
    return _mapComponentResource.FindResourceObject(roleName);
}

CEComponent* CEComponent::FindComponentByAddress( const CEComponentAddress& comAddress )
{
    CEComponent* result = CEComponent::FindComponentByName(comAddress.GetComponentName().GetBuffer());
    return (result != NULL && result->GetComponentAddress() == comAddress ? result : NULL);
}

bool CEComponent::IsComponentExist( const char* roleName )
{
    ASSERT(NEString::isEmpty<char>(roleName) == false);
    return _mapComponentResource.ResourceKeyExist(roleName);
}

CEComponentThread& CEComponent::_getCurrentComponentThread( void )
{
    CEComponentThread* result = RUNTIME_CAST(&(CEDispatcherThread::GetCurrentDispatcherThread()), CEComponentThread);
    ASSERT(result != NULL);
    return *result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor 
//////////////////////////////////////////////////////////////////////////
CEComponent::CEComponent( CEComponentThread & masterThread, const char* roleName)
    : CERuntimeObject           ( )

    , mComponentInfo    (masterThread, roleName)
    , mServerList       ( )
{
    _mapComponentResource.RegisterResourceObject(mComponentInfo.GetName(), this);
}

CEComponent::CEComponent( const char* roleName )
    : CERuntimeObject           ( )

    , mComponentInfo    (_getCurrentComponentThread(), roleName)
    , mServerList       ( )
{
    _mapComponentResource.RegisterResourceObject(mComponentInfo.GetName(), this);
}


CEComponent::~CEComponent( void )
{
    _mapComponentResource.UnregisterResourceObject(mComponentInfo.GetName());
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
CEWorkerThread* CEComponent::CreateWorkerThread( const char* threadName, IEWorkerThreadConsumer& consumer, CEComponentThread & /* masterThread */ )
{
    CEWorkerThread* workThread = mComponentInfo.FindWorkerThread(threadName);
    if (workThread == NULL)
    {
        OUTPUT_DBG("Going to Create WorkerThread object [ %s ]", threadName);
        workThread = DEBUG_NEW CEWorkerThread(threadName, self(), consumer);
        if (workThread != NULL)
        {
            if (workThread->CreateThread(CEThread::WAIT_INFINITE))
            {
                OUTPUT_DBG("Registering WorkerThread [ %s ]", threadName);
                mComponentInfo.RegisterWorkerThread(*workThread);
            }
            else
            {
                delete workThread;
                workThread = NULL;
            }
        }
    }

    return workThread;
}

void CEComponent::DeleteWorkerThread( const char* threadName )
{
    OUTPUT_DBG("Going to Delete WorkerThread object [ %s ]", threadName);
    CEWorkerThread* workThread = mComponentInfo.FindWorkerThread(threadName);
    if (workThread != NULL)
    {
        OUTPUT_DBG("Unregistering and deleting WorkerThread [ %s ]", threadName);
        workThread->DestroyThread(CEThread::WAIT_INFINITE);
        mComponentInfo.UnregisterWorkerThread(*workThread);
        delete workThread;
    }
}

void CEComponent::StartupComponent( CEComponentThread& /* comThread */ )
{
    LISTPOS pos = NULL;
    
    for (pos = mServerList.GetHeadPosition(); pos != NULL; pos = mServerList.GetNextPosition(pos))
    {
        CEStubBase * stub = mServerList[pos];
        ASSERT( stub != NULL );
        stub->StartupServiceInterface(self());
        CEServiceManager::RequestRegisterServer(stub->GetAddress());
    }
}

void CEComponent::ShutdownComponent( CEComponentThread& /* comThread */ )
{
    LISTPOS pos = NULL;

    for (pos = mServerList.GetHeadPosition(); pos != NULL; pos = mServerList.GetNextPosition(pos))
    {
        CEStubBase * stub = mServerList[pos];
        ASSERT(stub != NULL);
        OUTPUT_INFO("Shutting down Service Interface [ %s ] of component [ %s ]", stub->GetAddress().GetServiceName().GetBuffer(), GetRoleName().GetBuffer());
        stub->ShutdownServiceIntrface(self());
        CEServiceManager::RequestUnregisterServer(stub->GetAddress());
    }
    
    CEThreadAddress addrThread;
    CEWorkerThread * workerThread = mComponentInfo.GetFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        OUTPUT_INFO("Sutting down worker thread [ %s ] of component [ %s ]", workerThread->GetThreadName().GetBuffer(), GetRoleName().GetBuffer());
        workerThread->ShutdownThread();
        workerThread = mComponentInfo.GetNextWorkerThread(addrThread);
    }
}

void CEComponent::NotifyComponentShutdown( CEComponentThread& /*comThread */ )
{
    CEThreadAddress addrThread;
    CEWorkerThread * workerThread = mComponentInfo.GetFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        OUTPUT_INFO("Sutting down worker thread [ %s ] of component [ %s ]", workerThread->GetThreadName().GetBuffer(), GetRoleName().GetBuffer());
        workerThread->ShutdownThread();
        workerThread = mComponentInfo.GetNextWorkerThread(addrThread);
    }
}

void CEComponent::RegisterServerItem( CEStubBase& server )
{
    mServerList.AddTail(&server);
}

CEStubBase* CEComponent::FindServerByName( const char* serviceName )
{
    CEStubBase* result = NULL;
    LISTPOS pos = mServerList.GetHeadPosition();
    for ( ; pos != NULL; pos = mServerList.GetNextPosition(pos))
    {
        CEStubBase* stub = mServerList.GetAt(pos);
        ASSERT(stub != NULL);
        if (stub->GetAddress().GetServiceName() == serviceName)
        {
            result = stub;
            break;
        }
    }
    return result;
}

void CEComponent::WaitComponentCompletion( unsigned int waitTimeout )
{
    CEThreadAddress addrThread;
    CEWorkerThread * workerThread = mComponentInfo.GetFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        workerThread->CompletionWait(waitTimeout);
        workerThread = mComponentInfo.GetNextWorkerThread(addrThread);
    }
}

IEWorkerThreadConsumer* CEComponent::GetWorkerThreadConsumer( const char* /* consumerName       */
                                                            , const char* /* workerThreadName   */)
{
    return NULL;
}
