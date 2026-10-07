#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SENDBACKLOG_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SENDBACKLOG_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/SendBacklog.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the writer of a service send thread that never waits for one
 *              socket. What a socket cannot take now is kept per connection, in order, and
 *              written when the socket takes it.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/private/SocketLiveness.hpp"
#include "areg/component/private/EventQueue.hpp"
#include "areg/component/private/SimpleEvent.hpp"
#include "areg/ipc/private/ConnectionDefs.hpp"

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace areg::ext {

/**
 * \brief   The operating system part of one connection's backlog: the writes started and not
 *          completed yet. Empty where a write completes when it returns.
 **/
struct SendBacklogOs;

//!< Deletes the operating system part, which is complete only in the operating system files.
struct SendBacklogOsDelete
{
    void operator () (SendBacklogOs * os) const noexcept;
};

//!< Owns the operating system part of one connection's backlog.
using SendBacklogOsPtr = std::unique_ptr<SendBacklogOs, SendBacklogOsDelete>;

/**
 * \brief   The operating system objects a send thread waits on: its wake-up, and on Windows
 *          the completion port of its sockets.
 **/
struct SendBacklogWaitOs;

//!< Deletes the wait objects, which are complete only in the operating system files.
struct SendBacklogWaitOsDelete
{
    void operator () (SendBacklogWaitOs * wait) const noexcept;
};

//!< Owns the wait objects of a send thread.
using SendBacklogWaitOsPtr = std::unique_ptr<SendBacklogWaitOs, SendBacklogWaitOsDelete>;

class SendBacklog;

/**
 * \brief   The backlog of all connections one send thread writes. A message is written whole or
 *          the connection is closed: once its first byte is in a socket, the next bytes of that
 *          socket are the rest of it.
 *
 *          The map is shared with a thread that writes inline (see write_inline()), so it is
 *          locked; the data of an entry is touched only while holding the writer lock of the
 *          socket, see areg::SocketWriter. Only the send thread removes entries.
 *
 *          It is also the wait object of its send thread: the thread arms it in its event
 *          queue and blocks in wait() until a socket takes data, a queued event or an exit
 *          calls wake(), or the nearest deadline of a backlog is due.
 **/
class SendBacklog final : public areg::QueueWaiter
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< Bytes of one write a slow socket is given at once.
    static constexpr uint32_t   WRITE_CHUNK         { 256u * 1024u };
    //!< Milliseconds without progress after which a backlog above its cap closes the connection.
    static constexpr uint32_t   CAP_STALL_MS        { 1'000u };
    //!< A backlog above its cap holds new messages back only if it moved within these milliseconds.
    static constexpr uint32_t   CAP_PROGRESS_MS     { 50u };
    //!< Longest wait, in milliseconds, of a send thread without its wait objects (fallback mode).
    static constexpr uint32_t   RECHECK_MS          { 10u };
    //!< Longest wait, in milliseconds, between two liveness checks of an unlimited backlog.
    static constexpr uint32_t   TICK_MS             { 1'000u };
    //!< Milliseconds a cancelled write is waited for before its buffers are kept for ever.
    static constexpr uint32_t   CANCEL_WAIT_MS      { 5'000u };
    //!< Refusal limit that means: never close a peer for refusing data.
    static constexpr uint32_t   UNLIMITED           { areg::SEND_REFUSAL_UNLIMITED };

    //!< What happened to the data handed to write().
    enum class Result : uint8_t
    {
          Written   //!< The socket took everything.
        , Kept      //!< The socket took a part or nothing; the rest is in the backlog.
        , Failed    //!< The socket failed; the connection must be closed.
    };

    //!< Why a connection is closed by pump().
    enum class Reason : uint8_t
    {
          SocketError   //!< The socket failed.
        , Refused       //!< The peer took no byte for the refusal limit.
        , OverCap       //!< The backlog exceeds its cap and the peer takes nothing.
        , Unreachable   //!< The peer's stack stopped answering.
    };

    //!< The backlog of one connection.
    struct Entry
    {
        ITEM_ID                             cookie      { 0u };     //!< The connection.
        SOCKETHANDLE                        socket      { areg::InvalidSocketHandle };  //!< Its socket.
        std::deque<areg::MessageEnvelope>   messages    { };        //!< Not completed, in order.
        uint32_t                            headDone    { 0u };     //!< Completed bytes of the first message.
        uint64_t                            bytes       { 0u };     //!< Bytes not completed.
        uint64_t                            sinceMs     { 0u };     //!< When the backlog started.
        uint64_t                            progressMs  { 0u };     //!< When a byte was last completed.
        SendBacklogOsPtr      os          { };        //!< Writes in progress.
    };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates an empty backlog. The gate stays entered while any connection has a
     *          backlog, so that no inline write overtakes it.
     **/
    explicit SendBacklog(areg::SendQueueGate & gate);

    ~SendBacklog();

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Sets the limits. Thread safe, applied by the next pump().
     *
     * \param   capBytes    The most a connection may keep, in bytes.
     * \param   refuseMs    Milliseconds a peer may take nothing before it is closed; UNLIMITED never.
     * \param   keepaliveMs Milliseconds a silent peer is given.
     **/
    void set_limits(uint64_t capBytes, uint32_t refuseMs, uint32_t keepaliveMs) noexcept;

    /**
     * \brief   Returns true if no connection has a backlog. Lock free.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Writes the messages to the connection without waiting. If the connection has a
     *          backlog, they are added behind it. What the socket does not take is kept.
     *          Call it holding the writer lock of \a hSocket.
     *
     * \param   cookie      The connection.
     * \param   hSocket     Its socket.
     * \param   messages    The messages, in order.
     * \param   count       The number of messages.
     * \param   bytesDone   On return, the bytes the socket took now.
     * \param   msgsDone    On return, the messages the socket took whole now.
     * \return  What happened to the messages.
     **/
    Result write(ITEM_ID cookie, SOCKETHANDLE hSocket, const areg::MessageEnvelope * const * messages, uint32_t count, uint64_t & bytesDone, uint32_t & msgsDone);

    /**
     * \brief   Writes one message from a thread that is not the send thread. Nothing is written
     *          if the connection has a backlog. A rest the socket does not take is kept, and the
     *          caller wakes the send thread.
     *          Call it holding the writer lock of \a hSocket.
     *
     * \return  -1 if the socket failed, 0 if nothing was written and the message has to be queued,
     *          1 if it was written whole, 2 if a part was written and the rest kept.
     **/
    int32_t write_inline(ITEM_ID cookie, SOCKETHANDLE hSocket, const areg::MessageEnvelope & message);

    /**
     * \brief   The send thread that owns the backlog: it knows the connections and closes them.
     **/
    class Owner
    {
    public:
        virtual ~Owner() = default;

        //!< Returns the current socket of the connection, or an invalid one if it is gone.
        virtual SOCKETHANDLE backlog_socket(ITEM_ID cookie) = 0;

        //!< Counts the bytes and messages that left a backlog.
        virtual void backlog_sent(uint64_t bytes, uint32_t msgs) = 0;

        //!< Closes the connection of the entry. Its backlog is released after the call.
        virtual void backlog_close(const Entry & entry, Reason reason) = 0;
    };

    /**
     * \brief   Moves every backlog on as far as its socket allows, without waiting, and applies
     *          the limits. Called by the send thread only.
     *
     * \return  True if a backlog made progress.
     **/
    bool pump(Owner & owner);

    /**
     * \brief   Registers an accepted socket that this backlog writes. Where the operating system
     *          reports a completed write to a wait object, the socket is attached to it here.
     *          Thread safe. On failure the backlog works on in fallback mode, see RECHECK_MS.
     *
     * \return  False if the socket could not be attached.
     **/
    bool attach(SOCKETHANDLE hSocket) noexcept;

    /**
     * \brief   Prepares the next wait() in one pass over the backlogs: collects their sockets and
     *          returns the milliseconds until the nearest deadline, a limit that closes a
     *          connection, the end of the over cap state, or a liveness check.
     *          Called by the send thread only.
     *
     * \param   overCap On return, true if a connection keeps more than its cap while its peer
     *                  still takes data, within CAP_PROGRESS_MS. The send thread then stops
     *                  taking new messages until it is below. A peer that takes nothing holds no
     *                  one back; it is closed instead.
     * \return  The milliseconds wait() may block.
     **/
    [[nodiscard]]
    uint32_t prepare_wait(bool & overCap);

    /**
     * \brief   Makes the next wake() end the next wait(). Call it before arming the backlog in
     *          the event queue of the send thread.
     **/
    inline void reset_wake() noexcept;

    /**
     * \brief   Blocks until a socket collected by prepare_wait() can take data or a write
     *          completes, wake() is called, or \a timeoutMs elapses. Called by the send thread only.
     **/
    void wait(uint32_t timeoutMs);

    /**
     * \brief   Ends the wait of the send thread. Called by its event queue from any thread.
     **/
    void wake() noexcept override;

    /**
     * \brief   Logs why the connection of \a entry is closed, naming the configuration key
     *          that sets the limit.
     **/
    void log_close(const Entry & entry, Reason reason) const;

    /**
     * \brief   Releases every backlog and returns the sockets whose first message was cut,
     *          which must be closed before anything else is written into them.
     *          Called by the send thread when it stops.
     **/
    void release_all(std::deque<SOCKETHANDLE> & cutSockets);

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    //!< Returns the entry of the connection, or nullptr. Takes the map lock.
    Entry * find(ITEM_ID cookie, SOCKETHANDLE hSocket) noexcept;

    //!< Publishes a complete entry. Takes the map lock.
    void insert(Entry && entry);

    //!< Removes the entry and releases its messages. Takes the map lock.
    void remove(ITEM_ID cookie);

    //!< Releases the writes and the messages of an entry that is no longer in the map.
    void release_entry(Entry & entry) noexcept;

    //!< Cancels the writes in progress. Returns false if one did not end and its buffers must not be freed.
    bool cancel_writes(SendBacklogOsPtr & os, SOCKETHANDLE hSocket) noexcept;

    //!< Enters fallback mode once and logs why.
    void enter_fallback(const char * what) noexcept;

    //!< Completes \a bytes of the entry and returns the number of messages completed.
    uint32_t complete(Entry & entry, uint64_t bytes, uint64_t nowMs);

    //!< Returns the bytes an entry may have in flight.
    [[nodiscard]]
    uint64_t in_flight_limit() const noexcept;

    //!< Starts the writes of an entry that the socket takes now. Returns false on a socket error.
    bool start_writes(Entry & entry, uint64_t & bytesDone);

    //!< Applies the limits to one entry and returns false with the reason if it must be closed.
    bool check_limits(const Entry & entry, uint64_t nowMs, Reason & reason) const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The gate of the send queue, entered while any backlog exists.
    areg::SendQueueGate &                   mGate;
    //!< The backlogs by connection cookie.
    std::unordered_map<ITEM_ID, Entry>      mEntries;
    //!< Guards the map, not the entries.
    mutable std::mutex                      mLock;
    //!< The number of entries, read without the lock.
    std::atomic<uint32_t>                   mCount;
    //!< The cap of one connection, in bytes.
    std::atomic<uint64_t>                   mCapBytes;
    //!< The refusal limit, in milliseconds.
    std::atomic<uint32_t>                   mRefuseMs;
    //!< The keepalive time, in milliseconds.
    std::atomic<uint32_t>                   mKeepaliveMs;
    //!< The wait objects of the send thread, nullptr if they could not be created.
    SendBacklogWaitOsPtr                    mWait;
    //!< The wake-up used in fallback mode.
    areg::SimpleEvent                       mFallbackEvent;
    //!< True once wake() signalled the current wait.
    std::atomic<bool>                       mWoken;
    //!< True if a wait object is missing; waits then last at most RECHECK_MS.
    std::atomic<bool>                       mFallback;
    //!< The entries of one pump(), reused.
    std::vector<Entry *>                    mPumpList;
    //!< The sockets of one wait(), reused.
    std::vector<SOCKETHANDLE>               mWaitSockets;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    SendBacklog() = delete;
    AREG_NOCOPY_NOMOVE( SendBacklog );
};

//////////////////////////////////////////////////////////////////////////
// Operating system calls of the backlog
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Starts writing \a count buffers into \a hSocket without waiting.
 *
 * \param   os          The writes in progress of the connection; nullptr if it has no backlog.
 *                      Where writes complete later, a write that does not complete now is added
 *                      to it and \a os is created if needed.
 * \param   hSocket     The socket.
 * \param   buffers     The buffers, in order.
 * \param   count       The number of buffers, at most areg::DEFAULT_DRAIN_LIMIT.
 * \param   maxInFlight The most bytes that may be in progress after the call.
 * \param   started     On return, the bytes handed to the operating system, completed or not.
 * \return  The bytes completed now, or -1 if the socket failed.
 **/
int64_t backlog_os_start( SendBacklogOsPtr & os
                        , SOCKETHANDLE hSocket
                        , const areg::IoBuffer * buffers
                        , uint32_t count
                        , uint64_t maxInFlight
                        , uint64_t & started );

/**
 * \brief   Returns the bytes of started writes that completed since the last call, without
 *          waiting, or -1 if one of them failed.
 **/
int64_t backlog_os_collect(SendBacklogOs * os, SOCKETHANDLE hSocket);

/**
 * \brief   Returns the bytes handed to the operating system and not completed.
 **/
uint64_t backlog_os_in_flight(const SendBacklogOs * os) noexcept;

/**
 * \brief   Asks the operating system to cancel the writes in progress, without waiting.
 **/
void backlog_os_cancel(SendBacklogOs * os, SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Returns true if no started write is still in progress.
 **/
bool backlog_os_done(const SendBacklogOs * os) noexcept;

/**
 * \brief   Creates the wait objects of a send thread. Returns nullptr on failure.
 **/
SendBacklogWaitOs * backlog_os_wait_create() noexcept;

/**
 * \brief   Attaches an accepted socket to the wait objects. Returns false on failure.
 **/
bool backlog_os_attach(SendBacklogWaitOs * wait, SOCKETHANDLE hSocket) noexcept;

/**
 * \brief   Ends the current or the next backlog_os_wait(). Called from any thread.
 **/
void backlog_os_signal(SendBacklogWaitOs * wait) noexcept;

/**
 * \brief   Blocks until one of \a sockets can take data or a write completes, the wait is
 *          signalled, or \a timeoutMs elapses. Consumes the signal.
 *
 * \return  True if it consumed a completion or a signal.
 **/
bool backlog_os_wait(SendBacklogWaitOs * wait, const SOCKETHANDLE * sockets, uint32_t count, uint32_t timeoutMs) noexcept;

//////////////////////////////////////////////////////////////////////////
// SendBacklog inline and template methods
//////////////////////////////////////////////////////////////////////////

inline bool SendBacklog::is_empty() const noexcept
{
    return (mCount.load(std::memory_order_acquire) == 0u);
}

inline void SendBacklog::reset_wake() noexcept
{
    mWoken.store(false, std::memory_order_relaxed);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SENDBACKLOG_HPP
