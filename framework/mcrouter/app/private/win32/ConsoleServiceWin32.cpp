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
 * \brief       AREG Platform, Multi-cast routing ConsoleService that outputs statistics.
 *              Windows OS specific part implementation.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/ConsoleService.hpp"

#ifdef WINDOWS

#include "areg/base/NEMemory.hpp"
#include "areg/component/ComponentThread.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <iostream>
#include <chrono>

#if 0

bool ConsoleService::_osInitialize( void )
{
    Lock lock(mLock);

    if (mIsReady == false)
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
                mIsReady = true;
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
}

void ConsoleService::_osUninitialize(void)
{
    Lock lock(mLock);
    if (mIsReady)
    {
        // restore previous mode.
        HANDLE hStdOut  = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written   = 0;
        WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.size()), &written, NULL);
        DWORD mode = static_cast<DWORD>(mContext);
        SetConsoleMode(hStdOut, mode);
        mContext = 0;
    }
}

void ConsoleService::_osDataRate(uint32_t bytesSent, uint32_t bytesReceive, bool isInit)
{
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    NEMemory::memZero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hStdOut, &bufferInfo);
    const COORD& coord = reinterpret_cast<const COORD&>(bufferInfo.dwCursorPosition);

    do
    {
        SetConsoleCursorPosition(hStdOut, COORD{ 0, 0 });
        ConsoleService::DataRate dataRate(bytesSent);
        std::pair<float, std::string> rate = dataRate.getRate();
        printf(FORMAT_SEND_DATA_W.data(), rate.first, rate.second.c_str());
    } while (false);

    do
    {
        SetConsoleCursorPosition(hStdOut, COORD{ 0, 1 });
        ConsoleService::DataRate dataRate(bytesReceive);
        std::pair<float, std::string> rate = dataRate.getRate();
        printf(FORMAT_RECV_DATA_W.data(), rate.first, rate.second.c_str());
    } while (false);

    if (mDispError)
    {
        DWORD written = 0;
        WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.size()), &written, NULL);
    }

    SetConsoleCursorPosition(hStdOut, COORD{ 0, 2 });
    printf(FORMAT_WAIT_QUIT_W.data(), mCommand.getString());

    if ((isInit == false) && (mDispError == false))
    {
        SetConsoleCursorPosition(hStdOut, coord);
    }

    mDispError = false;
}

#endif

#endif  // WINDOWS
