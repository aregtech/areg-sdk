/************************************************************************
 * \file        pubprovider/src/LatencyProvider.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Latency benchmark service provider component.
 ************************************************************************/

#ifndef EXAMPLES_30_PUBLATENCY_PUBPROVIDER_SRC_LATENCYPROVIDER_HPP
#define EXAMPLES_30_PUBLATENCY_PUBPROVIDER_SRC_LATENCYPROVIDER_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "aregextend/console/Console.hpp"
#include "examples/30_publatency/services/LatencyProviderBase.hpp"

#include <algorithm>
#include <atomic>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// ProviderCmdData — carries a console command from input thread to component thread
//////////////////////////////////////////////////////////////////////////

struct ProviderCmdData
{
    static constexpr uint32_t F_QUIT  { 1u << 0 };
    static constexpr uint32_t F_HELP  { 1u << 1 };
    static constexpr uint32_t F_INFO  { 1u << 2 };
    static constexpr uint32_t F_ERROR { 1u << 3 };

    uint32_t flags { 0u };

    inline bool has_quit()  const noexcept { return (flags & F_QUIT)  != 0u; }
    inline bool has_help()  const noexcept { return (flags & F_HELP)  != 0u; }
    inline bool has_info()  const noexcept { return (flags & F_INFO)  != 0u; }
    inline bool has_error() const noexcept { return (flags & F_ERROR) != 0u; }
};

AREG_DECLARE_EVENT(ProviderCmdData, ProviderCmdEvent, ProviderCmdConsumerBase);

//////////////////////////////////////////////////////////////////////////
// LatencyProvider class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service provider component for the latency benchmark.
 *          Handles ping-pong requests by echoing timestamps and drives
 *          broadcast loops on a dedicated worker thread.
 **/
