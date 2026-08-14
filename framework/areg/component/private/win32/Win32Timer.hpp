#ifndef AREG_COMPONENT_PRIVATE_WIN32_WIN32TIMER_HPP
#define AREG_COMPONENT_PRIVATE_WIN32_WIN32TIMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/component/private/win32/Win32Timer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, lightweight single-object event - Windows implementation.
 ************************************************************************/
 /************************************************************************
  * Includes
  ************************************************************************/

#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>

namespace areg::os
{
    /**
     * \brief   Win32 specific timer handler object
     **/
    struct Win32TimerHandle
    {
        PTP_TIMER   timerPool   { nullptr };    //!< Thread-pool timer, created lazily.
        uintptr_t   contextId   { 0 };          //!< Watchdog identifier of the current arming.
    };
}

#endif  // AREG_COMPONENT_PRIVATE_WIN32_WIN32TIMER_HPP
