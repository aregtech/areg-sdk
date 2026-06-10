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
DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_service_stopping);

#define DO_STATS    0

ServiceClient::ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
    : areg::Component       ( entry, owner )
    , LargeDataConsumerBase ( entry.mDependencyServices[0].mRoleName, static_cast<areg::Component &>(self()) )
    , areg::TimerConsumer   ( )

    , mTimer                    ( static_cast<areg::TimerConsumer&>(self()), TIMER_NAME )
    , mReceivedBlocks           ( 0u )
    , mLastReceivedBlocks       ( 0u )
    , mCompleteFrames           ( 0u )
    , mIncompleteFrames         ( 0u )
    , mLastIncompleteFrames     ( 0u )
    , mFrameStats               ( )
    , mMissStats                ( )
{
}

void ServiceClient::startup_component(areg::ComponentThread& /* comThread */)
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, startup_component );
    LOG_WARN("The component [ %s ] has been started", role_name().as_string());

    areg::Application::enable_data_rate(true);
    areg::DataLiteral dataRate = areg::conv_data_size(0u);
    areg::ext::Console& console = areg::ext::Console::instance();
    console.clear_current_line();
    console.output_txt(COORD_TITLE,      MSG_APP_TITLE);
    console.output_txt(COORD_SEP1,       MSG_SEPARATOR);
    console.output_msg(COORD_DATA_RATE,  MSG_NET_RATE_SENT.data(), dataRate.first, dataRate.second.data(), 0u);
    console.output_msg(COORD_RECV_COUNT, MSG_RECV_COUNT.data(), 0u, 0u);
#if DO_STATS
    console.output_msg(COORD_DATA_STAT,  MSG_STATS_RATE.data(), 0u, 0u);
#ifdef DEBUG
    console.output_msg(COORD_MISS_DETAIL, MSG_MISS_DETAIL.data(), 0u, 0u);
#endif // DEBUG
#endif // DO_STATS
    console.output_txt(COORD_SEP2,       MSG_SEPARATOR);

    console.set_cursor_cur_position(COORD_CURSOR);
    console.refresh_screen();

    mFrameStats.clear();
    mMissStats.clear();
}

DEBUG_DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);
void ServiceClient::broadcast_image_block_acquired([[maybe_unused]] const areg::SharedBuffer& imageBlock)
{
    // Always count every delivered block, independent of DO_STATS. A non-zero per-second rate
    // here proves the wire envelope is correctly addressed and the broadcast callback fires.
    ++mReceivedBlocks;

#if DO_STATS
    DEBUG_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);

    const uint8_t* raw = imageBlock.buffer();
    if ((raw == nullptr) || (imageBlock.size_used() < sizeof(LargeData::RawImageBlock)) || mFrameStats.empty())
    {
        DEBUG_LOG_ERR("Discarding block: invalid buffer or uninitialized statistics");
        return;
    }

    const LargeData::RawImageBlock* block = reinterpret_cast<const LargeData::RawImageBlock*>(raw);
    if (block->channelId >= mFrameStats.size())
    {
        DEBUG_LOG_ERR("Invalid channel ID [ %u ], max allowed [ %zu ]", block->channelId, mFrameStats.size() - 1u);
        return;
    }

    FrameStats& stat = mFrameStats[block->channelId];
    const uint32_t seq  { block->frameSeqId };
    const uint32_t posY { static_cast<uint32_t>(block->imageData.imgStartPos.posY) };
    const uint32_t hgt  { block->imageData.imgHeight };

    // The frame geometry must match the announced ImageGenSetting; otherwise resynchronise.
    if (block->frameHeight != stat.height)
    {
        ++mMissStats.seqErrors;
        DEBUG_LOG_ERR("ch=%u height: exp=%u got=%u", block->channelId, stat.height, block->frameHeight);
        stat.started = false;
        return;
    }

    if (!stat.started)
    {
        // Lock onto the first clean frame start (posY == 0); skip a leading partial frame.
        if (posY != 0u)
            return;

        stat.started = true;
        stat.curSeq  = seq;
        stat.nextY   = hgt;
    }
    else if (posY == 0u)
    {
        // A new frame begins: judge the frame that just ended.
        if (stat.nextY == stat.height)
        {
            ++mCompleteFrames;
        }
        else
        {
            ++mIncompleteFrames;
            DEBUG_LOG_ERR("ch=%u seqNr=%u: frame incomplete, assembled %u of %u lines", block->channelId, stat.curSeq, stat.nextY, stat.height);
        }

        // frameSeqId must increase by exactly one from one frame to the next.
        if (seq != stat.curSeq + 1u)
        {
            ++mMissStats.seqErrors;
            DEBUG_LOG_ERR("ch=%u seqN: exp=%u got=%u", block->channelId, stat.curSeq + 1u, seq);
        }

        stat.curSeq = seq;
        stat.nextY  = hgt;
    }
    else
    {
        // Continuation block of the current frame.
        if (seq != stat.curSeq)
        {
            ++mMissStats.seqErrors;     // frame id changed without a posY == 0 start
            DEBUG_LOG_ERR("ch=%u seqN mid-frame: exp=%u got=%u", block->channelId, stat.curSeq, seq);
            stat.curSeq = seq;
        }

        if (posY != stat.nextY)
        {
            ++mMissStats.lineErrors;    // gap, overlap or out-of-order block
            DEBUG_LOG_ERR("ch=%u line: exp=%u got=%u seqNr=%u", block->channelId, stat.nextY, posY, seq);
        }

        stat.nextY = posY + hgt;        // resync to the actual position to avoid cascading errors
    }
