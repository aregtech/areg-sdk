/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/ConsoleWin32.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing console that outputs stistics.
 *              Windows OS specific part implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"

#ifdef WINDOWS

#include "areg/base/NEMemory.hpp"

#include <windows.h>


bool Console::_osInitialize( void )
{
    if (mSetupEnv == false)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if ((hStdOut != nullptr) && (GetConsoleMode(hStdOut, &mode) == TRUE))
        {
            mContext = static_cast<ptr_type>(mode);
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hStdOut, mode);
            DWORD written = 0;
            if (WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.size()), &written, NULL) == TRUE)
            {
                written = 0;
                WriteConsoleA(hStdOut, CMD_SCROLL_BACK.data(), static_cast<DWORD>(CMD_SCROLL_BACK.size()), &written, NULL);
                mSetupEnv = true;
            }
            else
            {
                // restore previous mode.
                mode = static_cast<DWORD>(mContext);
                SetConsoleMode(hStdOut, mode);
                mContext = 0;
            }
        }
    }

    return mSetupEnv;
}

void Console::_osUnitialize(void)
{
    if (mSetupEnv)
    {
        // restore previous mode.
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = static_cast<DWORD>(mContext);
        SetConsoleMode(hStdOut, mode);
        mContext    = 0;
        mSetupEnv   = false;
    }
}

void Console::_setCursorCurrentPos(const Console::sCoord& pos)
{
    if ((pos.posX >= 0) && (pos.posY >= 0))
    {
        HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
        COORD coord{ pos.posX, pos.posY };
        SetConsoleCursorPosition( h, coord );
    }
}

Console::sCoord Console::_getCursorCurrentPos(void)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    NEMemory::memZero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(h, &bufferInfo);
    const COORD & coord = reinterpret_cast<const COORD &>(bufferInfo.dwCursorPosition);
    return sCoord{ coord.X, coord.Y };
}

#endif  // WINDOWS
