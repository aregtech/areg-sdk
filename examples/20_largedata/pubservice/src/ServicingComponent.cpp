/************************************************************************
 * \file        pubservice/src/ServicingComponent.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
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
#include "areg/appbase/Console.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/trace/GETrace.h"

#include <chrono>

DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_startupServiceInterface);
DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_shutdownServiceIntrface);
DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_processTimer);
DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_processEvent);
DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_onThreadRuns);
DEF_TRACE_SCOPE(examples_20_pubservice_ServicingComponent_runImageThread);

Component * ServicingComponent::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServicingComponent(entry, owner);
}

void ServicingComponent::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServicingComponent::ServicingComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component         ( owner, entry.mRoleName)
    , LargeDataStub     ( static_cast<Component &>(self()) )
    , IETimerConsumer   ( )
    , IEOptionConsumer  ( )
    , IEThreadConsumer  ( )

    , mBitmap           ( )
    , mBlockList        ( )
    , mTimer            ( static_cast<IETimerConsumer &>(self()) , TIMER_NAME)
    , mInputThread      ( static_cast<IEThreadConsumer &>(self()), INPUT_THREAD_NAME)
    , mImageThread      ( static_cast<IEThreadConsumer &>(self()), IMAGE_THREAD_NAME)
    , mOptions          ( )
    , mQuitThread       ( true )
    , mOptionChanged    ( true )
    , mPauseEvent       ( false, false )
    , mClients          ( 0 )
    , mDataRate         ( 0 )
    , mItemRate         ( 0 )
    , mDidSleep         ( 0 )
    , mIgnoreSleep      ( 0 )
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
    TRACE_SCOPE(examples_20_pubservice_ServicingComponent_startupServiceInterface);

    mQuitThread = false;
    mOptionChanged = true;
    mPauseEvent.resetEvent();   // pause

    std::pair<double, std::string_view> dataRate = NELargeData::calcDataRate(mDataRate);
    Console& console = Console::getInstance();
    console.outputTxt(COORD_TITLE, MSG_APP_TITLE);
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data());
    console.outputMsg(COORD_ITEM_RATE, MSG_ITEM_RATE.data(), mItemRate, mDidSleep, mIgnoreSleep);
    _printInfo();

    _initBlockList();
    mInputThread.createThread(NECommon::WAIT_INFINITE);
    mImageThread.createThread(NECommon::WAIT_INFINITE);

    console.enableConsoleInput(true);

    LargeDataStub::startupServiceInterface(holder);
}

void ServicingComponent::shutdownServiceIntrface(Component& holder)
{
    TRACE_SCOPE(examples_20_pubservice_ServicingComponent_shutdownServiceIntrface);

    mQuitThread = true;
    mOptionChanged = true;
    mTimer.stopTimer();
    mPauseEvent.setEvent();

    mBitmap.release();
    mInputThread.destroyThread(NECommon::WAIT_INFINITE);
    mImageThread.destroyThread(NECommon::WAIT_INFINITE);

    LargeDataStub::shutdownServiceIntrface(holder);
}

void ServicingComponent::clientConnected(const ProxyAddress& client, bool isConnected)
{
    LargeDataStub::clientConnected(client, isConnected);

    if (isConnected)
    {
        mClients += 1;
    }
    else
    {
        mClients -= 1;
    }

    _printInfo();
}

void ServicingComponent::processTimer(Timer& timer)
{
    mLock.lock(NECommon::WAIT_INFINITE);

    uint32_t itemRate   = mItemRate;
    uint32_t didSleep   = mDidSleep;
    uint32_t ignoreSleep= mIgnoreSleep;

    std::pair<double, std::string_view> dataRate = NELargeData::calcDataRate(mDataRate);
    mItemRate = 0;
    mDataRate = 0;
    mDidSleep = 0;
    mIgnoreSleep = 0;
    mLock.unlock();

    Console& console = Console::getInstance();
    Console::Coord oldPos = console.getCursorCurPosition();
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data());
    console.outputMsg(COORD_ITEM_RATE, MSG_ITEM_RATE.data(), itemRate, didSleep, ignoreSleep);
    console.setCursorCurPosition(oldPos);
}

void ServicingComponent::processEvent(const OptionData& data)
{
    if (data.hasError())
    {
        Console::Coord oldPos = Console::getInstance().getCursorCurPosition();
        Console::getInstance().outputTxt(COORD_ERROR_INFO, MSG_INVALID_CMD);
        Console::getInstance().setCursorCurPosition(oldPos);
    }
    else if (data.hasQuit())
    {
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

        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mTimer.startTimer(NELargeData::TIMER_TIMEOUT, getComponentThread(), Timer::CONTINUOUSLY);
        mPauseEvent.setEvent();
    }
    else if (data.hasStop())
    {
        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.resetEvent();
        mTimer.stopTimer();
    }
    else if (data.hasPause())
    {
        mQuitThread = false;
        mOptionChanged = true;
        mOptions.update(data);
        mPauseEvent.resetEvent();
        mTimer.stopTimer();
    }
    else if (data.hasPrintHelp())
    {
        _printHelp();
    }
    else if (data.hasPrintInfo())
    {
        _printInfo();
    }
    else
    {
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
    TRACE_SCOPE(examples_20_pubservice_ServicingComponent_onThreadRuns);

    const String& threadName = Thread::getCurrentThreadName();
    if (threadName == INPUT_THREAD_NAME)
    {
        TRACE_DBG("Started console input thread.");
        _runInputThread();
    }
    else if (threadName == IMAGE_THREAD_NAME)
    {
        TRACE_DBG("Started generate image thread.");
        _runImageThread();
    }
}

void ServicingComponent::_runInputThread(void)
{
    Console& console = Console::getInstance();

    bool cmdQuit{ false };
    while ((cmdQuit == false) && (mQuitThread == false))
    {
        console.outputTxt(COORD_OPTIONS, MSG_INPUT_OPTION);
        String cmd = console.readString();
        OptionData newData;
        newData.parseCommand(cmd);
        cmdQuit = newData.hasQuit();
        EventOption::sendEvent(newData, static_cast<IEOptionConsumer&>(self()), getComponentThread());
    }
}

void ServicingComponent::_runImageThread(void)
{
    TRACE_SCOPE(examples_20_pubservice_ServicingComponent_runImageThread);

    uint32_t seqNr = 0;
    std::chrono::nanoseconds nsPerBlock{ mOptions.nsPerBlock() };
    uint32_t blocks = mOptions.blocksCount();

    Wait wait;

    while (mQuitThread == false)
    {
        if (mOptionChanged)
        {
            mOptionChanged = false;

            mPauseEvent.lock();
            if (!mOptions.hasPause())
            {
                seqNr = 0;
            }

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

            // std::chrono::nanoseconds diff = timeout - std::chrono::steady_clock::now();
            // TRACE_DBG("Difference: %d ", static_cast<int32_t>(diff.count()));
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
    Console::Coord curPos = console.getCursorCurPosition();
    console.setCursorCurPosition(COORD_OPT_INFO);

    uint32_t bytesPerBlock  = mOptions.bytesPerBlock();
    uint64_t timePerBlock   = mOptions.nsPerBlock();

    double blockRate = (static_cast<double>(NECommon::DURATION_1_SEC) / timePerBlock) * mOptions.mChannels;
    std::pair<double, std::string_view> dataRate = NELargeData::calcDataRate(static_cast<uint32_t>(blockRate * bytesPerBlock));
    std::pair<double, std::string_view> blockSize= NELargeData::calcDataRate(bytesPerBlock);
    std::pair<double, std::string_view> timeRate = NELargeData::calcDuration(timePerBlock);

    console.printTxt("---------------------------------------\n");
    console.printTxt("Printing image current options:\n");
    console.printMsg("The large data state is : %s\n"   , mOptions.getState().getBuffer());
    console.printMsg("\tWidth ...........: % 8u pix.\n" , mOptions.mWidth);
    console.printMsg("\tHeight ..........: % 8u pix.\n" , mOptions.mHeight);
    console.printMsg("\tLines per Block .: % 8u lns.\n" , mOptions.mLines);
    console.printMsg("\tPixel Time ......: % 8u ns.\n"  , mOptions.mPixelTime);
    console.printMsg("\tChannels ........: % 8u ch.\n"  , mOptions.mChannels);
    console.printMsg("\tTime per Block ..: % 8.02f %s.\n", static_cast<float>(timeRate.first), timeRate.second.data());
    console.printMsg("\tBlock Size ......: % 8.02f %s.\n", static_cast<float>(blockSize.first), blockSize.second.data());
    console.printMsg("\tBlock Rate ......: % 8u blocks / sec.\n", static_cast<uint32_t>(blockRate));
    console.printMsg("\tData Rate .......: % 7.02f %s / sec.\n", static_cast<float>(dataRate.first), dataRate.second.data());
    console.printMsg("\tConnected client : % 8d clients.\n", mClients);
    console.printTxt("---------------------------------------\n");

    console.setCursorCurPosition(curPos);
}

void ServicingComponent::_printHelp(void) const
{
    Console& console = Console::getInstance();
    Console::Coord curPos = console.getCursorCurPosition();
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
    console.printMsg("-o         or --stop .............: Stop generating image data and stop large data service.\n");
    console.printMsg("-a         or --pause ............: Pause generating image data and stop large data service.\n");
    console.printMsg("-q         or --quit .............: Stop service and quit application.\n");
    console.printTxt("---------------------------------------\n");

    console.setCursorCurPosition(curPos);
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
