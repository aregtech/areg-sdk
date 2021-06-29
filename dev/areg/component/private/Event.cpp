/************************************************************************
 * \file        areg/component/private/Event.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Base class implementation
 *
 ************************************************************************/
#include "areg/component/Event.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Event class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(Event, RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Event class, static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Predefined Bad Event object
 **/
const Event Event::BAD_EVENT(Event::EventUnknown);

bool Event::addListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread )
{
    return Event::addListener(classId, eventConsumer, DispatcherThread::getDispatcherThread(whichThread));
}

bool Event::addListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, DispatcherThread & dispThread )
{
    ASSERT( dispThread.isValid() );
    return dispThread.registerEventConsumer(classId, eventConsumer);
}

bool Event::removeListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread )
{
    return Event::removeListener(classId, eventConsumer, DispatcherThread::getDispatcherThread(whichThread));
}

bool Event::removeListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, DispatcherThread & dispThread )
{
    return dispThread.isRunning() ? dispThread.unregisterEventConsumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// Event class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

Event::Event( void )
    : RuntimeObject   ( )
    , mEventType        ( Event::EventUnknown )
    , mConsumer         ( NULL )
    , mTargetThread     ( NULL )
{
    ; // do nothing
}

Event::Event( Event::eEventType eventType )
    : RuntimeObject   ( )
    , mEventType        ( eventType )
    , mConsumer         ( NULL )
    , mTargetThread     ( NULL )
{
    ; // do nothing
}

Event::~Event( void )
{
    mConsumer       = NULL;
    mTargetThread   = NULL;
}

//////////////////////////////////////////////////////////////////////////
// Event class, methods
//////////////////////////////////////////////////////////////////////////

void Event::destroy( void )
{
    delete this;
}

EventDispatcher& Event::getDispatcher( void ) const
{
    return (mTargetThread != NULL ? mTargetThread->getEventDispatcher() : DispatcherThread::getCurrentDispatcher());
}

void Event::sendEvent( void )
{
    if ((mTargetThread != NULL) && mTargetThread->isRunning())
    {
        mTargetThread->getEventDispatcher().postEvent(*this);
    }
    else
    {
        OUTPUT_ERR("The event target is unknown! Event type [ %s ] is going to be deleted.", getRuntimeClassName());
        destroy();
    }
}

bool Event::registerForThread( ITEM_ID whichThread /*= 0*/ )
{
    return registerForThread(whichThread != 0 ? RUNTIME_CAST(Thread::findThreadById(whichThread), DispatcherThread) : 
                                                RUNTIME_CAST(Thread::getCurrentThread(), DispatcherThread));
}

bool Event::registerForThread( const char* whichThread )
{
    return registerForThread(whichThread != NULL ? RUNTIME_CAST(Thread::findThreadByName(whichThread), DispatcherThread) : NULL);
}

bool Event::registerForThread( DispatcherThread * dispatchThread )
{
    if ((dispatchThread != NULL) && dispatchThread->isValid())
    {
        mTargetThread = dispatchThread->isReady() ? dispatchThread : NULL;
    }

    return (mTargetThread != NULL);
}

bool Event::isEventRegistered( void ) const
{
    return getDispatcher().hasRegisteredConsumer(getRuntimeClassId());
}

bool Event::addEventListener( IEEventConsumer& eventConsumer )
{
    return getDispatcher().registerEventConsumer(getRuntimeClassId(), eventConsumer);
}

bool Event::removeEventListener( IEEventConsumer& eventConsumer )
{
    return getDispatcher().unregisterEventConsumer(getRuntimeClassId(), eventConsumer);
}

void Event::dispatchSelf( IEEventConsumer* consumer )
{
    consumer = consumer != NULL ? consumer : this->mConsumer;
    if (consumer != NULL)
    {
        if ( consumer->preprocessEvent(*this) )
        {
            consumer->startEventProcessing(*this);
        }
        else
        {
            OUTPUT_WARN("The Event [ %s ] is interrupted and not going to be processed", getRuntimeClassName());
        }
    }
    else
    {
        OUTPUT_ERR("The Event [ %s ] has invalid consumer. The event cannot be processed!", getRuntimeClassName());
    }
}
