#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/ServiceThreadHelper.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, shared hot-path helpers for server-side send and
 *              receive threads. Eliminates code duplication between
 *              ServerSendThread, PoolSendThread, ServerReceiveThread, and
 *              PoolReceiveThread without virtual dispatch or std::function overhead.
 *              All helpers are function templates so the compiler can inline the
 *              accumulator lambda at each call site.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/private/DebugDefs.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"
#include "aregextend/service/ServerConnection.hpp"
#include "aregextend/service/SystemServiceDefs.hpp"
#include "aregextend/service/private/SendBacklog.hpp"

#include <algorithm>
#include <cstring>
#include <unordered_map>

namespace areg::ext {

/**
 * \brief   Phase 2 of the send batch pipeline: sort \a batch in-place by
 *          socket handle using binary insertion sort.
 *
 *          Uses move-assignment (not memmove) because PendingSend contains
 *          an MessageEnvelope whose shared_ptr makes memmove undefined behavior.
 *
 * \param   batch   Batch array to sort.
 * \param   count   Number of valid entries in \a batch.
 **/
inline void sort_pending_sends(areg::ext::PendingSend * batch, uint32_t count) noexcept
{
    for ( uint32_t i{ 1u }; i < count; ++i )
    {
        if ( batch[i].socket >= batch[i - 1].socket )
            continue;

        areg::ext::PendingSend key{ std::move(batch[i]) };
        uint32_t lo{ 0u }, hi{ i };
        while ( lo < hi )
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            (batch[mid].socket <= key.socket) ? lo = mid + 1u : hi = mid;
        }

        for ( uint32_t j{ i }; j > lo; --j )
            batch[j] = std::move(batch[j - 1]);

        batch[lo] = std::move(key);
    }
}

/**
 * \brief   Phase 3 of the send batch pipeline: write each same-socket group without waiting
 *          and accumulate stats. What a socket does not take stays in \a backlog, in order.
 *
 * \param   batch   Sorted ascending by socket handle batch.
 * \param   count   Number of valid entries in \a batch.
 * \param   holds   The references to the sockets of the batch, kept until the batch is released.
 * \param   holdCount   Number of entries in \a holds.
 * \param   conn    Server connection (client-lookup API).
 * \param   handler Remote message handler (failure callback).
 * \param   backlog The writer of the send thread.
 * \param   accum   Callable(uint64_t bytes, uint32_t msgs) invoked after
 *                  each write to accumulate stats.
 **/
template<typename AccumFn>
inline void send_pending_groups( areg::ext::PendingSend * batch
                               , uint32_t count
                               , const areg::ext::HeldSocket * holds
                               , uint32_t holdCount
                               , ServerConnection & conn
                               , areg::RemoteMessageHandler & handler
                               , SendBacklog & backlog
                               , AccumFn && accum )
{
    for ( uint32_t i{ 0u }; i < count; )
    {
        const SOCKETHANDLE hSocket{ batch[i].socket };
        uint32_t j{ i + 1u };
        while ( (j < count) && (batch[j].socket == hSocket) )
            ++j;

        const uint32_t groupSize{ j - i };
        const areg::Socket::SocketHandle * hold{ areg::ext::find_hold(hSocket, holds, holdCount) };
        ASSERT(hold != nullptr);

        const areg::MessageEnvelope * messages[areg::DEFAULT_DRAIN_LIMIT];
        uint64_t sizes[areg::DEFAULT_DRAIN_LIMIT];
        uint32_t bufCount  { 0u };

        for ( uint32_t k{ 0u }; k < groupSize; ++k )
        {
            // PendingSend::msg is the wire-ready IPC envelope; header + payload sent verbatim.
            // internal1/internal2 were zeroed by the send thread before storage here.
            const areg::MessageEnvelope & env{ batch[i + k].msg };
            const areg::EventHeader* ipcHdr{ env.header() };
            if (ipcHdr == nullptr)
                continue;

            const uint64_t wireSize{ areg::wire_size(*ipcHdr) };
            messages[bufCount] = &env;
            sizes[bufCount++]  = wireSize;
        }

        // One write never carries more than MAX_SEND_BATCH_BYTES; a single message always fits.
        const ITEM_ID cookie{ static_cast<ITEM_ID>(batch[i].msg.target()) };
        bool failed{ false };
        {
            // Single writer per socket: the writes below belong to one message group and must not
            // be split by another thread writing into the same socket.
            areg::SocketWriteGuard writeGuard{ hSocket };
            for ( uint32_t start{ 0u }; start < bufCount; )
            {
                uint32_t end       { start };
                uint64_t batchBytes{ 0u };
                do
                {
                    batchBytes += sizes[end];
                    ++end;
                }
                while ( (end < bufCount) && (sizes[end] <= (areg::MAX_SEND_BATCH_BYTES - batchBytes)) );

                uint64_t bytesDone{ 0u };
                uint32_t msgsDone { 0u };
                const SendBacklog::Result result{ backlog.write(cookie, hSocket, messages + start, end - start, bytesDone, msgsDone, *hold) };
                if ( bytesDone != 0u )
                {
                    accum(bytesDone, msgsDone);
                }

                if ( result == SendBacklog::Result::Failed )
                {
                    failed = true;
                    break;
                }

                start = end;
            }
        }

        if ( failed && (conn.is_interrupted() == false) )
        {
            areg::SocketAccepted client{ conn.client_by_handle(hSocket) };
            handler.failed_send_message(batch[i].msg, client);
        }

        i = j;
    }
}

