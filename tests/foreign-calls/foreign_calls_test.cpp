/**
 * \file    foreign_calls_test.cpp
 * \brief   Requests, responses, broadcasts and attribute updates called from threads that do not
 *          own the client or the provider. Each must behave as if called on the owner thread.
 *
 * Usage: areg-foreign-calls-test <case>
 *   requests-two-threads   two foreign threads send requests through one client
 *   provider-set-broadcast a foreign thread updates an attribute and sends broadcasts
 *   provider-response      a foreign thread answers requests the provider left pending
 */

#include "foreign_calls.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "examples/33_tempalarm/services/TempMonitorServiceConsumerBase.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

namespace foreign
{
    std::atomic<ProviderCalls *> gProvider          { nullptr };
    std::atomic_uint32_t         gPendingThresholds { 0 };
}

namespace
{
    constexpr uint32_t  REQUESTS_PER_THREAD { 200 };
    constexpr uint32_t  UPDATES             { 200 };
    constexpr uint32_t  RESPONSE_ROUNDS     { 50 };
    constexpr int       WAIT_STEP_MS        { 10 };
    constexpr int       WAIT_LIMIT_MS       { 10000 };

    enum class TestCase { None, RequestsTwoThreads, ProviderSetBroadcast, ProviderResponse };

    TestCase                gCase               { TestCase::None };
    std::atomic_bool        gConnected          { false };
    std::atomic_uint32_t    gStatistics         { 0 };
    std::atomic_uint32_t    gThresholds         { 0 };
    std::atomic_uint32_t    gFailures           { 0 };
    std::atomic_uint32_t    gBroadcasts         { 0 };
    std::atomic_int         gLastReading        { -1 };

    //!< Waits until the predicate is true or the limit expires. Returns the predicate.
    template<typename Predicate>
    bool wait_for(Predicate pred)
    {
        for (int waited = 0; (waited < WAIT_LIMIT_MS) && !pred(); waited += WAIT_STEP_MS)
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_STEP_MS));

        return pred();
    }

    //!< Prints a counter and returns true if it has the expected value.
    bool expect(const char * what, uint32_t actual, uint32_t expected)
    {
        if (actual != expected)
            std::printf("%s: %u (expected %u)\n", what, actual, expected);

        return (actual == expected);
    }
}

//!< Client of the service. Published to the foreign threads by reference once connected.
class ForeignClient final : public    areg::Component
                          , public    TempMonitorServiceConsumerBase
{
public:
    ForeignClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component                   ( entry, owner )
        , TempMonitorServiceConsumerBase    ( entry.mDependencyServices[0], static_cast<areg::Component &>(*this) )
    {
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool relevant{ TempMonitorServiceConsumerBase::service_connected(status, proxy) };
        if (relevant && areg::is_service_connected(status))
        {
            notify_on_reading_update(true);
            notify_on_broadcast_alarm_raised(true);
            gConnected.store(true);
        }
        else if (relevant)
        {
            gConnected.store(false);
        }

        return relevant;
    }

    void on_reading_update(int16_t Reading, areg::DataState state) override
    {
        if (state == areg::DataState::DataIsOK)
            gLastReading.store(Reading);
    }

    void broadcast_alarm_raised(int16_t /*reading*/) override
    {
        gBroadcasts.fetch_add(1);
    }

    void response_get_statistics(int16_t /*lowest*/, int16_t /*highest*/, uint32_t /*alarms_raised*/) override
    {
        gStatistics.fetch_add(1);
    }

    void response_set_thresholds(bool /*accepted*/, const areg::String & /*reason*/) override
    {
        gThresholds.fetch_add(1);
    }

    void request_get_statistics_failed(areg::ResultType /*reason*/) override
    {
        gFailures.fetch_add(1);
    }

    void request_set_thresholds_failed(areg::ResultType /*reason*/) override
    {
        gFailures.fetch_add(1);
    }
};

BEGIN_MODEL("ForeignConsumerModel")
    BEGIN_REGISTER_THREAD( "ForeignConsumerThread" )
        BEGIN_REGISTER_COMPONENT( "ForeignClient", ForeignClient )
            REGISTER_DEPENDENCY( "TempMonitor" )
        END_REGISTER_COMPONENT( "ForeignClient" )
    END_REGISTER_THREAD( "ForeignConsumerThread" )
END_MODEL("ForeignConsumerModel")

namespace
{
    //!< The client component, found by its role name.
    ForeignClient * find_client()
    {
        return static_cast<ForeignClient *>(areg::Component::find_by_name(areg::String("ForeignClient")));
    }

