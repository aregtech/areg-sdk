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

#include "areg/component/TimerBase.hpp"

#ifdef  _WIN32

#include <Windows.h>

TIMERHANDLE TimerBase::_osCreateWaitableTimer()
{
    TCHAR * name{ nullptr };
    TCHAR convertName[MAX_PATH];

    if ( mName.isEmpty( ) == false )
    {
        NEString::copyString<TCHAR, char>( convertName, MAX_PATH, mName.getString( ), mName.getLength( ) );
        name = convertName;
    }

    return static_cast<TIMERHANDLE>(::CreateWaitableTimer( nullptr, FALSE, name ));
}

void TimerBase::_osDestroyWaitableTimer( TIMERHANDLE handle )
{
    ::CancelWaitableTimer( static_cast<HANDLE>(handle) );
    ::CloseHandle( static_cast<HANDLE>(handle) );
}

#endif // _WIN32
