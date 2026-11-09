/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/win32/ConsoleTerminalWin32.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the Windows part of the console: virtual terminal modes,
 *              the terminal size and the reading of keys.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/console/private/ConsoleTerminal.hpp"

#ifdef _WIN32

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
    #define NOMINMAX
#endif // !NOMINMAX
#include <Windows.h>


#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#endif  // ENABLE_VIRTUAL_TERMINAL_PROCESSING
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
    #define ENABLE_VIRTUAL_TERMINAL_INPUT       0x0200
#endif  // ENABLE_VIRTUAL_TERMINAL_INPUT

namespace {

    //!< The output mode of the console before the setup.
    DWORD       _outMode        { 0u };
    //!< True if the output mode was changed by the setup.
    bool        _outModeSet     { false };
    //!< The input mode of the console before the setup.
    DWORD       _inMode         { 0u };
    //!< True if the input is read key by key, without echo.
    bool        _keysMode       { false };
    //!< True if the control handler is installed.
    bool        _handlerSet     { false };
    //!< The auto reset event that wakes the thread waiting for a key.
    HANDLE      _wakeEvent      { nullptr };
    //!< The number of input records read from the console at once.
    constexpr DWORD MAX_RECORDS { 16u };
    //!< The input records read from the console and not handled yet.
    INPUT_RECORD _records[MAX_RECORDS] { };
    uint32_t    _recordsUsed    { 0u };
    uint32_t    _recordsRead    { 0u };
    //!< The bytes of one key press not handled yet.
    char        _keys[4]        { };
    uint32_t    _keysUsed       { 0u };
    uint32_t    _keysRead       { 0u };

    inline bool _is_valid(HANDLE handle) noexcept
    {
        return (handle != nullptr) && (handle != INVALID_HANDLE_VALUE);
    }

    //!< Restores the console modes and shows the cursor.
    void _restore_modes() noexcept
    {
        if (_keysMode)
        {
            ::SetConsoleMode(::GetStdHandle(STD_INPUT_HANDLE), _inMode);
            _keysMode = false;
        }

        if (_outModeSet)
        {
            HANDLE hOut{ ::GetStdHandle(STD_OUTPUT_HANDLE) };
            constexpr char show[]{ "\x1B[?25h" };
            DWORD written{ 0u };
            ::WriteConsoleA(hOut, show, static_cast<DWORD>(sizeof(show) - 1u), &written, nullptr);
            ::SetConsoleMode(hOut, _outMode);
            _outModeSet = false;
        }
    }

    //!< Sets the bytes of one press of a key. Arrows, Home, End and Delete become escape sequences.
    void _set_key(const KEY_EVENT_RECORD& key) noexcept
    {
        const WCHAR wch{ key.uChar.UnicodeChar };
        if (wch != 0)
        {
            if (wch < 0x80)
                _keys[_keysUsed++] = static_cast<char>(wch);

            return;
        }

        char final{ 0 };
        switch (key.wVirtualKeyCode)
        {
        case VK_UP:     final = 'A'; break;
        case VK_DOWN:   final = 'B'; break;
        case VK_RIGHT:  final = 'C'; break;
        case VK_LEFT:   final = 'D'; break;
        case VK_HOME:   final = 'H'; break;
        case VK_END:    final = 'F'; break;
        case VK_DELETE: final = '~'; break;
        default:        return;
        }

        _keys[_keysUsed++] = '\x1B';
        _keys[_keysUsed++] = '[';
        if (final == '~')
            _keys[_keysUsed++] = '3';
        _keys[_keysUsed++] = final;
    }

    //!< Takes one press of the next read key. A key repeated several times stays for the next press.
    void _take_key() noexcept
    {
        _keysUsed = 0u;
        _keysRead = 0u;
        INPUT_RECORD& record{ _records[_recordsRead] };
        if ((record.EventType == KEY_EVENT) && (record.Event.KeyEvent.bKeyDown != FALSE))
        {
            _set_key(record.Event.KeyEvent);
            if (record.Event.KeyEvent.wRepeatCount > 1u)
            {
                --record.Event.KeyEvent.wRepeatCount;
                return;
            }
        }

        ++_recordsRead;
    }

    //!< Restores the console before Ctrl+C, Ctrl+Break or closing ends the process.
    BOOL WINAPI _on_control(DWORD /*type*/)
    {
        _restore_modes();
        return FALSE;
    }

} // namespace

