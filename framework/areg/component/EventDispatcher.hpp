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
#include "areg/base/areg_global.h"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/EventRouter.hpp"
#include "areg/component/private/EventDispatcherBase.hpp"

namespace areg {

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
     * \brief   Creates EventDispatcher with specified name and event-queue parameters. The queue
     *          parameters follow a three-tier resolution (explicit value, then configuration, then
     *          built-in default); see EventDispatcherBase for the full semantics.
     *
     * \param   name            The name of Event Dispatcher
     * \param   maxQeueue       The event-queue ring capacity. areg::IGNORE_VALUE reads from configuration.
     * \param   dropOnFull      The full-ring policy. areg::Bool::Undefined reads from configuration.
     * \param   waitMs          The lossless full-ring block timeout. areg::WAIT_INFINITE reads from configuration.
     **/
    explicit EventDispatcher( const String & name
                            , uint32_t maxQeueue   = areg::IGNORE_VALUE
                            , areg::Bool dropOnFull = areg::Bool::Undefined
                            , uint32_t waitMs       = areg::WAIT_INFINITE );

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
    [[nodiscard]]
    bool on_thread_registered( Thread * threadObj ) override;

    /**
     * \brief   Triggered when thread object is being destroyed.
     **/
    void on_thread_unregistering() override;

    /**
     * \brief   Triggered when thread is running and fully operational.
     **/
    void on_run() override;

    /**
     * \brief   Triggered when thread is going to exit.
     *
     * \return  Return thread exit error code.
     **/
    int32_t on_exit() override;

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
    [[nodiscard]]
    inline DispatcherThread * dispatcher_thread() const noexcept;

protected:
    /**
     * \brief   Returns true if dispatcher has more queued external events.
     **/
    [[nodiscard]]
    inline bool has_more_events() const noexcept;

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
    EventDispatcher() = delete;
    AREG_NOCOPY_NOMOVE( EventDispatcher );
};

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class inline functions implementation
//////////////////////////////////////////////////////////////////////////
inline DispatcherThread * EventDispatcher::dispatcher_thread() const noexcept
{
    return mDispatcherThread;
}

inline bool EventDispatcher::has_more_events() const noexcept
{
    return mExternalEvents.has_pending();
}

} // namespace areg
#endif  // AREG_COMPONENT_EVENTDISPATCHER_HPP
