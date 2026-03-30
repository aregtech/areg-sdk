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

#include "areg/component/Event.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/logging/areg_log.h"

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   NullDispatcherThread is hidden and used only as a dummy thread
 *          to get invalid dispatcher. It will ignore all required operations.
 ************************************************************************/

namespace {
    /**
     * \brief   Predefined fixed name of invalid Null Dispatcher Thread.
     **/
    static constexpr std::string_view   NullDispatcherName{ "_NullDispatcherThread_" };
} // namespace

namespace areg {

class AREG_API NullDispatcherThread final   : public DispatcherThread
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class DispatcherThread;

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
// All operations are disabled: the null thread never runs and never holds
// any consumers. Every method is a guarded no-op or ASSERT.
//////////////////////////////////////////////////////////////////////////
protected:
    bool start(uint32_t waitForStartMs = areg::DO_NOT_WAIT) final;
    bool register_event_consumer( const RuntimeClassID & whichClass, EventConsumer & whichConsumer ) final;
    bool unregister_event_consumer( const RuntimeClassID & whichClass, EventConsumer & whichConsumer ) final;
    int32_t  remove_consumer( EventConsumer & whichConsumer ) final;
    bool has_registered_consumer( const RuntimeClassID & whichClass ) const final;
    [[nodiscard]] bool post_event( Event & eventElem ) final;
    bool on_thread_registered( Thread * threadObj ) final;
    [[nodiscard]] bool on_pre_run() final;
    void on_run() final;
    int32_t on_exit() final;

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
AREG_IMPLEMENT_RUNTIME(NullDispatcherThread, DispatcherThread)

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class Constructor 
//////////////////////////////////////////////////////////////////////////
inline NullDispatcherThread::NullDispatcherThread()
    : DispatcherThread( NullDispatcherName, areg::DEFAULT_STACK_SIZE, areg::IGNORE_VALUE )
{}

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool NullDispatcherThread::start(uint32_t /*waitForStartMs = areg::DO_NOT_WAIT*/)
{   return false;   }

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

bool NullDispatcherThread::on_pre_run()
{    return false;  }

void NullDispatcherThread::on_run()
{
    ASSERT(false);
}

int32_t NullDispatcherThread::on_exit()
{
    ASSERT(false);
    return static_cast<int32_t>(ThreadConsumer::ExitCode::Error);
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

DEF_LOG_SCOPE(areg_component_private_DispatcherThread, destroy_thread);
DEF_LOG_SCOPE(areg_component_private_DispatcherThread, trigger_exit);

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class runtime implementation
//////////////////////////////////////////////////////////////////////////
AREG_IMPLEMENT_RUNTIME(DispatcherThread, Thread)

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class statics
//////////////////////////////////////////////////////////////////////////

DispatcherThread & DispatcherThread::_null_dispather_thread() noexcept
{
    return static_cast<DispatcherThread &>(NullDispatcherThread::sSelfNullDispatcher);
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
DispatcherThread::DispatcherThread (const String & threadName, uint32_t stackSizeKb, uint32_t maxQeueue)
    : Thread          ( static_cast<ThreadConsumer &>(self()), threadName, stackSizeKb )
    , EventDispatcher ( threadName, maxQeueue )

    , mEventStarted   ( true, false )
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
    LOG_SCOPE( areg_component_private_DispatcherThread, trigger_exit );
    LOG_DBG( "Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
               , name( ).as_string( )
               , static_cast<id_type>(id( ))
               , mHasStarted.load() ? "STARTED" : "NOT STARTED" );

    stop_dispatcher();
}

Thread::ThreadCompletion DispatcherThread::shutdown( uint32_t waitForStopMs /*= areg::DO_NOT_WAIT*/ )
{
    LOG_SCOPE( areg_component_private_DispatcherThread, destroy_thread );
    LOG_DBG("Shutting down the thread [ %s ] with ID [ %p ]. The current state is [ %s ]"
                , name().as_string( )
                , static_cast<id_type>(id( ))
                , is_running() ? "RUNNING" : "NOT RUNNING" );

    stop_dispatcher( );
    Thread::ThreadCompletion result = Thread::shutdown(waitForStopMs);
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
        mEventStarted.set_signaled();
    }
    else
    {
        mEventStarted.reset( );
        EventDispatcher::ready_for_events( false );
    }
}

DispatcherThread * DispatcherThread::find_consumer_thread( const RuntimeClassID & whichClass ) noexcept
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

} // namespace areg
