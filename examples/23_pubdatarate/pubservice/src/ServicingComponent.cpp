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
#include "areg/logging/areg_log.h"
#include "aregextend/console/Console.hpp"

#include <chrono>

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
    mService.onOptionEvent( data );
}

//////////////////////////////////////////////////////////////////////////
// ServicingComponent::ServicingTimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void ServicingComponent::ServicingTimerConsumer::process_timer( areg::Timer & timer )
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
    : areg::Component         ( entry, owner )
    , LargeDataProviderBase     ( static_cast<areg::Component &>(self()) )
    , areg::ThreadConsumer  ( )

    , mBitmap           ( )
    , mSendList         ( )
    , mTimer            ( static_cast<areg::TimerConsumer &>(mTimerConsumer) , TIMER_NAME )
    , mInputThread      ( static_cast<areg::ThreadConsumer &>(self()), THREAD_WAITINPUT )
    , mImageThread      ( static_cast<areg::ThreadConsumer &>(self()), THREAD_GENERATE )
    , mOptions          ( )
    , mQuitThread       ( true )
    , mOptionChanged    ( true )
    , mPauseEvent       ( false, false )
    , mClients          ( 0 )
    , mDataRate         ( 0 )
    , mItemRate         ( 0 )
    , mDidSleep         ( 0 )
    , mIgnoreSleep      ( 0 )
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

void ServicingComponent::startup_service_interface( areg::Component & holder )
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, startup_service_interface );

    mQuitThread = false;
    mOptionChanged = false;
    // Start with the service in stopped state; image thread waits on mPauseEvent.
    mPauseEvent.reset();

    uint64_t sizeSent{ 0u }, sizeRecv{ 0u };
    uint32_t msgSent{ 0u }, msgRecv{ 0u };

    areg::Application::enable_data_rate(true);
    areg::Application::query_data_sent(sizeSent, msgSent);
    areg::Application::query_data_received(sizeRecv, msgRecv);
    uint64_t sizeItem = mItemRate != 0 ? mDataRate / mItemRate : 0;

    areg::DataLiteral dataRate = areg::conv_data_size(mDataRate);
    areg::DataLiteral itemRate = areg::conv_data_size( sizeItem );
    areg::DataLiteral sendRate = areg::conv_data_size(sizeSent);
    areg::DataLiteral rcvRate  = areg::conv_data_size(sizeRecv);

    // Compute theoretical (ideal) data rate for the initial display.
    uint64_t ns_per_block_0     = mOptions.nsPerBlock();
    uint64_t bytes_per_block_0  = mOptions.bytesPerBlock();
    double   ideal_bps_0        = (ns_per_block_0 > 0) ? (static_cast<double>(areg::DURATION_1_SEC) / static_cast<double>(ns_per_block_0)) : 0.0;
    uint64_t ideal_bytes_sec_0  = static_cast<uint64_t>(ideal_bps_0 * static_cast<double>(bytes_per_block_0) * static_cast<double>(mOptions.mChannels));
    uint32_t total_blocks_sec_0 = static_cast<uint32_t>(ideal_bps_0 * static_cast<double>(mOptions.mChannels));
    areg::DataLiteral idealRate0 = areg::conv_data_size(ideal_bytes_sec_0);

    areg::ext::Console& console = areg::ext::Console::instance();
    console.output_txt(COORD_TITLE,     MSG_APP_TITLE);
    console.output_txt(COORD_SEP1,      MSG_SEPARATOR);
    console.output_msg(COORD_COMM_RATE, MSG_COMM_RATE.data(), sendRate.first, sendRate.second.data(), rcvRate.first, rcvRate.second.data());
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data());
    console.output_msg(COORD_ITEM_RATE, MSG_ITEM_RATE.data(), msgSent, itemRate.first, itemRate.second.data());
    console.output_msg(COORD_STATS,     MSG_STATS.data(),     mDidSleep, mIgnoreSleep);
    console.output_msg(COORD_IDEAL_RATE,MSG_IDEAL_RATE.data(),idealRate0.first, idealRate0.second.data(), total_blocks_sec_0);
    console.output_txt(COORD_SEP2,      MSG_SEP2);
    _printInfo();

    _initBlockList();
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

    mQuitThread = true;
    mOptionChanged = true;
    mTimer.stop_timer();
    mPauseEvent.set_signaled();

    mBitmap.release();
    mInputThread.shutdown(areg::WAIT_INFINITE);
    mImageThread.shutdown(areg::WAIT_INFINITE);

    LargeDataProviderBase::shutdown_service_interface(holder);
}

