#pragma once

/************************************************************************
 * \file        pubservice/src/ServicingComponent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of large data 
 *              servicing component to get commands, update data rates and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/component/EventTemplate.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "examples/23_pubdatarate/services/LargeDataProviderBase.hpp"

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Timer.hpp"
#include "aregextend/console/Console.hpp"

#include "common/SimpleBitmap.hpp"
#include "common/LargeDataDefs.hpp"
#include "pubservice/src/UtilityDefs.hpp"

#include <atomic>
#include <chrono>
#include <string_view>

//!< Declare as a class to use in namespace.
using OptionData = util::OptionValues;

AREG_DECLARE_EVENT(OptionData, EventOption, IEOptionConsumer);

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple servicing component, which generates the bitmap image data
 *          and sends to the clients. This demo show the data rate when generates data
 *          and when sends data to the clients.
 **/
class ServicingComponent final  : public    areg::Component
                                , protected LargeDataProviderBase
                                , protected areg::ThreadConsumer
{
    friend class OptionConsumer;
    friend class ServicingTimerConsumer;

//////////////////////////////////////////////////////////////////////////
// Internal classes
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ServicingComponent::OptionConsumer class declaration
//////////////////////////////////////////////////////////////////////////
    //!< OptionConsumer class to receive options changed events
    class OptionConsumer    : public    IEOptionConsumer
    {
    public:
        OptionConsumer( ServicingComponent & service )
            : IEOptionConsumer  ( )
            , mService          ( service )
            {
            }

        virtual ~OptionConsumer() = default;

    private:
    /************************************************************************/
    // IEOptionConsumer interface overrides
    /************************************************************************/

        /**
         * \brief   Triggered when option event is fired.
         * \param   data    The option data.
         **/
        void process_event(const OptionData& data) final;

    private:
        ServicingComponent &    mService;   //!< The service, which handles the options

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
        OptionConsumer() = delete;
        AREG_NOCOPY_NOMOVE(OptionConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// ServicingComponent::ServicingTimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
    //!< The timer consumer object
    class ServicingTimerConsumer : public    areg::TimerConsumer
    {
    public:
        ServicingTimerConsumer( ServicingComponent & service )
            : areg::TimerConsumer ( )
            , mService      ( service )
            {
            }

        virtual ~ServicingTimerConsumer() = default;

    private:
    /************************************************************************/
    // TimerConsumer interface overrides.
    /************************************************************************/

        /**
         * \brief   Triggered when Timer is expired. 
         * \param   timer   The timer object that is expired.
         **/
        void process_timer( areg::Timer & timer ) final;

    private:
        ServicingComponent &    mService;   //!< The service, which handles the options

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
        ServicingTimerConsumer() = delete;
        AREG_NOCOPY_NOMOVE(ServicingTimerConsumer);
    };

//////////////////////////////////////////////////////////////////////////
// Internal constants and static members
//////////////////////////////////////////////////////////////////////////

    //!< Coordinates to output application title / headline
    static constexpr areg::ext::Console::Coord     COORD_TITLE     { 1,  2 };

    //!< Coordinates to output a long separator line below the title
    static constexpr areg::ext::Console::Coord     COORD_SEP1{ 1,  3 };

    //!< Coordinates to output network communication rate (row 5, blank row 4 between separator and data)
    static constexpr areg::ext::Console::Coord     COORD_COMM_RATE { 1,  5 };

    //!< Coordinates to output data rate
    static constexpr areg::ext::Console::Coord     COORD_DATA_RATE { 1,  6 };

    //!< Coordinates to output how much the send pipeline held the generator back
    static constexpr areg::ext::Console::Coord     COORD_ITEM_RATE { 1,  7 };

    //!< Coordinates to output on-time / late statistics (blank row 8 before this)
    static constexpr areg::ext::Console::Coord     COORD_STATS     { 1,  9 };

    //!< Coordinates to output the theoretical (ideal) data rate
    static constexpr areg::ext::Console::Coord     COORD_IDEAL_RATE{ 1, 10 };

    //!< Coordinates to output short separator below rate block
    static constexpr areg::ext::Console::Coord     COORD_SEP2      { 1, 11 };

    //!< Coordinates to input the option commands (blank row 12 before this)
    static constexpr areg::ext::Console::Coord     COORD_OPTIONS   { 1, 13 };

    //!< Coordinates to output the error information.
    static constexpr areg::ext::Console::Coord     COORD_ERROR_INFO{ 1, 15 };

    //!< Coordinates to output the options information or application help
    static constexpr areg::ext::Console::Coord     COORD_OPT_INFO  { 1, 17 };

    //!< Number of rows reserved for the info / help output block.
    static constexpr int16_t  OPT_INFO_LINES   { 18 };

    //!< Message to output as application title / headline
    static constexpr std::string_view   MSG_APP_TITLE       { " Application to test data rate, Service Provider part..." };

    //!< Long separator drawn below the title
    static constexpr std::string_view   MSG_SEPARATOR       { " --------------------------------------------------------------------------------------------" };

    //!< The message to output network communication rate.
    static constexpr std::string_view   MSG_NET_RATE_SENT   { " Network sent rate ..: data   [ %8.2f ] %s / sec, [ %u ] blocks/sec." };

    //!< The message to output broadcast data rate information (actual bytes broadcast per second).
    static constexpr std::string_view   MSG_QUEUE_RATE_SENT { " Broadcast rate .....: sent   [ %8.2f ] %s / sec, [ %u ] blocks/sec." };

    //!< The message to output how much the generator was held back by the send pipeline.
    static constexpr std::string_view   MSG_SEND_PRESSURE   { " Send back-pressure .: producer blocked [ %6.2f ] %% of the time, skipped [ %u ] blocks/sec." };

    //!< The message to output the theoretical (ideal) data rate based on image parameters.
    static constexpr std::string_view   MSG_IDEAL_RATE_SENT { " Theoretical rate ...: ideal  [ %8.2f ] %s / sec, [ %8u ] blocks/sec." };
    //!< The message to output on-time / late delivery statistics
    static constexpr std::string_view   MSG_STATS_RATE      { " Stats on data ......: ontime [ %u ] msg/sec, delayed [ %u ] msg/sec." };

    //!< Short separator drawn below the rate block
    static constexpr std::string_view   MSG_SEP2            { " ---------------------------------------" };

    //!< The message to output as application option input
    static constexpr std::string_view   MSG_INPUT_OPTION    { " Input options. Or type \'-q\' to quit application, or type \'-h\' to read help: " };

    //!< The message to output as an error.
    static constexpr std::string_view   MSG_INVALID_CMD     { " Invalid command or value, type \'-h\' or \'--help\' for commands." };

    //!< The option command input thread.
    static constexpr std::string_view   THREAD_WAITINPUT    { "ConsoleInputThread" };

    //!< The data generating thread.
    static constexpr std::string_view   THREAD_GENERATE     { "GenerateImageThread" };

    //!< Timer name.
    static constexpr std::string_view   TIMER_NAME          { "DataRateTimer" };

    struct Remote
    {
        areg::MessageEnvelope message{};
        uint32_t            offset{0u};
    };

    using ImageBlock    = LargeData::ImageBlock;
    using RawImageBlock = LargeData::RawImageBlock;
    using MessageList   = std::vector <Remote>;
    /**
     * \brief   Per-proxy pre-serialized message pool.
     *          Each connected consumer gets its own pool because the wire message header
     *          embeds the target proxy address, making the byte layout proxy-specific.
     **/
    struct ProxyPool
    {
        uint32_t    proxyId {}; //!< The digital id of the proxy, unit withing system
        MessageList messages{}; //!< The list of pre-build messages.
        uint32_t    frameId{};  //!< The sequence id of actual frame
        uint32_t    depth{};    //!< The depth of messages, i.e. `depth * block_duration` == 100ms
        uint32_t    channels{}; //!< Number of channels. The length of `messages` is `channels * blocks_per_frame * depths`
        uint32_t    loop{};     //!< The current loop in dept
    };

    using PrebuildMessages = std::vector<ProxyPool>;

    struct DataRate
    {
        DataRate() = default;
        DataRate(const DataRate&) = default;

        //!< Data Rate in bytes
        uint64_t    sentData    { 0u };
        //!< Image blocks rate, number blocks.
        uint32_t    sentBlocks  { 0u };
        //!< Number of blocks sent on time (within their target period).
        uint32_t    ontimeBlocks{ 0u };
        //!< Number of blocks sent late (past their target deadline).
        uint32_t    lateBlocks  { 0u };
        //!< Number of blocks that were dropped because the schedule had already moved past them.
        uint32_t    skippedBlocks{ 0u };
        //!< Time, in nanoseconds, the generator spent inside send_raw_message(). A non-zero
        //!< value means the send pipeline pushed back on the generator.
        uint64_t    blockedNs   { 0u };
        //!< Length of the interval these counters were collected over. The generator hands its
        //!< counters over in chunks of its own cadence, which does not line up with the display
        //!< timer, so every chunk carries the span it covers. Rates are divided by the sum of
        //!< those spans, never by the time between two display ticks -- otherwise a window that
        //!< happens to catch one chunk more or less than the previous one shows a rate jump
        //!< that the generator never had.
        uint64_t    spanNs      { 0u };
    };

    //!< The generator is paced in message slots, not in whole block rows: one slot is one
    //!< (image block, channel) pair and `channels` slots fill exactly one block period. The
    //!< offered load per block period is unchanged - `channels * bytesPerBlock()` - but it
    //!< leaves the generator as one message every `nsPerBlock() / channels` instead of as a
    //!< single burst of `channels` messages. Sleeping is always done against the absolute
    //!< slot deadline, so the schedule cannot drift.
    //!<
    //!< A wait is chopped into chunks of this size so that an option change, a stop request and
    //!< the statistics flush stay responsive even when one slot period is very long.
    static constexpr int64_t MAX_WAIT_CHUNK_NS{ 20'000'000LL };  // 20 ms

    //!< Statistics are handed over to the console this often.
    static constexpr uint64_t STATS_FLUSH_NS  { 100'000'000ULL };  // 100 ms

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServicingComponent(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

/************************************************************************/
// ProviderBase overrides. Triggered by Component on startup.
/************************************************************************/

    /**
     * \brief   This function is triggered by Component when starts up.
     *          Overwrite this method and set appropriate request and
     *          attribute update notification event listeners here
     * \param   holder  The holder component of service interface of Provider,
     *                  which started up.
     **/
    void startup_service_interface( areg::Component & holder ) final;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and provider cleanup
     * \param   holder  The holder component of service interface of provider,
     *                  which shuts down.
     **/
    void shutdown_service_interface ( areg::Component & holder ) noexcept final;

/************************************************************************/
// ProviderBase overrides
/************************************************************************/
    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client  The address of proxy client, which connection status is changed.
     * \param   status  The service consumer connection status.
     * \return  Returns true if connected service consumer is relevant to the provider.
     **/
    bool consumer_connected( const areg::ProxyAddress & client, areg::ServiceConnectionState status ) final;

/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          start() method should be called again.
     **/
    void on_run() final;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Triggered when option event is fired.
     **/
    void on_option_event( const OptionData& data );

    /**
     * \brief   Triggered when Timer is expired. 
     **/
    void on_timer_expired();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< Bitmap object to generate data.
    SimpleBitmap            mBitmap;
    //!< Pre-built flat send queue of size `blocks x channels`.
    std::vector<ImageBlock> mSendList;
    //!< Snapshot of connected proxy addresses used to rebuild mPrebuiltMessages.
    std::vector<areg::ProxyAddress>     mActiveProxies;
    //!< Prebuilt messages
    PrebuildMessages        mPrebuiltMessages;
    //!< Data rates
    DataRate                mDataRate;
    //!< Start of the window the values in `mDataRate` were collected over. Every printed
    //!< value is divided by the measured length of that window, never by an assumed second.
    std::chrono::steady_clock::time_point mRateStamp;
    //!< The timer to trigger to output data
    areg::Timer             mTimer;
    //!< The thread to input from console.
    areg::Thread            mInputThread;
    //!< The thread to generate image data.
    areg::Thread            mImageThread;
    //! The actual options.
    util::OptionValues      mOptions;
    //! The atomic object to quit input thread.
    std::atomic_bool        mQuitThread;
    //! The atomic object to notify that options changed.
    std::atomic_bool        mOptionChanged;
    //! The event to pause generate image. The data generating thread should be paused when non-signaled and should run when signaled.
    areg::SyncEvent         mPauseEvent;
    //!< Number of connected clients.
    int32_t                 mClients;
    //!< The object to receive option data change event
    OptionConsumer          mOptionConsumer;
    //!< The object to receive timer expired event
    ServicingTimerConsumer  mTimerConsumer;
    //!< The synchronization item.
    areg::CriticalSection   mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< Return reference to this object.
    inline ServicingComponent & self();

    //!< Called for the thread that waits for user option input command.
    void _run_input_thread();

    //!< Called for the thread that generates image data and sends to the clients.
    void _run_image_thread();

    //!< Outputs the options information.
    void _print_info() const;

    //!< Outputs the application help.
    void _print_help() const;

    //!< Clears the info/help output region before re-printing it.
    void _clear_opt_info() const;

    //!< Generates and initializes the image blocks.
    void _init_block_list();

    /**
     * \brief   Adds one collected sample to the statistics shown on the console. The generator
     *          calls it from its own thread every `STATS_FLUSH_NS`, so the console never has to
     *          reach into the generator's counters.
     *
     * \param   sample  The counters collected since the previous call, together with the span
     *                  of time they cover. The span is what the console divides by, so a sample
     *                  that covers more or less than the display period still reads correctly.
     */
    void _update_data(const DataRate & sample);

    /**
     * \brief   Returns the absolute time, in nanoseconds from the start of the stream, at which
     *          the given message slot is due. A slot is one (image block, channel) pair and
     *          `channels` consecutive slots cover one block period, so the slots of a block
     *          period are spread evenly over it.
     *
     * \param   slot        The absolute slot number since the stream started.
     * \param   blockTimeNs Duration of one image block in nanoseconds.
     * \param   channels    Number of channels, i.e. slots per image block.
     **/
    [[nodiscard]]
    static inline uint64_t _slot_due_ns(uint64_t slot, uint64_t blockTimeNs, uint32_t channels) noexcept;

    /**
     * \brief   The inverse of _slot_due_ns(): the slot that is due at the given time.
     *
     * \param   timeNs      Time in nanoseconds from the start of the stream.
     * \param   blockTimeNs Duration of one image block in nanoseconds.
     * \param   channels    Number of channels, i.e. slots per image block.
     **/
    [[nodiscard]]
    static inline uint64_t _slot_at_ns(uint64_t timeNs, uint64_t blockTimeNs, uint32_t channels) noexcept;

    [[nodiscard]]
    inline bool _is_running() const noexcept;

    inline void _broadcast_block(areg::SharedBuffer& entry);

    inline bool _can_loop() const noexcept;

    /**
     * \brief   Builds the pre-serialized wire-message pool directly from the current
     *          image blocks in `mSendList`.
     **/
    uint32_t _build_prebuilt_messages();

    inline uint64_t time_passed(const std::chrono::steady_clock::time_point& time_begin) const;

    inline void print_rates(areg::ext::Console& console);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent() = delete;
    AREG_NOCOPY_NOMOVE( ServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ServicingComponent & ServicingComponent::self()
{
    return (*this);
}

inline bool ServicingComponent::_is_running() const noexcept
{
    return (!mQuitThread.load(std::memory_order_relaxed) && !mOptionChanged.load(std::memory_order_relaxed) && mOptions.hasStart());
}

inline bool ServicingComponent::_can_loop() const noexcept
{
    return (!mQuitThread.load(std::memory_order_relaxed) && !mOptionChanged.load(std::memory_order_relaxed));
}

inline void ServicingComponent::_broadcast_block(areg::SharedBuffer& entry)
{
    broadcast_image_block_acquired(entry);
}

inline uint64_t ServicingComponent::_slot_due_ns(uint64_t slot, uint64_t blockTimeNs, uint32_t channels) noexcept
{
    ASSERT(channels != 0u);
    const uint64_t block{ slot / channels };
    const uint64_t phase{ slot % channels };
    return ((block * blockTimeNs) + ((phase * blockTimeNs) / channels));
}

inline uint64_t ServicingComponent::_slot_at_ns(uint64_t timeNs, uint64_t blockTimeNs, uint32_t channels) noexcept
{
    ASSERT((channels != 0u) && (blockTimeNs != 0u));
    const uint64_t block{ timeNs / blockTimeNs };
    const uint64_t rest { timeNs % blockTimeNs };
    return ((block * channels) + ((rest * channels) / blockTimeNs));
}

inline uint64_t ServicingComponent::time_passed(const std::chrono::steady_clock::time_point& time_begin) const
{
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - time_begin).count());
}