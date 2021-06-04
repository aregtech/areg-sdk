/************************************************************************
 * \file        areg/component/private/ComponentThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
    return (comThread != NULL ? comThread->mCurrentComponent : NULL);
}

bool ComponentThread::setCurrentComponent( Component* curComponent )
{
    ComponentThread* comThread = ComponentThread::_getCurrentComponentThread();
    if (comThread != NULL)
        comThread->mCurrentComponent = curComponent;
    return (comThread != NULL);
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
ComponentThread::ComponentThread( const char* threadName )
    : DispatcherThread    ( threadName )

    , mCurrentComponent     ( NULL )
    , mListComponent        ( )
{
    ; // do nothing
}

ComponentThread::~ComponentThread( void )
{
    ; // do nothing
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
        for (int i = 0; i < comList.getSize(); ++ i)
        {
            const NERegistry::ComponentEntry& entry = comList[i];
            if (entry.isValid() && entry.mFuncCreate != NULL)
            {
                Component *comObj = Component::loadComponent(entry, self());
                if (comObj != NULL)
                {
                    OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been created...", comObj->getRoleName().getString(), getName().getString());
                    mListComponent.pushLast(comObj);
                    result ++;
                }
                else
                {
                    OUTPUT_ERR("Failed creating component [ %s ] in UNKNOWN NULL thread");
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
        Component* comObj = mListComponent.removeLast();
        if (comObj != NULL)
        {
            OUTPUT_DBG("Destroying component [ %s ] in thread [ %s ]...", comObj->getRoleName().getString(), getName().getString());
            const NERegistry::ComponentEntry& entry = ComponentLoader::findComponentEntry(comObj->getRoleName(), getName());
            if (entry.isValid() && entry.mFuncDelete != NULL)
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
    LISTPOS pos = mListComponent.lastPosition();
    while (pos != NULL)
    {
        Component* comObj = mListComponent.getPrev(pos);
        ASSERT(comObj != NULL);
        comObj->startupComponent(self());
        OUTPUT_DBG("Component [ %s ] in thread [ %s ] has been started...", comObj->getRoleName().getString(), getName().getString());
    }
}

void ComponentThread::shutdownComponents( void )
{
    OUTPUT_DBG("Going to shutdown components in thread [ %s ].", getName().getString());

    TEArrayList<ProxyBase *, ProxyBase *> proxyList;
    ProxyBase::findThreadProxies( self(), proxyList );
    for ( int i = 0; i < proxyList.getSize(); ++ i)
    {
        ProxyBase * proxy = proxyList.getAt(i);
        ASSERT(proxy != NULL);
        proxy->stopProxy();
    }

    LISTPOS pos = mListComponent.firstPosition();
    while (pos != NULL)
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != NULL);
        comObj->shutdownComponent(self());
        OUTPUT_DBG("Shutdown component [ %s ] in thread [ %s ]...", comObj->getRoleName().getString(), getName().getString());
    }
}

DispatcherThread* ComponentThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    DispatcherThread* result = hasRegisteredConsumer(whichClass) ? static_cast<DispatcherThread *>(this) : NULL;
    if (result == NULL)
    {
        LISTPOS pos = mListComponent.firstPosition();
        while (pos != NULL && result == NULL)
        {
            Component* comObj = mListComponent.getNext(pos);
            ASSERT(comObj != NULL);
            result = comObj->findEventConsumer(whichClass);
        }
    }
    return result;
}

void ComponentThread::shutdownThread( void )
{
    LISTPOS pos = mListComponent.firstPosition();
    while (pos != NULL)
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != NULL);
        comObj->notifyComponentShutdown(self());
        OUTPUT_DBG("The component [ %s ] is notified thread [ %s ] is going to shutdown!", comObj->getRoleName().getString(), getName().getString());
    }
    DispatcherThread::shutdownThread();
}

bool ComponentThread::completionWait( unsigned int waitForCompleteMs /*= Thread::WAIT_INFINITE */ )
{
    LISTPOS pos = mListComponent.firstPosition();
    while ( pos != NULL )
    {
        Component* comObj = mListComponent.getNext(pos);
        ASSERT(comObj != NULL);
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
