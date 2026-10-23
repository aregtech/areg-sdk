/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/DispatcherThread.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/Event.hpp"
#include "areg/component/ExitEvent.hpp"
#include "areg/component/ProxyBase.hpp"
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

    //!< How many resolved dispatcher threads one thread caches. Must be a power of two.
    constexpr uint32_t                  DISPATCHER_CACHE_SIZE{ 8u };

    /**
     * \brief   One resolved dispatcher thread, valid while mGeneration matches Thread::registry_generation().
     **/
    struct DispatcherCacheEntry
    {
        UniqueNumber                mNumber;        //!< The unique CRC32 number that was searched.
        uint32_t                    mGeneration;    //!< Registry counter value of the search.
        areg::DispatcherThread *    mThread;        //!< The result, nullptr when nothing was found.
    };

    //!< The resolved dispatcher threads of the calling thread; private to it, so it needs no lock.
    AREG_THREAD_LOCAL DispatcherCacheEntry  _dispatcherCache[DISPATCHER_CACHE_SIZE]{};

    //!< Bit i is set while _dispatcherCache[i] holds a share of its thread.
    AREG_THREAD_LOCAL uint32_t              _dispatcherHeld{ 0u };
    static_assert(DISPATCHER_CACHE_SIZE <= 32u, "One bit of _dispatcherHeld per cache entry");
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
    bool register_event_consumer( const uint32_t whichClass, EventConsumer & whichConsumer ) final;
    bool unregister_event_consumer( const uint32_t whichClass, EventConsumer & whichConsumer ) final;
    int32_t  remove_consumer( EventConsumer & whichConsumer ) final;
    bool has_registered_consumer( const uint32_t whichClass ) const final;
    bool post_event( Event & eventElem ) final;
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
    : DispatcherThread( areg::NullTag{}, NullDispatcherName )
{}

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool NullDispatcherThread::start(uint32_t /*waitForStartMs = areg::DO_NOT_WAIT*/)
{   return false;   }

bool NullDispatcherThread::register_event_consumer( const uint32_t /* whichClass*/, EventConsumer & /*whichConsumer*/ )
{   return false;   }

bool NullDispatcherThread::unregister_event_consumer( const uint32_t /*whichClass*/, EventConsumer & /* whichConsumer*/ )
{   return false;   }

int32_t NullDispatcherThread::remove_consumer( EventConsumer & /* whichConsumer*/ )
{   return 0;       }

bool NullDispatcherThread::has_registered_consumer( const uint32_t /* whichClass */ ) const
{   return false;   }

