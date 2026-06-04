/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"
// EventDataStream removed — no longer part of the wire protocol
#include "areg/component/RemoteEventFactory.hpp"
#include "areg/component/ResponseEvents.hpp"
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>

DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, startup_service_interface);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, shutdown_service_intrface);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, on_option_event);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, on_run);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, run_input_thread);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent, run_image_thread);

//////////////////////////////////////////////////////////////////////////
// ServicingComponent::OptionConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void ServicingComponent::OptionConsumer::process_event(const OptionData& data)
{
    mService.on_option_event( data );
}

//////////////////////////////////////////////////////////////////////////
// ServicingComponent::ServicingTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void ServicingComponent::ServicingTimerConsumer::process_timer(areg::Timer& timer)
{
    if (&timer == &mService.mTimer)
    {
        mService.on_timer_expired();
    }
}

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class implementation
//////////////////////////////////////////////////////////////////////////

ServicingComponent::ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LargeDataProviderBase ( static_cast<areg::Component &>(self()) )
    , areg::ThreadConsumer  ( )

    , mBitmap           ( )
    , mSendList         ( )
    , mActiveProxies    ( )
    , mPrebuiltMessages ( )
    , mDataRate         (  )
    , mTimer            ( static_cast<areg::TimerConsumer &>(mTimerConsumer) , TIMER_NAME )
    , mInputThread      ( static_cast<areg::ThreadConsumer &>(self()), THREAD_WAITINPUT )
    , mImageThread      ( static_cast<areg::ThreadConsumer &>(self()), THREAD_GENERATE )
    , mOptions          ( )
    , mQuitThread       ( true )
    , mOptionChanged    ( true )
    , mPauseEvent       ( false, false )
    , mClients          ( 0 )
    , mOptionConsumer   ( self() )
    , mTimerConsumer    ( self() )
    , mLock             ( )
{
    mOptions.mWidth     = util::IMAGE_WIDTH;
    mOptions.mHeight    = util::IMAGE_HEIGHT;
    mOptions.mLines     = util::LINES_PER_BLOCK;
    mOptions.mPixelTime = util::DWELL_TIME;
    mOptions.mChannels  = util::CHANNELS_SOURCE;
    mOptions.mFlags     = static_cast<uint32_t>(util::OptionFlag::CmdStop);
}


inline void ServicingComponent::print_rates(areg::ext::Console& console)
{
    mLock.lock(areg::WAIT_INFINITE);

    uint64_t netDataSent{ 0u };
    uint32_t netMsgSent{ 0u };
    DataRate sentData{ mDataRate };

    const uint64_t ns_per_block_0{ mOptions.nsPerBlock() };
    const uint64_t bytes_per_block_0{ mOptions.bytesPerBlock() };
    const uint32_t channels_0{ mOptions.mChannels };

    mDataRate.sentData = 0u;
    mDataRate.sentBlocks = 0u;
    mDataRate.ontimeBlocks = 0u;
    mDataRate.lateBlocks = 0u;

    areg::Application::query_data_sent(netDataSent, netMsgSent);

    mLock.unlock();

    const areg::DataLiteral netRate = areg::conv_data_size(netDataSent);
    const areg::DataLiteral svcRate = areg::conv_data_size(sentData.sentData);

    const double ideal_bps_0 = (ns_per_block_0 > 0u) ? (static_cast<double>(areg::DURATION_1_SEC) / static_cast<double>(ns_per_block_0)) : 0.0;

    const uint64_t ideal_bytes_sec_0   = static_cast<uint64_t>(ideal_bps_0 * static_cast<double>(bytes_per_block_0) * static_cast<double>(channels_0));
    const uint32_t total_blocks_sec_0  = static_cast<uint32_t>(ideal_bps_0 * static_cast<double>(channels_0));
    const areg::DataLiteral idealRate0 = areg::conv_data_size(ideal_bytes_sec_0);

    console.output_txt(COORD_TITLE, MSG_APP_TITLE);
    console.output_txt(COORD_SEP1, MSG_SEPARATOR);
    console.output_msg(COORD_COMM_RATE, MSG_NET_RATE_SENT.data()    , netRate.first, netRate.second.data(), netMsgSent);
    console.output_msg(COORD_DATA_RATE, MSG_QUEUE_RATE_SENT.data()  , svcRate.first, svcRate.second.data(), sentData.sentBlocks);

    if (ns_per_block_0 != 0u)
    {
        console.output_msg(COORD_IDEAL_RATE, MSG_IDEAL_RATE_SENT.data(), idealRate0.first, idealRate0.second.data(), total_blocks_sec_0);
    }
    else
    {
        console.output_txt(COORD_IDEAL_RATE, " Theoretical rate .....: IPC-limited (pixel time = 0).");
    }

    console.output_msg(COORD_STATS, MSG_STATS_RATE.data(), sentData.ontimeBlocks, sentData.lateBlocks);
    console.output_txt(COORD_SEP2, MSG_SEP2);
}

void ServicingComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, startup_service_interface );

    mQuitThread     = false;
    mOptionChanged  = false;
    mPauseEvent.reset();
    areg::Application::enable_data_rate(true);

    areg::ext::Console& console = areg::ext::Console::instance();
    print_rates(console);
    _print_info();

    _init_block_list();
    mInputThread.start(areg::WAIT_INFINITE);
    mImageThread.start(areg::WAIT_INFINITE);

    console.enable_console_input(true);

    // If command-line args were parsed (e.g. `-s -t=0 -l=1 -c=10`), fire the initial
    // event now so the benchmark starts without waiting for user input.
    if ((util::g_startup_options.mFlags != static_cast<uint32_t>(util::OptionFlag::CmdNothing)) &&
        (util::g_startup_options.hasError() == false))
    {
        EventOption::send_event( util::g_startup_options
                               , static_cast<IEOptionConsumer&>(mOptionConsumer)
                               , component_thread() );
    }

    LargeDataProviderBase::startup_service_interface(holder);
}

void ServicingComponent::shutdown_service_interface(areg::Component& holder) noexcept
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, shutdown_service_intrface );

    mQuitThread.store(true, std::memory_order_relaxed);
    mOptionChanged.store(true, std::memory_order_relaxed);
    mTimer.stop_timer();
    mPauseEvent.set_signaled();

    mPrebuiltMessages.clear();
    mBitmap.release();
    mInputThread.shutdown(areg::WAIT_INFINITE);
    mImageThread.shutdown(areg::WAIT_INFINITE);

    LargeDataProviderBase::shutdown_service_interface(holder);
}

bool ServicingComponent::consumer_connected(const areg::ProxyAddress& client, areg::ServiceConnectionState connectionStatus )
{
    bool result = LargeDataProviderBase::consumer_connected(client, connectionStatus );

    do
    {
        areg::Lock lock(mLock);
        if (areg::is_service_connected(connectionStatus))
        {
            ++mClients;
            mActiveProxies.push_back(client);
            set_image_gen_setting({ mOptions.mWidth, mOptions.mHeight, mOptions.mLines, mOptions.mPixelTime, mOptions.mChannels });
        }
        else
        {
            --mClients;
            auto it = std::find(mActiveProxies.begin(), mActiveProxies.end(), client);
            if (it != mActiveProxies.end())
                mActiveProxies.erase(it);
        }

        // Immediately invalidate the pool and rebuild the pool
        mOptionChanged.store(true, std::memory_order_relaxed);
    } while (false);

    _print_info();

    return result;
}

