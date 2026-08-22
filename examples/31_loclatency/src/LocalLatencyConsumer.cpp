/************************************************************************
 * \file        src/LocalLatencyConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Service consumer of the local latency benchmark (example 31).
 ************************************************************************/

#include "src/LocalLatencyConsumer.hpp"

#include "areg/appbase/Application.hpp"
#include "common/common.hpp"

#include <cstdio>

namespace
{
    //!< Set once when the watchdog ends the session. Read by main() to pick the exit code.
    std::atomic_bool _theStalled{ false };
}

bool LocalLatencyConsumer::has_stalled() noexcept
{
    return _theStalled.load(std::memory_order_relaxed);
}

LocalLatencyConsumer::LocalLatencyConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component           (entry, owner)
    , LocalLatencyConsumerBase  (entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this))

    , mOptions                  (loclat::run_options())
    , mReport                   ( )
    , mResults                  ( )
    , mModeIndex                (0u)
    , mRepeat                   (1u)
    , mMode                     (LocalLatency::LatencyMode::Undefined)
    , mArrived                  (0u)
    , mRunBeginNs               (0u)
    , mClockCostNs              (0.0)
    , mRunning                  (false)
    , mProgress                 (0u)
    , mSessionDone              (false)
    , mWatchdogConsumer         (*this)
    , mWatchdogThread           (static_cast<areg::ThreadConsumer &>(mWatchdogConsumer), THREAD_WATCHDOG)
{
}

void LocalLatencyConsumer::startup_component(areg::ComponentThread & /* comThread */)
{
    mResults.reserve(mOptions.mModes.size() * mOptions.mRepeat);
    mClockCostNs = loclat::measure_clock_cost();
    mWatchdogThread.start(areg::WAIT_INFINITE);
}

void LocalLatencyConsumer::shutdown_component(areg::ComponentThread & /* comThread */) noexcept
{
    mRunning = false;
    mSessionDone.store(true, std::memory_order_relaxed);
    mWatchdogThread.shutdown(areg::WAIT_INFINITE);
}

bool LocalLatencyConsumer::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    const bool result{ LocalLatencyConsumerBase::service_connected(status, proxy) };

    if (is_connected())
    {
        // The notifications are switched on before the first pull call. Both travel the
        // same path in the same order, so the first broadcast can never be missed.
        notify_on_broadcast_message_0(true);
        notify_on_broadcast_message_8(true);
        notify_on_broadcast_message_16(true);
        notify_on_broadcast_message_32(true);
        notify_on_broadcast_message_64(true);
        notify_on_broadcast_message_128(true);
        notify_on_broadcast_message_256(true);
        notify_on_broadcast_message_512(true);
        notify_on_broadcast_message_1024(true);
        notify_on_broadcast_message_4096(true);
        notify_on_broadcast_message_65536(true);

        _begin_session();
    }
    else if (!mSessionDone.load(std::memory_order_relaxed))
    {
        std::printf("ERROR: the service provider became unreachable while measuring.\n");
        _theStalled.store(true, std::memory_order_relaxed);
        mRunning = false;
        mSessionDone.store(true, std::memory_order_relaxed);
        areg::Application::signal_quit();
    }

    return result;
}

void LocalLatencyConsumer::_begin_session()
{
    if (mRunning || mSessionDone.load(std::memory_order_relaxed))
        return;

    if (!mOptions.mQuiet)
    {
        std::printf("Session     : topology=%s modes=%u repeat=%u count=%u warmup=%u\n"
                  , loclat::topology_as_str(mOptions.mTopology)
                  , static_cast<uint32_t>(mOptions.mModes.size())
                  , mOptions.mRepeat
                  , mOptions.mCount
                  , mOptions.mWarmup);
        loclat::print_environment(mClockCostNs);
    }

    loclat::print_table_header();
    _start_run();
}

void LocalLatencyConsumer::_start_run()
{
    mMode       = mOptions.mModes[mModeIndex];
    mArrived    = 0u;
    mRunBeginNs = 0u;
    mRunning    = true;
    mReport.restart(mOptions.mCount);

    _send_next();
}

void LocalLatencyConsumer::_finish_run(uint64_t endNs)
{
    mRunning = false;

    const loclat::RunResult result
    {
        mReport.summarize(mOptions, mMode, mRepeat, endNs - mRunBeginNs, mClockCostNs)
    };

    mResults.push_back(result);
    loclat::print_table_row(result);
    loclat::append_csv(mOptions.mCsvPath, mOptions.mLabel, result);

    ++mModeIndex;
    if (mModeIndex >= mOptions.mModes.size())
    {
        mModeIndex = 0u;
        ++mRepeat;
    }

    if (mRepeat > mOptions.mRepeat)
        _end_session();
    else
        _start_run();
}

void LocalLatencyConsumer::_end_session()
{
    mSessionDone.store(true, std::memory_order_relaxed);

    if (!mOptions.mQuiet)
    {
        std::printf("\nFinished    : %u run(s) completed.\n", static_cast<uint32_t>(mResults.size()));
        if (!mOptions.mCsvPath.is_empty())
            std::printf("CSV         : %s\n", mOptions.mCsvPath.as_string());
    }

    std::fflush(stdout);
    areg::Application::signal_quit();
}

