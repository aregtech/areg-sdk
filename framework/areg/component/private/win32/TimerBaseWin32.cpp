/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
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
#include "areg/component/private/win32/Win32Timer.hpp"
#include <new>

namespace areg {

TIMERHANDLE TimerBase::_os_create() noexcept
{
    areg::os::Win32TimerHandle * h = new(std::nothrow) areg::os::Win32TimerHandle{};
    return static_cast<TIMERHANDLE>(h);
}

void TimerBase::_os_destroy( TIMERHANDLE handle ) noexcept
{
    areg::os::Win32TimerHandle * h = static_cast<areg::os::Win32TimerHandle *>(handle);
    if (h == nullptr)
        return;

    if (h->timerPool != nullptr)
    {
        ::SetThreadpoolTimer(h->timerPool, nullptr, 0, 0);
        ::WaitForThreadpoolTimerCallbacks(h->timerPool, TRUE);
        ::CloseThreadpoolTimer(h->timerPool);
        h->timerPool = nullptr;
    }

    delete h;
}

} // namespace areg
#endif // _WIN32
