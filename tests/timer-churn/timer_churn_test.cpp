/************************************************************************
 * Hammers the timer and the watchdog backend: arms, disarms, restarts and
 * destroys timers from several threads while they are firing, then tears the
 * whole application down and checks that Application::release() returns.
 *
 * Written for the defect that hung 26_pubsubmix on Cygwin. The generic POSIX
 * backend armed a 'timer_create' + 'SIGEV_THREAD' timer, so the OS ran the
 * expiry on a thread it created and destroyed itself. That thread took the
 * timer lock, the manager resource lock, the timer object lock and the event
 * queue of the owning dispatcher. A thread destroyed inside any of those
 * regions leaves the lock owned by a thread that no longer exists, and the next
 * caller waits for it forever: TimerManager::wait_timer_manager() never
 * returned and Application::release() never finished. The backend now keeps the
 * deadlines itself and fires them on the manager thread.
 *
 * The test fails, rather than hanging, in three ways:
 *   - the built-in watchdog thread ends the process with code 2 if any phase
 *     takes longer than TEST_WATCHDOG_MS, which is what a stranded lock does;
 *   - a cycle that produced no expiry at all fails, so a backend that never
 *     hangs because it never fires is not mistaken for a healthy one;
 *   - a component or thread left behind after release() fails.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/String.hpp"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

class ChurnComponent;

namespace
{
    constexpr char const    MODEL_NAME[]        { "TimerChurnModel" };
    constexpr char const    ROLE_ONE[]          { "TimerChurnOne" };
    constexpr char const    ROLE_TWO[]          { "TimerChurnTwo" };
    constexpr char const    THREAD_ONE[]        { "TimerChurnThreadOne" };
    constexpr char const    THREAD_TWO[]        { "TimerChurnThreadTwo" };

    //!< Timers per component. All of them are destroyed at teardown.
    constexpr uint32_t      TIMERS_PER_COMPONENT{ 6u };
    //!< The first ones are left alone and must keep firing: a backend that hangs is a
    //!< failure, and so is one that never hangs because it stopped delivering.
    constexpr uint32_t      STABLE_TIMERS       { 2u };
    //!< Milliseconds between two churn rounds. Without a pause the churned timers are
    //!< re-armed faster than their shortest timeout and the churn alone proves nothing.
    constexpr uint32_t      CHURN_PAUSE_MS      { 7u };
    //!< Load / churn / release cycles. The defect needs a teardown to show, so several.
    constexpr uint32_t      CYCLES              { 6u };
    //!< How long one cycle churns before the application is torn down, milliseconds.
    constexpr uint32_t      CHURN_MS            { 1200u };
    //!< Churn threads that fight the timers from outside the component threads.
    constexpr uint32_t      CHURN_THREADS       { 3u };
    //!< The shortest and the longest timeout handed to a timer, milliseconds.
    constexpr uint32_t      TIMEOUT_MIN_MS      { 1u };
    constexpr uint32_t      TIMEOUT_MAX_MS      { 17u };
    //!< A phase that takes longer than this is a hang, not a slow machine.
    constexpr uint32_t      TEST_WATCHDOG_MS    { 90u * 1000u };

    std::atomic_uint        gExpired    { 0u };      //!< expiries seen in the current cycle
    std::atomic_int         gAlive      { 0 };       //!< live components
    std::atomic_bool        gChurnStop  { false };   //!< tells the churn threads to leave
    std::atomic_bool        gFinished   { false };   //!< tells the watchdog the run is over
    std::atomic_uint        gPhase      { 0u };      //!< last phase the run reached

    /**
     * \brief   The live components, so a churn thread can reach them without asking the
     *          framework. A component removes itself here as the first thing its destructor
     *          does, and a churn thread holds the lock while it touches one, so 'in the
     *          registry' means 'alive for as long as the lock is held'.
     **/
    std::mutex                      gLiveLock;
    std::vector<ChurnComponent *>   gLive;

    /**
     * \brief   Ends the process instead of hanging, so a stranded lock is a failure with a
     *          location rather than a test that never returns.
     **/
    void start_test_watchdog()
    {
        std::thread guard([]()
        {
            for (uint32_t waited = 0u; waited < TEST_WATCHDOG_MS; waited += 100u)
            {
                if (gFinished.load())
                    return;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            std::fprintf(stderr, "TIMED OUT after %u ms in phase %u -- the run is stuck\n"
                        , TEST_WATCHDOG_MS, gPhase.load());
            std::fflush(stderr);
            std::fflush(stdout);
            ::_Exit(2);
        });

        guard.detach();
    }
}

