/**
 * \file    worker_proxy_test.cpp
 * \brief   Clients created, used and deleted in a worker thread. Each client has its proxy in
 *          the worker thread and must get every callback there.
 *
 * Usage: areg-worker-proxy-test <case>
 *   heap       a client created with new when the worker starts, deleted when it stops
 *   member     a client with a fixed address, constructed when the worker starts
 *   on-the-fly a new client for every round, the client of the previous round deleted
 *   late       clients added to the connected proxy of the worker
 *   reload     the heap case twice, the consumer model unloaded and loaded again
 */

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/WorkerThread.hpp"
#include "areg/component/WorkerThreadConsumer.hpp"
#include "examples/33_tempalarm/services/TempMonitorServiceConsumerBase.hpp"

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <optional>
#include <thread>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "33_generated")
#endif // _MSC_VER

namespace
{
    constexpr uint32_t  MAX_CLIENTS     { 32 };
    constexpr uint32_t  FLY_ROUNDS      { 20 };
    constexpr uint32_t  LATE_CLIENTS    { 5 };
    constexpr int16_t   HIGH_LIMIT      { 77 };
    constexpr int       WAIT_STEP_MS    { 10 };
    constexpr int       WAIT_LIMIT_MS   { 10000 };

    const char * const  ROLE_NAME       { "TempMonitor" };

    enum class TestCase { None, Heap, Member, OnTheFly, Late, Reload };

    //!< The callbacks one client received.
    struct ClientCounters
    {
        std::atomic_uint32_t    connects    { 0 };
        std::atomic_uint32_t    disconnects { 0 };
        std::atomic_uint32_t    statistics  { 0 };
        std::atomic_uint32_t    thresholds  { 0 };
        std::atomic_uint32_t    broadcasts  { 0 };
        std::atomic_uint32_t    failures    { 0 };
        std::atomic_int         reading     { -1 };
    };

    TestCase                gCase           { TestCase::None };
    ClientCounters          gClients[MAX_CLIENTS];
    std::atomic<id_type>    gWorkerThreadId { 0 };
    std::atomic_bool        gWorkerReady    { false };
    std::atomic_uint32_t    gWrongThread    { 0 };
    std::atomic_uint32_t    gRoundsCreated  { 0 };

    //!< Waits until the predicate is true or the limit expires. Returns the predicate.
    template<typename Predicate>
    bool wait_for(Predicate pred)
    {
        for (int waited = 0; (waited < WAIT_LIMIT_MS) && !pred(); waited += WAIT_STEP_MS)
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_STEP_MS));

        return pred();
    }

    //!< Prints a mismatch and returns true if the value is the expected one.
    bool expect(const char * what, uint32_t slot, uint32_t actual, uint32_t expected)
    {
        if (actual != expected)
            std::printf("client %u %s: %u (expected %u)\n", slot, what, actual, expected);

        return (actual == expected);
    }

    //!< True when the client of the slot has finished its exchange with the provider.
    bool exchange_done(uint32_t slot)
    {
        const ClientCounters & c{ gClients[slot] };
        return (c.statistics.load() >= 1) && (c.thresholds.load() >= 1) && (c.broadcasts.load() >= 1) && (c.reading.load() == HIGH_LIMIT);
    }

    //!< Checks the callbacks of a client that completed its exchange and was deleted while connected.
    //!< A deleted client gets its Disconnected in the destructor of its base class, not in its own.
    bool check_client(uint32_t slot, uint32_t broadcasts = 1)
    {
        const ClientCounters & c{ gClients[slot] };
        bool ok{ expect("connects", slot, c.connects.load(), 1) };
        ok = expect("disconnects", slot, c.disconnects.load(), 0) && ok;
        ok = expect("statistics responses", slot, c.statistics.load(), 1) && ok;
        ok = expect("threshold responses", slot, c.thresholds.load(), 1) && ok;
        ok = expect("broadcasts", slot, c.broadcasts.load(), broadcasts) && ok;
        ok = expect("failures", slot, c.failures.load(), 0) && ok;
        return expect("last reading", slot, static_cast<uint32_t>(c.reading.load()), static_cast<uint32_t>(HIGH_LIMIT)) && ok;
    }

    void reset_counters()
    {
        for (ClientCounters & c : gClients)
        {
            c.connects = 0; c.disconnects = 0; c.statistics = 0; c.thresholds = 0;
            c.broadcasts = 0; c.failures = 0; c.reading = -1;
        }

        gRoundsCreated = 0;
        gWorkerReady = false;
    }

    void check_thread()
    {
        if (areg::Thread::current_thread_id() != gWorkerThreadId.load())
            gWrongThread.fetch_add(1);
    }
}

