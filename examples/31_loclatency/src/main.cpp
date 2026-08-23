//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : Local latency benchmark. Measures how long one message needs
//               inside a single process: between two components of one thread,
//               and between two components of two threads. The message router
//               is not involved, so nothing else has to be started.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentLoader.hpp"

#include "examples/31_loclatency/services/LocalLatency.hpp"
#include "src/LatencyOptions.hpp"
#include "src/LocalLatencyConsumer.hpp"
#include "src/LocalLatencyProvider.hpp"

#include <cstdio>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
    #pragma comment(lib, "31_generated")
#endif // _MSC_VER

namespace
{
    //!< Role name of the provider component. The consumer names it as its dependency.
    constexpr char const PROVIDER_ROLE[]    { "LocalLatencyProvider" };
    //!< Role name of the consumer component.
    constexpr char const CONSUMER_ROLE[]    { "LocalLatencyConsumer" };

    //!< Model in which both components share ONE component thread.
    constexpr char const MODEL_SAME[]       { "LocalLatencySameThread" };
    //!< Model in which each component has its OWN component thread.
    constexpr char const MODEL_CROSS[]      { "LocalLatencyCrossThread" };

    constexpr char const THREAD_SHARED[]    { "LocalLatencySharedThread" };
    constexpr char const THREAD_PROVIDER[]  { "LocalLatencyProviderThread" };
    constexpr char const THREAD_CONSUMER[]  { "LocalLatencyConsumerThread" };

    /**
     * \brief   The event queue of a component thread. The benchmark keeps only one
     *          message on the way at a time, so a small queue is enough. It is stated
     *          here on purpose: a queue that has to grow while measuring would allocate
     *          memory and spoil the result.
     **/
    constexpr uint32_t QUEUE_SIZE           { 256u };

    /**
     * \brief   Describes the "same thread" model: provider and consumer are two
     *          components of one and the same component thread.
     *
     *          A message that one of them sends is put into the event queue of the thread
     *          and is taken out of it by that very same thread. The queue therefore never
     *          runs empty while the test is going on, the thread never falls asleep, and
     *          no thread wake-up is involved. This is the lower limit of what the
     *          framework can do.
     **/
    void _register_same_thread_model()
    {
        BEGIN_MODEL_LOCAL(MODEL_SAME)

            BEGIN_REGISTER_THREAD_EX2(THREAD_SHARED, areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, QUEUE_SIZE, areg::Bool::Undefined, areg::WAIT_INFINITE)

                BEGIN_REGISTER_COMPONENT(PROVIDER_ROLE, LocalLatencyProvider)
                    REGISTER_IMPLEMENT_SERVICE(LocalLatency::ServiceName, LocalLatency::InterfaceVersion)
                END_REGISTER_COMPONENT(PROVIDER_ROLE)

                BEGIN_REGISTER_COMPONENT(CONSUMER_ROLE, LocalLatencyConsumer)
                    REGISTER_DEPENDENCY(PROVIDER_ROLE)
                END_REGISTER_COMPONENT(CONSUMER_ROLE)

            END_REGISTER_THREAD(THREAD_SHARED)

        END_MODEL_LOCAL(MODEL_SAME)
    }

    /**
     * \brief   Describes the "cross thread" model: provider and consumer each have their
     *          own component thread, inside the same process.
     *
     *          Every message now crosses one thread boundary. The sending thread puts the
     *          message into the queue of the receiving thread and wakes that thread up.
     *          The difference to the "same thread" model is therefore the price of one
     *          thread wake-up, which is what this benchmark is meant to show.
     **/
    void _register_cross_thread_model()
    {
        BEGIN_MODEL_LOCAL(MODEL_CROSS)

            BEGIN_REGISTER_THREAD_EX2(THREAD_PROVIDER, areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, QUEUE_SIZE, areg::Bool::Undefined, areg::WAIT_INFINITE)
                BEGIN_REGISTER_COMPONENT(PROVIDER_ROLE, LocalLatencyProvider)
                    REGISTER_IMPLEMENT_SERVICE(LocalLatency::ServiceName, LocalLatency::InterfaceVersion)
                END_REGISTER_COMPONENT(PROVIDER_ROLE)
            END_REGISTER_THREAD(THREAD_PROVIDER)

            BEGIN_REGISTER_THREAD_EX2(THREAD_CONSUMER, areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, QUEUE_SIZE, areg::Bool::Undefined, areg::WAIT_INFINITE)
                BEGIN_REGISTER_COMPONENT(CONSUMER_ROLE, LocalLatencyConsumer)
                    REGISTER_DEPENDENCY(PROVIDER_ROLE)
                END_REGISTER_COMPONENT(CONSUMER_ROLE)
            END_REGISTER_THREAD(THREAD_CONSUMER)

        END_MODEL_LOCAL(MODEL_CROSS)
    }
}

/**
 * \brief   Parses the command line, builds the model that matches the requested topology,
 *          runs the benchmark and returns when it is finished.
 *
 * \return  Returns 0 when the benchmark finished, 1 when the command line was wrong and
 *          2 when the benchmark got stuck. A script can rely on these values.
 **/
int main(int argc, char * argv[])
{
    bool exitNow{ false };
    if (!loclat::parse_command_line(argc, argv, loclat::run_options(), exitNow))
        return 1;

    if (exitNow)
        return 0;

    const bool sameThread{ loclat::run_options().mTopology == loclat::Topology::SameThread };
    const char * modelName{ sameThread ? MODEL_SAME : MODEL_CROSS };

    if (sameThread)
        _register_same_thread_model();
    else
        _register_cross_thread_model();

    // Logging and the message router stay switched off: both would add work to the very
    // path that is being measured, and a Private service needs neither.
    areg::Application::setup(false, true, false, true, false, nullptr, nullptr);

    areg::Application::load_model(modelName);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(modelName);
    areg::Application::release();

    if (LocalLatencyConsumer::has_stalled())
    {
        std::printf("The benchmark did not finish. See the message above.\n");
        return 2;
    }

    return 0;
}