void LocalLatencyConsumer::_send_next()
{
    if (!mRunning || !is_connected())
        return;

    switch (mMode)
    {
    case LocalLatency::LatencyMode::Request0:
        request_ping_pong_0(mArrived, loclat::now_ns());
        break;
    case LocalLatency::LatencyMode::Request8:
        request_ping_pong_8(mArrived, loclat::now_ns(), Latency::latency8());
        break;
    case LocalLatency::LatencyMode::Request16:
        request_ping_pong_16(mArrived, loclat::now_ns(), Latency::latency16());
        break;
    case LocalLatency::LatencyMode::Request32:
        request_ping_pong_32(mArrived, loclat::now_ns(), Latency::latency32());
        break;
    case LocalLatency::LatencyMode::Request64:
        request_ping_pong_64(mArrived, loclat::now_ns(), Latency::latency64());
        break;
    case LocalLatency::LatencyMode::Request128:
        request_ping_pong_128(mArrived, loclat::now_ns(), Latency::latency128());
        break;
    case LocalLatency::LatencyMode::Request256:
        request_ping_pong_256(mArrived, loclat::now_ns(), Latency::latency256());
        break;
    case LocalLatency::LatencyMode::Request512:
        request_ping_pong_512(mArrived, loclat::now_ns(), Latency::latency512());
        break;
    case LocalLatency::LatencyMode::Request1024:
        request_ping_pong_1024(mArrived, loclat::now_ns(), Latency::latency1024());
        break;
    case LocalLatency::LatencyMode::Request4096:
        request_ping_pong_4096(mArrived, loclat::now_ns(), Latency::latency4096());
        break;
    case LocalLatency::LatencyMode::Request65536:
        request_ping_pong_65536(mArrived, loclat::now_ns(), Latency::latency65536());
        break;
    default:
        request_message_next(mMode, mArrived);
        break;
    }
}

void LocalLatencyConsumer::_on_arrival(uint64_t begin, uint64_t replied)
{
    // The arrival timestamp is the first thing taken: every instruction before it would
    // be counted as message latency.
    const uint64_t end{ loclat::now_ns() };

    if (!mRunning)
        return;

    ++mArrived;
    mProgress.fetch_add(1u, std::memory_order_relaxed);

    if (mArrived <= mOptions.mWarmup)
    {
        _send_next();
        return;
    }

    if (mReport.count() == 0u)
        mRunBeginNs = begin;

    mReport.add_sample( static_cast<int64_t>(end - begin)
                      , replied != 0u ? static_cast<int64_t>(replied - begin) : 0 );

    if (mReport.count() >= mOptions.mCount)
        _finish_run(end);
    else
        _send_next();
}

void LocalLatencyConsumer::WatchdogConsumer::on_run()
{
    mOwner._run_watchdog();
}

void LocalLatencyConsumer::_run_watchdog()
{
    uint64_t lastSeen{ 0u };
    uint32_t idleTicks{ 0u };

    while (!mSessionDone.load(std::memory_order_relaxed))
    {
        areg::Thread::sleep(WATCHDOG_TICK_MS);

        const uint64_t seen{ mProgress.load(std::memory_order_relaxed) };
        if (seen != lastSeen)
        {
            lastSeen  = seen;
            idleTicks = 0u;
            continue;
        }

        if (++idleTicks < WATCHDOG_STALL_TICKS)
            continue;

        if (mSessionDone.load(std::memory_order_relaxed))
            break;

        std::printf("\nERROR: no message arrived for %u seconds -- the benchmark is stuck.\n"
                    "       topology=%s mode=%s arrived=%u of %u+%u\n"
                  , (WATCHDOG_TICK_MS * WATCHDOG_STALL_TICKS) / 1000u
                  , loclat::topology_as_str(mOptions.mTopology)
                  , loclat::mode_as_str(mMode)
                  , mArrived
                  , mOptions.mWarmup
                  , mOptions.mCount);
        std::fflush(stdout);

        _theStalled.store(true, std::memory_order_relaxed);
        mSessionDone.store(true, std::memory_order_relaxed);
        areg::Application::signal_quit();
        break;
    }
}

void LocalLatencyConsumer::response_ping_pong_0(uint32_t /* id */, uint64_t begin, uint64_t replied)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_8(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data8 & /* data8 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_16(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data16 & /* data16 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_32(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data32 & /* data32 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_64(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data64 & /* data64 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_128(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data128 & /* data128 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_256(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data256 & /* data256 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_512(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data512 & /* data512 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_1024(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data1024 & /* data1024 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_4096(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data4096 & /* data4096 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::response_ping_pong_65536(uint32_t /* id */, uint64_t begin, uint64_t replied, const LocalLatency::Data65536 & /* data65536 */)
{
    _on_arrival(begin, replied);
}

void LocalLatencyConsumer::broadcast_message_0(uint32_t /* id */, uint64_t begin)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_8(uint32_t /* id */, uint64_t begin, const LocalLatency::Data8 & /* data8 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_16(uint32_t /* id */, uint64_t begin, const LocalLatency::Data16 & /* data16 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_32(uint32_t /* id */, uint64_t begin, const LocalLatency::Data32 & /* data32 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_64(uint32_t /* id */, uint64_t begin, const LocalLatency::Data64 & /* data64 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_128(uint32_t /* id */, uint64_t begin, const LocalLatency::Data128 & /* data128 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_256(uint32_t /* id */, uint64_t begin, const LocalLatency::Data256 & /* data256 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_512(uint32_t /* id */, uint64_t begin, const LocalLatency::Data512 & /* data512 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_1024(uint32_t /* id */, uint64_t begin, const LocalLatency::Data1024 & /* data1024 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_4096(uint32_t /* id */, uint64_t begin, const LocalLatency::Data4096 & /* data4096 */)
{
    _on_arrival(begin, 0u);
}

void LocalLatencyConsumer::broadcast_message_65536(uint32_t /* id */, uint64_t begin, const LocalLatency::Data65536 & /* data65536 */)
{
    _on_arrival(begin, 0u);
}
