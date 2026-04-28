#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/service/private/PoolSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, pool send thread serving a set of clients.
 *              One instance is created per pool slot; it resolves the target
 *              socket at send time via client_by_cookie() so it can serve
 *              multiple clients simultaneously. Stats are accumulated into
 *              the global ServerSendThread so DataRateHelper sees correct totals.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/SendMessageEvent.hpp"

#include <atomic>
#include <string_view>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
} // namespace areg

namespace areg::ext {
    class ServerConnection;
    class ServerSendThread;
    class ClientConnectionPair;
} // namespace areg::ext

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// PoolSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Pool send thread: serves all clients routed to this pool slot.
 *          The target socket is resolved on each send by cookie lookup, so
 *          no per-client state is stored in the thread. Stats are forwarded
 *          to the global ServerSendThread for DataRateHelper aggregation.
 **/
class PoolSendThread final  : public    DispatcherThread
                            , public    SendMessageEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    using BatchEntries = std::array<areg::PendingSend, areg::THREAD_DRAIN_LIMIT>;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a pool send thread.
     *
     * \param   remoteService   Remote message handler (for failure callbacks).
     * \param   connection      Server connection object (owns the socket map).
     * \param   globalStats     The global send thread whose atomic counters accumulate
     *                          the bytes/messages contributed by this thread.
     * \param   threadName      Unique name for this dispatcher thread.
     **/
    PoolSendThread( ClientConnectionPair & owner
                    , areg::RemoteMessageHandler & remoteService
                    , ServerConnection & connection
                    , ServerSendThread & globalStats
                    , std::string_view threadName );

    virtual ~PoolSendThread() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns and resets the accumulated sent byte count.
     **/
    [[nodiscard]]
    inline uint64_t bytes_sent() const noexcept;

    /**
     * \brief   Returns and resets the accumulated sent message count.
     **/
    [[nodiscard]]
    inline uint32_t messages_sent() const noexcept;

    /**
     * \brief   Enables or disables per-thread data rate tracking.
     *          Resetting counters ensures stale data does not contaminate later queries.
     *
     * \param   enable  True to enable tracking; false to disable.
     **/
    inline void set_data_rate_enabled(bool enable) noexcept;

    /**
     * \brief   Returns whether per-thread data rate tracking is enabled.
     **/
    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

    inline void data_stat(uint64_t& byteSent, uint32_t& msgSent) const noexcept;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Registers or unregisters this thread as a SendMessageEvent consumer and
     *          enables / disables the inherited DispatcherThread event loop.
     *
     * \param   is_ready    True when the thread is ready to receive events; false on shutdown.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter overrides
/************************************************************************/

    /**
     * \brief   Accepts only SendMessageEvent; all others are silently destroyed.
     *
     * \param   eventElem   Event to post.
     * \return  Returns true if the event was accepted and queued.
     **/
    [[nodiscard]]
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// SendMessageEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Dispatches one send event: resolves the target socket by cookie,
     *          sends the message, and updates counters.
     *
     * \param   data    Event payload carrying the remote message to send.
     **/
    void process_event( const SendMessageEventData & data ) final;

    /**
     * \brief   Resolves the target socket via client_by_cookie(), sends a single
     *          message, and updates byte/message counters on success.
     *          Calls failed_send_message() on the remote service on failure.
     *
     * \param   msg     Remote message to send.
     * \return  Returns true on success or if the target is no longer connected.
     *          Returns false on a send error that should abort the drain loop.
     **/
    bool _do_send( const areg::RemoteMessage & msg );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientConnectionPair&       mOwner;         //!< Reference to the owning connection pair for socket lookup.
    areg::RemoteMessageHandler& mRemoteService; //!< Failure callbacks.
    ServerConnection &          mConnection;    //!< Server connection (socket lookup + send API).
    ServerSendThread &          mGlobalStats;   //!< Global counters accumulated here.
    BatchEntries                mBatch;         //!< Batching list

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    PoolSendThread() = delete;
    AREG_NOCOPY_NOMOVE( PoolSendThread );
};

//////////////////////////////////////////////////////////////////////////
// PoolSendThread inline methods
//////////////////////////////////////////////////////////////////////////

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
