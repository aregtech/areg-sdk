/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Component.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component class implementation.
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
AREG_IMPLEMENT_RUNTIME(Component, RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Static Methods
//////////////////////////////////////////////////////////////////////////

Component::MapComponentResource& Component::resource_map()
{
    static Component::MapComponentResource _mapResource;
    return _mapResource;
}

Component* Component::loadComponent(const areg::ComponentEntry &entry, ComponentThread & componentThread )
{
    ASSERT(entry.mFuncCreate != nullptr);

    Component* component = entry.mFuncCreate(entry, componentThread);
    if (component != nullptr)
    {
        const areg::WorkerThreadList& wThreads = entry.getWorkerThreads();
        for (uint32_t i = 0; i < wThreads.mListWorkers.getSize(); ++ i)
        {
            const areg::WorkerThreadEntry& wtEntry = wThreads.mListWorkers[i];
            WorkerThreadConsumer* consumer = static_cast<Component *>(component)->workerThreadConsumer(wtEntry.mConsumerName.getString(), wtEntry.mThreadName.getBuffer());
            if (consumer != nullptr)
            {
                WorkerThread * wThread = component->createWorkerThread( wtEntry.mThreadName.getString()
                                                                      , *consumer
                                                                      , componentThread
                                                                      , wtEntry.mWatchdogTimeout
                                                                      , wtEntry.mStackSizeKb
                                                                      , wtEntry.mMaxQueue);

                if (wThread != nullptr)
                {
                    component->notifyWorkerThreadStarted(*consumer, *wThread);
                }
            }
        }
    }

    return component;
}

void Component::unloadComponent( Component& comItem, const areg::ComponentEntry& entry )
{
    ASSERT(entry.mFuncDelete != nullptr);

    const areg::WorkerThreadList& wThreads = entry.getWorkerThreads();
    for (uint32_t i = 0; i < wThreads.mListWorkers.getSize(); ++i)
    {
        comItem.deleteWorkerThread(wThreads.mListWorkers[i].mThreadName.getString());
    }

    entry.mFuncDelete(comItem, entry);
}

Component* Component::findComponentByName( const areg::String & roleName )
{
    ASSERT(roleName.isEmpty() == false);
    
    return Component::resource_map().findResourceObject(areg::crc32Calculate(roleName.getString()));
}

Component * Component::findComponentByNumber(uint32_t magicNum)
{
    ASSERT(magicNum != areg::CHECKSUM_IGNORE);

    return Component::resource_map().findResourceObject(magicNum);
}

Component* Component::findComponentByAddress( const ComponentAddress& comAddress )
{
    Component* result = Component::resource_map().findResourceObject( static_cast<uint32_t>(comAddress.getRoleName()) );
    return (result != nullptr && result->getAddress() == comAddress ? result : nullptr);
}

bool Component::existComponent( const areg::String & roleName )
{
    ASSERT(roleName.isEmpty() == false);
    return Component::resource_map().existResource(areg::crc32Calculate(roleName.getString()));
}

ComponentThread& Component::_getCurrentComponentThread()
{
    ComponentThread* result = AREG_RUNTIME_CAST(&(DispatcherThread::getCurrentDispatcherThread()), ComponentThread);
    ASSERT(result != nullptr);
    return *result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor 
//////////////////////////////////////////////////////////////////////////
Component::Component( const areg::String & roleName, ComponentThread & ownerThread )
    : RuntimeObject ( )

    , mComponentInfo( ownerThread, roleName )
    , mMagicNum     ( Component::_magicNumber(self()) )
    , mServerList   ( )
{
    Component::resource_map().registerResourceObject(mMagicNum, this);
}

Component::Component( const areg::ComponentEntry & regEntry, ComponentThread & ownerThread )
    : RuntimeObject ( )

    , mComponentInfo( ownerThread, regEntry.mRoleName)
    , mMagicNum     ( Component::_magicNumber(self()) )
    , mServerList   ( )
{
    Component::resource_map().registerResourceObject(mMagicNum, this);
}

Component::Component( const areg::String & roleName )
    : RuntimeObject           ( )

    , mComponentInfo    (_getCurrentComponentThread(), roleName)
    , mMagicNum         ( Component::_magicNumber(self()) )
    , mServerList       ( )
{
    Component::resource_map().registerResourceObject(mMagicNum, this);
}


Component::~Component()
{
    Component::resource_map().unregisterResourceObject(mMagicNum);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
WorkerThread* Component::createWorkerThread(  const areg::String & threadName
                                            , WorkerThreadConsumer& consumer
                                            , ComponentThread & /* ownerThread */
                                            , uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE */
                                            , uint32_t stackSizeKb      /* = areg::STACK_SIZE_DEFAULT */
                                            , uint32_t maxQeueue        /* = areg::IGNORE_VALUE */)
{
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread == nullptr)
    {
        workThread = DEBUG_NEW WorkerThread(threadName, self(), consumer, watchdogTimeout, stackSizeKb, maxQeueue);
        if (workThread != nullptr)
        {
            if (workThread->createThread(areg::WAIT_INFINITE))
            {
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

void Component::deleteWorkerThread( const areg::String & threadName )
{
    WorkerThread* workThread = mComponentInfo.findWorkerThread(threadName);
    if (workThread != nullptr)
    {
        workThread->shutdownThread(areg::WAIT_INFINITE);
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
        workerThread->shutdownThread( areg::WAIT_INFINITE );
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::notifyComponentShutdown( ComponentThread& /*comThread */ )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != nullptr)
    {
        workerThread->shutdownThread( areg::WAIT_INFINITE );
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

void Component::terminateSelf()
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

StubBase* Component::findServerByName( const areg::String & serviceName )
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

void Component::waitComponentCompletion( uint32_t waitTimeout )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.getFirstWorkerThread(addrThread);
    while (workerThread != nullptr)
    {
        workerThread->shutdownThread(waitTimeout);
        workerThread = mComponentInfo.getNextWorkerThread(addrThread);
    }
}

WorkerThreadConsumer* Component::workerThreadConsumer( const areg::String & /* consumerName */, const areg::String & /* workerThreadName */)
{
    return nullptr;
}

void Component::notifyWorkerThreadStarted(WorkerThreadConsumer& /*consumer*/, WorkerThread& /*workerThread*/)
{
}

uint32_t Component::_magicNumber(Component & comp)
{
    uint32_t result = areg::CHECKSUM_IGNORE;
    if ( comp.getAddress().isValid() )
    {
        result = areg::crc32Calculate(comp.getRoleName().getString());
    }

    return result;
}

inline void Component::_shutdownServices()
{
    for (ListServers::LISTPOS pos = mServerList.firstPosition(); mServerList.isValidPosition(pos); pos = mServerList.nextPosition(pos))
    {
        StubBase* stub = mServerList.valueAtPosition(pos);
        ASSERT(stub != nullptr);

        stub->shutdownServiceInterface(self());
        ServiceManager::requestUnregisterServer(stub->getAddress(), areg::DisconnectReason::ProviderDisconnected);
    }
}