#endif  // DO_STATS
}

void ServiceClient::broadcast_service_stopping()
{
    LOG_SCOPE( examples_23_clientdatarate_ServiceClient, broadcast_service_stopping );
    LOG_WARN("Service stopped, quit application");

    mTimer.stop_timer();
    areg::Application::signal_quit();
}

DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, on_image_gen_setting_update);
void ServiceClient::on_image_gen_setting_update(const LargeData::ImageGenerator& ImageGenSetting, areg::DataState state)
{
    LOG_SCOPE(examples_23_clientdatarate_ServiceClient, on_image_gen_setting_update);
    if (state != areg::DataState::DataIsOK)
    {
        mFrameStats.clear();
        return;
    }

    if (ImageGenSetting.channels > LargeData::MAX_CHANNELS)
    {
        LOG_WARN("Ignoring image settings with invalid channel count [ %u ], max allowed [ %u ]", ImageGenSetting.channels, LargeData::MAX_CHANNELS);
        return;
    }
    else if (ImageGenSetting.channels == 0)
    {
        LOG_WARN("Release frame statistics");
        mFrameStats.clear();
        return;
    }

    mFrameStats.clear();
    mFrameStats.resize(ImageGenSetting.channels);
    for (uint32_t i = 0; i < ImageGenSetting.channels; ++i)
    {
        mFrameStats[i] = FrameStats{ ImageGenSetting.height, 0u, 0u, false };
    }

    mReceivedBlocks         = 0u;
    mLastReceivedBlocks     = 0u;
    mCompleteFrames         = 0u;
    mIncompleteFrames       = 0u;
    mLastIncompleteFrames   = 0u;
    mMissStats.clear();
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
    notify_on_image_gen_setting_update(isConnected);
    notify_on_broadcast_service_stopping(isConnected);
    notify_on_broadcast_image_block_acquired(isConnected);

    if (isConnected)
    {
        mReceivedBlocks         = 0u;
        mLastReceivedBlocks     = 0u;
        mCompleteFrames         = 0u;
        mIncompleteFrames       = 0u;
        mLastIncompleteFrames   = 0u;
        mFrameStats.clear();
        mMissStats.clear();
        mTimer.start_timer(LargeData::TIMER_TIMEOUT, areg::Timer::CONTINUOUSLY);
    }
    else
    {
        mTimer.stop_timer();
        areg::Application::signal_quit();
    }

    return result;
}

DEBUG_DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, process_timer);
void ServiceClient::process_timer(areg::Timer& /* timer */)
{
    DEBUG_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, process_timer);

    // Snapshot the always-on received-block counter and derive the per-second rate.
    const uint32_t totalRecv = mReceivedBlocks;
    const uint32_t deltaRecv = totalRecv - mLastReceivedBlocks;
    mLastReceivedBlocks = totalRecv;

#if DO_STATS
    const uint32_t snapOk   = mCompleteFrames;
    const uint32_t snapMiss = mIncompleteFrames;
    const MissStats snapMs  = mMissStats;

    const uint32_t deltaMiss = snapMiss - mLastIncompleteFrames;
    mLastIncompleteFrames = snapMiss;
    mMissStats.clear();

    if ((deltaMiss > 0u) || (snapMs.seqErrors > 0u) || (snapMs.lineErrors > 0u))
    {
        DEBUG_LOG_WARN("Frame stats/sec: complete=%u incomplete=%u (new=%u) | seqErr=%u lineErr=%u",
                            snapOk, snapMiss, deltaMiss,
                            snapMs.seqErrors, snapMs.lineErrors);
    }
#endif  // DO_STATS

    uint64_t sizeRecv{ 0u };
    uint32_t msgRecv{ 0u };

    areg::Application::query_data_received(sizeRecv, msgRecv);
    areg::ext::Console& console = areg::ext::Console::instance();
    areg::DataLiteral dataRate = areg::conv_data_size( sizeRecv );

    console.save_cursor_position();
    console.output_msg(COORD_DATA_RATE,   MSG_NET_RATE_SENT.data(), dataRate.first, dataRate.second.data(), msgRecv);
    console.output_msg(COORD_RECV_COUNT,  MSG_RECV_COUNT.data(), totalRecv, deltaRecv);
#if DO_STATS
    console.output_msg(COORD_DATA_STAT,   MSG_STATS_RATE.data(),   snapOk, snapMiss);
#ifdef DEBUG
    console.output_msg(COORD_MISS_DETAIL, MSG_MISS_DETAIL.data(), snapMs.seqErrors, snapMs.lineErrors);
#endif // DEBUG
#endif  // DO_STATS
    console.restore_cursor_position();
    console.refresh_screen();
}
