/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of servicing component
 *              without requests
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "pubservice/src/ServicingComponent.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/logging/GELog.h"
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

void ServicingComponent::OptionConsumer::processEvent(const OptionData& data)
{
    mService.onOptionEvent( data );
}

//////////////////////////////////////////////////////////////////////////
// ServicingComponent::TimerConsumer class implementation
//////////////////////////////////////////////////////////////////////////

void ServicingComponent::TimerConsumer::processTimer( Timer & timer )
{
    if (&timer == &mService.mTimer)
    {
        mService.onTimerExpired();
    }
}

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class implementation
//////////////////////////////////////////////////////////////////////////

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( entry, owner )
    , LargeDataStub     ( static_cast<Component &>(self()) )
    , IEThreadConsumer  ( )

    , mBitmap           ( )
    , mBlockList        ( )
    , mTimer            ( static_cast<IETimerConsumer &>(mTimerConsumer) , TIMER_NAME )
    , mInputThread      ( static_cast<IEThreadConsumer &>(self()), THREAD_WAITINPUT )
    , mImageThread      ( static_cast<IEThreadConsumer &>(self()), THREAD_GENERATE )
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
    mOptions.mWidth     = NEUtilities::IMAGE_WIDTH;
    mOptions.mHeight    = NEUtilities::IMAGE_HEIGHT;
    mOptions.mLines     = NEUtilities::LINES_PER_BLOCK;
    mOptions.mPixelTime = NEUtilities::DWELL_TIME;
    mOptions.mChannels  = NEUtilities::CHANNELS_SOURCE;
    mOptions.mFlags     = static_cast<uint32_t>(NEUtilities::eOptionFlags::CmdStop);
}

void ServicingComponent::startupServiceInterface( Component & holder )
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_startupServiceInterface);

    unsigned int sizeSend{ 0 }, sizeReceive{ 0 };
    mQuitThread = false;
    mOptionChanged = true;
    mPauseEvent.resetEvent();   // pause

    Application::queryCommunicationData( sizeSend, sizeReceive );
    uint64_t sizeItem = mItemRate != 0 ? mDataRate / mItemRate : 0;

    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize(mDataRate);
    NEUtilities::DataLiteral sendRate = NEUtilities::convDataSize( sizeSend );
    NEUtilities::DataLiteral rcvRate  = NEUtilities::convDataSize( sizeReceive );
    NEUtilities::DataLiteral itemRate = NEUtilities::convDataSize( sizeItem );


    Console& console = Console::getInstance();
    console.outputTxt(COORD_TITLE, MSG_APP_TITLE);
    console.outputMsg(COORD_COMM_RATE, MSG_COMM_RATE.data(), sendRate.first, sendRate.second.data(), rcvRate.first, rcvRate.second.data());
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data());
    console.outputMsg(COORD_ITEM_RATE, MSG_ITEM_RATE.data(), mItemRate, itemRate.first, itemRate.second.data(), mDidSleep, mIgnoreSleep);
    _printInfo();

    _initBlockList();
    mInputThread.createThread(NECommon::WAIT_INFINITE);
    mImageThread.createThread(NECommon::WAIT_INFINITE);

    console.enableConsoleInput(true);

    LargeDataStub::startupServiceInterface(holder);
}

void ServicingComponent::shutdownServiceIntrface(Component& holder)
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_shutdownServiceIntrface);

    mQuitThread = true;
    mOptionChanged = true;
    mTimer.stopTimer();
    mPauseEvent.setEvent();

    mBitmap.release();
    mInputThread.shutdownThread(NECommon::WAIT_INFINITE);
    mImageThread.shutdownThread(NECommon::WAIT_INFINITE);

    LargeDataStub::shutdownServiceIntrface(holder);
}

bool ServicingComponent::clientConnected(const ProxyAddress& client, NEService::eServiceConnection connectionStatus )
{
    bool result = LargeDataStub::clientConnected(client, connectionStatus );
    mClients += (NEService::isServiceConnected( connectionStatus ) ? 1 : -1);
    _printInfo();

    return result;
}