void ServicingComponent::on_timer_expired()
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();
    print_rates(console);
    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::on_option_event(const OptionData& data)
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, on_option_event );

    if (data.hasError())
    {
        LOG_WARN("Error input of command");
        areg::ext::Console& console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_txt(COORD_ERROR_INFO, MSG_INVALID_CMD);
        console.restore_cursor_position();
        console.refresh_screen();
    }
    else if (data.hasQuit())
    {
        LOG_WARN("Requested to quit application");

        mQuitThread.store(true, std::memory_order_relaxed);
        mOptionChanged.store(true, std::memory_order_relaxed);
        mOptions.update(data);
        mTimer.stop_timer();
        // Unblock the image thread so it can exit its wait.
        mPauseEvent.set_signaled();

        broadcast_service_stopping();
        areg::Application::signal_quit();
    }
    else if (data.hasStart())
    {
        LOG_INFO("Requested to start generating data");

        mQuitThread.store(false, std::memory_order_relaxed);
        mOptions.update(data);
        mOptionChanged.store(true, std::memory_order_relaxed);
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, component_thread(), areg::Timer::CONTINUOUSLY);
        // Signal the image thread to wake up.
        mPauseEvent.set_signaled();
        _print_info();
    }
    else if (data.hasStop())
    {
        LOG_INFO("Requested to stop generating data");

        mOptionChanged.store(true, std::memory_order_relaxed);
        mOptions.update(data);
        // Pause the image thread: it will exit the inner loop and block on mPauseEvent again.
        mPauseEvent.reset();
        _print_info();
    }
    else if (data.hasPrintHelp())
    {
        LOG_INFO("Requested to print help");
        _print_help();
    }
    else if (data.hasPrintInfo())
    {
        LOG_INFO("Requested to print info");
        _print_info();
    }
    else
    {
        LOG_INFO("Requested to change generating data parameter(s)");

        const bool was_running = mOptions.hasStart();

        mLock.lock(areg::WAIT_INFINITE);
        mOptions.update(data);
        mLock.unlock();

        _print_info();

        // Signal the image thread to reload params. It will call set_image_gen_setting()
        // after stopping the current frame stream and rebuilding the pool.
        mOptionChanged.store(true, std::memory_order_relaxed);

        if (was_running)
        {
            // Keep running: signal mPauseEvent so the thread re-enters the frame loop.
            mPauseEvent.set_signaled();
        }
        else
        {
            // Service was stopped; keep mPauseEvent non-signaled so the thread stays paused.
            mPauseEvent.reset();
        }
    }
}

void ServicingComponent::on_run()
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, on_run );

    const areg::String& threadName = areg::Thread::current_thread_name();
    if (threadName == THREAD_WAITINPUT )
    {
        LOG_DBG("Started console input thread.");
        _run_input_thread();
    }
    else if (threadName == THREAD_GENERATE )
    {
        LOG_DBG("Started generate image thread.");
        _run_image_thread();
    }
}

void ServicingComponent::_run_input_thread()
{
    areg::ext::Console& console = areg::ext::Console::instance();
    const areg::ext::Console::Coord prompt_pos
    {
          static_cast<int16_t>(COORD_OPTIONS.posX + static_cast<int16_t>(MSG_INPUT_OPTION.size()))
        , COORD_OPTIONS.posY
    };

    bool cmdQuit{ false };
    while ((cmdQuit == false) && (mQuitThread == false))
    {
        LOG_SCOPE( examples_23_pubservice_ServicingComponent, run_input_thread );
        LOG_DBG("Waiting to enter option command ...");

        console.output_txt(COORD_OPTIONS, MSG_INPUT_OPTION);
        console.set_cursor_cur_position(prompt_pos);
        console.refresh_screen();
        areg::String cmd = console.wait_for_input(nullptr);

        console.lock_console();
        console.clear_line(COORD_OPTIONS);
        console.clear_line(COORD_ERROR_INFO);
        console.output_txt(COORD_OPTIONS, MSG_INPUT_OPTION);
        console.set_cursor_cur_position(prompt_pos);
        console.refresh_screen();
        console.unlock_console();

        cmd.make_lower();
        OptionData newData;
        newData.parseCommand(cmd);
        cmdQuit = newData.hasQuit();
        EventOption::send_event(newData, static_cast<IEOptionConsumer&>(mOptionConsumer), component_thread());

        LOG_DBG("Received option command [ %s ]", cmd.as_string());
    }
}


