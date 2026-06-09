/************************************************************************
 * \file        pubconsumer/src/LatencyConsumer.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service consumer component implementation.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubconsumer/src/LatencyConsumer.hpp"
#include "common/latency_common.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ComponentThread.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// LatencyConsumer::CmdConsumer implementation
//////////////////////////////////////////////////////////////////////////

void LatencyConsumer::CmdConsumer::process_event(const CmdData & data)
{
    mOwner._on_cmd_event(data);
}

//////////////////////////////////////////////////////////////////////////
// LatencyConsumer implementation
//////////////////////////////////////////////////////////////////////////

LatencyConsumer::LatencyConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LatencyConsumerBase   ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(*this) )
    , areg::TimerConsumer   ( )
    , areg::ThreadConsumer  ( )

    , mInputThread  ( static_cast<areg::ThreadConsumer &>(*this), THREAD_INPUT )
    , mQuit         ( false )
    , mCmdConsumer  ( *this )
    , mDisplayConsumer( *this )
    , mDisplayThread( static_cast<areg::ThreadConsumer &>(mDisplayConsumer), THREAD_DISPLAY )

    , mMode         ( Latency::LatencyMode::Request0 )
    , mCount        ( DEFAULT_COUNT )
    , mWarmup       ( DEFAULT_WARMUP )
    , mDurationSec  ( DEFAULT_DURATION )
    , mCsvEnabled   ( false )
    , mCsvPath      ( )

    , mCurrentSeq   ( 0u )
    , mTestRunning  ( false )
    , mTotalRuns    ( 0u )
    , mSamples      ( )
    , mSampleCount  ( 0u )

    , mRunMin       ( INT64_MAX )
    , mRunMax       ( 0 )
    , mRunSum       ( 0 )
    , mLastRateCount( 0u )

    , mResults      { }
{
}

void LatencyConsumer::startup_component(areg::ComponentThread & /* comThread */)
{
    mQuit.store(false, std::memory_order_relaxed);
    mTestRunning    = false;
    mCsvEnabled     = false;
    mCurrentSeq     = 0u;
    mTotalRuns      = 0u;
    mSamples.clear();
    mRunMin         = INT64_MAX;
    mRunMax         = 0;
    mRunSum         = 0;
    mLastRateCount  = 0u;

    for (uint32_t i = 0u; i < MAX_RESULT_ROWS; ++i)
        mResults[i] = ResultEntry {};

    mSampleCount.store(0u, std::memory_order_relaxed);
    areg::ext::Console::instance().enable_console_input(true);
    _redraw_layout();
    mDisplayThread.start(areg::WAIT_INFINITE);
    mInputThread.start(areg::WAIT_INFINITE);
}

void LatencyConsumer::shutdown_component(areg::ComponentThread & /* comThread */) noexcept
{
    mTestRunning = false;
    mQuit.store(true, std::memory_order_relaxed);
    areg::ext::Console::instance().enable_console_input(false);
    mInputThread.shutdown(areg::WAIT_INFINITE);
    mDisplayThread.shutdown(areg::WAIT_INFINITE);
}

bool LatencyConsumer::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    const bool result = LatencyConsumerBase::service_connected(status, proxy);

    if (is_connected())
    {
        notify_on_latency_setup_update(true);
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
    }
    else
    {
        clear_all_notifications();
        mTestRunning = false;
        areg::Application::signal_quit();
    }

    _update_settings();
    return result;
}

void LatencyConsumer::response_start_mode(const Latency::LantencySetup & setup)
{
    const Latency::LatencyMode mode = setup.mode;

    if (mode == Latency::LatencyMode::QuitApp)
    {
        mTestRunning = false;
        return;
    }

    if (mode == Latency::LatencyMode::Stop)
        return;

    // Provider confirmed the test setup — now activate the test on the consumer side.
    mTestRunning = true;

    const uint16_t val       = static_cast<uint16_t>(mode);
    const bool     is_request = (val >= static_cast<uint16_t>(Latency::LatencyMode::Request0));

    // Kick the loop. Request (ping-pong) sends the first request; broadcast (one-way) pulls the
    // first message. Both keep exactly one message in flight, so the pipeline stays warm.
    if (is_request)
        _send_next_ping();
    else
        _send_next_oneway();
}

