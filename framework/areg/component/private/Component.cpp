/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Component.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
    ASSERT(entry.mFuncCreate != nullptr);

    Component* component = entry.mFuncCreate(entry, componentThread);
    if (component != nullptr)
    {
        const NERegistry::WorkerThreadList& wThreads = entry.getWorkerThreads();
        for (uint32_t i = 0; i < wThreads.mListWorkers.getSize(); ++ i)
        {
            const NERegistry::WorkerThreadEntry& wtEntry = wThreads.mListWorkers[i];
            IEWorkerThreadConsumer* consumer = static_cast<Component *>(component)->workerThreadConsumer(wtEntry.mConsumerName.getString(), wtEntry.mThreadName.getBuffer());
            if (consumer != nullptr)
            {
                component->createWorkerThread(wtEntry.mThreadName.getString(), *consumer, componentThread, wtEntry.mWatchdogTimeout);
            }
        }
    }

    return component;
}

void Component::unloadComponent( Component& comItem, const NERegistry::ComponentEntry& entry )
{
    ASSERT(entry.mFuncDelete != nullptr);

    const NERegistry::WorkerThreadList& wThreads = entry.getWorkerThreads();
    for (uint32_t i = 0; i < wThreads.mListWorkers.getSize(); ++i)
    {
        comItem.deleteWorkerThread(wThreads.mListWorkers[i].mThreadName.getString());
    }

    entry.mFuncDelete(comItem, entry);
}

Component* Component::findComponentByName( const String & roleName )
{
    ASSERT(roleName.isEmpty() == false);
    
    return _mapComponentResource.findResourceObject(NEMath::crc32Calculate(roleName.getString()));
}

Component * Component::findComponentByNumber(unsigned int magicNum)
{
    ASSERT(magicNum != NEMath::CHECKSUM_IGNORE);

    return _mapComponentResource.findResourceObject(magicNum);
}

Component* Component::findComponentByAddress( const ComponentAddress& comAddress )
{
    Component* result = _mapComponentResource.findResourceObject( static_cast<unsigned int>(comAddress.getRoleName()) );
    return (result != nullptr && result->getAddress() == comAddress ? result : nullptr);
}

bool Component::existComponent( const String & roleName )
{
    ASSERT(roleName.isEmpty() == false);
    return _mapComponentResource.existResource(NEMath::crc32Calculate(roleName.getString()));
}

ComponentThread& Component::_getCurrentComponentThread( void )
{
    ComponentThread* result = RUNTIME_CAST(&(DispatcherThread::getCurrentDispatcherThread()), ComponentThread);
    ASSERT(result != nullptr);
    return *result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor 
//////////////////////////////////////////////////////////////////////////
Component::Component( ComponentThread & masterThread, const String & roleName)
    : RuntimeObject           ( )

    , mComponentInfo    (masterThread, roleName)
    , mMagicNum         ( Component::_magicNumber(self()) )
    , mServerList       ( )
{
    _mapComponentResource.registerResourceObject(mMagicNum, this);
}

Component::Component( const String & roleName )
    : RuntimeObject           ( )

    , mComponentInfo    (_getCurrentComponentThread(), roleName)
    , mMagicNum         ( Component::_magicNumber(self()) )
    , mServerList       ( )
{
    _mapComponentResource.registerResourceObject(mMagicNum, this);
}


Component::~Component( void )
{
    _mapComponentResource.unregisterResourceObject(mMagicNum);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
WorkerThread* Component::createWorkerThread( const String & threadName, IEWorkerThreadConsumer& consumer, ComponentThread & /* masterThread */, uint32_t watchdogTimeout)
{
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread == nullptr)
    {
        OUTPUT_DBG("Going to Create WorkerThread object [ %s ]", threadName.getString());
        workThread = DEBUG_NEW WorkerThread(threadName, self(), consumer, watchdogTimeout);
        if (workThread != nullptr)
        {
            if (workThread->createThread(NECommon::WAIT_INFINITE))
            {
                OUTPUT_DBG("Registering WorkerThread [ %s ]", threadName.getString());
                mComponentInfo.registerWorkerThread(*workThread);
            }
            else
            {
                delete workThread;
                workThread = nullptr;
            }
        }
    }

    return workThread;
}

void Component::deleteWorkerThread( const String & threadName )
{
    OUTPUT_DBG("Going to Delete WorkerThread object [ %s ]", threadName.getString());
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread != nullptr)
    {
        OUTPUT_DBG("Unregistering and deleting WorkerThread [ %s ]", threadName.getString());
        workThread->destroyThread(NECommon::WAIT_INFINITE);
        mComponentInfo.unregisterWorkerThread(*workThread);
        delete workThread;
    }
}