    bool run_requests_two_threads(ForeignClient & client)
    {
        std::mutex lock;
        std::vector<uint64_t> seqNrs;
        auto sender = [&client, &lock, &seqNrs]()
        {
            std::vector<uint64_t> own;
            for (uint32_t i = 0; i < REQUESTS_PER_THREAD; ++i)
                own.push_back(static_cast<uint64_t>(client.request_get_statistics()));

            std::lock_guard<std::mutex> guard(lock);
            seqNrs.insert(seqNrs.end(), own.begin(), own.end());
        };

        std::thread first(sender);
        std::thread second(sender);
        first.join();
        second.join();

        constexpr uint32_t total{ 2 * REQUESTS_PER_THREAD };
        static_cast<void>(wait_for([]() { return (gStatistics.load() + gFailures.load()) >= total; }));
        std::sort(seqNrs.begin(), seqNrs.end());
        const uint32_t distinct{ static_cast<uint32_t>(std::unique(seqNrs.begin(), seqNrs.end()) - seqNrs.begin()) };

        bool ok{ expect("responses", gStatistics.load(), total) };
        ok = expect("failures", gFailures.load(), 0) && ok;
        return expect("distinct sequence numbers", distinct, total) && ok;
    }

    bool run_provider_set_broadcast()
    {
        foreign::ProviderCalls * provider{ foreign::gProvider.load() };
        std::thread updater([provider]()
        {
            for (uint32_t i = 1; i <= UPDATES; ++i)
            {
                provider->call_set_reading(static_cast<int16_t>(i));
                provider->call_broadcast_alarm_raised(static_cast<int16_t>(i));
            }
        });

        updater.join();
        static_cast<void>(wait_for([]() { return (gBroadcasts.load() >= UPDATES) && (gLastReading.load() == static_cast<int>(UPDATES)); }));
        const bool ok{ expect("broadcasts", gBroadcasts.load(), UPDATES) };
        return expect("last reading", static_cast<uint32_t>(gLastReading.load()), UPDATES) && ok;
    }

    bool run_provider_response(ForeignClient & client)
    {
        foreign::ProviderCalls * provider{ foreign::gProvider.load() };
        bool ok{ true };
        std::thread responder([provider, &ok]()
        {
            for (uint32_t round = 1; round <= RESPONSE_ROUNDS; ++round)
            {
                const bool pending{ wait_for([round]() { return foreign::gPendingThresholds.load() >= round; }) };
                if (pending == false)
                {
                    std::printf("round %u: the request did not arrive\n", round);
                    ok = false;
                    return;
                }

                provider->call_response_set_thresholds();
            }
        });

        for (uint32_t round = 1; (round <= RESPONSE_ROUNDS) && ok; ++round)
        {
            client.request_set_thresholds(50, 2);
            if (wait_for([round]() { return (gThresholds.load() + gFailures.load()) >= round; }) == false)
            {
                std::printf("round %u: no response\n", round);
                ok = false;
            }
        }

        responder.join();
        ok = expect("responses", gThresholds.load(), RESPONSE_ROUNDS) && ok;
        return expect("failures", gFailures.load(), 0) && ok;
    }
}

int main(int argc, char * argv[])
{
    const char * name{ argc > 1 ? argv[1] : "" };
    if (std::strcmp(name, "requests-two-threads") == 0)
        gCase = TestCase::RequestsTwoThreads;
    else if (std::strcmp(name, "provider-set-broadcast") == 0)
        gCase = TestCase::ProviderSetBroadcast;
    else if (std::strcmp(name, "provider-response") == 0)
        gCase = TestCase::ProviderResponse;

    if (gCase == TestCase::None)
    {
        std::printf("Usage: areg-foreign-calls-test <requests-two-threads|provider-set-broadcast|provider-response>\n");
        return 2;
    }

    areg::Application::setup();
    areg::Application::load_model("ForeignProviderModel");
    areg::Application::load_model("ForeignConsumerModel");

    bool ok{ wait_for([]() { return gConnected.load() && (foreign::gProvider.load() != nullptr) && (gLastReading.load() == 0); }) };
    ForeignClient * client{ find_client() };
    if ((ok == false) || (client == nullptr))
    {
        std::printf("the client did not connect\n");
        ok = false;
    }
    else
    {
        switch (gCase)
        {
        case TestCase::RequestsTwoThreads:      ok = run_requests_two_threads(*client); break;
        case TestCase::ProviderSetBroadcast:    ok = run_provider_set_broadcast();      break;
        case TestCase::ProviderResponse:        ok = run_provider_response(*client);    break;
        default:                                                                        break;
        }
    }

    areg::Application::unload_model("ForeignConsumerModel");
    areg::Application::unload_model("ForeignProviderModel");
    areg::Application::release();

    std::printf("%s: %s\n", name, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
