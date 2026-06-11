#pragma once

/************************************************************************
 * \file        locservice/src/ServiceClient.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              This file contains simple implementation of service client to
 *              request message output
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/component/Component.hpp"
#include "examples/23_pubdatarate/services/LargeDataConsumerBase.hpp"
#include "areg/component/TimerConsumer.hpp"

#include "areg/component/Timer.hpp"
#include "aregextend/console/Console.hpp"

#include "common/LargeDataDefs.hpp"

#include <array>

//////////////////////////////////////////////////////////////////////////
// ServicingComponent class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Service client, connects to the large data service to receive
 *          generated image data. It outputs the statistics of received
 *          data, item blocks, and frame completeness without rebuilding
 *          the full image payload in the hot path.
 **/
class ServiceClient final : public    areg::Component
                          , protected LargeDataConsumerBase
                          , protected areg::TimerConsumer
{
private:

    //!< The coordinates to output client application title / headline.
    static constexpr areg::ext::Console::Coord     COORD_TITLE      { 1, 2 };

    //!< Coordinates to output a long separator line below the title
    static constexpr areg::ext::Console::Coord     COORD_SEP1       { 1, 3 };
    //!< Coordinates to output data rate information of large data client.
    static constexpr areg::ext::Console::Coord     COORD_DATA_RATE  { 1, 5 };
    //!< Coordinates to output the application-level count of received image blocks (always shown).
    static constexpr areg::ext::Console::Coord     COORD_RECV_COUNT { 1, 6 };
    //!< Coordinates to output data rate information of large data client.
    static constexpr areg::ext::Console::Coord     COORD_DATA_STAT  { 1, 7 };
    //!< Coordinates to output per-second miss-type breakdown.
    static constexpr areg::ext::Console::Coord     COORD_MISS_DETAIL{ 1, 8 };
    static constexpr areg::ext::Console::Coord     COORD_SEP2       { 1, 9 };

    //!< Cursor resting position: one row below the last output line (no user input needed).
    static constexpr areg::ext::Console::Coord     COORD_CURSOR     { 1, 10 };

    //!< Timer name.
    static constexpr std::string_view   TIMER_NAME      { "DataRateTimer" };

    //!< Message to output as application title / headline
    static constexpr std::string_view   MSG_APP_TITLE       { " Application to test data rate, Service Consumer part..." };
    //!< Long separator drawn below the title
    static constexpr std::string_view   MSG_SEPARATOR       { " --------------------------------------------------------------------------------------------" };
    //!< The message to output network communication rate.
    static constexpr std::string_view   MSG_NET_RATE_SENT   { " Network receive rate ..: data      [ %8.2f ] %s / sec, message [ %u ] blocks/sec." };
    //!< The message to output the application-level received-block count (proves the broadcast callback fires).
    static constexpr std::string_view   MSG_RECV_COUNT      { " Image blocks received .: total     [ %10u ] blocks, [ %u ] blocks/sec." };
    //!< The message to output frame-assembly statistics (cumulative totals).
    static constexpr std::string_view   MSG_STATS_RATE      { " Frame stats on receive : complete  [ %u ], incomplete [ %u ]." };
    //!< Per-second sequencing-error breakdown; values reset each timer tick.
    static constexpr std::string_view   MSG_MISS_DETAIL     { " Sequencing errors /sec : seqErr=%-7u lineErr=%-7u" };

    /**
     * \brief   Per-channel frame-assembly state. Fixed-size: zero heap allocation in the
     *          hot path. Uses only block metadata -- no pixel data accessed.
     *
     *          A frame is a contiguous run of blocks covering posY 0 .. height (by imgHeight),
     *          all carrying the same frameSeqId; frameSeqId increases by one for the next frame.
     **/
    struct FrameStats
    {
        uint32_t    height  { 0u };     //!< Expected total frame height in lines.
        uint32_t    curSeq  { 0u };     //!< frameSeqId of the frame currently being assembled.
        uint32_t    nextY   { 0u };     //!< posY expected for the next block of the current frame.
        bool        started { false };  //!< True once locked onto a clean frame start (posY == 0).
    };

    using Stats = std::vector<FrameStats>;

    /**
     * \brief   Per-second sequencing-error breakdown. Reset on every timer tick so the
     *          console shows a rate (errors/sec) rather than a cumulative total.
     **/
    struct MissStats
    {
        uint32_t seqErrors  { 0u }; //!< frameSeqId not contiguous across frames, or changed mid-frame.
        uint32_t lineErrors { 0u }; //!< Block posY gap / overlap / out-of-order within a frame.

        void clear() noexcept { seqErrors = lineErrors = 0u; }
    };
//////////////////////////////////////////////////////////////////////////
// Constructor / destructor
//////////////////////////////////////////////////////////////////////////
public:
    ServiceClient(const areg::ComponentEntry & entry, areg::ComponentThread & owner);

protected:

/************************************************************************/
// Component overrides
/************************************************************************/
    /**
     * \brief	This function is triggered by component thread when it 
     *          requires component to start up. Set listeners and make
     *          initialization in this function call.
     * \param	comThread	The component thread, which triggered startup command
     **/
    void startup_component( areg::ComponentThread & comThread ) final;

    /**
     * \brief   Server broadcast.
     *          Called to notify all connected clients that new image data is available.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     * \param   imageBlock  Acquired image block.
     **/
    void broadcast_image_block_acquired( const areg::SharedBuffer & imageBlock ) final;

    /**
     * \brief   Server broadcast.
     *          Called to notify all clients that service is shutting down.
     *          Overwrite, if need to handle Broadcast call of server object.
     *          This call will be automatically triggered, on every appropriate request call
     **/
    void broadcast_service_stopping() final;

    /**
     * \brief   Triggered when the ImageGenSetting attribute is updated. Contains the
     *          attribute value and validation flag. When notification is enabled,
     *          override this method in the derived class.
     *          Attribute ImageGenSetting description:
     *          Image Generator Settings.
     * \param   ImageGenSetting The value of the ImageGenSetting attribute.
     * \param   state           The data validation flag.
     **/
    void on_image_gen_setting_update(const LargeData::ImageGenerator& ImageGenSetting, areg::DataState state) final;

/************************************************************************/
// ProxyListener Overrides
/************************************************************************/
    /**
     * \brief   Triggered when receives service provider connected / disconnected event.
     *          When the service provider is connected, the client objects can set the listeners here.
     *          When the service provider is disconnected, the client object should clean the listeners.
     *          Up from connected status, the clients can subscribe and unsubscribe on updates,
     *          responses and broadcasts, can trigger requests. Before connection, the clients cannot
     *          neither trigger requests, nor receive data update messages.
     * \param   status  The service connection status.
     * \param   proxy   The Service Interface Proxy object, which is notifying service connection.
     * \return  Return true if this service connect notification was relevant to client object.
     **/
    bool service_connected( areg::ServiceConnectionState status, areg::ProxyBase & proxy ) final;

/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    void process_timer( areg::Timer & timer ) final;

//////////////////////////////////////////////////////////////////////////
// member variables
//////////////////////////////////////////////////////////////////////////
private:
    areg::Timer mTimer;                  //!< Timer to output message.
    uint32_t    mReceivedBlocks;         //!< Cumulative count of image blocks delivered to the broadcast callback.
    uint32_t    mLastReceivedBlocks;     //!< Value of mReceivedBlocks at the previous timer tick (used for per-second delta).
    uint32_t    mCompleteFrames;         //!< Cumulative complete frame count.
    uint32_t    mIncompleteFrames;       //!< Cumulative incomplete frame count.
    uint32_t    mLastIncompleteFrames;   //!< Value of mIncompleteFrames at the previous timer tick (used for per-second delta).
    Stats       mFrameStats;             //!< Per-channel frame tracking.
    MissStats   mMissStats;              //!< Per-second sequencing-error breakdown (reset each timer tick).

//////////////////////////////////////////////////////////////////////////
// hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    inline ServiceClient & self();

//////////////////////////////////////////////////////////////////////////
// forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceClient() = delete;
    AREG_NOCOPY_NOMOVE( ServiceClient );
};

inline ServiceClient & ServiceClient::self()
{
    return (*this);
}
