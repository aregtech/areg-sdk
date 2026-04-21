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

DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, startup_component);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, process_timer);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_service_stopping);

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LargeDataConsumerBase ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )

    , mBitmap               ( )
    , mDataSize             ( 0 )
    , mBlockCount           ( 0 )
    , mTimer                (static_cast<areg::TimerConsumer&>(self()), TIMER_NAME)
{
}

void ServiceClient::startup_component(areg::ComponentThread& /* comThread */)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, startup_component );
    LOG_DBG("The component [ %s ] has been started", role_name().as_string());

    areg::Application::enable_data_rate(true);
    areg::DataLiteral dataRate = areg::conv_data_size(mDataSize);
    areg::ext::Console& console = areg::ext::Console::instance();
    console.clear_current_line();
    console.output_txt(COORD_TITLE,     MSG_APP_TITLE);
    console.output_txt(COORD_SEP,       MSG_SEPARATOR);
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), mBlockCount);
    console.set_cursor_cur_position(COORD_CURSOR);
    console.refresh_screen();
}

void ServiceClient::broadcast_image_block_acquired(const LargeData::ImageBlock& imageBlock)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired );
    const LargeData::RawImageBlock* block = imageBlock.block();
    if ((block != nullptr) && mBitmap.allocate_bitmap(block->frameWidth, block->frameHeight))
    {
        mBitmap.set_block(imageBlock);
        mDataSize   += imageBlock.getSize();
        mBlockCount += 1;
    }
}

void ServiceClient::broadcast_service_stopping()
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, broadcast_service_stopping );
    LOG_DBG("Service stopped, quit application");

    mTimer.stop_timer();
    notify_on_broadcast_service_stopping(false);
    notify_on_broadcast_image_block_acquired(false);

    if (mBitmap.is_valid())
    {
        mBitmap.save(FILE_NAME.data());
    }

    areg::Application::signal_quit();
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, service_connected );
    bool result = LargeDataConsumerBase::service_connected(status, proxy);

    // dynamic subscribe on messages.
    notify_on_broadcast_service_stopping(is_connected());
    notify_on_broadcast_image_block_acquired(is_connected());

    if (is_connected())
    {
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, areg::Timer::CONTINUOUSLY );
    }
    else
    {
        mTimer.stop_timer( );
        areg::Application::signal_quit( );
    }

    return result;
}

void ServiceClient::process_timer(areg::Timer& /* timer */)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, process_timer );
    uint64_t sizeRecv{ 0u };
    uint32_t msgRecv{ 0u };
    areg::Application::query_data_received(sizeRecv, msgRecv);

    areg::ext::Console& console = areg::ext::Console::instance();
    areg::DataLiteral dataRate = areg::conv_data_size( sizeRecv );
    LOG_DBG("The timeout expired, output data rate: [ %f %s]", static_cast<double>(dataRate.first), dataRate.second.data());
    console.save_cursor_position();
    console.output_msg(COORD_DATA_RATE, MSG_DATA_RATE.data(), dataRate.first, dataRate.second.data(), msgRecv);
    console.restore_cursor_position();
    console.refresh_screen();

    mDataSize = 0;
    mBlockCount = 0;
}
