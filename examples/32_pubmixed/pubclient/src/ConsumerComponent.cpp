/************************************************************************
 * \file        pubclient/src/ConsumerComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: consumer of the mixed traffic service.
 ************************************************************************/

#include "pubclient/src/ConsumerComponent.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>

namespace
{
    inline uint64_t now_us()
    {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::microseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    uint32_t percentile(const std::vector<uint32_t>& sorted, double p)
    {
        if (sorted.empty())
            return 0u;

        size_t idx{ static_cast<size_t>(sorted.size() * p) };
        if (idx >= sorted.size())
            idx = sorted.size() - 1u;

        return sorted[idx];
    }
}

//////////////////////////////////////////////////////////////////////////
// ConsumerComponent
//////////////////////////////////////////////////////////////////////////

mixed::ConsumerOptions & ConsumerComponent::options() noexcept
{
    static mixed::ConsumerOptions _options;
    return _options;
}

ConsumerComponent::ConsumerComponent(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
    : areg::Component           ( entry, owner )
    , MixedTrafficConsumerBase  ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component&>(*this) )
    , areg::TimerConsumer       ( )

    , mPingTimer                ( static_cast<areg::TimerConsumer&>(self()), TIMER_PING )
    , mReportTimer              ( static_cast<areg::TimerConsumer&>(self()), TIMER_REPORT )
    , mRtt                      ( )
    , mRttAll                   ( )
    , mBulkBytes                ( 0u )
    , mBulkBlocks               ( 0u )
    , mPingsSent                ( 0u )
    , mSeconds                  ( 0u )
{
    mRtt.reserve(4096u);
    mRttAll.reserve(mixed::RTT_SAMPLE_CAPACITY);
}

bool ConsumerComponent::service_connected(areg::ServiceConnectionState status, areg::ProxyBase& proxy)
{
    const bool result{ MixedTrafficConsumerBase::service_connected(status, proxy) };

    if (areg::is_service_connected(status))
    {
        areg::Application::enable_data_rate(true);
        notify_on_broadcast_bulk_block(true);

        const uint32_t pingMs{ ConsumerComponent::options().pingUs / 1000u };
        mPingTimer.start_timer(pingMs != 0u ? pingMs : 1u, areg::Timer::CONTINUOUSLY);
        mReportTimer.start_timer(mixed::REPORT_TIMEOUT_MS, areg::Timer::CONTINUOUSLY);

        ::printf("Example 32 consumer: ping every %u us\n", ConsumerComponent::options().pingUs);
        ::fflush(stdout);
    }
    else
    {
        mPingTimer.stop_timer();
        mReportTimer.stop_timer();
    }

    return result;
}

void ConsumerComponent::broadcast_bulk_block(const areg::SharedBuffer& block, uint32_t /*producer*/)
{
    ++mBulkBlocks;
    mBulkBytes += block.size_used();
}

void ConsumerComponent::response_pong(uint64_t stamp)
{
    const uint64_t sent{ stamp };
    const uint64_t back{ now_us() };
    const uint32_t rtt { back > sent ? static_cast<uint32_t>(back - sent) : 0u };

    mRtt.push_back(rtt);
    if (mRttAll.size() < mixed::RTT_SAMPLE_CAPACITY)
    {
        mRttAll.push_back(rtt);
    }
}

void ConsumerComponent::broadcast_service_stopping()
{
    mPingTimer.stop_timer();
    mReportTimer.stop_timer();
    areg::Application::signal_quit();
}

void ConsumerComponent::process_timer(areg::Timer& timer)
{
    if (&timer == &mPingTimer)
    {
        send_ping();
    }
    else if (&timer == &mReportTimer)
    {
        report();
    }
}

void ConsumerComponent::send_ping()
{
    ++mPingsSent;
    request_ping(now_us());
}

void ConsumerComponent::report()
{
    std::vector<uint32_t> sorted{ mRtt };
    std::sort(sorted.begin(), sorted.end());

    uint64_t netBytes{ 0u };
    uint32_t netMsgs { 0u };
    areg::Application::query_data_received(netBytes, netMsgs);

    const areg::DataLiteral bulk{ areg::conv_data_size(mBulkBytes) };

    ::printf("consumer | bulk %6u blk/s %8.2f %s/s | ping %5zu | rtt us: p50 %7u p95 %7u p99 %7u max %7u\n"
             , mBulkBlocks, bulk.first, bulk.second.data(), sorted.size()
             , percentile(sorted, 0.50), percentile(sorted, 0.95)
             , percentile(sorted, 0.99), sorted.empty() ? 0u : sorted.back());
    ::fflush(stdout);

    mBulkBlocks = 0u;
    mBulkBytes  = 0u;
    mRtt.clear();

    ++mSeconds;
    const uint32_t limit{ ConsumerComponent::options().runSeconds };
    if ((limit != 0u) && (mSeconds >= limit))
    {
        std::vector<uint32_t> all{ mRttAll };
        std::sort(all.begin(), all.end());
        ::printf("\nTOTAL over %u s | samples %zu | rtt us: min %u p50 %u p95 %u p99 %u max %u\n"
                 , mSeconds, all.size()
                 , all.empty() ? 0u : all.front()
                 , percentile(all, 0.50), percentile(all, 0.95), percentile(all, 0.99)
                 , all.empty() ? 0u : all.back());
        ::fflush(stdout);
        areg::Application::signal_quit();
    }
}
