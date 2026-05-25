/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/win32/SpinSyncEventWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, SpinSyncEvent OS sleep/wake primitives — Windows implementation.
 *              Uses WaitOnAddress / WakeByAddressSingle / WakeByAddressAll.
 *              Requires Windows 8 / Windows Server 2012 or later.
 *
 *              The spin phase and sleeper-count logic live in SpinSyncEvent.cpp.
 *              These functions implement only the OS-level sleep and wake, entered after
 *              the spin phase has already run.
 *
 ************************************************************************/

#ifdef _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/SpinSyncEvent.hpp"

#ifndef NOMINMAX
    #define NOMINMAX
#endif  // !NOMINMAX
#include <Windows.h>
// WaitOnAddress / WakeByAddress are declared in synchapi.h (included via Windows.h)
// and require linking against Synchronization.lib (or the equivalent umbrella library).
#pragma comment(lib, "Synchronization.lib")

#include <algorithm>
#include <climits>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// SpinSyncEvent — Windows OS sleep/wake primitives
//////////////////////////////////////////////////////////////////////////

bool SpinSyncEvent::_os_wait(uint32_t timeout_ms) noexcept
{
    if (timeout_ms == areg::DO_NOT_WAIT)
        return _try_consume();

    const bool      infinite  { timeout_ms == areg::WAIT_INFINITE };
    const ULONGLONG deadline  { infinite ? ULLONG_MAX : GetTickCount64() + static_cast<ULONGLONG>(timeout_ms) };

    for (;;)
    {
        if (_try_consume())
            return true;

        DWORD remaining { INFINITE };
        if (!infinite)
        {
            const ULONGLONG now { GetTickCount64() };
            if (now >= deadline)
                return false;

            remaining = static_cast<DWORD>(
                std::min<ULONGLONG>(deadline - now, static_cast<ULONGLONG>(INFINITE - 1u)));
        }

        // WaitOnAddress atomically checks *(&mState) == expected before sleeping.
        // Returns immediately (FALSE, ERROR_TIMEOUT=FALSE) if mState != 0 at call time.
        uint32_t expected { 0u };
        if (!WaitOnAddress(&mState, &expected, sizeof(expected), remaining))
        {
            if (GetLastError() == ERROR_TIMEOUT)
                return false;
            // Spurious return — re-check via _try_consume at top of loop
        }
    }
}

void SpinSyncEvent::_os_wake() noexcept
{
    if (mAutoReset)
        WakeByAddressSingle(&mState);
    else
        WakeByAddressAll(&mState);
}

void SpinSyncEvent::_os_wake_all() noexcept
{
    WakeByAddressAll(&mState);
}

bool SpinSyncEvent::_os_wait_on(std::atomic<uint32_t>* word, uint32_t timeout_ms) noexcept
{
    if (timeout_ms == areg::DO_NOT_WAIT)
        return (word->load(std::memory_order_acquire) != 0u);

    const bool      infinite { timeout_ms == areg::WAIT_INFINITE };
    const ULONGLONG deadline { infinite ? ULLONG_MAX : GetTickCount64() + static_cast<ULONGLONG>(timeout_ms) };

    for (;;)
    {
        if (word->load(std::memory_order_acquire) != 0u)
            return true;

        DWORD remaining { INFINITE };
        if (!infinite)
        {
            const ULONGLONG now { GetTickCount64() };
            if (now >= deadline)
                return false;

            remaining = static_cast<DWORD>(
                std::min<ULONGLONG>(deadline - now, static_cast<ULONGLONG>(INFINITE - 1u)));
        }

        uint32_t expected { 0u };
        if (!WaitOnAddress(word, &expected, sizeof(expected), remaining))
        {
            if (GetLastError() == ERROR_TIMEOUT)
                return false;
        }
    }
}

void SpinSyncEvent::_os_wake_on(std::atomic<uint32_t>* word) noexcept
{
    WakeByAddressAll(word);
}

} // namespace areg

#endif  // _WIN32
