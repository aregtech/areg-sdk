/************************************************************************
 * \file        locservice/src/ServiceClient.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "pubclient/src/ServiceClient.hpp"
#include "areg/logging/areg_log.h"
#include "areg/appbase/Application.hpp"
#include "aregextend/console/Console.hpp"

DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient_startupComponent);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient_serviceConnected);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient_processTimer);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient_broadcastImageBlockAcquired);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient_broadcastServiceStopping);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component             ( entry, owner )
    , LargeDataClientBase   ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer       ( )

    , mBitmap               ( )
    , mDataSize             ( 0 )
    , mBlockCount           ( 0 )
    , mTimer                (static_cast<areg::TimerConsumer&>(self()), TIMER_NAME)
{
}

void ServiceClient::startup_component(areg::ComponentThread& /* comThread */)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient_startupComponent);
    LOG_DBG("The component [ %s ] has been started", role_name().as_string());

    areg::DataLiteral dataRate = areg::conv_data_size(mDataSize);
    aregext::Console& console = aregext::Console::instance();
    console.clear_current_line();
    console.output_txt(COORD_TITLE, MSG_APP_TITLE);
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), mBlockCount);
    console.refresh_screen();
}

void ServiceClient::broadcastImageBlockAcquired(const NELargeData::ImageBlock& imageBlock)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient_broadcastImageBlockAcquired);
    const NELargeData::RawImageBlock* block = imageBlock.getBlock();
    if ((block != nullptr) && mBitmap.allocateBitmap(block->frameWidth, block->frameHeight))
    {
        mBitmap.setBlock(imageBlock);
        mDataSize   += imageBlock.size();
        mBlockCount += 1;
    }
}

void ServiceClient::broadcastServiceStopping()
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient_broadcastServiceStopping);
    LOG_DBG("Service stopped, quit application");

    mTimer.stop_timer();
    notifyOnBroadcastServiceStopping(false);
    notifyOnBroadcastImageBlockAcquired(false);

    if (mBitmap.is_valid())
    {
        mBitmap.save(FILE_NAME.data());
    }

    areg::Application::signal_app_quit();
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient_serviceConnected);
    bool result = LargeDataClientBase::service_connected(status, proxy);

    // dynamic subscribe on messages.
    notifyOnBroadcastServiceStopping(is_connected());
    notifyOnBroadcastImageBlockAcquired(is_connected());

    if (is_connected())
    {
        mTimer.start_timer( areg::TIMER_TIMEOUT, areg::Timer::CONTINUOUSLY );
    }
    else
    {
        mTimer.stop_timer( );
        areg::Application::signal_app_quit( );
    }

    return result;
}

void ServiceClient::process_timer(areg::Timer& /* timer */)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient_processTimer);
    aregext::Console& console = aregext::Console::instance();
    areg::DataLiteral dataRate = areg::conv_data_size( mDataSize );
    LOG_DBG("The timeout expired, output data rate: [ %f %s]", static_cast<double>(dataRate.first), dataRate.second.data());
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), mBlockCount);
    console.refresh_screen();

    mDataSize = 0;
    mBlockCount = 0;
}