void ServicingComponent::_init_block_list()
{
    const uint32_t blocks = mOptions.blocksCount();

    mBitmap.create_bitmap(mOptions.mWidth, mOptions.mHeight);
    mSendList.clear();
    mSendList.resize(blocks);

    for (uint32_t i = 0; i < blocks; ++i)
    {
        ImageBlock& entry = mSendList[i];
        mBitmap.block(entry, i * mOptions.mLines, mOptions.mLines);
    }
}

uint32_t ServicingComponent::_build_prebuilt_messages()
{
    constexpr uint32_t message_id{ static_cast<uint32_t>(LargeData::MessageIDs::MsgId_broadcast_image_block_acquired) };

    const uint32_t channels { mOptions.mChannels };
    const uint64_t frame_ns { mOptions.nsPerImage() };
    const uint32_t blocks   { mOptions.blocksCount() };
    const uint32_t depth{ frame_ns <= (util::TIME_IN_DEPTH / 2) ? static_cast<uint32_t>((static_cast<double>(util::TIME_IN_DEPTH) / static_cast<double>(frame_ns)) + 0.5f) : 2u };

    const areg::Channel& channel = areg::connection_channel();
    const std::vector<areg::ProxyAddress> proxies{ mActiveProxies };
    const size_t msgSize{ static_cast<size_t>(depth) * static_cast<size_t>(blocks) * static_cast<size_t>(channels) };

    mPrebuiltMessages.clear();

    for (const areg::ProxyAddress& proxy : proxies)
    {
        mPrebuiltMessages.push_back(ProxyPool());
        ProxyPool& pool = mPrebuiltMessages.back();

        pool.proxyId = static_cast<uint32_t>(proxy);
        pool.frameId = 0u;
        pool.depth = depth;
        pool.channels = channels;
        pool.loop = 0u;
        pool.messages.clear();
        pool.messages.resize(msgSize);

        uint32_t index{ 0u };
        for (uint32_t generation = 0u; generation < depth; ++generation)
        {
            const uint32_t shift{ generation * blocks * channels };
            for (uint32_t block_index = 0u; block_index < blocks; ++block_index)
            {
                ImageBlock& entry = mSendList[block_index];
                for (uint32_t ch = 0; ch < channels; ++ ch, ++index)
                {
                    areg::SharedBuffer buf, data;
                    Remote& remote = pool.messages[shift + block_index * channels + ch];
                    entry.set_ids(ch, 0u);
                    buf << entry;
                    data << buf;

                    // Pre-build the wire envelope exactly as a normal broadcast would: create the
                    // response event (sets consumer endpoint, message id, class id), then stamp the
                    // wire routing cookies (source = this process, target = proxy). The payload is a
                    // raw copy of `data`, i.e. [uint32 length][serialized ImageBlock]; the patchable
                    // RawImageBlock starts right after the length prefix.
                    areg::ServiceResponseEvent respEvent{ create_response(proxy, message_id, areg::ResultType::RequestOK, data) };
                    areg::RemoteEventFactory::route_outgoing_message(respEvent, channel);
                    remote.message = respEvent.envelope();
                    remote.offset  = static_cast<uint32_t>(sizeof(uint32_t));
                }

                ASSERT(index == shift + (block_index + 1) * channels);
            }

            ASSERT(index == (generation + 1) * blocks * channels);
        }

        ASSERT(index == msgSize);
    }

    return depth;
}