/**
 * \brief   Moves the backlog of a send thread on after a batch. While a connection keeps more
 *          than its cap, new messages wait: the thread takes nothing from its queue and wakes
 *          only for its sockets, an exit or a deadline. Otherwise the thread returns as soon as
 *          a message is queued, and while the queue is empty it waits for whichever comes
 *          first: a socket that takes data, a queued message, an exit or the nearest deadline.
 *
 * \param   thread  The send thread: has_queued_events(), wait_backlog(), is_exit_requested().
 * \param   backlog The writer of the thread.
 * \param   owner   The owner of the backlog, the send thread.
 **/
template<typename ThreadT>
inline void serve_backlog(ThreadT & thread, SendBacklog & backlog, SendBacklog::Owner & owner)
{
    while ( backlog.is_empty() == false )
    {
        static_cast<void>(backlog.pump(owner));
        if ( backlog.is_empty() || thread.is_exit_requested() )
            break;

        bool overCap{ false };
        const uint32_t timeoutMs{ backlog.prepare_wait(overCap) };
        if ( (overCap == false) && thread.has_queued_events() )
            break;

        thread.wait_backlog(overCap == false, timeoutMs);
    }
}

/**
 * \brief   The working set a send thread hands to run_send_batch(). Every array holds
 *          areg::DEFAULT_DRAIN_LIMIT entries and belongs to the thread, which reuses it in
 *          every drain cycle.
 **/
struct SendBatchContext
{
    areg::ext::PendingSend *        batch;      //!< Batch work list, sorted by socket handle in phase 3.
    areg::Event *                   events;     //!< Drain window handed to pop_events().
    ITEM_ID *                       targets;    //!< Target cookie of every batch entry.
    SOCKETHANDLE *                  sockets;    //!< Socket resolved for every target cookie.
    areg::ext::HeldSocket *         holds;      //!< One reference to each distinct socket of the batch.
    areg::SendQueueGate *           gate;       //!< The gate of the send queue of the thread.
    ServerConnection *              connection; //!< The connection the groups are written into.
    areg::RemoteMessageHandler *    handler;    //!< Notified when a group cannot be written.
    SendBacklog *                   backlog;    //!< The writer of the thread.
};

