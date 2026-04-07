/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerBasePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Timer base class implementation.
 *              Posix specific implementation.
 *
 ************************************************************************/

#if defined(_POSIX) || defined(POSIX)

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/component/TimerBase.hpp"
#include "areg/component/private/posix/TimerPosix.hpp"

namespace areg {

 //////////////////////////////////////////////////////////////////////////
 // Posix specific methods
 //////////////////////////////////////////////////////////////////////////

TIMERHANDLE TimerBase::_os_create() noexcept
{
    return static_cast<TIMERHANDLE>(DEBUG_NEW areg::os::TimerPosix( ));
}

void TimerBase::_os_destroy( TIMERHANDLE handle ) noexcept
{
    areg::os::TimerPosix * timer = reinterpret_cast<areg::os::TimerPosix *>(handle);
    timer->destroy_timer( );
    delete timer;
}

} // namespace areg
#endif  // defined(_POSIX) || defined(POSIX)
