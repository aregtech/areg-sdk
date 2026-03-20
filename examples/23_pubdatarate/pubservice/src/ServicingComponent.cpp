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

DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent_startupServiceInterface);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent_shutdownServiceIntrface);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent_onOptionEvent);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent_onThreadRuns);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent__runInputThread);
DEF_LOG_SCOPE(examples_23_pubservice_ServicingComponent__runImageThread);

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
        mService.onTimerExpired();
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
    , mBlockList        ( )
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
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_startupServiceInterface);

    uint32_t sizeSend{ 0 }, sizeReceive{ 0 };
    mQuitThread = false;
    mOptionChanged = true;
    mPauseEvent.reset();   // pause

    areg::Application::query_communication_data( sizeSend, sizeReceive );
    uint64_t sizeItem = mItemRate != 0 ? mDataRate / mItemRate : 0;

    areg::DataLiteral dataRate = areg::conv_data_size(mDataRate);
    areg::DataLiteral sendRate = areg::conv_data_size( sizeSend );
    areg::DataLiteral rcvRate  = areg::conv_data_size( sizeReceive );
    areg::DataLiteral itemRate = areg::conv_data_size( sizeItem );


    areg::ext::Console& console = areg::ext::Console::instance();
    console.output_txt(COORD_TITLE, MSG_APP_TITLE);
    console.output_msg(COORD_COMM_RATE, MSG_COMM_RATE.data(), sendRate.first, sendRate.second.data(), rcvRate.first, rcvRate.second.data());
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data());
    console.output_msg(COORD_ITEM_RATE, MSG_ITEM_RATE.data(), mItemRate, itemRate.first, itemRate.second.data(), mDidSleep, mIgnoreSleep);
    _printInfo();

    _initBlockList();
    mInputThread.start(areg::WAIT_INFINITE);
    mImageThread.start(areg::WAIT_INFINITE);

    console.enable_console_input(true);

    LargeDataProviderBase::startup_service_interface(holder);
}

void ServicingComponent::shutdown_service_interface(areg::Component& holder) noexcept
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_shutdownServiceIntrface);

    mQuitThread = true;
    mOptionChanged = true;
    mTimer.stop_timer();
    mPauseEvent.set_signaled();

    mBitmap.release();
    mInputThread.shutdown(areg::WAIT_INFINITE);
    mImageThread.shutdown(areg::WAIT_INFINITE);

    LargeDataProviderBase::shutdown_service_interface(holder);
}

bool ServicingComponent::client_connected(const areg::ProxyAddress& client, areg::ServiceConnectionState connectionStatus )
{
    bool result = LargeDataProviderBase::client_connected(client, connectionStatus );
    mClients += (areg::is_service_connected( connectionStatus ) ? 1 : -1);
    _printInfo();

    return result;
}

void ServicingComponent::onTimerExpired()
{
    mLock.lock(areg::WAIT_INFINITE);

    uint32_t rateItem   = mItemRate;
    uint32_t didSleep   = mDidSleep;
    uint32_t ignoreSleep= mIgnoreSleep;

    uint32_t sizeSend{ 0 }, sizeReceive{ 0 };
    areg::Application::query_communication_data( sizeSend, sizeReceive );
    uint64_t sizeItem = rateItem != 0 ? mDataRate / rateItem : 0;

    areg::DataLiteral dataRate = areg::conv_data_size( mDataRate );
    areg::DataLiteral sendRate = areg::conv_data_size( sizeSend );
    areg::DataLiteral rcvRate  = areg::conv_data_size( sizeReceive );
    areg::DataLiteral itemRate = areg::conv_data_size( sizeItem );

    mItemRate = 0;
    mDataRate = 0;
    mDidSleep = 0;
    mIgnoreSleep = 0;
    mLock.unlock( );

    areg::ext::Console & console = areg::ext::Console::instance( );
    console.save_cursor_position( );

    console.output_msg( COORD_COMM_RATE, MSG_COMM_RATE.data( ), sendRate.first, sendRate.second.data( ), rcvRate.first, rcvRate.second.data( ) );
    console.output_msg( COORD_DATA_RATE, MSG_DATA_RATE.data( ), dataRate.first, dataRate.second.data( ) );
    console.output_msg( COORD_ITEM_RATE, MSG_ITEM_RATE.data( ), rateItem, itemRate.first, itemRate.second.data( ), didSleep, ignoreSleep );

    console.restore_cursor_position( );
    console.refresh_screen();
}

