/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework examples
 * \author      Artak Avetyan
 * \brief       Collection of AREG SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"
#include "areg/trace/GETrace.h"
#include "areg/appbase/Application.hpp"
#include "areg/appbase/Console.hpp"

DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_serviceConnected);

Component * ServiceClient::CreateComponent(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
{
    return DEBUG_NEW ServiceClient(entry, owner);
}

void ServiceClient::DeleteComponent(Component & compObject, const NERegistry::ComponentEntry & entry)
{
    delete (&compObject);
}

ServiceClient::ServiceClient(const NERegistry::ComponentEntry & entry, ComponentThread & owner)
    : Component             ( entry, owner )
    , LargeDataClientBase   ( entry.mDependencyServices[0].mRoleName, static_cast<Component &>(self()) )
    , IETimerConsumer       ( )

    , mBitmap               ( )
    , mDataSize             ( 0 )
    , mBlockCount           ( 0 )
    , mTimer                (static_cast<IETimerConsumer&>(self()), TIMER_NAME)
{
}

void ServiceClient::startupComponent(ComponentThread& comThread)
{
    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize(mDataSize);
    Console& console = Console::getInstance();
    console.clearCurrentLine();
    console.outputTxt(COORD_TITLE, MSG_APP_TITLE);
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), static_cast<float>(dataRate.first), dataRate.second.data(), mBlockCount);
    console.refreshScreen();
}

void ServiceClient::broadcastImageBlockAcquired(const NELargeData::ImageBlock& imageBlock)
{
    const NELargeData::sImageBlock* block = imageBlock.getBlock();
    if ((block != nullptr) && mBitmap.allocateBitmap(block->frameWidth, block->frameHeight))
    {
        mBitmap.setBlock(imageBlock);
        mDataSize   += imageBlock.getSize();
        mBlockCount += 1;
    }
}

void ServiceClient::broadcastServiceStopping(void)
{
    mTimer.stopTimer();
    notifyOnBroadcastServiceStopping(false);
    notifyOnBroadcastImageBlockAcquired(false);

    if (mBitmap.isValid())
    {
        mBitmap.save(FILE_NAME.data());
    }

    Application::signalAppQuit();
}

bool ServiceClient::serviceConnected(bool isConnected, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_serviceConnected);
    bool result = LargeDataClientBase::serviceConnected(isConnected, proxy);

    TRACE_DBG("Client [ %s ] of [ %s ] service is [ %s ]"
                , proxy.getProxyAddress().getRoleName().getString()
                , proxy.getProxyAddress().getServiceName().getString()
                , isConnected ? "connected" : "disconnected");

    // dynamic subscribe on messages.
    notifyOnBroadcastServiceStopping(isConnected);
    notifyOnBroadcastImageBlockAcquired(isConnected);

    if (isConnected == false)
    {
        mTimer.stopTimer();
        Application::signalAppQuit();
    }
    else
    {
        mTimer.startTimer(NELargeData::TIMER_TIMEOUT, Timer::CONTINUOUSLY);
    }

    return result;
}

void ServiceClient::processTimer(Timer& timer)
{
    Console& console = Console::getInstance();
    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize( mDataSize );
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), mBlockCount);
    console.refreshScreen();

    mDataSize = 0;
    mBlockCount = 0;
}
