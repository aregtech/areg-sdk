#ifndef AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
#define AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/EventDispatcherBase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Global Areg Platform, Event Dispatcher base class.
 *              The class is providing basic dispatching functionalities,
 *              keeps track of queued events and registered event consumers,
 *              starts and stops event dispatcher, triggers Event Processing.
 *              The Event Dispatcher is Thread blocker, it runs and pick up
 *              event objects from queue in the loop, until do not get request
 *              to stop dispatcher. To stop Dispatcher and release Thread,
 *              call stop_dispatcher() function or it will be automatically
 *              triggered in Dispatcher Thread when request to stop thread
 *              is received.
 *
 ************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/private/QueueListener.hpp"

#include "areg/component/private/EventConsumerMap.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/component/private/MpscEventQueue.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class DispatcherThread;
} // namespace areg

namespace areg {

using ExternalQueue = areg::MpscEventQueue;

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Provides event dispatching basic functionalities. It is an implementation of Event Queue
 *          Listener and Event Dispatcher interfaces. Class tracks Event queue to be able to pickup
 *          events from queue and finish executing when receives Quit request. Every Dispatcher
 *          thread should contain one Event Dispatcher running in Loop. Class contains map of
 *          registered Event Consumers to trigger Event Processing.
 **/
class AREG_API EventDispatcherBase  : protected QueueListener
{
//////////////////////////////////////////////////////////////////////////
// Internal defines and constants.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   EventDispatcherBase::EventSignal
     *          Used in main loop to identify event signal.
     **/
    enum class EventSignal : int32_t
    {
          Error = -1    //!< Error happened during waiting for event
        , Exit  =  0    //!< Exit event has been signaled.
        , Queue =  1    //!< Queue event has been signaled.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor. Protected.
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Initializes Dispatcher and assigns name.
     *
     * \param   name            The name of Dispatcher.
     * \param   maxQeueue       The maximum number of event elements in the queue.
     **/
    EventDispatcherBase( const String & name, uint32_t maxQeueue );

    virtual ~EventDispatcherBase();

//////////////////////////////////////////////////////////////////////////
// Public overrides
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Starts dispatcher. Returns true if successfully started.
     **/
    virtual bool start_dispatcher();

    /**
     * \brief   Stops running dispatcher.
     **/
    virtual void stop_dispatcher() noexcept;

    /**
     * \brief   Called when dispatcher completed the job and exit. The cleanups should be done here.
     **/
    virtual void exit_dispatcher() noexcept;

    /**
     * \brief   Call to register specified event consumer for the specified event class type. One
     *          event class type can have several registered consumers and one consumer can be
     *          registered for several class types.
     *
     * \param   whichClass          The runtime class ID object of an event to register consumer.
     * \param   whichConsumer       Reference to event consumer object to register as an event
     *                              listener.
     * \return  Return true if consumer has been registered with success.
     **/
    virtual bool register_event_consumer(const uint32_t whichClass, EventConsumer& whichConsumer);

    /**
     * \brief   Call to unregister specified event consumer previously registered for specified
     *          event class type.
     *
     * \param   whichClass          The runtime class ID of event object.
     * \param   whichConsumer       Reference to consumer that should be unregistered.
     * \return  Returns true if successfully unregistered event consumer.
     **/
    virtual bool unregister_event_consumer(const uint32_t whichClass, EventConsumer& whichConsumer);

    /**
     * \brief   Call to remove specified consumer for all registered event class types.
     *
     * \param   whichConsumer       Reference to consumer object to unregister.
     * \return  Returns unregister count.
     **/
    virtual int32_t  remove_consumer(EventConsumer& whichConsumer);

    /**
     * \brief   Call to check whether specified event class type has any registered consumer.
     *
     * \param   whichClass      Runtime class ID to be checked.
     * \return  Returns true if dispatcher has at least one registered consumer.
     **/
    [[nodiscard]]
    virtual bool has_registered_consumer(const uint32_t whichClass) const;

    /**
     * \brief   Call to queue event object in the event queue of dispatcher. The passed event
     *          parameter should be allocated in memory and should be globally accessed (for
     *          example, via new operator).
     *
     * \param   eventElem       Event object to queue in event stack of dispatcher.
     * \return  Returns true, if Event was queued. Otherwise, it was not queued and the object
     *          should be deleted.
     **/
    bool queue_event(Event& eventElem);

    /**
     * \brief   Pushes a batch of events (by move) with a single priority-lane lock acquisition.
     *          Events that cannot be enqueued due to capacity overflow remain in their slots and
     *          the overflow count is returned.
     *
     * \param[in,out]   listEvents  Array of Event values sorted by priority. Overflow events are
     *                              compacted to the front on output.
     * \param           count       Number of events in \a listEvents.
     * \return  Number of events NOT enqueued due to capacity overflow. 0 = all enqueued.
     **/
    uint32_t queue_events(Event* listEvents, uint32_t count);

    /**
     * \brief   Pops up to \a count events into the caller-supplied array (by move).
     *          Returns the number of events actually dequeued.
     *
     * \param[out]  listEvents  Caller array receiving dequeued events; highest priority first.
     * \param       count       Array capacity.
     * \return  Number of events filled.
     **/
    uint32_t pop_events(Event* listEvents, uint32_t count) noexcept;

    /**
     * \brief   Picks up a single Event from the external queue.
     *          Returns an invalid Event (is_valid() == false) when the queue is empty.
     *          The ExitEvent is returned as a value copy (check with is_exit_prio()).
     **/
    [[nodiscard]]
    Event pick_event() noexcept;

