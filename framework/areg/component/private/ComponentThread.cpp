/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Thread.
 *
 ************************************************************************/
#include "areg/component/ComponentThread.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/Model.hpp"
namespace areg {

//////////////////////////////////////////////////////////////////////////
// ComponentThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement runtime
//////////////////////////////////////////////////////////////////////////

AREG_IMPLEMENT_RUNTIME(ComponentThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Implement static methods
//////////////////////////////////////////////////////////////////////////
Component* ComponentThread::current_component()
{
    ComponentThread* comThread = ComponentThread::_current_component_thread();
    return (comThread != nullptr ? comThread->mCurrentComponent : nullptr);
}

bool ComponentThread::set_current_component( Component* curComponent )
{
    ComponentThread* comThread = ComponentThread::_current_component_thread();
    if (comThread != nullptr)
        comThread->mCurrentComponent = curComponent;
    return (comThread != nullptr);
}

inline ComponentThread & ComponentThread::self()
{
    return (*this);
}

inline ComponentThread* ComponentThread::_current_component_thread()
{
    return AREG_RUNTIME_CAST( &(DispatcherThread::current_dispatcher_thread( )), ComponentThread );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentThread::ComponentThread( const String & threadName
                                , uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE      */
                                , uint32_t stackSizeKb      /* = areg::STACK_SIZE_DEFAULT   */
                                , uint32_t maxQueue         /* = areg::IGNORE_VALUE         */ )
    : DispatcherThread  ( threadName, stackSizeKb, maxQueue )

    , mCurrentComponent ( nullptr )
    , mWatchdog         ( self(), watchdogTimeout )
    , mListComponent    ( )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool ComponentThread::post_event( Event& eventElem )
{
    return EventDispatcher::post_event(eventElem);
}

bool ComponentThread::run_dispatcher()
{
    bool result{ false };
    if (create_components() > 0)
    {
        ready_for_events( true );
        start_components();
        result = DispatcherThread::run_dispatcher();
    }

    return result;
}

int32_t ComponentThread::create_components()
{
    int32_t result = 0;
    const areg::ComponentList& comList = ComponentLoader::find_component_list(name());
    if (comList.is_valid())
    {
        for (uint32_t i = 0; i < comList.mListComponents.size(); ++ i)
        {
            const areg::ComponentEntry& entry = comList.mListComponents[i];
            if (entry.is_valid() && entry.mFuncCreate != nullptr)
            {
                Component *comObj = Component::load_component(entry, self());
                if (comObj != nullptr)
                {
                    mListComponent.push_last(comObj);
                    result ++;
                }
            }
        }
    }

    return result;
}

void ComponentThread::destroy_components()
{
    while (mListComponent.is_empty() == false)
    {
        Component* comObj = nullptr;
        if (mListComponent.remove_last(comObj))
        {
            ASSERT(comObj != nullptr);
            const areg::ComponentEntry& entry = ComponentLoader::find_component_entry(comObj->role_name(), name());
            if (entry.is_valid() && entry.mFuncDelete != nullptr)
            {
                Component::unload_component(*comObj, entry);
            }
            else
            {
                delete comObj;
            }
        }
    }
}

void ComponentThread::start_components()
{
    ListComponent::LISTPOS pos = mListComponent.last_position();
    while (mListComponent.is_valid_position(pos))
    {
        Component* comObj = mListComponent.prev(pos);
        ASSERT(comObj != nullptr);
        comObj->startup_component(self());
    }
}

void ComponentThread::shutdown_components()
{
    _shutdown_proxies();
    _shutdown_components();
}

DispatcherThread* ComponentThread::event_consumer_thread( const RuntimeClassID& whichClass )
{
    DispatcherThread* result = has_registered_consumer(whichClass) ? static_cast<DispatcherThread *>(this) : nullptr;
    if (result == nullptr)
    {
        ListComponent::LISTPOS pos = mListComponent.first_position();
        while (mListComponent.is_valid_position(pos) && (result == nullptr))
        {
            Component* comObj = mListComponent.next(pos);
            ASSERT(comObj != nullptr);
            result = comObj->find_event_consumer(whichClass);
        }
    }

    return result;
}

void ComponentThread::terminate_self()
{
    mHasStarted = false;
    remove_all_events();
    mEventExit.set_event();

    _shutdown_proxies();

    while (mListComponent.is_empty() == false)
    {
        Component* component{ nullptr };
        mListComponent.remove_last(component);
        ASSERT(component != nullptr);
        component->terminate_self();
    }

    ArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::find_thread_proxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.size(); ++i)
    {
        std::shared_ptr<ProxyBase> proxy = proxyList[i];
        ASSERT(proxy != nullptr);
        proxy->terminate_self();
    }

    DispatcherThread::shutdown(areg::TIMEOUT_10_MS);

    delete this;
}

inline void ComponentThread::_shutdown_proxies()
{
    ArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::find_thread_proxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.size(); ++i)
    {
        std::shared_ptr<ProxyBase> proxy = proxyList[i];
        ASSERT(proxy != nullptr);
        proxy->stop_proxy();
    }
}

inline void ComponentThread::_shutdown_components()
{
    ListComponent::LISTPOS pos = mListComponent.first_position();
    while (mListComponent.is_valid_position(pos))
    {
        Component* comObj = mListComponent.next(pos);
        ASSERT(comObj != nullptr);
        comObj->shutdown_component(self());
    }
}

Thread::ThreadCompletion ComponentThread::shutdown( uint32_t waitForStopMs /*= areg::DO_NOT_WAIT*/ )
{
    ListComponent::LISTPOS pos = mListComponent.first_position( );
    while ( mListComponent.is_valid_position( pos ) )
    {
        Component * comObj = mListComponent.next( pos );
        ASSERT( comObj != nullptr );
        comObj->notify_component_shutdown( self( ) );
    }

    return DispatcherThread::shutdown( waitForStopMs );
}

bool ComponentThread::wait_completion( uint32_t waitForCompleteMs /*= areg::WAIT_INFINITE */ )
{
    ListComponent::LISTPOS pos = mListComponent.first_position();
    while ( mListComponent.is_valid_position(pos) )
    {
        Component* comObj = mListComponent.next(pos);
        ASSERT(comObj != nullptr);
        comObj->wait_component_completion(waitForCompleteMs);
    }

    return DispatcherThread::wait_completion(waitForCompleteMs);
}

int32_t ComponentThread::on_thread_exit()
{
    shutdown_components();
    destroy_components();

    return DispatcherThread::on_thread_exit( );
}

bool ComponentThread::dispatch_event(Event& eventElem)
{
    mWatchdog.start_guard();
    bool result = DispatcherThread::dispatch_event(eventElem);
    mWatchdog.stop_guard();

    return result;
}

} // namespace areg
