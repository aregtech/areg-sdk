/************************************************************************
 * \file        pubconsumer/src/LatencyConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service consumer component.
 ************************************************************************/

#ifndef EXAMPLES_30_PUBLATENCY_PUBCONSUMER_SRC_LATENCYCONSUMER_HPP
#define EXAMPLES_30_PUBLATENCY_PUBCONSUMER_SRC_LATENCYCONSUMER_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/ArrayList.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/console/OptionParser.hpp"
#include "examples/30_publatency/services/LatencyConsumerBase.hpp"

#include <atomic>
#include <chrono>
#include <climits>

//////////////////////////////////////////////////////////////////////////
// LatencySample -- one measured round-trip record
//////////////////////////////////////////////////////////////////////////

struct LatencySample
{
    uint32_t    seq     { 0u };     //!< 1-based sequence number of measured sample
    uint64_t    t1_ns   { 0u };     //!< Client send timestamp (ns)
    uint64_t    t2_ns   { 0u };     //!< Service echo timestamp (ns); 0 for broadcast
    uint64_t    t4_ns   { 0u };     //!< Client receive timestamp (ns)
    int64_t     rtt_ns  { 0  };     //!< Round-trip time = t4 - t1 (ns)
};

//////////////////////////////////////////////////////////////////////////
// ResultEntry -- summary statistics for one completed test run
//////////////////////////////////////////////////////////////////////////

struct ResultEntry
{
    uint32_t        run_no  { 0u };
    areg::String    mode    {    };
    uint32_t        count   { 0u };
    double          min_us  { 0.0 };
    double          p50_us  { 0.0 };
    double          p95_us  { 0.0 };
    double          p99_us  { 0.0 };
    double          max_us  { 0.0 };
    double          mean_us { 0.0 };
    double          dur_ms  { 0.0 };   //!< Actual measured wall-clock duration of the run (ms)
    double          mps     { 0.0 };   //!< Throughput: measured messages per second
    bool            valid   { false };

    ResultEntry() = default;
    ResultEntry(const ResultEntry &) = default;
    ResultEntry & operator=(const ResultEntry &) = default;
};

//////////////////////////////////////////////////////////////////////////
// CmdData -- carries a parsed console command to the component thread
//////////////////////////////////////////////////////////////////////////

struct CmdData
{
    bool                    quit        { false };
    bool                    quit_local  { false };
    bool                    help        { false };
    bool                    info        { false };
    bool                    start       { false };
    bool                    stop        { false };
    bool                    set_mode    { false };
    bool                    set_count   { false };
    bool                    set_warmup  { false };
    bool                    set_duration{ false };
    bool                    set_output  { false };
    bool                    enable_file { false };  //!< -f: enable CSV output (optionally with path)
    bool                    error       { false };
    Latency::LatencyMode    mode        { Latency::LatencyMode::Request0 };
    uint32_t                count       { 0u };
    uint32_t                warmup      { 0u };
    uint32_t                duration_ms { 0u };  //!< -d value, milliseconds (run duration)
    areg::String            csv_path    {    };  //!< path from -o=<path>
    areg::String            csv_file    {    };  //!< path from -f=<path> (empty = auto)

    CmdData() = default;
    CmdData(const CmdData &) = default;
    CmdData & operator=(const CmdData &) = default;
};

AREG_DECLARE_EVENT(CmdData, EventCommand, IECmdConsumer);

//////////////////////////////////////////////////////////////////////////
// LatencyConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service consumer component for the latency benchmark.
 *          Drives ping-pong and broadcast tests, collects samples, shows
 *          live stats, and maintains a circular results table (last 8 runs).
 *          Commands are parsed by OptionParser (both short -m= and long mode= forms).
 **/
