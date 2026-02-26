/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Event.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Base class implementation
 *
 ************************************************************************/
#include "areg/component/Event.hpp"

#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// Event class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Event class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME_EVENT(Event, areg::RuntimeObject)

//////////////////////////////////////////////////////////////////////////
// Event class, static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Predefined Bad Event object
 **/
const Event Event::BAD_EVENT(Event::EventType::EventUnknown);

bool Event::addListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, const areg::String & whichThread )
{
    return Event::addListener(classId, eventConsumer, areg::DispatcherThread::getDispatcherThread(whichThread));
}

bool Event::addListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, id_type whichThread )
{
    return Event::addListener( classId, eventConsumer, areg::DispatcherThread::getDispatcherThread( whichThread ) );
}

bool Event::addListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, areg::DispatcherThread & dispThread )
{
    return ( dispThread.isRunning() ? dispThread.registerEventConsumer(classId, eventConsumer) : false );
}

bool Event::removeListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, const areg::String & whichThread )
{
    return Event::removeListener(classId, eventConsumer, areg::DispatcherThread::getDispatcherThread(whichThread));
}

bool Event::removeListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, id_type whichThread )
{
    return Event::removeListener( classId, eventConsumer, areg::DispatcherThread::getDispatcherThread( whichThread ) );
}

bool Event::removeListener( const areg::RuntimeClassID & classId, areg::EventConsumer & eventConsumer, areg::DispatcherThread & dispThread )
{
    return dispThread.isRunning() ? dispThread.unregisterEventConsumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// Event class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

Event::Event()
    : areg::RuntimeObject ( )
    , mEventType    ( Event::EventType::EventUnknown )
    , mEventPrio    ( DefaultPriority )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::Event( Event::EventType eventType )
    : areg::RuntimeObject ( )
    , mEventType    ( eventType )
    , mEventPrio    ( DefaultPriority )
    , mConsumer     ( nullptr )
    , mTargetThread ( nullptr )
{
}

Event::~Event()
{
    mConsumer       = nullptr;
    mTargetThread   = nullptr;
}

inline Event & Event::self()
{
    return (*this);
}

//////////////////////////////////////////////////////////////////////////
// Event class, methods
//////////////////////////////////////////////////////////////////////////

void Event::destroy()
{
    delete this;
}

EventDispatcher& Event::getDispatcher() const
{
    return (mTargetThread != nullptr ? mTargetThread->getEventDispatcher() : areg::DispatcherThread::getCurrentDispatcher());
}

void Event::deliverEvent()
{
    EventDispatcher * dispatcher = mTargetThread != nullptr ? &mTargetThread->getEventDispatcher( ) : nullptr;
    if ((dispatcher == nullptr) || (dispatcher->postEvent(*this) == false))
    {
        destroy();
    }
}

bool Event::registerForThread( id_type whichThread /*= 0*/ )
{
    return registerForThread(whichThread != 0 ? AREG_RUNTIME_CAST(areg::Thread::findThreadById(whichThread), areg::DispatcherThread)
                                              : AREG_RUNTIME_CAST(areg::Thread::getCurrentThread(), areg::DispatcherThread));
}

bool Event::registerForThread( const char* whichThread )
{
    return registerForThread(whichThread != nullptr ? AREG_RUNTIME_CAST(areg::Thread::findThreadByName(whichThread), areg::DispatcherThread) : nullptr);
}

bool Event::registerForThread( areg::DispatcherThread * dispatchThread )
{
    if ((dispatchThread != nullptr) && dispatchThread->isValid())
    {
        mTargetThread = dispatchThread->isReady() ? dispatchThread : nullptr;
    }

    return (mTargetThread != nullptr);
}

bool Event::isEventRegistered() const
{
    return getDispatcher().hasRegisteredConsumer(getRuntimeClassId());
}

bool Event::addEventListener( areg::EventConsumer& eventConsumer )
{
    return getDispatcher().registerEventConsumer(getRuntimeClassId(), eventConsumer);
}

bool Event::removeEventListener( areg::EventConsumer& eventConsumer )
{
    return getDispatcher().unregisterEventConsumer(getRuntimeClassId(), eventConsumer);
}

void Event::dispatchSelf( areg::EventConsumer* consumer )
{
    consumer = consumer != nullptr ? consumer : this->mConsumer;
    if ((consumer != nullptr) && consumer->preprocessEvent(*this) )
    {
        consumer->startEventProcessing(*this);
    }
}
