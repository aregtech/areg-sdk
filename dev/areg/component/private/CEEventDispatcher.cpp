/************************************************************************
 * \file        areg/component/private/CEEventDispatcher.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/CEEventDispatcher.hpp"

#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/CEEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcher class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEEventDispatcher::CEEventDispatcher( const char* name )
    : IEThreadConsumer     ( )
    , CEEventDispatcherBase ( name )

    , mDispatcherThread     ( NULL )
{
    ; // do nothing
}

CEEventDispatcher::~CEEventDispatcher( void )
{
    mDispatcherThread   = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcher class, methods
//////////////////////////////////////////////////////////////////////////
bool CEEventDispatcher::ThreadRegistered( CEThread * threadObj )
{
    mDispatcherThread = RUNTIME_CAST(threadObj, CEDispatcherThread);
    ASSERT(mDispatcherThread != NULL);

    CEEventDispatcherBase::RemoveAllEvents( );
    return CEEventDispatcherBase::mEventExit.ResetEvent();
}

void CEEventDispatcher::ThreadUnregistering( void )
{
    StopDispatcher();
    mDispatcherThread   = NULL;
}

void CEEventDispatcher::ThreadRuns( void )
{
    ASSERT(mDispatcherThread != NULL);
    StartDispatcher();
}

int CEEventDispatcher::ThreadExit( void )
{
    StopDispatcher( );
    mDispatcherThread   = NULL;
    return IEThreadConsumer::EXIT_NORMAL;
}

bool CEEventDispatcher::PostEvent( CEEvent& eventElem )
{
    bool result = mDispatcherThread != NULL ? QueueEvent(eventElem) : false;
    if (result == false)
    {
        OUTPUT_ERR("Failed to queue event of type [ %s ], going to destroy", eventElem.GetRuntimeClassName());
        eventElem.Destroy();
    }
    return result;
}

bool CEEventDispatcher::RegisterEventConsumer( const CERuntimeClassID& whichClass, IEEventConsumer& whichConsumer )
{
    return CEEventDispatcherBase::RegisterEventConsumer(whichClass, whichConsumer);
}

bool CEEventDispatcher::UnregisterEventConsumer( const CERuntimeClassID& whichClass, IEEventConsumer& whichConsumer )
{
    return CEEventDispatcherBase::UnregisterEventConsumer(whichClass, whichConsumer);
}

int CEEventDispatcher::RemoveConsumer( IEEventConsumer& whichConsumer )
{
    return CEEventDispatcherBase::RemoveConsumer(whichConsumer);
}

bool CEEventDispatcher::HasRegisteredConsumer( const CERuntimeClassID& whichClass ) const
{
    return CEEventDispatcherBase::HasRegisteredConsumer(whichClass);
}

bool CEEventDispatcher::IsReady( void ) const
{
    return CEEventDispatcherBase::IsReady();
}

void CEEventDispatcher::RemoveEvents( bool keepSpecials )
{
    CEEventDispatcherBase::RemoveEvents(keepSpecials);
}

int CEEventDispatcher::RemoveExternalEventType( const CERuntimeClassID & eventClassId )
{
    return CEEventDispatcherBase::RemoveExternalEventType(eventClassId);
}

void CEEventDispatcher::RemoveAllEvents(void)
{
    CEEventDispatcherBase::RemoveAllEvents( );
}
