#pragma once

/************************************************************************
 * \file        pubservice/src/ServicingComponent.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of large data 
 *              servicing component to get commands, update data rates and shutdown.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/TEEvent.hpp"
#include "generated/src/LargeDataStub.hpp"

#include "areg/appbase/Console.hpp"
#include "areg/base/SynchObjects.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/Timer.hpp"

#include "common/SimpleBitmap.hpp"
#include "common/NELargeData.hpp"
#include "pubservice/src/NEUtilities.hpp"

#include <atomic>
#include <string_view>

//!< Declare as a class to use in namespace.
class OptionData : public NEUtilities::sOptionData
{
};

DECLARE_EVENT(OptionData, EventOption, IEOptionConsumer);

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A demo of simple servicing component, which generates the bitmap image data
 *          and sends to the clients. This demo show the data rate when generates data
 *          and when sends data to the clients.
 **/
class ServicingComponent    : public    Component
                            , protected LargeDataStub
                            , protected IETimerConsumer
                            , protected IEOptionConsumer
                            , protected IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal constants and static members
//////////////////////////////////////////////////////////////////////////

    //!< Coordinates to output application title / headline
    static constexpr Console::Coord     COORD_TITLE     { 0, 0 };

    //!< Coordinates to output data rate
    static constexpr Console::Coord     COORD_DATA_RATE { 0, 1 };

    //!< Coordinates to output item rate
    static constexpr Console::Coord     COORD_ITEM_RATE { 0, 2 };

    //!< Coordinates to output information of thread suspend statistics
    static constexpr Console::Coord     COORD_INFO_SLEEP{ 0, 3 };

    //!< Coordinates to input the option commands
    static constexpr Console::Coord     COORD_OPTIONS   { 0, 4 };

    //!< Coordinates to output the error information.
    static constexpr Console::Coord     COORD_ERROR_INFO{ 0, 5 };

    //!< Coordinates to output the options information or application help
    static constexpr Console::Coord     COORD_OPT_INFO  { 0, 7 };

    //!< Message to output as application title / headline
    static constexpr std::string_view   MSG_APP_TITLE   { "Application to test data rate, service part..." };

    //!< Message to output as application option input
    static constexpr std::string_view   MSG_INPUT_OPTION{ "Input options. Or type \'-q\' to quit application, or type \'-h\' to read help: " };

    //!< Message to output as an error.
    static constexpr std::string_view   MSG_INVALID_CMD { "Invalid command or value, type \'-h\' or \'--help\' for commands." };

    //!< Message to output data rate information
    static constexpr std::string_view   MSG_DATA_RATE   { "Data rate: sent [ % 4.02f ] %s / sec." };

    //!< Message to output item rate information
    static constexpr std::string_view   MSG_ITEM_RATE   { "Item rate: sent [ %u ] items / sec. Sleep [ %u ] times, ignored [ %u ] times." };

    //!< The option command input thread.
    static constexpr std::string_view   THREAD_WAITINPUT{ "ConsoleInputThread" };

    //!< The data generating thread.
    static constexpr std::string_view   THREAD_GENERATE { "GenerateImageThread" };

    //!< Timer name.
    static constexpr std::string_view   TIMER_NAME     { "LargeDataTimer" };

    using ImageBlock = NELargeData::ImageBlock;

//////////////////////////////////////////////////////////////////////////
// Static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Called by system to instantiate the component.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     * \return  Returns instantiated component to run in the system
     **/
    static Component * CreateComponent( const NERegistry::ComponentEntry & entry, ComponentThread & owner );

    /**
     * \brief   Called by system to delete component and free resources.
     * \param   compObject  The instance of component previously created by CreateComponent method.
     * \param   entry   The entry of registry, which describes the component.
     **/
    static void DeleteComponent( Component & compObject, const NERegistry::ComponentEntry & entry );

//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Instantiates the component object.
     * \param   entry   The entry of registry, which describes the component.
     * \param   owner   The component owning thread.
     **/
    ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner);

    /**
     * \brief   Destructor.
     **/
    virtual ~ServicingComponent(void) = default;

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
    virtual void startupServiceInterface( Component & holder ) override;

    /**
     * \brief   This function is triggered by Component when shuts down.
     *          Overwrite this method to remove listeners and stub cleanup
     * \param   holder  The holder component of service interface of Stub,
     *                  which shuts down.
     **/
    virtual void shutdownServiceIntrface ( Component & holder ) override;

    /**
     * \brief   Triggered when proxy client either connected or disconnected to stub.
     * \param   client      The address of proxy client, which connection status is changed.
     * \param   isConnected Flag, indicating whether client is connected or disconnected.
     *                      When client disconnects, all listeners are removed.
     **/
    virtual void clientConnected( const ProxyAddress & client, bool isConnected ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   Triggered when option event is fired.
     * \param   data    The option data.
     **/
    virtual void processEvent(const OptionData& data) override;
    
/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is 
     *          running and fully operable. If thread needs run in loop, the loop 
     *          should be implemented here. When consumer exits this function, 
     *          the thread will complete work. To restart thread running, 
     *          createThread() method should be called again.
     **/
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< Bitmap object to generate data.
    SimpleBitmap                mBitmap;
    //!< List of generated image blocks.
    std::vector<ImageBlock>     mBlockList;
    //! The timer to trigger to output data
    Timer                       mTimer;
    //! The thread to input from console.
    Thread                      mInputThread;
    //! The thread to generate image data.
    Thread                      mImageThread;
    //! The actual options.
    NEUtilities::sOptionData    mOptions;
    //! The atomic object to quit input thread.
    std::atomic_bool            mQuitThread;
    //! The atomic object to notify that options changed.
    std::atomic_bool            mOptionChanged;
    //! The event to pause generate image.
    //! The data generating thread should be paused when non-signaled and should run when signaled.
    SynchEvent                  mPauseEvent;
    //!< Number of connected clients.
    int32_t                     mClients;
    //!< Data Rate in bytes
    uint64_t                    mDataRate;
    //!< Image blocks rate, number blocks.
    uint32_t                    mItemRate;
    //!< Number of blocks that put to sleep.
    uint32_t                    mDidSleep;
    //!< Number of blocks that ignored sleep.
    uint32_t                    mIgnoreSleep;
    //! The synchronization item.
    CriticalSection             mLock;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< Return reference to this object.
    inline ServicingComponent & self( void );

    //!< Called for the thread that waits for user option input command.
    void _runInputThread(void);

    //!< Called for the thread that generates image data and sends to the clients.
    void _runImageThread(void);

    //!< Calculates and returns time in nanoseconds required to generate one image block.
    uint64_t _getBlockImageTime(void) const;

    //!< Outputs the options information.
    void _printInfo(void) const;

    //!< Outputs the application help.
    void _printHelp(void) const;

    //!< Generates and initializes the image blocks.
    void _initBlockList(void);

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
    void _updateData(uint64_t genData, uint32_t genBlocks, Wait::eWaitResult waitResult);

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServicingComponent( void ) = delete;
    DECLARE_NOCOPY_NOMOVE( ServicingComponent );
};

//////////////////////////////////////////////////////////////////////////
// ServicingComponent inline methods
//////////////////////////////////////////////////////////////////////////
inline ServicingComponent & ServicingComponent::self( void )
{
    return (*this);
}
