/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/private/linux/ClientSendThreadLinux.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform. Linux-specific ClientSendThread methods:
 *              zerocopy ring buffer management (MSG_ZEROCOPY / ERRQUEUE).
 ************************************************************************/
#ifdef __linux__

#include "areg/ipc/private/ClientSendThread.hpp"

#include "areg/base/SocketDefs.hpp"
#include "areg/ipc/ClientConnection.hpp"

namespace areg {

void ClientSendThread::_release_ring_slot(ZerocopyEntry& slot) noexcept
{
    // Destroy drained events (indices 1..N-1 of original batch).
    for (uint32_t i = 0u; i < slot.event_count; ++i)
    {
        if (slot.events[i] != nullptr)
        {
            slot.events[i]->destroy();
            slot.events[i] = nullptr;
        }
    }

    // Clear trigger_msg: drops its ref on the SharedBuffer.
    // Once this ref drops to 1 (only the pool holds it), the hot loop
    // sees is_shared() == false and is safe to patch the buffer again.
    slot.trigger_msg = RemoteMessage{ };
    slot.hi_id       = 0u;
    slot.event_count = 0u;

    mZerocopyRingTail = (mZerocopyRingTail + 1u) % ZEROCOPY_RING_SIZE;
    --mZerocopyRingCount;
}

void ClientSendThread::_drain_available() noexcept
{
    if (mZerocopyRingCount == 0u)
        return;

    areg::socket_drain_zerocopy_nb(mConnection.socket().handle(), mZerocopyConfirmed);

    // Release all slots whose hi_id is confirmed by the global watermark.
    while (mZerocopyRingCount > 0u)
    {
        ZerocopyEntry& tail = mZerocopyRing[mZerocopyRingTail];
        // Wrap-safe: (int32_t)(confirmed - hi_id) >= 0 iff hi_id <= confirmed.
        if (static_cast<int32_t>(mZerocopyConfirmed - tail.hi_id) < 0)
            break;

        _release_ring_slot(tail);
    }
}

void ClientSendThread::_drain_oldest_blocking() noexcept
{
    if (mZerocopyRingCount == 0u)
        return;

    ZerocopyEntry& tail = mZerocopyRing[mZerocopyRingTail];
    // Block until the kernel confirms the oldest slot's hi_id.
    areg::socket_drain_zerocopy(mConnection.socket().handle(), tail.hi_id);

    if (static_cast<int32_t>(tail.hi_id - mZerocopyConfirmed) > 0)
        mZerocopyConfirmed = tail.hi_id;

    _release_ring_slot(tail);
}

void ClientSendThread::_flush_ring() noexcept
{
    // Force-release all occupied ring slots without waiting for ERRQUEUE.
    // Called during shutdown when the socket is about to be closed; the kernel
    // will not issue further DMA or ERRQUEUE completions after close.
    while (mZerocopyRingCount > 0u)
    {
        ZerocopyEntry& tail = mZerocopyRing[mZerocopyRingTail];
        _release_ring_slot(tail);
    }
}

} // namespace areg

#endif  // __linux__