class LatencyConsumer final : public    areg::Component
                            , protected LatencyConsumerBase
                            , protected areg::TimerConsumer
                            , protected areg::ThreadConsumer
{
    friend class areg::Component;
    friend class CmdConsumer;
    friend class DisplayConsumer;

//////////////////////////////////////////////////////////////////////////
// Nested command consumer -- forwards events to the component thread
//////////////////////////////////////////////////////////////////////////
    class CmdConsumer : public IECmdConsumer
    {
    public:
        explicit CmdConsumer(LatencyConsumer & owner)
            : IECmdConsumer ( )
            , mOwner        ( owner )
        { }

        virtual ~CmdConsumer() = default;

    private:
        void process_event(const CmdData & data) final;

    private:
        LatencyConsumer & mOwner;
        AREG_NOCOPY_NOMOVE(CmdConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Nested display consumer -- drives the 1-second console refresh thread
//////////////////////////////////////////////////////////////////////////
    class DisplayConsumer : public areg::ThreadConsumer
    {
    public:
        explicit DisplayConsumer(LatencyConsumer & owner)
            : areg::ThreadConsumer( )
            , mOwner              ( owner )
        { }

        virtual ~DisplayConsumer() = default;

    private:
        void on_run() final;

    private:
        LatencyConsumer & mOwner;
        AREG_NOCOPY_NOMOVE(DisplayConsumer);
    };

    enum class ConsumerCmd : int32_t
    {
          Quit     = 1
        , Help     = 2
        , Info     = 3
        , Start    = 4
        , Stop     = 5
        , Mode     = 6
        , Count    = 7
        , Warmup   = 8
        , Duration = 9
        , Output   = 10
        , File     = 11
    };

    //!< Selects how a measured run is bounded and (optionally) paced, chosen from the bounds given
    //!< on the start command line: `-c` only, `-d` only, or both `-c` and `-d` together.
    enum class StopMode : uint8_t
    {
          ByCount       //!< `-c` only: stop after mCount measured samples; messages sent at max rate.
        , ByDuration    //!< `-d` only: stop after mDurationNs of measured wall-clock; messages at max rate.
        , Paced         //!< `-c` and `-d`: send exactly mCount messages spread across mDurationNs
                        //!< (target rate = mCount / mDurationNs). Stops by count; rate is best-effort.
    };

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr uint32_t MAX_RESULT_ROWS { 8u };

    static constexpr std::string_view THREAD_INPUT   { "LatencyConsumerInputThread" };
    static constexpr std::string_view THREAD_DISPLAY { "LatencyConsumerDisplayThread" };
    static constexpr std::string_view TIMER_PACE     { "LatencyConsumerPaceTimer" };
    static constexpr uint32_t DEFAULT_COUNT         { 1000u };
    static constexpr uint32_t DEFAULT_WARMUP        { 10u };
    static constexpr uint64_t DEFAULT_DURATION_NS   { 0u };
    //!< Pre-reservation estimate for duration runs: a deliberate over-estimate (~2x the fastest
    //!< realistic 1-in-flight rate) so the measurement loop never reallocates mid-run.
    static constexpr uint32_t DURATION_RESERVE_PER_SEC { 200000u };
    //!< Upper bound on pre-reserved samples, to keep memory bounded for long duration runs.
    static constexpr uint32_t MAX_SAMPLE_RESERVE    { 4000000u };

//////////////////////////////////////////////////////////////////////////
// Console layout coordinates  (col=1, rows 2-32)
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr areg::ext::Console::Coord COORD_TITLE    { 1,  2 };
    static constexpr areg::ext::Console::Coord COORD_SEP1     { 1,  3 };
    static constexpr areg::ext::Console::Coord COORD_LIVE_1   { 1,  4 };
    static constexpr areg::ext::Console::Coord COORD_LIVE_2   { 1,  5 };
    static constexpr areg::ext::Console::Coord COORD_SEP2     { 1,  6 };
    static constexpr areg::ext::Console::Coord COORD_PROMPT   { 1,  7 };
    static constexpr areg::ext::Console::Coord COORD_INPUT    { 1,  8 };
    static constexpr areg::ext::Console::Coord COORD_SEP3     { 1,  9 };
    static constexpr areg::ext::Console::Coord COORD_EX_HDR   { 1, 10 };
    static constexpr areg::ext::Console::Coord COORD_EX1      { 1, 11 };
    static constexpr areg::ext::Console::Coord COORD_EX2      { 1, 12 };
    static constexpr areg::ext::Console::Coord COORD_EX3      { 1, 13 };
    static constexpr areg::ext::Console::Coord COORD_EX4      { 1, 14 };
    static constexpr areg::ext::Console::Coord COORD_EX5      { 1, 15 };
    static constexpr areg::ext::Console::Coord COORD_EX6      { 1, 16 };
    static constexpr areg::ext::Console::Coord COORD_EX7      { 1, 17 };
    static constexpr areg::ext::Console::Coord COORD_EX8      { 1, 18 };
    static constexpr areg::ext::Console::Coord COORD_SEP4     { 1, 19 };
    static constexpr areg::ext::Console::Coord COORD_SETTINGS { 1, 20 };
    static constexpr areg::ext::Console::Coord COORD_SEP5     { 1, 21 };
    static constexpr areg::ext::Console::Coord COORD_RES_HDR  { 1, 22 };
    static constexpr areg::ext::Console::Coord COORD_TBL_TOP  { 1, 23 };
    static constexpr areg::ext::Console::Coord COORD_TBL_HDR  { 1, 24 };
    static constexpr areg::ext::Console::Coord COORD_TBL_SEP  { 1, 25 };
    static constexpr areg::ext::Console::Coord COORD_TBL_ROW0 { 1, 26 };  //!< First data row
    static constexpr areg::ext::Console::Coord COORD_TBL_BOT  { 1, 34 };  //!< 26 + MAX_RESULT_ROWS

//////////////////////////////////////////////////////////////////////////
// Console message strings
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr std::string_view MSG_TITLE     { " Latency Benchmark -- Service Consumer (example #30)" };
    static constexpr std::string_view MSG_SEPARATOR { " --------------------------------------------------------------------------------------------------------------" };
    static constexpr std::string_view MSG_LIVE_1    { " Mode: %-6s  Status: %-8s  Samples: %-6u  Rate: %8.1f msg/sec" };
    static constexpr std::string_view MSG_LIVE_2    { " Min: %9.3f us   Mean: %9.3f us   Max: %9.3f us" };
    static constexpr std::string_view MSG_LIVE_FINAL{ " Min:%9.3f  P50:%9.3f  P95:%9.3f  P99:%9.3f  Max:%9.3f  Mean:%9.3f  (us)" };
    static constexpr std::string_view MSG_PROMPT    { " Commands: start | stop | -q | -q=1 | -h | -m=<mode> -c=<n> -w=<n> -d=<ms> -f[=<path>] -o=<path>" };
    static constexpr std::string_view MSG_EX_HDR    { " Examples (short or long form, combinable on one line):" };
    static constexpr std::string_view MSG_EX1       { "   -s (or `start`)" };
    static constexpr std::string_view MSG_EX2       { "   -s -m=pp0 -c=1000 -w=50           (or `start mode=pp0 count=1000 warmup=50`)" };
    static constexpr std::string_view MSG_EX3       { "   -s -m=pp64 -d=2000 -w=500         (run pp64 for 2000 ms at max rate; report count and msg/s)" };
    static constexpr std::string_view MSG_EX4       { "   -s -m=pp64 -c=2000 -d=2000        (paced: send 2000 msgs spread over 2000 ms = ~1000 msg/s)" };
    static constexpr std::string_view MSG_EX5       { "   -s -m=bc8 -c=1000 -f=results.csv  (or `start mode=bc8 count=1000 file=results.csv`)" };
    static constexpr std::string_view MSG_EX6       { " PP Modes: pp0 | pp8 | pp16 | pp32 | pp64 | pp128 | pp512 | pp1024 | pp4096 | pp65536" };
    static constexpr std::string_view MSG_EX7       { " BC Modes: bc0 | bc8 | bc16 | bc32 | bc64 | bc128 | bc512 | bc1024 | bc4096 | bc65536" };
    static constexpr std::string_view MSG_EX8       { " Other Opt: -p (`stop`) | -i (`info`) | -h (`help`) | -q (global `quit`) | -q=1 (local `quit`)" };
    static constexpr std::string_view MSG_SETTINGS  { " mode=%-6s  stop=%-5s  count=%-6u  dur=%-6u ms  warmup=%-3u  connected=%-3s  running=%-3s  csv: %s" };
    static constexpr std::string_view MSG_RES_HDR   { " Test Results (last 8 runs):" };
    // Table strings -- all 115 chars wide (1 leading space + 114 table chars)
    static constexpr std::string_view MSG_TBL_SEP   { " +-----+---------+--------+----------+----------+----------+----------+----------+----------+----------+----------+" };
    static constexpr std::string_view MSG_TBL_HDR   { " |  #  |    Mode |  Count |  Min(us) |  P50(us) |  P95(us) |  P99(us) |  Max(us) | Mean(us) |  Dur(ms) |    Msg/s |" };
    static constexpr std::string_view MSG_TBL_EMPTY { " |     |         |        |          |          |          |          |          |          |          |          |" };
    static constexpr std::string_view MSG_TBL_ROW   { " | %3u | %-7s | %6u | %8.2f | %8.2f | %8.2f | %8.2f | %8.2f | %8.2f | %8.0f | %8.0f |" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LatencyConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    ~LatencyConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Component lifecycle overrides
//////////////////////////////////////////////////////////////////////////
protected:
    void startup_component(areg::ComponentThread & comThread) final;
    void shutdown_component(areg::ComponentThread & comThread) noexcept final;

//////////////////////////////////////////////////////////////////////////
// LatencyConsumerBase overrides
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Triggered when the service provider connects or disconnects.
     *          When the service provider connects, set listeners here.
     *          When the service provider disconnects, clean the listeners.
     *          Once connected, the consumer can subscribe and unsubscribe on updates,
     *          responses and broadcasts, and trigger requests. Before connection,
     *          the consumer cannot trigger requests or receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object reporting the connection.
     * \return  Returns true if this service connection notification was relevant to this consumer.
     **/
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final;

    /**
     * \brief   Response callback.
     *          Response of latency benchmark test request
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \param   mode    The latency test mode that was setup
     * \see     request_start_mode
     **/
    void response_start_mode(const Latency::LantencySetup& mode) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and without extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_0
     **/
    void response_ping_pong_0(uint32_t id, uint64_t start, uint64_t replied) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 8 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_8
     **/
    void response_ping_pong_8(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data8& data8) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 16 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_16
     **/
    void response_ping_pong_16(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data16& data16) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 32 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_32
     **/
    void response_ping_pong_32(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data32& data32) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 64 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_64
     **/
    void response_ping_pong_64(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data64& data64) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 128 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_128
     **/
    void response_ping_pong_128(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data128& data128) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 256 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_256
     **/
    void response_ping_pong_256(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data256& data256) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 512 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_512
     **/
    void response_ping_pong_512(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data512& data512) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 1024 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_1024
     **/
    void response_ping_pong_1024(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data1024& data1024) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 4096 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_4096
     **/
    void response_ping_pong_4096(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data4096& data4096) final;

    /**
     * \brief   Response callback.
     *          Response with id and timestamp, and with 65536 Bytes extra data
     *          Override this method to handle the Response call from the service provider.
     *          This call is automatically triggered on every appropriate request call.
     * \see     request_ping_pong_65536
     **/
    void response_ping_pong_65536(uint32_t id, uint64_t begin, uint64_t replied, const Latency::Data65536& data65536) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp without extra data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_0(uint32_t id, uint64_t begin) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 8 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_8(uint32_t id, uint64_t begin, const Latency::Data8 & data8) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 16 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_16(uint32_t id, uint64_t begin, const Latency::Data16 & data16) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 32 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_32(uint32_t id, uint64_t begin, const Latency::Data32 & data32) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 64 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_64(uint32_t id, uint64_t begin, const Latency::Data64 & data64) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 128 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_128(uint32_t id, uint64_t begin, const Latency::Data128 & data128) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 256 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_256(uint32_t id, uint64_t begin, const Latency::Data256 & data256) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 512 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_512(uint32_t id, uint64_t begin, const Latency::Data512 & data512) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 1024 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_1024(uint32_t id, uint64_t begin, const Latency::Data1024 & data1024) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 4096 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_4096(uint32_t id, uint64_t begin, const Latency::Data4096 & data4096) final;

    /**
     * \brief   Service provider broadcast.
     *          Broadcast message with id and timestamp with extra 655336 Bytes data
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     **/
    void broadcast_message_65536(uint32_t id, uint64_t begin, const Latency::Data65536 & data65536) final;

    /**
     * \brief   Triggered when the latency_setup attribute is updated. Contains the
     *          attribute value and validation flag. When notification is enabled,
     *          override this method in the derived class.
     *          Attribute latency_setup description:
     *          Current latency benchmark setup
     * \param   latency_setup   The value of the latency_setup attribute.
     * \param   state           The data validation flag.
     **/
    void on_latency_setup_update(const Latency::LantencySetup & latency_setup, areg::DataState state) final;

//////////////////////////////////////////////////////////////////////////
// TimerConsumer override
//////////////////////////////////////////////////////////////////////////
protected:
    void process_timer(areg::Timer & timer) final;

//////////////////////////////////////////////////////////////////////////
// ThreadConsumer override
//////////////////////////////////////////////////////////////////////////
protected:
    void on_run() final;

//////////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////////
private:
    void _redraw_layout();
    void _update_live();
    void _update_settings() const;
    void _run_input_thread();
    void _run_display_thread();
    void _on_cmd_event(const CmdData & data);
    void _start_test();
    void _stop_test();
    void _send_next_ping();
    void _send_next_oneway();   //!< Broadcast/one-way: pull the next message from the provider.
    void _send_next_paced();    //!< Dispatch the next ping/one-way send (used by the pacing timer).
    void _record_sample(uint64_t t1, uint64_t t2, uint64_t t4);

    /**
     * \brief   Computes final stats, updates the live display with the final summary,
     *          stores a ResultEntry in the circular buffer, draws the table row, and
     *          saves the CSV.  Called when a test completes (count reached or stopped).
     **/
    void _finish_test();

    void _draw_result_row(uint32_t slot) const;
    void _redraw_results_table() const;

    /**
     * \brief   Writes all collected samples to CSV, preceded by # metadata lines
     *          with run parameters and final summary statistics.
     **/
    void _save_csv(const ResultEntry & result) const;

    [[nodiscard]]
    areg::String _default_csv_path() const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::Thread            mInputThread;   //!< Console input thread
    std::atomic_bool        mQuit;          //!< Set to true to stop input and display threads
    CmdConsumer             mCmdConsumer;   //!< Receives EventCommand on component thread
    DisplayConsumer         mDisplayConsumer;   //!< Must be declared before mDisplayThread
    areg::Thread            mDisplayThread;     //!< 1-second console refresh thread

    Latency::LatencyMode    mMode;
    uint32_t                mCount;
    uint32_t                mWarmup;
    uint64_t                mDurationNs;    //!< -d run duration in nanoseconds (0 = unset)
    StopMode                mStopMode;      //!< Active run regime: count-, duration-bounded, or paced
    uint64_t                mPacingIntervalNs;  //!< Paced mode: target ns between measured sends (mDurationNs/mCount)
    bool                    mCsvEnabled;    //!< true only when -f was given; gates all CSV writes
    areg::String            mCsvPath;

    uint32_t                mCurrentSeq;    //!< Total arrivals since test start (warmup + measured)
    uint64_t                mMeasureStartNs;//!< Send timestamp (t1) of the first measured sample; window origin
    bool                    mTestRunning;
    uint32_t                mTotalRuns;     //!< Completed test runs since startup

    areg::ArrayList<LatencySample>  mSamples;
    std::atomic<uint32_t>   mSampleCount;   //!< Atomic count of mSamples; safe to read from display thread

    int64_t                 mRunMin;
    int64_t                 mRunMax;
    int64_t                 mRunSum;
    uint32_t                mLastRateCount;

    areg::Timer             mPaceTimer;     //!< One-shot timer gating the next paced send (no CPU spin)

    ResultEntry             mResults[MAX_RESULT_ROWS];  //!< Circular results buffer

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LatencyConsumer);
};

//////////////////////////////////////////////////////////////////////////
// LatencyConsumer inline methods
//////////////////////////////////////////////////////////////////////////

#endif  // EXAMPLES_30_PUBLATENCY_PUBCONSUMER_SRC_LATENCYCONSUMER_HPP
