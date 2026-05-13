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
    //!< Coordinates to output data rate information of large data client.
    static constexpr areg::ext::Console::Coord     COORD_DATA_STAT  { 1, 7 };
    static constexpr areg::ext::Console::Coord     COORD_SEP2       { 1, 8 };

    //!< Cursor resting position: one row below the last output line (no user input needed).
    static constexpr areg::ext::Console::Coord     COORD_CURSOR     { 1, 9 };

    //!< Timer name.
    static constexpr std::string_view   TIMER_NAME      { "DataRateTimer" };

    //!< Message to output as application title / headline
    static constexpr std::string_view   MSG_APP_TITLE       { " Application to test data rate, Service Consumer part..." };
    //!< Long separator drawn below the title
    static constexpr std::string_view   MSG_SEPARATOR       { " --------------------------------------------------------------------------------------------" };
    //!< The message to output network communication rate.
    static constexpr std::string_view   MSG_NET_RATE_SENT   { " Network receive rate ..: data      [ %8.2f ] %s / sec, message [ %u ] blocks/sec." };
    //!< The message to output on-time / late delivery statistics
    static constexpr std::string_view   MSG_STATS_RATE      { " Stats on data receive .: OK frames [ %u ], MISS frames [ %u ]." };

    enum FrameState : int32_t
    {
          Unknown       = -1
        , Initial       =  0
        , Pending       =  1
        , Missing       =  2
        , Complete      =  3
    };

    /**
     * \brief   Per-channel frame tracking state. Fixed-size: zero heap allocation in
     *          the hot path. Uses only block metadata -- no pixel data accessed.
     **/
    struct FrameStats
    {
        int32_t     channelId   { -1 };
        FrameState  state       { Unknown };
        int32_t     height      { -1 };
        int32_t     line        { -1 };
        uint32_t    seqNr       { 0u };
    };

    using Stats = std::vector<FrameStats>;
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
     * \brief   Service provider broadcast.
     *          Triggered when image settings are changed
     *          Override this method to handle the Broadcast call from the service provider.
     *          This call is automatically triggered on every appropriate broadcast event.
     * \param   width       width of image
     * \param   height      hight of image
     * \param   pixelTime   The pixel time in nanoseconds. Value 0 means non-stop streaming.
     * \param   linesBlock  lines in block, maximum should be equal to `height`, minimum is 1
     * \param   channels    number of channels
     **/
    void broadcast_image_settings( uint32_t width, uint32_t height, uint32_t pixelTime, uint32_t linesBlock, uint32_t channels ) final;

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
    areg::Timer mTimer;             //!< Timer to output message.
    uint32_t    mCompleteFrames;    //!< Complete frame counters.
    uint32_t    mIncompleteFrames;  //!< Incomplete frame counters.
    Stats       mFrameStats;        //!< Per-channel frame tracking.

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
