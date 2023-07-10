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
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
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
void EventDispatcherBase::signalEvent( uint32_t eventCount )
{
    eventCount != 0 ? mEventQueue.setEvent() : mEventQueue.resetEvent();
}

bool EventDispatcherBase::startDispatcher( void )
{
    bool result = true;
    if (mHasStarted == false)
    {
        mHasStarted = true;
        mEventExit.resetEvent();
        result      = runDispatcher();
    }

    return result;
}

void EventDispatcherBase::stopDispatcher( void )
{
    mExternaEvents.lockQueue();
    removeEvents(false);
    mExternaEvents.pushEvent(ExitEvent::getExitEvent());
    mHasStarted = false;
    mExternaEvents.unlockQueue();
    mEventExit.setEvent();
}

void EventDispatcherBase::exitDispatcher(void)
{
    mInternalEvents.removeAllEvents();
    mExternaEvents.removeAllEvents();

    mHasStarted = false;
    mEventExit.setEvent();
}

void EventDispatcherBase::shutdownDispatcher( void )
{
    if ( mHasStarted )
    {
        mExternaEvents.lockQueue();
        removeEvents( true );
        mExternaEvents.pushEvent(ExitEvent::getExitEvent());
        mHasStarted = false;
        mExternaEvents.unlockQueue();
    }
}

bool EventDispatcherBase::queueEvent( Event& eventElem )
{
    bool result = false;
    if ( mHasStarted )
    {
        result = true;
        Event::eEventType eventType = eventElem.getEventType();
        if ( Event::isInternal(eventType) )
            mInternalEvents.pushEvent(eventElem);
        else if ( Event::isExternal(eventType) )
            mExternaEvents.pushEvent(eventElem);
        else
            result = false;
    }

    return result;
}

bool EventDispatcherBase::registerEventConsumer( const RuntimeClassID& whichClass, IEEventConsumer& whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    OUTPUT_DBG("[ %s ] dispatcher: Registers consumer [ %p ] for event [ %s ]. The dispatcher state is [ %s ]"
                , mDispatcherName.getString()
                , reinterpret_cast<void *>(&whichConsumer)
                , whichClass.getName().getString()
                , isReady() ? "Ready" : "Not ready");

    EventConsumerList* listConsumers = mConsumerMap.findResourceObject(whichClass);
    if (listConsumers == nullptr)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Did not find consumer list for event [ %s ], creating new"
                        , mDispatcherName.getString()
                        , whichClass.getName().getString());
        
        listConsumers   = DEBUG_NEW EventConsumerList();
        if (listConsumers != nullptr)
            mConsumerMap.registerResourceObject(whichClass, listConsumers);
    }
#ifdef DEBUG
    else
    {
        OUTPUT_DBG("[ %s ] dispatcher: Fount consumer list for event [ %s ]"
                        , mDispatcherName.getString()
                        , whichClass.getName().getString());
    }
