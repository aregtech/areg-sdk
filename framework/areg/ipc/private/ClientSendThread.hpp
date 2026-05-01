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
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/ipc/SendMessageEvent.hpp"

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
                                , public    SendMessageEventConsumer
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
    [[nodiscard]]
    bool post_event( Event & eventElem ) final;

private:
/************************************************************************/
// SendMessageEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Processes send message events dispatched by worker or component thread.
     *
     * \param   data    Send message event data to process.
     **/
    void process_event( const SendMessageEventData & data ) final;

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
     * \brief   Accumulative value of sent data size.
     **/
    mutable std::atomic_uint64_t    mBytesSend;
    /**
     * \brief   Accumulative count of sent messages.
     **/
    mutable std::atomic_uint32_t    mMsgsSend;
    /**
     * \brief   Flag, indicating whether should calculate send data size or not. By default it does not compute.
     **/
    bool                            mSaveDataSend;

#if defined(__linux__)
    /**
     * \brief   One slot in the zerocopy ring buffer.
     *
     *          A slot is occupied while the kernel (or NIC) still holds a reference to the
     *          underlying SharedBuffer via MSG_ZEROCOPY DMA.  The slot keeps the buffers
     *          alive by holding copies of the RemoteMessage objects that were sent in the
     *          same batch.
     *
     *          Layout:
     *            trigger_msg  — copy of the triggering event's message (evtPtrs[0]).
     *            events[0..event_count-1] — drained events (evtPtrs[1..N-1]), owned by us.
     *            hi_id        — highest zerocopy sequence ID assigned to this batch.
     *
     *          The slot is released when socket_drain_zerocopy_nb() or
     *          socket_drain_zerocopy() confirms hi_id.
     **/
    struct ZerocopyEntry
    {
        uint32_t          hi_id         { 0u };
        uint32_t          event_count   { 0u };
        RemoteMessage     trigger_msg;
        SendMessageEvent* events[areg::THREAD_DRAIN_LIMIT] {};
    };

    static constexpr uint32_t ZEROCOPY_RING_SIZE { areg::DEFAULT_ZEROCOPY_RING_SIZE };

    /**
     * \brief   Circular ring of in-flight zerocopy batch slots.
     *          Head is the next slot to write; tail is the oldest in-flight slot.
     **/
    ZerocopyEntry   mZerocopyRing[ZEROCOPY_RING_SIZE];
    uint32_t        mZerocopyRingHead    { 0u };
    uint32_t        mZerocopyRingTail    { 0u };
    uint32_t        mZerocopyRingCount   { 0u };

    /**
     * \brief   Next zerocopy sequence ID to assign.  Advances by sends_made per batch.
     **/
    uint32_t        mZerocopyIdNext      { 0u };

    /**
     * \brief   Highest confirmed zerocopy sequence ID (global watermark).
     *          Initialized to UINT32_MAX so nothing appears confirmed before any real
     *          ERRQUEUE notification arrives.
     **/
    uint32_t        mZerocopyConfirmed   { UINT32_MAX };

    /**
     * \brief   Non-blocking ERRQUEUE drain.  Updates mZerocopyConfirmed and releases
     *          any ring slots whose hi_id is now <= mZerocopyConfirmed.
     **/
    void _drain_available() noexcept;

    /**
     * \brief   Blocking drain of the oldest ring slot (ring-full recovery path).
     *          Blocks until the kernel confirms the oldest slot's hi_id, then releases it.
     **/
    void _drain_oldest_blocking() noexcept;

    /**
     * \brief   Releases a single ring slot: destroys owned drained events, clears
     *          trigger_msg (drops ref), advances tail.
     *
     * \param   slot    Ring slot to release.  Must be ring[mZerocopyRingTail].
     **/
    void _release_ring_slot(ZerocopyEntry& slot) noexcept;

    /**
     * \brief   Force-releases all occupied ring slots without draining.
     *          Called during shutdown BEFORE close_socket().
     *          The kernel will not complete DMA after socket close, so buffers
     *          become safe to release immediately.
     **/
    void _flush_ring() noexcept;

#endif  // defined(__linux__)

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
    return mBytesSend.exchange(0u, std::memory_order_relaxed);
}

inline uint32_t ClientSendThread::extract_msgs_sent() const noexcept
{
    return mMsgsSend.exchange(0u, std::memory_order_relaxed);
}

inline void ClientSendThread::set_data_rate_enabled(bool enable) noexcept
{
    if (mSaveDataSend != enable)
    {
        mBytesSend.store(0u, std::memory_order_relaxed);
        mMsgsSend.store(0u, std::memory_order_relaxed);
        mSaveDataSend = enable;
    }
}

inline bool ClientSendThread::is_data_rate_enabled() const noexcept
{
    return mSaveDataSend;
}

inline void ClientSendThread::accumulate_sent(uint64_t bytes, uint32_t msgs) noexcept
{
    if (mSaveDataSend)
    {
        mBytesSend.fetch_add(bytes, std::memory_order_relaxed);
        mMsgsSend.fetch_add(msgs, std::memory_order_relaxed);
    }
}

} // namespace areg
#endif  // AREG_IPC_PRIVATE_CLIENTSENDTHREAD_HPP
