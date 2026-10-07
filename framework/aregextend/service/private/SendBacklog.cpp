/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/service/private/SendBacklog.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the writer of a service send thread that never waits for one
 *              socket. Operating system independent part.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/service/private/SendBacklog.hpp"

#include "areg/base/private/SocketLiveness.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <chrono>
#include <limits>
#include <new>

DEF_LOG_SCOPE(areg_aregextend_service_SendBacklog, log_close);
DEF_LOG_SCOPE(areg_aregextend_service_SendBacklog, enter_fallback);

namespace areg::ext {

namespace
{
    inline uint64_t now_ms() noexcept
    {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    //!< The bytes of a message on the wire: the header and the data.
    inline uint64_t wire_size(const areg::MessageEnvelope & message) noexcept
    {
        const areg::EventHeader * header{ message.header() };
        return (header != nullptr ? static_cast<uint64_t>(sizeof(areg::EventHeader)) + header->bufHeader.biUsed : 0u);
    }

    inline const uint8_t * wire_data(const areg::MessageEnvelope & message) noexcept
    {
        return reinterpret_cast<const uint8_t *>(message.header());
    }
}

//////////////////////////////////////////////////////////////////////////
// SendBacklog class implementation
//////////////////////////////////////////////////////////////////////////

SendBacklog::SendBacklog(areg::SendQueueGate & gate)
    : mGate         ( gate )
    , mEntries      ( )
    , mLock         ( )
    , mCount        ( 0u )
    , mCapBytes     ( static_cast<uint64_t>(areg::SOCKET_SEND_BUFFER_SIZE) )
    , mRefuseMs     ( SendBacklog::UNLIMITED )
    , mKeepaliveMs  ( areg::SOCKET_KEEPALIVE_SEC * 1'000u )
    , mWait         ( backlog_os_wait_create() )
    , mFallbackEvent( )
    , mWoken        ( false )
    , mFallback     ( false )
    , mPumpList     ( )
    , mWaitSockets  ( )
{
    if (mWait == nullptr)
    {
        enter_fallback("no wait object");
    }
}

SendBacklog::~SendBacklog()
{
    std::deque<SOCKETHANDLE> cutSockets;
    release_all(cutSockets);
}

void SendBacklog::set_limits(uint64_t capBytes, uint32_t refuseMs, uint32_t keepaliveMs) noexcept
{
    mCapBytes.store(capBytes != 0u ? capBytes : static_cast<uint64_t>(areg::SOCKET_SEND_BUFFER_SIZE), std::memory_order_relaxed);
    mRefuseMs.store(refuseMs != 0u ? refuseMs : SendBacklog::UNLIMITED, std::memory_order_relaxed);
    mKeepaliveMs.store(keepaliveMs != 0u ? keepaliveMs : areg::SOCKET_KEEPALIVE_SEC * 1'000u, std::memory_order_relaxed);
}

SendBacklog::Result SendBacklog::write(ITEM_ID cookie, SOCKETHANDLE hSocket, const areg::MessageEnvelope * const * messages, uint32_t count, uint64_t & bytesDone, uint32_t & msgsDone)
{
    ASSERT((messages != nullptr) && (count != 0u) && (count <= areg::DEFAULT_DRAIN_LIMIT));
    bytesDone = 0u;
    msgsDone  = 0u;

    Entry * entry{ is_empty() ? nullptr : find(cookie, hSocket) };
    if (entry != nullptr)
    {
        for (uint32_t i = 0u; i < count; ++i)
        {
            entry->messages.push_back(*messages[i]);
            entry->bytes += wire_size(*messages[i]);
        }

        const uint32_t before{ static_cast<uint32_t>(entry->messages.size()) };
        const bool result{ start_writes(*entry, bytesDone) };
        msgsDone = before - static_cast<uint32_t>(entry->messages.size());
        return (result ? Result::Kept : Result::Failed);
    }

    areg::IoBuffer buffers[areg::DEFAULT_DRAIN_LIMIT];
    uint64_t total{ 0u };
    for (uint32_t i = 0u; i < count; ++i)
    {
        const uint64_t size{ wire_size(*messages[i]) };
        buffers[i] = { wire_data(*messages[i]), static_cast<std::size_t>(size) };
        total += size;
    }

    SendBacklogOsPtr os{ };
    uint64_t started{ 0u };
    const int64_t written{ backlog_os_start(os, hSocket, buffers, count, in_flight_limit(), started) };
    if (written < 0)
    {
        if (cancel_writes(os, hSocket) == false)
        {
            // The operating system may still read these buffers: they are never released.
            auto * kept{ new (std::nothrow) std::deque<areg::MessageEnvelope>() };
            for (uint32_t i = 0u; (kept != nullptr) && (i < count); ++i)
            {
                kept->push_back(*messages[i]);
            }
        }

        return Result::Failed;
    }

    bytesDone = static_cast<uint64_t>(written);
    if (bytesDone == total)
    {
        msgsDone = count;
        return Result::Written;
    }

    const uint64_t nowMs{ now_ms() };
    Entry prepared{ };
    prepared.cookie     = cookie;
    prepared.socket     = hSocket;
    prepared.sinceMs    = nowMs;
    prepared.os         = std::move(os);
    for (uint32_t i = 0u; i < count; ++i)
    {
        prepared.messages.push_back(*messages[i]);
    }

    prepared.bytes = total;
    msgsDone = complete(prepared, bytesDone, nowMs);
    insert(std::move(prepared));
    return Result::Kept;
}

int32_t SendBacklog::write_inline(ITEM_ID cookie, SOCKETHANDLE hSocket, const areg::MessageEnvelope & message)
{
    if ((is_empty() == false) && (find(cookie, hSocket) != nullptr))
        return 0;

    const uint64_t size{ wire_size(message) };
    const areg::IoBuffer buffer{ wire_data(message), static_cast<std::size_t>(size) };
    SendBacklogOsPtr os{ };
    uint64_t started{ 0u };
    const int64_t written{ backlog_os_start(os, hSocket, &buffer, 1u, in_flight_limit(), started) };
    if (written < 0)
    {
        if (cancel_writes(os, hSocket) == false)
        {
            // The operating system may still read this buffer: it is never released.
            static_cast<void>(new (std::nothrow) areg::MessageEnvelope(message));
        }

        return -1;
    }
    else if (static_cast<uint64_t>(written) == size)
    {
        return 1;
    }
    else if ((written == 0) && (started == 0u))
    {
        return 0;
    }

    const uint64_t nowMs{ now_ms() };
    Entry prepared{ };
    prepared.cookie     = cookie;
    prepared.socket     = hSocket;
    prepared.sinceMs    = nowMs;
    prepared.os         = std::move(os);
    prepared.messages.push_back(message);
    prepared.bytes      = size;
    complete(prepared, static_cast<uint64_t>(written), nowMs);
    insert(std::move(prepared));
    return 2;
}

bool SendBacklog::pump(SendBacklog::Owner & owner)
{
    if (is_empty())
        return false;

    mPumpList.clear();
    {
        std::lock_guard<std::mutex> lock(mLock);
        for (auto & pair : mEntries)
        {
            mPumpList.push_back(&pair.second);
        }
    }

    bool progressed{ false };
    for (Entry * entry : mPumpList)
    {
        const SOCKETHANDLE current{ owner.backlog_socket(entry->cookie) };
        if (current != entry->socket)
        {
            // The connection is gone, closed by whoever noticed it first.
            remove(entry->cookie);
            continue;
        }

        uint64_t bytesDone{ 0u };
        uint32_t msgsDone{ 0u };
        bool failed{ false };
        Reason reason{ Reason::SocketError };
        {
            areg::SocketWriteGuard writeGuard{ entry->socket };
            const uint32_t before{ static_cast<uint32_t>(entry->messages.size()) };
            failed = (start_writes(*entry, bytesDone) == false);
            msgsDone = before - static_cast<uint32_t>(entry->messages.size());
            if ((failed == false) && (entry->messages.empty() == false))
            {
                failed = (check_limits(*entry, now_ms(), reason) == false);
            }
        }

        if (bytesDone != 0u)
        {
            progressed = true;
            owner.backlog_sent(bytesDone, msgsDone);
        }

        if (failed)
        {
            owner.backlog_close(*entry, reason);
            remove(entry->cookie);
        }
        else if (entry->messages.empty())
        {
            remove(entry->cookie);
        }
    }

    mPumpList.clear();
    return progressed;
}

bool SendBacklog::attach(SOCKETHANDLE hSocket) noexcept
{
    if ((mWait != nullptr) && backlog_os_attach(mWait.get(), hSocket))
        return true;

    enter_fallback("a socket could not be attached");
    return false;
}

uint32_t SendBacklog::prepare_wait(bool & overCap)
{
    const uint64_t nowMs{ now_ms() };
    const uint64_t cap{ mCapBytes.load(std::memory_order_relaxed) };
    const uint32_t refuseMs{ mRefuseMs.load(std::memory_order_relaxed) };
    uint64_t deadline{ std::numeric_limits<uint64_t>::max() };
    if (refuseMs == SendBacklog::UNLIMITED)
    {
        const uint32_t keepaliveMs{ mKeepaliveMs.load(std::memory_order_relaxed) };
        deadline = nowMs + std::max(1u, std::min(SendBacklog::TICK_MS, keepaliveMs / 5u));
    }

    overCap = false;
    mWaitSockets.clear();
    {
        std::lock_guard<std::mutex> lock(mLock);
        for (const auto & pair : mEntries)
        {
            const Entry & entry{ pair.second };
            mWaitSockets.push_back(entry.socket);
            if (refuseMs != SendBacklog::UNLIMITED)
            {
                deadline = std::min(deadline, entry.progressMs + refuseMs);
            }

            if (entry.bytes > cap)
            {
                deadline = std::min(deadline, entry.progressMs + SendBacklog::CAP_STALL_MS);
                if ((nowMs - entry.progressMs) < SendBacklog::CAP_PROGRESS_MS)
                {
                    overCap = true;
                    deadline = std::min(deadline, entry.progressMs + SendBacklog::CAP_PROGRESS_MS);
                }
            }
        }
    }

    const uint64_t remaining{ deadline > nowMs ? deadline - nowMs : 1u };
    return static_cast<uint32_t>(std::min<uint64_t>(remaining, areg::WAIT_INFINITE - 1u));
}

void SendBacklog::wait(uint32_t timeoutMs)
{
    if (mFallback.load(std::memory_order_relaxed))
    {
        timeoutMs = std::min(timeoutMs, SendBacklog::RECHECK_MS);
    }

    if (mWait == nullptr)
    {
        static_cast<void>(mFallbackEvent.lock(timeoutMs));
        return;
    }

    static_cast<void>(backlog_os_wait(mWait.get(), mWaitSockets.data(), static_cast<uint32_t>(mWaitSockets.size()), timeoutMs));
}

void SendBacklog::wake() noexcept
{
    if (mWoken.exchange(true, std::memory_order_acq_rel))
        return;

    if (mWait != nullptr)
    {
        backlog_os_signal(mWait.get());
    }
    else
    {
        static_cast<void>(mFallbackEvent.set_signaled());
    }
}

void SendBacklog::release_all(std::deque<SOCKETHANDLE> & cutSockets)
{
    std::unordered_map<ITEM_ID, Entry> entries;
    {
        std::lock_guard<std::mutex> lock(mLock);
        entries.swap(mEntries);
        if (mCount.exchange(0u, std::memory_order_acq_rel) != 0u)
        {
            mGate.leave(1u);
        }
    }

    for (auto & pair : entries)
    {
        Entry & entry{ pair.second };
        if ((entry.headDone != 0u) || (backlog_os_in_flight(entry.os.get()) != 0u))
        {
            cutSockets.push_back(entry.socket);
        }

        release_entry(entry);
    }
}

void SendBacklog::log_close(const Entry & entry, Reason reason) const
{
    LOG_SCOPE(areg_aregextend_service_SendBacklog, log_close);

    switch (reason)
    {
    case Reason::Refused:
        LOG_WARN("Client [ %u ] took no data for [ %u ] ms, the limit of net::*::tcpip::timeout; closing the connection"
                    , static_cast<uint32_t>(entry.cookie)
                    , mRefuseMs.load(std::memory_order_relaxed));
        break;

    case Reason::OverCap:
        LOG_ERR("Client [ %u ] takes no data and keeps [ %llu ] bytes, more than net::*::tcpip::sndbuf allows; closing the connection. Raise sndbuf to keep a paused client longer"
                    , static_cast<uint32_t>(entry.cookie)
                    , static_cast<unsigned long long>(entry.bytes));
        break;

    case Reason::Unreachable:
        LOG_WARN("Client [ %u ] stopped answering for [ %u ] ms, net::*::tcpip::keepalive; closing the connection"
                    , static_cast<uint32_t>(entry.cookie)
                    , mKeepaliveMs.load(std::memory_order_relaxed));
        break;

    case Reason::SocketError:
    default:
        LOG_WARN("Failed to write to client [ %u ], socket [ %u ]; closing the connection"
                    , static_cast<uint32_t>(entry.cookie)
                    , static_cast<uint32_t>(entry.socket));
        break;
    }
}

SendBacklog::Entry * SendBacklog::find(ITEM_ID cookie, SOCKETHANDLE hSocket) noexcept
{
    std::lock_guard<std::mutex> lock(mLock);
    auto pos{ mEntries.find(cookie) };
    return (((pos != mEntries.end()) && (pos->second.socket == hSocket)) ? &pos->second : nullptr);
}

void SendBacklog::insert(Entry && entry)
{
    entry.progressMs = (entry.progressMs != 0u ? entry.progressMs : entry.sinceMs);
    std::lock_guard<std::mutex> lock(mLock);
    const ITEM_ID cookie{ entry.cookie };
    mEntries.emplace(cookie, std::move(entry));
    if (mCount.fetch_add(1u, std::memory_order_acq_rel) == 0u)
    {
        mGate.enter();
    }
}

void SendBacklog::remove(ITEM_ID cookie)
{
    Entry removed{ };
    {
        std::lock_guard<std::mutex> lock(mLock);
        auto pos{ mEntries.find(cookie) };
        if (pos == mEntries.end())
            return;

        removed = std::move(pos->second);
        mEntries.erase(pos);
        if (mCount.fetch_sub(1u, std::memory_order_acq_rel) == 1u)
        {
            mGate.leave(1u);
        }
    }

    release_entry(removed);
}

void SendBacklog::release_entry(Entry & entry) noexcept
{
    if (cancel_writes(entry.os, entry.socket) == false)
    {
        // The operating system may still read these buffers: they are never released.
        static_cast<void>(new (std::nothrow) std::deque<areg::MessageEnvelope>(std::move(entry.messages)));
    }

    for (areg::MessageEnvelope & message : entry.messages)
    {
        message.destroy_event();
    }

    entry.messages.clear();
    entry.bytes = 0u;
}

bool SendBacklog::cancel_writes(SendBacklogOsPtr & os, SOCKETHANDLE hSocket) noexcept
{
    if (backlog_os_done(os.get()))
    {
        os.reset();
        return true;
    }

    // The buffers belong to the operating system until every write ended.
    backlog_os_cancel(os.get(), hSocket);
    const uint64_t deadline{ now_ms() + SendBacklog::CANCEL_WAIT_MS };
    const bool fallback{ mFallback.load(std::memory_order_relaxed) };
    bool consumed{ false };
    for (uint64_t nowMs{ now_ms() }; (backlog_os_done(os.get()) == false) && (nowMs < deadline); nowMs = now_ms())
    {
        const uint32_t remaining{ static_cast<uint32_t>(deadline - nowMs) };
        if (mWait == nullptr)
        {
            static_cast<void>(mFallbackEvent.lock(std::min(remaining, SendBacklog::RECHECK_MS)));
        }
        else
        {
            consumed = backlog_os_wait(mWait.get(), nullptr, 0u, fallback ? std::min(remaining, SendBacklog::RECHECK_MS) : remaining) || consumed;
        }
    }

    if (consumed)
    {
        // A completion of another socket may have been taken here: the next wait looks again.
        backlog_os_signal(mWait.get());
    }

    if (backlog_os_done(os.get()))
    {
        os.reset();
        return true;
    }

    static_cast<void>(os.release());
    return false;
}

void SendBacklog::enter_fallback(const char * what) noexcept
{
    if (mFallback.exchange(true, std::memory_order_acq_rel))
        return;

    LOG_SCOPE(areg_aregextend_service_SendBacklog, enter_fallback);
    LOG_WARN("Send backlog: %s, a slow connection is now served every [ %u ] ms"
                , what
                , SendBacklog::RECHECK_MS);
}

uint32_t SendBacklog::complete(Entry & entry, uint64_t bytes, uint64_t nowMs)
{
    uint32_t result{ 0u };
    if (bytes != 0u)
    {
        entry.progressMs = nowMs;
    }

    while ((bytes != 0u) && (entry.messages.empty() == false))
    {
        const uint64_t left{ wire_size(entry.messages.front()) - entry.headDone };
        if (bytes >= left)
        {
            bytes       -= left;
            entry.bytes -= left;
            entry.headDone = 0u;
            entry.messages.front().destroy_event();
            entry.messages.pop_front();
            ++result;
        }
        else
        {
            entry.headDone += static_cast<uint32_t>(bytes);
            entry.bytes    -= bytes;
            bytes = 0u;
        }
    }

    return result;
}

uint64_t SendBacklog::in_flight_limit() const noexcept
{
    return mCapBytes.load(std::memory_order_relaxed);
}

bool SendBacklog::start_writes(Entry & entry, uint64_t & bytesDone)
{
    const uint64_t nowMs{ now_ms() };
    const int64_t collected{ backlog_os_collect(entry.os.get(), entry.socket) };
    if (collected < 0)
        return false;

    bytesDone += static_cast<uint64_t>(collected);
    complete(entry, static_cast<uint64_t>(collected), nowMs);

    const uint64_t limit{ in_flight_limit() };
    for ( ; ; )
    {
        // The first byte not handed to the operating system yet.
        uint64_t skip{ entry.headDone + backlog_os_in_flight(entry.os.get()) };
        areg::IoBuffer buffers[areg::DEFAULT_DRAIN_LIMIT];
        uint32_t count{ 0u };
        uint64_t requested{ 0u };
        for (auto pos = entry.messages.begin(); (pos != entry.messages.end()) && (count < areg::DEFAULT_DRAIN_LIMIT) && (requested < SendBacklog::WRITE_CHUNK); ++pos)
        {
            const uint64_t size{ wire_size(*pos) };
            if (skip >= size)
            {
                skip -= size;
                continue;
            }

            uint64_t length{ size - skip };
            if (requested + length > SendBacklog::WRITE_CHUNK)
            {
                length = SendBacklog::WRITE_CHUNK - requested;
            }

            buffers[count++] = { wire_data(*pos) + skip, static_cast<std::size_t>(length) };
            requested += length;
            skip = 0u;
        }

        if (requested == 0u)
            break;

        uint64_t started{ 0u };
        const int64_t written{ backlog_os_start(entry.os, entry.socket, buffers, count, limit, started) };
        if (written < 0)
            return false;

        bytesDone += static_cast<uint64_t>(written);
        complete(entry, static_cast<uint64_t>(written), nowMs);
        if (started < requested)
            break;
    }

    return true;
}

bool SendBacklog::check_limits(const Entry & entry, uint64_t nowMs, Reason & reason) const noexcept
{
    const uint64_t stalledMs{ nowMs - entry.progressMs };
    const uint32_t refuseMs{ mRefuseMs.load(std::memory_order_relaxed) };
    if ((refuseMs != SendBacklog::UNLIMITED) && (stalledMs >= refuseMs))
    {
        reason = Reason::Refused;
        return false;
    }

    if ((entry.bytes > mCapBytes.load(std::memory_order_relaxed)) && (stalledMs >= SendBacklog::CAP_STALL_MS))
    {
        reason = Reason::OverCap;
        return false;
    }

    if (refuseMs == SendBacklog::UNLIMITED)
    {
        // Without a refusal limit the system bounds no wait; tell an unreachable peer apart here.
        const areg::SocketProgress progress{ areg::socket_progress(entry.socket) };
        const uint64_t waitingMs{ nowMs - entry.sinceMs };
        if (areg::is_peer_unreachable( progress, true
                                     , waitingMs < 0xFFFF'FFFFu ? static_cast<uint32_t>(waitingMs) : 0xFFFF'FFFFu
                                     , mKeepaliveMs.load(std::memory_order_relaxed)))
        {
            reason = Reason::Unreachable;
            return false;
        }
    }

    return true;
}

} // namespace areg::ext