//////////////////////////////////////////////////////////////////////////
// A component that keeps a handful of timers and never stops touching them.
//////////////////////////////////////////////////////////////////////////
class ChurnComponent   : public areg::Component
                       , private areg::TimerConsumer
{
public:
    static areg::Component * create(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    {
        return new ChurnComponent(entry, owner);
    }

    static void destroy(areg::Component & item, const areg::ComponentEntry &)
    {
        delete (&item);
    }

    ChurnComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   (entry, owner)
        , areg::TimerConsumer()
        , mTimers           ()
        , mSeed             (entry.mRoleName.length() + 1u)
    {
        {
            std::lock_guard<std::mutex> guard(gLiveLock);
            gLive.push_back(this);
        }

        gAlive.fetch_add(1);
    }

    virtual ~ChurnComponent()
    {
        {
            // Out of reach of the churn threads before anything of this object is touched.
            std::lock_guard<std::mutex> guard(gLiveLock);
            for (uint32_t i = 0u; i < gLive.size(); ++i)
            {
                if (gLive[i] == this)
                {
                    gLive.erase(gLive.begin() + i);
                    break;
                }
            }
        }

        // Destroys every timer while the manager may still be looking at it. This is the
        // call that used to run ::timer_delete() with the timer lock held.
        mTimers.clear();
        gAlive.fetch_sub(1);
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);

        for (uint32_t i = 0u; i < TIMERS_PER_COMPONENT; ++i)
        {
            areg::String name{ role_name() + "_churn_" + areg::String::make_string(i) };
            mTimers.push_back(std::make_unique<areg::Timer>(static_cast<areg::TimerConsumer &>(*this), name));
            mTimers.back()->start_timer(_timeout(i), comThread, areg::Timer::CONTINUOUSLY);
        }
    }

    void shutdown_component(areg::ComponentThread & comThread) override
    {
        for (auto & timer : mTimers)
        {
            timer->stop_timer();
        }

        areg::Component::shutdown_component(comThread);
    }

    /**
     * \brief   Re-arms the timers from outside the component thread, which is what a churn
     *          thread does. Every verb of the timer API is used, including the one-shot
     *          path, which is the one that disarms itself from inside the expiry.
     **/
    void churn(uint32_t step)
    {
        // The stable timers are skipped on purpose, see STABLE_TIMERS.
        for (uint32_t i = STABLE_TIMERS; i < mTimers.size(); ++i)
        {
            areg::Timer * timer = mTimers[i].get();
            switch ((step + i) % 4u)
            {
            case 0u:
                timer->stop_timer();
                break;

            case 1u:
                timer->start_timer(_timeout(step + i), areg::Timer::CONTINUOUSLY);
                break;

            case 2u:
                timer->start_timer(_timeout(step + i), 1u);     // one shot
                break;

            default:
                break;      // leave this one running, so something is always armed
            }
        }
    }

private:

    void process_timer(areg::Timer & /* timer */) override
    {
        gExpired.fetch_add(1u, std::memory_order_relaxed);
    }

    [[nodiscard]]
    uint32_t _timeout(uint32_t step) const
    {
        const uint32_t span{ TIMEOUT_MAX_MS - TIMEOUT_MIN_MS + 1u };
        return TIMEOUT_MIN_MS + ((step + mSeed) % span);
    }

    std::vector<std::unique_ptr<areg::Timer>>   mTimers;
    uint32_t                                    mSeed;
};

BEGIN_MODEL(MODEL_NAME)
    BEGIN_REGISTER_THREAD(THREAD_ONE)
        BEGIN_REGISTER_COMPONENT(ROLE_ONE, ChurnComponent)
        END_REGISTER_COMPONENT(ROLE_ONE)
    END_REGISTER_THREAD(THREAD_ONE)

    BEGIN_REGISTER_THREAD(THREAD_TWO)
        BEGIN_REGISTER_COMPONENT(ROLE_TWO, ChurnComponent)
        END_REGISTER_COMPONENT(ROLE_TWO)
    END_REGISTER_THREAD(THREAD_TWO)
END_MODEL(MODEL_NAME)

namespace
{
    void churn_thread_routine(uint32_t index)
    {
        for (uint32_t step = index; !gChurnStop.load(std::memory_order_relaxed); ++step)
        {
            {
                std::lock_guard<std::mutex> guard(gLiveLock);
                for (ChurnComponent * component : gLive)
                {
                    component->churn(step);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(CHURN_PAUSE_MS));
        }
    }
}

int main()
{
    start_test_watchdog();

    bool failed{ false };

    for (uint32_t cycle = 1u; cycle <= CYCLES; ++cycle)
    {
        gPhase.store(cycle * 10u + 1u);
        gExpired.store(0u);
        gChurnStop.store(false);

        // Watchdog on: it uses the same TimerPosix object and the same manager loop as the
        // timer, so the watchdog backend is exercised by every dispatch below.
        areg::Application::setup(false, true, false, true, true, nullptr);
        areg::Application::load_model(MODEL_NAME);

        gPhase.store(cycle * 10u + 2u);
        std::vector<std::thread> churners;
        churners.reserve(CHURN_THREADS);
        for (uint32_t i = 0u; i < CHURN_THREADS; ++i)
        {
            churners.emplace_back(churn_thread_routine, i);
        }

        areg::Thread::sleep(CHURN_MS);

        gPhase.store(cycle * 10u + 3u);
        gChurnStop.store(true);
        for (std::thread & churner : churners)
        {
            churner.join();
        }

        const uint32_t expired{ gExpired.load() };

        // The teardown the defect used to hang in. Nothing here may block.
        gPhase.store(cycle * 10u + 4u);
        areg::Application::release();

        gPhase.store(cycle * 10u + 5u);
        const int    alive      { gAlive.load() };
        const bool   threadsGone{ (areg::Thread::find_by_address(areg::ThreadAddress(THREAD_ONE)) == nullptr) &&
                                  (areg::Thread::find_by_address(areg::ThreadAddress(THREAD_TWO)) == nullptr) };

        std::printf("cycle %u: expiries %u, components left %d, threads gone %s\n"
                   , cycle, expired, alive, threadsGone ? "yes" : "no");
        std::fflush(stdout);

        if (expired == 0u)
        {
            std::printf("FAILED: no timer expired in cycle %u\n", cycle);
            failed = true;
        }

        if (alive != 0)
        {
            std::printf("FAILED: %d component(s) left after release() in cycle %u\n", alive, cycle);
            failed = true;
        }

        if (!threadsGone)
        {
            std::printf("FAILED: a component thread stayed in the registry in cycle %u\n", cycle);
            failed = true;
        }
    }

    gFinished.store(true);

    std::printf("%s\n", failed ? "timer churn test FAILED" : "timer churn test passed");
    std::fflush(stdout);

    return failed ? 1 : 0;
}