void LatencyConsumer::response_ping_pong_0(uint32_t /* id */, uint64_t start, uint64_t replied)
{
    _record_sample(start, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_8(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data8 & /* data8 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_16(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data16 & /* data16 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_32(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data32 & /* data32 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_64(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data64 & /* data64 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_128(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data128 & /* data128 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_256(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data256 & /* data256 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_512(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data512 & /* data512 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_1024(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data1024 & /* data1024 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_4096(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data4096 & /* data4096 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::response_ping_pong_65536(uint32_t /* id */, uint64_t begin, uint64_t replied, const Latency::Data65536 & /* data65536 */)
{
    _record_sample(begin, replied, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_0(uint32_t /* id */, uint64_t begin)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_8(uint32_t /* id */, uint64_t begin, const Latency::Data8 & /* data8 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_16(uint32_t /* id */, uint64_t begin, const Latency::Data16 & /* data16 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_32(uint32_t /* id */, uint64_t begin, const Latency::Data32 & /* data32 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_64(uint32_t /* id */, uint64_t begin, const Latency::Data64 & /* data64 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_128(uint32_t /* id */, uint64_t begin, const Latency::Data128 & /* data128 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_256(uint32_t /* id */, uint64_t begin, const Latency::Data256 & /* data256 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_512(uint32_t /* id */, uint64_t begin, const Latency::Data512 & /* data512 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_1024(uint32_t /* id */, uint64_t begin, const Latency::Data1024 & /* data1024 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_4096(uint32_t /* id */, uint64_t begin, const Latency::Data4096 & /* data4096 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::broadcast_message_65536(uint32_t /* id */, uint64_t begin, const Latency::Data65536 & /* data65536 */)
{
    _record_sample(begin, 0u, Latency::now_ns());
}

void LatencyConsumer::on_latency_setup_update(const Latency::LantencySetup & setup, areg::DataState /* state */)
{
    if (setup.mode == Latency::LatencyMode::QuitApp)
    {
        mTestRunning = false;
        areg::Application::signal_quit();
        return;
    }
}

void LatencyConsumer::process_timer(areg::Timer & /* timer */)
{
}

void LatencyConsumer::on_run()
{
    _run_input_thread();
}

void LatencyConsumer::DisplayConsumer::on_run()
{
    mOwner._run_display_thread();
}

void LatencyConsumer::_redraw_layout()
{
    areg::ext::Console & console = areg::ext::Console::instance();
    console.clear_screen();
    console.output_txt(COORD_TITLE,   MSG_TITLE);
    console.output_txt(COORD_SEP1,    MSG_SEPARATOR);
    console.output_msg(COORD_LIVE_1,  "%-120s", "");
    console.output_msg(COORD_LIVE_2,  "%-120s", "");
    console.output_txt(COORD_SEP2,    MSG_SEPARATOR);
    console.output_txt(COORD_PROMPT,  MSG_PROMPT);
    console.output_msg(COORD_INPUT,   " > %-100s", "");
    console.output_txt(COORD_SEP3,    MSG_SEPARATOR);
    console.output_txt(COORD_EX_HDR,  MSG_EX_HDR);
    console.output_txt(COORD_EX1,     MSG_EX1);
    console.output_txt(COORD_EX2,     MSG_EX2);
    console.output_txt(COORD_EX3,     MSG_EX3);
    console.output_txt(COORD_EX4,     MSG_EX4);
    console.output_txt(COORD_EX5,     MSG_EX5);
    console.output_txt(COORD_EX6,     MSG_EX6);
    console.output_txt(COORD_EX7,     MSG_EX7);
    console.output_txt(COORD_EX8,     MSG_EX8);
    console.output_txt(COORD_SEP4,    MSG_SEPARATOR);
    console.output_msg(COORD_SETTINGS,"%-120s", "");
    console.output_txt(COORD_SEP5,    MSG_SEPARATOR);
    _update_settings();
    _redraw_results_table();
}

void LatencyConsumer::_update_live()
{
    const uint32_t samples = mSampleCount.load(std::memory_order_relaxed);
    const uint32_t delta   = (samples >= mLastRateCount) ? (samples - mLastRateCount) : 0u;
    mLastRateCount          = samples;

    const char * state_str = mTestRunning
                           ? (mCurrentSeq > mWarmup ? "running" : "warmup")
                           : (samples > 0u ? "done"    : "stopped");

    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();

    console.output_msg(COORD_LIVE_1, "%-120s", "");
    console.output_msg(COORD_LIVE_2, "%-120s", "");

    console.output_msg(COORD_LIVE_1, MSG_LIVE_1.data()
                     , Latency::mode_as_str(mMode), state_str, samples, static_cast<double>(delta));

    if ((samples != 0u) && (mRunMin != INT64_MAX))
    {
        const double min_us  = static_cast<double>(mRunMin) / 1000.0;
        const double mean_us = static_cast<double>(mRunSum) / static_cast<double>(samples) / 1000.0;
        const double max_us  = static_cast<double>(mRunMax) / 1000.0;
        console.output_msg(COORD_LIVE_2, MSG_LIVE_2.data(), min_us, mean_us, max_us);
    }

    console.restore_cursor_position();
    console.refresh_screen();
}

void LatencyConsumer::_update_settings() const
{
    areg::ext::Console & console = areg::ext::Console::instance();
    const areg::String path = !mCsvEnabled ? areg::String("off")
                            : mCsvPath.is_empty() ? areg::String("auto")
                            : mCsvPath;
    console.save_cursor_position();
    console.output_msg(COORD_SETTINGS, MSG_SETTINGS.data()
                     , Latency::mode_as_str(mMode)
                     , mCount
                     , mDurationSec
                     , mWarmup
                     , is_connected() ? "yes" : "no"
                     , mTestRunning   ? "yes" : "no"
                     , path.as_string());
    console.restore_cursor_position();
    console.refresh_screen();
}

void LatencyConsumer::_run_input_thread()
{
    static const areg::ext::OptionParser::OptionSetup opts[]
    {
          { "-q", "quit",   static_cast<int32_t>(ConsumerCmd::Quit),     areg::ext::OptionParser::FREESTYLE_DATA,   {}, {}, {} }
        , { "-h", "help",   static_cast<int32_t>(ConsumerCmd::Help),     areg::ext::OptionParser::NO_DATA,          {}, {}, {} }
        , { "-i", "info",   static_cast<int32_t>(ConsumerCmd::Info),     areg::ext::OptionParser::NO_DATA,          {}, {}, {} }
        , { "-s", "start",  static_cast<int32_t>(ConsumerCmd::Start),    areg::ext::OptionParser::NO_DATA,          {}, {}, {} }
        , { "-p", "stop",   static_cast<int32_t>(ConsumerCmd::Stop),     areg::ext::OptionParser::NO_DATA,          {}, {}, {} }
        , { "-m", "mode",   static_cast<int32_t>(ConsumerCmd::Mode),     areg::ext::OptionParser::STRING_IN_RANGE,  {}, {}, { "pp0", "pp8", "pp16", "pp32", "pp64", "pp128", "pp256", "pp512", "pp1024", "pp4096", "pp65536", "bc0", "bc8", "bc16", "bc32", "bc64", "bc128", "bc256", "bc512", "bc1024", "bc4096", "bc65536" } }
        , { "-c", "count",  static_cast<int32_t>(ConsumerCmd::Count),    areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {} }
        , { "-w", "warmup", static_cast<int32_t>(ConsumerCmd::Warmup),   areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {} }
        , { "-d", "dur",    static_cast<int32_t>(ConsumerCmd::Duration), areg::ext::OptionParser::INTEGER_NO_RANGE, {}, {}, {} }
        , { "-o", "out",    static_cast<int32_t>(ConsumerCmd::Output),   areg::ext::OptionParser::FREESTYLE_DATA,   {}, {}, {} }
        , { "-f", "file",   static_cast<int32_t>(ConsumerCmd::File),     areg::ext::OptionParser::FREESTYLE_DATA,   {}, {}, {} }
    };

    areg::ext::OptionParser parser(opts, static_cast<uint32_t>(sizeof(opts) / sizeof(opts[0])));
    areg::ext::Console & console = areg::ext::Console::instance();

    while (!mQuit.load(std::memory_order_relaxed))
    {
        console.output_msg(COORD_INPUT, " > %-100s", "");
        console.set_cursor_cur_position({ 4, COORD_INPUT.posY });
        console.refresh_screen();

        const areg::String line = console.wait_for_input(nullptr);

        // Erase the echoed command and reset cursor, prevents the typed text
        // from drifting down to the next row after Enter.
        console.output_msg(COORD_INPUT, " > %-100s", "");
        console.set_cursor_cur_position({ 4, COORD_INPUT.posY });
        console.refresh_screen();

        if (mQuit.load(std::memory_order_relaxed))
            break;

        areg::String trimmed(line);
        trimmed.trim_all();
        if (trimmed.is_empty())
            continue;

        parser.parse_option_line(trimmed.as_string());

        CmdData data;

        const areg::ext::OptionParser::InputOptionList & parsed = parser.options();
        for (uint32_t i = 0u; i < parsed.size(); ++i)
        {
            const areg::ext::OptionParser::InputOption & opt = parsed.value_at(i);

            if (areg::ext::OptionParser::has_input_error(opt.inField))
            {
                data.error = true;
                continue;
            }

            switch (static_cast<ConsumerCmd>(opt.inCommand))
            {
            case ConsumerCmd::Quit:
                if (!opt.inString.empty() && opt.inString[0] == "1")
                    data.quit_local = true;
                else
                    data.quit = true;
                break;

            case ConsumerCmd::Help:
                data.help = true;
                break;

            case ConsumerCmd::Info:
                data.info = true;
                break;

            case ConsumerCmd::Start:
                data.start = true;
                break;

            case ConsumerCmd::Stop:
                data.stop = true;
                break;

            case ConsumerCmd::Mode:
                if (!opt.inString.empty())
                {
                    // String range check is case-insensitive; stored value preserves user case.
                    areg::String lc(opt.inString[0]);
                    lc.make_lower();
                    data.mode = Latency::string_as_mode(lc);
                    if (data.mode == Latency::LatencyMode::Undefined)
                        data.error = true;
                    else
                        data.set_mode = true;
                }
                else
                {
                    data.error = true;
                }
                break;

            case ConsumerCmd::Count:
                if (areg::ext::OptionParser::is_integer(opt.inField))
                {
                    data.count     = static_cast<uint32_t>(opt.inValue.valInt);
                    data.set_count = true;
                }
                else { data.error = true; }
                break;

            case ConsumerCmd::Warmup:
                if (areg::ext::OptionParser::is_integer(opt.inField))
                {
                    data.warmup     = static_cast<uint32_t>(opt.inValue.valInt);
                    data.set_warmup = true;
                }
                else { data.error = true; }
                break;

            case ConsumerCmd::Duration:
                if (areg::ext::OptionParser::is_integer(opt.inField))
                {
                    data.duration_sec = static_cast<uint32_t>(opt.inValue.valInt);
                    data.set_duration = true;
                }
                else { data.error = true; }
                break;

            case ConsumerCmd::Output:
                if (!opt.inString.empty())
                {
                    data.csv_path   = opt.inString[0];
                    data.set_output = true;
                }
                else { data.error = true; }
                break;

            case ConsumerCmd::File:
                data.enable_file = true;
                if (!opt.inString.empty())
                    data.csv_file = opt.inString[0];
                break;

            default:
                data.error = true;
                break;
            }
        }

        if (data.quit || data.quit_local)
            mQuit.store(true, std::memory_order_relaxed);

        const bool has_any = data.quit || data.quit_local || data.help || data.info
                           || data.start || data.stop || data.set_mode || data.set_count
                           || data.set_warmup || data.set_duration || data.set_output
                           || data.enable_file || data.error;

        if (has_any)
        {
            EventCommand::send_event(data, static_cast<IECmdConsumer &>(mCmdConsumer), master_thread());
        }
    }
}

void LatencyConsumer::_on_cmd_event(const CmdData & data)
{
    if (data.error)
    {
        areg::ext::Console & console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_msg(COORD_SETTINGS, " ERROR: unknown command or bad value. Type -h for help.%-50s", "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    if (data.set_mode)     mMode        = data.mode;
    if (data.set_count)    mCount       = (data.count > 0u) ? data.count : mCount;
    if (data.set_warmup)   mWarmup      = data.warmup;
    if (data.set_duration) mDurationSec = data.duration_sec;
    if (data.set_output)   mCsvPath     = data.csv_path;
    if (data.enable_file)
    {
        mCsvEnabled = true;
        if (!data.csv_file.is_empty())
            mCsvPath = data.csv_file;
    }

    if (data.quit)
    {
        mTestRunning = false;
        if (is_connected())
            request_start_mode(Latency::LatencyMode::QuitApp, 0u, 0u, 0u);

        areg::Application::signal_quit();
    }
    else if (data.quit_local)
    {
        mTestRunning = false;
        areg::Application::signal_quit();
    }
    else if (data.help)
    {
        _redraw_layout();
    }
    else if (data.info)
    {
        _update_settings();
    }
    else if (data.stop)
    {
        _stop_test();
    }
    else if (data.start)
    {
        _start_test();
    }
    else
    {
        _update_settings();
    }
}

void LatencyConsumer::_start_test()
{
    areg::ext::Console & console = areg::ext::Console::instance();

    if (!is_connected())
    {
        console.save_cursor_position();
        console.output_msg(COORD_SETTINGS, " ERROR: not connected to service provider.%-50s", "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    if (mCount == 0u)
    {
        console.save_cursor_position();
        console.output_msg(COORD_SETTINGS, " ERROR: count must be > 0. Use -c=N or count=N.%-40s", "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    mSamples.clear();
    mSamples.reserve(mCount);
    mSampleCount.store(0u, std::memory_order_relaxed);
    mCurrentSeq     = 0u;
    mTestRunning    = false;    // will be set to true when provider confirms in response_start_mode
    mRunMin         = INT64_MAX;
    mRunMax         = 0;
    mRunSum         = 0;
    mLastRateCount  = 0u;

    const uint16_t val       = static_cast<uint16_t>(mMode);
    const bool     is_request = (val >= static_cast<uint16_t>(Latency::LatencyMode::Request0));

    if (is_request)
    {
        // Request (ping-pong) mode: pass warmup count; duration is driven by count, not time.
        request_start_mode(mMode, 0u, mWarmup, mCount);
    }
    else
    {
        // Broadcast (one-way) mode: count-bounded and pull-driven (warm, 1-in-flight), exactly
        // like ping-pong. `duration` is no longer used for pacing (kept only as CSV metadata);
        // the run stops as soon as `count` samples are collected, so it can never run endlessly.
        request_start_mode(mMode, mDurationSec, mWarmup, mCount);
    }

    _update_settings();
}

void LatencyConsumer::_stop_test()
{
    if (!mTestRunning)
    {
        areg::ext::Console & console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_msg(COORD_SETTINGS, " No test is running.%-80s", "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    mTestRunning = false;

    if (is_connected())
        request_start_mode(Latency::LatencyMode::Stop, 0u, 0u, 0u);

    if (!mSamples.is_empty())
        _finish_test();
}

void LatencyConsumer::_send_next_ping()
{
    if (!is_connected() || !mTestRunning)
        return;

    switch (mMode)
    {
    case Latency::LatencyMode::Request0:
        request_ping_pong_0(mCurrentSeq, Latency::now_ns());
        break;
    case Latency::LatencyMode::Request8:
        { Latency::Data8   d {}; request_ping_pong_8(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request16:
        { Latency::Data16  d {}; request_ping_pong_16(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request32:
        { Latency::Data32  d {}; request_ping_pong_32(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request64:
        { Latency::Data64  d {}; request_ping_pong_64(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request128:
        { Latency::Data128 d {}; request_ping_pong_128(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request256:
        { Latency::Data256 d {}; request_ping_pong_256(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request512:
        { Latency::Data512 d {}; request_ping_pong_512(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request1024:
        { Latency::Data1024 d {}; d.data_rest.set_size_used(1024 - 128 - 64); request_ping_pong_1024(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request4096:
    { Latency::Data4096 d{}; d.data_rest.set_size_used(4096 - 128 - 64); request_ping_pong_4096(mCurrentSeq, Latency::now_ns(), d); }
        break;
    case Latency::LatencyMode::Request65536:
        { Latency::Data65536 d {}; d.data_rest.set_size_used(65536 - 128 - 64); request_ping_pong_65536(mCurrentSeq, Latency::now_ns(), d); }
        break;
    default:
        break;
    }
}

void LatencyConsumer::_send_next_oneway()
{
    // Pull the next one-way broadcast from the provider. Sent once per received broadcast (and
    // through warmup), this keeps exactly one message in flight: the provider's send thread is
    // idle when it stamps the timestamp, so the measured now_ns() - begin is a true one-way
    // delivery latency rather than a queuing delay. The provider stamps its own running id; the
    // value we pass is only for drop detection.
    if (!is_connected() || !mTestRunning)
        return;

    request_message_next(mCurrentSeq);
}

void LatencyConsumer::_record_sample(uint64_t t1, uint64_t t2, uint64_t t4)
{
    if (!mTestRunning)
        return;

    ++mCurrentSeq;
    const int64_t rtt = static_cast<int64_t>(t4) - static_cast<int64_t>(t1);

    if (mCurrentSeq <= mWarmup)
    {
        const uint16_t val = static_cast<uint16_t>(mMode);
        if (val >= static_cast<uint16_t>(Latency::LatencyMode::Request0))
            _send_next_ping();
        else
            _send_next_oneway();    // keep the one-way loop alive through warmup
        return;
    }

    LatencySample s;
    s.seq    = mCurrentSeq - mWarmup;
    s.t1_ns  = t1;
    s.t2_ns  = t2;
    s.t4_ns  = t4;
    s.rtt_ns = rtt;
    mSamples.add(s);
    mSampleCount.fetch_add(1u, std::memory_order_relaxed);

    if (rtt < mRunMin) mRunMin = rtt;
    if (rtt > mRunMax) mRunMax = rtt;
    mRunSum += rtt;

    const uint32_t measured        = mSamples.size();
    const bool     test_done       = (mCount > 0u) && (measured >= mCount);
    const uint16_t val             = static_cast<uint16_t>(mMode);
    const bool     is_request_mode = (val >= static_cast<uint16_t>(Latency::LatencyMode::Request0));

    if (test_done)
    {
        mTestRunning = false;
        _finish_test();

        if (is_connected())
            request_start_mode(Latency::LatencyMode::Stop, 0u, 0u, 0u);
    }
    else if (is_request_mode)
    {
        _send_next_ping();
    }
    else
    {
        _send_next_oneway();    // pull the next one-way message
    }
}

void LatencyConsumer::_finish_test()
{
    const uint32_t N = mSamples.size();
    if (N == 0u)
    {
        areg::ext::Console & console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_msg(COORD_LIVE_1, MSG_LIVE_1.data(), Latency::mode_as_str(mMode), "done", 0u, 0.0);
        console.output_msg(COORD_LIVE_2, "%-120s", " No samples collected.");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    std::vector<int64_t> sorted;
    sorted.reserve(N);
    for (uint32_t i = 0u; i < N; ++i)
        sorted.push_back(mSamples.value_at(i).rtt_ns);

    std::sort(sorted.begin(), sorted.end());

    int64_t sum = 0;
    for (int64_t v : sorted) sum += v;
    const double mean_ns = static_cast<double>(sum) / static_cast<double>(N);

    auto percentile = [&](double pct) -> int64_t
    {
        const size_t idx = static_cast<size_t>(pct * static_cast<double>(N - 1u) / 100.0 + 0.5);
        return sorted[idx < static_cast<size_t>(N) ? idx : static_cast<size_t>(N) - 1u];
    };

    const double min_us  = static_cast<double>(sorted.front()) / 1000.0;
    const double p50_us  = static_cast<double>(percentile(50.0)) / 1000.0;
    const double p95_us  = static_cast<double>(percentile(95.0)) / 1000.0;
    const double p99_us  = static_cast<double>(percentile(99.0)) / 1000.0;
    const double max_us  = static_cast<double>(sorted.back())  / 1000.0;
    const double mean_us = mean_ns / 1000.0;

    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();
    console.output_msg(COORD_LIVE_1, "%-120s", "");
    console.output_msg(COORD_LIVE_2, "%-120s", "");
    console.output_msg(COORD_LIVE_1, " Mode: %-6s  Status: done      Samples: %-6u", Latency::mode_as_str(mMode), N);
    console.output_msg(COORD_LIVE_2, MSG_LIVE_FINAL.data(), min_us, p50_us, p95_us, p99_us, max_us, mean_us);
    console.restore_cursor_position();
    console.refresh_screen();

    ++mTotalRuns;
    const uint32_t slot = (mTotalRuns - 1u) % MAX_RESULT_ROWS;

    ResultEntry & r = mResults[slot];
    r.run_no  = mTotalRuns;
    r.mode    = Latency::mode_as_str(mMode);
    r.count   = N;
    r.min_us  = min_us;
    r.p50_us  = p50_us;
    r.p95_us  = p95_us;
    r.p99_us  = p99_us;
    r.max_us  = max_us;
    r.mean_us = mean_us;
    r.valid   = true;

    _draw_result_row(slot);
    if (mCsvEnabled)
        _save_csv(r);
}

void LatencyConsumer::_draw_result_row(uint32_t slot) const
{
    const areg::ext::Console::Coord coord
    {
          COORD_TBL_ROW0.posX
        , static_cast<int16_t>(COORD_TBL_ROW0.posY + static_cast<int16_t>(slot))
    };

    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();

    const ResultEntry & r = mResults[slot];
    if (r.valid)
    {
        console.output_msg(coord, MSG_TBL_ROW.data()
                         , r.run_no, r.mode.as_string(), r.count
                         , r.min_us, r.p50_us, r.p95_us, r.p99_us, r.max_us, r.mean_us);
    }
    else
    {
        console.output_txt(coord, MSG_TBL_EMPTY);
    }

    console.restore_cursor_position();
    console.refresh_screen();
}

void LatencyConsumer::_redraw_results_table() const
{
    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();

    console.output_txt(COORD_RES_HDR, MSG_RES_HDR);
    console.output_txt(COORD_TBL_TOP, MSG_TBL_SEP);
    console.output_txt(COORD_TBL_HDR, MSG_TBL_HDR);
    console.output_txt(COORD_TBL_SEP, MSG_TBL_SEP);

    for (uint32_t i = 0u; i < MAX_RESULT_ROWS; ++i)
    {
        const areg::ext::Console::Coord coord
        {
              COORD_TBL_ROW0.posX
            , static_cast<int16_t>(COORD_TBL_ROW0.posY + static_cast<int16_t>(i))
        };

        const ResultEntry & r = mResults[i];
        if (r.valid)
        {
            console.output_msg(coord, MSG_TBL_ROW.data()
                             , r.run_no, r.mode.as_string(), r.count
                             , r.min_us, r.p50_us, r.p95_us, r.p99_us, r.max_us, r.mean_us);
        }
        else
        {
            console.output_txt(coord, MSG_TBL_EMPTY);
        }
    }

    console.output_txt(COORD_TBL_BOT, MSG_TBL_SEP);
    console.restore_cursor_position();
    console.refresh_screen();
}

areg::String LatencyConsumer::_default_csv_path() const
{
    const areg::String ts = areg::DateTime::now().format_time("%Y%m%d_%H%M%S");
    areg::String name;
    name.format("latency_%s_%s.csv", Latency::mode_as_str(mMode), ts.as_string());
    return areg::File::current_dir() + areg::File::PATH_SEPARATOR + name;
}

void LatencyConsumer::_save_csv(const ResultEntry & result) const
{
    const uint32_t N = mSamples.size();
    if (N == 0u)
        return;

    const areg::String output_path = mCsvPath.is_empty() ? _default_csv_path() : mCsvPath;

    const uint32_t mode = static_cast<uint32_t>(areg::FileBase::OpenMode::Write)
                        | static_cast<uint32_t>(areg::FileBase::OpenMode::Text)
                        | static_cast<uint32_t>(areg::FileBase::OpenMode::Create);

    areg::File csv;
    if (!csv.open(output_path, mode))
    {
        areg::ext::Console & console = areg::ext::Console::instance();
        console.save_cursor_position();
        console.output_msg(COORD_SETTINGS, " WARNING: could not write CSV to '%s'%-20s", output_path.as_string(), "");
        console.restore_cursor_position();
        console.refresh_screen();
        return;
    }

    // Metadata comment block — lets the user see run parameters and summary in the file header.
    areg::String line;
    line.format("# Run #%u: mode=%s, count=%u, warmup=%u, dur=%us"
              , result.run_no, result.mode.as_string(), result.count, mWarmup, mDurationSec);
    csv.write_line(line);

    line.format("# Min: %.2f us, P50: %.2f us, P95: %.2f us, P99: %.2f us, Max: %.2f us, Mean: %.2f us"
              , result.min_us, result.p50_us, result.p95_us, result.p99_us, result.max_us, result.mean_us);
    csv.write_line(line);

    csv.write_line("seq,mode,t1_ns,t2_ns,t4_ns,rtt_ns,rtt_us");

    for (uint32_t i = 0u; i < N; ++i)
    {
        const LatencySample & s = mSamples.value_at(i);
        line.format("%u,%s,%llu,%llu,%llu,%lld,%.3f"
                  , s.seq
                  , result.mode.as_string()
                  , static_cast<unsigned long long>(s.t1_ns)
                  , static_cast<unsigned long long>(s.t2_ns)
                  , static_cast<unsigned long long>(s.t4_ns)
                  , static_cast<long long>(s.rtt_ns)
                  , static_cast<double>(s.rtt_ns) / 1000.0);
        csv.write_line(line);
    }

    csv.close();

    areg::ext::Console & console = areg::ext::Console::instance();
    console.save_cursor_position();
    console.output_msg(COORD_SETTINGS, " Saved: %s%-40s", output_path.as_string(), "");
    console.restore_cursor_position();
    console.refresh_screen();
}

void LatencyConsumer::_run_display_thread()
{
    while (!mQuit.load(std::memory_order_relaxed))
    {
        areg::Thread::sleep(1000u);
        if (mQuit.load(std::memory_order_relaxed))
            break;

        _update_live();
        _update_settings();
    }
}
