/************************************************************************
 * \file        src/LocalLatencyConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Service consumer of the local latency benchmark (example 31).
 ************************************************************************/

#ifndef EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYCONSUMER_HPP
#define EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYCONSUMER_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "examples/31_loclatency/services/LocalLatencyConsumerBase.hpp"
#include "src/LatencyOptions.hpp"
#include "src/LatencyReport.hpp"

#include <atomic>
#include <cstdint>
#include <vector>

/**
 * \brief   The driver of the local latency benchmark.
 *
 *          The consumer owns the whole session. It walks through the list of modes given
 *          on the command line, measures each of them, prints one result line per run and
 *          asks the application to quit when the list is finished. Nobody has to type
 *          anything: the program can be started from a script and will always end.
 *
 *          ONE MESSAGE AT A TIME
 *          The consumer never has more than one message on the way. The next message is
 *          sent only after the previous one has arrived. This is what makes the result a
 *          latency and not a throughput number: no queue can build up and hide the real
 *          cost of a single message.
 *
 *          HOW A SAMPLE IS MEASURED
 *          - Round trip (pp*): the consumer takes a timestamp, sends the request, and
 *            takes a second timestamp when the answer arrives. The difference is the
 *            sample. The provider's timestamp comes back inside the answer, so the run
 *            also reports how much of the round trip was the way there.
 *          - One way (bc*): the consumer asks the provider for one message. The provider
 *            takes the timestamp just before it sends, the consumer takes its own when
 *            the message arrives. The difference is the sample. The asking call itself
 *            happens before the first timestamp and is therefore not part of the sample.
 *
 *          WARM-UP
 *          The first `--warmup` messages of every run are sent and received exactly like
 *          the measured ones, but their times are thrown away. They bring the code, the
 *          data and the memory allocator into the state a running system is in.
 *
 *          STALL WATCHDOG
 *          A small helper thread watches a counter that grows with every message. If the
 *          counter stops growing, the benchmark is stuck and would otherwise wait for
 *          ever. The watchdog then prints an explanation and ends the program with a
 *          non-zero exit code, so an unattended script notices the problem. The watchdog
 *          only reads one atomic counter and never touches the measured path.
 **/
