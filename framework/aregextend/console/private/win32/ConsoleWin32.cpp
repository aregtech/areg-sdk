
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/win32/ConsoleWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Basic OS specific console implementation.
 *              Windows specific implementation based on Win32 API.
 * 
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"

#if defined(_WIN32) && (AREG_EXTENDED)

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <string.h>

namespace {

    thread_local areg::ext::Console::Coord _savedCursorPos{ 0, 0 };
    thread_local bool _hasSavedCursorPos{ false };

    /**
     * \brief   True when the standard output of the process is attached to a real Windows
     *          console. When it is redirected to a file or to a pipe -- a build server, a
     *          system service, a shell pipeline -- none of the console API calls succeed,
     *          so every screen operation falls back to plain stream output. The flag is a
     *          property of the process, and Console is a singleton, so it is set once in
     *          Console::_os_setup() and read without synchronization afterwards.
     **/
    bool _isConsole{ false };

    /**
     * \brief   Writes the text to the standard output stream. Used when the process has no
     *          console: positioned output degrades to one line per call, and the stream is
     *          flushed so that the output survives a termination of the process.
     **/
    void _write_stream(const char* data, uint32_t len, bool newLine)
    {
        if ((data != nullptr) && (len != 0u))
        {
            ::fwrite(data, 1u, static_cast<size_t>(len), stdout);
        }

        if (newLine)
        {
            ::fputc('\n', stdout);
        }

        ::fflush(stdout);
    }

    /**
     * \brief   Writes text at the given position WITHOUT moving the visible
     *          cursor.  First clears the line from posX to the right edge,
     *          then writes the text.  The cursor stays wherever it was (e.g.
     *          at the input prompt), so gets_s / fgets are not disturbed.
     *          Newline and carriage-return characters are replaced with spaces
     *          because WriteConsoleOutputCharacterA renders them as glyphs (?).
     **/
    void _write_at(HANDLE hOut, SHORT posX, SHORT posY, const char* data, DWORD len)
    {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        GetConsoleScreenBufferInfo(hOut, &info);

        const SHORT width = info.dwSize.X;
        const COORD writePos{ posX, posY };

        // Clear the line from posX to the right edge.
        DWORD filled{ 0 };
        FillConsoleOutputCharacterA(hOut, ' ', static_cast<DWORD>(width - posX), writePos, &filled);
        FillConsoleOutputAttribute(hOut, info.wAttributes, static_cast<DWORD>(width - posX), writePos, &filled);

        // Strip trailing \r and \n: WriteConsoleOutputCharacterA renders them
        // as visible glyphs (?) instead of control characters.
        while ((len > 0) && ((data[len - 1] == '\n') || (data[len - 1] == '\r')))
        {
            --len;
        }

        // Write the text at the position (cursor does not move).
        DWORD written{ 0 };
        WriteConsoleOutputCharacterA(hOut, data, len, writePos, &written);
    }

} // namespace

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console Windows OS specific implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_os_setup() noexcept
{
    if (mIsReady == false)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        _isConsole = (hStdOut != nullptr) && (hStdOut != INVALID_HANDLE_VALUE) &&
                     (GetConsoleMode(hStdOut, &mode) == TRUE);
        if (_isConsole == false)
        {
            // The output is redirected. There is no screen to control, but the object must
            // still report itself ready: Console::enable_console_input() refuses to enable
            // the input of an object that is not ready, and an application that cannot read
            // its input never reaches its own exit point.
            mContext = 0;
            mIsReady = true;
        }
        else
        {
            mContext = static_cast<ptr_type>(mode);
            mode |= ENABLE_PROCESSED_OUTPUT;
            SetConsoleMode(hStdOut, mode);

            // Clear the screen using Win32 API (no VT sequences needed).
            CONSOLE_SCREEN_BUFFER_INFO info{};
            if (GetConsoleScreenBufferInfo(hStdOut, &info))
            {
                DWORD cells = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
                DWORD filled{ 0 };
                COORD origin{ 0, 0 };
                FillConsoleOutputCharacterA(hStdOut, ' ', cells, origin, &filled);
                FillConsoleOutputAttribute(hStdOut, info.wAttributes, cells, origin, &filled);
                SetConsoleCursorPosition(hStdOut, origin);
                mIsReady = true;
            }
            else
            {
                mode = static_cast<DWORD>(mContext);
                SetConsoleMode(hStdOut, mode);
                mContext = 0;
            }
        }
    }

    return mIsReady;
}

