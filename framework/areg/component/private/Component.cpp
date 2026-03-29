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
namespace areg {

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

Component::MapComponentResource& Component::resource_map() noexcept
{
    static Component::MapComponentResource _mapResource;
    return _mapResource;
}

Component* Component::load_component(const areg::ComponentEntry &entry, ComponentThread & componentThread )
{
    ASSERT(entry.mFuncCreate != nullptr);

    Component* component = entry.mFuncCreate(entry, componentThread);
    if (component == nullptr)
        return nullptr;
    
    const areg::WorkerThreadList& wThreads = entry.worker_threads();
    for (uint32_t i = 0; i < wThreads.mListWorkers.size(); ++ i)
    {
        const areg::WorkerThreadEntry& wtEntry = wThreads.mListWorkers[i];
        WorkerThreadConsumer* consumer = static_cast<Component *>(component)->worker_thread_consumer(wtEntry.mConsumerName.as_string(), wtEntry.mThreadName.buffer());
        if (consumer == nullptr)
            continue;
        
        WorkerThread * wThread = component->create_worker_thread( wtEntry.mThreadName.as_string()
                                                                , *consumer
                                                                , componentThread
                                                                , wtEntry.mWatchdogTimeout
                                                                , wtEntry.mStackSizeKb
                                                                , wtEntry.mMaxQueue);

        if (wThread != nullptr)
        {
            component->notify_thread_started(*consumer, *wThread);
        }
    }

    return component;
}

void Component::unload_component( Component& comItem, const areg::ComponentEntry& entry ) noexcept
{
    ASSERT(entry.mFuncDelete != nullptr);

    const areg::WorkerThreadList& wThreads = entry.worker_threads();
    for (uint32_t i = 0; i < wThreads.mListWorkers.size(); ++i)
    {
        comItem.delete_worker_thread(wThreads.mListWorkers[i].mThreadName.as_string());
    }

    entry.mFuncDelete(comItem, entry);
}

Component* Component::find_by_name( const String & roleName ) noexcept
{
    ASSERT(roleName.is_empty() == false);
    
    return Component::resource_map().find_resource_object(areg::crc32_calculate(roleName.as_string()));
}

Component * Component::find_by_number(uint32_t magicNum) noexcept
{
    ASSERT(magicNum != areg::CHECKSUM_IGNORE);

    return Component::resource_map().find_resource_object(magicNum);
}

Component* Component::find_by_address( const ComponentAddress& comAddress ) noexcept
{
    Component* result = Component::resource_map().find_resource_object( static_cast<uint32_t>(comAddress.role_name()) );
    return (result != nullptr && result->address() == comAddress ? result : nullptr);
}

bool Component::exist( const String & roleName ) noexcept
{
    ASSERT(roleName.is_empty() == false);
    return Component::resource_map().exist(areg::crc32_calculate(roleName.as_string()));
}

ComponentThread& Component::_current_component_thread() noexcept
{
    ComponentThread* result = AREG_RUNTIME_CAST(&(DispatcherThread::current_dispatcher_thread()), ComponentThread);
    ASSERT(result != nullptr);
    return *result;
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor 
//////////////////////////////////////////////////////////////////////////
Component::Component( const String & roleName, ComponentThread & ownerThread )
    : RuntimeObject ( )

    , mComponentInfo( ownerThread, roleName )
    , mMagicNum     ( Component::_magic_number(self()) )
    , mServerList   ( )
{
    Component::resource_map().register_resource_object(mMagicNum, this);
}

Component::Component( const areg::ComponentEntry & regEntry, ComponentThread & ownerThread )
    : RuntimeObject ( )

    , mComponentInfo( ownerThread, regEntry.mRoleName)
    , mMagicNum     ( Component::_magic_number(self()) )
    , mServerList   ( )
{
    Component::resource_map().register_resource_object(mMagicNum, this);
}

Component::Component( const String & roleName )
    : RuntimeObject           ( )

    , mComponentInfo    (_current_component_thread(), roleName)
    , mMagicNum         ( Component::_magic_number(self()) )
    , mServerList       ( )
{
    Component::resource_map().register_resource_object(mMagicNum, this);
}


Component::~Component()
{
    Component::resource_map().unregister_resource_object(mMagicNum);
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
WorkerThread* Component::create_worker_thread(  const String & threadName
                                            , WorkerThreadConsumer& consumer
                                            , ComponentThread & /* ownerThread */
                                            , uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE */
                                            , uint32_t stackSizeKb      /* = areg::DEFAULT_STACK_SIZE */
                                            , uint32_t maxQeueue        /* = areg::IGNORE_VALUE */)
{
    WorkerThread* workThread = mComponentInfo.find_worker_thread(threadName);
    if (workThread != nullptr)
        return workThread;
    
    workThread = DEBUG_NEW WorkerThread(threadName, self(), consumer, watchdogTimeout, stackSizeKb, maxQeueue);
    if (workThread == nullptr)
        return nullptr;
    
    if (workThread->start(areg::WAIT_INFINITE))
    {
        mComponentInfo.register_worker_thread(*workThread);
        return workThread;
    }
    else
    {
        delete workThread;
        return nullptr;
    }
}

void Component::delete_worker_thread( const String & threadName )
{
    WorkerThread* workThread = mComponentInfo.find_worker_thread(threadName);
    if (workThread != nullptr)
    {
        workThread->shutdown(areg::WAIT_INFINITE);
        mComponentInfo.unregister_worker_thread(*workThread);
        delete workThread;
    }
}

void Component::startup_component( ComponentThread& /* comThread */ )
{
    for (ListServers::LISTPOS pos = mServerList.first_position(); mServerList.is_valid_position(pos); pos = mServerList.next_position(pos))
    {
        StubBase * stub = mServerList.value_at(pos);
        ASSERT( stub != nullptr );
        stub->startup_service_interface(self());
        ServiceManager::request_register_provider(stub->address());
    }
}

void Component::shutdown_component( ComponentThread& /* comThread */ )
{
    _shutdown_services();

    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.first_worker_thread(addrThread);
    while (workerThread != nullptr)
    {
        workerThread->shutdown( areg::WAIT_INFINITE );
        workerThread = mComponentInfo.next_worker_thread(addrThread);
    }
}

void Component::notify_component_shutdown( ComponentThread& /*comThread */ )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.first_worker_thread(addrThread);
    while (workerThread != nullptr)
    {
        workerThread->shutdown( areg::WAIT_INFINITE );
        workerThread = mComponentInfo.next_worker_thread(addrThread);
    }
}

void Component::terminate_self()
{
    _shutdown_services();

    ThreadAddress threadAddress;

    while (mComponentInfo.has_worker_threads())
    {
        WorkerThread* workThread = mComponentInfo.remove_worker_thread(threadAddress);
        ASSERT(workThread != nullptr);
        workThread->terminate_self();
    }

    delete this;
}

StubBase* Component::find_provider( const String & serviceName ) noexcept
{
    StubBase* result = nullptr;
    for (ListServers::LISTPOS  pos = mServerList.first_position(); mServerList.is_valid_position(pos); pos = mServerList.next_position(pos))
    {
        StubBase* stub = mServerList.value_at(pos);
        ASSERT(stub != nullptr);
        if (stub->address().service_name() == serviceName)
        {
            result = stub;
            break;
        }
    }

    return result;
}

void Component::wait_component_completion( uint32_t waitTimeout )
{
    ThreadAddress addrThread;
    WorkerThread * workerThread = mComponentInfo.first_worker_thread(addrThread);
    while (workerThread != nullptr)
    {
        workerThread->shutdown(waitTimeout);
        workerThread = mComponentInfo.next_worker_thread(addrThread);
    }
}

WorkerThreadConsumer* Component::worker_thread_consumer( const String & /* consumerName */, const String & /* workerThreadName */)
{
    return nullptr;
}

void Component::notify_thread_started(WorkerThreadConsumer& /*consumer*/, WorkerThread& /*workerThread*/)
{
}

uint32_t Component::_magic_number(Component & comp) noexcept
{
    return ( comp.address().is_valid() ? areg::crc32_calculate(comp.role_name().as_string()) : areg::CHECKSUM_IGNORE );
}

inline void Component::_shutdown_services()
{
    for (ListServers::LISTPOS pos = mServerList.first_position(); mServerList.is_valid_position(pos); pos = mServerList.next_position(pos))
    {
        StubBase* stub = mServerList.value_at(pos);
        ASSERT(stub != nullptr);

        stub->shutdown_service_interface(self());
        ServiceManager::request_unregister_provider(stub->address(), areg::DisconnectReason::ProviderDisconnected);
    }
}

} // namespace areg
