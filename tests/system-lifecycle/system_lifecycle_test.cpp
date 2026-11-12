/************************************************************************
 * The lifecycle of models and component threads: load, unload, the watchdog and the
 * deadlines of start and stop, and the failure policies Exit, Restart and Ignore.
 * A case that ends the process runs this executable as a child and checks its exit code.
 *
 * Usage: areg-system-lifecycle-test <case>
 *   exit-watchdog      a hung thread with the default policy ends the process with the failure code
 *   restart-watchdog   a hung thread with policy Restart is replaced; the old instance is freed
 *                      when it returns and does not unregister the new one
 *   ignore-watchdog    a hung thread with policy Ignore is abandoned; its consumer is disconnected,
 *                      the process goes on, the old instance is freed when it returns
 *   unload-restart     the model is unloaded while the watchdog restarts its thread
 *   double-unload      two threads unload the same model at the same time
 *   start-hang-exit    a component that hangs in its startup ends the process
 *   start-hang-ignore  a component that hangs in its startup is abandoned and the waiting load returns
 *   start-hang-register a provider that hangs in its startup before it registers is abandoned;
 *                      its registration reaches no consumer, a new instance does
 *   stop-hang-exit     a component that hangs in its shutdown ends the process
 *   stop-hang-ignore   a component that hangs in its shutdown is abandoned and the unload returns
 *   own-unload         a component unloads its own model without waiting for itself
 *   release-delete     release() destroys the components with the delete function of the model
 *   restart-bound      with two hung instances of a thread, the next one starts when one returns;
 *                      the process goes on
 *   load-refusals      the requests that do not match the model state
 *   load-wait          the load returns once the threads started, or once the components started
 *   logging-toggle     threads log while logging is stopped and started again and again
 *   user-state         every component gets the user state change on its own thread
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/Thread.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/ScopeMessage.hpp"
#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"
#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#ifndef _WIN32
    #include <sys/wait.h>
#endif  // _WIN32

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

#ifdef THREAD_FAILURE_POLICY
    #define SL_POLICY(policy)       THREAD_FAILURE_POLICY(policy)
    #define SL_LIFECYCLE(timeout)   THREAD_LIFECYCLE_TIMEOUT(timeout)
#else   // THREAD_FAILURE_POLICY
    #define SL_POLICY(policy)
    #define SL_LIFECYCLE(timeout)
#endif  // THREAD_FAILURE_POLICY

namespace
{
    constexpr uint32_t      WAIT_MS             { 30'000 };
    constexpr uint32_t      WATCH_MS            { 100 };
    constexpr uint32_t      LIFECYCLE_MS        { 300 };
    //!< The most a held thread waits for its release; a call that waited for it took this long.
    constexpr uint32_t      HANG_MS             { 40'000 };
    //!< A child that is not ended by its policy lives this long; it must be ended before.
    constexpr uint32_t      CHILD_LIVES_MS      { 40'000 };
    constexpr int           FAILURE_EXIT_CODE   { 70 };
    constexpr uint32_t      UNLOAD_ROUNDS       { 20 };
    constexpr uint32_t      DOUBLE_ROUNDS       { 200 };
    constexpr uint32_t      SELF_QUEUE_WAIT_MS  { 40'000 };
    //!< A post or an unload that returns later than this has waited for a timeout.
    constexpr uint32_t      SELF_POST_LIMIT_MS  { SELF_QUEUE_WAIT_MS / 2 };
    constexpr uint32_t      SELF_UNLOAD_LIMIT_MS{ areg::LIFECYCLE_TIMEOUT_DEFAULT / 2 };

    //!< A counter a thread waits on, with a deadline, until it reaches a value.
    class Gate
    {
    public:
        void advance()
        {
            {
                std::lock_guard<std::mutex> lock(mLock);
                ++ mValue;
            }

            mCond.notify_all();
        }

        bool wait_for(uint32_t value, uint32_t timeoutMs = WAIT_MS)
        {
            std::unique_lock<std::mutex> lock(mLock);
            return mCond.wait_for(lock, std::chrono::milliseconds(timeoutMs), [this, value] { return mValue >= value; });
        }

    private:
        std::mutex              mLock;      //!< Guards the value.
        std::condition_variable mCond;      //!< Signalled on every advance.
        uint32_t                mValue{ 0 };//!< The number of advances.
    };

    //!< What a component does when it runs. A held thread does not answer until the test releases it.
    struct Behaviour
    {
        bool        holdRequest     { false };  //!< The provider is held in a request.
        bool        hangEveryone    { false };  //!< Every instance is held, not only the first.
        uint32_t    blockMs         { 0 };      //!< The blocker waits this long for an exit request in its event.
        bool        blockOnExit     { true };   //!< The blocker returns when asked to exit, else it is held.
        uint32_t    blockInstance   { 0 };      //!< The only instance that blocks, 0 for every one.
        bool        holdStartup     { false };  //!< The component is held in its startup.
    bool        holdBeforeBase  { false };  //!< The startup hold comes before the base startup registers the services.
        uint32_t    startupInstance { 0 };      //!< The only instance held in its startup, 0 for every one.
        bool        holdShutdown    { false };  //!< The component is held in its shutdown.
        const char* selfUnload      { nullptr };//!< The model a component unloads from its own thread.
    };

    Behaviour               gBehaviour;
    std::atomic_uint32_t    gCreated        { 0 };
    std::atomic_uint32_t    gDeleted        { 0 };
    std::atomic_uint32_t    gConnects       { 0 };
    std::atomic_uint32_t    gDisconnects    { 0 };
    std::atomic_uint32_t    gResponses      { 0 };
    std::atomic_uint32_t    gFailures       { 0 };
    std::atomic_uint32_t    gCustomDeletes  { 0 };
    std::atomic_uint32_t    gSelfUnloadMs   { 0 };
    std::atomic_uint32_t    gSelfPostMs     { 0 };

    Gate                    gStarted;
    Gate                    gDeletedGate;
    Gate                    gConnectedGate;
    Gate                    gDisconnectedGate;
    Gate                    gResponseGate;
    Gate                    gSelfUnloaded;
    Gate                    gUserStateGate;
    Gate                    gReleased;
    std::atomic_uint32_t    gHoldLevel      { 1 };
    std::atomic_uint32_t    gUserStateCalls { 0 };
    std::atomic_uint32_t    gUserStateWrong { 0 };
    std::atomic_uint32_t    gUserStateLast  { 0 };

    //!< Milliseconds since the given point.
    uint32_t _elapsed_ms(const std::chrono::steady_clock::time_point & since)
    {
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - since).count());
    }

    //!< Sleeps at least ms by the steady clock without looking at the exit request: a thread that does not answer.
    void _hang(uint32_t ms)
    {
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(ms) };
        for (auto now{ std::chrono::steady_clock::now() }; now < until; now = std::chrono::steady_clock::now())
        {
            std::this_thread::sleep_for(until - now);
        }
    }

    //!< Does not answer, ignoring the exit request, until the test released the current hold level, at most HANG_MS.
    void _hold()
    {
        static_cast<void>(gReleased.wait_for(gHoldLevel.load(), HANG_MS));
    }

    //!< Releases the threads held at the current level.
    void _release()
    {
        gReleased.advance();
    }
}

//////////////////////////////////////////////////////////////////////////
// Events
//////////////////////////////////////////////////////////////////////////

class BlockData
{
public:
    BlockData() = default;
    BlockData(const BlockData & /*src*/) = default;
    BlockData & operator = (const BlockData & /*src*/) = default;
};