void Component::startupComponent( ComponentThread& /* comThread */ )
{
    for (ListServers::LISTPOS pos = mServerList.firstPosition(); mServerList.isValidPosition(pos); pos = mServerList.nextPosition(pos))
    {
        StubBase * stub = mServerList.valueAtPosition(pos);
        ASSERT( stub != nullptr );
        stub->startupServiceInterface(self());
        ServiceManager::requestRegisterServer(stub->getAddress());
    }
}

void Component::shutdownComponent( ComponentThread& /* comThread */ )
{
    _shutdownServices();

    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != nullptr)
    {
        OUTPUT_INFO("Shutting down worker thread [ %s ] of component [ %s ]", workerThread->getName().getString(), getRoleName().getString());
        workerThread->shutdownThread();
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::notifyComponentShutdown( ComponentThread& /*comThread */ )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != nullptr)
    {
        OUTPUT_INFO("Shutting down worker thread [ %s ] of component [ %s ]", workerThread->getName().getString(), getRoleName().getString());
        workerThread->shutdownThread();
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::terminateSelf(void)
{
    _shutdownServices();

    ThreadAddress threadAddress;

    while (mComponentInfo.hasWorkerThreads())
    {
        WorkerThread* workThread = mComponentInfo.removeFirstWorkerThread(threadAddress);
        ASSERT(workThread != nullptr);
        workThread->terminateSelf();
    }

    delete this;
}

void Component::registerServerItem( StubBase& server )
{
    mServerList.pushLast(&server);
}

StubBase* Component::findServerByName( const String & serviceName )
{
    StubBase* result = nullptr;
    for (ListServers::LISTPOS  pos = mServerList.firstPosition(); mServerList.isValidPosition(pos); pos = mServerList.nextPosition(pos))
    {
        StubBase* stub = mServerList.valueAtPosition(pos);
        ASSERT(stub != nullptr);
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
    while (workerThread != nullptr)
    {
        workerThread->completionWait(waitTimeout);
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

IEWorkerThreadConsumer* Component::workerThreadConsumer( const String & /* consumerName */, const String & /* workerThreadName */)
{
    return nullptr;
}

unsigned int Component::_magicNumber(Component & comp)
{
    unsigned int result = NEMath::CHECKSUM_IGNORE;
    if ( comp.getAddress().isValid() )
    {
        result = NEMath::crc32Calculate(comp.getRoleName().getString());
    }

    return result;
}

inline void Component::_shutdownServices(void)
{
    for (ListServers::LISTPOS pos = mServerList.firstPosition(); mServerList.isValidPosition(pos); pos = mServerList.nextPosition(pos))
    {
        StubBase* stub = mServerList.valueAtPosition(pos);
        ASSERT(stub != nullptr);
        OUTPUT_INFO("Shutting down Service Interface [ %s ] of component [ %s ]", stub->getAddress().getServiceName().getString(), getRoleName().getString());
        stub->shutdownServiceIntrface(self());
        ServiceManager::requestUnregisterServer(stub->getAddress());
    }
}
