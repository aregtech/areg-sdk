#ifndef AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
#define AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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
#include "areg/base/RemoteMessage.hpp"
#include "areg/base/SocketAccepted.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/RemoteMessageHandler.hpp"
#include "areg/ipc/SendMessageEvent.hpp"
#include "aregextend/service/ServerConnection.hpp"

#include <cstring>

namespace areg::ext {

/**
 * \brief   Phase 2 of the send batch pipeline: sort \a batch in-place by
 *          socket handle using binary insertion sort.
 *
 *          PendingSend is trivially copyable (SOCKETHANDLE + 2 pointers =
 *          24 bytes on 64-bit), so memmove is the fastest possible shift.
 *
 *          Fast path (1:1 / same-socket fan-out): the >= guard fires on
 *          every iteration — O(n) comparisons, zero memmove calls.
 *          Slow path (M:N / mixed targets): binary search in O(log n),
 *          then one SIMD-optimised memmove per displaced element.
 *
 * \param   batch   Batch array to sort.
 * \param   count   Number of valid entries in \a batch.
 **/
inline void sort_pending_sends(areg::PendingSend * batch, uint32_t count) noexcept
{
    for (uint32_t i{ 1 }; i < count; ++i)
    {
        if (batch[i].socket >= batch[i - 1].socket)
            continue;

        const areg::PendingSend key{ batch[i] };
        uint32_t lo{ 0 }, hi{ i };
        while (lo < hi)
        {
            const uint32_t mid{ lo + ((hi - lo) >> 1) };
            (batch[mid].socket <= key.socket) ? lo = mid + 1 : hi = mid;
        }

        ::memmove( &batch[lo + 1], &batch[lo]
                 , static_cast<size_t>(i - lo) * sizeof(areg::PendingSend) );
        batch[lo] = key;
    }
}

/**
 * \brief   Phase 3 of the send batch pipeline: send each same-socket group
 *          with a single syscall and accumulate stats.
 *
 *          Single-message groups use send_message(); multi-message groups
 *          build an IoBuffer array and use send_messages_batch() for one
 *          scatter/gather syscall. On failure, failed_send_message() is
 *          called unless the connection is already interrupted.
 *
 *          All SendMessageEvent entries in \a batch are destroyed before
 *          the function returns.
 *
 * \param   batch   Pre-sorted batch (sorted ascending by socket handle).
 * \param   count   Number of valid entries in \a batch.
 * \param   conn    Server connection (send + client-lookup API).
 * \param   handler Remote message handler (failure callback).
 * \param   accum   Callable(uint64_t bytes, uint32_t msgs) invoked after
 *                  each successful send to accumulate stats.
 **/
template<typename AccumFn>
inline void send_pending_groups( areg::PendingSend * batch
                               , uint32_t count
                               , ServerConnection & conn
                               , areg::RemoteMessageHandler & handler
                               , AccumFn && accum )
{
    for (uint32_t i{ 0 }; i < count; )
    {
        const SOCKETHANDLE hSocket{ batch[i].socket };
        uint32_t j{ i + 1 };
        while ((j < count) && (batch[j].socket == hSocket))
            ++j;

        const uint32_t groupSize{ j - i };

        if (groupSize == 1u)
        {
            const areg::RemoteMessage & message{ batch[i].sendEvt->data().remote_message() };
            const int32_t sent{ conn.send_message(message, hSocket) };
            if (sent > 0)
            {
                accum(static_cast<uint64_t>(sent), 1u);
            }
            else if (!conn.is_interrupted())
            {
                areg::SocketAccepted client{ conn.client_by_handle(hSocket) };
                handler.failed_send_message(message, client);
            }
        }
        else
        {
            areg::IoBuffer ioBuffer[areg::THREAD_DRAIN_LIMIT];
            uint32_t bufCount  { 0u };
            uint32_t totalSize { 0u };

            for (uint32_t k{ 0 }; k < groupSize; ++k)
            {
                const areg::RemoteMessage & msg = batch[i + k].sendEvt->data().remote_message();
                const areg::MessageHeader * hdr = msg.header();
                if (hdr != nullptr)
                {
                    msg.buffer_completion_fix();
                    const uint32_t bufSize = sizeof(areg::MessageHeader) + hdr->rbhBufHeader.biUsed;
                    ioBuffer[bufCount++] = { reinterpret_cast<const uint8_t *>(hdr), bufSize };
                    totalSize += bufSize;
                }
            }

            const int32_t sent = conn.send_messages_batch(ioBuffer, bufCount, hSocket, totalSize);
            if (sent > 0)
            {
                accum(static_cast<uint64_t>(sent), groupSize);
            }
            else if (!conn.is_interrupted())
            {
                areg::SocketAccepted client{ conn.client_by_handle(hSocket) };
                handler.failed_send_message(batch[i].sendEvt->data().remote_message(), client);
            }
        }

        for (uint32_t k{ i }; k < j; ++k)
            batch[k].sendEvt->destroy();

        i = j;
    }
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
 *          Returns false on the first receive failure. The caller is
 *          responsible for any failure cleanup (socket unregister, close,
 *          failed_receive_message(), etc.).
 *
 * \param   conn            Server connection (receive API).
 * \param   handler         Remote message handler (process callback).
 * \param   msgReceived     Reusable message buffer; overwritten on each call.
 * \param   clientSocket    Socket whose read-ahead cache to drain.
 * \param   accum           Callable(uint64_t bytes, uint32_t msgs) on success.
 * \return  true if all cached data was consumed; false on receive failure.
 **/
template<typename AccumFn>
inline bool drain_recv_cache( ServerConnection & conn
                            , areg::RemoteMessageHandler & handler
                            , areg::RemoteMessage & msgReceived
                            , areg::SocketAccepted & clientSocket
                            , AccumFn && accum )
{
    while (areg::recv_data_available(clientSocket.handle()) != 0u)
    {
        const int32_t cached = conn.receive_message(msgReceived, clientSocket);
        if (cached <= 0)
            return false;

        accum(static_cast<uint64_t>(cached), 1u);
        handler.process_received_message(msgReceived, clientSocket);
    }

    return true;
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_SERVICE_PRIVATE_SERVICETHREADHELPER_HPP
