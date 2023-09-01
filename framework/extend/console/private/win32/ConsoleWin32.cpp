
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extend/console/private/win32/Console.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Basic OS specific console implementation.
 *              Windows specific implementation based on Win32 API.
 * 
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "extend/console/Console.hpp"

#if defined(WINDOWS) && (AREG_EXTENDED)

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

#ifdef MS_VISUAL_CPP
    #define SCAN_S(fmt, buff, size)   scanf_s((fmt), (buff), size)
#else   // !MS_VISUAL_CPP
    #define SCAN_S(fmt, buff, size)   scanf((fmt), (buff))
#endif  // MS_VISUAL_CPP

namespace
{
    //!< Clear the screen.
    constexpr std::string_view  CMD_CLEAR_SCREEN    { "\x1B[2J" };
    //!< Scroll cursor back.
    constexpr std::string_view  CMD_SCROLL_BACK     { "\x1B[3J" };
    //!< Clear line.
    constexpr std::string_view  CMD_CLEAR_LINE      { "\x1B[2K" };
    //!< Reset.
    constexpr std::string_view  CMD_RESET           { "\x1B[0m" };
    //!< The command to save cursor position in memory
    constexpr std::string_view  CMD_SAVE_CURSOR     { "\x1B[s" };
    //!< The command to restore previously saved cursor position
    constexpr std::string_view  CMD_RESTORE_CURSOR  { "\x1B[u" };
    //!< The command to move cursor one line up from current position
    constexpr std::string_view  CMD_ONE_LINE_UP     { "\x1B[1F" };
    //!< The command to move cursor one line down from current position
    constexpr std::string_view  CMD_ONE_LINE_DOWN   { "\x1B[1E" };
}

//////////////////////////////////////////////////////////////////////////
// Console Windows OS specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_osSetup(void)
{
    if (mIsReady == false)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if ((hStdOut != nullptr) && (GetConsoleMode(hStdOut, &mode) == TRUE))
        {
            mContext = static_cast<ptr_type>(mode);
            mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hStdOut, mode);
            DWORD written = 0;
            if (WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.length()), &written, NULL) == TRUE)
            {
                written = 0;
                WriteConsoleA(hStdOut, CMD_RESET.data(), static_cast<DWORD>(CMD_RESET.length()), &written, NULL);
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

void Console::_osRelease(void)
{
    if (mIsReady)
    {
        // restore previous mode.
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written = 0;
        WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.length()), &written, NULL);
        written = 0;
        WriteConsoleA(hStdOut, CMD_RESET.data(), static_cast<DWORD>(CMD_RESET.length()), &written, NULL);
        DWORD mode = static_cast<DWORD>(mContext);
        SetConsoleMode(hStdOut, mode);
        mContext = 0;
        mIsReady = false;
    }
}

void Console::_osOutputText(Console::Coord pos, const String& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{static_cast<int16_t>(pos.posX), static_cast<int16_t>(pos.posY)});
    WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.length()), &written, NULL);
    WriteConsoleA(hStdOut, text.getString(), static_cast<DWORD>(text.getLength()), &written, NULL);
}

void Console::_osOutputText(Console::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{ static_cast<int16_t>(pos.posX), static_cast<int16_t>(pos.posY) });
    WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.length()), &written, NULL);
    WriteConsoleA(hStdOut, text.data(), static_cast<DWORD>(text.length()), &written, NULL);
}

void Console::_osOutputText(const String& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, text.getString(), static_cast<DWORD>(text.getLength()), &written, NULL);
}

void Console::_osOutputText(const std::string_view& text) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, text.data(), static_cast<DWORD>(text.length()), &written, NULL);
}

Console::Coord Console::_osGetCursorPosition(void) const
{
    Lock lock(mLock);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    NEMemory::memZero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hStdOut, &bufferInfo);
    const COORD& coord = reinterpret_cast<const COORD&>(bufferInfo.dwCursorPosition);

    return Console::Coord{ coord.X, coord.Y };
}

void Console::_osSetCursorCurPosition(Console::Coord pos) const
{
    Lock lock(mLock);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hStdOut, COORD{ static_cast<int16_t>(pos.posX), static_cast<int16_t>(pos.posY) });
}

void Console::_osWaitInput(const char * fmt, char* buffer, uint32_t size) const
{
    ASSERT(buffer != nullptr);
    if (SCAN_S(fmt, buffer, size) <= 0)
    {
        *buffer = '\0';
    }
}

void Console::_osRefreshScreen(void) const
{
    Lock lock(mLock);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    FlushConsoleInputBuffer(hStdOut);
}

void Console::_osClearLine( void ) const
{
    Lock lock(mLock);

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, CMD_CLEAR_LINE.data(), static_cast<DWORD>(CMD_CLEAR_LINE.length()), &written, NULL);
}

void Console::_osClearScreen( void ) const
{
    Lock lock(mLock);

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut != nullptr)
    {
        DWORD written = 0;
        WriteConsoleA(hStdOut, CMD_CLEAR_SCREEN.data(), static_cast<DWORD>(CMD_CLEAR_SCREEN.length()), &written, NULL);
    }
}

bool Console::_osReadInputList(const char* format, va_list varList) const
{
    return (vscanf(format, varList) > 0);
}

void Console::_osSaveCursorPosition(void) const
{
    Lock lock(mLock);
    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, CMD_SAVE_CURSOR.data(), static_cast<DWORD>(CMD_SAVE_CURSOR.length()), &written, NULL);
}

void Console::_osRestoreCursorPosition(void) const
{
    Lock lock(mLock);
    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, CMD_RESTORE_CURSOR.data(), static_cast<DWORD>(CMD_RESTORE_CURSOR.length()), &written, NULL);
}

void Console::_osMoveCursorOneLineUp(void) const
{
    Lock lock(mLock);
    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, CMD_ONE_LINE_UP.data(), static_cast<DWORD>(CMD_ONE_LINE_UP.length()), &written, NULL);
}

void Console::_osMoveCursorOneLineDown(void) const
{
    Lock lock(mLock);
    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, CMD_ONE_LINE_DOWN.data(), static_cast<DWORD>(CMD_ONE_LINE_DOWN.length()), &written, NULL);
}

#endif  // defined(WINDOWS) && (AREG_EXTENDED)
