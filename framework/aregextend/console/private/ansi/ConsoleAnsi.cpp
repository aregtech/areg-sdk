/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        aregextend/console/private/ansi/ConsoleAnsi.cpp
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

#else   // !_WIN32

    // POSIX raw-mode terminal control for character-by-character input.
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <poll.h>

#endif // _WIN32

#include <stdio.h>

namespace {
    //!< Clear the screen.
    constexpr std::string_view  CMD_CLEAR_SCREEN{ "\x1B[2J\x1B[1;1f" };
    //!< Clear from cursor position to the end of the line.
    constexpr std::string_view  CMD_CLEAR_LINE  { "\x1B[0K" };
    //!< Hide cursor (DECTCEM).
    // constexpr std::string_view  CMD_CURSOR_HIDE { "\x1B[?25l" };
    //!< Show cursor (DECTCEM).
    constexpr std::string_view  CMD_CURSOR_SHOW { "\x1B[?25h" };

    //!< Enables the ASCII control sequence for applications compiled with Win32 API.
    void _enable_ascii_control_sequence()
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

bool Console::_os_setup() noexcept
{
    _enable_ascii_control_sequence();
    mIsReady = true;
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
#ifndef _WIN32
    if (mInterruptPipe[0] == -1)
    {
        if (::pipe(mInterruptPipe) == 0)
        {
            const int flags = ::fcntl(mInterruptPipe[1], F_GETFL);
            if (flags >= 0)
            {
                ::fcntl(mInterruptPipe[1], F_SETFL, flags | O_NONBLOCK);
            }
        }
    }
#endif  // !_WIN32
    return mIsReady;
}

void Console::_os_release() noexcept
{
    if (mIsReady)
    {
        mIsReady = false;
        const int32_t finalRow = static_cast<int>(mMaxUsedRow + 2);
        printf("%s\x1B[%d;1H\n", CMD_CURSOR_SHOW.data(), finalRow);
        ::fflush(stdout);
#ifndef _WIN32
        if (mInterruptPipe[0] != -1) { ::close(mInterruptPipe[0]); mInterruptPipe[0] = -1; }
        if (mInterruptPipe[1] != -1) { ::close(mInterruptPipe[1]); mInterruptPipe[1] = -1; }
#endif  // !_WIN32
    }
}

void Console::_os_output_text(Console::Coord pos, const String& text) const noexcept
{
    Lock lock(mLock);
    const int32_t col     = (pos.posX     > 0) ? pos.posX     : 1;
    const int32_t row     = (pos.posY     > 0) ? pos.posY     : 1;
    const int32_t savecol = (mSavedPos.posX > 0) ? mSavedPos.posX : 1;
    const int32_t saverow = (mSavedPos.posY > 0) ? mSavedPos.posY : 1;
    if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
    {
        mMaxUsedRow = static_cast<int32_t>(pos.posY);
    }

    printf("\x1B[%d;%dH%s%s\x1B[%d;%dH", row, col, CMD_CLEAR_LINE.data(), text.as_string(), saverow, savecol);
    ::fflush(stdout);
}

void Console::_os_output_text(Console::Coord pos, std::string_view text) const noexcept
{
    Lock lock(mLock);
    // Move to target row/col, clear line, write text, return cursor to mSavedPos.
    const int32_t col     = (pos.posX     > 0) ? pos.posX     : 1;
    const int32_t row     = (pos.posY     > 0) ? pos.posY     : 1;
    const int32_t savecol = (mSavedPos.posX > 0) ? mSavedPos.posX : 1;
    const int32_t saverow = (mSavedPos.posY > 0) ? mSavedPos.posY : 1;
    if (static_cast<int32_t>(pos.posY) > mMaxUsedRow)
    {
        mMaxUsedRow = static_cast<int32_t>(pos.posY);
    }

    printf("\x1B[%d;%dH%s%s\x1B[%d;%dH", row, col, CMD_CLEAR_LINE.data(), text.data(), saverow, savecol);
    ::fflush(stdout);
}

void Console::_os_output_text(const String& text) const noexcept
{
    Lock lock(mLock);
    printf("%s", text.as_string());
    ::fflush(stdout);
}

void Console::_os_output_text(std::string_view text) const noexcept
{
    Lock lock(mLock);
    printf("%s", text.data());
    ::fflush(stdout);
}

Console::Coord Console::_os_get_cursor_position() const noexcept
{
    Lock lock(mLock);
    constexpr int32_t _EOY{ static_cast<int32_t>(';') };
    constexpr int32_t _EOX{ static_cast<int32_t>('R') };
    constexpr int32_t _ZERO{ static_cast<int32_t>('0') };

    Console::Coord result{ 0, 0 };
    printf("\x1B[6n");
    if ((getchar() == '\x1B') && (getchar() == '['))
    {
        int32_t ch;
        while ((ch = getchar()) != _EOY)
        {
            result.posY = result.posY * 10 + (ch - _ZERO);
        }

        while ((ch = getchar()) != _EOX)
        {
            result.posX = result.posX * 10 + (ch - _ZERO);
        }
    }

    return result;
}

void Console::_os_set_cursor_cur_position(Console::Coord pos) const noexcept
{
    const int32_t col = (pos.posX > 0) ? pos.posX : 1;
    const int32_t row = (pos.posY > 0) ? pos.posY : 1;
    Lock lock(mLock);
    // Track in software so _os_output_text can restore here without a DSR query.
    mSavedPos = { col, row };
    printf("\x1B[%d;%dH", row, col);
    ::fflush(stdout);
}

bool Console::_os_wait_input_string(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);

