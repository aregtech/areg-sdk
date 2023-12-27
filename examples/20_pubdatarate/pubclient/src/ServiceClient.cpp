/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit examples
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
#include "extend/console/Console.hpp"

DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_startupComponent);
DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_serviceConnected);
DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_processTimer);
DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_broadcastImageBlockAcquired);
DEF_TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_broadcastServiceStopping);

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
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_startupComponent);
    TRACE_DBG("The component [ %s ] has been started", getRoleName().getString());

    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize(mDataSize);
    Console& console = Console::getInstance();
    console.clearCurrentLine();
    console.outputTxt(COORD_TITLE, MSG_APP_TITLE);
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), static_cast<float>(dataRate.first), dataRate.second.data(), mBlockCount);
    console.refreshScreen();
}

void ServiceClient::broadcastImageBlockAcquired(const NELargeData::ImageBlock& imageBlock)
{
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_broadcastImageBlockAcquired);
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
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_broadcastServiceStopping);
    TRACE_DBG("Service stopped, quit application");

    mTimer.stopTimer();
    notifyOnBroadcastServiceStopping(false);
    notifyOnBroadcastImageBlockAcquired(false);

    if (mBitmap.isValid())
    {
        mBitmap.save(FILE_NAME.data());
    }

    Application::signalAppQuit();
}

bool ServiceClient::serviceConnected( NEService::eServiceConnection status, ProxyBase & proxy)
{
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_serviceConnected);
    bool result = LargeDataClientBase::serviceConnected(status, proxy);

    // dynamic subscribe on messages.
    notifyOnBroadcastServiceStopping(isConnected());
    notifyOnBroadcastImageBlockAcquired(isConnected());

    if (isConnected())
    {
        mTimer.startTimer( NELargeData::TIMER_TIMEOUT, Timer::CONTINUOUSLY );
    }
    else
    {
        mTimer.stopTimer( );
        Application::signalAppQuit( );
    }

    return result;
}

void ServiceClient::processTimer(Timer& timer)
{
    TRACE_SCOPE(examples_20_clientdatarate_ServiceClient_processTimer);
    Console& console = Console::getInstance();
    NEUtilities::DataLiteral dataRate = NEUtilities::convDataSize( mDataSize );
    TRACE_DBG("The timeout expired, output data rate: [ %f %s]", static_cast<float>(dataRate.first), dataRate.second.data());
    console.outputMsg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), mBlockCount);
    console.refreshScreen();

    mDataSize = 0;
    mBlockCount = 0;
}
