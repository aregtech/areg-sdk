/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2022 Aregtech UG. All rights reserved.
 * \file        areg/component/private/posix/TimerBasePosix.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Timer base class implementation.
 *              Posix specific implementation.
 *
 ************************************************************************/

#include "areg/component/TimerBase.hpp"

#if defined(_POSIX) || defined(POSIX)

#include "areg/component/private/posix/TimerPosix.hpp"

 //////////////////////////////////////////////////////////////////////////
 // Posix specific methods
 //////////////////////////////////////////////////////////////////////////

TIMERHANDLE TimerBase::_osCreateWaitableTimer( void )
{
    return static_cast<TIMERHANDLE>(DEBUG_NEW TimerPosix( ));
}

void TimerBase::_osDestroyWaitableTimer( TIMERHANDLE handle )
{
    TimerPosix * timer = reinterpret_cast<TimerPosix *>(handle);
    timer->destroyTimer( );
    delete timer;
}

#endif  // defined(_POSIX) || defined(POSIX)
