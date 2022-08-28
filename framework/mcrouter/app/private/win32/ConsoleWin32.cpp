/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2021 Aregtech UG. All rights reserved.
 * \file        mcrouter/app/private/Console.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Multi-cast routing, OS specific console.
 *              Windows specific implementation
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "mcrouter/app/private/Console.hpp"

#ifdef WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>

#include "mcrouter/app/NEMulticastRouterSettings.hpp"

bool Console::_osInitialize(void)
{
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
            if (WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.length()), &written, NULL) == TRUE)
            {
                written = 0;
                WriteConsoleA(hStdOut, CMD_SCROLL_BACK.data(), static_cast<DWORD>(CMD_SCROLL_BACK.length()), &written, NULL);
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

    return mIsReady;
}

void Console::_osUninitialize(void)
{
    if (mIsReady)
    {
        // restore previous mode.
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written = 0;
        WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.length()), &written, NULL);
        DWORD mode = static_cast<DWORD>(mContext);
        SetConsoleMode(hStdOut, mode);
        mContext = 0;
        mIsReady = false;
    }
}

void Console::_osOutputText(NEMulticastRouterSettings::Coord pos, const String& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{pos.posX, pos.posY});
    WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.length()), &written, NULL);
    WriteConsoleA(hStdOut, text.getString(), static_cast<DWORD>(text.getLength()), &written, NULL);
}

void Console::_osOutputText(NEMulticastRouterSettings::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{ pos.posX, pos.posY });
    WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.length()), &written, NULL);
    WriteConsoleA(hStdOut, text.data(), static_cast<DWORD>(text.length()), &written, NULL);
}

NEMulticastRouterSettings::Coord Console::_osGetCursorPosition(void) const
{
    Lock lock(mLock);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    NEMemory::memZero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hStdOut, &bufferInfo);
    const COORD& coord = reinterpret_cast<const COORD&>(bufferInfo.dwCursorPosition);

    return NEMulticastRouterSettings::Coord{ coord.X, coord.Y };
}

void Console::_osSetCursorCurPosition(NEMulticastRouterSettings::Coord pos)
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{ pos.posX, pos.posY });
}

void Console::_osWaitInput(void)
{
    char buffer[32]{ 0 };
    static_cast<void>(scanf_s("%s", buffer, 32));

    mUsrInput = buffer;

#if 0
    do
    {
        char ch = getchar();

        if ((ch == '\n') || (ch == static_cast<char>(27)) || (ch == static_cast<char>(0)))
        {
            break;
        }
        else
        {
            mUsrInput += ch;
        }

    } while (true);
#endif
}

void Console::_osRefreshScreen(void) const
{
}

#endif  // WINDOWS