void ServicingComponent::onTimerExpired( void )
{
    mLock.lock(NECommon::WAIT_INFINITE);

    uint32_t rateItem   = mItemRate;
    uint32_t didSleep   = mDidSleep;
    uint32_t ignoreSleep= mIgnoreSleep;

    unsigned int sizeSend{ 0 }, sizeReceive{ 0 };
    Application::queryCommunicationData( sizeSend, sizeReceive );
    uint64_t sizeItem = rateItem != 0 ? mDataRate / rateItem : 0;

    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize( mDataRate );
    NEUtilities::DataLiteral sendRate = NEUtilities::convDataSize( sizeSend );
    NEUtilities::DataLiteral rcvRate  = NEUtilities::convDataSize( sizeReceive );
    NEUtilities::DataLiteral itemRate = NEUtilities::convDataSize( sizeItem );

    mItemRate = 0;
    mDataRate = 0;
    mDidSleep = 0;
    mIgnoreSleep = 0;
    mLock.unlock( );

    Console & console = Console::getInstance( );
    console.saveCursorPosition( );

    console.outputMsg( COORD_COMM_RATE, MSG_COMM_RATE.data( ), sendRate.first, sendRate.second.data( ), rcvRate.first, rcvRate.second.data( ) );
    console.outputMsg( COORD_DATA_RATE, MSG_DATA_RATE.data( ), dataRate.first, dataRate.second.data( ) );
    console.outputMsg( COORD_ITEM_RATE, MSG_ITEM_RATE.data( ), rateItem, itemRate.first, itemRate.second.data( ), didSleep, ignoreSleep );

    console.restoreCursorPosition( );
    console.refreshScreen();
}

void ServicingComponent::onOptionEvent(const OptionData& data)
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_onOptionEvent);
    
    if (data.hasError())
    {
        LOG_WARN("Error input of command");
        Console& console = Console::getInstance();

        console.saveCursorPosition();
        console.outputTxt(COORD_ERROR_INFO, MSG_INVALID_CMD);
        console.restoreCursorPosition();
    }
    else if (data.hasQuit())
    {
        LOG_WARN("Reqeusted to quit application");
        
        mQuitThread = true;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.setEvent();
        mTimer.stopTimer();

        broadcastServiceStopping();

        Application::signalAppQuit();
    }
    else if (data.hasStart())
    {
        LOG_INFO("Requested to start the generating data");

        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mTimer.startTimer(NELargeData::TIMER_TIMEOUT, getComponentThread(), Timer::CONTINUOUSLY);
        mPauseEvent.setEvent();
        _printInfo();
    }
    else if (data.hasStop())
    {
        LOG_INFO("Requested to stop generating data");

        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.resetEvent();
        mTimer.stopTimer();
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
            mPauseEvent.resetEvent();
        }

        mOptionChanged = true;

        mLock.lock(NECommon::WAIT_INFINITE);
        _initBlockList();
        mLock.unlock();
        
        if (isRunning)
        {
            mPauseEvent.setEvent();
        }
    }
}

void ServicingComponent::onThreadRuns(void)
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent_onThreadRuns);

    const String& threadName = Thread::getCurrentThreadName();
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

void ServicingComponent::_runInputThread(void)
{
    Console& console = Console::getInstance();

    bool cmdQuit{ false };
    while ((cmdQuit == false) && (mQuitThread == false))
    {
        LOG_SCOPE(examples_23_pubservice_ServicingComponent__runInputThread);
        LOG_DBG("Waiting to enter option command ...");

        console.outputTxt(COORD_OPTIONS, MSG_INPUT_OPTION);
        console.refreshScreen();
        String cmd = console.readString();
        cmd.makeLower();
        OptionData newData;
        newData.parseCommand(cmd);
        cmdQuit = newData.hasQuit();
        EventOption::sendEvent(newData, static_cast<IEOptionConsumer&>(mOptionConsumer), getComponentThread());

        LOG_DBG("Have go the option command [ %s ]", cmd.getString());
    }
}

void ServicingComponent::_runImageThread(void)
{
    LOG_SCOPE(examples_23_pubservice_ServicingComponent__runImageThread);

    uint32_t seqNr = 0;
    std::chrono::nanoseconds nsPerBlock{ mOptions.nsPerBlock() };
    uint32_t blocks = mOptions.blocksCount();

    Wait wait;

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

                LargeDataStub::broadcastImageBlockAcquired(block);
            }

            _updateData(dataGenerated, blockGenerated, wait.waitUntil(timeout));
        }
    }
}