class LatencyProvider final : public    areg::Component
                            , protected LatencyProviderBase
                            , protected areg::TimerConsumer
                            , protected areg::ThreadConsumer
{
    friend class areg::Component;
    friend class DisplayConsumer;

//////////////////////////////////////////////////////////////////////////
// Nested command consumer — delivers ProviderCmdData on the component thread
//////////////////////////////////////////////////////////////////////////
    class ProviderCmdConsumer : public ProviderCmdConsumerBase
    {
    public:
        explicit ProviderCmdConsumer(LatencyProvider & owner)
            : ProviderCmdConsumerBase( )
            , mOwner                 ( owner )
        {
        }

        virtual ~ProviderCmdConsumer() = default;

    private:
        void process_event(const ProviderCmdData & data) final;

    private:
        LatencyProvider & mOwner;
        AREG_NOCOPY_NOMOVE(ProviderCmdConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Nested display consumer — drives the 1-second console refresh thread
//////////////////////////////////////////////////////////////////////////
    class DisplayConsumer : public areg::ThreadConsumer
    {
    public:
        explicit DisplayConsumer(LatencyProvider & owner)
            : areg::ThreadConsumer( )
            , mOwner              ( owner )
        {
        }

        virtual ~DisplayConsumer() = default;

    private:
        void on_run() final;

    private:
        LatencyProvider & mOwner;
        AREG_NOCOPY_NOMOVE(DisplayConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Console layout coordinates
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr areg::ext::Console::Coord COORD_TITLE   { 1, 2 };
    static constexpr areg::ext::Console::Coord COORD_SEP1    { 1, 3 };
    //!< Live stats row: mode, clients, messages served, rate
    static constexpr areg::ext::Console::Coord COORD_LIVE    { 1, 4 };
    //!< One-way request latency stats row (request mode only, updated every second)
    static constexpr areg::ext::Console::Coord COORD_LATENCY { 1, 5 };
    static constexpr areg::ext::Console::Coord COORD_SEP2    { 1, 6 };
    static constexpr areg::ext::Console::Coord COORD_PROMPT  { 1, 7 };
    //!< Input row — " > " drawn here; cursor rests here
    static constexpr areg::ext::Console::Coord COORD_INPUT   { 1, 8 };
    static constexpr areg::ext::Console::Coord COORD_SEP3    { 1, 9 };

//////////////////////////////////////////////////////////////////////////
// Console message strings
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr std::string_view MSG_TITLE        { " Latency Benchmark -- Service Provider (example #30)" };
    static constexpr std::string_view MSG_SEPARATOR    { " -----------------------------------------------------------------------------------------------------------------------" };
    static constexpr std::string_view MSG_LIVE         { " Mode: %-8s  Status: %-8s  Samples: %-8u  Rate: %8.1f msg/sec" };
    static constexpr std::string_view MSG_LATENCY      { " One-way> Min:%8.3f  P50:%8.3f  P95:%8.3f  P99:%8.3f  Max:%8.3f  Mean:%8.3f  (us)" };
    static constexpr std::string_view MSG_LATENCY_NONE { " One-way> Min:     ---  P50:     ---  P95:     ---  P99:     ---  Max:     ---  Mean:     ---  (us)" };
    static constexpr std::string_view MSG_PROMPT       { " Type '-q' to quit, '-h' for help, '-i' for info:" };

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    static constexpr std::string_view THREAD_DISPLAY    { "LatencyProviderDisplayThread" };
    static constexpr std::string_view THREAD_INPUT       { "LatencyProviderInputThread" };
    static constexpr std::string_view TIMER_BROADCAST    { "LatencyProviderBroadcastTimer" };

    //!< Last computed one-way stats; shown persistently between timer ticks in request mode:
    struct LatencyStats
    {
        double   min_us  { 0.0 };
        double   mean_us { 0.0 };
        double   p50_us  { 0.0 };
        double   p95_us  { 0.0 };
        double   p99_us  { 0.0 };
        double   max_us  { 0.0 };
        bool     valid   { false };
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LatencyProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner);
    ~LatencyProvider() = default;

//////////////////////////////////////////////////////////////////////////
// LatencyProviderBase overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Request call.
     *          Call to start or stop the latency benchmark test depending on the mode flag
     * \param   mode        Latency benchmark test mode to start or prepare.
     * \param   duration    Duration of the test in seconds
     * \param   warmup      The number of elements to warmup. Value 0 means no warm up.
     * \param   count       Broadcast: number of messages per second. Request-response: number of requests to expect, plus warmup value.
     * \see     response_start_mode
     **/
    void request_start_mode(Latency::LatencyMode mode, uint32_t duration, uint32_t warmup, uint32_t count) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and without extra data
     * \see     response_ping_pong_0
     **/
    void request_ping_pong_0(uint32_t id, uint64_t start) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 8 Bytes extra data
     * \see     response_ping_pong_8
     **/
    void request_ping_pong_8(uint32_t id, uint64_t begin, const Latency::Data8& data8) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 16 Bytes extra data
     * \see     response_ping_pong_16
     **/
    void request_ping_pong_16(uint32_t id, uint64_t begin, const Latency::Data16& data16) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 32 Bytes extra data
     * \see     response_ping_pong_32
     **/
    void request_ping_pong_32(uint32_t id, uint64_t begin, const Latency::Data32& data32) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 64 Bytes extra data
     * \see     response_ping_pong_64
     **/
    void request_ping_pong_64(uint32_t id, uint64_t begin, const Latency::Data64& data64) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 128 Bytes extra data
     * \see     response_ping_pong_128
     **/
    void request_ping_pong_128(uint32_t id, uint64_t begin, const Latency::Data128& data128) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 256 Bytes extra data
     * \see     response_ping_pong_256
     **/
    void request_ping_pong_256(uint32_t id, uint64_t begin, const Latency::Data256& data256) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 512 Bytes extra data
     * \see     response_ping_pong_512
     **/
    void request_ping_pong_512(uint32_t id, uint64_t begin, const Latency::Data512& data512) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 1024 Bytes extra data
     * \see     response_ping_pong_1024
     **/
    void request_ping_pong_1024(uint32_t id, uint64_t begin, const Latency::Data1024& data1024) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 4096 Bytes extra data
     * \see     response_ping_pong_4096
     **/
    void request_ping_pong_4096(uint32_t id, uint64_t begin, const Latency::Data4096& data4096) final;

    /**
     * \brief   Request call.
     *          Request with id and timestamp, and with 65536 Bytes extra data
     * \see     response_ping_pong_65536
     **/
    void request_ping_pong_65536(uint32_t id, uint64_t begin, const Latency::Data65536& data65536) final;

protected:
/************************************************************************/
// StubBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Stub,
     *                  which started up.
     **/
    void startup_service_interface(areg::Component & holder) final;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    void shutdown_service_interface(areg::Component & holder) noexcept final;

    /**
     * \brief   Called when a proxy client connects to or disconnects from this stub.
     *
     * \param   client      The address of the connecting or disconnecting client.
     * \param   status      The new connection status.
     * \return  Returns true if the client is relevant to this service; false otherwise.
     **/
    bool consumer_connected( const areg::ProxyAddress & client, areg::ServiceConnectionState status ) final;

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
    void _run_broadcast_burst();
    void _run_input_thread();
    void _run_display_thread();
    void _on_provider_cmd(const ProviderCmdData & data);
    void _redraw_layout();
    void _update_live();
    void _record_oneway(uint64_t begin_ns) noexcept;
    void _update_latency_stats();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::Thread                        mInputThread;
    areg::Timer                         mBroadcastTimer;//!< 50ms broadcast burst timer
    std::atomic_bool                    mQuit;
    std::atomic_bool                    mQuitInput;
    std::atomic<uint32_t>               mDurationSec;
    std::atomic<Latency::LatencyMode>   mBroadcastMode;
    ProviderCmdConsumer                 mProviderCmdConsumer;
    DisplayConsumer                     mDisplayConsumer;   //!< Must be declared before mDisplayThread
    areg::Thread                        mDisplayThread;     //!< 1-second console refresh thread

    //!< These are accessed only on the component thread (timer + consumer_connected):
    uint32_t                            mClientCount;           //!< Number of connected consumers
    uint32_t                            mLastRateCount;         //!< Served count at last stats tick
    uint32_t                            mBroadcastCyclesLeft;   //!< Remaining 50ms broadcast ticks (0xFFFFFFFFu = run indefinitely)
    uint32_t                            mMsgId;                 //!< Running broadcast message sequence number
    uint32_t                            mBroadcastBulk;         //!< Messages sent per 50ms timer tick

    //!< Incremented from both the component thread (ping-pong) and broadcast thread:
    std::atomic<uint32_t>               mTotalServed;

    LatencyStats                        mLastStats;

    //!< Cumulative one-way running stats for the current test (request mode only):
    std::vector<int64_t>                mOneWayLatencies;   //!< All one-way samples (ns); cleared on new test start
    int64_t                             mRunMin;    //!< Running minimum one-way (ns); INT64_MAX = no samples
    int64_t                             mRunMax;    //!< Running maximum one-way (ns)
    int64_t                             mRunSum;    //!< Running sum for mean computation (ns)
    uint32_t                            mRunCount;  //!< Number of one-way samples accumulated
    double                              mLastRate;  //!< Last non-zero msg/sec rate, shown when test is idle

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LatencyProvider);
};

//////////////////////////////////////////////////////////////////////////
// LatencyProvider inline methods
//////////////////////////////////////////////////////////////////////////

#endif  // EXAMPLES_30_PUBLATENCY_PUBPROVIDER_SRC_LATENCYPROVIDER_HPP