bool ServicingComponent::consumer_connected(const areg::ProxyAddress& client, areg::ServiceConnectionState connectionStatus )
{
    bool result = LargeDataProviderBase::consumer_connected(client, connectionStatus );
    mClients += (areg::is_service_connected( connectionStatus ) ? 1 : -1);
    _printInfo();

    return result;
}

void ServicingComponent::on_timer_expired()
{
    mLock.lock(areg::WAIT_INFINITE);

    uint32_t rateItem    = mItemRate;
    uint32_t didSleep    = mDidSleep;
    uint32_t ignoreSleep = mIgnoreSleep;

    uint64_t sizeSent{ 0u }, sizeRecv{ 0u };
    uint32_t msgSent{ 0u }, msgRecv{ 0u };

    areg::Application::query_data_sent(sizeSent, msgSent);
    areg::Application::query_data_received(sizeRecv, msgRecv);
    uint64_t sizeItem = rateItem != 0 ? mDataRate / rateItem : 0;

    areg::DataLiteral dataRate = areg::conv_data_size( mDataRate );
    areg::DataLiteral itemRate = areg::conv_data_size(sizeItem);
    areg::DataLiteral sendRate = areg::conv_data_size(sizeSent);
    areg::DataLiteral rcvRate  = areg::conv_data_size(sizeRecv);

    mItemRate    = 0;
    mDataRate    = 0;
    mDidSleep    = 0;
    mIgnoreSleep = 0;
    mLock.unlock();

    // Compute the theoretical data rate from the current image parameters.
    // This is what the client should be receiving and is independent of OS timing jitter.
    uint64_t ns_per_block     = mOptions.nsPerBlock();
    uint64_t bytes_per_block  = mOptions.bytesPerBlock();
    double   ideal_blocks_sec = (ns_per_block > 0) ? (static_cast<double>(areg::DURATION_1_SEC) / static_cast<double>(ns_per_block)) : 0.0;
    uint64_t ideal_bytes_sec  = static_cast<uint64_t>(ideal_blocks_sec * static_cast<double>(bytes_per_block) * static_cast<double>(mOptions.mChannels));
    areg::DataLiteral idealRate = areg::conv_data_size(ideal_bytes_sec);

    const uint32_t total_blocks_sec = static_cast<uint32_t>(ideal_blocks_sec * static_cast<double>(mOptions.mChannels));

    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();

    console.output_msg( COORD_COMM_RATE,  MSG_COMM_RATE.data(), sendRate.first, sendRate.second.data(), rcvRate.first, rcvRate.second.data() );
    console.output_msg( COORD_DATA_RATE,  MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data() );
    console.output_msg( COORD_ITEM_RATE,  MSG_ITEM_RATE.data(), msgSent, itemRate.first, itemRate.second.data() );
    console.output_msg( COORD_STATS,      MSG_STATS.data(),     didSleep, ignoreSleep );
    if (ns_per_block == 0)
    {
        console.output_txt( COORD_IDEAL_RATE, " Theoretical rate .....: IPC-limited (pixel time = 0)." );
    }
    else
    {
        console.output_msg( COORD_IDEAL_RATE, MSG_IDEAL_RATE.data(), idealRate.first, idealRate.second.data(), total_blocks_sec );
    }

    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::onOptionEvent(const OptionData& data)
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

        mQuitThread    = true;
        mOptionChanged = true;
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

        mQuitThread    = false;
        mOptionChanged = true;
        mOptions.update(data);
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, component_thread(), areg::Timer::CONTINUOUSLY);
        // Signal the image thread to wake up and start sending.
        mPauseEvent.set_signaled();
        _printInfo();
    }
    else if (data.hasStop())
    {
        LOG_INFO("Requested to stop generating data");

        mOptionChanged = true;
        mOptions.update(data);
        mTimer.stop_timer();
        // Pause the image thread: it will exit the inner loop and block on mPauseEvent again.
        mPauseEvent.reset();
        _printInfo();
    }
    else if (data.hasPrintHelp())
    {
        LOG_INFO("Requested to print help");
        _printHelp();
    }
    else if (data.hasPrintInfo())
    {
        LOG_INFO("Requested to print info");
        _printInfo();
    }
    else
    {
        // Parameter-only change (width / height / lines / pixel-time / channels).
        LOG_INFO("Requested to change generating data parameter(s)");

        const bool was_running = mOptions.hasStart();

        // update() preserves the CmdStart/CmdStop flag when no start/stop is in 'data'.
        // NOTE: Do NOT call _initBlockList() here. The image thread reads mOptions and
        // mSendList without holding mLock between mPauseEvent.lock() and the inner loop.
        // Rebuilding mSendList here (after mOptions is already updated) creates a race
        // where the image thread can read the new blocksCount() from mOptions but find
        // mSendList still sized for the old options (or vice-versa).
        // Instead, _initBlockList() is called inside _runImageThread() under mLock after
        // the thread wakes, ensuring mSendList is always consistent with mOptions.
        mLock.lock(areg::WAIT_INFINITE);
        mOptions.update(data);
        mLock.unlock();

        _printInfo();

        // Tell the image thread to reload its parameters.  It will exit the inner
        // frame loop, fall through to mPauseEvent.lock(), and re-read mOptions and
        // rebuild mSendList atomically.
        mOptionChanged = true;

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
        _runInputThread();
    }
    else if (threadName == THREAD_GENERATE )
    {
        LOG_DBG("Started generate image thread.");
        _runImageThread();
    }
}





