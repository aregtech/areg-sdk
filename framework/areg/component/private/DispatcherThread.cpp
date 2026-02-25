/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/DispatcherThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/private/ExitEvent.hpp"
#include "areg/logging/GELog.h"

DEF_LOG_SCOPE( areg_component_private_DispatcherThread_destroyThread);
DEF_LOG_SCOPE( areg_component_private_DispatcherThread_shutdownThread);
DEF_LOG_SCOPE( areg_component_private_DispatcherThread_triggerExit);

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   NullDispatcherThread is hidden and used only as a dummy thread
 *          to get invalid dispatcher. It will ignore all required operations.
 ************************************************************************/

namespace
{
    /**
     * \brief   Predefined fixed name of invalid Null Dispatcher Thread.
     **/
    static constexpr std::string_view   NullDispatcherName{ "_NullDispatcherThread_" };
}

class AREG_API NullDispatcherThread    : public ComponentThread
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class DispatcherThread;
    friend class ComponentThread;
    friend class Component;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   singleton Null Dispatcher Thread. It is not valid and runnable thread.
     **/
    static NullDispatcherThread     sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime to be able to cast and validate thread.
     **/
    AREG_DECLARE_RUNTIME(NullDispatcherThread)
//////////////////////////////////////////////////////////////////////////
// Constructor. Hidden. The object cannot be instantiated.
//////////////////////////////////////////////////////////////////////////
private:
    NullDispatcherThread();
    virtual ~NullDispatcherThread() = default;
//////////////////////////////////////////////////////////////////////////
// Overrides to disable base functionalities.
//////////////////////////////////////////////////////////////////////////
protected:
    //////////////////////////////////////////////////////////////////////////
    // Disable consumer registration functionalities
    //////////////////////////////////////////////////////////////////////////
    virtual bool register_event_consumer( const RuntimeClassID & whichClass, EventConsumer & whichConsumer ) override;
    virtual bool unregister_event_consumer( const RuntimeClassID & whichClass, EventConsumer & whichConsumer ) override;
    virtual int32_t  remove_consumer( EventConsumer & whichConsumer ) override;
    virtual bool has_registered_consumer( const RuntimeClassID & whichClass ) const override;

    //////////////////////////////////////////////////////////////////////////
    // Disable post event
    //////////////////////////////////////////////////////////////////////////
    virtual bool post_event( Event & eventElem ) override;

    //////////////////////////////////////////////////////////////////////////
    // Disable running function and return error on exit.
    //////////////////////////////////////////////////////////////////////////
    virtual bool on_thread_registered( Thread * threadObj ) override;
    virtual void on_thread_runs() override;
    virtual int32_t on_thread_exit() override;

    //////////////////////////////////////////////////////////////////////////
    // Disable Thread locking
    //////////////////////////////////////////////////////////////////////////
    virtual bool wait_start( uint32_t waitTimeout = NECommon::WAIT_INFINITE ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( NullDispatcherThread );
};

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   singleton invalid Null Dispatcher Thread.
 **/
NullDispatcherThread   NullDispatcherThread::sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Runtime information to identify NullDispatcher object
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(NullDispatcherThread, ComponentThread)

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class Constructor 
//////////////////////////////////////////////////////////////////////////
inline NullDispatcherThread::NullDispatcherThread()
    : ComponentThread(NullDispatcherName)
{                   }

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool NullDispatcherThread::register_event_consumer( const RuntimeClassID & /* whichClass*/, EventConsumer & /*whichConsumer*/ )
{   return false;   }

bool NullDispatcherThread::unregister_event_consumer( const RuntimeClassID & /*whichClass*/, EventConsumer & /* whichConsumer*/ )
{   return false;   }

int32_t NullDispatcherThread::remove_consumer( EventConsumer & /* whichConsumer*/ )
{   return 0;       }

bool NullDispatcherThread::has_registered_consumer( const RuntimeClassID & /* whichClass */ ) const
{   return false;   }

bool NullDispatcherThread::post_event( Event& eventElem )
{
    eventElem.destroy();
    ASSERT(false);
    return false;
}

bool NullDispatcherThread::on_thread_registered( Thread * /* threadObj */)
{
    ASSERT(false);
    return false;
}

void NullDispatcherThread::on_thread_runs()
{
    ASSERT(false);
}

