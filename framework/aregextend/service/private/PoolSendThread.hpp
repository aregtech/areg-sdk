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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/EventConsumer.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

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
                            , public    areg::EventConsumer
                            , private   SendBacklog::Owner
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:
    using BatchEntries = std::array<areg::ext::PendingSend, areg::DEFAULT_DRAIN_LIMIT>;

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

/************************************************************************/
// Actions and attributes.
/************************************************************************/
public:
    /**
     * \brief   Returns the send batch limit of this thread, in messages. Resolved once, when the
     *          thread becomes ready, so reading it costs one load.
     **/
    [[nodiscard]]
    inline uint32_t drain_limit() const noexcept;

    /**
     * \brief   Returns the gate of the send queue of this thread. A producer that wants to write
     *          a message into the socket itself must find the gate clear first, and must announce
     *          the message with SendQueueGate::enter() when it queues it instead.
     **/
    [[nodiscard]]
    inline areg::SendQueueGate & send_gate() noexcept;

    /**
     * \brief   Hands one outbound message to this send thread and reports whether the queue took it.
     *          When it returns false the message never reaches the socket, so the caller must
     *          release what it reserved for it.
     *
     * \param   eventElem   The event to queue. Its target dispatcher must already be this thread.
     * \return  true if the queue took the event, false if it did not.
     **/
    inline bool queue_message( areg::Event & eventElem );

    /**
     * \brief   Returns the writer of this thread.
     **/
    [[nodiscard]]
    inline SendBacklog & backlog() noexcept;

    /**
     * \brief   Returns true if a message or the exit waits in the queue.
     **/
    [[nodiscard]]
    inline bool has_queued_events() const noexcept;

    /**
     * \brief   Waits up to \a timeoutMs for a message in the queue.
     **/
    inline void wait_queued_events(uint32_t timeoutMs) noexcept;

    /**
     * \brief   Returns true if the thread is asked to exit.
     **/
    [[nodiscard]]
    inline bool is_exit_requested() const noexcept;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables / disables the inherited DispatcherThread event loop.
     *
     * \param   is_ready    True when the thread is ready to receive events; false on shutdown.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter overrides
/************************************************************************/

    /**
     * \brief   Posts the event to this thread's queue.
     *
     * \param   eventElem   Event to post.
     * \return  Returns true if the event was accepted and queued.
     **/
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// EventConsumer interface override.
/************************************************************************/

    /**
     * \brief   Receives IPC outbound events and exit signals dispatched to this send thread.
     *          Zeros internal1/internal2/custom before wire transmission.
     *          Exits on is_exit_prio().
     **/
    void start_event_processing( areg::Event & eventElem ) final;

// SendBacklog::Owner overrides

    SOCKETHANDLE backlog_socket(ITEM_ID cookie) final;

    void backlog_sent(uint64_t bytes, uint32_t msgs) final;

    void backlog_close(const SendBacklog::Entry & entry, SendBacklog::Reason reason) final;

    //!< Releases the backlog and closes every socket whose message was cut.
    void release_backlog();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    ClientConnectionPair&       mOwner;         //!< Reference to the owning connection pair for socket lookup.
    areg::RemoteMessageHandler& mRemoteService; //!< Failure callbacks.
    ServerConnection &          mConnection;    //!< Server connection (socket lookup + send API).
    ServerSendThread &          mGlobalStats;   //!< Global counters accumulated here.

    /**
     * \brief   How many messages this thread may put into one batch, within the range
     *          1 .. areg::DEFAULT_DRAIN_LIMIT. Resolved when the thread becomes ready.
     **/
    uint32_t                    mDrainLimit;
    BatchEntries                mBatch;         //!< Pre-allocated batch work list reused each drain cycle.
    //!< Reused scratch: per-slot target cookies and resolved socket handles (POD; off the stack).
    std::array<ITEM_ID, areg::DEFAULT_DRAIN_LIMIT>       mTargets;
    std::array<SOCKETHANDLE, areg::DEFAULT_DRAIN_LIMIT>  mSockets;
    //!< Reusable single-window drain buffer (pop_events); constructed once.
    std::array<areg::Event, areg::DEFAULT_DRAIN_LIMIT>   mEvents;
    //!< Tells the producers whether this queue still owes a message to a socket.
    areg::SendQueueGate                                  mSendGate;
    //!< Keeps what a socket cannot take now. Declared after the gate it enters.
    SendBacklog                                          mBacklog;

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

inline uint32_t PoolSendThread::drain_limit() const noexcept
{
    return mDrainLimit;
}

inline areg::SendQueueGate & PoolSendThread::send_gate() noexcept
{
    return mSendGate;
}

inline bool PoolSendThread::queue_message( areg::Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

inline SendBacklog & PoolSendThread::backlog() noexcept
{
    return mBacklog;
}

inline bool PoolSendThread::has_queued_events() const noexcept
{
    return mExternalEvents.has_pending();
}

inline void PoolSendThread::wait_queued_events(uint32_t timeoutMs) noexcept
{
    static_cast<void>(mExternalEvents.wait_event(timeoutMs));
}

inline bool PoolSendThread::is_exit_requested() const noexcept
{
    return mExternalEvents.is_exit_triggered();
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_POOLSENDTHREAD_HPP
