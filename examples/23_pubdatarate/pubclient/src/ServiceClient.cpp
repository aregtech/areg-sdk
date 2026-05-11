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

#include <algorithm>

DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, startup_component);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, service_connected);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, process_timer);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_service_stopping);
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_settings);


ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LargeDataConsumerBase ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )

    , mTimer                ( static_cast<areg::TimerConsumer&>(self()), TIMER_NAME )
    , mCompleteFrames       ( 0u )
    , mIncompleteFrames     ( 0u )
    , mFrameStats           ( )
{
}

void ServiceClient::startup_component(areg::ComponentThread& /* comThread */)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, startup_component );
    LOG_DBG("The component [ %s ] has been started", role_name().as_string());

    areg::Application::enable_data_rate(true);
    areg::DataLiteral dataRate = areg::conv_data_size(0u);
    areg::ext::Console& console = areg::ext::Console::instance();
    console.clear_current_line();
    console.output_txt(COORD_TITLE,     MSG_APP_TITLE);
    console.output_txt(COORD_SEP1,      MSG_SEPARATOR);
    console.output_msg(COORD_DATA_RATE, MSG_NET_RATE_SENT.data(), dataRate.first, dataRate.second.data(), 0u);
    // console.output_msg(COORD_DATA_STAT, MSG_STATS_RATE.data()   , 0u, 0u);
    console.output_txt(COORD_SEP2,      MSG_SEPARATOR);

    console.set_cursor_cur_position(COORD_CURSOR);
    console.refresh_screen();

    mFrameStats.clear();
}

#if defined(DEBUG)
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);
void ServiceClient::broadcast_image_block_acquired(const areg::SharedBuffer& imageBlock)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired );

    const uint8_t* raw = imageBlock.buffer();
    const uint32_t imgSize = imageBlock.size_used();  // equals blockSize
    if (raw == nullptr || imgSize < sizeof(LargeData::RawImageBlock))
    {
        LOG_ERR("Invalid buffer data");
        ++mIncompleteFrames;
        return;
    }

    if (mFrameStats.empty())
    {
        LOG_ERR("The frame statistics is not initialized, cannot continue");
        ++mIncompleteFrames;
        return;
    }

    const LargeData::RawImageBlock* block = reinterpret_cast<const LargeData::RawImageBlock*>(raw);
    if (block->channelId >= static_cast<uint32_t>(mFrameStats.size()))
    {
        LOG_ERR("Invalid channel ID [ %u ], it cannot be bigger than [ %u ]", block->channelId, static_cast<uint32_t>(mFrameStats.size() - 1));
        ++mIncompleteFrames;
        return;
    }

    bool ok{ true };
    FrameStats& stat = mFrameStats[block->channelId];
    if (stat.height != static_cast<int32_t>(block->frameHeight))
    {
        ok = false;
        LOG_ERR("Invalid image height [ %u ], expected height is [ %u ]", block->frameHeight, static_cast<uint32_t>(stat.height));
    }

    switch (stat.state)
    {
    case Complete:              // fall through
        stat.seqNr  += 1;
        stat.line   = 0;
        stat.state  = Pending;
    case Pending:               // fall through
    case Missing:
        if (stat.seqNr != block->frameSeqId)
        {
            ok = false;
            LOG_WARN("Invalid sequence number [ %u ], expected sequence number [ %u ], probably missed image blocks", block->frameSeqId, static_cast<uint32_t>(stat.seqNr));
            stat.seqNr = block->frameSeqId;
        }
        else if (stat.line != block->imageData.imgStartPos.posY)
        {
            ok = false;
            LOG_WARN("Invalid line position [ %u ], expected line position [ %u ]", block->imageData.imgStartPos.posY, static_cast<uint32_t>(stat.line));
        }

        stat.line = block->imageData.imgStartPos.posY + block->imageData.imgHeight;
        if (stat.line > stat.height)
        {
            ok = false;
            LOG_WARN("Invalid line ending [ %u ], cannot be more than [ %u ]", static_cast<uint32_t>(stat.line), static_cast<uint32_t>(stat.height));
            stat.line = stat.height;
        }
        break;

    case Initial:
        stat.state  = Pending;
        stat.seqNr  = block->frameSeqId;
        stat.line = block->imageData.imgStartPos.posY + block->imageData.imgHeight;
        if (stat.line > stat.height)
        {
            ok = false;
            LOG_WARN("Invalid line ending [ %u ], cannot be more than [ %u ]", static_cast<uint32_t>(stat.line), static_cast<uint32_t>(stat.height));
            stat.line = stat.height;
        }
        break;

    default:
        ASSERT(false);
        ok = false;
    }

    stat.state = ok ? stat.state : Missing;
    if (stat.line == stat.height)
    {
        if (stat.state == Pending)
            ++mCompleteFrames;
        else if (stat.state == Missing)
            ++mIncompleteFrames;
        stat.state = Complete;
    }
}
#else
void ServiceClient::broadcast_image_block_acquired(const areg::SharedBuffer& /*imageBlock*/)
{
}
#endif

void ServiceClient::broadcast_service_stopping()
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, broadcast_service_stopping );
    LOG_DBG("Service stopped, quit application");

    mTimer.stop_timer();
    areg::Application::signal_quit();
}

void ServiceClient::broadcast_image_settings(uint32_t /*width*/, uint32_t height, uint32_t /*pixelTime*/, uint32_t /*linesBlock*/, uint32_t channels)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_settings);

    if (channels > LargeData::MAX_CHANNELS)
    {
        LOG_WARN("Ignoring image settings with invalid channel count [ %u ], max allowed [ %u ]", channels, LargeData::MAX_CHANNELS);
        return;
    }
    else if (channels == 0)
    {
        LOG_INFO("Release frame statistics");
        mFrameStats.clear();
        return;
    }

    mFrameStats.clear();
    mFrameStats.resize(channels);
    for (uint32_t i = 0; i < channels; ++i)
    {
        FrameStats entry{ static_cast<int32_t>(i), Initial, static_cast<int32_t>(height), 0, 0u };
        mFrameStats[i] = entry;
    }

    mCompleteFrames     = 0u;
    mIncompleteFrames   = 0u;
    if (mTimer.is_stopped())
    {
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, areg::Timer::CONTINUOUSLY);
    }
}

bool ServiceClient::service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, service_connected );
    bool result = LargeDataConsumerBase::service_connected(status, proxy);

    bool isConnected{ is_connected() };
    notify_on_broadcast_image_settings(isConnected);
    notify_on_broadcast_service_stopping(isConnected);
    notify_on_broadcast_image_block_acquired(isConnected);

    if (is_connected())
    {
        mCompleteFrames     = 0u;
        mIncompleteFrames   = 0u;
        mFrameStats.clear();
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, areg::Timer::CONTINUOUSLY);
    }
    else
    {
        mTimer.stop_timer();
        areg::Application::signal_quit();
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
    console.output_msg(COORD_DATA_RATE, MSG_NET_RATE_SENT.data(), dataRate.first, dataRate.second.data(), msgRecv);
    // console.output_msg(COORD_DATA_STAT, MSG_STATS_RATE.data()   , mCompleteFrames, mIncompleteFrames);

    console.restore_cursor_position();
    console.refresh_screen();
}
