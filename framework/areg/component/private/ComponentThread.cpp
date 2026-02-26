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

namespace areg
{

    //////////////////////////////////////////////////////////////////////////
    // ComponentThread class implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Implement runtime
    //////////////////////////////////////////////////////////////////////////

    AREG_IMPLEMENT_RUNTIME(ComponentThread, areg::DispatcherThread)

    //////////////////////////////////////////////////////////////////////////
    // Implement static methods
    //////////////////////////////////////////////////////////////////////////
    Component* ComponentThread::getCurrentComponent()
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

    inline ComponentThread & ComponentThread::self()
    {
        return (*this);
    }

    inline ComponentThread* ComponentThread::_getCurrentComponentThread()
    {
        return AREG_RUNTIME_CAST( &(areg::DispatcherThread::getCurrentDispatcherThread( )), ComponentThread );
    }

    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    ComponentThread::ComponentThread( const areg::String & threadName
                                    , uint32_t watchdogTimeout  /* = areg::WATCHDOG_IGNORE      */
                                    , uint32_t stackSizeKb      /* = areg::STACK_SIZE_DEFAULT   */
                                    , uint32_t maxQueue         /* = areg::IGNORE_VALUE         */ )
        : areg::DispatcherThread  ( threadName, stackSizeKb, maxQueue )

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

    bool ComponentThread::runDispatcher()
    {
        bool result{ false };
        if (createComponents() > 0)
        {
            readyForEvents( true );
            startComponents();
            result = areg::DispatcherThread::runDispatcher();
        }

        return result;
    }

    int32_t ComponentThread::createComponents()
    {
        int32_t result = 0;
        const areg::ComponentList& comList = ComponentLoader::findComponentList(getName());
        if (comList.isValid())
        {
            for (uint32_t i = 0; i < comList.mListComponents.getSize(); ++ i)
            {
                const areg::ComponentEntry& entry = comList.mListComponents[i];
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

    void ComponentThread::destroyComponents()
    {
        while (mListComponent.isEmpty() == false)
        {
            Component* comObj = nullptr;
            if (mListComponent.removeLast(comObj))
            {
                ASSERT(comObj != nullptr);
                const areg::ComponentEntry& entry = ComponentLoader::findComponentEntry(comObj->getRoleName(), getName());
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

    void ComponentThread::startComponents()
    {
        ListComponent::LISTPOS pos = mListComponent.lastPosition();
        while (mListComponent.isValidPosition(pos))
        {
            Component* comObj = mListComponent.getPrev(pos);
            ASSERT(comObj != nullptr);
            comObj->startupComponent(self());
        }
    }

    void ComponentThread::shutdownComponents()
    {
        _shutdownProxies();
        _shutdownComponents();
    }

    areg::DispatcherThread* ComponentThread::getEventConsumerThread( const areg::RuntimeClassID& whichClass )
    {
        areg::DispatcherThread* result = hasRegisteredConsumer(whichClass) ? static_cast<areg::DispatcherThread *>(this) : nullptr;
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

    void ComponentThread::terminateSelf()
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

        areg::ArrayList<std::shared_ptr<ProxyBase>> proxyList;
        ProxyBase::findThreadProxies(self(), proxyList);
        for (uint32_t i = 0; i < proxyList.getSize(); ++i)
        {
            std::shared_ptr<ProxyBase> proxy = proxyList[i];
            ASSERT(proxy != nullptr);
            proxy->terminateSelf();
        }

        areg::DispatcherThread::shutdownThread(areg::TIMEOUT_10_MS);

        delete this;
    }

    inline void ComponentThread::_shutdownProxies()
    {
        areg::ArrayList<std::shared_ptr<ProxyBase>> proxyList;
        ProxyBase::findThreadProxies(self(), proxyList);
        for (uint32_t i = 0; i < proxyList.getSize(); ++i)
        {
            std::shared_ptr<ProxyBase> proxy = proxyList[i];
            ASSERT(proxy != nullptr);
            proxy->stopProxy();
        }
    }

    inline void ComponentThread::_shutdownComponents()
    {
        ListComponent::LISTPOS pos = mListComponent.firstPosition();
        while (mListComponent.isValidPosition(pos))
        {
            Component* comObj = mListComponent.getNext(pos);
            ASSERT(comObj != nullptr);
            comObj->shutdownComponent(self());
        }
    }

    areg::Thread::ThreadCompletion ComponentThread::shutdownThread( uint32_t waitForStopMs /*= areg::DO_NOT_WAIT*/ )
    {
        ListComponent::LISTPOS pos = mListComponent.firstPosition( );
        while ( mListComponent.isValidPosition( pos ) )
        {
            Component * comObj = mListComponent.getNext( pos );
            ASSERT( comObj != nullptr );
            comObj->notifyComponentShutdown( self( ) );
        }

        return areg::DispatcherThread::shutdownThread( waitForStopMs );
    }

    bool ComponentThread::completionWait( uint32_t waitForCompleteMs /*= areg::WAIT_INFINITE */ )
    {
        ListComponent::LISTPOS pos = mListComponent.firstPosition();
        while ( mListComponent.isValidPosition(pos) )
        {
            Component* comObj = mListComponent.getNext(pos);
            ASSERT(comObj != nullptr);
            comObj->waitComponentCompletion(waitForCompleteMs);
        }

        return areg::DispatcherThread::completionWait(waitForCompleteMs);
    }

    int32_t ComponentThread::onThreadExit()
    {
        shutdownComponents();
        destroyComponents();

        return areg::DispatcherThread::onThreadExit( );
    }

    bool ComponentThread::dispatchEvent(Event& eventElem)
    {
        mWatchdog.startGuard();
        bool result = areg::DispatcherThread::dispatchEvent(eventElem);
        mWatchdog.stopGuard();

        return result;
    }

} // namespace areg
