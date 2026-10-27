/************************************************************************
 * A service provider and its consumer in one component thread, whose queue can be full.
 *
 * Usage: areg-same-thread-test <case>
 *   request-reply   the consumer sends requests one after another and gets every response
 *   full-request    the consumer fills its own queue and sends a request: the request is
 *                   dropped at once, and the thread answers the next request
 *   full-response   another thread fills the queue while the provider handles a request: the
 *                   response is dropped at once, and the thread answers the next request
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/EventTemplate.hpp"
#include "examples/02_minimalipc/services/HelloServiceConsumerBase.hpp"
#include "examples/02_minimalipc/services/HelloServiceProviderBase.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "02_generated")
#endif // _MSC_VER

namespace
{
    constexpr char const    _model[]        { "StModel" };
    constexpr char const    _thread[]       { "StOneThread" };
    constexpr char const    _providerRole[] { "StProvider" };
    constexpr char const    _consumerRole[] { "StConsumer" };

    constexpr uint32_t      WAIT_MS         { 5000 };
    constexpr uint32_t      QUEUE_SIZE      { areg::QUEUE_MIN_RING_CAPACITY };
    constexpr uint32_t      QUEUE_WAIT_MS   { 2000 };
    constexpr uint32_t      POST_LIMIT_MS   { 500 };
    constexpr uint32_t      FILL_LIMIT      { 2 * QUEUE_SIZE };
    constexpr uint32_t      REQUESTS        { 10 };

    enum class Mode : uint32_t { RequestReply, FullRequest, FullResponse };

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

    //!< Milliseconds since the given point.
    uint32_t _elapsed_ms(const std::chrono::steady_clock::time_point & since)
    {
        return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - since).count());
    }

    std::atomic<Mode>       gMode       { Mode::RequestReply };
    std::atomic_uint32_t    gResponses  { 0 };
    std::atomic_uint32_t    gFailures   { 0 };
    std::atomic_uint32_t    gFilled     { 0 };      //!< The fill events the queue took.
    std::atomic_uint32_t    gFills      { 0 };      //!< The fill events delivered.
    std::atomic_uint32_t    gPostMs     { 0 };      //!< How long the post to the full queue took.
    std::atomic_bool        gLastFill   { true };   //!< What the post of a fill event to the full queue returned.

    Gate                    gInRequest;
    Gate                    gQueueFull;
    Gate                    gDone;
}

//////////////////////////////////////////////////////////////////////////
// Events
//////////////////////////////////////////////////////////////////////////

//!< An event that only takes a slot in the queue.
class FillData
{
public:
    FillData() = default;
    FillData(const FillData & /*src*/) = default;
    FillData & operator = (const FillData & /*src*/) = default;
};

AREG_DECLARE_EVENT(FillData, FillEvent, FillConsumer);

namespace
{
    std::atomic<areg::ComponentThread *>    gThread     { nullptr };
    std::atomic<FillConsumer *>             gConsumer   { nullptr };
}

//////////////////////////////////////////////////////////////////////////
// Components
//////////////////////////////////////////////////////////////////////////

//!< Answers every request; in full-response it answers the first one after the queue is full.
class StProvider final : public    areg::Component
                       , protected HelloServiceProviderBase
{
public:
    StProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceProviderBase  ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void request_hello_service() override
    {
        if ((gMode.load() == Mode::FullResponse) && (mBlocked == false))
        {
            mBlocked = true;
            gInRequest.advance();
            static_cast<void>(gQueueFull.wait_for(1, WAIT_MS + QUEUE_WAIT_MS));

            const auto start{ std::chrono::steady_clock::now() };
            response_hello_service();
            gPostMs.store(_elapsed_ms(start));
            return;
        }

        response_hello_service();
    }

private:
    bool    mBlocked{ false };  //!< The first request has waited for the full queue.
};

//!< Sends a request on the connection and then as each case says.
class StConsumer final : public    areg::Component
                       , protected HelloServiceConsumerBase
                       , public    FillConsumer
{
public:
    StConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , HelloServiceConsumerBase  ( _providerRole, static_cast<areg::Component &>(*this) )
        , FillConsumer              ( )
    {
    }

    void startup_component(areg::ComponentThread & comThread) override
    {
        areg::Component::startup_component(comThread);
        gThread.store(&comThread);
        gConsumer.store(static_cast<FillConsumer *>(this));
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ HelloServiceConsumerBase::service_connected(status, proxy) };
        if (areg::is_service_connected(status))
        {
            request_hello_service();
        }

        return result;
    }

    void response_hello_service() override
    {
        const uint32_t responses{ gResponses.fetch_add(1) + 1 };
        if ((gMode.load() == Mode::RequestReply) && (responses < REQUESTS))
        {
            request_hello_service();
        }
        else if ((gMode.load() == Mode::FullRequest) && (responses == 1))
        {
            _fill_and_request();
        }
        else
        {
            gDone.advance();
        }
    }

    void request_hello_service_failed(areg::ResultType /*reason*/) override
    {
        gFailures.fetch_add(1);
    }

    //!< Once the last fill event is delivered, the queue is drained and the next request is sent.
    void process_event(const FillData & /*data*/) override
    {
        if ((gFills.fetch_add(1) + 1) == gFilled.load())
        {
            request_hello_service();
        }
    }

