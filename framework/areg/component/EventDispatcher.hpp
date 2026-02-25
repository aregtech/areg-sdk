#ifndef AREG_COMPONENT_EVENTDISPATCHER_HPP
#define AREG_COMPONENT_EVENTDISPATCHER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/EventDispatcher.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher class
 *              Posts, receives dispatcher event objects, registers
 *              event consumers in dispatcher registry and triggers
 *              appropriate event processing function of event consumer.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/EventRouter.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"

//////////////////////////////////////////////////////////////////////////
// EventDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Component of Dispatcher Thread to queue, dispatch, and process events in a thread. Runs
 *          in a loop until exit event is received.
 **/
class AREG_API EventDispatcher  : public    EventDispatcherBase
                                , public    ThreadConsumer
                                , public    EventRouter
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Creates EventDispatcher with specified name and maximum queue size.
     *
     * \param   name            The name of Event Dispatcher
     * \param   maxQeueue       The maximum number of queued external events.
     **/
    explicit EventDispatcher( const String & name, uint32_t maxQeueue);
    /**
     * \brief   Destructor.
     **/
    virtual ~EventDispatcher();

//////////////////////////////////////////////////////////////////////////
// Override operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when thread object has been created. Returns true if thread should
     *          continue running; false if not.
     *
     * \param   threadObj       The new created Thread object, which contains this consumer.
     * \return  Return true if thread should run. Return false, it should not run.
     **/
    bool on_thread_registered( Thread * threadObj ) override;

    /**
     * \brief   Triggered when thread object is being destroyed.
     **/
    void on_thread_unregistering() override;

    /**
     * \brief   Triggered when thread is running and fully operational.
     **/
    void on_thread_runs() override;

    /**
     * \brief   Triggered when thread is going to exit.
     *
     * \return  Return thread exit error code.
     **/
    int32_t on_thread_exit() override;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers it to its target thread or process.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true if target was found and the event delivered successfully. Otherwise
     *          returns false.
     **/
    bool post_event(Event& eventElem) override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Returns pointer to Dispatcher Thread where current dispatcher is registered.
     **/
    inline DispatcherThread * dispatcher_thread() const;

protected:
    /**
     * \brief   Returns true if dispatcher has more queued external events.
     **/
    inline bool has_more_events() const;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer to dispatcher thread, which is holding dispatcher.
     *          The pointer is set after thread has been created and reset
     *          when it is destroyed.
     **/
    DispatcherThread *  mDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief
     **/
    EventDispatcher() = delete;
    AREG_NOCOPY_NOMOVE( EventDispatcher );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline DispatcherThread * EventDispatcher::dispatcher_thread() const
{
    return mDispatcherThread;
}

inline bool EventDispatcher::has_more_events() const
{
    return (mExternalEvents.is_empty() == false);
}

#endif  // AREG_COMPONENT_EVENTDISPATCHER_HPP