#endif // DEBUG

    if ( (listConsumers != nullptr) && (listConsumers->existConsumer(whichConsumer) == false) )
    {
        OUTPUT_DBG("[ %s ] dispatcher: Add new consumer. There are [ %d ] registered consumers for event [ %s ]. There are [ %d ] Consumer Lists in map"
                    , mDispatcherName.getString()
                    , listConsumers->getSize()
                    , whichClass.getName().getString()
                    , mConsumerMap.getSize());

        result = listConsumers->addConsumer(whichConsumer);
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer )
{
    mConsumerMap.lock();

    bool result = false;
    OUTPUT_DBG("[ %s ] dispatcher: Going to unregister consumer [ %p ] for event class [ %s ]"
                , mDispatcherName.getString()
                , static_cast<void *>(&whichConsumer)
                , whichClass.getName().getString());

    EventConsumerList* listConsumers = mConsumerMap.findResourceObject(whichClass);
    if (listConsumers != nullptr)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Found Consumer List for event class [ %s ], going to remove consumer. Number of consumers in list [ %d ]"
                    , mDispatcherName.getString()
                    , whichClass.getName().getString()
                    , listConsumers->getSize());

        result = listConsumers->removeConsumer(whichConsumer);
        if (listConsumers->isEmpty())
        {
            mConsumerMap.unregisterResourceObject(whichClass);
            delete listConsumers;
            OUTPUT_DBG("[ %s ] dispatcher: Consumer List of event [ %s ] is empty, unregistered from map and deleted. Still [ %d ] lists in map"
                        , mDispatcherName.getString()
                        , whichClass.getName().getString()
                        , mConsumerMap.getSize());
        }
    }
    else
    {
        OUTPUT_WARN("[ %s ] dispatcher: Did not find registered consumer list for event [ %s ]", mDispatcherName.getString(), whichClass.getName().getString());
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

    OUTPUT_DBG("[ %s ] dispatcher: Removing Consumer [ %p ] from Consumer Map", mDispatcherName.getString(), static_cast<void *>(&whichConsumer));

    int result = 0;
    TELinkedList<RuntimeClassID> removedList;
    RuntimeClassID     Key(RuntimeClassID::createEmptyClassID());
    EventConsumerList* Value = nullptr;

    Value = mConsumerMap.resourceFirstKey(Key);
    while (Value != nullptr)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Found registered event entry [ %s ] for consumer [ %p ]"
                    , mDispatcherName.getString()
                    , Key.getName().getString()
                    , static_cast<void*>(&whichConsumer));
        ASSERT(Value->isEmpty() == false);

        result += Value->removeConsumer(whichConsumer) ? 1 : 0;
        if (Value->isEmpty())
        {
            OUTPUT_WARN("[ %s ] dispatcher: The Consumer List of event entry [ %s ] is empty, marking for deleting"
                            , mDispatcherName.getString()
                            , Key.getName().getString());
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

        OUTPUT_WARN("[ %s ] dispatcher: Unregistered and deleted Consumer List of event entry [ %s ]. There are still [ %d ] Consumer Lists in map"
                        , mDispatcherName.getString()
                        , Key.getName().getString()
                        , mConsumerMap.getSize());
    }

    mConsumerMap.unlock();
    return result;
}

bool EventDispatcherBase::runDispatcher( void )
{
    IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
    MultiLock multiLock(syncObjects, 2, false);

    int whichEvent  = static_cast<int>(EventDispatcherBase::eEventOrder::EventError);
    const ExitEvent& exitEvent = ExitEvent::getExitEvent();

    readyForEvents(true);

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
                    OUTPUT_WARN("The event object is nullptr. Ignoring and waiting for lock.");
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
            else
            {
                OUTPUT_WARN(">>> Going to exit [ %s ] dispatcher", static_cast<const char *>(mDispatcherName.getString()));
            }
        }
        else
        {
            whichEvent = static_cast<int>(EventDispatcherBase::eEventOrder::EventExit);
            OUTPUT_WARN("Received exit event. Going to exit [ %s ] dispatcher", static_cast<const char *>(mDispatcherName.getString()));
        }
    } while (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventQueue));

    readyForEvents(false);
    mHasStarted = false;
    removeAllEvents( );
    _clean();

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.getString());

    return (whichEvent == static_cast<int>(EventDispatcherBase::eEventOrder::EventExit));
}

void EventDispatcherBase::readyForEvents( bool /* isReady */ )
{
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

    OUTPUT_WARN("[ %s ] dispatcher: Cleaning up, there are [ %d ] registered consumer maps", mDispatcherName.getString(), mConsumerMap.getSize());
    RuntimeClassID     Key(RuntimeClassID::createEmptyClassID());
    while (mConsumerMap.isEmpty() == false)
    {
        mConsumerMap.resourceFirstKey(Key);
        EventConsumerList* Value =  mConsumerMap.unregisterResourceObject(Key);
        Value->removeAllConsumers();
        delete Value;

        OUTPUT_WARN("[ %s ] dispatcher: Unregistered and deleted Consumer List of event entry [ %s ]. There are still [ %d ] Consumer Lists in map"
                        , mDispatcherName.getString()
                        , Key.getName().getString()
                        , mConsumerMap.getSize());
    }

    mConsumerMap.unlock();
}

bool EventDispatcherBase::pulseExit(void)
{
    return mEventExit.setEvent();
}
