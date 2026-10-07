#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/ServerSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Service connectivity server send message thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"
#include "areg/ipc/DataRateStats.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

#include <array>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
} // namespace areg

namespace areg::ext {
    class ServerConnection;
} // namespace areg::ext

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The IPC message sender thread.
 **/
class ServerSendThread final    : public    DispatcherThread
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
     * \brief   Initializes connection servicing handler and server connection objects.
     *
     * \param   remoteService       The instance of remote servicing handle to set.
     * \param   connection          The instance of server socket connection object.
     **/
    ServerSendThread(RemoteMessageHandler& remoteService, ServerConnection & connection );

    virtual ~ServerSendThread() = default;

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
     * \brief   Returns accumulative value of sent data size and resets the existing value to zero.
     *          The operations are atomic. The value can be used to display data rate, for example.
     **/
    [[nodiscard]]
    inline uint64_t extract_data_send() const noexcept;

    /**
     * \brief   Returns accumulative count of sent messages and resets the existing value to zero.
     *          The operations are atomic.
     **/
    [[nodiscard]]
    inline uint32_t extract_msgs_sent() const noexcept;

    /**
     * \brief   Call to enable or disable the received data calculation. It also resets the existing
     *          calculated data.
     *
     * \param   enable      Flag, indicating whether data calculation is enabled or not.
     **/
    inline void set_data_rate_enabled(bool enable) noexcept;

    [[nodiscard]]
    inline bool is_data_rate_enabled() const noexcept;

    /**
     * \brief   Accumulates bytes and message counts from a per-client send thread into the
     *          global counters queried by DataRateHelper. Called by PoolSendThread when
     *          mSaveDataSend is enabled. Thread-safe: uses atomic add.
     *
     * \param   bytes   Number of bytes sent.
     * \param   msgs    Number of messages sent.
     **/
    inline void accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept;

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
     * \brief   Blocks in the backlog wait for up to \a timeoutMs: until a socket with a backlog
     *          takes data, an exit, or, if \a takeEvents is true, a queued message.
     **/
    inline void wait_backlog(bool takeEvents, uint32_t timeoutMs);

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
     * \brief   Call to enable or disable event dispatching threads to receive events. Override if
     *          need to make event dispatching preparation job.
     *
     * \param   is_ready    The flag to indicate whether the dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter class overrides
/************************************************************************/

    /**
     * \brief   Posts event and delivers to its target. Since the Dispatcher Thread is a Base object
     *          for Worker and Component threads, it does nothing and only destroys event object
     *          without processing. Override this method or use Worker / Component thread.
     *
     * \param   eventElem       Event object to post
     * \return  In this class it always returns true.
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

/************************************************************************/
// SendBacklog::Owner overrides
/************************************************************************/

    SOCKETHANDLE backlog_socket(ITEM_ID cookie) final;

    void backlog_sent(uint64_t bytes, uint32_t msgs) final;

    void backlog_close(const SendBacklog::Entry & entry, SendBacklog::Reason reason) final;

    /**
     * \brief   Releases the backlog and closes every socket whose message was cut.
     **/
    void release_backlog();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote servicing interface object
     **/
    RemoteMessageHandler&           mRemoteService;
    /**
     * \brief   The instance of server connection object
     **/
    ServerConnection &              mConnection;
    /**
     * \brief   How many messages this thread may put into one batch, within the range
     *          1 .. areg::DEFAULT_DRAIN_LIMIT. Resolved when the thread becomes ready.
     **/
    uint32_t                        mDrainLimit;
    /**
     * \brief   Pre-allocated batch work list reused across every drain cycle.
     **/
    BatchEntries                    mBatch;
    //!< Reused scratch: per-slot target cookies and resolved socket handles (POD; off the stack).
    std::array<ITEM_ID, areg::DEFAULT_DRAIN_LIMIT>       mTargets;
    std::array<SOCKETHANDLE, areg::DEFAULT_DRAIN_LIMIT>  mSockets;
    //!< Reusable single-window drain buffer (pop_events); constructed once.
    std::array<areg::Event, areg::DEFAULT_DRAIN_LIMIT>   mEvents;
    /**
     * \brief   Atomic stats (bytes + messages sent + enabled flag).
     **/
    DataRateStats                   mSendStats;
    /**
     * \brief   Tells the producers whether this queue still owes a message to a socket.
     **/
    areg::SendQueueGate             mSendGate;
    /**
     * \brief   Keeps what a socket cannot take now. Declared after the gate it enters.
     **/
    SendBacklog                     mBacklog;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServerSendThread() = delete;
    AREG_NOCOPY_NOMOVE( ServerSendThread );
};

//////////////////////////////////////////////////////////////////////////
// ServerSendThread class inline methods
//////////////////////////////////////////////////////////////////////////

inline uint64_t ServerSendThread::extract_data_send() const noexcept
{
    return mSendStats.extract_bytes();
}

inline uint32_t ServerSendThread::extract_msgs_sent() const noexcept
{
    return mSendStats.extract_msgs();
}

inline void ServerSendThread::set_data_rate_enabled(bool enable) noexcept
{
    mSendStats.set_enabled(enable);
}

inline bool ServerSendThread::is_data_rate_enabled() const noexcept
{
    return mSendStats.is_enabled();
}

inline void ServerSendThread::accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept
{
    mSendStats.accumulate(bytes, msgs);
}

inline uint32_t ServerSendThread::drain_limit() const noexcept
{
    return mDrainLimit;
}

inline areg::SendQueueGate & ServerSendThread::send_gate() noexcept
{
    return mSendGate;
}

inline bool ServerSendThread::queue_message( areg::Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

inline SendBacklog & ServerSendThread::backlog() noexcept
{
    return mBacklog;
}

inline bool ServerSendThread::has_queued_events() const noexcept
{
    return mExternalEvents.has_pending();
}

inline void ServerSendThread::wait_backlog(bool takeEvents, uint32_t timeoutMs)
{
    mBacklog.reset_wake();
    if ( mExternalEvents.arm_waiter(mBacklog, takeEvents) )
    {
        mBacklog.wait(timeoutMs);
    }

    mExternalEvents.disarm_waiter();
}

inline bool ServerSendThread::is_exit_requested() const noexcept
{
    return mExternalEvents.is_exit_triggered();
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVERSENDTHREAD_HPP