    Console& console = Console::instance();

    Console::Coord startPos;
    {
        Lock lock(console.mLock);
        startPos = console.mSavedPos;
        printf("\x1B[%d;%dH%s", startPos.posY, startPos.posX, CMD_CLEAR_LINE.data());
        ::fflush(stdout);
    }

    uint32_t len = 0;

#if defined(_WIN32)

    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD oldMode{ 0u };
    const bool rawMode{ GetConsoleMode(hIn, &oldMode) == TRUE };
    if (!rawMode)
    {
#if !defined(_MINGW)
        const bool ok{ ::gets_s(buffer, size) != nullptr };
#else
        const bool ok{ ::fgets(buffer, static_cast<int>(size), stdin) != nullptr };
#endif
        areg::trim_all<char>(buffer);
        return (ok && (areg::is_empty(buffer) == false));
    }

    SetConsoleMode(hIn, (oldMode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_PROCESSED_INPUT);

    while (len < (size - 1u))
    {
        char ch{ 0 };
        DWORD nRead{ 0u };
        if ((ReadConsoleA(hIn, &ch, 1u, &nRead, nullptr) == FALSE) || (nRead == 0u))
        {
            break;  // Console closed or error
        }

        if (ch == '\r')
        {
            // Enter key on Windows delivers \r, not \n.
            break;
        }
        else if (ch == '\b')
        {
            if (len > 0u)
            {
                Lock lock(console.mLock);
                --len;
                printf("\b \b");
                ::fflush(stdout);
                --console.mSavedPos.posX;
            }
        }
        else if ((static_cast<unsigned char>(ch) >= 32u) && (static_cast<unsigned char>(ch) < 127u))
        {
            // Printable ASCII: echo and advance the tracked cursor position.
            Lock lock(console.mLock);
            buffer[len++] = ch;
            printf("%c", ch);
            ::fflush(stdout);
            ++console.mSavedPos.posX;
        }
        // Ignore \n, non-printable control codes, and high bytes (function
        // keys are handled by the Windows console before reaching here).
    }

    if (rawMode)
    {
        SetConsoleMode(hIn, oldMode);
    }

#else   // POSIX

    struct termios old_tio{};
    const bool rawMode{ tcgetattr(STDIN_FILENO, &old_tio) == 0 };
    if (rawMode)
    {
        struct termios raw_tio{ old_tio };
        raw_tio.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
        raw_tio.c_cc[VMIN]  = 1;   // block until at least 1 byte available
        raw_tio.c_cc[VTIME] = 0;   // no read timeout
        tcsetattr(STDIN_FILENO, TCSANOW, &raw_tio);
    }

    if (!rawMode)
    {
        const bool ok{ ::fgets(buffer, static_cast<int>(size), stdin) != nullptr };
        areg::trim_all<char>(buffer);
        return (ok && (areg::is_empty(buffer) == false));
    }

    while (len < (size - 1u))
    {
        const int pipeReadFd = console.mInterruptPipe[0];
        if (pipeReadFd >= 0)
        {
            struct pollfd fds[2]{};
            fds[0].fd     = STDIN_FILENO;
            fds[0].events = POLLIN;
            fds[1].fd     = pipeReadFd;
            fds[1].events = POLLIN;

            const int r = ::poll(fds, 2, -1);
            if (r < 0)
            {
                if (errno == EINTR)
                    continue;

                break;
            }

            if ((fds[1].revents & POLLIN) != 0)
            {
                // Interrupt pipe triggered — drain it and abort the read.
                char dummy{ 0 };
                (void)::read(pipeReadFd, &dummy, 1);
                len = 0;
                break;
            }

            if ((fds[0].revents & POLLIN) == 0)
                continue;
        }

        char ch{ 0 };
        const ssize_t n = read(STDIN_FILENO, &ch, 1);
        if (n <= 0)
        {
            break;  // EOF or EINTR (e.g. Ctrl-D, SIGINT)
        }

        const auto uch = static_cast<unsigned char>(ch);

        if ((ch == '\n') || (ch == '\r'))
        {
            break;
        }
        else if ((ch == '\b') || (uch == 127u))
        {
            // Backspace (^H) or DEL
            if (len > 0u)
            {
                Lock lock(console.mLock);
                --len;
                printf("\b \b");
                ::fflush(stdout);
                --console.mSavedPos.posX;
            }
        }
        else if (ch == '\x1B')
        {
            char seq{ 0 };
            if ((read(STDIN_FILENO, &seq, 1) == 1) && (seq == '['))
            {
                char fin{ 0 };
                while (read(STDIN_FILENO, &fin, 1) == 1)
                {
                    if (((fin >= 'A') && (fin <= 'Z')) ||
                        ((fin >= 'a') && (fin <= 'z')) ||
                        (fin == '~'))
                    {
                        break;
                    }
                }
            }
        }
        else if ((ch == '\x03') || (ch == '\x04'))
        {
            // Ctrl-C (ETX) or Ctrl-D (EOT): abort cleanly
            break;
        }
        else if ((uch >= 32u) && (uch < 127u))
        {
            // Printable ASCII: echo and advance the tracked cursor position.
            Lock lock(console.mLock);
            buffer[len++] = ch;
            printf("%c", ch);
            ::fflush(stdout);
            ++console.mSavedPos.posX;
        }
        // Ignore other control characters and non-ASCII bytes (high UTF-8 bytes)
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

#endif  // defined(_WIN32)

    buffer[len] = '\0';

    {
        Lock lock(console.mLock);
        console.mSavedPos = startPos;
    }

    areg::trim_all<char>(buffer);
    return (areg::is_empty(buffer) == false);
}

void Console::_os_interrupt_input() noexcept
{
#ifdef _WIN32
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
#else
    if (mInterruptPipe[1] >= 0)
    {
        const char wake{ '\n' };
        (void)::write(mInterruptPipe[1], &wake, 1);
    }
#endif  // _WIN32
}

void Console::_os_refresh_screen() const noexcept
{
    Lock lock(mLock);
    ::fflush(stdout);
}

void Console::_os_clear_line() const noexcept
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_LINE.data());
    ::fflush(stdout);
}

