/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Component Thread.
 *
 ************************************************************************/
#include "areg/component/ComponentThread.hpp"

#include "areg/component/Component.hpp"
#include "areg/component/ProxyBase.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/NERegistry.hpp"

//////////////////////////////////////////////////////////////////////////
// ComponentThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement runtime
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_RUNTIME(ComponentThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Implement static methods
//////////////////////////////////////////////////////////////////////////
Component* ComponentThread::getCurrentComponent( void )
{
    ComponentThread* comThread = ComponentThread::_getCurrentComponentThread();
    return (comThread != nullptr ? comThread->mCurrentComponent : nullptr);
}

bool ComponentThread::setCurrentComponent( Component* curComponent )
{
    ComponentThread* comThread = ComponentThread::_getCurrentComponentThread();
    if (comThread != nullptr)
        comThread->mCurrentComponent = curComponent;
    return (comThread != nullptr);
}

inline ComponentThread & ComponentThread::self( void )
{
    return (*this);
}

inline ComponentThread* ComponentThread::_getCurrentComponentThread( void )
{
    return RUNTIME_CAST( &(DispatcherThread::getCurrentDispatcherThread( )), ComponentThread );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentThread::ComponentThread( const String & threadName, uint32_t watchdogTimeout /*= NECommon::WATCHDOG_IGNORE*/, uint32_t stackSizeKb /*= NECommon::STACK_SIZE_DEFAULT*/)
    : DispatcherThread  ( threadName, stackSizeKb )

    , mCurrentComponent ( nullptr )
    , mWatchdog         ( self(), watchdogTimeout )
    , mListComponent    ( )
{
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool ComponentThread::postEvent( Event& eventElem )
{
    return EventDispatcher::postEvent(eventElem);
}

bool ComponentThread::runDispatcher( void )
{
    bool result{ false };
    if (createComponents() > 0)
    {
        readyForEvents( true );
        startComponents();
        result = DispatcherThread::runDispatcher();
    }

    return result;
}

int ComponentThread::createComponents( void )
{
    int result = 0;
    const NERegistry::ComponentList& comList = ComponentLoader::findComponentList(getName());
    if (comList.isValid())
    {
        for (uint32_t i = 0; i < comList.mListComponents.getSize(); ++ i)
        {
            const NERegistry::ComponentEntry& entry = comList.mListComponents[i];
            if (entry.isValid() && entry.mFuncCreate != nullptr)
            {
                Component *comObj = Component::loadComponent(entry, self());
                if (comObj != nullptr)
                {
                    mListComponent.pushLast(comObj);
                    result ++;
                }
            }
        }
    }

    return result;
}

void ComponentThread::destroyComponents( void )
{
    while (mListComponent.isEmpty() == false)
    {
        Component* comObj = nullptr;
        if (mListComponent.removeLast(comObj))
        {
            ASSERT(comObj != nullptr);
            const NERegistry::ComponentEntry& entry = ComponentLoader::findComponentEntry(comObj->getRoleName(), getName());
            if (entry.isValid() && entry.mFuncDelete != nullptr)
            {
                Component::unloadComponent(*comObj, entry);
            }
            else
            {
                delete comObj;
            }
        }
    }
}

void ComponentThread::startComponents( void )
{
    ListComponent::LISTPOS pos = mListComponent.lastPosition();
    while (mListComponent.isValidPosition(pos))
    {
        Component* comObj = mListComponent.getPrev(pos);
        ASSERT(comObj != nullptr);
        comObj->startupComponent(self());
    }
}

void ComponentThread::shutdownComponents( void )
{
    _shutdownProxies();
    _shutdownComponents();
}

DispatcherThread* ComponentThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    DispatcherThread* result = hasRegisteredConsumer(whichClass) ? static_cast<DispatcherThread *>(this) : nullptr;
    if (result == nullptr)
    {
        ListComponent::LISTPOS pos = mListComponent.firstPosition();
        while (mListComponent.isValidPosition(pos) && (result == nullptr))
        {
            Component* comObj = mListComponent.getNext(pos);
            ASSERT(comObj != nullptr);
            result = comObj->findEventConsumer(whichClass);
        }
    }

    return result;
}

void ComponentThread::terminateSelf(void)
{
    mHasStarted = false;
    removeAllEvents();
    mEventExit.setEvent();

    _shutdownProxies();

    while (mListComponent.isEmpty() == false)
    {
        Component* component{ nullptr };
        mListComponent.removeLast(component);
        ASSERT(component != nullptr);
        component->terminateSelf();
    }

    TEArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::findThreadProxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.getSize(); ++i)
    {
        std::shared_ptr<ProxyBase> proxy = proxyList[i];
        ASSERT(proxy != nullptr);
        proxy->terminateSelf();
    }

    DispatcherThread::shutdownThread(NECommon::TIMEOUT_10_MS);

    delete this;
}

inline void ComponentThread::_shutdownProxies(void)
{
    TEArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::findThreadProxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.getSize(); ++i)
    {
        std::shared_ptr<ProxyBase> proxy = proxyList[i];
        ASSERT(proxy != nullptr);
        proxy->stopProxy();
    }
}

inline void ComponentThread::_shutdownComponents(void)
{
    ListComponent::LISTPOS pos = mListComponent.firstPosition();
    while (mListComponent.isValidPosition(pos))
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != nullptr);
        comObj->shutdownComponent(self());
    }
}

Thread::eCompletionStatus ComponentThread::shutdownThread( unsigned int waitForStopMs /*= NECommon::DO_NOT_WAIT*/ )
{
    ListComponent::LISTPOS pos = mListComponent.firstPosition( );
    while ( mListComponent.isValidPosition( pos ) )
    {
        Component * comObj = mListComponent.getNext( pos );
        ASSERT( comObj != nullptr );
        comObj->notifyComponentShutdown( self( ) );
    }

    return DispatcherThread::shutdownThread( waitForStopMs );
}

bool ComponentThread::completionWait( unsigned int waitForCompleteMs /*= NECommon::WAIT_INFINITE */ )
{
    ListComponent::LISTPOS pos = mListComponent.firstPosition();
    while ( mListComponent.isValidPosition(pos) )
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != nullptr);
        comObj->waitComponentCompletion(waitForCompleteMs);
    }

    return DispatcherThread::completionWait(waitForCompleteMs);
}

int ComponentThread::onThreadExit(void)
{
    shutdownComponents();
    destroyComponents();

    return DispatcherThread::onThreadExit( );
}

bool ComponentThread::dispatchEvent(Event& eventElem)
{
    mWatchdog.startGuard();
    bool result = DispatcherThread::dispatchEvent(eventElem);
    mWatchdog.stopGuard();

    return result;
}
