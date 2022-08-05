/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        areg/component/private/Watchdog.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Thread watchdog object implementation.
 *              Windows OS specific implementation.
 *
 ************************************************************************/

#include "areg/component/private/Watchdog.hpp"

#ifdef  _WINDOWS

#include "areg/base/String.hpp"

#include <windows.h>

TIMERHANDLE Watchdog::_createWaitableTimer(const Watchdog& watchdog)
{
    TIMERHANDLE result = nullptr;

    if ((watchdog.getTimeout() != Watchdog::NO_WATCHDOG))
    {
        TCHAR* name = nullptr;
        TCHAR convertName[MAX_PATH];

        const String& timerName = watchdog.getName();

        if (timerName.isEmpty() == false)
        {
            NEString::copyString<TCHAR, char>(convertName, MAX_PATH, timerName.getString(), timerName.getLength());
            name = convertName;
        }

        result = static_cast<TIMERHANDLE>(::CreateWaitableTimer(nullptr, FALSE, name));

#ifdef _DEBUG
        if (result == nullptr)
        {
            OUTPUT_ERR("Failed creating timer [ %s ], the system error code is [ 0xp ]", timerName.getString(), GetLastError());
        }
#endif // _DEBUG
    }

    return result;
}

void Watchdog::_destroyWaitableTimer(const Watchdog& watchdog)
{
    if (watchdog.getHandle() != nullptr)
    {
        ::CancelWaitableTimer(static_cast<HANDLE>(watchdog.getHandle()));
        ::CloseHandle(static_cast<HANDLE>(watchdog.getHandle()));
    }
}

#endif // _WINDOWS
