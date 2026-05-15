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
#if DO_STATS
    console.output_msg(COORD_DATA_STAT,  MSG_STATS_RATE.data(), 0u, 0u);
#ifdef DEBUG
    console.output_msg(COORD_MISS_DETAIL, MSG_MISS_DETAIL.data(), 0u, 0u, 0u, 0u);
#endif // DEBUG
#endif // DO_STATS
    console.output_txt(COORD_SEP2,       MSG_SEPARATOR);

    console.set_cursor_cur_position(COORD_CURSOR);
    console.refresh_screen();

    mFrameStats.clear();
    mMissStats.clear();
}

#if DO_STATS
DEBUG_DEF_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);
void ServiceClient::broadcast_image_block_acquired(const areg::SharedBuffer& imageBlock)
{
    DEBUG_LOG_SCOPE(examples_23_clientdatarate_ServiceClient, broadcast_image_block_acquired);

    const uint8_t* raw = imageBlock.buffer();
    const uint32_t imgSize = imageBlock.size_used();  // equals blockSize
    if (raw == nullptr || imgSize < sizeof(LargeData::RawImageBlock))
    {
        DEBUG_LOG_ERR("Invalid buffer data");
        ++mIncompleteFrames;
        return;
    }

    if (mFrameStats.empty())
    {
        DEBUG_LOG_ERR("The frame statistics is not initialized, cannot continue");
        ++mIncompleteFrames;
        return;
    }

    const LargeData::RawImageBlock* block = reinterpret_cast<const LargeData::RawImageBlock*>(raw);
    if (block->channelId >= static_cast<uint32_t>(mFrameStats.size()))
    {
        DEBUG_LOG_ERR("Invalid channel ID [ %u ], it cannot be bigger than [ %u ]", block->channelId, static_cast<uint32_t>(mFrameStats.size() - 1));
        ++mIncompleteFrames;
        return;
    }

    bool ok{ true };
    FrameStats& stat = mFrameStats[block->channelId];
    if (stat.height != static_cast<int32_t>(block->frameHeight))
    {
        ok = false;
        DEBUG_LOG_ERR("ch=%u height: exp=%d got=%u", block->channelId, stat.height, block->frameHeight);
        return;
    }

    switch (stat.state)
    {
    case Complete:              // fall through
        // Mirror the server's frame_id = (frame_id + 1) % height cycling.
        {
#if AREG_LOG_DEBUG
            const uint32_t wrap = static_cast<uint32_t>(stat.height > 0 ? stat.height : 1);
#endif // AREG_LOG_DEBUG
            stat.seqNr = stat.seqNr + 1u;
        }
        stat.line   = 0;
        stat.state  = Pending;
    case Pending:               // fall through
    case Missing:
        if (stat.seqNr != block->frameSeqId)
        {
            ++mMissStats.seqMismatch;
            if (stat.line > 0 && stat.line < stat.height)
            {
                // Previous frame abandoned mid-way; count it as incomplete.
                ++mIncompleteFrames;
                ++mMissStats.abandoned;
                DEBUG_LOG_ERR("ch=%u aband: seqNr=%u at=%u/%u -> new=%u posY=%u"
                                , block->channelId
                                , stat.seqNr
                                , static_cast<uint32_t>(stat.line)
                                , static_cast<uint32_t>(stat.height)
                                , block->frameSeqId
                                , static_cast<uint32_t>(block->imageData.imgStartPos.posY));
            }
            else
#if AREG_LOG_DEBUG
            {
                const int32_t gap = static_cast<int32_t>(block->frameSeqId) - static_cast<int32_t>(stat.seqNr);
                DEBUG_LOG_ERR("ch=%u seqN: exp=%u got=%u gap=%+d line=%u/%u"
                                , block->channelId
                                , stat.seqNr
                                , block->frameSeqId
                                , gap
                                , static_cast<uint32_t>(stat.line)
                                , static_cast<uint32_t>(stat.height));
            }
#endif // AREG_LOG_DEBUG

            stat.seqNr = block->frameSeqId;
            stat.line  = 0;
            // The new frame is clean only when its first block starts at the top.
            if (block->imageData.imgStartPos.posY != 0)
            {
                ok = false;
            }

            stat.state = ok ? Pending : Missing;
        }
        else if (stat.line != block->imageData.imgStartPos.posY)
        {
            ok = false;
            ++mMissStats.lineMismatch;
            DEBUG_LOG_ERR("ch=%u line: exp=%u got=%u seqNr=%u"
                                , block->channelId
                                , static_cast<uint32_t>(stat.line)
                                , static_cast<uint32_t>(block->imageData.imgStartPos.posY)
                                , block->frameSeqId);
        }

        stat.line = block->imageData.imgStartPos.posY + block->imageData.imgHeight;
        if (stat.line > stat.height)
        {
            ok = false;
            DEBUG_LOG_ERR("ch=%u overflow: line=%u>%u seqNr=%u"
                                , block->channelId
                                , static_cast<uint32_t>(stat.line)
                                , static_cast<uint32_t>(stat.height)
                                , block->frameSeqId);
            stat.line = stat.height;
        }
        break;

    case Initial:
        stat.state  = Pending;
        stat.seqNr  = block->frameSeqId;
        // First block is mid-frame: we already missed blocks, so the frame is incomplete.
        if (block->imageData.imgStartPos.posY != 0)
        {
            ok = false;
            ++mMissStats.initialMid;
            DEBUG_LOG_ERR("ch=%u init: posY=%u seqNr=%u", block->channelId, static_cast<uint32_t>(block->imageData.imgStartPos.posY), block->frameSeqId);
        }
        stat.line = block->imageData.imgStartPos.posY + block->imageData.imgHeight;
        if (stat.line > stat.height)
        {
            ok = false;
            DEBUG_LOG_ERR("ch=%u overflow: line=%u>%u seqNr=%u", block->channelId, static_cast<uint32_t>(stat.line), static_cast<uint32_t>(stat.height), block->frameSeqId);
            stat.line = stat.height;
        }
        break;

    default:
        ASSERT(false);
        DEBUG_LOG_ERR("Undefined State [ %u ]", static_cast<uint32_t>(stat.state));
        ok = false;
    }

    stat.state = ok ? stat.state : Missing;
    if (stat.line == stat.height)
    {
        if (stat.state == Pending)
        {
            ++mCompleteFrames;
        }
        else if (stat.state == Missing)
        {
            DEBUG_LOG_ERR("ch=%u seqNr=%u: frame complete with missing or corrupted blocks", block->channelId, block->frameSeqId);
            ++mIncompleteFrames;
        }

        stat.state = Complete;
    }
}
#else   // !DO_STATS
void ServiceClient::broadcast_image_block_acquired(const areg::SharedBuffer& /*imageBlock*/)
{
}
#endif  // !DO_STATS

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
        FrameStats entry{ static_cast<int32_t>(i), Initial, static_cast<int32_t>(ImageGenSetting.height), 0, 0u };
        mFrameStats[i] = entry;
    }

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

    if (is_connected())
    {
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
#if DO_STATS
    // Snapshot and log BEFORE activating any log scope so these messages are
    // never filtered out by scope configuration regardless of the log settings.
    const uint32_t snapOk   = mCompleteFrames;
    const uint32_t snapMiss = mIncompleteFrames;
    const MissStats snapMs  = mMissStats;

    const uint32_t deltaMiss = snapMiss - mLastIncompleteFrames;
    mLastIncompleteFrames = snapMiss;
    mMissStats.clear();

    if (deltaMiss > 0u || snapMs.seqMismatch > 0u || snapMs.lineMismatch > 0u ||
        snapMs.abandoned > 0u || snapMs.initialMid > 0u)
    {
        DEBUG_LOG_WARN("Stats/sec: OK=%u MISS=%u (new=%u) | seqN=%u line=%u aband=%u init=%u",
                            snapOk, snapMiss, deltaMiss,
                            snapMs.seqMismatch, snapMs.lineMismatch,
                            snapMs.abandoned,   snapMs.initialMid);
    }
#endif  // DO_STATS

    uint64_t sizeRecv{ 0u };
    uint32_t msgRecv{ 0u };

    areg::Application::query_data_received(sizeRecv, msgRecv);
    areg::ext::Console& console = areg::ext::Console::instance();
    areg::DataLiteral dataRate = areg::conv_data_size( sizeRecv );

    console.save_cursor_position();
    console.output_msg(COORD_DATA_RATE,   MSG_NET_RATE_SENT.data(), dataRate.first, dataRate.second.data(), msgRecv);
#if DO_STATS
    console.output_msg(COORD_DATA_STAT,   MSG_STATS_RATE.data(),   snapOk, snapMiss);
#ifdef DEBUG
    console.output_msg(COORD_MISS_DETAIL, MSG_MISS_DETAIL.data(), snapMs.seqMismatch, snapMs.lineMismatch, snapMs.abandoned,   snapMs.initialMid);
#endif // DEBUG
#endif  // DO_STATS
    console.restore_cursor_position();
    console.refresh_screen();
}
