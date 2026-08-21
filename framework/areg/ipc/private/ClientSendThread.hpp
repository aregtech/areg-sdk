#ifndef AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
#define AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/ipc/private/ClientSendThread.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Send Message Thread
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/SocketDefs.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/ipc/DataRateStats.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include <atomic>

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg {
    class RemoteMessageHandler;
    class ClientConnection;
} // namespace areg

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ClientSendThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Message sender thread that queues and sends all messages to remote routing service.
 **/
class ClientSendThread final    : public    DispatcherThread
                                , public    EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes message sender with service handler and client connection.
     *
     * \param   remoteService       Remote service handler for processing messages.
     * \param   connection          Client connection object for sending messages.
     * \param   namePrefix          Prefix for thread name to ensure uniqueness.
     **/
    ClientSendThread(RemoteMessageHandler& remoteService, ClientConnection & connection, const String & namePrefix );
    virtual ~ClientSendThread() = default;

    /************************************************************************/
    // Actions and attributes.
    /************************************************************************/
public:
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
     * \brief   Tells the thread that the connection is being closed on purpose, so a
     *          send that fails from here on is expected and must not be reported.
     *          Reporting it would ask for a reconnect and deliver failure events into
     *          a connection that is already going away. Set from the dispatcher thread
     *          before the drain, read by this thread only when a send fails.
     **/
    inline void set_closing() noexcept;

    /**
     * \brief   Returns the gate of this send queue. A producer thread that wants to write its
     *          message into the socket itself must find the gate clear first, because a clear
     *          gate is the proof that this queue owes no older message to the same socket.
     **/
    [[nodiscard]]
    inline areg::SendQueueGate & send_gate() noexcept;

    /**
     * \brief   Hands one outbound message to this send thread and reports whether the queue took
     *          it. This is the entry point a producer uses instead of Event::deliver_event(),
     *          which discards that answer. A false result means the message was NOT queued and
     *          will never reach the socket, so the caller must release whatever it reserved for
     *          it and report the failure onwards.
     *
     * \param   eventElem   The event to queue. Its target dispatcher must already be this thread.
     * \return  true if the queue took the event, false if it did not.
     **/
    inline bool queue_message( areg::Event & eventElem );

    /**
     * \brief   Returns the send batch limit resolved for this thread, in messages.
     *          A plain member read - the configuration is consulted once, when the thread
     *          becomes ready, never on the message path.
     **/
    [[nodiscard]]
    inline uint32_t drain_limit() const noexcept;

    /**
     * \brief   Reports a message that a producer thread failed to write into the socket itself.
     *          The report is passed to the message handler, exactly as this thread would do for
     *          its own failed batch, and it is suppressed while the connection is closing on
     *          purpose.
     *
     * \param   msgFailed   The message whose write failed.
     * \param   whichTarget The socket the write was attempted on.
     **/
    void report_failed_send(const areg::MessageEnvelope & msgFailed, areg::Socket & whichTarget);

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Enables or disables event dispatching. Override to perform preparation work.
     *
     * \param   is_ready     Flag indicating whether dispatcher is ready for events.
     **/
    void ready_for_events( bool is_ready ) final;

/************************************************************************/
// EventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Posts and delivers event to target. Override in derived classes to process events.
     *
     * \param   eventElem       Event object to post.
     * \return  Returns true.
     **/
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// EventConsumer interface override.
/************************************************************************/
    /**
     * \brief   Receives IPC outbound events and exit signals dispatched to this send thread.
     *          Zeros internal1/internal2/custom in the event header before wire transmission.
     *          Exits on is_exit_prio().
     **/
    void start_event_processing( Event & eventElem ) final;

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of remote service handler to dispatch messages.
     **/
    RemoteMessageHandler&           mRemoteService;
    /**
     * \brief   The instance of connection to send messages from remote routing service.
     **/
    ClientConnection &              mConnection;
    /**
     * \brief   Atomic stats (bytes + messages sent + enabled flag).
     **/
    DataRateStats                   mSendStats;
    /**
     * \brief   Reusable holder keeping drained message buffers alive until the writev completes.
     *          Constructed once; slots are reset (not reconstructed) per send. ClientSendThread context only.
     **/
    areg::RawBufferPtr              mDrain[areg::DEFAULT_DRAIN_LIMIT];
    /**
     * \brief   Reusable continues region of buffers and data size.
     **/
    areg::IoBuffer                  mIoBuffer[areg::DEFAULT_DRAIN_LIMIT];
    /**
     * \brief   Reusable batch buffer for the single-window queue drain (pop_events).
     *          Constructed once; ClientSendThread context only.
     **/
    areg::Event                     mEvents[areg::DEFAULT_DRAIN_LIMIT];
    /**
     * \brief   Set while the connection is closing on purpose. Suppresses the send
     *          failure callback, which is off the successful send path.
     **/
    std::atomic_bool                mIsClosing;
    /**
     * \brief   The gate of this send queue. Counts the messages that were handed to this
     *          thread and have not reached the socket yet.
     **/
    areg::SendQueueGate             mSendGate;

    /**
     * \brief   How many messages this thread may put into one batch. Resolved from the
     *          configuration when the thread becomes ready and never touched afterwards, so
     *          reading it costs one load. Always within 1 .. areg::DEFAULT_DRAIN_LIMIT.
     **/
    uint32_t                        mDrainLimit;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ClientSendThread() = delete;
    AREG_NOCOPY_NOMOVE( ClientSendThread );
};

//////////////////////////////////////////////////////////////////////////
// ClientSendThread class inline methods
//////////////////////////////////////////////////////////////////////////

inline uint64_t ClientSendThread::extract_data_send() const noexcept
{
    return mSendStats.extract_bytes();
}

inline uint32_t ClientSendThread::extract_msgs_sent() const noexcept
{
    return mSendStats.extract_msgs();
}

inline void ClientSendThread::set_data_rate_enabled(bool enable) noexcept
{
    mSendStats.set_enabled(enable);
}

inline bool ClientSendThread::is_data_rate_enabled() const noexcept
{
    return mSendStats.is_enabled();
}

inline void ClientSendThread::set_closing() noexcept
{
    mIsClosing.store(true, std::memory_order_relaxed);
}

inline void ClientSendThread::accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept
{
    mSendStats.accumulate(bytes, msgs);
}

inline areg::SendQueueGate & ClientSendThread::send_gate() noexcept
{
    return mSendGate;
}

inline bool ClientSendThread::queue_message( areg::Event & eventElem )
{
    return EventDispatcher::post_event( eventElem );
}

inline uint32_t ClientSendThread::drain_limit() const noexcept
{
    return mDrainLimit;
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
