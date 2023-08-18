/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        extensions/console/private/posix/ConsolePosix.cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Basic OS specific console implementation
 *              based on standard ANSI C API, which can be used under any OS.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "extensions/console/Console.hpp"

#if !(AREG_EXTENDED)

#ifdef WINDOWS

    // This is required only to enable ASCII control sequence on Windows console.
    // Ignored in other cases.
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #include <Windows.h>

#endif // WINDOWS

#include <stdio.h>

#ifdef MS_VISUAL_CPP
    #define SCAN_S(fmt, buff, size)   scanf_s((fmt), (buff), size)
#else   // !MS_VISUAL_CPP
    #define SCAN_S(fmt, buff, size)   scanf((fmt), (buff))
#endif  // MS_VISUAL_CPP

namespace
{
    //!< Clear the screen.
    constexpr std::string_view  CMD_CLEAR_SCREEN{ "\x1B[2J\x1B[1;1f" };
    //!< Clear line.
    constexpr std::string_view  CMD_CLEAR_LINE  { "\x1B[2K" };

    //!< Enables the ASCII control sequence for applications compiled with Win32 API.
    void _enableAsciiControlSequence(void)
    {

#ifdef WINDOWS
        //////////////////////////////////////////////////////////////
        // 
        // The ASCII control sequence (or ANSI Escape Sequences) may not work properly
        // on Windows system (in particular, it may not work in cmd.exe console, but may
        // work in Power Shell). To make it running properly, there is a need to enable
        // the sequence code in following way:
        // On Windows system this might not work by default and the additional work would require.
        //
        //////////////////////////////////////////////////////////////

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);

        //////////////////////////////////////////////////////////////
        // 
        // Once it is enabled, it may display right characters.
        // Otherwise, it may display errors characters on Windows console.
        // Opposite to this, the PowerShell does not require any additional enabling.
        // These methods are part of <Windows.h>
        //
        //////////////////////////////////////////////////////////////

#endif // WINDOWS

    }
}

//////////////////////////////////////////////////////////////////////////
// Console Windows OS specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_osSetup(void)
{
    _enableAsciiControlSequence();
    mIsReady = true;
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
    return mIsReady;
}

void Console::_osRelease(void)
{
    mIsReady = false;
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
}

void Console::_osOutputText(Console::Coord pos, const String& text) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH%s%s", pos.posY, pos.posX, CMD_CLEAR_LINE.data(), text.getString());
}

void Console::_osOutputText(Console::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH%s%s", pos.posY, pos.posX, CMD_CLEAR_LINE.data(), text.data());
}

void Console::_osOutputText(const String& text) const
{
    Lock lock(mLock);
    printf("%s", text.getString());
}

void Console::_osOutputText(const std::string_view& text) const
{
    Lock lock(mLock);
    printf("%s", text.data());
}

Console::Coord Console::_osGetCursorPosition(void) const
{
    Lock lock(mLock);
    constexpr int _EOY{ static_cast<int>(';') };
    constexpr int _EOX{ static_cast<int>('R') };
    constexpr int _ZERO{ static_cast<int>('0') };

    Console::Coord result{ 0, 0 };
    printf("\x1B[6n");
    if ((getchar() == '\x1B') && (getchar() == '['))
    {
        int ch = getchar();
        while (ch != _EOY)
        {
            result.posY = result.posY * 10 + (ch - _ZERO);
        }

        ch = getchar();
        while (ch != _EOX)
        {
            result.posX = result.posX * 10 + (ch - _ZERO);
        }
    }

    return result;
}

void Console::_osSetCursorCurPosition(Console::Coord pos) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH", pos.posY, pos.posX);
}

void Console::_osWaitInput(char* buffer, uint32_t size) const
{
    int result = SCAN_S("%s", buffer, size);
    if ((result < 0) && (buffer != nullptr))
    {
        *buffer = '\0';
    }
}

void Console::_osRefreshScreen(void) const
{
    Lock lock(mLock);
    ::fflush(stdout);
}

void Console::_osClearLine( void ) const
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_LINE.data());
    ::fflush(stdout);
}

void Console::_osClearScreen( void ) const
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
}

bool Console::_osReadInputList(const char* format, va_list varList) const
{
    return (vscanf(format, varList) > 0);
}

void Console::_osSaveCursorPosition(void) const
{
    Lock lock(mLock);
    printf("\x1B[s");
}

void Console::_osRestoreCursorPosition(void) const
{
    Lock lock(mLock);
    printf("\x1B[u");
}

void Console::_osMoveCursorOneLineUp(void) const
{
    Lock lock(mLock);
    printf("\x1B[1F");
}

void Console::_osMoveCursorOneLineDown(void) const
{
    Lock lock(mLock);
    printf("\x1B[1E");
}

#endif  // !(AREG_EXTENDED)
