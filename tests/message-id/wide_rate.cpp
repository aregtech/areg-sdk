/************************************************************************
 * Round trips and attribute updates over the generated WideService interface (256 requests,
 * each with its own response, and 64 attributes), provider and consumer in two threads of one
 * process. Every request and every attribute is used in turn.
 *
 * Usage: areg-message-id-rate [-m=pp|win|attr] [-c=<count>] [-w=<warmup>] [-r=<runs>]
 *   pp     one request outstanding; latency of each round trip
 *   win    64 different requests outstanding; round trips per second
 *   attr   each request makes the provider update 8 attributes; updates per second
 *
 * Prints one line per run:
 *   run <n> mode <m> samples <s> min <us> p50 <us> p95 <us> p99 <us> max <us> mean <us> dur <ms> ms rate <r> msg/s
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "tests/message-id/services/WideServiceConsumerBase.hpp"
#include "tests/message-id/services/WideServiceProviderBase.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "message_id_wide_generated")
#endif // _MSC_VER

#define WR_HEX16(M, h)  M(h##0) M(h##1) M(h##2) M(h##3) M(h##4) M(h##5) M(h##6) M(h##7) \
                        M(h##8) M(h##9) M(h##a) M(h##b) M(h##c) M(h##d) M(h##e) M(h##f)
#define WR_REQUESTS(M)  WR_HEX16(M, 0) WR_HEX16(M, 1) WR_HEX16(M, 2) WR_HEX16(M, 3) \
                        WR_HEX16(M, 4) WR_HEX16(M, 5) WR_HEX16(M, 6) WR_HEX16(M, 7) \
                        WR_HEX16(M, 8) WR_HEX16(M, 9) WR_HEX16(M, a) WR_HEX16(M, b) \
                        WR_HEX16(M, c) WR_HEX16(M, d) WR_HEX16(M, e) WR_HEX16(M, f)
#define WR_ATTRIBUTES(M) WR_HEX16(M, 0) WR_HEX16(M, 1) WR_HEX16(M, 2) WR_HEX16(M, 3)

namespace
{
    constexpr char const    _model[]        { "WrModel" };
    constexpr char const    _providerThread[]{ "WrProviderThread" };
    constexpr char const    _consumerThread[]{ "WrConsumerThread" };
    constexpr char const    _providerRole[] { "WrProvider" };
    constexpr char const    _consumerRole[] { "WrConsumer" };

    constexpr uint32_t      REQUEST_COUNT   { 256 };
    constexpr uint32_t      ATTRIBUTE_COUNT { 64 };
    constexpr uint32_t      WINDOW          { 64 };
    constexpr uint32_t      ATTR_PER_CALL   { 8 };
    constexpr uint32_t      WAIT_MS         { 600000 };

    enum class Mode : uint32_t { PingPong, Window, Attribute };

    Mode        gMode   { Mode::PingPong };
    uint32_t    gCount  { 200000 };
    uint32_t    gWarmup { 20000 };
    uint32_t    gRuns   { 1 };

    std::mutex              gLock;
    std::condition_variable gChanged;
    bool                    gFinished   { false };

    using Clock = std::chrono::steady_clock;
}

//////////////////////////////////////////////////////////////////////////
// Provider
//////////////////////////////////////////////////////////////////////////

//!< Answers every request at once; in attribute mode it first updates attributes.
class WrProvider final  : public    areg::Component
                        , protected WideServiceProviderBase
{
public:
    WrProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , WideServiceProviderBase   ( static_cast<areg::Component &>(*this) )
    {
    }

protected:
    void startup_service_interface(areg::Component & holder) override
    {
        WideServiceProviderBase::startup_service_interface(holder);
#define WR_INIT_ATTR(n) set_value##n(0u);
        WR_ATTRIBUTES(WR_INIT_ATTR)
#undef WR_INIT_ATTR
    }

#define WR_PROVIDER_REQUEST(n)                          \
    void request_call_##n(uint32_t value) override      \
    {                                                   \
        if (gMode == Mode::Attribute)                   \
            update_attributes(value);                   \
        response_call_##n(value);                       \
    }
    WR_REQUESTS(WR_PROVIDER_REQUEST)
#undef WR_PROVIDER_REQUEST

private:
    void update_attributes(uint32_t value)
    {
        using Setter = void (WrProvider::*)(const uint32_t &);
        static const Setter setters[ATTRIBUTE_COUNT]
        {
#define WR_SETTER(n) &WrProvider::set_value##n,
            WR_ATTRIBUTES(WR_SETTER)
#undef WR_SETTER
        };

        for (uint32_t i = 0; i < ATTR_PER_CALL; ++i)
        {
            (this->*setters[mNextAttr])(value);
            mNextAttr = (mNextAttr + 1u) % ATTRIBUTE_COUNT;
        }
    }

    uint32_t    mNextAttr{ 0u };    //!< Index of the next attribute to update.
};

//////////////////////////////////////////////////////////////////////////
// Consumer
//////////////////////////////////////////////////////////////////////////

//!< Sends the requests in turn and measures each run.
class WrConsumer final  : public    areg::Component
                        , protected WideServiceConsumerBase
{
    using Sender = SequenceNumber (WrConsumer::*)(uint32_t);

public:
    WrConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component           ( entry, owner )
        , WideServiceConsumerBase   ( _providerRole, static_cast<areg::Component &>(*this) )
    {
        mSent.resize(REQUEST_COUNT);
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) override
    {
        const bool result{ WideServiceConsumerBase::service_connected(status, proxy) };
        if (!areg::is_service_connected(status) || mStarted)
            return result;

        mStarted = true;
        if (gMode == Mode::Attribute)
        {
#define WR_SUBSCRIBE(n) notify_on_value##n##_update(true);
            WR_ATTRIBUTES(WR_SUBSCRIBE)
#undef WR_SUBSCRIBE
        }

        start_run();
        return result;
    }

#define WR_CONSUMER_RESPONSE(n)                         \
    void response_call_##n(uint32_t value) override     \
    {                                                   \
        on_response(value);                             \
    }
    WR_REQUESTS(WR_CONSUMER_RESPONSE)
#undef WR_CONSUMER_RESPONSE

#define WR_CONSUMER_UPDATE(n)                                           \
    void on_value##n##_update(uint32_t, areg::DataState state) override \
    {                                                                   \
        if (state == areg::DataState::DataIsOK)                         \
            ++ mUpdates;                                                \
    }
    WR_ATTRIBUTES(WR_CONSUMER_UPDATE)
#undef WR_CONSUMER_UPDATE

private:
    void send_next()
    {
        static const Sender senders[REQUEST_COUNT]
        {
#define WR_SENDER(n) &WrConsumer::request_call_##n,
            WR_REQUESTS(WR_SENDER)
#undef WR_SENDER
        };

        const uint32_t index{ mNextReq };
        mNextReq = (mNextReq + 1u) % REQUEST_COUNT;
        mSent[index] = Clock::now();
        ++ mInFlight;
        (this->*senders[index])(index);
    }

    void start_run()
    {
        mDone       = 0u;
        mUpdates    = 0u;
        mStart      = Clock::now();
        mLatency.clear();
        mLatency.reserve(gCount);
        const uint32_t window{ gMode == Mode::Window ? WINDOW : 1u };
        for (uint32_t i = 0; i < window; ++i)
        {
            send_next();
        }
    }

    void on_response(uint32_t index)
    {
        -- mInFlight;
        ++ mDone;
        const uint32_t total{ gWarmup + gCount };
        if (mDone == gWarmup)
        {
            mStart      = Clock::now();
            mUpdates    = 0u;
        }

        if ((mDone > gWarmup) && (gMode == Mode::PingPong))
        {
            mLatency.push_back(static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - mSent[index]).count()));
        }

        if ((mDone + mInFlight) < total)
        {
            send_next();
        }
        else if (mInFlight == 0u)
        {
            report(Clock::now());
            if (++ mRun < gRuns)
            {
                start_run();
            }
            else
            {
                { std::lock_guard<std::mutex> lock(gLock); gFinished = true; }
                gChanged.notify_all();
            }
        }
    }

    void report(Clock::time_point end)
    {
        const double durMs{ std::chrono::duration<double, std::milli>(end - mStart).count() };
        const double events{ gMode == Mode::Attribute ? static_cast<double>(mUpdates) : static_cast<double>(gCount) };
        const double rate{ durMs > 0.0 ? events * 1000.0 / durMs : 0.0 };
        double minUs{ 0.0 }, p50{ 0.0 }, p95{ 0.0 }, p99{ 0.0 }, maxUs{ 0.0 }, mean{ 0.0 };
        if (!mLatency.empty())
        {
            std::sort(mLatency.begin(), mLatency.end());
            const size_t n{ mLatency.size() };
            const auto at = [this, n](double q) { return static_cast<double>(mLatency[std::min(n - 1, static_cast<size_t>(q * static_cast<double>(n)))]) / 1000.0; };
            double sum{ 0.0 };
            for (uint32_t v : mLatency)
                sum += static_cast<double>(v);
            minUs = static_cast<double>(mLatency.front()) / 1000.0;
            maxUs = static_cast<double>(mLatency.back()) / 1000.0;
            p50 = at(0.50);
            p95 = at(0.95);
            p99 = at(0.99);
            mean = sum / static_cast<double>(n) / 1000.0;
        }

        static const char * const names[]{ "pp", "win", "attr" };
        printf("run %u mode %s samples %u min %.3f p50 %.3f p95 %.3f p99 %.3f max %.3f mean %.3f dur %.3f ms rate %.1f msg/s\n"
              , mRun + 1u, names[static_cast<uint32_t>(gMode)], gCount, minUs, p50, p95, p99, maxUs, mean, durMs, rate);
    }

    std::vector<Clock::time_point>  mSent;              //!< Send time per request index.
    std::vector<uint32_t>           mLatency;           //!< Round trip of each measured response, ns.
    Clock::time_point               mStart;             //!< Send time of the first measured request.
    uint32_t                        mNextReq{ 0u };     //!< Index of the next request to send.
    uint32_t                        mInFlight{ 0u };    //!< Requests sent and not answered.
    uint32_t                        mDone{ 0u };        //!< Responses received in this run.
    uint32_t                        mUpdates{ 0u };     //!< Attribute updates received while measuring.
    uint32_t                        mRun{ 0u };         //!< Index of the current run.
    bool                            mStarted{ false };  //!< The first run was started.
};

//////////////////////////////////////////////////////////////////////////
// Model
//////////////////////////////////////////////////////////////////////////

namespace wr_model
{
BEGIN_MODEL(_model)
    BEGIN_REGISTER_THREAD(_providerThread)
        BEGIN_REGISTER_COMPONENT(_providerRole, WrProvider)
            REGISTER_IMPLEMENT_SERVICE(WideService::ServiceName, WideService::InterfaceVersion)
        END_REGISTER_COMPONENT(_providerRole)
    END_REGISTER_THREAD(_providerThread)

    BEGIN_REGISTER_THREAD(_consumerThread)
        BEGIN_REGISTER_COMPONENT(_consumerRole, WrConsumer)
            REGISTER_DEPENDENCY(_providerRole)
        END_REGISTER_COMPONENT(_consumerRole)
    END_REGISTER_THREAD(_consumerThread)
END_MODEL(_model)
}

static uint32_t option_value(const char * arg, const char * key, uint32_t current)
{
    const size_t len{ std::strlen(key) };
    return (std::strncmp(arg, key, len) == 0) ? static_cast<uint32_t>(std::strtoul(arg + len, nullptr, 10)) : current;
}

int main(int argc, char * argv[])
{
    setvbuf(stdout, nullptr, _IONBF, 0);
    for (int i = 1; i < argc; ++i)
    {
        const char * arg{ argv[i] };
        if (std::strcmp(arg, "-m=win") == 0)
            gMode = Mode::Window;
        else if (std::strcmp(arg, "-m=attr") == 0)
            gMode = Mode::Attribute;
        else if (std::strcmp(arg, "-m=pp") == 0)
            gMode = Mode::PingPong;
        gCount  = option_value(arg, "-c=", gCount);
        gWarmup = option_value(arg, "-w=", gWarmup);
        gRuns   = option_value(arg, "-r=", gRuns);
    }

    if ((gCount == 0u) || (gRuns == 0u))
    {
        printf("count and runs must be above 0\n");
        return 2;
    }

    areg::Application::setup(false, true, false, true, false, nullptr);
    bool ok{ areg::Application::load_model(_model) };
    if (ok)
    {
        std::unique_lock<std::mutex> lock(gLock);
        ok = gChanged.wait_for(lock, std::chrono::milliseconds(WAIT_MS), [] { return gFinished; });
    }

    areg::Application::unload_model(_model);
    areg::Application::release();
    if (!ok)
    {
        printf("FAIL: not finished\n");
    }

    return ok ? 0 : 1;
}
