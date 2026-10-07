/************************************************************************
 * Delivery test of internal events sent where no dispatch is running.
 *
 *   - Phase 1: a component sends itself an internal event from startup_component,
 *     and nothing else ever reaches its thread. The event must still be delivered.
 *   - Phase 2: while the component thread keeps sending itself a chain of internal
 *     events, several foreign threads send it internal events too. Every event of
 *     both kinds must be delivered exactly once.
 *
 * The test fails instead of hanging: each phase has a deadline, and a watchdog
 * thread ends the process with code 2 if the teardown does not return.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/base/Thread.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

namespace
{
    constexpr char const    MODEL_NAME[]        { "InternalEventsModel" };
    constexpr char const    ROLE_NAME[]         { "InternalEventsRole" };
    constexpr char const    THREAD_NAME[]       { "InternalEventsThread" };

    //!< The kinds of event the component receives.
    enum class Kind : uint32_t
    {
          Start     //!< Sent by the component from startup_component.
        , Hop       //!< Sent by the component to itself, one per received Hop.
        , Foreign   //!< Sent by a foreign thread.
    };

    //!< Hops of the chain the component thread sends itself.
    constexpr uint32_t      HOPS                { 50000u };
    //!< Foreign threads and the events each of them sends.
    constexpr uint32_t      FOREIGN_THREADS     { 4u };
    constexpr uint32_t      FOREIGN_EVENTS      { 10000u };
    //!< Deadlines of the two phases, milliseconds.
    constexpr uint32_t      START_DEADLINE_MS   { 2000u };
    constexpr uint32_t      CHURN_DEADLINE_MS   { 60000u };
    //!< The whole run must end within this time, milliseconds.
    constexpr uint32_t      TEST_WATCHDOG_MS    { 100u * 1000u };

    std::atomic_uint        gStarted    { 0u };     //!< Start events received
    std::atomic_uint        gHops       { 0u };     //!< Hop events received
    std::atomic_uint        gForeign    { 0u };     //!< Foreign events received
    std::atomic_bool        gFinished   { false };  //!< tells the watchdog the run is over

    //!< Ends the process with code 2 if the run takes longer than TEST_WATCHDOG_MS.
    void start_test_watchdog()
    {
        std::thread([]()
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(TEST_WATCHDOG_MS) };
            while (!gFinished.load())
            {
                if (std::chrono::steady_clock::now() > deadline)
                {
                    std::printf("FAILED: the run did not finish within %u ms\n", TEST_WATCHDOG_MS);
                    std::fflush(stdout);
                    std::_Exit(2);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }).detach();
    }

    //!< Waits until the counter reaches the expected value or the deadline passes.
    bool wait_for(const std::atomic_uint & counter, uint32_t expected, uint32_t deadlineMs)
    {
        const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(deadlineMs) };
        while (counter.load() < expected)
        {
            if (std::chrono::steady_clock::now() > deadline)
                return false;

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        return true;
    }
}

//!< The payload of the test event.
class KindData
{
public:
    KindData() = default;
    explicit KindData(Kind kind) : mKind(kind) {}
    KindData(const KindData & src) = default;
    KindData & operator = (const KindData & src) = default;

    inline Kind kind() const { return mKind; }

private:
    Kind    mKind{ Kind::Start };
};

AREG_DECLARE_INTERNAL_EVENT(KindData, KindEvent, KindConsumer);

//!< Counts every event it receives and keeps the Hop chain going.
class KindComponent final   : public    areg::Component
                            , public    KindConsumer
{
public:
    KindComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , KindConsumer      ( )
    {
    }

protected:
    void startup_component(areg::ComponentThread & comThread) final
    {
        areg::Component::startup_component(comThread);
        KindEvent::add_listener(static_cast<KindConsumer &>(self()), static_cast<areg::DispatcherThread &>(comThread));
        KindEvent::send_event(KindData(Kind::Start));
    }

    void shutdown_component(areg::ComponentThread & comThread) final
    {
        KindEvent::remove_listener(static_cast<KindConsumer &>(self()), static_cast<areg::DispatcherThread &>(comThread));
        areg::Component::shutdown_component(comThread);
    }

    void process_event(const KindData & data) final
    {
        switch (data.kind())
        {
        case Kind::Start:
            gStarted.fetch_add(1u);
            break;

        case Kind::Hop:
            if (gHops.fetch_add(1u) + 1u < HOPS)
            {
                KindEvent::send_event(KindData(Kind::Hop));
            }
            break;

        case Kind::Foreign:
            gForeign.fetch_add(1u);
            break;
        }
    }

private:
    inline KindComponent & self()
    {   return (*this); }
};

BEGIN_MODEL(MODEL_NAME)
    BEGIN_REGISTER_THREAD(THREAD_NAME)
        BEGIN_REGISTER_COMPONENT(ROLE_NAME, KindComponent)
        END_REGISTER_COMPONENT(ROLE_NAME)
    END_REGISTER_THREAD(THREAD_NAME)
END_MODEL(MODEL_NAME)

int main()
{
    // Unbuffered: what the run printed before a crash still reaches the log.
    setvbuf(stdout, nullptr, _IONBF, 0);

    start_test_watchdog();
    bool failed{ false };

    areg::Application::setup(false, true, false, true, true, nullptr);
    areg::Application::load_model(MODEL_NAME);

    const bool started{ wait_for(gStarted, 1u, START_DEADLINE_MS) };
    std::printf("phase 1: the event sent from startup_component %s\n", started ? "arrived" : "did not arrive");
    std::fflush(stdout);
    failed = failed || (started == false);

    // The first Hop is sent from a foreign thread; the component sends every later one itself.
    KindEvent::send_event(KindData(Kind::Hop));
    std::vector<std::thread> senders;
    senders.reserve(FOREIGN_THREADS);
    for (uint32_t i = 0u; i < FOREIGN_THREADS; ++i)
    {
        senders.emplace_back([]()
        {
            for (uint32_t n = 0u; n < FOREIGN_EVENTS; ++n)
            {
                KindEvent::send_event(KindData(Kind::Foreign));
            }
        });
    }

    for (std::thread & sender : senders)
    {
        sender.join();
    }

    const bool foreignDone{ wait_for(gForeign, FOREIGN_THREADS * FOREIGN_EVENTS, CHURN_DEADLINE_MS) };
    const bool hopsDone{ wait_for(gHops, HOPS, CHURN_DEADLINE_MS) };
    areg::Thread::sleep(50u);

    const uint32_t foreign{ gForeign.load() };
    const uint32_t hops{ gHops.load() };
    std::printf("phase 2: foreign events %u of %u, hops %u of %u\n"
               , foreign, FOREIGN_THREADS * FOREIGN_EVENTS, hops, HOPS);
    std::fflush(stdout);
    failed = failed || !foreignDone || !hopsDone || (foreign != FOREIGN_THREADS * FOREIGN_EVENTS) || (hops != HOPS) || (gStarted.load() != 1u);

    areg::Application::unload_model(MODEL_NAME);
    areg::Application::release();
    gFinished.store(true);

    std::printf("%s\n", failed ? "FAILED" : "PASSED");
    return (failed ? 1 : 0);
}