namespace areg::ext::terminal {

bool open(int (&pipe)[2]) noexcept
{
    pipe[0] = -1;
    pipe[1] = -1;

    HANDLE hOut{ ::GetStdHandle(STD_OUTPUT_HANDLE) };
    DWORD mode{ 0u };
    if (_is_valid(hOut) && (::GetConsoleMode(hOut, &mode) != FALSE))
    {
        _outMode    = mode;
        _outModeSet = (::SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != FALSE);
    }

    HANDLE hIn{ ::GetStdHandle(STD_INPUT_HANDLE) };
    if (_outModeSet && _is_valid(hIn) && (::GetConsoleMode(hIn, &_inMode) != FALSE))
    {
        const DWORD keysMode{ (_inMode & ~static_cast<DWORD>(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_PROCESSED_INPUT };
        _keysMode = (::SetConsoleMode(hIn, keysMode | ENABLE_VIRTUAL_TERMINAL_INPUT) != FALSE) || (::SetConsoleMode(hIn, keysMode) != FALSE);
    }

    if (_outModeSet)
    {
        _handlerSet = (::SetConsoleCtrlHandler(&_on_control, TRUE) != FALSE);
    }

    if (_keysMode)
    {
        _wakeEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
    }

    _recordsUsed = 0u;
    _recordsRead = 0u;
    _keysUsed = 0u;
    _keysRead = 0u;
    return _outModeSet;
}

void close(int (&pipe)[2]) noexcept
{
    if (_handlerSet)
    {
        ::SetConsoleCtrlHandler(&_on_control, FALSE);
        _handlerSet = false;
    }

    _restore_modes();
    if (_wakeEvent != nullptr)
    {
        ::CloseHandle(_wakeEvent);
        _wakeEvent = nullptr;
    }

    _recordsUsed = 0u;
    _recordsRead = 0u;
    _keysUsed = 0u;
    _keysRead = 0u;
    pipe[0] = -1;
    pipe[1] = -1;
}

areg::Coord screen_size() noexcept
{
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &info) == FALSE)
    {
        return areg::Coord{ 0, 0 };
    }

    return areg::Coord{ info.srWindow.Right - info.srWindow.Left + 1, info.srWindow.Bottom - info.srWindow.Top + 1 };
}

bool is_key_mode() noexcept
{
    return _keysMode;
}

KeyRead read_byte(int /*pipeFd*/, int timeoutMs, char& ch) noexcept
{
    HANDLE hIn{ ::GetStdHandle(STD_INPUT_HANDLE) };
    if (_keysMode == false)
    {
        DWORD count{ 0u };
        if (timeoutMs >= 0)
            return KeyRead::Timeout;
        else if ((::ReadFile(hIn, &ch, 1u, &count, nullptr) == FALSE) || (count != 1u))
            return KeyRead::Closed;

        return KeyRead::Byte;
    }

    while (_keysRead == _keysUsed)
    {
        if (_recordsRead != _recordsUsed)
        {
            _take_key();
            continue;
        }

        HANDLE handles[2]{ _wakeEvent, hIn };
        const DWORD count{ _wakeEvent != nullptr ? 2u : 1u };
        const DWORD waited{ ::WaitForMultipleObjects(count, handles + (2u - count), FALSE, timeoutMs >= 0 ? static_cast<DWORD>(timeoutMs) : INFINITE) };
        if (waited == WAIT_TIMEOUT)
        {
            return KeyRead::Timeout;
        }
        else if ((count == 2u) && (waited == WAIT_OBJECT_0))
        {
            return KeyRead::Interrupted;
        }
        else if (waited != (WAIT_OBJECT_0 + count - 1u))
        {
            return KeyRead::Closed;
        }

        DWORD read{ 0u };
        if ((::ReadConsoleInputW(hIn, _records, MAX_RECORDS, &read) == FALSE) || (read == 0u))
        {
            return KeyRead::Closed;
        }

        _recordsUsed = static_cast<uint32_t>(read);
        _recordsRead = 0u;
    }

    ch = _keys[_keysRead++];
    return KeyRead::Byte;
}

void interrupt(const int (& /*pipe*/)[2]) noexcept
{
    if (_wakeEvent != nullptr)
    {
        ::SetEvent(_wakeEvent);
    }
}

} // namespace areg::ext::terminal

#endif  // _WIN32
