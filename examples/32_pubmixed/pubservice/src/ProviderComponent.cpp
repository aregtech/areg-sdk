/************************************************************************
 * \file        pubservice/src/ProviderComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: provider of the mixed traffic service.
 ************************************************************************/

#include "pubservice/src/ProviderComponent.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/component/ComponentThread.hpp"

#include <chrono>
#include <cstdio>
#include <vector>

namespace
{
    //!< Options of this process, filled by main() before the model is loaded.
    mixed::ProviderOptions  g_options;
}

mixed::ProviderOptions& provider_options()
{
    return g_options;
}

//////////////////////////////////////////////////////////////////////////
// ProviderComponent::ReportTimerConsumer
//////////////////////////////////////////////////////////////////////////

void ProviderComponent::ReportTimerConsumer::process_timer(areg::Timer& timer)
{
    if (&timer == &mOwner.mTimer)
    {
        mOwner.on_report_timer();
    }
}

//////////////////////////////////////////////////////////////////////////
// ProviderComponent
//////////////////////////////////////////////////////////////////////////

ProviderComponent::ProviderComponent(const areg::ComponentEntry& entry, areg::ComponentThread& owner)
    : areg::Component           ( entry, owner )
    , MixedTrafficProviderBase  ( static_cast<areg::Component&>(*this) )
    , areg::ThreadConsumer      ( )

    , mBlock                    ( )
    , mProducers                ( )
    , mBulkBlocks               ( 0u )
    , mPings                    ( 0u )
    , mQuit                     ( false )
    , mTimer                    ( static_cast<areg::TimerConsumer&>(mTimerConsumer), TIMER_NAME )
    , mTimerConsumer            ( self() )
{
}

void ProviderComponent::startup_service_interface(areg::Component& holder)
{
    const mixed::ProviderOptions& opt{ provider_options() };

    areg::Application::enable_data_rate(true);

    {
        std::vector<uint8_t> payload(opt.blockBytes, static_cast<uint8_t>(0x5A));
        mBlock = areg::SharedBuffer(opt.blockBytes, payload.data(), opt.blockBytes);
    }

    MixedTrafficProviderBase::startup_service_interface(holder);

    mTimer.start_timer(mixed::REPORT_TIMEOUT_MS, component_thread(), areg::Timer::CONTINUOUSLY);

    if (opt.bulkOff == false)
    {
        mProducers.reserve(opt.producers);
        for (uint32_t i = 0u; i < opt.producers; ++i)
        {
            areg::String name{ THREAD_PREFIX.data() };
            name += areg::String::make_string(i);
            mProducers.push_back(std::make_unique<areg::Thread>(static_cast<areg::ThreadConsumer&>(self()), name));
            mProducers.back()->start(areg::WAIT_INFINITE);
        }
    }

    ::printf("Example 32 provider: block %u bytes, %u producer thread(s), gap %u us%s\n"
             , opt.blockBytes, (opt.bulkOff ? 0u : opt.producers), opt.gapUs
             , opt.bulkOff ? "  [bulk disabled]" : "");
}

void ProviderComponent::shutdown_service_interface(areg::Component& holder) noexcept
{
    mQuit.store(true, std::memory_order_relaxed);
    mTimer.stop_timer();

    for (auto& thread : mProducers)
    {
        thread->shutdown(areg::WAIT_INFINITE);
    }

    mProducers.clear();

    MixedTrafficProviderBase::shutdown_service_interface(holder);
}

void ProviderComponent::request_ping(uint64_t stamp)
{
    mPings.fetch_add(1u, std::memory_order_relaxed);
    response_pong(stamp);
}

void ProviderComponent::on_run()
{
    const uint32_t gapUs{ provider_options().gapUs };
    areg::Wait wait;

    while (mQuit.load(std::memory_order_relaxed) == false)
    {
        broadcast_bulk_block(mBlock, 0u);
        mBulkBlocks.fetch_add(1u, std::memory_order_relaxed);

        if (gapUs != 0u)
        {
            wait.wait_for(std::chrono::microseconds{ gapUs });
        }
    }
}

void ProviderComponent::on_report_timer()
{
    const uint32_t blocks{ mBulkBlocks.exchange(0u, std::memory_order_relaxed) };
    const uint32_t pings { mPings.exchange(0u, std::memory_order_relaxed) };

    uint64_t netBytes{ 0u };
    uint32_t netMsgs { 0u };
    areg::Application::query_data_sent(netBytes, netMsgs);

    const areg::DataLiteral rate{ areg::conv_data_size(netBytes) };
    ::printf("provider | bulk %6u blk/s | pings %6u /s | wire %8.2f %s/s, %6u msg/s\n"
             , blocks, pings, rate.first, rate.second.data(), netMsgs);
    ::fflush(stdout);
}