bool NullDispatcherThread::post_event( Event& /*eventElem*/ )
{
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

DispatcherThread * DispatcherThread::_find_dispatcher_thread( const UniqueNumber threadNumber ) noexcept
{
    const uint32_t generation{ Thread::registry_generation() };
    const DispatcherCacheEntry & entry{ _dispatcherCache[threadNumber & (DISPATCHER_CACHE_SIZE - 1u)] };
    if ((entry.mGeneration != generation) || (entry.mNumber != threadNumber))
    {
        DispatcherThread::_refresh_cache_entry(threadNumber, generation);
    }

    return entry.mThread;
}

struct DispatcherThread::CacheGuard
{
    ~CacheGuard()
    {
        DispatcherThread::_drop_cache_holds();
    }
};

void DispatcherThread::_refresh_cache_entry( const UniqueNumber threadNumber, const uint32_t generation ) noexcept
{
    bool held{ false };
    Thread * found{ Thread::_find_and_hold(threadNumber, held) };
    DispatcherThread * dispatcher{ AREG_RUNTIME_CAST(found, DispatcherThread) };
    if (held && (dispatcher == nullptr))
    {
        Thread::_drop_hold(found);
        held = false;
    }

    // The stale shares are dropped after the new one is taken, so a thread found again stays allocated.
    DispatcherThread * stale[DISPATCHER_CACHE_SIZE]{ };
    uint32_t count{ 0u };
    const uint32_t index{ threadNumber & (DISPATCHER_CACHE_SIZE - 1u) };
    uint32_t heldMask{ _dispatcherHeld };
    for (uint32_t i = 0u; i < DISPATCHER_CACHE_SIZE; ++ i)
    {
        DispatcherCacheEntry & cached{ _dispatcherCache[i] };
        if (((heldMask & (1u << i)) != 0u) && ((cached.mGeneration != generation) || (i == index)))
        {
            stale[count ++] = cached.mThread;
            cached.mThread  = nullptr;
            heldMask       &= ~(1u << i);
        }
    }

    DispatcherCacheEntry & entry{ _dispatcherCache[index] };
    entry.mNumber       = threadNumber;
    entry.mGeneration   = generation;
    entry.mThread       = dispatcher;
    _dispatcherHeld     = held ? (heldMask | (1u << index)) : heldMask;
    if (held)
    {
        static thread_local CacheGuard _guard;
        static_cast<void>(_guard);
    }

    for (uint32_t i = 0u; i < count; ++ i)
    {
        Thread::_drop_hold(stale[i]);
    }
}

void DispatcherThread::_drop_cache_holds() noexcept
{
    for (uint32_t i = 0u; i < DISPATCHER_CACHE_SIZE; ++ i)
    {
        const uint32_t bit{ 1u << i };
        DispatcherThread * dispatcher{ (_dispatcherHeld & bit) != 0u ? _dispatcherCache[i].mThread : nullptr };
        _dispatcherHeld    &= ~bit;
        _dispatcherCache[i] = DispatcherCacheEntry{ };
        if (dispatcher != nullptr)
        {
            Thread::_drop_hold(dispatcher);
        }
    }
}

void DispatcherThread::release_shared( DispatcherThread * dispatcher ) noexcept
{
    if (dispatcher == nullptr)
        return;

    const uint32_t life{ dispatcher->mLifeState.load(std::memory_order_acquire) };
    ASSERT((life & LIFE_REGISTERED) == 0u);
    if ((life & LIFE_SHARES) == 0u)
    {
        delete dispatcher;
    }
    else
    {
        Thread::_drop_hold(dispatcher);
    }
}

void DispatcherThread::share_with_lookups() noexcept
{
    ASSERT(is_running() == false);
    if ((mLifeState.load(std::memory_order_relaxed) & LIFE_SHARES) == 0u)
    {
        mLifeState.fetch_add(LIFE_SHARE, std::memory_order_relaxed);
    }
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
DispatcherThread::DispatcherThread (const String & threadName, uint32_t stackSizeKb, uint32_t maxQeueue, areg::Bool dropOnFull /*= areg::Bool::Undefined*/, uint32_t waitMs /*= areg::WAIT_INFINITE*/)
    : EventDispatcher ( threadName, maxQeueue, dropOnFull, waitMs )
    , Thread          ( static_cast<ThreadConsumer &>(*this), threadName, stackSizeKb )
    , mEventStarted   ( true, false )
{
}

DispatcherThread::DispatcherThread( areg::NullTag, const String & threadName ) noexcept
    : EventDispatcher ( areg::NullTag{} )
    , Thread          ( areg::NullTag{}, static_cast<ThreadConsumer &>(*this), threadName )
    , mEventStarted   ( areg::NullTag{} )
{
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Methods
//////////////////////////////////////////////////////////////////////////
void DispatcherThread::trigger_exit()
{
    LOG_SCOPE( areg_component_private_DispatcherThread, trigger_exit );
    LOG_DBG( "Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
               , name( ).as_string( )
               , static_cast<id_type>(id( ))
               , mHasStarted.load() ? "STARTED" : "NOT STARTED" );

    stop_dispatcher();
}

void DispatcherThread::trigger_exit_drained()
{
    stop_dispatcher_drained();
}

Thread::ThreadCompletion DispatcherThread::shutdown( uint32_t waitForStopMs /*= areg::WAIT_INFINITE*/ )
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

DispatcherThread* DispatcherThread::event_consumer_thread( const uint32_t whichClass ) noexcept
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

DispatcherThread * DispatcherThread::find_consumer_thread( const uint32_t whichClass ) noexcept
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

void DispatcherThread::shutdown_proxies()
{
    ArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::find_thread_proxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.size(); ++i)
    {
        ASSERT(proxyList[i] != nullptr);
        proxyList[i]->stop_proxy();
    }
}

void DispatcherThread::detach_proxies()
{
    ArrayList<std::shared_ptr<ProxyBase>> proxyList;
    ProxyBase::find_thread_proxies(self(), proxyList);
    for (uint32_t i = 0; i < proxyList.size(); ++i)
    {
        ASSERT(proxyList[i] != nullptr);
        proxyList[i]->detach_from_registry();
    }
}

} // namespace areg
