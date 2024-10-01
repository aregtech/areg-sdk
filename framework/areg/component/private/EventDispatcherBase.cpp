/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcherBase.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Dispatcher Base class
 *
 ************************************************************************/
#include "areg/component/private/EventDispatcherBase.hpp"

#include "areg/component/Event.hpp"
#include "areg/component/IEEventConsumer.hpp"
#include "areg/component/private/ExitEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
EventDispatcherBase::EventDispatcherBase(const String & name )
    : IEEventDispatcher( )
    , IEQueueListener  ( )

    , mDispatcherName   ( name )
    , mExternaEvents    ( static_cast<IEQueueListener &>(self()) )
    , mInternalEvents   ( )
    , mConsumerMap      ( )
    , mEventExit        ( false, false )
    , mEventQueue       ( true, false )
    , mHasStarted       ( false )
{
}

EventDispatcherBase::~EventDispatcherBase( void )
{
    mHasStarted = false;
}

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class, methods
//////////////////////////////////////////////////////////////////////////

bool EventDispatcherBase::isExitEvent( const Event * anEvent ) const
{
    return (anEvent == static_cast<const Event *>(&ExitEvent::getExitEvent( )));
}

void EventDispatcherBase::signalEvent( uint32_t eventCount )
{
    eventCount != 0 ? mEventQueue.setEvent() : mEventQueue.resetEvent();
}

bool EventDispatcherBase::startDispatcher( void )
{
    mEventExit.resetEvent( );
    return runDispatcher( );
}

void EventDispatcherBase::stopDispatcher( void )
{
    mExternaEvents.lockQueue( );
    if ( mHasStarted )
    {
        removeEvents( true );
        mExternaEvents.pushEvent( ExitEvent::getExitEvent( ) );
    }

    mEventExit.setEvent( );
    mExternaEvents.unlockQueue( );
}

void EventDispatcherBase::exitDispatcher(void)
{
    mInternalEvents.removeAllEvents();
    mExternaEvents.removeAllEvents();

    mEventExit.setEvent();
}

void EventDispatcherBase::shutdownDispatcher( void )
{
    mExternaEvents.lockQueue( );
    if ( mHasStarted )
    {
        removeEvents( true );
        mExternaEvents.pushEvent(ExitEvent::getExitEvent());
    }

    mEventExit.setEvent( );
    mExternaEvents.unlockQueue( );
}

bool EventDispatcherBase::queueEvent( Event& eventElem )
{
    bool result{ false };
    if ( mHasStarted )
    {
        Event::eEventType eventType = eventElem.getEventType();
        if (Event::isInternal(eventType))
        {
            mInternalEvents.pushEvent(eventElem);
            result = true;
        }
        else if (Event::isExternal(eventType))
        {
            mExternaEvents.pushEvent(eventElem);
            result = true;
        }
    }

    return result;
}

