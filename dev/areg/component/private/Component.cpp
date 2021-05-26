/************************************************************************
 * \file        areg/component/private/Component.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Component class implementation.
 *
 ************************************************************************/
#include "areg/component/Component.hpp"

#include "areg/component/WorkerThread.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/StubBase.hpp"
#include "areg/component/private/ServiceManager.hpp"
#include "areg/component/ComponentLoader.hpp"

//////////////////////////////////////////////////////////////////////////
// Component class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(Component, RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Static variables
//////////////////////////////////////////////////////////////////////////
Component::MapComponentResource       Component::_mapComponentResource;

//////////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////////
Component* Component::loadComponent(const NERegistry::ComponentEntry &entry, ComponentThread & componentThread )
{
    ASSERT(entry.mFuncCreate != NULL);

    Component* component = entry.mFuncCreate(entry, componentThread);
    if (component != NULL)
    {
        const NERegistry::WorkerThreadList& wThreads = entry.getWorkerThreads();
        for (int i = 0; i < wThreads.getSize(); ++ i)
        {
            const NERegistry::WorkerThreadEntry& wtEntry = wThreads[i];
            IEWorkerThreadConsumer* consumer = static_cast<Component *>(component)->workerThreadConsumer(wtEntry.mConsumerName.getString(), wtEntry.mThreadName.getBuffer());
            if (consumer != NULL)
                component->createWorkerThread(wtEntry.mThreadName.getString(), *consumer, componentThread);
        }
    }
    return component;
}

void Component::unloadComponent( Component& comItem, const NERegistry::ComponentEntry& entry )
{
    ASSERT(entry.mFuncDelete != NULL);

    const NERegistry::WorkerThreadList& wThreads = entry.getWorkerThreads();
    for (int i = 0; i < wThreads.getSize(); ++ i)
        comItem.deleteWorkerThread(wThreads[i].mThreadName.getString());

    entry.mFuncDelete(comItem, entry);
}

Component* Component::findComponentByName( const char* roleName )
{
    ASSERT(NEString::isEmpty<char>(roleName) == false);
    return _mapComponentResource.findResourceObject(roleName);
}

Component* Component::findComponentByAddress( const ComponentAddress& comAddress )
{
    Component* result = Component::findComponentByName(comAddress.getRoleName().getString());
    return (result != NULL && result->getAddress() == comAddress ? result : NULL);
}

bool Component::existComponent( const char* roleName )
{
    ASSERT(NEString::isEmpty<char>(roleName) == false);
    return _mapComponentResource.existResource(roleName);
}

ComponentThread& Component::_getCurrentComponentThread( void )
{
    ComponentThread* result = RUNTIME_CAST(&(DispatcherThread::getCurrentDispatcherThread()), ComponentThread);
    ASSERT(result != NULL);
    return *result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor 
//////////////////////////////////////////////////////////////////////////
Component::Component( ComponentThread & masterThread, const char* roleName)
    : RuntimeObject           ( )

    , mComponentInfo    (masterThread, roleName)
    , mServerList       ( )
{
    _mapComponentResource.registerResourceObject(mComponentInfo.getRoleName(), this);
}

Component::Component( const char* roleName )
    : RuntimeObject           ( )

    , mComponentInfo    (_getCurrentComponentThread(), roleName)
    , mServerList       ( )
{
    _mapComponentResource.registerResourceObject(mComponentInfo.getRoleName(), this);
}


Component::~Component( void )
{
    _mapComponentResource.unregisterResourceObject(mComponentInfo.getRoleName());
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
WorkerThread* Component::createWorkerThread( const char* threadName, IEWorkerThreadConsumer& consumer, ComponentThread & /* masterThread */ )
{
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread == NULL)
    {
        OUTPUT_DBG("Going to Create WorkerThread object [ %s ]", threadName);
        workThread = DEBUG_NEW WorkerThread(threadName, self(), consumer);
        if (workThread != NULL)
        {
            if (workThread->createThread(Thread::WAIT_INFINITE))
            {
                OUTPUT_DBG("Registering WorkerThread [ %s ]", threadName);
                mComponentInfo.registerWorkerThread(*workThread);
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

void Component::deleteWorkerThread( const char* threadName )
{
    OUTPUT_DBG("Going to Delete WorkerThread object [ %s ]", threadName);
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread != NULL)
    {
        OUTPUT_DBG("Unregistering and deleting WorkerThread [ %s ]", threadName);
        workThread->destroyThread(Thread::WAIT_INFINITE);
        mComponentInfo.unregisterWorkerThread(*workThread);
        delete workThread;
    }
}

void Component::startupComponent( ComponentThread& /* comThread */ )
{
    LISTPOS pos = NULL;
    
    for (pos = mServerList.firstPosition(); pos != NULL; pos = mServerList.nextPosition(pos))
    {
        StubBase * stub = mServerList[pos];
        ASSERT( stub != NULL );
        stub->startupServiceInterface(self());
        ServiceManager::requestRegisterServer(stub->getAddress());
    }
}

void Component::shutdownComponent( ComponentThread& /* comThread */ )
{
    LISTPOS pos = NULL;

    for (pos = mServerList.firstPosition(); pos != NULL; pos = mServerList.nextPosition(pos))
    {
        StubBase * stub = mServerList[pos];
        ASSERT(stub != NULL);
        OUTPUT_INFO("Shutting down Service Interface [ %s ] of component [ %s ]", stub->getAddress().getServiceName().getString(), getRoleName().getBuffer());
        stub->shutdownServiceIntrface(self());
        ServiceManager::requestUnregisterServer(stub->getAddress());
    }
    
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        OUTPUT_INFO("Sutting down worker thread [ %s ] of component [ %s ]", workerThread->getName().getString(), getRoleName().getBuffer());
        workerThread->shutdownThread();
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::notifyComponentShutdown( ComponentThread& /*comThread */ )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        OUTPUT_INFO("Sutting down worker thread [ %s ] of component [ %s ]", workerThread->getName().getString(), getRoleName().getBuffer());
        workerThread->shutdownThread();
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::registerServerItem( StubBase& server )
{
    mServerList.pushLast(&server);
}

StubBase* Component::findServerByName( const char* serviceName )
{
    StubBase* result = NULL;
    LISTPOS pos = mServerList.firstPosition();
    for ( ; pos != NULL; pos = mServerList.nextPosition(pos))
    {
        StubBase* stub = mServerList.getAt(pos);
        ASSERT(stub != NULL);
        if (stub->getAddress().getServiceName() == serviceName)
        {
            result = stub;
            break;
        }
    }
    return result;
}

void Component::waitComponentCompletion( unsigned int waitTimeout )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != NULL)
    {
        workerThread->completionWait(waitTimeout);
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

IEWorkerThreadConsumer* Component::workerThreadConsumer( const char* /* consumerName */, const char* /* workerThreadName */)
{
    return NULL;
}