AREG_DECLARE_EVENT(BlockData, BlockEvent, BlockConsumer);

//////////////////////////////////////////////////////////////////////////
// Components
//////////////////////////////////////////////////////////////////////////

//!< Counts its instances; the base of every component of the test.
class SlCounted : public areg::Component
{
public:
    SlCounted(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , mInstance         ( gCreated.fetch_add(1) + 1 )
    {
    }

    ~SlCounted() override
    {
        gDeleted.fetch_add(1);
        gDeletedGate.advance();
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        const bool held{ gBehaviour.holdStartup && ((gBehaviour.startupInstance == 0) || (gBehaviour.startupInstance == mInstance)) };
        if (held && gBehaviour.holdBeforeBase)
        {
            _hold();
        }

        areg::Component::startup_component(comThread);
        if (held && (gBehaviour.holdBeforeBase == false))
        {
            _hold();
        }

        gStarted.advance();
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        if (gBehaviour.holdShutdown)
        {
            _hold();
        }

        areg::Component::shutdown_component(comThread);
    }

protected:
    const uint32_t  mInstance;  //!< 1 for the first instance created in the process.
};

//!< The provider of the hello service; it may hang in the request.
class SlProvider final : public    SlCounted
                       , protected HelloServiceProviderBase
{
public:
    SlProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : SlCounted                 ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void request_hello_service() override
    {
        if (gBehaviour.holdRequest && (gBehaviour.hangEveryone || (mInstance == 1)))
        {
            _hold();
        }

        response_hello_service();
    }
};

//!< The consumer of the hello service; it sends a request on every connection.
class SlClient final : public    areg::Component
                     , protected HelloServiceConsumerBase
{
public:
    SlClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
    {
    }

    //!< Sends one more request from the component thread.
    void send_request()
    {
        request_hello_service();
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (areg::is_service_connected(status))
        {
            gConnects.fetch_add(1);
            gConnectedGate.advance();
            request_hello_service();
        }
        else if (gConnects.load() > gDisconnects.load())
        {
            gDisconnects.fetch_add(1);
            gDisconnectedGate.advance();
        }

        return result;
    }

    void response_hello_service() override
    {
        gResponses.fetch_add(1);
        gResponseGate.advance();
    }

    void request_hello_service_failed(areg::ResultType /*reason*/) override
    {
        gFailures.fetch_add(1);
    }
};

//!< Posts an event to its own thread at startup and blocks in it.
class SlBlocker final : public    SlCounted
                      , public    BlockConsumer
{
public:
    SlBlocker(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : SlCounted     ( entry, owner )
        , BlockConsumer ( )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        static_cast<void>(BlockEvent::add_listener(static_cast<BlockConsumer &>(*this), comThread));
        BlockEvent block(BlockData{ });
        if (block.register_for_thread(&comThread))
        {
            block.deliver_event();
        }

        SlCounted::startup_component(comThread);
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        BlockEvent::remove_listener(static_cast<BlockConsumer &>(*this), comThread);
        SlCounted::shutdown_component(comThread);
    }

protected:
    void process_event(const BlockData & /*data*/) override
    {
        if ((gBehaviour.blockInstance != 0) && (gBehaviour.blockInstance != mInstance))
        {
            return;
        }

        if (gBehaviour.selfUnload != nullptr)
        {
            const auto start{ std::chrono::steady_clock::now() };
            areg::Application::unload_model(gBehaviour.selfUnload);
            gSelfUnloadMs.store(_elapsed_ms(start));
            gSelfUnloaded.advance();
        }
        else if (gBehaviour.blockOnExit)
        {
            static_cast<void>(areg::Thread::wait_exit(gBehaviour.blockMs));
        }
        else
        {
            _hold();
        }
    }
};

#ifdef THREAD_FAILURE_POLICY
//!< Records the user state changes it gets, and checks that they arrive on its own thread.
class SlUserState final : public SlCounted
{
public:
    SlUserState(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : SlCounted ( entry, owner )
        , mPrevious ( 0u )
    {
    }

protected:
    void on_user_state(uint32_t previous, uint32_t current) override
    {
        const bool ownThread{ areg::Thread::current_thread() == static_cast<areg::Thread *>(&master_thread()) };
        if ((ownThread == false) || (previous != mPrevious))
        {
            gUserStateWrong.fetch_add(1);
        }

        mPrevious = current;
        gUserStateLast.store(current);
        gUserStateCalls.fetch_add(1);
        gUserStateGate.advance();
    }

private:
    uint32_t    mPrevious;  //!< The user state this component got last.
};
#endif  // THREAD_FAILURE_POLICY

//!< Posts one event more than its queue holds to its own thread at startup.
class SlSelfFiller final : public    SlCounted
                         , public    BlockConsumer
{
public:
    SlSelfFiller(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : SlCounted     ( entry, owner )
        , BlockConsumer ( )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        static_cast<void>(BlockEvent::add_listener(static_cast<BlockConsumer &>(*this), comThread));
        const auto start{ std::chrono::steady_clock::now() };
        for (uint32_t i = 0; i <= areg::QUEUE_MIN_RING_CAPACITY; ++ i)
        {
            BlockEvent block(BlockData{ });
            if (block.register_for_thread(&comThread))
            {
                block.deliver_event();
            }
        }

        gSelfPostMs.store(_elapsed_ms(start));
        SlCounted::startup_component(comThread);
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        BlockEvent::remove_listener(static_cast<BlockConsumer &>(*this), comThread);
        SlCounted::shutdown_component(comThread);
    }

protected:
    void process_event(const BlockData & /*data*/) override
    {
    }
};

//!< Does nothing but count.
class SlPlain final : public SlCounted
{
public:
    SlPlain(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : SlCounted ( entry, owner )
    {
    }
};

//////////////////////////////////////////////////////////////////////////
// Models
//////////////////////////////////////////////////////////////////////////

namespace model_1
{
BEGIN_MODEL("SlExitWatchModel")
    BEGIN_REGISTER_THREAD_EX("SlExitWatchThread", WATCH_MS)
        SL_LIFECYCLE(LIFECYCLE_MS)
        BEGIN_REGISTER_COMPONENT("SlExitWatchBlocker", SlBlocker)
        END_REGISTER_COMPONENT("SlExitWatchBlocker")
    END_REGISTER_THREAD("SlExitWatchThread")
END_MODEL("SlExitWatchModel")
}

namespace model_2
{
BEGIN_MODEL("SlRestartModel")
    BEGIN_REGISTER_THREAD_EX("SlRestartProviderThread", WATCH_MS)
        SL_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT("SlRestartProvider", SlProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("SlRestartProvider")
    END_REGISTER_THREAD("SlRestartProviderThread")
    BEGIN_REGISTER_THREAD("SlRestartClientThread")
        BEGIN_REGISTER_COMPONENT("SlRestartClient", SlClient)
            REGISTER_DEPENDENCY("SlRestartProvider")
        END_REGISTER_COMPONENT("SlRestartClient")
    END_REGISTER_THREAD("SlRestartClientThread")
END_MODEL("SlRestartModel")
}

namespace model_3
{
BEGIN_MODEL("SlIgnoreModel")
    BEGIN_REGISTER_THREAD_EX("SlIgnoreProviderThread", WATCH_MS)
        SL_POLICY(areg::FailurePolicy::Ignore)
        BEGIN_REGISTER_COMPONENT("SlIgnoreProvider", SlProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("SlIgnoreProvider")
    END_REGISTER_THREAD("SlIgnoreProviderThread")
    BEGIN_REGISTER_THREAD("SlIgnoreClientThread")
        BEGIN_REGISTER_COMPONENT("SlIgnoreClient", SlClient)
            REGISTER_DEPENDENCY("SlIgnoreProvider")
        END_REGISTER_COMPONENT("SlIgnoreClient")
    END_REGISTER_THREAD("SlIgnoreClientThread")
END_MODEL("SlIgnoreModel")
}

namespace model_4
{
BEGIN_MODEL("SlUnloadRestartModel")
    BEGIN_REGISTER_THREAD_EX("SlUnloadRestartThread", WATCH_MS / 2)
        SL_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT("SlUnloadRestartBlocker", SlBlocker)
        END_REGISTER_COMPONENT("SlUnloadRestartBlocker")
    END_REGISTER_THREAD("SlUnloadRestartThread")
END_MODEL("SlUnloadRestartModel")
}

namespace model_5
{
BEGIN_MODEL("SlDoubleModel")
    BEGIN_REGISTER_THREAD("SlDoubleThread")
        BEGIN_REGISTER_COMPONENT("SlDoublePlain", SlPlain)
        END_REGISTER_COMPONENT("SlDoublePlain")
    END_REGISTER_THREAD("SlDoubleThread")
END_MODEL("SlDoubleModel")
}

namespace model_6
{
BEGIN_MODEL("SlStartExitModel")
    BEGIN_REGISTER_THREAD("SlStartExitThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        BEGIN_REGISTER_COMPONENT("SlStartExitPlain", SlPlain)
        END_REGISTER_COMPONENT("SlStartExitPlain")
    END_REGISTER_THREAD("SlStartExitThread")
END_MODEL("SlStartExitModel")
}

namespace model_7
{
BEGIN_MODEL("SlStartIgnoreModel")
    BEGIN_REGISTER_THREAD("SlStartIgnoreThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Ignore)
        BEGIN_REGISTER_COMPONENT("SlStartIgnorePlain", SlPlain)
        END_REGISTER_COMPONENT("SlStartIgnorePlain")
    END_REGISTER_THREAD("SlStartIgnoreThread")
END_MODEL("SlStartIgnoreModel")
}

namespace model_18
{
BEGIN_MODEL("SlLateProviderModel")
    BEGIN_REGISTER_THREAD("SlLateProviderThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Ignore)
        BEGIN_REGISTER_COMPONENT("SlLateProvider", SlProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT("SlLateProvider")
    END_REGISTER_THREAD("SlLateProviderThread")
END_MODEL("SlLateProviderModel")
}

namespace model_19
{
BEGIN_MODEL("SlLateClientModel")
    BEGIN_REGISTER_THREAD("SlLateClientThread")
        BEGIN_REGISTER_COMPONENT("SlLateClient", SlClient)
            REGISTER_DEPENDENCY("SlLateProvider")
        END_REGISTER_COMPONENT("SlLateClient")
    END_REGISTER_THREAD("SlLateClientThread")
END_MODEL("SlLateClientModel")
}

namespace model_8
{
BEGIN_MODEL("SlStopExitModel")
    BEGIN_REGISTER_THREAD("SlStopExitThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        BEGIN_REGISTER_COMPONENT("SlStopExitPlain", SlPlain)
        END_REGISTER_COMPONENT("SlStopExitPlain")
    END_REGISTER_THREAD("SlStopExitThread")
END_MODEL("SlStopExitModel")
}

namespace model_9
{
BEGIN_MODEL("SlStopIgnoreModel")
    BEGIN_REGISTER_THREAD("SlStopIgnoreThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Ignore)
        BEGIN_REGISTER_COMPONENT("SlStopIgnorePlain", SlPlain)
        END_REGISTER_COMPONENT("SlStopIgnorePlain")
    END_REGISTER_THREAD("SlStopIgnoreThread")
END_MODEL("SlStopIgnoreModel")
}

namespace model_10
{
BEGIN_MODEL("SlOwnUnloadModel")
    BEGIN_REGISTER_THREAD("SlOwnUnloadThread")
        BEGIN_REGISTER_COMPONENT("SlOwnUnloadBlocker", SlBlocker)
        END_REGISTER_COMPONENT("SlOwnUnloadBlocker")
    END_REGISTER_THREAD("SlOwnUnloadThread")
END_MODEL("SlOwnUnloadModel")
}

namespace model_11
{
BEGIN_MODEL("SlReleaseModel")
    BEGIN_REGISTER_THREAD("SlReleaseThread")
        BEGIN_REGISTER_COMPONENT_EX("SlReleasePlain"
                                   , nullptr
                                   , FUNC_CREATE_COMP(SlPlain)
                                   , ([](areg::Component & comp, const areg::ComponentEntry & /*entry*/) -> void
                                        {
                                            gCustomDeletes.fetch_add(1);
                                            delete static_cast<areg::Component *>(&comp);
                                        }))
        END_REGISTER_COMPONENT("SlReleasePlain")
    END_REGISTER_THREAD("SlReleaseThread")
END_MODEL("SlReleaseModel")
}

namespace model_12
{
BEGIN_MODEL("SlBoundModel")
    BEGIN_REGISTER_THREAD_EX("SlBoundThread", WATCH_MS)
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT("SlBoundBlocker", SlBlocker)
        END_REGISTER_COMPONENT("SlBoundBlocker")
    END_REGISTER_THREAD("SlBoundThread")
END_MODEL("SlBoundModel")
}

namespace model_15
{
BEGIN_MODEL("SlStartRestartModel")
    BEGIN_REGISTER_THREAD_EX("SlStartRestartThread", WATCH_MS)
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT("SlStartRestartBlocker", SlBlocker)
        END_REGISTER_COMPONENT("SlStartRestartBlocker")
    END_REGISTER_THREAD("SlStartRestartThread")
END_MODEL("SlStartRestartModel")
}

namespace model_17
{
BEGIN_MODEL("SlStartLoadModel")
    BEGIN_REGISTER_THREAD("SlStartLoadThread")
        SL_LIFECYCLE(LIFECYCLE_MS)
        SL_POLICY(areg::FailurePolicy::Restart)
        BEGIN_REGISTER_COMPONENT("SlStartLoadPlain", SlPlain)
        END_REGISTER_COMPONENT("SlStartLoadPlain")
    END_REGISTER_THREAD("SlStartLoadThread")
END_MODEL("SlStartLoadModel")
}

namespace model_16
{
BEGIN_MODEL("SlSelfPostModel")
    BEGIN_REGISTER_THREAD_EX2("SlSelfPostThread", 0, areg::DEFAULT_STACK_SIZE, areg::QUEUE_MIN_RING_CAPACITY, areg::Bool::False, SELF_QUEUE_WAIT_MS)
        BEGIN_REGISTER_COMPONENT("SlSelfPostFiller", SlSelfFiller)
        END_REGISTER_COMPONENT("SlSelfPostFiller")
    END_REGISTER_THREAD("SlSelfPostThread")
END_MODEL("SlSelfPostModel")
}

#ifdef THREAD_FAILURE_POLICY
namespace model_user
{
BEGIN_MODEL("SlUserModel")
    BEGIN_REGISTER_THREAD("SlUserThreadOne")
        BEGIN_REGISTER_COMPONENT("SlUserOne", SlUserState)
        END_REGISTER_COMPONENT("SlUserOne")
    END_REGISTER_THREAD("SlUserThreadOne")
    BEGIN_REGISTER_THREAD("SlUserThreadTwo")
        BEGIN_REGISTER_COMPONENT("SlUserTwo", SlUserState)
        END_REGISTER_COMPONENT("SlUserTwo")
    END_REGISTER_THREAD("SlUserThreadTwo")
END_MODEL("SlUserModel")
}
#endif  // THREAD_FAILURE_POLICY

namespace model_14
{
BEGIN_MODEL("SlWaitModel")
    BEGIN_REGISTER_THREAD("SlWaitThread")
        BEGIN_REGISTER_COMPONENT("SlWaitPlain", SlPlain)
        END_REGISTER_COMPONENT("SlWaitPlain")
    END_REGISTER_THREAD("SlWaitThread")
END_MODEL("SlWaitModel")
}

namespace model_13
{
BEGIN_MODEL("SlRefuseModel")
    BEGIN_REGISTER_THREAD("SlRefuseThread")
        BEGIN_REGISTER_COMPONENT("SlRefusePlain", SlPlain)
        END_REGISTER_COMPONENT("SlRefusePlain")
    END_REGISTER_THREAD("SlRefuseThread")
END_MODEL("SlRefuseModel")
}

//////////////////////////////////////////////////////////////////////////
// Helpers
//////////////////////////////////////////////////////////////////////////

namespace
{
    //!< Runs this executable with the given case and returns its exit code, or -1.
    int _run_child(const char * childCase, uint32_t & elapsedMs)
    {
        const std::string command{ std::string("\"") + areg::Process::instance().full_path().as_string() + "\" " + childCase };
        const auto start{ std::chrono::steady_clock::now() };
        const int status{ std::system(command.c_str()) };
        elapsedMs = _elapsed_ms(start);
#ifdef _WIN32
        return status;
#else   // _WIN32
        return (WIFEXITED(status) ? WEXITSTATUS(status) : -1);
#endif  // _WIN32
    }

    //!< Expects the child to end with the failure exit code within the bound.
    bool _expect_failure_exit(const char * childCase, uint32_t boundMs)
    {
        uint32_t elapsed{ 0 };
        const int code{ _run_child(childCase, elapsed) };
        printf("child %s ...: exit code %d after %u ms (expected %d within %u ms)\n", childCase, code, elapsed, FAILURE_EXIT_CODE, boundMs);
        return (code == FAILURE_EXIT_CODE) && (elapsed < boundMs);
    }

    //!< The body of a child: loads the model and lives long enough to be ended by the policy.
    int _child(const char * model, bool unload)
    {
        areg::Application::setup(false, true, false, true, true, nullptr);
        areg::Application::load_model(model);
        if (unload)
        {
            static_cast<void>(gStarted.wait_for(1));
            areg::Application::unload_model(model);
        }

        _hang(CHILD_LIVES_MS);
        areg::Application::release();
        return 0;
    }

    //!< True if the thread of the given name is abandoned, where the tree can tell.
    bool _is_abandoned(const char * threadName)
    {
#ifdef THREAD_FAILURE_POLICY
        return (areg::Application::thread_state(threadName) == areg::ThreadState::Abandoned);
#else   // THREAD_FAILURE_POLICY
        printf("thread_state() does not exist on this tree\n");
        static_cast<void>(threadName);
        return false;
#endif  // THREAD_FAILURE_POLICY
    }

    //!< True once the thread of the given name has a running instance, where the tree can tell.
    bool _wait_running(const char * threadName)
    {
#ifdef THREAD_FAILURE_POLICY
        const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
        while ((areg::Application::thread_state(threadName) != areg::ThreadState::Running) && (std::chrono::steady_clock::now() < until))
        {
            _hang(10);
        }

        return (areg::Application::thread_state(threadName) == areg::ThreadState::Running);
#else   // THREAD_FAILURE_POLICY
        printf("thread_state() does not exist on this tree\n");
        static_cast<void>(threadName);
        return false;
#endif  // THREAD_FAILURE_POLICY
    }

    void _print_counts()
    {
        printf("created %u, deleted %u, connects %u, disconnects %u, responses %u, failures %u\n"
                , gCreated.load(), gDeleted.load(), gConnects.load(), gDisconnects.load(), gResponses.load(), gFailures.load());
    }
}

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

//!< The provider hangs, is replaced by a new instance, and the old one is freed when it returns.
static bool restart_watchdog()
{
    gBehaviour.holdRequest = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlRestartModel") };
    ok = ok && gDisconnectedGate.wait_for(1);
    ok = ok && gConnectedGate.wait_for(2);
    ok = ok && gResponseGate.wait_for(1);
    // The old instance is released and freed; the new one must stay registered.
    _release();
    ok = ok && gDeletedGate.wait_for(1);
    _hang(300);
    const uint32_t responses{ gResponses.load() };
    ok = ok && (gDisconnects.load() == 1) && (gConnects.load() == 2);
    areg::Application::unload_model("SlRestartModel");
    areg::Application::release();
    _print_counts();
    printf("responses before the unload .: %u\n", responses);
    return ok && (gCreated.load() == 2) && (gDeleted.load() == 2);
}

//!< The provider hangs and is abandoned; the consumer is disconnected, the process goes on.
static bool ignore_watchdog()
{
    gBehaviour.holdRequest = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlIgnoreModel") };
    ok = ok && gDisconnectedGate.wait_for(1);
    const bool abandoned{ _is_abandoned("SlIgnoreProviderThread") };
    // The abandoned instance is freed once its handler returns.
    _release();
    ok = ok && gDeletedGate.wait_for(1);
    _hang(300);
    ok = ok && abandoned && (gConnects.load() == 1);

    const auto start{ std::chrono::steady_clock::now() };
    areg::Application::unload_model("SlIgnoreModel");
    const uint32_t unloadMs{ _elapsed_ms(start) };
    areg::Application::release();
    _print_counts();
    printf("abandoned %d, unload %u ms\n", abandoned ? 1 : 0, unloadMs);
    return ok && (unloadMs < WAIT_MS) && (gCreated.load() == 1) && (gDeleted.load() == 1);
}

//!< Unloads the model again and again while the watchdog restarts its thread.
static bool unload_restart()
{
    gBehaviour.blockMs = HANG_MS;
    gBehaviour.blockOnExit = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    uint32_t round{ 0 };
    for ( ; round < UNLOAD_ROUNDS; ++ round)
    {
        if (areg::Application::load_model("SlUnloadRestartModel") == false)
            break;

        // Unloads while the restarts run: the first start and two restarts at least.
        if (gStarted.wait_for(gCreated.load() + 2) == false)
            break;

        areg::Application::unload_model("SlUnloadRestartModel");
    }

    areg::Application::release();
    _print_counts();
    printf("rounds %u of %u\n", round, UNLOAD_ROUNDS);
    return (round == UNLOAD_ROUNDS) && (gCreated.load() == gDeleted.load());
}

//!< Two threads unload one model at the same time.
static bool double_unload()
{
    areg::Application::setup(false, true, false, true, true, nullptr);
    uint32_t round{ 0 };
    for ( ; round < DOUBLE_ROUNDS; ++ round)
    {
        if (areg::Application::load_model("SlDoubleModel") == false)
            break;

        if (gStarted.wait_for(round + 1) == false)
            break;

        std::thread first([] { areg::Application::unload_model("SlDoubleModel"); });
        std::thread second([] { areg::Application::unload_model("SlDoubleModel"); });
        first.join();
        second.join();
        if (gDeleted.load() != (round + 1))
            break;
    }

    areg::Application::release();
    _print_counts();
    printf("rounds %u of %u\n", round, DOUBLE_ROUNDS);
    return (round == DOUBLE_ROUNDS) && (gCreated.load() == DOUBLE_ROUNDS) && (gDeleted.load() == DOUBLE_ROUNDS);
}

//!< The startup hangs past the lifecycle timeout; policy Ignore: the load returns, the thread is abandoned.
static bool start_hang_ignore()
{
    gBehaviour.holdStartup = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    const auto start{ std::chrono::steady_clock::now() };
#ifdef THREAD_FAILURE_POLICY
    const bool loaded{ areg::Application::load_model("SlStartIgnoreModel", true) };
#else   // THREAD_FAILURE_POLICY
    const bool loaded{ areg::Application::load_model("SlStartIgnoreModel") };
#endif  // THREAD_FAILURE_POLICY
    const uint32_t loadMs{ _elapsed_ms(start) };
    const bool abandoned{ _is_abandoned("SlStartIgnoreThread") };
    // Freed by itself once the startup returns, before the model is unloaded.
    _release();
    const bool freed{ gDeletedGate.wait_for(1) };
    areg::Application::unload_model("SlStartIgnoreModel");
    areg::Application::release();
    _print_counts();
    printf("loaded %d after %u ms, abandoned %d, freed before unload %d\n", loaded ? 1 : 0, loadMs, abandoned ? 1 : 0, freed ? 1 : 0);
    return loaded && (loadMs >= LIFECYCLE_MS) && (loadMs < HANG_MS) && abandoned && freed && (gDeleted.load() == 1);
}

//!< The provider hangs in its startup before it registers; policy Ignore: the abandoned thread registers in vain.
static bool start_hang_register()
{
#ifdef THREAD_FAILURE_POLICY
    gBehaviour.holdStartup = true;
    gBehaviour.holdBeforeBase = true;
    gBehaviour.startupInstance = 1;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlLateClientModel", true) };
    const bool loaded{ areg::Application::load_model("SlLateProviderModel", true) };
    const bool abandoned{ _is_abandoned("SlLateProviderThread") };
    // The released startup registers the provider, then the thread is freed.
    _release();
    const bool freed{ gDeletedGate.wait_for(1) };
    // The unload delivers the rest of the consumer queue: a connection the registry made is counted.
    areg::Application::unload_model("SlLateClientModel");
    const uint32_t abandonedConnects{ gConnects.load() };
    areg::Application::unload_model("SlLateProviderModel");
    // The second instance does not hang and connects the consumer.
    ok = ok && areg::Application::load_model("SlLateClientModel", true) && areg::Application::load_model("SlLateProviderModel", true);
    const bool connected{ ok && gConnectedGate.wait_for(1) };
    areg::Application::release();
    _print_counts();
    printf("loaded %d, abandoned %d, freed %d, connects of the abandoned provider %u, new instance connected %d\n"
            , loaded ? 1 : 0, abandoned ? 1 : 0, freed ? 1 : 0, abandonedConnects, connected ? 1 : 0);
    return ok && loaded && abandoned && freed && (abandonedConnects == 0) && connected
        && (gConnects.load() == 1) && (gCreated.load() == 2) && (gDeleted.load() == 2);
#else   // THREAD_FAILURE_POLICY
    printf("thread_state() does not exist on this tree\n");
    return false;
#endif  // THREAD_FAILURE_POLICY
}

//!< The instance a restart created hangs in its startup; policy Restart: it is replaced as well.
static bool start_hang_restart()
{
    // The first instance blocks until its watchdog restarts it, the second one hangs in its startup.
    gBehaviour.blockMs = HANG_MS;
    gBehaviour.blockInstance = 1;
    gBehaviour.holdStartup = true;
    gBehaviour.startupInstance = 2;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlStartRestartModel") };
    // The third instance replaces the held second one, which is released after that.
    const auto until{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
    while (ok && (gCreated.load() < 3) && (std::chrono::steady_clock::now() < until))
    {
        _hang(1);
    }

    _release();
    ok = ok && gStarted.wait_for(3);
    const bool running{ _wait_running("SlStartRestartThread") };
    areg::Application::unload_model("SlStartRestartModel");
    ok = ok && gDeletedGate.wait_for(gCreated.load(), WAIT_MS);
    areg::Application::release();
    _print_counts();
    printf("started %d, running instance %d\n", ok ? 1 : 0, running ? 1 : 0);
    return ok && running && (gCreated.load() == 3) && (gDeleted.load() == 3);
}

//!< The first startup hangs past the lifecycle timeout; policy Restart: the load fails, nothing replaces it.
static bool start_hang_load()
{
    gBehaviour.holdStartup = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    const auto start{ std::chrono::steady_clock::now() };
#ifdef THREAD_FAILURE_POLICY
    const bool loaded{ areg::Application::load_model("SlStartLoadModel", true) };
#else   // THREAD_FAILURE_POLICY
    const bool loaded{ areg::Application::load_model("SlStartLoadModel") };
#endif  // THREAD_FAILURE_POLICY
    const uint32_t loadMs{ _elapsed_ms(start) };
    // The abandoned instance is freed once its startup returns; no new one is created.
    _release();
    const bool freed{ gDeletedGate.wait_for(1) };
    _hang(LIFECYCLE_MS);
    areg::Application::unload_model("SlStartLoadModel");
    areg::Application::release();
    _print_counts();
    printf("loaded %d after %u ms, freed %d\n", loaded ? 1 : 0, loadMs, freed ? 1 : 0);
    return (loaded == false) && (loadMs >= LIFECYCLE_MS) && (loadMs < HANG_MS) && freed && (gCreated.load() == 1) && (gDeleted.load() == 1);
}

//!< A component posts to its own full queue at startup; the post fails at once instead of waiting.
static bool start_self_post()
{
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlSelfPostModel") };
    ok = ok && gStarted.wait_for(1);
    areg::Application::unload_model("SlSelfPostModel");
    ok = ok && gDeletedGate.wait_for(1);
    areg::Application::release();
    _print_counts();
    printf("posts at startup took %u ms (limit %u ms, queue wait %u ms)\n", gSelfPostMs.load(), SELF_POST_LIMIT_MS, SELF_QUEUE_WAIT_MS);
    return ok && (gSelfPostMs.load() < SELF_POST_LIMIT_MS);
}

//!< The shutdown hangs past the drain and the lifecycle timeout; policy Ignore: the unload returns.
static bool stop_hang_ignore()
{
    gBehaviour.holdShutdown = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlStopIgnoreModel") };
    ok = ok && gStarted.wait_for(1);
    const auto start{ std::chrono::steady_clock::now() };
    areg::Application::unload_model("SlStopIgnoreModel");
    const uint32_t unloadMs{ _elapsed_ms(start) };
    const bool abandoned{ _is_abandoned("SlStopIgnoreThread") };
    _release();
    ok = ok && gDeletedGate.wait_for(1);
    areg::Application::release();
    _print_counts();
    printf("unload %u ms, abandoned %d\n", unloadMs, abandoned ? 1 : 0);
    return ok && (unloadMs < HANG_MS) && abandoned && (gDeleted.load() == 1);
}

//!< A component unloads its own model; the call returns at once and the model is unloaded.
static bool own_unload()
{
    gBehaviour.selfUnload = "SlOwnUnloadModel";
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlOwnUnloadModel") };
    const bool returned{ gSelfUnloaded.wait_for(1) };
    ok = ok && returned && gDeletedGate.wait_for(1);
    printf("the unload from its own thread returned %d after %u ms\n", returned ? 1 : 0, gSelfUnloadMs.load());
    if (returned == false)
    {
        // The thread waits for itself and the process cannot end in order.
        printf("FAIL\n");
        std::_Exit(1);
    }

    areg::Application::release();
    _print_counts();
    return ok && (gSelfUnloadMs.load() < SELF_UNLOAD_LIMIT_MS) && (gDeleted.load() == 1);
}

//!< release() without unload_model() destroys the component with the delete function of the model.
static bool release_delete()
{
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlReleaseModel") };
    ok = ok && gStarted.wait_for(1);
    areg::Application::release();
    _print_counts();
    printf("delete function calls %u\n", gCustomDeletes.load());
    return ok && (gDeleted.load() == 1) && (gCustomDeletes.load() == 1);
}

//!< The requests that do not match the state of the model are refused at once.
static bool load_refusals()
{
#ifdef THREAD_FAILURE_POLICY
    gBehaviour.holdShutdown = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlRefuseModel", true) };
    ok = ok && (areg::Application::model_state("SlRefuseModel") == areg::ModelState::Loaded);
    ok = ok && areg::Application::load_model("SlRefuseModel");

    std::thread unloader([] { areg::Application::unload_model("SlRefuseModel"); });
    const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
    while ((areg::Application::model_state("SlRefuseModel") != areg::ModelState::Unloading) && (std::chrono::steady_clock::now() < deadline))
    {
        _hang(1);
    }

    const bool unloading{ areg::Application::model_state("SlRefuseModel") == areg::ModelState::Unloading };
    const bool refused{ areg::Application::load_model("SlRefuseModel") == false };
    _release();
    unloader.join();
    const bool unloaded{ areg::Application::model_state("SlRefuseModel") == areg::ModelState::Unloaded };
    areg::Application::unload_model("SlRefuseModel");
    const bool reloaded{ areg::Application::load_model("SlRefuseModel") };
    areg::Application::unload_model("SlRefuseModel");
    const bool unknown{ areg::Application::thread_state("SlNoSuchThread") == areg::ThreadState::Unknown };
    areg::Application::release();
    _print_counts();
    printf("unloading %d, refused %d, unloaded %d, reloaded %d, unknown %d\n", unloading, refused, unloaded, reloaded, unknown);
    return ok && unloading && refused && unloaded && reloaded && unknown && (gCreated.load() == 2) && (gDeleted.load() == 2);
#else   // THREAD_FAILURE_POLICY
    printf("model_state() does not exist on this tree\n");
    return false;
#endif  // THREAD_FAILURE_POLICY
}

//!< Two instances hang: the next one waits until one of them returns, and the process goes on.
static bool restart_bound()
{
    gBehaviour.blockOnExit = false;
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlBoundModel") };
    ok = ok && gStarted.wait_for(2);
    // Both instances hang and are abandoned; no third one starts while they hang.
    _hang(1200);
    const uint32_t whileHung{ gCreated.load() };
    const bool abandoned{ _is_abandoned("SlBoundThread") };
    // The held instances return, and the third one starts.
    _release();
    ok = ok && gStarted.wait_for(3);
    areg::Application::unload_model("SlBoundModel");
    ok = ok && gDeletedGate.wait_for(gCreated.load(), WAIT_MS);
    areg::Application::release();
    _print_counts();
    printf("instances while two hang %u, abandoned %d\n", whileHung, abandoned ? 1 : 0);
    return ok && (whileHung == 2) && abandoned && (gCreated.load() == gDeleted.load());
}

//!< The load returns once the threads started by default, and once the components started on request.
static bool load_wait()
{
#ifdef THREAD_FAILURE_POLICY
    gBehaviour.holdStartup = true;
    areg::Application::setup(false, true, false, true, true, nullptr);
    auto start{ std::chrono::steady_clock::now() };
    bool ok{ areg::Application::load_model("SlWaitModel") };
    const uint32_t threadsMs{ _elapsed_ms(start) };
    const bool loading{ areg::Application::model_state("SlWaitModel") == areg::ModelState::Loading };
    _release();
    ok = ok && gStarted.wait_for(1);
    areg::Application::unload_model("SlWaitModel");

    // The load that waits for the components returns only after the startup is released.
    gHoldLevel.store(2);
    std::atomic_bool released{ false };
    std::thread releaser([&released]
        {
            _hang(LIFECYCLE_MS);
            released.store(true);
            _release();
        });

    ok = ok && areg::Application::load_model("SlWaitModel", true);
    const bool waited{ released.load() };
    releaser.join();
    const bool loaded{ areg::Application::model_state("SlWaitModel") == areg::ModelState::Loaded };
    areg::Application::unload_model("SlWaitModel");
    areg::Application::release();
    _print_counts();
    printf("threads %u ms, loading %d; waited for the components %d, loaded %d\n", threadsMs, loading ? 1 : 0, waited ? 1 : 0, loaded ? 1 : 0);
    return ok && (threadsMs < SELF_UNLOAD_LIMIT_MS) && loading && waited && loaded && (gCreated.load() == 2) && (gDeleted.load() == 2);
#else   // THREAD_FAILURE_POLICY
    printf("load_model() has no wait flag on this tree\n");
    return false;
#endif  // THREAD_FAILURE_POLICY
}

//!< Threads log without a pause while logging is stopped and started again.
static bool logging_toggle()
{
#if AREG_LOGGING
    constexpr uint32_t LOG_TOGGLES{ 50 };
    constexpr uint32_t LOG_WRITERS{ 4 };
    areg::Application::setup(true, false, false, true, false, nullptr);
    std::atomic_bool stop{ false };
    std::atomic_uint32_t sent{ 0 };
    std::vector<std::thread> writers;
    for (uint32_t i = 0; i < LOG_WRITERS; ++ i)
    {
        writers.emplace_back([&stop, &sent]
            {
                while (stop.load() == false)
                {
                    areg::ScopeMessage::log(areg::LogPriority::PrioWarning, "lifecycle logging toggle %u", sent.fetch_add(1));
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            });
    }

    for (uint32_t i = 0; i < LOG_TOGGLES; ++ i)
    {
        areg::Application::stop_logging();
        areg::Application::start_logging(true);
    }

    stop.store(true);
    for (std::thread & writer : writers)
    {
        writer.join();
    }

    areg::Application::release();
    printf("messages %u, toggles %u\n", sent.load(), LOG_TOGGLES);
    return (sent.load() > 0);
#else   // AREG_LOGGING
    printf("logging is not compiled in\n");
    return true;
#endif  // AREG_LOGGING
}

//!< The user state reaches every component once per change, on its own thread.
static bool user_state()
{
#ifdef THREAD_FAILURE_POLICY
    areg::Application::setup(false, true, false, true, true, nullptr);
    bool ok{ areg::Application::load_model("SlUserModel") && gStarted.wait_for(2) };
    const uint32_t initial{ areg::Application::user_state() };
    areg::Application::set_user_state(5u);
    ok = ok && gUserStateGate.wait_for(2);
    areg::Application::set_user_state(5u);
    areg::Application::set_user_state(7u);
    ok = ok && gUserStateGate.wait_for(4);
    _hang(200);
    const uint32_t current{ areg::Application::user_state() };
    areg::Application::unload_model("SlUserModel");
    areg::Application::release();
    printf("initial %u, current %u, calls %u, wrong %u, last %u\n", initial, current, gUserStateCalls.load(), gUserStateWrong.load(), gUserStateLast.load());
    return ok && (initial == 0u) && (current == 7u) && (gUserStateCalls.load() == 4u) && (gUserStateWrong.load() == 0u);
#else   // THREAD_FAILURE_POLICY
    printf("set_user_state() does not exist on this tree\n");
    return false;
#endif  // THREAD_FAILURE_POLICY
}

int main(int argc, char * argv[])
{
    // Unbuffered: what the run printed before a crash still reaches the log.
    setvbuf(stdout, nullptr, _IONBF, 0);

    const char * name{ argc > 1 ? argv[1] : "" };
    bool ok{ false };

    // With AREG_TEST_LOG_CONFIG naming a configuration file, the case runs with logging started.
#ifdef _WIN32
    char * logConfig{ nullptr };
    std::size_t logConfigLen{ 0 };
    if ((::_dupenv_s(&logConfig, &logConfigLen, "AREG_TEST_LOG_CONFIG") == 0) && (logConfig != nullptr) && (*logConfig != '\0'))
    {
        static_cast<void>(areg::start_logging(logConfig));
    }

    std::free(logConfig);
#else   // _WIN32
    const char * logConfig{ std::getenv("AREG_TEST_LOG_CONFIG") };
    if ((logConfig != nullptr) && (*logConfig != '\0'))
    {
        static_cast<void>(areg::start_logging(logConfig));
    }
#endif  // _WIN32

    // The children, ended by their failure policy.
    if (std::strcmp(name, "exit-watchdog-child") == 0)
    {
        gBehaviour.blockOnExit = false;
        return _child("SlExitWatchModel", false);
    }
    else if (std::strcmp(name, "start-hang-exit-child") == 0)
    {
        gBehaviour.holdStartup = true;
        return _child("SlStartExitModel", false);
    }
    else if (std::strcmp(name, "stop-hang-exit-child") == 0)
    {
        gBehaviour.holdShutdown = true;
        return _child("SlStopExitModel", true);
    }

    // A child the policy did not end exits only after CHILD_LIVES_MS.
    if (std::strcmp(name, "exit-watchdog") == 0)
    {
        ok = _expect_failure_exit("exit-watchdog-child", CHILD_LIVES_MS);
    }
    else if (std::strcmp(name, "start-hang-exit") == 0)
    {
        ok = _expect_failure_exit("start-hang-exit-child", CHILD_LIVES_MS);
    }
    else if (std::strcmp(name, "stop-hang-exit") == 0)
    {
        ok = _expect_failure_exit("stop-hang-exit-child", CHILD_LIVES_MS);
    }
    else if (std::strcmp(name, "restart-bound") == 0)
    {
        ok = restart_bound();
    }
    else if (std::strcmp(name, "load-wait") == 0)
    {
        ok = load_wait();
    }
    else if (std::strcmp(name, "restart-watchdog") == 0)
    {
        ok = restart_watchdog();
    }
    else if (std::strcmp(name, "ignore-watchdog") == 0)
    {
        ok = ignore_watchdog();
    }
    else if (std::strcmp(name, "unload-restart") == 0)
    {
        ok = unload_restart();
    }
    else if (std::strcmp(name, "double-unload") == 0)
    {
        ok = double_unload();
    }
    else if (std::strcmp(name, "start-hang-ignore") == 0)
    {
        ok = start_hang_ignore();
    }
    else if (std::strcmp(name, "start-hang-register") == 0)
    {
        ok = start_hang_register();
    }
    else if (std::strcmp(name, "start-hang-restart") == 0)
    {
        ok = start_hang_restart();
    }
    else if (std::strcmp(name, "start-hang-load") == 0)
    {
        ok = start_hang_load();
    }
    else if (std::strcmp(name, "start-self-post") == 0)
    {
        ok = start_self_post();
    }
    else if (std::strcmp(name, "stop-hang-ignore") == 0)
    {
        ok = stop_hang_ignore();
    }
    else if (std::strcmp(name, "own-unload") == 0)
    {
        ok = own_unload();
    }
    else if (std::strcmp(name, "release-delete") == 0)
    {
        ok = release_delete();
    }
    else if (std::strcmp(name, "load-refusals") == 0)
    {
        ok = load_refusals();
    }
    else if (std::strcmp(name, "logging-toggle") == 0)
    {
        ok = logging_toggle();
    }
    else if (std::strcmp(name, "user-state") == 0)
    {
        ok = user_state();
    }
    else
    {
        printf("unknown case [ %s ]\n", name);
        return 2;
    }

    printf("case %s: %s\n", name, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