/**
 * \brief   The body of a system service send thread: takes the event that woke the thread up,
 *          drains whatever is queued behind it, resolves every target cookie to a socket, groups
 *          the messages by socket and writes each group with a single system call. Used by both
 *          ServerSendThread and PoolSendThread, which differ only in the callables they pass.
 *
 * \param   thread      The send thread, used for pop_events() and trigger_exit().
 * \param   eventElem   The event that started this processing round.
 * \param   context     The working set of the thread, see SendBatchContext.
 * \param   resolve     Callable(ITEM_ID * targets, SOCKETHANDLE * sockets, uint32_t count,
 *                      HeldSocket * holds, uint32_t & holdCount) that resolves the
 *                      cookies of the whole batch in one lock window and keeps a reference to
 *                      each socket found, so none is closed while the batch writes it.
 * \param   accumulate  Callable(uint64_t bytes, uint32_t msgs), counts what left the socket.
 * \param   onExit      Callable() invoked before the thread leaves, to close what it owns.
 * \param   onDiscard   Callable(uint32_t messageId, ITEM_ID target) for a message whose target
 *                      is gone. It stays at the call site, where the log scope is defined.
 **/
template<typename ThreadT, typename ResolveFn, typename AccumFn, typename ExitFn, typename DiscardFn>
inline void run_send_batch( ThreadT & thread
                          , areg::Event & eventElem
                          , const SendBatchContext & context
                          , ResolveFn && resolve
                          , AccumFn && accumulate
                          , ExitFn && onExit
                          , DiscardFn && onDiscard )
{
    if ( eventElem.is_exit_prio() )
    {
        onExit();
        thread.trigger_exit();
        return;
    }

    // Zero local-only routing fields before wire transmission.
    areg::EventHeader * hdr0{ eventElem.header() };
    ASSERT( hdr0 != nullptr );
    hdr0->internal1 = 0u;
    hdr0->internal2 = 0u;
    ASSERT( hdr0->custom == 0u );

    uint32_t batchCount{ 0u };

    context.targets[batchCount]      = static_cast<ITEM_ID>(hdr0->target);
    context.batch[batchCount].socket = areg::InvalidSocketHandle;
    context.batch[batchCount].msg    = eventElem.envelope();  // O(1) shared_ptr copy; fields already zeroed
    ++batchCount;

    // Phase 1: drain additional queued events into the batch via a single dequeue window.
    const uint32_t drained{ thread.pop_events(context.events, thread.drain_limit() - batchCount) };
    for ( uint32_t k{ 0u }; k < drained; ++k )
    {
        areg::Event & evt{ context.events[k] };
        if ( evt.is_exit_prio() )
        {
            for ( uint32_t i{ 0u }; i < batchCount; ++i )
                context.batch[i].msg.destroy_event();

            context.gate->leave(batchCount);
            onExit();
            thread.trigger_exit();
            return;
        }

        areg::EventHeader * hdr{ evt.header() };
        ASSERT( hdr != nullptr );
        hdr->internal1 = 0u;
        hdr->internal2 = 0u;
        ASSERT( hdr->custom == 0u );

        context.targets[batchCount]      = static_cast<ITEM_ID>(hdr->target);
        context.batch[batchCount].socket = areg::InvalidSocketHandle;
        context.batch[batchCount].msg    = evt.envelope();  // O(1) shared_ptr copy; the batch keeps the buffer alive
        evt.destroy_event();                                // release the drain window slot
        ++batchCount;
    }

    // Phase 2: resolve all cookies in one lock window.
    uint32_t holdCount{ 0u };
    resolve(context.targets, context.sockets, batchCount, context.holds, holdCount);

    // Phase 3: compact + insertion-sort by socket handle in one pass. A message whose target is
    // gone is dropped, there is no socket left to write it into.
    uint32_t validCount{ 0u };
    for ( uint32_t i{ 0u }; i < batchCount; ++i )
    {
        if ( !areg::is_valid_socket(context.sockets[i]) )
        {
            onDiscard(context.batch[i].msg.message_id(), context.targets[i]);
            continue;
        }

        areg::ext::PendingSend entry{ context.sockets[i], std::move(context.batch[i].msg) };
        uint32_t lo{ 0u }, hi{ validCount };
        while ( lo < hi )
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            context.batch[mid].socket <= entry.socket ? lo = mid + 1u : hi = mid;
        }

        // Shift right using move-assignment
        for ( uint32_t j{ validCount }; j > lo; --j )
            context.batch[j] = std::move(context.batch[j - 1]);

        context.batch[lo] = std::move(entry);
        ++validCount;
    }

    if ( validCount != 0u )
    {
        // Phase 4: the batch is sorted, send the same-socket groups directly.
        AREG_LT_SCOPE(areg::LtStage::SendSyscall);  // isolate the ::send() from resolve + sort
        areg::ext::send_pending_groups( context.batch, validCount, context.holds, holdCount, *context.connection, *context.handler
                                      , *context.backlog, std::forward<AccumFn>(accumulate) );
    }

    // Phase 5: release every wire buffer, including the entries discarded in phase 3, and the sockets.
    for ( uint32_t i{ 0u }; i < batchCount; ++i )
        context.batch[i].msg.destroy_event();

    for ( uint32_t i{ 0u }; i < holdCount; ++i )
        context.holds[i].hold.reset();

    // The gate opens only after the write, never before it.
    context.gate->leave(batchCount);
}

