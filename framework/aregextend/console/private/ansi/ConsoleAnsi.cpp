/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/private/posix/ConsolePosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Basic OS specific console implementation
 *              based on standard ANSI C API, which can be used under any OS.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"

#if !(AREG_EXTENDED)

#ifdef _WIN32

    // This is required only to enable ASCII control sequence on Windows console.
    // Ignored in other cases.
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif  // WIN32_LEAN_AND_MEAN
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif // !NOMINMAX
    #include <Windows.h>

#endif // _WIN32

#include <stdio.h>

namespace {
    //!< Clear the screen.
    constexpr std::string_view  CMD_CLEAR_SCREEN{ "\x1B[2J\x1B[1;1f" };
    //!< Clear line.
    constexpr std::string_view  CMD_CLEAR_LINE  { "\x1B[2K" };

    //!< Enables the ASCII control sequence for applications compiled with Win32 API.
    void _enableAsciiControlSequence()
    {

#ifdef _WIN32
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

#endif // _WIN32

    }
} // namespace

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console Windows OS specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_os_setup()
{
    _enableAsciiControlSequence();
    mIsReady = true;
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
    return mIsReady;
}

void Console::_os_release()
{
    mIsReady = false;
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
}

void Console::_os_output_text(Console::Coord pos, const String& text) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH%s%s", pos.posY, pos.posX, CMD_CLEAR_LINE.data(), text.as_string());
}

void Console::_os_output_text(Console::Coord pos, const std::string_view& text) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH%s%s", pos.posY, pos.posX, CMD_CLEAR_LINE.data(), text.data());
}

void Console::_os_output_text(const String& text) const
{
    Lock lock(mLock);
    printf("%s", text.as_string());
}

void Console::_os_output_text(const std::string_view& text) const
{
    Lock lock(mLock);
    printf("%s", text.data());
}

Console::Coord Console::_os_get_cursor_position() const
{
    Lock lock(mLock);
    constexpr int32_t _EOY{ static_cast<int32_t>(';') };
    constexpr int32_t _EOX{ static_cast<int32_t>('R') };
    constexpr int32_t _ZERO{ static_cast<int32_t>('0') };

    Console::Coord result{ 0, 0 };
    printf("\x1B[6n");
    if ((getchar() == '\x1B') && (getchar() == '['))
    {
        int32_t ch = getchar();
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

void Console::_os_set_cursor_cur_position(Console::Coord pos) const
{
    Lock lock(mLock);
    printf("\x1B[%d;%dH", pos.posY, pos.posX);
}

bool Console::_os_wait_input_string(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);
#if !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    #if defined(_WIN32) && !defined(_MINGW)
        if (::gets_s(buffer, size) == nullptr)
            return false;
    #else   // defined(_WIN32)
        if (::fgets(buffer, size, stdin) == nullptr)
            return false;
    #endif  // defined(_WIN32)
#else  // !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    #if defined(_POSIX) || defined(POSIX)
        if (::fgets(buffer, size, stdin) == nullptr)
            return false;
    #else // defined(_POSIX) || defined(POSIX)
        if (::gets_s(buffer, size) == nullptr)
            return false;
    #endif // defined(_POSIX) || defined(POSIX)
#endif // !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)

    areg::trim_all<char>(buffer);
    return ( areg::is_empty(buffer) == false );
}

void Console::_os_refresh_screen() const
{
    Lock lock(mLock);
    ::fflush(stdout);
}

void Console::_os_clear_line() const
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_LINE.data());
    ::fflush(stdout);
}

void Console::_os_clear_screen() const
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
}

bool Console::_os_read_input_list(const char* format, va_list varList) const
{
    return (vscanf(format, varList) > 0);
}

void Console::_os_save_cursor_position() const
{
    Lock lock(mLock);
    printf("\x1B[s");
}

void Console::_os_restore_cursor_position() const
{
    Lock lock(mLock);
    printf("\x1B[u");
}

void Console::_os_move_cursor_one_line_up() const
{
    Lock lock(mLock);
    printf("\x1B[1F");
}

void Console::_os_move_cursor_one_line_down() const
{
    Lock lock(mLock);
    printf("\x1B[1E");
}

} // namespace areg::ext

#endif  // !(AREG_EXTENDED)