void ServicingComponent::_updateData(uint64_t genData, uint32_t genBlocks, Wait::eWaitResult waitResult)
{
    Lock lock(mLock);
    mDataRate += genData;
    mItemRate += genBlocks;
    if (waitResult >= Wait::eWaitResult::WaitInMicro)
    {
        mDidSleep += 1;
    }
    else
    {
        mIgnoreSleep += 1;
    }
}


uint64_t ServicingComponent::_getBlockImageTime(void) const
{
    return static_cast<uint64_t>(mOptions.mWidth) * mOptions.mLines * mOptions.mPixelTime;
}

void ServicingComponent::_printInfo(void) const
{
    Console& console = Console::getInstance();
    console.saveCursorPosition();
    console.setCursorCurPosition(COORD_OPT_INFO);

    uint32_t bytesPerBlock  = mOptions.bytesPerBlock();
    uint64_t timePerBlock   = mOptions.nsPerBlock();

    double blockRate = (static_cast<double>(NECommon::DURATION_1_SEC) / static_cast<double>(timePerBlock)) * static_cast<double>(mOptions.mChannels);
    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize(static_cast<uint32_t>(blockRate * bytesPerBlock));
    NEUtilities::DataLiteral blockSize= NEUtilities::convDataSize(bytesPerBlock);
    NEUtilities::DataLiteral timeRate = NEUtilities::convDuration(timePerBlock);

    console.printTxt("---------------------------------------\n");
    console.printTxt("Printing image current options:\n");
    console.printMsg("The large data state is : %s\n"   , mOptions.getState().getBuffer());
    console.printMsg("\tWidth ...........: % 8u pix.\n" , mOptions.mWidth);
    console.printMsg("\tHeight ..........: % 8u pix.\n" , mOptions.mHeight);
    console.printMsg("\tLines per Block .: % 8u lns.\n" , mOptions.mLines);
    console.printMsg("\tPixel Time ......: % 8u ns.\n"  , mOptions.mPixelTime);
    console.printMsg("\tChannels ........: % 8u ch.\n"  , mOptions.mChannels);
    console.printMsg("\tTime per Block ..: % 8.02f %s.\n", static_cast<double>(timeRate.first), timeRate.second.data());
    console.printMsg("\tBlock Size ......: % 8.02f %s.\n", static_cast<double>(blockSize.first), blockSize.second.data());
    console.printMsg("\tBlock Rate ......: % 8u blocks / sec.\n", static_cast<uint32_t>(blockRate));
    console.printMsg("\tData Rate .......: % 8.02f %s / sec.\n", static_cast<double>(dataRate.first), dataRate.second.data());
    console.printMsg("\tConnected client : % 8d clients.\n", mClients);
    console.printTxt("---------------------------------------\n");

    console.restoreCursorPosition();
    console.refreshScreen();
}

void ServicingComponent::_printHelp(void) const
{
    Console& console = Console::getInstance();
    console.saveCursorPosition();
    console.setCursorCurPosition(COORD_OPT_INFO);

    console.printTxt("---------------------------------------\n");
    console.printTxt("Printing help for the commands. Use short or long command, one or a few of them.");
    console.printMsg("-w=<value> or --width=<value> ....: Image width. Range [32 .. 32768]\n");
    console.printMsg("-h=<value> or --height=<value> ...: Image height. Range [32 .. 32768]\n");
    console.printMsg("-l=<value> or --lines=<value> ....: Lines per image block, not larger than \'height\'.\n");
    console.printMsg("-t=<value> or --time=<value> .....: Time in nanoseconds when 1 pixel is generated.\n");
    console.printMsg("-c=<value> or --channels=<value> .: Image data source channels. Range [1 .. 64].\n");
    console.printMsg("-i         or --info .............: Print option status.\n");
    console.printMsg("-h         or --help .............: Print this help.\n");
    console.printMsg("-s         or --start ............: Start and run large data service.\n");
    console.printMsg("-p         or --stop .............: Stop generating image data and stop large data service.\n");
    console.printMsg("-q         or --quit .............: Stop service and quit application.\n");
    console.printTxt("---------------------------------------\n");

    console.restoreCursorPosition();
    console.refreshScreen();
}

void ServicingComponent::_initBlockList(void)
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