int32_t NullDispatcherThread::on_thread_exit()
{
    ASSERT(false);
    return static_cast<int32_t>(ThreadConsumer::ExitCode::Error);
}

bool NullDispatcherThread::wait_start( uint32_t /* waitTimeout */ /*= NECommon::WAIT_INFINITE */ )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread END
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class runtime implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(DispatcherThread, Thread)

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class statics
//////////////////////////////////////////////////////////////////////////

DispatcherThread & DispatcherThread::_null_dispather_thread()
{
    return static_cast<DispatcherThread &>(NullDispatcherThread::sSelfNullDispatcher);
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
DispatcherThread::DispatcherThread (const String & threadName, uint32_t stackSizeKb, uint32_t maxQeueue)
    : Thread          ( static_cast<ThreadConsumer &>(self()), threadName, stackSizeKb )
    , EventDispatcher ( threadName, maxQeueue )

    , mEventStarted     ( true, false )
{
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Methods
//////////////////////////////////////////////////////////////////////////
bool DispatcherThread::post_event( Event& eventElem )
{
    eventElem.destroy();
    ASSERT(false);  // <= this should not be called.
    // You may want to call EventDispatcher::post_event() and/or filter events here

    return true;
}

void DispatcherThread::trigger_exit()
{
    LOG_SCOPE( areg_component_private_DispatcherThread_triggerExit );
    LOG_DBG( "Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
               , name( ).as_string( )
               , static_cast<id_type>(id( ))
               , mHasStarted ? "STARTED" : "NOT STARTED" );

    mExternalEvents.lock_queue( );
    if ( mHasStarted )
    {
        remove_events( true );
        mExternalEvents.push_event( ExitEvent::exit_event( ), nullptr );
    }

    mEventExit.set_event( );
    mExternalEvents.unlock_queue( );
}

Thread::ThreadCompletion DispatcherThread::shutdown_thread( uint32_t waitForStopMs /*= NECommon::DO_NOT_WAIT*/ )
{
    LOG_SCOPE( areg_component_private_DispatcherThread_destroyThread);
    LOG_DBG("Shutting down the thread [ %s ] with ID [ %p ]. The current state is [ %s ]"
                , name().as_string( )
                , static_cast<id_type>(id( ))
                , is_running() ? "RUNNING" : "NOT RUNNING" );

    stop_dispatcher( );
    Thread::ThreadCompletion result = Thread::shutdown_thread(waitForStopMs);
    remove_all_events( );
    return result;
}

DispatcherThread* DispatcherThread::event_consumer_thread( const RuntimeClassID& whichClass )
{
    return (has_registered_consumer(whichClass) ? this : nullptr);
}

void DispatcherThread::ready_for_events( bool is_ready )
{
    if ( is_ready )
    {
        EventDispatcher::ready_for_events( true );
        mEventStarted.set_event( );
    }
    else
    {
        mEventStarted.reset( );
        EventDispatcher::ready_for_events( false );
    }
}

bool DispatcherThread::wait_start( uint32_t waitTimeout /*= NECommon::WAIT_INFINITE */ )
{
    return mEventStarted.lock(waitTimeout);
}

bool DispatcherThread::is_exit_event(const Event * checkEvent) const
{
    return ( checkEvent == static_cast<const Event *>(&ExitEvent::exit_event()) );
}

DispatcherThread * DispatcherThread::find_consumer_thread( const RuntimeClassID & whichClass )
{
    DispatcherThread * result = nullptr;
    Thread* dispThread = AREG_RUNTIME_CAST(Thread::current_thread(), DispatcherThread);
    result = dispThread != nullptr ? static_cast<DispatcherThread *>(dispThread)->event_consumer_thread(whichClass) : nullptr;

    id_type threadId = Thread::INVALID_THREAD_ID;
    dispThread = Thread::first_thread(threadId);
    while ((result == nullptr) && (dispThread != nullptr))
    {
        dispThread = dispThread != nullptr ? AREG_RUNTIME_CAST(dispThread, DispatcherThread) : nullptr;
        result = dispThread != nullptr ? static_cast<DispatcherThread *>(dispThread)->event_consumer_thread(whichClass) : nullptr;

        dispThread = Thread::next_thread(threadId);
    }

    return result;
}