void ServicingComponent::_run_image_thread()
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent, run_image_thread);

    using Clock = std::chrono::steady_clock;
    using Nanoseconds = std::chrono::nanoseconds;

    static constexpr uint64_t STATS_FLUSH_NS{ 100'000'000u };   // 100 ms

    areg::Wait wait;

    while (!mQuitThread.load(std::memory_order_relaxed))
    {
        mPauseEvent.lock();
        if (mQuitThread.load(std::memory_order_relaxed))
        {
            break;
        }

        mLock.lock(areg::WAIT_INFINITE);

        mOptionChanged.store(false, std::memory_order_relaxed);
        _init_block_list();

        uint32_t frame_id{ 0u };
        const uint64_t block_time_ns = mOptions.nsPerBlock();
        const uint32_t blocks_per_frame = mOptions.blocksCount();
        const uint32_t channel_count = mOptions.mChannels;
        const uint32_t depth = _build_prebuilt_messages();

        mLock.unlock();

        if (mPrebuiltMessages.empty())
        {
            while (_can_loop())
                wait.wait(1u);

            continue;
        }

        set_image_gen_setting({ mOptions.mWidth, mOptions.mHeight, mOptions.mLines, mOptions.mPixelTime, mOptions.mChannels });

        const uint64_t blocks_per_quantum = (block_time_ns == 0u) ? 1u : std::max<uint64_t>(1u, static_cast<uint64_t>(COARSE_SLEEP_NS) / block_time_ns);

        uint64_t statDataSent{ 0u };
        uint32_t statBlockSent{ 0u };
        uint32_t statSentOntime{ 0u };
        uint32_t statSentLate{ 0u };

        const Clock::time_point time_begin{ Clock::now() };
        Clock::time_point stats_begin{ time_begin };

        auto flush_stats = [&]()
            {
                if (statBlockSent == 0u)
                {
                    return;
                }

                _update_data(statDataSent, statBlockSent, statSentOntime, statSentLate);

                statDataSent = 0u;
                statBlockSent = 0u;
                statSentOntime = 0u;
                statSentLate = 0u;
                stats_begin = Clock::now();
            };

        auto send_block_row = [&](uint32_t frame_id, uint32_t block_index)
            {
                const uint32_t generation = frame_id % depth;
                const uint32_t base_index = (generation * blocks_per_frame + block_index) * channel_count;

                for (auto& pool : mPrebuiltMessages)
                {
                    for (uint32_t ch = 0u; ch < channel_count; ++ch)
                    {
                        Remote& remote = pool.messages[base_index + ch];
                        areg::EventEnvelope& message = remote.message;
                        uint8_t* buffer = message.buffer();
                        ASSERT(buffer != nullptr);
                        RawImageBlock* block = reinterpret_cast<RawImageBlock*>(buffer + remote.offset);
                        block->frameSeqId = frame_id;
                        statDataSent += areg::send_raw_message(message) ? message.size_used() : 0;
                    }

                    statBlockSent += channel_count;
                }
            };

        uint64_t sent_blocks_total{ 0u };
        while (_can_loop())
        {
            if (block_time_ns == 0u)
            {
                for (uint32_t block_index{ 0u }; (block_index < blocks_per_frame) && _is_running(); ++block_index)
                {
                    send_block_row(block_index == 0 ? ++frame_id : frame_id, block_index);
                }

                statSentOntime    += (channel_count * blocks_per_frame);

                if (time_passed(stats_begin) >= STATS_FLUSH_NS)
                {
                    flush_stats();
                }

                continue;
            }

            const uint64_t elapsed_ns = time_passed(time_begin);
            const uint64_t should_have_sent = elapsed_ns / block_time_ns;

            if (should_have_sent <= sent_blocks_total)
            {
                const uint64_t next_deadline_ns = (sent_blocks_total + 1u) * block_time_ns;
                const int64_t delta_ns = static_cast<int64_t>(next_deadline_ns - elapsed_ns);

                if (delta_ns > COARSE_SLEEP_NS)
                {
                    flush_stats();
                    wait.wait_for(Nanoseconds{ COARSE_SLEEP_NS });
                }

                continue;
            }

            uint64_t due_blocks = should_have_sent - sent_blocks_total;
            if (due_blocks > blocks_per_quantum)
            {
                due_blocks = blocks_per_quantum;
            }

            if (due_blocks > 1024u)
            {
                due_blocks = 1024u;
            }

            while ((due_blocks != 0u) && _can_loop())
            {
                const uint32_t block_index = static_cast<uint32_t>(sent_blocks_total % blocks_per_frame);

                send_block_row(block_index == 0 ? ++frame_id : frame_id, block_index);
                ++sent_blocks_total;

                const uint64_t target_ns = sent_blocks_total * block_time_ns;
                const uint64_t now_ns = time_passed(time_begin);

                if (now_ns <= target_ns)
                {
                    statSentOntime += channel_count;
                }
                else
                {
                    statSentLate += channel_count;
                }

                --due_blocks;
            }

            if (time_passed(stats_begin) >= STATS_FLUSH_NS)
            {
                flush_stats();
            }
        }

        flush_stats();
    }
}