void Console::_os_clear_line_at_position(Console::Coord pos) const noexcept
{
    // Clamp to 1-based (same rule as _os_set_cursor_cur_position).
    const int32_t col  = (pos.posX       > 0) ? pos.posX       : 1;
    const int32_t row  = (pos.posY       > 0) ? pos.posY       : 1;

    Lock lock(mLock);

    const int32_t save_col = (mSavedPos.posX > 0) ? mSavedPos.posX : 1;
    const int32_t save_row = (mSavedPos.posY > 0) ? mSavedPos.posY : 1;

    char buf[64];
    const int32_t len = snprintf(buf, sizeof(buf),
                             "\x1B[%d;%dH\x1B[K\x1B[%d;%dH",
                             row, col, save_row, save_col);
    if (len > 0)
    {
        ::fwrite(buf, 1, static_cast<size_t>(len), stdout);
        ::fflush(stdout);
    }
}

void Console::_os_clear_screen() const noexcept
{
    Lock lock(mLock);
    printf("%s", CMD_CLEAR_SCREEN.data());
    ::fflush(stdout);
}

bool Console::_os_read_input_list(const char* format, va_list varList) const
{
    return (vscanf(format, varList) > 0);
}

void Console::_os_save_cursor_position() const noexcept
{
    // No-op
}

void Console::_os_restore_cursor_position() const noexcept
{
    Lock lock(mLock);
    const int32_t col = (mSavedPos.posX > 0) ? mSavedPos.posX : 1;
    const int32_t row = (mSavedPos.posY > 0) ? mSavedPos.posY : 1;
    printf("\x1B[%d;%dH%s", row, col, CMD_CURSOR_SHOW.data());
    ::fflush(stdout);
}

void Console::_os_move_cursor_one_line_up() const noexcept
{
    Lock lock(mLock);
    printf("\x1B[1F");
    ::fflush(stdout);
}

void Console::_os_move_cursor_one_line_down() const noexcept
{
    Lock lock(mLock);
    printf("\x1B[1E");
    ::fflush(stdout);
}

} // namespace areg::ext

#endif  // !(AREG_EXTENDED)