private:
    //!< Fills the queue of this thread with its own events, then sends a request into the full queue.
    void _fill_and_request()
    {
        areg::ComponentThread & thread{ *gThread.load() };
        uint32_t filled{ 0 };
        bool posted{ true };
        while (posted && (filled < FILL_LIMIT))
        {
            posted = FillEvent::send_event(FillData(), static_cast<FillConsumer &>(*this), thread);
            filled += posted ? 1 : 0;
        }

        gLastFill.store(posted);
        gFilled.store(filled);

        const auto start{ std::chrono::steady_clock::now() };
        request_hello_service();
        gPostMs.store(_elapsed_ms(start));
    }
};

//////////////////////////////////////////////////////////////////////////
// Model
//////////////////////////////////////////////////////////////////////////

BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD_EX2(_thread, areg::WATCHDOG_IGNORE, areg::DEFAULT_STACK_SIZE, QUEUE_SIZE, areg::Bool::False, QUEUE_WAIT_MS)
        BEGIN_REGISTER_COMPONENT(_providerRole, StProvider)
            REGISTER_IMPLEMENT_SERVICE(HelloService::ServiceName, HelloService::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
        BEGIN_REGISTER_COMPONENT(_consumerRole, StConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_thread)
END_MODEL(_model)

//////////////////////////////////////////////////////////////////////////
// Cases
//////////////////////////////////////////////////////////////////////////

static void _print_counts()
{
    printf("responses ................: %u\n", gResponses.load());
    printf("failures .................: %u\n", gFailures.load());
    printf("fill events queued .......: %u\n", gFilled.load());
    printf("fill events delivered ....: %u\n", gFills.load());
}

static bool request_reply()
{
    gMode.store(Mode::RequestReply);
    bool ok{ areg::Application::load_model(_model) };
    ok = ok && gDone.wait_for(1);
    areg::Application::unload_model(_model);

    _print_counts();
    return ok && (gResponses.load() == REQUESTS) && (gFailures.load() == 0);
}

static bool full_request()
{
    gMode.store(Mode::FullRequest);
    bool ok{ areg::Application::load_model(_model) };
    ok = ok && gDone.wait_for(1, WAIT_MS + 3 * QUEUE_WAIT_MS);
    areg::Application::unload_model(_model);

    _print_counts();
    printf("post of the fill event ...: %s\n", gLastFill.load() ? "true" : "false");
    printf("request to the full queue : %u ms (limit %u ms, queue wait %u ms)\n", gPostMs.load(), POST_LIMIT_MS, QUEUE_WAIT_MS);
    return ok
        && (gLastFill.load() == false)
        && (gFilled.load() != 0)
        && (gFills.load() == gFilled.load())
        && (gResponses.load() == 2)
        && (gPostMs.load() < POST_LIMIT_MS);
}

static bool full_response()
{
    gMode.store(Mode::FullResponse);
    bool ok{ areg::Application::load_model(_model) };
    ok = ok && gInRequest.wait_for(1);

    // This thread fills the queue; its post that finds the queue full waits for a slot and fails.
    uint32_t filled{ 0 };
    uint32_t waitMs{ 0 };
    bool posted{ true };
    while (ok && posted && (filled < FILL_LIMIT))
    {
        const auto start{ std::chrono::steady_clock::now() };
        posted = FillEvent::send_event(FillData(), *gConsumer.load(), *gThread.load());
        waitMs = _elapsed_ms(start);
        filled += posted ? 1 : 0;
    }

    gFilled.store(filled);
    gQueueFull.advance();
    ok = ok && gDone.wait_for(1);
    areg::Application::unload_model(_model);

    _print_counts();
    printf("foreign post to full queue: %s after %u ms\n", posted ? "true" : "false", waitMs);
    printf("response to the full queue: %u ms (limit %u ms)\n", gPostMs.load(), POST_LIMIT_MS);
    return ok
        && (posted == false)
        && (waitMs + 50 >= QUEUE_WAIT_MS)
        && (filled != 0)
        && (gFills.load() == filled)
        && (gResponses.load() == 1)
        && (gPostMs.load() < POST_LIMIT_MS);
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);

    const char * name{ argc > 1 ? argv[1] : "request-reply" };
    areg::Application::setup(true, true, false, true, false);

    bool ok{ false };
    if (std::strcmp(name, "request-reply") == 0)
    {
        ok = request_reply();
    }
    else if (std::strcmp(name, "full-request") == 0)
    {
        ok = full_request();
    }
    else if (std::strcmp(name, "full-response") == 0)
    {
        ok = full_response();
    }
    else
    {
        printf("unknown case [ %s ]\n", name);
        areg::Application::release();
        return 2;
    }

    areg::Application::release();

    printf("case .....................: %s\n", name);
    printf("%s\n", ok ? "PASS" : "FAIL");
    return ok ? 0 : 1;
}