//!< A client of the service in the worker thread. Runs one exchange when it is connected.
class WorkerClient final : public TempMonitorServiceConsumerBase
{
public:
    WorkerClient(uint32_t slot, areg::DispatcherThread & workerThread)
        : TempMonitorServiceConsumerBase(ROLE_NAME, workerThread)
        , mSlot(slot)
    {
        check_thread();
    }

    virtual ~WorkerClient() override
    {
        check_thread();
    }

    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        check_thread();
        const bool relevant{ TempMonitorServiceConsumerBase::service_connected(status, proxy) };
        if (relevant && areg::is_service_connected(status))
        {
            gClients[mSlot].connects.fetch_add(1);
            notify_on_reading_update(true);
            notify_on_broadcast_alarm_raised(true);
            request_get_statistics();
            request_set_thresholds(HIGH_LIMIT, 1);
        }
        else if (relevant)
        {
            gClients[mSlot].disconnects.fetch_add(1);
        }

        return relevant;
    }

    void on_reading_update(int16_t Reading, areg::DataState state) override
    {
        check_thread();
        if (state == areg::DataState::DataIsOK)
            gClients[mSlot].reading.store(Reading);
    }

    void broadcast_alarm_raised(int16_t /*reading*/) override
    {
        check_thread();
        gClients[mSlot].broadcasts.fetch_add(1);
    }

    void response_get_statistics(int16_t /*lowest*/, int16_t /*highest*/, uint32_t /*alarms_raised*/) override
    {
        check_thread();
        gClients[mSlot].statistics.fetch_add(1);
    }

    void response_set_thresholds(bool /*accepted*/, const areg::String & /*reason*/) override
    {
        check_thread();
        gClients[mSlot].thresholds.fetch_add(1);
    }

    void request_get_statistics_failed(areg::ResultType /*reason*/) override
    {
        gClients[mSlot].failures.fetch_add(1);
    }

    void request_set_thresholds_failed(areg::ResultType /*reason*/) override
    {
        gClients[mSlot].failures.fetch_add(1);
    }

private:
    const uint32_t  mSlot;
};

//!< Asks the worker to create the client of a round.
class RoundData
{
public:
    RoundData() = default;
    explicit RoundData(uint32_t slot) : mSlot(slot) {}
    RoundData(const RoundData & src) = default;
    RoundData & operator = (const RoundData & src) = default;

    inline uint32_t slot() const { return mSlot; }

private:
    uint32_t    mSlot{ 0 };
};

AREG_DECLARE_EVENT(RoundData, RoundEvent, RoundEventConsumer);

//!< Runs in the worker thread: creates, keeps and deletes the clients of the case.
class ClientWorker final : public    areg::WorkerThreadConsumer
                         , public    RoundEventConsumer
{
public:
    explicit ClientWorker(const areg::String & consumerName)
        : areg::WorkerThreadConsumer( consumerName )
        , mThread                   ( nullptr )
        , mClients                  { }
        , mMember                   ( )
    {
    }

protected:
    void register_event_consumers(areg::WorkerThread & workThread, areg::ComponentThread & /*masterThread*/) override
    {
        mThread = &workThread;
        gWorkerThreadId.store(areg::Thread::current_thread_id());
        RoundEvent::add_listener(static_cast<RoundEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(workThread));
        gWorkerReady.store(true);

        if ((gCase == TestCase::Heap) || (gCase == TestCase::Reload) || (gCase == TestCase::Late))
            mClients[0] = new WorkerClient(0, workThread);
        else if (gCase == TestCase::Member)
            mMember.emplace(0, workThread);
    }

    void unregister_event_consumers(areg::WorkerThread & workThread) override
    {
        RoundEvent::remove_listener(static_cast<RoundEventConsumer &>(*this), static_cast<areg::DispatcherThread &>(workThread));
        for (WorkerClient *& client : mClients)
        {
            delete client;
            client = nullptr;
        }

        mMember.reset();
        mThread = nullptr;
    }

    void process_event(const RoundData & data) override
    {
        const uint32_t slot{ data.slot() };
        if ((mThread == nullptr) || (slot >= MAX_CLIENTS))
            return;

        if ((gCase == TestCase::OnTheFly) && (slot > 0))
        {
            delete mClients[slot - 1];
            mClients[slot - 1] = nullptr;
        }

        mClients[slot] = new WorkerClient(slot, *mThread);
        gRoundsCreated.fetch_add(1);
    }

private:
    areg::WorkerThread *        mThread;
    WorkerClient *              mClients[MAX_CLIENTS];
    std::optional<WorkerClient> mMember;
};

