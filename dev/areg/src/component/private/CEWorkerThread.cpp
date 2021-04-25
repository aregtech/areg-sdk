/************************************************************************
 * \file        areg/src/component/private/CEWorkerThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Worker Thread implementation.
 *
 ************************************************************************/
#include "areg/src/component/CEWorkerThread.hpp"

#include "areg/src/component/IEWorkerThreadConsumer.hpp"
#include "areg/src/component/CEEvent.hpp"
#include "areg/src/component/CEComponentThread.hpp"
#include "areg/src/component/CEComponent.hpp"
#include "areg/src/base/NEUtilities.hpp"

//////////////////////////////////////////////////////////////////////////
// CEWorkerThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CEWorkerThread, CEDispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEWorkerThread::CEWorkerThread( const char * threadName, CEComponent & bindingComponent, IEWorkerThreadConsumer & threadConsumer)
    : CEDispatcherThread    ( threadName )

    , mBindingComponent     ( bindingComponent )
    , mWorkerThreadConsumer ( threadConsumer )
{
    ASSERT(NEString::isEmpty<char>(threadName) == false);
}

CEWorkerThread::~CEWorkerThread( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
bool CEWorkerThread::PostEvent( CEEvent& eventElem )
{
    bool result = false;
    if ( (eventElem.GetEventType() & CEEvent::EventCustom) != 0 )
    {
        CEEventDispatcher::PostEvent(eventElem);
    }
    else
    {
        OUTPUT_ERR("Wrong event to post, event type [ %s ], category [ %d ]", eventElem.GetRuntimeClassName(), eventElem.GetEventType());
        eventElem.Destroy();
        ASSERT(false);
    }

    return result;
}

bool CEWorkerThread::RunDispatcher( void )
{
    mWorkerThreadConsumer.RegisterEventConsumers(self(), mBindingComponent.GetMasterThread());
    bool result = CEDispatcherThread::RunDispatcher();
    mWorkerThreadConsumer.UnregisterEventConsumers(self());
    return result;
}

CEDispatcherThread* CEWorkerThread::GetEventConsumerThread( const CERuntimeClassID& whichClass )
{
    return (HasRegisteredConsumer(whichClass) ? static_cast<CEDispatcherThread *>(this) : GetBindingComponent().FindEventConsumer(whichClass));
}

CEComponentThread & CEWorkerThread::GetBindingComponentThread( void ) const
{
    return mBindingComponent.GetMasterThread();
}