bool EventDispatcherBase::registerEventConsumer( const RuntimeClassID& whichClass, IEEventConsumer& whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    EventConsumerList* listConsumers = mConsumerMap.findResourceObject(whichClass);
    if (listConsumers == nullptr)
    {
        listConsumers   = DEBUG_NEW EventConsumerList();
        if (listConsumers != nullptr)
            mConsumerMap.registerResourceObject(whichClass, listConsumers);
    }

    if ( (listConsumers != nullptr) && (listConsumers->existConsumer(whichConsumer) == false) )
    {
        result = listConsumers->addConsumer(whichConsumer);
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    EventConsumerList* listConsumers = mConsumerMap.findResourceObject(whichClass);
    if (listConsumers != nullptr)
    {
        result = listConsumers->removeConsumer(whichConsumer);
        if (listConsumers->isEmpty())
        {
            mConsumerMap.unregisterResourceObject(whichClass);
            delete listConsumers;
        }
    }
    else
    {
        // AAvetyan:    The reason why it does not find, because it is cleaned in _clean() function.
        //              This is mainly happening in component, which has server interface implementation.
        //              To make graceful shutdown, in _clean() method should be set filtering.
        //              But the consumer map indeed at this point is empty and the consumer is unregistered.
        mConsumerMap.unregisterResourceObject(whichClass);
    }

    mConsumerMap.unlock();
    return result;
}


int EventDispatcherBase::removeConsumer( IEEventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    int result = 0;
    TELinkedList<RuntimeClassID> removedList;
    RuntimeClassID     Key(RuntimeClassID::createEmptyClassID());
    EventConsumerList* Value = nullptr;

    Value = mConsumerMap.resourceFirstKey(Key);
    while (Value != nullptr)
    {
        ASSERT(Value->isEmpty() == false);
        result += Value->removeConsumer(whichConsumer) ? 1 : 0;
        if (Value->isEmpty())
        {
            removedList.pushFirst(Key);
        }

        Value = mConsumerMap.resourceNextKey(Key);
    }

    while (removedList.removeLast(Key))
    {
        Value   = mConsumerMap.unregisterResourceObject(Key);
        ASSERT(Value != nullptr);
        delete Value;
        Value = nullptr;
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::runDispatcher( void )
{
    readyForEvents( true );

    IESynchObject* syncObjects[2] {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);
    int whichEvent  = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    const ExitEvent& exitEvent = ExitEvent::getExitEvent();

    do 
    {
        whichEvent = multiLock.lock(NECommon::WAIT_INFINITE, false);
        Event* eventElem = whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) ? pickEvent() : nullptr;
        if ( static_cast<const Event *>(eventElem) != static_cast<const Event *>(&exitEvent) )
        {
            if ( whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) )
            {
                if (eventElem == nullptr)
                {
                    continue;
                }

                do 
                {
                    // proceed one external event.
                    if (prepareDispatchEvent(eventElem) )
                    {
                        dispatchEvent(*eventElem);
                    }

                    postDispatchEvent(eventElem);

                    // proceed all internal events after external.
                    // needed for notifications. For example in case of Proxy.
                    // But before popping internal event from stack, check whether
                    // there is no request to exit thread.
                    eventElem = nullptr;
                    int eventLock = multiLock.lock(NECommon::DO_NOT_WAIT);
                    if ( eventLock == MultiLock::LOCK_INDEX_TIMEOUT ||  eventLock == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue) )
                    {
                        eventElem = static_cast<EventQueue &>(mInternalEvents).isEmpty() == false ? mInternalEvents.popEvent() : nullptr;
                    }

                } while (eventElem != nullptr);
            }
        }
        else
        {
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventExit);
        }

    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue));

    readyForEvents(false);
    removeAllEvents( );
    _clean();

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}

void EventDispatcherBase::readyForEvents( bool isReady )
{
    mExternaEvents.lockQueue( );
    mHasStarted = isReady;
    mExternaEvents.unlockQueue( );
}

Event* EventDispatcherBase::pickEvent( void )
{
    return mExternaEvents.popEvent();
}

bool EventDispatcherBase::prepareDispatchEvent( Event* eventElem )
{
    return (eventElem != nullptr);
}

void EventDispatcherBase::postDispatchEvent( Event* eventElem )
{
    if (eventElem != nullptr)
    {
        eventElem->destroy();
    }
}

bool EventDispatcherBase::dispatchEvent( Event& eventElem )
{
    EventConsumerList processingList;
    IEEventConsumer* consumer = eventElem.getEventConsumer();
    if ( consumer != nullptr)
    {
        processingList.pushFirst(consumer);
    }
    else
    {
        // Lock resource, before get any information
        mConsumerMap.lock();

        EventConsumerList* listConsumers = mConsumerMap.findResourceObject(eventElem.getRuntimeClassId());
        if (listConsumers != nullptr)
            processingList = *listConsumers;

        // Unlock resource as soon as possible. Otherwise we may block other threads
        // on registering / unregistering consumer.
        mConsumerMap.unlock();
    }

    EventConsumerList::LISTPOS pos = processingList.firstPosition();
    while (processingList.isValidPosition(pos))
    {
        consumer = processingList.getNext(pos);
        eventElem.dispatchSelf(consumer);
        consumer = nullptr;
    }

    return (processingList.isEmpty() == false);
}

bool EventDispatcherBase::hasRegisteredConsumer( const RuntimeClassID& whichClass ) const
{
    return mConsumerMap.existResource(whichClass);
}

inline void EventDispatcherBase::_clean()
{
    mConsumerMap.lock();

    RuntimeClassID     Key(RuntimeClassID::createEmptyClassID());
    while (mConsumerMap.isEmpty() == false)
    {
        mConsumerMap.resourceFirstKey(Key);
        EventConsumerList* Value =  mConsumerMap.unregisterResourceObject(Key);
        Value->removeAllConsumers();
        delete Value;
    }

    mConsumerMap.unlock();
}

bool EventDispatcherBase::pulseExit(void)
{
    return mEventExit.setEvent();
}