/**
 * \brief   Drains the read-ahead receive cache for \a clientSocket.
 *
 *          After a successful recv_message() call, _os_recv_data may have
 *          pulled extra bytes into a thread-local cache. Those bytes are
 *          invisible to the kernel multiplexer (epoll / kqueue / WSAPoll),
 *          so they must be consumed before returning to the wait loop.
 *
 *          For each cached message, process_received_message() is called and
 *          \a accum is invoked with the byte and message counts.
 *
 *          Returns -1 on the first receive failure. The caller is
 *          responsible for any failure cleanup (socket unregister, close,
 *          failed_receive_message(), etc.).
 *
 *          A return value equal to \a maxDrain means the ceiling stopped the
 *          drain and the cache may still hold messages. The caller has to come
 *          back to the socket: the multiplexer cannot see a user space buffer.
 *
 * \param   conn            Server connection (receive API).
 * \param   handler         Remote message handler (process callback).
 * \param   maxDrain        Maximum number of cached messages to drain.
 * \param   clientSocket    Socket whose read-ahead cache to drain.
 * \param   msgReceived     Reusable message buffer; overwritten on each call.
 * \param   accum           Callable(uint64_t bytes, uint32_t msgs) on success.
 * \return  The number of messages drained, or -1 on receive failure.
 **/
template<typename AccumFn>
inline int32_t drain_recv_cache( ServerConnection & conn
                               , areg::RemoteMessageHandler & handler
                               , uint32_t maxDrain
                               , areg::SocketAccepted & clientSocket
                               , areg::MessageEnvelope & msgReceived
                               , AccumFn && accum )
{
    uint32_t drain{ 0u };
    while ( (areg::recv_data_available(clientSocket.handle()) != 0u) && (drain < maxDrain) )
    {
        const int32_t cached{ conn.receive_message(msgReceived, clientSocket) };
        if ( cached <= 0 )
            return -1;

        handler.process_received_message(msgReceived, clientSocket);
        accum(static_cast<uint64_t>(cached), 1u);
        ++drain;
    }

    return static_cast<int32_t>(drain);
}

/**
 * \brief   The messages a receive thread is receiving that do not fit the read-ahead cache of a
 *          socket, one per socket at most. Each has its whole body allocated and is filled as its
 *          bytes arrive. Used by the receive thread that serves the sockets only.
 **/
class ReceiveParts
{
public:
    //!< A message being received.
    struct Part
    {
        areg::MessageEnvelope   msg;    //!< The message, its body allocated in full.
        uint8_t *               body;   //!< The body buffer of the message.
        uint32_t                size;   //!< The size of the body.
        uint32_t                done;   //!< The bytes of the body received so far.
    };

    //!< Returns the message being received from the socket, or nullptr if there is none.
    inline Part * find(SOCKETHANDLE hSocket) noexcept
    {
        if (mParts.empty())
            return nullptr;

        auto pos{ mParts.find(hSocket) };
        return (pos != mParts.end() ? &pos->second : nullptr);
    }

    //!< Starts a message of the socket.
    inline Part & start(SOCKETHANDLE hSocket)
    {
        return mParts[hSocket];
    }

    //!< Drops the message of the socket, if any.
    inline void drop(SOCKETHANDLE hSocket) noexcept
    {
        if (mParts.empty() == false)
            mParts.erase(hSocket);
    }

