/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/win32/TimerBaseWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer base class implementation.
 *              Windows OS specific implementation.
 *
 ************************************************************************/

#ifdef  _WIN32

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/TimerBase.hpp"
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <new>

namespace {

struct Win32TimerHandle
{
    HANDLE      timerHandle { nullptr };    //!< Standard WaitableTimer for WatchdogManager APC delivery.
    PTP_TIMER   timerPool   { nullptr };    //!< Thread-pool timer for TimerManager (created lazily).
};

} // namespace

namespace areg {

TIMERHANDLE TimerBase::_os_create() noexcept
{
    Win32TimerHandle * h = new(std::nothrow) Win32TimerHandle{};
    if (h == nullptr)
        return nullptr;

    // Standard (non-high-res) WaitableTimer — supports APC, used by WatchdogManager.
    // TimerManager uses the timerPool field (thread pool) for precision delivery instead.
    h->timerHandle = ::CreateWaitableTimer(nullptr, FALSE, nullptr);
    if (h->timerHandle == nullptr)
    {
        delete h;
        return nullptr;
    }

    return static_cast<TIMERHANDLE>(h);
}

void TimerBase::_os_destroy( TIMERHANDLE handle ) noexcept
{
    Win32TimerHandle * h = static_cast<Win32TimerHandle *>(handle);
    if (h == nullptr)
        return;

    if (h->timerPool != nullptr)
    {
        // Disarm and drain any in-flight callback before closing.
        ::SetThreadpoolTimer(h->timerPool, nullptr, 0, 0);
        ::WaitForThreadpoolTimerCallbacks(h->timerPool, TRUE);
        ::CloseThreadpoolTimer(h->timerPool);
        h->timerPool = nullptr;
    }

    if (h->timerHandle != nullptr)
    {
        ::CancelWaitableTimer(h->timerHandle);
        ::CloseHandle(h->timerHandle);
        h->timerHandle = nullptr;
    }

    delete h;
}

} // namespace areg
#endif // _WIN32