void ServicingComponent::_update_data(uint64_t sendData, uint32_t sentBlocks, uint32_t ontimeBlocks, uint32_t lateBlocks)
{
    areg::Lock lock(mLock);
    mDataRate.sentData      += sendData;
    mDataRate.sentBlocks    += sentBlocks;
    mDataRate.ontimeBlocks  += ontimeBlocks;
    mDataRate.lateBlocks    += lateBlocks;
}

void ServicingComponent::_print_info() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();

    _clear_opt_info();

    uint64_t bytes_per_block = mOptions.bytesPerBlock();
    uint64_t ns_per_block    = mOptions.nsPerBlock();

    // Blocks per second per channel.
    double block_rate = (ns_per_block > 0)
                        ? (static_cast<double>(areg::DURATION_1_SEC) / static_cast<double>(ns_per_block))
                        : 0.0;

    // Per-channel and total theoretical rates.
    uint64_t per_ch_bytes_sec = static_cast<uint64_t>(block_rate * static_cast<double>(bytes_per_block));
    uint64_t total_bytes_sec  = per_ch_bytes_sec * static_cast<uint64_t>(mOptions.mChannels);
    uint32_t total_blocks_sec = static_cast<uint32_t>(block_rate * static_cast<double>(mOptions.mChannels));

    areg::DataLiteral per_ch_rate = areg::conv_data_size(per_ch_bytes_sec);
    areg::DataLiteral total_rate  = areg::conv_data_size(total_bytes_sec);
    areg::DataLiteral block_size  = areg::conv_data_size(bytes_per_block);
    areg::DataLiteral time_rate   = areg::conv_duration(ns_per_block);

    // All label colons are aligned at column 26 (0-indexed: 25).  Format:
    //   8-space indent + label + fill-to-col-25 + ":" + " " + value
    int16_t row = static_cast<int16_t>(COORD_OPT_INFO.posY);
    console.output_txt ({ 1, row++ }, " ---------------------------------------" );
    console.output_txt ({ 1, row++ }, " Printing image current options:" );
    console.output_msg ({ 1, row++ }, " The large data state is : %s"             , mOptions.state().as_string());
    console.output_msg ({ 1, row++ }, "        Width ...........: %7u pix."       , mOptions.mWidth);
    console.output_msg ({ 1, row++ }, "        Height ..........: %7u pix."       , mOptions.mHeight);
    console.output_msg ({ 1, row++ }, "        Lines per Block .: %7u lns."       , mOptions.mLines);
    if (mOptions.mPixelTime == 0)
    {
        console.output_txt ({ 1, row++ }, "        Pixel Time ......:       0 ns (full speed).");
    }
    else
    {
        console.output_msg ({ 1, row++ }, "        Pixel Time ......: %7u ns."    , mOptions.mPixelTime);
    }

    console.output_msg ({ 1, row++ }, "        Channels ........: %7u ch."        , mOptions.mChannels);
    if (ns_per_block == 0)
    {
        console.output_txt ({ 1, row++ }, "        Time per Block ..:    Full Speed (IPC-limited).");
        console.output_msg ({ 1, row++ }, "        Block Size ......: %7.2f %s."  , static_cast<double>(block_size.first), block_size.second.data());
        console.output_txt ({ 1, row++ }, "        Block Rate ......:    IPC-limited (see client stats).");
        console.output_txt ({ 1, row++ }, "        Theoretical rate.:    N/A (pixel time = 0, rate is IPC-bound).");
        console.output_txt ({ 1, row++ }, "        Total Block Rate :    N/A");
        console.output_txt ({ 1, row++ }, "        Total rate ......:    N/A (IPC-limited).");
    }
    else
    {
        console.output_msg ({ 1, row++ }, "        Time per Block ..: %7.2f %s."  , static_cast<double>(time_rate.first),    time_rate.second.data());
        console.output_msg ({ 1, row++ }, "        Block Size ......: %7.2f %s."  , static_cast<double>(block_size.first),   block_size.second.data());
        console.output_msg ({ 1, row++ }, "        Block Rate ......: %7u blocks/sec/ch.", static_cast<uint32_t>(block_rate));
        console.output_msg ({ 1, row++ }, "        Theoretical rate.: %7.2f %s / sec (per channel)"
                        , static_cast<double>(per_ch_rate.first), per_ch_rate.second.data());
        console.output_msg ({ 1, row++ }, "        Total Block Rate : %7u blocks/sec" , total_blocks_sec);
        console.output_msg ({ 1, row++ }, "        Total rate ......: %7.2f %s / sec (all channels)"
                        , static_cast<double>(total_rate.first), total_rate.second.data());
    }

    console.output_msg ({ 1, row++ }, "        Connected client : %7d clients."   , mClients);
    console.output_txt ({ 1, row   }, " ---------------------------------------" );

    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::_print_help() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();

    // Clear the entire info/help region before writing.
    _clear_opt_info();

    int16_t row = static_cast<int16_t>(COORD_OPT_INFO.posY);
    console.output_txt ({ 1, row++ }, "---------------------------------------" );
    console.output_txt ({ 1, row++ }, "Commands (use short or long form, combine with spaces):" );
    console.output_txt ({ 1, row++ }, "-w=<value> or --width=<value> ....: Image width. Range [32 .. 32768]" );
    console.output_txt ({ 1, row++ }, "-h=<value> or --height=<value> ...: Image height. Range [32 .. 32768]" );
    console.output_txt ({ 1, row++ }, "-l=<value> or --lines=<value> ....: Lines per block, not larger than height." );
    console.output_txt ({ 1, row++ }, "-t=<value> or --time=<value> .....: Pixel dwell time in nanoseconds. Range [0 .. 100000], 0 = full speed." );
    console.output_txt ({ 1, row++ }, "-c=<value> or --channels=<value> .: Data source channels. Range [1 .. 64]." );
    console.output_txt ({ 1, row++ }, "-i         or --info .............: Print current option values." );
    console.output_txt ({ 1, row++ }, "-h         or --help .............: Print this help." );
    console.output_txt ({ 1, row++ }, "-s         or --start ............: Start generating and sending data." );
    console.output_txt ({ 1, row++ }, "-p         or --stop .............: Stop generating data." );
    console.output_txt ({ 1, row++ }, "-q         or --quit .............: Stop service and quit application." );
    console.output_txt ({ 1, row   }, "---------------------------------------" );

    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::_clear_opt_info() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    for (int16_t i = 0; i < OPT_INFO_LINES; ++i)
    {
        console.clear_line({ 1, static_cast<int16_t>(COORD_OPT_INFO.posY + i) });
    }
}