void Console::_os_release() noexcept
{
    if (mIsReady && (_isConsole == false))
    {
        ::fflush(stdout);
        mContext = 0;
        mIsReady = false;
    }
    else if (mIsReady)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        // Ensure the cursor is visible before exiting.
        CONSOLE_CURSOR_INFO ci{};
        if ((hStdOut != nullptr) && GetConsoleCursorInfo(hStdOut, &ci))
        {
            ci.bVisible = TRUE;
            SetConsoleCursorInfo(hStdOut, &ci);
        }

        CONSOLE_SCREEN_BUFFER_INFO info{};
        if ((hStdOut != nullptr) && GetConsoleScreenBufferInfo(hStdOut, &info))
        {
            const SHORT finalRow = static_cast<SHORT>(mMaxUsedRow + 1);
            COORD pos{ 0, (finalRow < info.dwSize.Y) ? finalRow : static_cast<SHORT>(info.dwSize.Y - 1) };
            SetConsoleCursorPosition(hStdOut, pos);
            DWORD written{ 0 };
            WriteConsoleA(hStdOut, "\n", 1, &written, nullptr);
        }

        DWORD mode = static_cast<DWORD>(mContext);
        SetConsoleMode(hStdOut, mode);
        mContext  = 0;
        mIsReady  = false;
    }
}

void Console::_os_output_text(Console::Coord pos, const String& text) const noexcept
{
    Lock lock(mLock);
    DWORD len = static_cast<DWORD>(text.length());
    if (_isConsole)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        _write_at(hStdOut, static_cast<SHORT>(pos.posX), static_cast<SHORT>(pos.posY), text.as_string(), len);
    }
    else
    {
        _write_stream(text.as_string(), static_cast<uint32_t>(len), true);
    }

    if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
    {
        mMaxUsedRow = static_cast<int32_t>(pos.posY);
    }
}

void Console::_os_output_text(Console::Coord pos, std::string_view text) const noexcept
{
    Lock lock(mLock);
    DWORD len = static_cast<DWORD>(text.length());
    if (_isConsole)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        _write_at(hStdOut, static_cast<SHORT>(pos.posX), static_cast<SHORT>(pos.posY),
                  text.data(), len);
    }
    else
    {
        _write_stream(text.data(), static_cast<uint32_t>(len), true);
    }

    if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
    {
        mMaxUsedRow = static_cast<int32_t>(pos.posY);
    }
}

void Console::_os_output_text(const String& text) const noexcept
{
    Lock lock(mLock);

    if (_isConsole == false)
    {
        _write_stream(text.as_string(), static_cast<uint32_t>(text.length()), false);
        return;
    }

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, text.as_string(), static_cast<DWORD>(text.length()), &written, nullptr);
}

void Console::_os_output_text(std::string_view text) const noexcept
{
    Lock lock(mLock);

    if (_isConsole == false)
    {
        _write_stream(text.data(), static_cast<uint32_t>(text.length()), false);
        return;
    }

    DWORD written = 0;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    WriteConsoleA(hStdOut, text.data(), static_cast<DWORD>(text.length()), &written, nullptr);
}

Console::Coord Console::_os_get_cursor_position() const noexcept
{
    Lock lock(mLock);

    if (_isConsole == false)
    {
        return mSavedPos;
    }

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    areg::mem_zero(&bufferInfo, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
    GetConsoleScreenBufferInfo(hStdOut, &bufferInfo);
    const COORD& coord = reinterpret_cast<const COORD&>(bufferInfo.dwCursorPosition);

    return Console::Coord{ coord.X, coord.Y };
}

void Console::_os_set_cursor_cur_position(Console::Coord pos) const noexcept
{
    Lock lock(mLock);

    mSavedPos = { static_cast<int16_t>(pos.posX), static_cast<int16_t>(pos.posY) };
    if (_isConsole)
    {
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(hStdOut, COORD{ static_cast<int16_t>(pos.posX), static_cast<int16_t>(pos.posY) });
    }
}

bool Console::_os_wait_input_string(char* buffer, uint32_t size)
{
    if (_isConsole == false)
    {
        // Redirected input. 'gets_s' aborts the process when the line does not fit, and it
        // is a console function; a redirected stream is read with 'fgets', which keeps the
        // line separator that the callers do not expect, so it is removed here.
        if (::fgets(buffer, static_cast<int>(size), stdin) == nullptr)
        {
            return false;
        }

        size_t len = ::strlen(buffer);
        while ((len > 0u) && ((buffer[len - 1u] == '\n') || (buffer[len - 1u] == '\r')))
        {
            buffer[--len] = '\0';
        }

        return true;
    }

#if !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    #if defined(_WIN32) && !defined(_MINGW)
        return (::gets_s(buffer, size) != nullptr);
    #else   // defined(_WIN32)
        return (::fgets(buffer, size, stdin) != nullptr);
    #endif  // defined(_WIN32)
#else   // !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
    return (::gets_s(buffer, size) != nullptr);
#endif  // !defined(__STDC_WANT_LIB_EXT1__) || !(__STDC_WANT_LIB_EXT1__)
}

void Console::_os_interrupt_input() noexcept
{
    if (_isConsole == false)
    {
        // Nothing to inject: there is no console input queue. A reader that is parked in
        // 'fgets' leaves when the writing end of the redirected input is closed.
        return;
    }

    HANDLE hIn = ::GetStdHandle(STD_INPUT_HANDLE);
    if ((hIn == INVALID_HANDLE_VALUE) || (hIn == nullptr))
        return;

    INPUT_RECORD ir[2]{};
    ir[0].EventType                        = KEY_EVENT;
    ir[0].Event.KeyEvent.bKeyDown          = TRUE;
    ir[0].Event.KeyEvent.wRepeatCount      = 1;
    ir[0].Event.KeyEvent.wVirtualKeyCode   = VK_RETURN;
    ir[0].Event.KeyEvent.wVirtualScanCode  = 0x1Cu;
    ir[0].Event.KeyEvent.uChar.AsciiChar   = '\r';
    ir[0].Event.KeyEvent.dwControlKeyState = 0;
    ir[1]                                  = ir[0];
    ir[1].Event.KeyEvent.bKeyDown          = FALSE;

    DWORD written{ 0u };
    ::WriteConsoleInputA(hIn, ir, 2, &written);
}

void Console::_os_refresh_screen() const noexcept
{
    // no-op
}

void Console::_os_clear_line() const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(hStdOut, &info))
    {
        DWORD filled{ 0 };
        const COORD curPos = info.dwCursorPosition;
        const DWORD clearLen = static_cast<DWORD>(info.dwSize.X - curPos.X);
        FillConsoleOutputCharacterA(hStdOut, ' ', clearLen, curPos, &filled);
        FillConsoleOutputAttribute(hStdOut, info.wAttributes, clearLen, curPos, &filled);
    }
}

