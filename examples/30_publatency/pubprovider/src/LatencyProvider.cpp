/************************************************************************
 * \file        pubprovider/src/LatencyProvider.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service provider component implementation.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubprovider/src/LatencyProvider.hpp"
#include "common/latency_common.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/Containers.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/ComponentThread.hpp"

#include <vector>

//////////////////////////////////////////////////////////////////////////
// LatencyProvider::ProviderCmdConsumer implementation
//////////////////////////////////////////////////////////////////////////

void LatencyProvider::ProviderCmdConsumer::process_event(const ProviderCmdData & data)
{
    mOwner._on_provider_cmd(data);
}

//////////////////////////////////////////////////////////////////////////
// LatencyProvider class implementation
//////////////////////////////////////////////////////////////////////////

LatencyProvider::LatencyProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LatencyProviderBase   ( static_cast<areg::Component &>(*this) )
    , areg::TimerConsumer   ( )
    , areg::ThreadConsumer  ( )

    , mInputThread          ( static_cast<areg::ThreadConsumer &>(*this), THREAD_INPUT )
    , mBroadcastTimer       ( static_cast<areg::TimerConsumer &>(*this),  TIMER_BROADCAST  )
    , mQuit                 ( false )
    , mQuitInput            ( false )
    , mDurationSec          ( 0u )
    , mBroadcastCount       ( 0u )
    , mBroadcastMode        ( Latency::LatencyMode::Stop )
    , mProviderCmdConsumer  ( *this )
    , mDisplayConsumer      ( *this )
    , mDisplayThread        ( static_cast<areg::ThreadConsumer &>(mDisplayConsumer), THREAD_DISPLAY )
    , mClientCount          ( 0u )
    , mLastRateCount        ( 0u )
    , mBroadcastCyclesLeft  ( 0u )
    , mMsgId                ( 0u )
    , mTotalServed          ( 0u )
    , mRunMin               ( INT64_MAX )
    , mRunMax               ( 0 )
    , mRunSum               ( 0 )
    , mRunCount             ( 0u )
    , mLastRate             ( 0.0 )
{
}

void LatencyProvider::startup_service_interface(areg::Component & holder)
{
    mQuit.store(false, std::memory_order_relaxed);
    mQuitInput.store(false, std::memory_order_relaxed);
    mDurationSec.store(0u, std::memory_order_relaxed);
    mBroadcastCount.store(0u, std::memory_order_relaxed);
    mBroadcastMode.store(Latency::LatencyMode::Stop, std::memory_order_relaxed);
    mTotalServed.store(0u, std::memory_order_relaxed);
    mClientCount          = 0u;
    mLastRateCount        = 0u;
    mBroadcastCyclesLeft  = 0u;
    mMsgId                = 0u;

    set_latency_setup(Latency::LantencySetup{Latency::LatencyMode::Stop, 0u, 0u, 0u});
    mLastStats       = {};
    mOneWayLatencies.clear();
    mLastRate        = 0.0;
    mRunMin          = INT64_MAX;
    mRunMax          = 0;
    mRunSum          = 0;
    mRunCount        = 0u;

    areg::ext::Console::instance().enable_console_input(true);
    _redraw_layout();
    mDisplayThread.start(areg::WAIT_INFINITE);
    mInputThread.start(areg::WAIT_INFINITE);

    LatencyProviderBase::startup_service_interface(holder);
}

void LatencyProvider::shutdown_service_interface(areg::Component & holder) noexcept
{
    mQuit.store(true, std::memory_order_relaxed);
    mQuitInput.store(true, std::memory_order_relaxed);
    mBroadcastMode.store(Latency::LatencyMode::Stop, std::memory_order_relaxed);

    mBroadcastTimer.stop_timer();
    areg::ext::Console::instance().enable_console_input(false);

    mInputThread.shutdown(areg::WAIT_INFINITE);
    mDisplayThread.shutdown(areg::WAIT_INFINITE);

    LatencyProviderBase::shutdown_service_interface(holder);
}

bool LatencyProvider::consumer_connected(const areg::ProxyAddress & client, areg::ServiceConnectionState status)
{
    const bool result = LatencyProviderBase::consumer_connected(client, status);

    if (areg::is_service_connected(status))
    {
        ++mClientCount;
    }
    else if (mClientCount > 0u)
    {
        --mClientCount;
    }

    return result;
}

void LatencyProvider::request_start_mode(Latency::LatencyMode mode, uint32_t duration, uint32_t warmup, uint32_t count)
{
    mBroadcastTimer.stop_timer();
    mBroadcastCyclesLeft = 0u;

    if (mode == Latency::LatencyMode::QuitApp)
    {
        mBroadcastMode.store(Latency::LatencyMode::Stop, std::memory_order_relaxed);
        set_latency_setup(Latency::LantencySetup{mode, 0u, 0u, 0u});
        response_start_mode(Latency::LantencySetup{mode, 0u, 0u, 0u});
        areg::Application::signal_quit();
        return;
    }

    const bool is_request  = (mode >= Latency::LatencyMode::Request0) && (mode <= Latency::LatencyMode::Request65536);;
    const bool is_broadcast= (mode >= Latency::LatencyMode::Broadcast0) && (mode <= Latency::LatencyMode::Broadcast65536);

    if (is_request)
    {
        mOneWayLatencies.reserve(count + warmup);
        mLastStats  = {};
        mRunMin     = INT64_MAX;
        mRunMax     = 0;
        mRunSum     = 0;
        mRunCount   = 0u;
    }

    mDurationSec.store(duration, std::memory_order_relaxed);
    mBroadcastCount.store(count + warmup, std::memory_order_relaxed);
    mBroadcastMode.store(mode, std::memory_order_relaxed);

    set_latency_setup(Latency::LantencySetup{ mode, duration, warmup, count });
    response_start_mode(static_cast<const Latency::LantencySetup &>(latency_setup()));

    if (is_broadcast && (count != 0u))
    {
        mMsgId = 0u;
        _run_broadcast_burst();
        mBroadcastCyclesLeft = duration != 0u ? duration - 1 : 0xFFFFFFFFu;  // 0 means run until stopped
        _update_live();
        if (mBroadcastCyclesLeft > 0u)
        {
            mBroadcastTimer.start_timer(1000u, component_thread(), areg::Timer::CONTINUOUSLY);
        }
    }
}

void LatencyProvider::request_ping_pong_0(uint32_t id, uint64_t start)
{
    _record_oneway(start);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_0(id, start, Latency::now_ns());
}

void LatencyProvider::request_ping_pong_8(uint32_t id, uint64_t begin, const Latency::Data8 & data8)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_8(id, begin, Latency::now_ns(), data8);
}

void LatencyProvider::request_ping_pong_16(uint32_t id, uint64_t begin, const Latency::Data16 & data16)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_16(id, begin, Latency::now_ns(), data16);
}

void LatencyProvider::request_ping_pong_32(uint32_t id, uint64_t begin, const Latency::Data32 & data32)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_32(id, begin, Latency::now_ns(), data32);
}

void LatencyProvider::request_ping_pong_64(uint32_t id, uint64_t begin, const Latency::Data64 & data64)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_64(id, begin, Latency::now_ns(), data64);
}

void LatencyProvider::request_ping_pong_128(uint32_t id, uint64_t begin, const Latency::Data128 & data128)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_128(id, begin, Latency::now_ns(), data128);
}

void LatencyProvider::request_ping_pong_256(uint32_t id, uint64_t begin, const Latency::Data256 & data256)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_256(id, begin, Latency::now_ns(), data256);
}

void LatencyProvider::request_ping_pong_512(uint32_t id, uint64_t begin, const Latency::Data512& data512)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_512(id, begin, Latency::now_ns(), data512);
}

void LatencyProvider::request_ping_pong_1024(uint32_t id, uint64_t begin, const Latency::Data1024 & data1024)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_1024(id, begin, Latency::now_ns(), data1024);
}

void LatencyProvider::request_ping_pong_4096(uint32_t id, uint64_t begin, const Latency::Data4096 & data4096)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_4096(id, begin, Latency::now_ns(), data4096);
}

void LatencyProvider::request_ping_pong_65536(uint32_t id, uint64_t begin, const Latency::Data65536 & data65536)
{
    _record_oneway(begin);
    mTotalServed.fetch_add(1u, std::memory_order_relaxed);
    response_ping_pong_65536(id, begin, Latency::now_ns(), data65536);
}

void LatencyProvider::process_timer(areg::Timer & timer)
{
    if (&timer != &mBroadcastTimer)
        return;

    _run_broadcast_burst();
    if (--mBroadcastCyclesLeft == 0u)
    {
        mBroadcastTimer.stop_timer();
        mBroadcastMode.store(Latency::LatencyMode::Stop, std::memory_order_relaxed);
    }
}

void LatencyProvider::on_run()
{
    _run_input_thread();
}

void LatencyProvider::DisplayConsumer::on_run()
{
    mOwner._run_display_thread();
}

void LatencyProvider::_run_display_thread()
{
    while (!mQuit.load(std::memory_order_relaxed))
    {
        areg::Thread::sleep(1000u);
        if (mQuit.load(std::memory_order_relaxed))
            break;

        _update_live();
        _update_latency_stats();
    }
}

void LatencyProvider::_run_broadcast_burst()
{
    const uint32_t             count = mBroadcastCount.load(std::memory_order_relaxed);
    const Latency::LatencyMode mode  = mBroadcastMode.load(std::memory_order_relaxed);

    if (count == 0u)
        return;

    areg::Wait waiter;
    const uint64_t interval_ns = 1'000'000'000ULL / static_cast<uint64_t>(count);

    switch (mode)
    {
    case Latency::LatencyMode::Broadcast0:
        {
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_0(mMsgId++, Latency::now_ns());
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast8:
        {
            Latency::Data8 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_8(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast16:
        {
            Latency::Data16 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_16(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast32:
        {
            Latency::Data32 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_32(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast64:
        {
            Latency::Data64 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_64(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast128:
        {
            Latency::Data128 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_128(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast256:
        {
            Latency::Data256 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_256(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast512:
        {
            Latency::Data512 d {};
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_512(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast1024:
        {
            Latency::Data1024 d {};
            d.data_rest.set_size_used(1024 - 128 - 64);
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_1024(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast4096:
        {
            Latency::Data4096 d {};
            d.data_rest.set_size_used(4096 - 128 - 64);
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_4096(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    case Latency::LatencyMode::Broadcast65536:
        {
            Latency::Data65536 d {};
            d.data_rest.set_size_used(65536 - 128 - 64);
            uint32_t i { 0u };
            uint64_t target = Latency::now_ns();
            do
            {
                broadcast_message_65536(mMsgId++, Latency::now_ns(), d);
                mTotalServed.fetch_add(1u, std::memory_order_relaxed);
                target += interval_ns;
                const int64_t delta = static_cast<int64_t>(target - Latency::now_ns());
                if (delta > COARSE_SLEEP_NS)
                {
                    waiter.wait_for(areg::Wait::Duration{ delta });
                    target = Latency::now_ns();
                }
            } while (++i < count);
        }
        break;

    default:
        break;
    }
}

void LatencyProvider::_run_input_thread()
{
    areg::ext::Console & console = areg::ext::Console::instance();

    while (!mQuitInput.load(std::memory_order_relaxed))
    {
        console.output_msg(COORD_INPUT, " > %-100s", "");
        console.set_cursor_cur_position({ 4, COORD_INPUT.posY });
        console.refresh_screen();

        const areg::String line = console.wait_for_input(nullptr);

        if (mQuitInput.load(std::memory_order_relaxed))
            break;

        areg::String cmd(line);
        cmd.make_lower();
        cmd.trim_all();

        if (cmd.is_empty())
            continue;

        areg::Tokenizer tokens(cmd, " ", false);
        const areg::StringArray & list = tokens.list();

        ProviderCmdData data;

        for (uint32_t i = 0u; i < list.size(); ++i)
        {
            areg::String tok(list[i]);
            tok.trim_all();
            if (tok.is_empty())
                continue;

            if (tok == "-q" || tok == "quit")
                data.flags |= ProviderCmdData::F_QUIT;
            else if (tok == "-h" || tok == "help" || tok == "--help")
                data.flags |= ProviderCmdData::F_HELP;
            else if (tok == "-i" || tok == "info")
                data.flags |= ProviderCmdData::F_INFO;
            else
                data.flags |= ProviderCmdData::F_ERROR;
        }

        // Set the quit flag before sending the event so the while-loop condition
        // is false by the time we return here — avoids re-entering wait_for_input.
        if (data.has_quit())
        {
            mQuitInput.store(true, std::memory_order_relaxed);
        }

        if (data.flags != 0u)
        {
            ProviderCmdEvent::send_event(data, static_cast<ProviderCmdConsumerBase &>(mProviderCmdConsumer), master_thread());
        }
    }
}

void LatencyProvider::_on_provider_cmd(const ProviderCmdData & data)
{
    if (data.has_error())
    {
        areg::ext::Console & console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_msg(COORD_LIVE, " ERROR: unknown command. Use -q, -h, -i.%-60s", "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    if (data.has_quit())
    {
        mQuitInput.store(true, std::memory_order_relaxed);
        set_latency_setup(Latency::LantencySetup{Latency::LatencyMode::QuitApp, 0u, 0u, 0u});
        mBroadcastMode.store(Latency::LatencyMode::Stop, std::memory_order_relaxed);
        mBroadcastTimer.stop_timer();
        areg::Application::signal_quit();
    }
    else if (data.has_help())
    {
        _redraw_layout();
    }
    else if (data.has_info())
    {
        _update_live();
    }
}

void LatencyProvider::_redraw_layout()
{
    areg::ext::Console & console = areg::ext::Console::instance();
    console.clear_screen();
    console.output_txt(COORD_TITLE,   MSG_TITLE);
    console.output_txt(COORD_SEP1,    MSG_SEPARATOR);
    console.output_msg(COORD_LIVE,    "%-120s", "");
    console.output_msg(COORD_LATENCY, "%-120s", "");
    console.output_txt(COORD_SEP2,    MSG_SEPARATOR);
    console.output_txt(COORD_PROMPT,  MSG_PROMPT);
    console.output_txt(COORD_SEP3,    MSG_SEPARATOR);
    console.refresh_screen();
    _update_live();
    _update_latency_stats();
}

void LatencyProvider::_update_live()
{
    const uint32_t served = mTotalServed.load(std::memory_order_relaxed);
    const uint32_t delta  = (served >= mLastRateCount) ? (served - mLastRateCount) : 0u;
    mLastRateCount = served;

    if (delta > 0u)
        mLastRate = static_cast<double>(delta);

    const Latency::LatencyMode mode = mBroadcastMode.load(std::memory_order_relaxed);
    const bool is_active = (mode != Latency::LatencyMode::Stop) && (mode != Latency::LatencyMode::QuitApp);
    const char * status  = is_active ? "running" : (mTotalServed.load(std::memory_order_relaxed) > 0u ? "done" : "stopped");

    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();
    console.output_msg(COORD_LIVE
                     , MSG_LIVE.data()
                     , Latency::mode_as_str(mode)
                     , status
                     , mTotalServed.load(std::memory_order_relaxed)
                     , mLastRate);
    console.restore_cursor_position();
    console.refresh_screen();
}

void LatencyProvider::_record_oneway(uint64_t begin_ns) noexcept
{
    const int64_t oneway = static_cast<int64_t>(Latency::now_ns()) - static_cast<int64_t>(begin_ns);
    if (oneway <= 0)
        return;

    if (oneway < mRunMin) mRunMin = oneway;
    if (oneway > mRunMax) mRunMax = oneway;
    mRunSum += oneway;
    ++mRunCount;

    mOneWayLatencies.push_back(oneway);
}

void LatencyProvider::_update_latency_stats()
{
    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();

    const Latency::LatencyMode mode = mBroadcastMode.load(std::memory_order_relaxed);
    const uint16_t val         = static_cast<uint16_t>(mode);
    const bool     is_active   = (mode != Latency::LatencyMode::Stop) && (mode != Latency::LatencyMode::QuitApp);
    const bool     is_request  = is_active && (val >= static_cast<uint16_t>(Latency::LatencyMode::Request0));
    const bool     is_broadcast = is_active && !is_request;

    if (is_broadcast)
    {
        // Broadcast mode: show dashes — do NOT touch mLastStats (preserve prior request test results)
        console.output_txt(COORD_LATENCY, MSG_LATENCY_NONE);
    }
    else if (is_request && !mOneWayLatencies.empty())
    {
        // Active request test: sort all samples and compute full percentiles
        std::vector<int64_t> sorted(mOneWayLatencies.begin(), mOneWayLatencies.end());
        std::sort(sorted.begin(), sorted.end());
        const size_t N = sorted.size();

        int64_t sum = 0;
        for (int64_t v : sorted) sum += v;

        auto percentile = [&](double pct) -> int64_t
        {
            const size_t idx = static_cast<size_t>(pct * static_cast<double>(N - 1u) / 100.0 + 0.5);
            return sorted[idx < N ? idx : N - 1u];
        };

        mLastStats.min_us  = static_cast<double>(sorted.front()) / 1000.0;
        mLastStats.p50_us  = static_cast<double>(percentile(50.0)) / 1000.0;
        mLastStats.p95_us  = static_cast<double>(percentile(95.0)) / 1000.0;
        mLastStats.p99_us  = static_cast<double>(percentile(99.0)) / 1000.0;
        mLastStats.max_us  = static_cast<double>(sorted.back()) / 1000.0;
        mLastStats.mean_us = static_cast<double>(sum) / static_cast<double>(N) / 1000.0;
        mLastStats.valid   = true;

        console.output_msg(COORD_LATENCY, MSG_LATENCY.data()
                         , mLastStats.min_us, mLastStats.p50_us, mLastStats.p95_us
                         , mLastStats.p99_us, mLastStats.max_us, mLastStats.mean_us);
    }
    else if (mLastStats.valid)
    {
        // Stopped/idle: show persistent results from the last completed request test
        console.output_msg(COORD_LATENCY, MSG_LATENCY.data()
                         , mLastStats.min_us, mLastStats.p50_us, mLastStats.p95_us
                         , mLastStats.p99_us, mLastStats.max_us, mLastStats.mean_us);
    }
    else
    {
        // No data yet
        console.output_txt(COORD_LATENCY, MSG_LATENCY_NONE);
    }

    console.restore_cursor_position();
    console.refresh_screen();
}
