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

#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Timer.hpp"
#include "aregextend/console/Console.hpp"

#include "common/SimpleBitmap.hpp"
#include "common/LargeDataDefs.hpp"
#include "pubservice/src/UtilityDefs.hpp"

#include <atomic>
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

    //!< Coordinates to output item rate
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

    //!< The message to output the theoretical (ideal) data rate based on image parameters.
    static constexpr std::string_view   MSG_IDEAL_RATE_SENT { " Theoretical rate ...: ideal  [ %8.2f ] %s / sec, [ %8u ] blocks/sec." };
    //!< The message to output on-time / late delivery statistics
    static constexpr std::string_view   MSG_STATS_RATE      { " Stats on data ......: ontime [ %u ] msg, delayed [ %u ] msg." };

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
        areg::RemoteMessage message{};
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
    };

    //!< Use a coarse sleep chunk and a busy loop for the final gap. This keeps the long-run
    //!< rate locked to the absolute block schedule while avoiding sub-millisecond sleeps.
#if defined(_WIN32)
    static constexpr int64_t COARSE_SLEEP_NS{ 15'000'000LL };   // 15 ms
#elif defined(__APPLE__)
    static constexpr int64_t COARSE_SLEEP_NS{ 2'000'000LL };    //  2 ms
#else
    static constexpr int64_t COARSE_SLEEP_NS{ 500'000LL };      //  0.5 ms (was 4 ms)
#endif

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
    //!< Pre-built flat send queue of size `blocks × channels`.
    std::vector<ImageBlock> mSendList;
    //!< Snapshot of connected proxy addresses used to rebuild mPrebuiltMessages.
    std::vector<areg::ProxyAddress>     mActiveProxies;
    //!< Prebuilt messages
    PrebuildMessages        mPrebuiltMessages;
    //!< Data rates
    DataRate                mDataRate;
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
     * \brief   Updates the statistics  to output on console. Called each time when
     *          when image block is generated.
     * 
     * \param genData       The information of generated data rate.
     * \param genBlocks     The information of generated image blocks.
     * \param waitResult    The flag, indicating whether thread went to sleep after generating
     *                      data or ignored. It is used to compute blocks that were put in 
     *                      sleep or ignored.
     */
    void _update_data(uint64_t sendData, uint32_t sentBlocks, uint32_t ontimeBlocks, uint32_t lateBlocks);

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

inline uint64_t ServicingComponent::time_passed(const std::chrono::steady_clock::time_point& time_begin) const
{
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - time_begin).count());
}