void Console::_os_clear_line_at_position(Console::Coord pos) const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(hStdOut, &info))
    {
        const COORD writePos{ static_cast<SHORT>(pos.posX), static_cast<SHORT>(pos.posY) };
        const DWORD clearLen = static_cast<DWORD>(info.dwSize.X - pos.posX);
        DWORD filled{ 0 };
        FillConsoleOutputCharacterA(hStdOut, ' ', clearLen, writePos, &filled);
        FillConsoleOutputAttribute(hStdOut, info.wAttributes, clearLen, writePos, &filled);
    }
}

void Console::_os_clear_screen() const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if ((hStdOut != nullptr) && GetConsoleScreenBufferInfo(hStdOut, &info))
    {
        DWORD cells = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
        DWORD filled{ 0 };
        COORD origin{ 0, 0 };
        FillConsoleOutputCharacterA(hStdOut, ' ', cells, origin, &filled);
        FillConsoleOutputAttribute(hStdOut, info.wAttributes, cells, origin, &filled);
        SetConsoleCursorPosition(hStdOut, origin);
    }
}

bool Console::_os_read_input_list(const char* format, va_list varList) const
{
    return (vscanf(format, varList) > 0);
}

void Console::_os_save_cursor_position() const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO info{};
    if ((hStdOut != nullptr) && (GetConsoleScreenBufferInfo(hStdOut, &info) == TRUE))
    {
        _savedCursorPos   = { info.dwCursorPosition.X, info.dwCursorPosition.Y };
        _hasSavedCursorPos = true;
    }
    else
    {
        _savedCursorPos   = mSavedPos;
        _hasSavedCursorPos = true;
    }
}

void Console::_os_restore_cursor_position() const noexcept
{
    Lock lock(mLock);
    if (_isConsole == false)
    {
        _hasSavedCursorPos = false;
        return;
    }

    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    const Console::Coord target = _hasSavedCursorPos ? _savedCursorPos : mSavedPos;

    CONSOLE_SCREEN_BUFFER_INFO info{};
    SHORT targetX{ static_cast<SHORT>(target.posX) };
    SHORT targetY{ static_cast<SHORT>(target.posY) };
    if ((hStdOut != nullptr) && (GetConsoleScreenBufferInfo(hStdOut, &info) == TRUE))
    {
        targetX = static_cast<SHORT>(targetX < 0 ? 0 : (targetX >= info.dwSize.X ? info.dwSize.X - 1 : targetX));
        targetY = static_cast<SHORT>(targetY < 0 ? 0 : (targetY >= info.dwSize.Y ? info.dwSize.Y - 1 : targetY));
    }

    SetConsoleCursorPosition(hStdOut, COORD{ targetX, targetY });
    _hasSavedCursorPos = false;
}

void Console::_os_move_cursor_one_line_up() const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(hStdOut, &info) && info.dwCursorPosition.Y > 0)
    {
        COORD pos{ 0, static_cast<SHORT>(info.dwCursorPosition.Y - 1) };
        SetConsoleCursorPosition(hStdOut, pos);
    }
}

void Console::_os_move_cursor_one_line_down() const noexcept
{
    Lock lock(mLock);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(hStdOut, &info))
    {
        COORD pos{ 0, static_cast<SHORT>(info.dwCursorPosition.Y + 1) };
        SetConsoleCursorPosition(hStdOut, pos);
    }
}

} // namespace areg::ext

#endif  // defined(_WIN32) && (AREG_EXTENDED)
