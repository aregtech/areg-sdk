/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Event.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
const Event Event::BAD_EVENT(Event::eEventType::EventUnknown);

bool Event::addListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread )
{
    return Event::addListener(classId, eventConsumer, DispatcherThread::getDispatcherThread(whichThread));
}

bool Event::addListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, id_type whichThread )
{
    return Event::addListener( classId, eventConsumer, DispatcherThread::getDispatcherThread( whichThread ) );
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

bool Event::removeListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, id_type whichThread )
{
    return Event::removeListener( classId, eventConsumer, DispatcherThread::getDispatcherThread( whichThread ) );
}

bool Event::removeListener( const RuntimeClassID & classId, IEEventConsumer & eventConsumer, DispatcherThread & dispThread )
{
    return dispThread.isRunning() ? dispThread.unregisterEventConsumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// Event class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

Event::Event( void )
    : RuntimeObject ( )
    , mEventType    ( Event::eEventType::EventUnknown )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::Event( Event::eEventType eventType )
    : RuntimeObject ( )
    , mEventType    ( eventType )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::~Event( void )
{
    mConsumer       = nullptr;
    mTargetThread   = nullptr;
}

inline Event & Event::self( void )
{
    return (*this);
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
    return (mTargetThread != nullptr ? mTargetThread->getEventDispatcher() : DispatcherThread::getCurrentDispatcher());
}

void Event::deliverEvent( void )
{
    EventDispatcher * dispatcher = mTargetThread != nullptr ? &mTargetThread->getEventDispatcher( ) : nullptr;
    if ( (dispatcher != nullptr) && (dispatcher->isReady()) )
    {
        dispatcher->postEvent(*this);
    }
    else
    {
        OUTPUT_ERR("The event target is unknown! Event type [ %s ] is going to be deleted.", getRuntimeClassName().getString());
        destroy();
    }
}

bool Event::registerForThread( id_type whichThread /*= 0*/ )
{
    return registerForThread(whichThread != 0 ? RUNTIME_CAST(Thread::findThreadById(whichThread), DispatcherThread) : 
                                                RUNTIME_CAST(Thread::getCurrentThread(), DispatcherThread));
}

bool Event::registerForThread( const char* whichThread )
{
    return registerForThread(whichThread != nullptr ? RUNTIME_CAST(Thread::findThreadByName(whichThread), DispatcherThread) : nullptr);
}

bool Event::registerForThread( DispatcherThread * dispatchThread )
{
    if ((dispatchThread != nullptr) && dispatchThread->isValid())
    {
        mTargetThread = dispatchThread->isReady() ? dispatchThread : nullptr;
    }

    return (mTargetThread != nullptr);
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
    consumer = consumer != nullptr ? consumer : this->mConsumer;
    if (consumer != nullptr)
    {
        if ( consumer->preprocessEvent(*this) )
        {
            consumer->startEventProcessing(*this);
        }
        else
        {
            OUTPUT_WARN("The Event [ %s ] is interrupted and not going to be processed", getRuntimeClassName().getString());
        }
    }
    else
    {
        OUTPUT_ERR("The Event [ %s ] has invalid consumer. The event cannot be processed!", getRuntimeClassName().getString());
    }
}
