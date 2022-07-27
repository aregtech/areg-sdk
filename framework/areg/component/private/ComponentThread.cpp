/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/ComponentThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
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
ComponentThread::ComponentThread( const String & threadName )
    : DispatcherThread  ( threadName )

    , mCurrentComponent ( nullptr )
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
    bool result = false;
    if (createComponents() > 0)
    {
        startComponents();
        result = DispatcherThread::runDispatcher();
    }

    return result;
}

int ComponentThread::createComponents( void )
{
    OUTPUT_DBG("Starting to create components in thread [ %s ].", getName().getString());
    int result = 0;
    const NERegistry::ComponentList& comList = ComponentLoader::getComponentList(getName());
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
                    OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been created...", comObj->getRoleName().getString(), getName().getString());
                    mListComponent.pushLast(comObj);
                    result ++;
                }
                else
                {
                    OUTPUT_ERR("Failed creating component [ %s ] in UNKNOWN nullptr thread", entry.mRoleName.getString());
                }
            }
        }
    }

    return result;
}

void ComponentThread::destroyComponents( void )
{
    OUTPUT_DBG("Going to destroy components in thread [ %s ]. There are [ %d ] components in the thread.", getName().getString(), mListComponent.getSize());
    while (mListComponent.isEmpty() == false)
    {
        Component* comObj = nullptr;
        if (mListComponent.removeLast(comObj))
        {
            ASSERT(comObj != nullptr);
            OUTPUT_DBG("Destroying component [ %s ] in thread [ %s ]...", comObj->getRoleName().getString(), getName().getString());
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
    OUTPUT_DBG("Starting components in thread [ %s ].", getName().getString());
    ListComponent::LISTPOS pos = mListComponent.lastPosition();
    while (mListComponent.isValidPosition(pos))
    {
        Component* comObj = mListComponent.getPrev(pos);
        ASSERT(comObj != nullptr);
        comObj->startupComponent(self());
        OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been started...", comObj->getRoleName().getString(), getName().getString());
    }
}

void ComponentThread::shutdownComponents( void )
{
    OUTPUT_DBG("Going to shutdown components in thread [ %s ].", getName().getString());

    TEArrayList<ProxyBase *> proxyList;
    ProxyBase::findThreadProxies( self(), proxyList );
    for ( uint32_t i = 0; i < proxyList.getSize(); ++ i)
    {
        ProxyBase * proxy = proxyList.getAt(i);
        ASSERT(proxy != nullptr);
        proxy->stopProxy();
    }

    ListComponent::LISTPOS pos = mListComponent.firstPosition();
    while (mListComponent.isValidPosition(pos))
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != nullptr);
        comObj->shutdownComponent(self());
        OUTPUT_DBG("Shutdown component [ %s ] in thread [ %s ]...", comObj->getRoleName().getString(), getName().getString());
    }
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

void ComponentThread::shutdownThread( void )
{
    ListComponent::LISTPOS pos = mListComponent.firstPosition();
    while (mListComponent.isValidPosition(pos))
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != nullptr);
        comObj->notifyComponentShutdown(self());
        OUTPUT_DBG("The component [ %s ] is notified thread [ %s ] is going to shutdown!", comObj->getRoleName().getString(), getName().getString());
    }

    DispatcherThread::shutdownThread();
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
    int result = DispatcherThread::onThreadExit();

    shutdownComponents();
    destroyComponents();

    return result;
}