//!< Owns the worker thread of the test. Provides and consumes nothing itself.
class WorkerHost final : public areg::Component
{
public:
    WorkerHost(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component   ( entry, owner )
        , mWorker           ( entry.mWorkerThreads[0].mConsumerName )
    {
    }

    areg::WorkerThreadConsumer * worker_thread_consumer(const areg::String & consumerName, const areg::String & workerThreadName) override
    {
        return (mWorker.consumer_name() == consumerName) ? &mWorker : areg::Component::worker_thread_consumer(consumerName, workerThreadName);
    }

private:
    ClientWorker    mWorker;
};

BEGIN_MODEL("WorkerConsumerModel")
    BEGIN_REGISTER_THREAD( "WorkerHostThread" )
        BEGIN_REGISTER_COMPONENT( "WorkerHost", WorkerHost )
            REGISTER_WORKER_THREAD( "ClientWorkerThread", "ClientWorker" )
        END_REGISTER_COMPONENT( "WorkerHost" )
    END_REGISTER_THREAD( "WorkerHostThread" )
END_MODEL("WorkerConsumerModel")

namespace
{
    //!< One client created when the worker starts: exchange, then deleted when the worker stops.
    bool run_single()
    {
        reset_counters();
        areg::Application::load_model("WorkerConsumerModel");
        bool ok{ wait_for([]() { return exchange_done(0); }) };
        areg::Application::unload_model("WorkerConsumerModel");
        ok = check_client(0) && ok;
        return ok;
    }

    bool run_on_the_fly()
    {
        reset_counters();
        areg::Application::load_model("WorkerConsumerModel");
        bool ok{ wait_for([]() { return gWorkerReady.load(); }) };
        for (uint32_t round = 0; (round < FLY_ROUNDS) && ok; ++round)
        {
            ok = RoundEvent::send_event(RoundData(round));
            ok = ok && wait_for([round]() { return exchange_done(round); });
            if (ok == false)
                std::printf("round %u did not complete\n", round);
        }

        areg::Application::unload_model("WorkerConsumerModel");
        ok = expect("rounds created", 0, gRoundsCreated.load(), FLY_ROUNDS) && ok;
        for (uint32_t round = 0; round < FLY_ROUNDS; ++round)
            ok = check_client(round) && ok;

        return ok;
    }

    bool run_late()
    {
        reset_counters();
        areg::Application::load_model("WorkerConsumerModel");
        bool ok{ wait_for([]() { return exchange_done(0); }) };
        for (uint32_t slot = 1; (slot <= LATE_CLIENTS) && ok; ++slot)
        {
            ok = RoundEvent::send_event(RoundData(slot));
            ok = ok && wait_for([slot]() { return exchange_done(slot); });
            if (ok == false)
                std::printf("late client %u did not complete\n", slot);
        }

        areg::Application::unload_model("WorkerConsumerModel");
        // A client gets the broadcast of its own request and of every later client's request.
        for (uint32_t slot = 0; slot <= LATE_CLIENTS; ++slot)
            ok = check_client(slot, LATE_CLIENTS + 1 - slot) && ok;

        return ok;
    }
}

int main(int argc, char * argv[])
{
    const char * name{ argc > 1 ? argv[1] : "" };
    if (std::strcmp(name, "heap") == 0)
        gCase = TestCase::Heap;
    else if (std::strcmp(name, "member") == 0)
        gCase = TestCase::Member;
    else if (std::strcmp(name, "on-the-fly") == 0)
        gCase = TestCase::OnTheFly;
    else if (std::strcmp(name, "late") == 0)
        gCase = TestCase::Late;
    else if (std::strcmp(name, "reload") == 0)
        gCase = TestCase::Reload;

    if (gCase == TestCase::None)
    {
        std::printf("Usage: areg-worker-proxy-test <heap|member|on-the-fly|late|reload>\n");
        return 2;
    }

    areg::Application::setup();
    areg::Application::load_model("WorkerProviderModel");

    bool ok{ false };
    switch (gCase)
    {
    case TestCase::Heap:
    case TestCase::Member:
        ok = run_single();
        break;

    case TestCase::Reload:
        ok = run_single();
        ok = run_single() && ok;
        break;

    case TestCase::OnTheFly:
        ok = run_on_the_fly();
        break;

    case TestCase::Late:
        ok = run_late();
        break;

    default:
        break;
    }

    areg::Application::unload_model("WorkerProviderModel");
    areg::Application::release();

    if (gWrongThread.load() != 0)
    {
        std::printf("callbacks outside the worker thread: %u\n", gWrongThread.load());
        ok = false;
    }

    std::printf("%s: %s\n", name, ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