void ServicingComponent::onOptionEvent(const OptionData& data)
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_onOptionEvent);
    
    if (data.hasError())
    {
        LOG_WARN("Error input of command");
        areg::ext::Console& console = areg::ext::Console::instance();

        console.save_cursor_position();
        console.output_txt(COORD_ERROR_INFO, MSG_INVALID_CMD);
        console.restore_cursor_position();
    }
    else if (data.hasQuit())
    {
        LOG_WARN("Reqeusted to quit application");
        
        mQuitThread = true;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.set_signaled();
        mTimer.stop_timer();

        broadcast_service_stopping();

        areg::Application::signal_quit();
    }
    else if (data.hasStart())
    {
        LOG_INFO("Requested to start the generating data");

        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, component_thread(), areg::Timer::CONTINUOUSLY);
        mPauseEvent.set_signaled();
        _printInfo();
    }
    else if (data.hasStop())
    {
        LOG_INFO("Requested to stop generating data");

        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.reset();
        mTimer.stop_timer();
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
        LOG_INFO("Requested to change the generating data parameter(s)");

        bool isRunning = mOptions.hasStart();
        mOptions.update(data);
        _printInfo();

        mQuitThread = false;
        if (isRunning)
        {
            mPauseEvent.reset();
        }

        mOptionChanged = true;

        mLock.lock(areg::WAIT_INFINITE);
        _initBlockList();
        mLock.unlock();
        
        if (isRunning)
        {
            mPauseEvent.set_signaled();
        }
    }
}

void ServicingComponent::on_run()
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_onThreadRuns);

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
        LOG_SCOPE(examples_23_pubservice_ServicingComponent__runInputThread);
        LOG_DBG("Waiting to enter option command ...");

        console.output_txt(COORD_OPTIONS, MSG_INPUT_OPTION);
        console.refresh_screen();
        areg::String cmd = console.read_string();
        cmd.make_lower();
        OptionData newData;
        newData.parseCommand(cmd);
        cmdQuit = newData.hasQuit();
        EventOption::send_event(newData, static_cast<IEOptionConsumer&>(mOptionConsumer), component_thread());

        LOG_DBG("Have go the option command [ %s ]", cmd.as_string());
    }
}

