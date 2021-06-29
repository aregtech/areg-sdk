/************************************************************************
 * \file        areg/component/private/WorkerThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Worker Thread implementation.
 *
 ************************************************************************/
#include "areg/component/WorkerThread.hpp"

#include "areg/component/IEWorkerThreadConsumer.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Component.hpp"
#include "areg/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// WorkerThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(WorkerThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
WorkerThread::WorkerThread( const char * threadName, Component & bindingComponent, IEWorkerThreadConsumer & threadConsumer)
    : DispatcherThread    ( threadName )

    , mBindingComponent     ( bindingComponent )
    , mWorkerThreadConsumer ( threadConsumer )
{
    ASSERT(NEString::isEmpty<char>(threadName) == false);
}

WorkerThread::~WorkerThread( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool WorkerThread::postEvent( Event& eventElem )
{
    bool result = false;
    if ( (eventElem.getEventType() & Event::EventCustom) != 0 )
    {
        EventDispatcher::postEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Wrong event to post, event type [ %s ], category [ %d ]", eventElem.getRuntimeClassName(), eventElem.getEventType());
        eventElem.destroy();
        ASSERT(false);
    }

    return result;
}

bool WorkerThread::runDispatcher( void )
{
    mWorkerThreadConsumer.registerEventConsumers(self(), mBindingComponent.getMasterThread());
    bool result = DispatcherThread::runDispatcher();
    mWorkerThreadConsumer.unregisterEventConsumers(self());
    return result;
}

DispatcherThread* WorkerThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    return (hasRegisteredConsumer(whichClass) ? static_cast<DispatcherThread *>(this) : getBindingComponent().findEventConsumer(whichClass));
}

ComponentThread & WorkerThread::getBindingComponentThread( void ) const
{
    return mBindingComponent.getMasterThread();
}