void ServicingComponent::_runInputThread()
{
    areg::ext::Console& console = areg::ext::Console::instance();

    bool cmdQuit{ false };
    while ((cmdQuit == false) && (mQuitThread == false))
    {
        LOG_SCOPE( examples_23_pubservice_ServicingComponent, run_input_thread );
        LOG_DBG("Waiting to enter option command ...");

        // Output the prompt at the designated row then block on a full-line read.
        // wait_for_input() uses gets_s / fgets which consume the entire line including
        // the newline, so no stale input remains in stdin between iterations.
        console.output_txt(COORD_OPTIONS, MSG_INPUT_OPTION);
        console.set_cursor_cur_position({ static_cast<int16_t>(COORD_OPTIONS.posX + static_cast<int16_t>(MSG_INPUT_OPTION.size()))
                                        , COORD_OPTIONS.posY });
        console.refresh_screen();
        areg::String cmd = console.wait_for_input(nullptr);

        // Erase any characters the user typed that remain visible in the console
        // buffer on the prompt row (gets_s writes them there directly).  Also clear
        // the error row so a previous error message does not linger.
        console.clear_line(COORD_OPTIONS);
        console.clear_line(COORD_ERROR_INFO);

        cmd.make_lower();
        OptionData newData;
        newData.parseCommand(cmd);
        cmdQuit = newData.hasQuit();
        EventOption::send_event(newData, static_cast<IEOptionConsumer&>(mOptionConsumer), component_thread());

        LOG_DBG("Received option command [ %s ]", cmd.as_string());
    }
}