void ServicingComponent::_runImageThread()
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent__runImageThread);

    uint32_t seqNr = 0;
    std::chrono::nanoseconds nsPerBlock{ mOptions.nsPerBlock() };
    uint32_t blocks = mOptions.blocksCount();

    areg::Wait wait;

    while (mQuitThread == false)
    {
        if (mOptionChanged)
        {
            seqNr = 0;
            mOptionChanged = false;
            mPauseEvent.lock();

            nsPerBlock = std::chrono::nanoseconds(mOptions.nsPerBlock());
            blocks = mOptions.blocksCount();
        }
        else
        {
            seqNr++;
        }

        seqNr = seqNr % mOptions.mHeight;

        for (uint32_t i = 0; !mOptionChanged && (i < blocks); ++i)
        {
            uint32_t  dataGenerated{ 0 }, blockGenerated{ 0 };
            std::chrono::steady_clock::time_point timeout = std::chrono::steady_clock::now() + nsPerBlock;
            for (uint32_t ch = 0; !mOptionChanged && (ch < mOptions.mChannels); ++ch)
            {
                ImageBlock & block = mBlockList.at(i);
                block.setIds(ch, seqNr);
                blockGenerated += 1;
                dataGenerated += block.getSize();

                LargeDataProviderBase::broadcast_image_block_acquired(block);
            }

            _updateData(dataGenerated, blockGenerated, wait.wait_until(timeout));
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
    console.set_cursor_cur_position(COORD_OPT_INFO);

    uint32_t bytesPerBlock  = mOptions.bytesPerBlock();
    uint64_t timePerBlock   = mOptions.nsPerBlock();

    double blockRate = (static_cast<double>(areg::DURATION_1_SEC) / static_cast<double>(timePerBlock)) * static_cast<double>(mOptions.mChannels);
    areg::DataLiteral dataRate = areg::conv_data_size(static_cast<uint32_t>(blockRate * bytesPerBlock));
    areg::DataLiteral blockSize= areg::conv_data_size(bytesPerBlock);
    areg::DataLiteral timeRate = areg::conv_duration(timePerBlock);

    console.print_txt("---------------------------------------\n");
    console.print_txt("Printing image current options:\n");
    console.print_msg("The large data state is : %s\n"   , mOptions.state().buffer());
    console.print_msg("\tWidth ...........: % 8u pix.\n" , mOptions.mWidth);
    console.print_msg("\tHeight ..........: % 8u pix.\n" , mOptions.mHeight);
    console.print_msg("\tLines per Block .: % 8u lns.\n" , mOptions.mLines);
    console.print_msg("\tPixel Time ......: % 8u ns.\n"  , mOptions.mPixelTime);
    console.print_msg("\tChannels ........: % 8u ch.\n"  , mOptions.mChannels);
    console.print_msg("\tTime per Block ..: % 8.02f %s.\n", static_cast<double>(timeRate.first), timeRate.second.data());
    console.print_msg("\tBlock Size ......: % 8.02f %s.\n", static_cast<double>(blockSize.first), blockSize.second.data());
    console.print_msg("\tBlock Rate ......: % 8u blocks / sec.\n", static_cast<uint32_t>(blockRate));
    console.print_msg("\tData Rate .......: % 8.02f %s / sec.\n", static_cast<double>(dataRate.first), dataRate.second.data());
    console.print_msg("\tConnected client : % 8d clients.\n", mClients);
    console.print_txt("---------------------------------------\n");

    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::_printHelp() const
{
    areg::ext::Console& console = areg::ext::Console::instance();
    console.save_cursor_position();
    console.set_cursor_cur_position(COORD_OPT_INFO);

    console.print_txt("---------------------------------------\n");
    console.print_txt("Printing help for the commands. Use int16_t or long command, one or a few of them.");
    console.print_msg("-w=<value> or --width=<value> ....: Image width. Range [32 .. 32768]\n");
    console.print_msg("-h=<value> or --height=<value> ...: Image height. Range [32 .. 32768]\n");
    console.print_msg("-l=<value> or --lines=<value> ....: Lines per image block, not larger than \'height\'.\n");
    console.print_msg("-t=<value> or --time=<value> .....: Time in nanoseconds when 1 pixel is generated.\n");
    console.print_msg("-c=<value> or --channels=<value> .: Image data source channels. Range [1 .. 64].\n");
    console.print_msg("-i         or --info .............: Print option status.\n");
    console.print_msg("-h         or --help .............: Print this help.\n");
    console.print_msg("-s         or --start ............: Start and run large data service.\n");
    console.print_msg("-p         or --stop .............: Stop generating image data and stop large data service.\n");
    console.print_msg("-q         or --quit .............: Stop service and quit application.\n");
    console.print_txt("---------------------------------------\n");

    console.restore_cursor_position();
    console.refresh_screen();
}

void ServicingComponent::_initBlockList()
{
    mBitmap.createBitmap(mOptions.mWidth, mOptions.mHeight);
    mBlockList.clear();
    uint32_t blocks = mOptions.blocksCount();
    mBlockList.resize(blocks);

    for (uint32_t i = 0; i < blocks; ++i)
    {
        mBlockList[i] = mBitmap.getBlock(i * mOptions.mLines, mOptions.mLines);
    }
}