    //!< Drops every message.
    inline void clear() noexcept
    {
        mParts.clear();
    }

private:
    std::unordered_map<SOCKETHANDLE, Part>  mParts; //!< The messages, by socket.
};

/**
 * \brief   Receives what a ready socket has now, without waiting, and dispatches every message
 *          that is complete. A message that is not complete stays in the read-ahead cache, or in
 *          \a parts when it does not fit the cache, until the rest arrives.
 *
 * \param   conn            Server connection (receive API).
 * \param   handler         Remote message handler (process callback).
 * \param   maxDrain        Maximum number of messages to dispatch.
 * \param   clientSocket    The ready socket.
 * \param   msgReceived     Reusable message buffer; overwritten on each message.
 * \param   parts           The messages of the thread that do not fit the cache.
 * \param   accum           Callable(uint64_t bytes, uint32_t msgs) on each message.
 * \return  The number of messages dispatched, or -1 if the connection ended, failed or sent a
 *          header that is not valid. A return equal to \a maxDrain means the cache may still hold
 *          messages, see drain_recv_cache().
 **/
template<typename AccumFn>
inline int32_t receive_ready_socket( ServerConnection & conn
                                   , areg::RemoteMessageHandler & handler
                                   , uint32_t maxDrain
                                   , areg::SocketAccepted & clientSocket
                                   , areg::MessageEnvelope & msgReceived
                                   , ReceiveParts & parts
                                   , AccumFn && accum )
{
    const SOCKETHANDLE hSocket{ clientSocket.handle() };
    int32_t completed{ 0 };

    // Moves a part on with what the socket has; dispatches it when it is complete.
    auto receivePart = [&](ReceiveParts::Part & part) -> bool
    {
        const int32_t received{ areg::receive_data_nowait(hSocket, part.body + part.done, part.size - part.done) };
        if ( received < 0 )
            return false;

        part.done += static_cast<uint32_t>(received);
        if ( part.done == part.size )
        {
            msgReceived = std::move(part.msg);
            msgReceived.set_size_used(part.size);
            msgReceived.move_to_begin();
            const uint64_t bytes{ static_cast<uint64_t>(areg::WIRE_HEADER_SIZE) + part.size };
            parts.drop(hSocket);
            handler.process_received_message(msgReceived, clientSocket);
            accum(bytes, 1u);
            ++completed;
        }

        return true;
    };

    ReceiveParts::Part * part{ parts.find(hSocket) };
    if ( part != nullptr )
    {
        if ( receivePart(*part) == false )
            return -1;
        else if ( completed == 0 )
            return 0;
    }

    // A cache that holds a complete message is drained before it is filled.
    const bool ended{ (areg::recv_data_available(hSocket) == 0u) && (areg::recv_fill_nowait(hSocket) < 0) };
    const uint32_t ceiling{ maxDrain - static_cast<uint32_t>(completed) };
    const int32_t drained{ areg::ext::drain_recv_cache(conn, handler, ceiling, clientSocket, msgReceived, accum) };
    if ( drained < 0 )
        return -1;

    completed += drained;
    if ( static_cast<uint32_t>(drained) == ceiling )
        return completed;

    const areg::ThreadCache & tc{ areg::thread_rx_cache(hSocket) };
    if ( tc.unread >= areg::WIRE_HEADER_SIZE )
    {
        areg::EventHeader header{ };
        std::memcpy(areg::wire_header(header), tc.buffer.get() + tc.head, areg::WIRE_HEADER_SIZE);
        if ( static_cast<uint64_t>(areg::wire_size(header)) > tc.space )
        {
            // A message larger than the cache is received into its own buffer.
            static_cast<void>(areg::receive_data_nowait(hSocket, areg::wire_header(header), areg::WIRE_HEADER_SIZE));
            ReceiveParts::Part & started{ parts.start(hSocket) };
            started.body = areg::SocketConnectionBase::prepare_message(started.msg, header);
            started.size = header.bufHeader.biUsed;
            started.done = 0u;
            if ( (started.body == nullptr) || (receivePart(started) == false) )
            {
                parts.drop(hSocket);
                return -1;
            }
        }
    }

    return (ended ? -1 : completed);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