class LocalLatencyConsumer final : public    areg::Component
                                 , protected LocalLatencyConsumerBase
{
    friend class areg::Component;

//////////////////////////////////////////////////////////////////////////
// Nested stall watchdog
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Runs on its own thread and ends the program when the benchmark stops
     *          making progress. See the "stall watchdog" note of the owner class.
     **/
    class WatchdogConsumer : public areg::ThreadConsumer
    {
    public:
        explicit WatchdogConsumer(LocalLatencyConsumer & owner)
            : areg::ThreadConsumer  ( )
            , mOwner                ( owner )
        {
        }

        virtual ~WatchdogConsumer() = default;

    private:
        void on_run() final;

    private:
        LocalLatencyConsumer & mOwner;
        AREG_NOCOPY_NOMOVE(WatchdogConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< Name of the watchdog thread.
    static constexpr std::string_view   THREAD_WATCHDOG     { "LocalLatencyWatchdogThread" };
    //!< How often the watchdog looks at the progress counter, in milliseconds.
    static constexpr uint32_t           WATCHDOG_TICK_MS    { 1000u };
    //!< How many ticks without any progress mean that the benchmark is stuck.
    static constexpr uint32_t           WATCHDOG_STALL_TICKS{ 20u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by the framework when the component thread starts.
     *
     * \param   entry   The registry entry that describes this component.
     * \param   owner   The component thread that owns this component.
     **/
    LocalLatencyConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

    ~LocalLatencyConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Tells whether the session ended because the benchmark got stuck.
     *          The program uses it to choose its exit code.
     **/
    [[nodiscard]]
    static bool has_stalled() noexcept;

//////////////////////////////////////////////////////////////////////////
// Component lifecycle overrides
//////////////////////////////////////////////////////////////////////////
protected:
    void startup_component(areg::ComponentThread & comThread) final;
    void shutdown_component(areg::ComponentThread & comThread) noexcept final;

//////////////////////////////////////////////////////////////////////////
// LocalLatencyConsumerBase overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Called when the provider becomes reachable or unreachable.
     *          The session starts as soon as the provider is there, and the application
     *          quits if the provider disappears while a run is going on.
     *
     * \param   status  The new connection state.
     * \param   proxy   The proxy that reports the change.
     * \return  Returns true when the notification belonged to this consumer.
     **/
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final;

    /**
     * \brief   Answer of a round trip request without extra data.
     *
     * \param   id          The sequence number this consumer had sent.
     * \param   begin       The timestamp this consumer had taken before sending.
     * \param   replied     The timestamp the provider took when the request arrived.
     **/
    void response_ping_pong_0(uint32_t id, uint64_t begin, uint64_t replied) final;

    /**
     * \brief   Answer of a round trip request with 8 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_8(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data8 & data8) final;

    /**
     * \brief   Answer of a round trip request with 16 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_16(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data16 & data16) final;

    /**
     * \brief   Answer of a round trip request with 32 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_32(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data32 & data32) final;

    /**
     * \brief   Answer of a round trip request with 64 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_64(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data64 & data64) final;

    /**
     * \brief   Answer of a round trip request with 128 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_128(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data128 & data128) final;

    /**
     * \brief   Answer of a round trip request with 256 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_256(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data256 & data256) final;

    /**
     * \brief   Answer of a round trip request with 512 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_512(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data512 & data512) final;

    /**
     * \brief   Answer of a round trip request with 1024 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_1024(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data1024 & data1024) final;

    /**
     * \brief   Answer of a round trip request with 4096 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_4096(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data4096 & data4096) final;

    /**
     * \brief   Answer of a round trip request with 65536 bytes of extra data.
     * \see     response_ping_pong_0
     **/
    void response_ping_pong_65536(uint32_t id, uint64_t begin, uint64_t replied, const LocalLatency::Data65536 & data65536) final;

    /**
     * \brief   One-way message without extra data.
     *
     * \param   id      The sequence number stamped by the provider.
     * \param   begin   The timestamp the provider took just before sending.
     **/
    void broadcast_message_0(uint32_t id, uint64_t begin) final;

    /**
     * \brief   One-way message with 8 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_8(uint32_t id, uint64_t begin, const LocalLatency::Data8 & data8) final;

    /**
     * \brief   One-way message with 16 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_16(uint32_t id, uint64_t begin, const LocalLatency::Data16 & data16) final;

    /**
     * \brief   One-way message with 32 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_32(uint32_t id, uint64_t begin, const LocalLatency::Data32 & data32) final;

    /**
     * \brief   One-way message with 64 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_64(uint32_t id, uint64_t begin, const LocalLatency::Data64 & data64) final;

    /**
     * \brief   One-way message with 128 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_128(uint32_t id, uint64_t begin, const LocalLatency::Data128 & data128) final;

    /**
     * \brief   One-way message with 256 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_256(uint32_t id, uint64_t begin, const LocalLatency::Data256 & data256) final;

    /**
     * \brief   One-way message with 512 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_512(uint32_t id, uint64_t begin, const LocalLatency::Data512 & data512) final;

    /**
     * \brief   One-way message with 1024 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_1024(uint32_t id, uint64_t begin, const LocalLatency::Data1024 & data1024) final;

    /**
     * \brief   One-way message with 4096 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_4096(uint32_t id, uint64_t begin, const LocalLatency::Data4096 & data4096) final;

    /**
     * \brief   One-way message with 65536 bytes of extra data.
     * \see     broadcast_message_0
     **/
    void broadcast_message_65536(uint32_t id, uint64_t begin, const LocalLatency::Data65536 & data65536) final;

//////////////////////////////////////////////////////////////////////////
// Private methods
//////////////////////////////////////////////////////////////////////////
private:

    //!< Starts the very first run of the session.
    void _begin_session();

    //!< Prepares and starts the run of the mode the session currently points at.
    void _start_run();

    //!< Closes the current run, prints and stores its result, and either starts the next
    //!< run or ends the session.
    void _finish_run(uint64_t endNs);

    //!< Prints the closing summary and asks the application to quit.
    void _end_session();

    //!< Sends the next message of the current mode. One message at a time.
    void _send_next();

    //!< Takes the arrival timestamp and turns one arrived message into a sample or, while
    //!< warming up, into a discarded message. Then sends the next message.
    void _on_arrival(uint64_t begin, uint64_t replied);

    //!< Body of the watchdog thread.
    void _run_watchdog();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    const loclat::RunOptions &      mOptions;       //!< Settings of the session.
    loclat::LatencyReport           mReport;        //!< Samples of the run that is going on.
    std::vector<loclat::RunResult>  mResults;       //!< One entry per finished run.

    size_t                          mModeIndex;     //!< Position in mOptions.mModes.
    uint32_t                        mRepeat;        //!< 1-based number of the current repetition.
    LocalLatency::LatencyMode       mMode;          //!< The mode that is being measured.
    uint32_t                        mArrived;       //!< Messages arrived in this run, warm-up included.
    uint64_t                        mRunBeginNs;    //!< Start of the measured window of this run.
    double                          mClockCostNs;   //!< Cost of one clock reading, measured at start-up.
    bool                            mRunning;       //!< True while a run is going on.

    std::atomic<uint64_t>           mProgress;      //!< Grows with every arrived message; read by the watchdog.
    std::atomic_bool                mSessionDone;   //!< True once no further message is expected.

    WatchdogConsumer                mWatchdogConsumer;  //!< Must be declared before mWatchdogThread.
    areg::Thread                    mWatchdogThread;    //!< Runs the stall watchdog.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LocalLatencyConsumer);
};

#endif  // EXAMPLES_31_LOCLATENCY_SRC_LOCALLATENCYCONSUMER_HPP