void ServicingComponent::_runImageThread()
{
    LOG_SCOPE( examples_23_pubservice_ServicingComponent, run_image_thread );

    // Threshold below which per-block waits are unreliable on most OSes.
    // Below this value we use frame-level timing instead.
    static constexpr int64_t MIN_BLOCK_WAIT_NS{ 250'000LL };  // 1 ms

    areg::Wait wait;

    while (mQuitThread == false)
    {
        // Block here until the service is started or options change.
        mPauseEvent.lock();
        if (mQuitThread)
        {
            break;
        }

        mOptionChanged.store(false, std::memory_order_relaxed);

        // Rebuild the flat send list under mLock so it is always consistent with mOptions.
        mLock.lock(areg::WAIT_INFINITE);
        _initBlockList();
        const int64_t  ns_per_block  = static_cast<int64_t>(mOptions.nsPerBlock());
        const uint32_t blocks        = mOptions.blocksCount();
        const uint32_t channels      = mOptions.mChannels;
        mLock.unlock();

        // Total entries in the pre-built send list (blocks × channels).
        const uint32_t total_sends   = blocks * channels;
        // Pre-computed per-frame totals: constant across all frames for the current config.
        const uint32_t block_size    = total_sends > 0u ? mSendList[0].getSize() : 0u;
        const uint32_t frame_bytes   = block_size * total_sends;
        const int64_t  ns_per_frame  = ns_per_block * static_cast<int64_t>(blocks);
        uint32_t seqNr = 0;

        // Choose timing strategy based on the block period:
        //   - Per-block timing: each block position's channels are sent, then we wait for
        //     its individual deadline. Reliable when nsPerBlock >= 1ms.
        //   - Frame-level timing: all entries sent as fast as possible; single wait at the
        //     end of the frame. Used when nsPerBlock < 1ms to avoid spin-wait overhead.
        const bool use_frame_timing = (ns_per_block < MIN_BLOCK_WAIT_NS);

        // Run frames until options change or service stops.
        while ((mQuitThread.load(std::memory_order_relaxed) == false) &&
               (mOptionChanged.load(std::memory_order_relaxed) == false) &&
               mOptions.hasStart())
        {
            const std::chrono::steady_clock::time_point frame_start    = std::chrono::steady_clock::now();
            const std::chrono::steady_clock::time_point frame_deadline =
                frame_start + std::chrono::nanoseconds(ns_per_frame);

            if (use_frame_timing)
            {
                // Hot path: send all entries as fast as possible, then wait once per frame.
                // Single flat loop — no inner channel loop, no bounds check, one write per entry.
                for (uint32_t j = 0; (mOptionChanged.load(std::memory_order_relaxed) == false) && (j < total_sends); ++j)
                {
                    mSendList[j].set_frame_id(seqNr);
                    LargeDataProviderBase::broadcast_image_block_acquired(mSendList[j]);
                }

                _updateData(frame_bytes, total_sends, wait.wait_until(frame_deadline));
            }
            else
            {
                // Per-block timing: broadcast all channels for block i, then wait for its deadline.
                const uint32_t block_bytes = block_size * channels;

                for (uint32_t i = 0; (mOptionChanged.load(std::memory_order_relaxed) == false) && (i < blocks); ++i)
                {
                    const uint32_t base = i * channels;
                    for (uint32_t k = base; k < base + channels; ++k)
                    {
                        mSendList[k].set_frame_id(seqNr);
                        LargeDataProviderBase::broadcast_image_block_acquired(mSendList[k]);
                    }

                    const std::chrono::steady_clock::time_point block_deadline =
                        frame_start + std::chrono::nanoseconds(static_cast<int64_t>(i + 1) * ns_per_block);
                    _updateData(block_bytes, channels, wait.wait_until(block_deadline));
                }
            }

            seqNr = (seqNr + 1) % (mOptions.mHeight != 0 ? mOptions.mHeight : 1u);
        }
    }
}

void ServicingComponent::_updateData(uint64_t genData, uint32_t genBlocks, areg::Wait::WaitResolution waitResult)
{
    areg::Lock lock(mLock);
    mDataRate += genData;
    mItemRate += genBlocks;
    if (waitResult >= areg::Wait::WaitResolution::Microsecond)
    {
        mDidSleep += 1;
    }
    else
    {
        mIgnoreSleep += 1;
    }
}


uint64_t ServicingComponent::_getBlockImageTime() const
{
    return static_cast<uint64_t>(mOptions.mWidth) * mOptions.mLines * mOptions.mPixelTime;
}

void ServicingComponent::_printInfo() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();

    // Clear the entire info/help region before writing so no stale text from a
    // previous _printHelp (or longer _printInfo) remains visible.
    _clearOptInfo();

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
    console.output_msg ({ 1, row++ }, " The large data state is : %s"             , mOptions.state().buffer());
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

void ServicingComponent::_printHelp() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();

    // Clear the entire info/help region before writing.
    _clearOptInfo();

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

void ServicingComponent::_initBlockList()
{
    mBitmap.create_bitmap(mOptions.mWidth, mOptions.mHeight);

    const uint32_t blocks   = mOptions.blocksCount();
    const uint32_t channels = mOptions.mChannels;

    // Pre-build a flat send queue of size blocks × channels.
    // Entry [i * channels + ch] is a complete copy of image block i with channelId = ch
    // already stamped. The hot loop only needs to update frameSeqId via set_frame_id(),
    // removing the inner channel iteration and all per-channel block re-fetches.
    mSendList.clear();
    mSendList.resize(blocks * channels);

    for (uint32_t i = 0; i < blocks; ++i)
    {
        for (uint32_t ch = 0; ch < channels; ++ch)
        {
            ImageBlock& entry = mSendList[i * channels + ch];
            entry = mBitmap.block(i * mOptions.mLines, mOptions.mLines);
            entry.setIds(ch, 0u);   // pre-stamp channelId; frameSeqId is set per-frame
        }
    }
}

void ServicingComponent::_clearOptInfo() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    for (int16_t i = 0; i < OPT_INFO_LINES; ++i)
    {
        console.clear_line({ 1, static_cast<int16_t>(COORD_OPT_INFO.posY + i) });
    }
}