    /**
     * \brief   Sets exit event in the dispatcher without blocking.
     **/
    bool pulse_exit();

/************************************************************************/
// EventDispatcherBase operations
/************************************************************************/
    /**
     * \brief   Returns true if dispatcher is ready to receive events.
     **/
    [[nodiscard]]
    inline bool is_ready() const noexcept;

    /**
     * \brief   Removes all internal events, and all external events except exit events.
     *
     * \param   keepSpecials    If true, keeps special reserved events.
     **/
    inline void remove_events( bool keepSpecials ) noexcept;

    /**
     * \brief   Removes all events. Makes event queue empty.
     **/
    inline void remove_all_events() noexcept;

    /**
     * \brief   Removes specified event type from external event queue.
     *
     * \param   eventClassId    The class ID of external event objects to remove.
     **/
    inline void remove_event_type(const uint32_t eventClassId) noexcept;

    /**
     * \brief   Returns true if the specified event object is a special reserved exit event.
     *
     * \param   anEvent     A pointer to the event object to check.
     **/
    [[nodiscard]]
    bool is_exit_event( const Event * anEvent ) const;

//////////////////////////////////////////////////////////////////////////
// Protected overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// QueueListener overrides
/************************************************************************/

    /**
     * \brief   Triggered from Event Queue every time when new event element is pushed into
     *          queue or when queue is empty.
     *
     * \param   eventCount      The number of event elements currently in the queue.
     **/
    inline void signal_event(uint32_t eventCount) final;

/************************************************************************/
// EventDispatcherBase overrides
/************************************************************************/

    /**
     * \brief   Dispatches the event to the appropriate registered consumer.
     **/
    virtual bool dispatch_event( Event & eventElem );

    /**
     * \brief   Called before dispatching. Returns false to drop the event.
     **/
    virtual bool prepare_dispatch_event( Event & eventElem ) noexcept;

    /**
     * \brief   Called after dispatching. Releases the event's buffer (no pointer delete needed
     *          since Event is now a value type returned from pick_event()).
     **/
    virtual void post_dispatch_event( Event & eventElem );

    /**
     * \brief   Runs the main dispatching loop.
     **/
    virtual bool run_dispatcher();

    /**
     * \brief   Notifies exit event to shutdown dispatcher.
     **/
    virtual void shutdown_dispatcher() noexcept;

    /**
     * \brief   Enables or disables event dispatching.
     **/
    virtual void ready_for_events( bool is_ready );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   The name of dispatcher
     **/
    String              mDispatcherName;

    /**
     * \brief   External Event Queue element.
     *          Selected at compile time: MpscEventQueue (default) or ExternalEventQueue.
     *          Multiple producer threads may push; only the owner thread pops.
     **/
    ExternalQueue       mExternalEvents;

    /**
     * \brief   Internal Event Queue element. One Internal queue per one dispatcher.
     *          Non-locking queue, only current owning thread can queue elements.
     **/
    InternalEventQueue  mInternalEvents;

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER

    /**
     * \brief   Flag indicating whether the dispatcher is in the dispatching loop.
     **/
    std::atomic<bool>   mHasStarted;

    /**
     * \brief   Map of registered consumers.
     **/
    EventConsumerMap    mConsumerMap;

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER

    /**
     * \brief   Exit Synchronization Event.
     *          Signaled when dispatcher should be stopped.
     **/
    SyncEvent           mEventExit;

    /**
     * \brief   Queue Synchronization Event.
     *          Signaled when new event is pushed; reset when queue is empty.
     **/
    SyncEvent           mEventQueue;

//////////////////////////////////////////////////////////////////////////
// Hidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    inline EventDispatcherBase & self() noexcept;
    void _clean() noexcept;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    EventDispatcherBase() = delete;
    AREG_NOCOPY_NOMOVE( EventDispatcherBase );
};

//////////////////////////////////////////////////////////////////////////
// EventDispatcherBase class inline implementation
//////////////////////////////////////////////////////////////////////////

inline uint32_t EventDispatcherBase::queue_events(Event* listEvents, uint32_t count)
{
    return mExternalEvents.push_events(listEvents, count);
}

inline uint32_t EventDispatcherBase::pop_events(Event* listEvents, uint32_t count) noexcept
{
    return mExternalEvents.pop_events(listEvents, count);
}

inline void EventDispatcherBase::signal_event(uint32_t eventCount)
{
    if (eventCount == 1)
        mEventQueue.set_signaled();
    else if (eventCount == 0)
        mEventQueue.reset();
}

inline bool EventDispatcherBase::is_ready() const noexcept
{
    return mHasStarted;
}

inline void EventDispatcherBase::remove_events(bool /*keepSpecials*/) noexcept
{
    mExternalEvents.lock_queue();
    mInternalEvents.remove_events( );
    mExternalEvents.remove_events( );
    mExternalEvents.unlock_queue();
}

inline void EventDispatcherBase::remove_all_events() noexcept
{
    mExternalEvents.lock_queue();
    mInternalEvents.remove_all_events( );
    mExternalEvents.remove_all_events( );
    mExternalEvents.unlock_queue();
}

inline void EventDispatcherBase::remove_event_type( const uint32_t eventClassId ) noexcept
{
    mExternalEvents.remove_events(eventClassId);
}

inline EventDispatcherBase& EventDispatcherBase::self() noexcept
{
    return (*this);
}

} // namespace areg
#endif  // AREG_COMPONENT_PRIVATE_EVENTDISPATCHERBASE_HPP
