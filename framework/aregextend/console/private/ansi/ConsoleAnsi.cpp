/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/ansi/ConsoleAnsi.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, console implementation based on ANSI escape
 *              sequences. Used on every POSIX system, and on Windows when
 *              the extended Win32 console is not selected.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "aregextend/console/Console.hpp"

#if !(defined(_WIN32) && (AREG_EXTENDED))

#include "aregextend/console/private/ansi/AnsiWriter.hpp"

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

#else   // !_WIN32

    #include <errno.h>
    #include <fcntl.h>
    #include <poll.h>
    #include <signal.h>
    #include <stdlib.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>

#endif // _WIN32

#include <iterator>
#include <stdio.h>
#include <string.h>

namespace {

    //!< Clears the screen and moves the cursor to the top left corner.
    constexpr std::string_view  CMD_CLEAR_SCREEN    { "\x1B[2J\x1B[H" };

    //!< Milliseconds to wait for the rest of an escape sequence typed on the keyboard.
    constexpr int               ESCAPE_WAIT_MS      { 50 };

    //!< The longest escape sequence of a key that is kept.
    constexpr uint32_t          ESCAPE_MAX_PARAMS   { 8u };

    constexpr char  KEY_ESC         { '\x1B' };
    constexpr char  KEY_CTRL_A      { '\x01' };
    constexpr char  KEY_CTRL_E      { '\x05' };
    constexpr char  KEY_CTRL_U      { '\x15' };
    constexpr char  KEY_BACKSPACE   { '\b' };
    constexpr char  KEY_DELETE      { '\x7F' };

    //!< The result of reading one byte of input.
    enum class KeyRead
    {
          Byte          //!< A byte was read.
        , Timeout       //!< No byte arrived in time.
        , Closed        //!< The input reached its end or failed.
        , Interrupted   //!< The read was interrupted by enable_console_input(false).
    };

    void _write_stdout(const char* data, uint32_t size, void* /*context*/) noexcept
    {
        ::fwrite(data, 1u, static_cast<size_t>(size), stdout);
        ::fflush(stdout);
    }

    //!< The collected output. Used only with the console lock held.
    areg::ext::AnsiWriter   _writer(&_write_stdout, nullptr);

    //!< The cursor is hidden until the next flush.
    bool    _cursorHidden   { false };

    //!< The cursor left the input position since the last flush.
    bool    _cursorAway     { false };

    //!< The screen size was read since the last flush.
    bool    _screenRead     { false };

    //!< Printed text left the cursor away from the input position.
    bool    _cursorDetached { false };

    //!< Edits the input line in a caller buffer. The caret is an index in the text.
    class LineEditor
    {
    public:
        LineEditor(char* buffer, uint32_t size) noexcept
            : mBuffer   (buffer)
            , mCapacity (size > 0u ? size - 1u : 0u)
            , mLength   (0u)
            , mCaret    (0u)
        {
        }

        inline std::string_view text() const noexcept
        {
            return std::string_view(mBuffer, mLength);
        }

        inline uint32_t caret() const noexcept
        {
            return mCaret;
        }

        bool insert(char ch) noexcept
        {
            if (mLength >= mCapacity)
                return false;

            ::memmove(mBuffer + mCaret + 1u, mBuffer + mCaret, mLength - mCaret);
            mBuffer[mCaret++] = ch;
            ++mLength;
            return true;
        }

        bool erase_back() noexcept
        {
            if (mCaret == 0u)
                return false;

            --mCaret;
            return erase();
        }

        bool erase() noexcept
        {
            if (mCaret == mLength)
                return false;

            ::memmove(mBuffer + mCaret, mBuffer + mCaret + 1u, mLength - mCaret - 1u);
            --mLength;
            return true;
        }

        bool move_to(uint32_t caret) noexcept
        {
            caret = (caret > mLength) ? mLength : caret;
            const bool moved{ caret != mCaret };
            mCaret = caret;
            return moved;
        }

        bool left() noexcept
        {
            return (mCaret != 0u) && move_to(mCaret - 1u);
        }

        bool right() noexcept
        {
            return move_to(mCaret + 1u);
        }

        bool clear() noexcept
        {
            const bool changed{ mLength != 0u };
            mLength = 0u;
            mCaret  = 0u;
            return changed;
        }

        //!< Applies the key of an escape sequence: 'kind' is '[' or 'O', 'final' is the last byte.
        bool apply(char kind, std::string_view params, char final) noexcept
        {
            switch (final)
            {
            case 'D':
                return left();
            case 'C':
                return right();
            case 'H':
                return move_to(0u);
            case 'F':
                return move_to(mLength);
            case '~':
                if (kind != '[')
                    return false;
                else if ((params == "1") || (params == "7"))
                    return move_to(0u);
                else if ((params == "4") || (params == "8"))
                    return move_to(mLength);
                else if (params == "3")
                    return erase();
                return false;
            default:
                return false;
            }
        }

        void finish() noexcept
        {
            mBuffer[mLength] = '\0';
        }

    private:
        char*       mBuffer;
        uint32_t    mCapacity;
        uint32_t    mLength;
        uint32_t    mCaret;
    };

#ifdef _WIN32

    //!< The output mode of the console before the setup.
    DWORD       _outMode        { 0u };
    //!< True if the output mode was changed by the setup.
    bool        _outModeSet     { false };
    //!< True if the standard input is a console that can be read key by key.
    bool        _inConsole      { false };
    //!< The typed bytes read from the console and not handled yet.
    char        _keys[64]       { };
    uint32_t    _keysUsed       { 0u };
    uint32_t    _keysRead       { 0u };

    areg::Coord _read_screen_size() noexcept
    {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        if (::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &info) == FALSE)
        {
            return areg::Coord{ 0, 0 };
        }

        return areg::Coord{ info.srWindow.Right - info.srWindow.Left + 1, info.srWindow.Bottom - info.srWindow.Top + 1 };
    }

    bool _input_is_raw() noexcept
    {
        return _inConsole;
    }

    //!< Reads one typed byte. With a timeout only an already read byte is returned.
    KeyRead _read_byte(int /*pipeFd*/, int timeoutMs, char& ch) noexcept
    {
        if (_keysRead == _keysUsed)
        {
            if (timeoutMs >= 0)
            {
                return KeyRead::Timeout;
            }

            DWORD count{ 0u };
            if ((::ReadConsoleA(::GetStdHandle(STD_INPUT_HANDLE), _keys, static_cast<DWORD>(std::size(_keys)), &count, nullptr) == FALSE) || (count == 0u))
            {
                return KeyRead::Closed;
            }

            _keysUsed = static_cast<uint32_t>(count);
            _keysRead = 0u;
        }

        ch = _keys[_keysRead++];
        return KeyRead::Byte;
    }

#else   // !_WIN32

    //!< The terminal settings before the setup, and the settings while the console reads keys.
    struct termios          _ttyOriginal    { };
    struct termios          _ttyRaw         { };
    //!< Not 0 while the terminal is in the key by key mode.
    volatile sig_atomic_t   _ttyRawActive   { 0 };

    //!< The signals that restore the terminal settings.
    constexpr int           _SIGNALS[]      { SIGINT, SIGTERM, SIGQUIT, SIGABRT, SIGCONT };
    struct sigaction        _oldActions[std::size(_SIGNALS)] { };
    bool                    _installed[std::size(_SIGNALS)]  { };

    //!< Restores the terminal and raises the signal again, or re-enters key by key mode on SIGCONT.
    void _on_signal(int sig)
    {
        const int error{ errno };
        if (sig == SIGCONT)
        {
            if (_ttyRawActive != 0)
            {
                ::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyRaw);
            }
        }
        else
        {
            if (_ttyRawActive != 0)
            {
                ::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyOriginal);
            }

            constexpr char restore[]{ "\x1B[?25h\r\n" };
            [[maybe_unused]] const ssize_t ignored{ ::write(STDOUT_FILENO, restore, sizeof(restore) - 1u) };
            ::raise(sig);
        }

        errno = error;
    }

    //!< Installs the handler for every signal whose action is the default one.
    void _install_signals() noexcept
    {
        for (size_t i = 0u; i < std::size(_SIGNALS); ++i)
        {
            struct sigaction current{};
            if ((::sigaction(_SIGNALS[i], nullptr, &current) != 0) || ((current.sa_flags & SA_SIGINFO) != 0) || (current.sa_handler != SIG_DFL))
            {
                continue;
            }

            struct sigaction action{};
            action.sa_handler = &_on_signal;
            ::sigemptyset(&action.sa_mask);
            action.sa_flags = (_SIGNALS[i] == SIGCONT) ? SA_RESTART : (SA_RESTART | SA_RESETHAND);
            _installed[i] = (::sigaction(_SIGNALS[i], &action, &_oldActions[i]) == 0);
        }
    }

    //!< Restores the actions replaced by _install_signals(), unless the application changed them since.
    void _remove_signals() noexcept
    {
        for (size_t i = 0u; i < std::size(_SIGNALS); ++i)
        {
            struct sigaction current{};
            if (_installed[i] && (::sigaction(_SIGNALS[i], nullptr, &current) == 0) && ((current.sa_flags & SA_SIGINFO) == 0) && (current.sa_handler == &_on_signal))
            {
                ::sigaction(_SIGNALS[i], &_oldActions[i], nullptr);
            }

            _installed[i] = false;
        }
    }

    //!< Switches a terminal input of the foreground process to key by key mode without echo.
    void _enter_raw_mode() noexcept
    {
        if ((::isatty(STDIN_FILENO) != 1) || (::tcgetpgrp(STDIN_FILENO) != ::getpgrp()) || (::tcgetattr(STDIN_FILENO, &_ttyOriginal) != 0))
        {
            return;
        }

        _ttyRaw = _ttyOriginal;
        _ttyRaw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
        _ttyRaw.c_cc[VMIN]  = 1;
        _ttyRaw.c_cc[VTIME] = 0;
        _install_signals();
        if (::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyRaw) == 0)
        {
            _ttyRawActive = 1;
        }
        else
        {
            _remove_signals();
        }
    }

    void _leave_raw_mode() noexcept
    {
        if (_ttyRawActive != 0)
        {
            _ttyRawActive = 0;
            ::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyOriginal);
            _remove_signals();
        }
    }

    areg::Coord _read_screen_size() noexcept
    {
        struct winsize size{};
        if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) != 0)
        {
            return areg::Coord{ 0, 0 };
        }

        return areg::Coord{ static_cast<int32_t>(size.ws_col), static_cast<int32_t>(size.ws_row) };
    }

    bool _input_is_raw() noexcept
    {
        return (_ttyRawActive != 0);
    }

    //!< Reads one byte of the standard input, watching the interrupt pipe. A negative timeout waits for ever.
    KeyRead _read_byte(int pipeFd, int timeoutMs, char& ch) noexcept
    {
        while (true)
        {
            struct pollfd fds[2]{};
            fds[0].fd       = STDIN_FILENO;
            fds[0].events   = POLLIN;
            fds[1].fd       = pipeFd;
            fds[1].events   = POLLIN;
            const nfds_t count{ static_cast<nfds_t>(pipeFd >= 0 ? 2 : 1) };

            const int ready{ ::poll(fds, count, timeoutMs) };
            if (ready < 0)
            {
                if (errno == EINTR)
                    continue;

                return KeyRead::Closed;
            }
            else if (ready == 0)
            {
                return KeyRead::Timeout;
            }
            else if ((count == 2) && ((fds[1].revents & POLLIN) != 0))
            {
                char dummy{ 0 };
                [[maybe_unused]] const ssize_t ignored{ ::read(pipeFd, &dummy, 1u) };
                return KeyRead::Interrupted;
            }
            else if (fds[0].revents != 0)
            {
                const ssize_t bytes{ ::read(STDIN_FILENO, &ch, 1u) };
                if (bytes == 1)
                    return KeyRead::Byte;
                else if ((bytes < 0) && ((errno == EINTR) || (errno == EAGAIN)))
                    continue;

                return KeyRead::Closed;
            }
        }
    }

#endif  // _WIN32

    //!< Reads the size once per flush, hides the cursor and marks that it must return.
    void _begin_drawing() noexcept
    {
        if (_screenRead == false)
        {
            const areg::Coord size{ _read_screen_size() };
            _writer.set_screen(size.posX, size.posY);
            _screenRead = true;
        }

        if (_cursorHidden == false)
        {
            _writer.hide_cursor();
            _cursorHidden = true;
        }

        _cursorAway = true;
    }

    //!< Reads the rest of a typed escape sequence and applies it. A bare Esc and Alt with a key are ignored.
    //!< Returns true if the line changed.
    bool _read_escape(LineEditor& line, int pipeFd, KeyRead& status) noexcept
    {
        char kind{ 0 };
        const KeyRead first{ _read_byte(pipeFd, ESCAPE_WAIT_MS, kind) };
        if (first != KeyRead::Byte)
        {
            status = (first == KeyRead::Timeout) ? KeyRead::Byte : first;
            return false;
        }
        else if ((kind != '[') && (kind != 'O'))
        {
            return false;
        }

        char params[ESCAPE_MAX_PARAMS]{ };
        uint32_t count{ 0u };
        while (true)
        {
            char ch{ 0 };
            const KeyRead next{ _read_byte(pipeFd, ESCAPE_WAIT_MS, ch) };
            if (next != KeyRead::Byte)
            {
                status = (next == KeyRead::Timeout) ? KeyRead::Byte : next;
                return false;
            }
            else if ((ch >= 0x40) && (ch <= 0x7E))
            {
                return line.apply(kind, std::string_view(params, count), ch);
            }
            else if (count < ESCAPE_MAX_PARAMS)
            {
                params[count++] = ch;
            }
        }
    }

} // namespace

namespace areg::ext {

//////////////////////////////////////////////////////////////////////////
// Console::ScopedLock class implementation
//////////////////////////////////////////////////////////////////////////

class Console::ScopedLock
{
public:
    explicit ScopedLock(const Console& console) noexcept
        : mConsole(console)
    {
        mConsole._lock();
    }

    ~ScopedLock() noexcept
    {
        mConsole._unlock();
    }

    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator = (const ScopedLock&) = delete;

private:
    const Console&  mConsole;   //!< The locked console.
};

//////////////////////////////////////////////////////////////////////////
// Console ANSI implementation
//////////////////////////////////////////////////////////////////////////

bool Console::_os_setup() noexcept
{
    if (mIsReady)
    {
        return true;
    }

#ifdef _WIN32

    HANDLE hOut{ ::GetStdHandle(STD_OUTPUT_HANDLE) };
    DWORD mode{ 0u };
    if ((hOut != nullptr) && (hOut != INVALID_HANDLE_VALUE) && (::GetConsoleMode(hOut, &mode) == TRUE))
    {
        _outMode    = mode;
        _outModeSet = (::SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING) == TRUE);
        mIsTerminal = _outModeSet;
    }

    HANDLE hIn{ ::GetStdHandle(STD_INPUT_HANDLE) };
    DWORD inMode{ 0u };
    _inConsole = mIsTerminal && (hIn != nullptr) && (hIn != INVALID_HANDLE_VALUE) && (::GetConsoleMode(hIn, &inMode) == TRUE);

#else   // !_WIN32

    const char* term{ ::getenv("TERM") };
    mIsTerminal = (::isatty(STDOUT_FILENO) == 1) && ((term == nullptr) || (::strcmp(term, "dumb") != 0));
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

    if (mIsTerminal)
    {
        _enter_raw_mode();
    }

#endif  // _WIN32

    mSavedPos   = Coord{ 0, 0 };
    mMaxUsedRow = 0;
    mInputClosed.store(false, std::memory_order_relaxed);
    if (mIsTerminal)
    {
        _writer.raw(CMD_CLEAR_SCREEN);
        _writer.flush();
    }

    mIsReady = true;
    return mIsReady;
}

void Console::_os_release() noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }

    mIsReady = false;
    if (mIsTerminal)
    {
        const Coord size{ _read_screen_size() };
        int32_t row{ mMaxUsedRow + 1 };
        if ((size.posY > 0) && (row >= size.posY))
        {
            row = size.posY - 1;
        }

        _writer.move_to(Coord{ 0, row });
        _writer.show_cursor();
        _writer.raw("\n");
        _cursorAway     = false;
        _cursorHidden   = false;
    }

    _writer.flush();

#ifdef _WIN32

    if (_outModeSet)
    {
        ::SetConsoleMode(::GetStdHandle(STD_OUTPUT_HANDLE), _outMode);
        _outModeSet = false;
    }

    _inConsole = false;

#else   // !_WIN32

    _leave_raw_mode();
    if (mInterruptPipe[0] != -1) { ::close(mInterruptPipe[0]); mInterruptPipe[0] = -1; }
    if (mInterruptPipe[1] != -1) { ::close(mInterruptPipe[1]); mInterruptPipe[1] = -1; }

#endif  // _WIN32
}

void Console::_os_flush() const noexcept
{
    if (mIsTerminal && mIsReady)
    {
        if (_cursorAway)
        {
            _writer.move_to(mSavedPos);
            _cursorDetached = false;
        }

        if (_cursorHidden)
        {
            _writer.show_cursor();
        }
    }

    _cursorAway     = false;
    _cursorHidden   = false;
    _screenRead     = false;
    _writer.flush();
}

Console::Coord Console::_os_screen_size() const noexcept
{
    ScopedLock lock(*this);
    return (mIsTerminal ? _read_screen_size() : Coord{ 0, 0 });
}

void Console::_os_output_text(Console::Coord pos, const String& text) const noexcept
{
    _os_output_text(pos, std::string_view(text.as_string(), static_cast<size_t>(text.length())));
}

void Console::_os_output_text(Console::Coord pos, std::string_view text) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }
    else if (mIsTerminal)
    {
        _begin_drawing();
        const int32_t last{ _writer.text_at(pos, text) };
        mMaxUsedRow = (last > mMaxUsedRow) ? last : mMaxUsedRow;
    }
    else
    {
        _writer.plain_line(text);
    }
}

void Console::_os_output_text(const String& text) const noexcept
{
    _os_output_text(std::string_view(text.as_string(), static_cast<size_t>(text.length())));
}

void Console::_os_output_text(std::string_view text) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady == false)
    {
        return;
    }
    else if (mIsTerminal)
    {
        if (_cursorAway)
        {
            _writer.move_to(mSavedPos);
            _cursorAway = false;
        }

        _writer.raw(text);
        _cursorDetached = true;
    }
    else
    {
        _writer.plain(text);
    }
}

Console::Coord Console::_os_get_cursor_position() const noexcept
{
    ScopedLock lock(*this);
    return mSavedPos;
}

void Console::_os_set_cursor_cur_position(Console::Coord pos) const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ (pos.posX > 0) ? pos.posX : 0, (pos.posY > 0) ? pos.posY : 0 };
    _cursorAway = mIsTerminal;
}

bool Console::_os_wait_input_string(char* buffer, uint32_t size)
{
    ASSERT(buffer != nullptr);
    if (size == 0u)
    {
        return false;
    }

    buffer[0] = '\0';
    Console& console = Console::instance();
    LineEditor line(buffer, size);
    KeyRead status{ KeyRead::Byte };

#ifdef _WIN32

    HANDLE hIn{ ::GetStdHandle(STD_INPUT_HANDLE) };
    DWORD oldMode{ 0u };
    bool raw{ _input_is_raw() && (::GetConsoleMode(hIn, &oldMode) == TRUE) };
    if (raw)
    {
        const DWORD rawMode{ (oldMode & ~static_cast<DWORD>(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT)) | ENABLE_PROCESSED_INPUT };
        raw = (::SetConsoleMode(hIn, rawMode | ENABLE_VIRTUAL_TERMINAL_INPUT) == TRUE) || (::SetConsoleMode(hIn, rawMode) == TRUE);
    }

    if (raw == false)
    {
        if (::fgets(buffer, static_cast<int>(size), stdin) == nullptr)
        {
            if (::feof(stdin) != 0)
            {
                console.mInputClosed.store(true, std::memory_order_relaxed);
            }

            buffer[0] = '\0';
            return false;
        }

        size_t length{ ::strlen(buffer) };
        if ((length != 0u) && (buffer[length - 1u] == '\n'))
        {
            buffer[--length] = '\0';
        }
        else
        {
            // The rest of a line that does not fit is dropped.
            int ch{ 0 };
            while (((ch = ::fgetc(stdin)) != EOF) && (ch != '\n'))
                ;
        }

        areg::trim_all<char>(buffer);
        return (areg::is_empty(buffer) == false);
    }

    const int pipeFd{ -1 };

#else   // !_WIN32

    const bool raw{ _input_is_raw() };
    const int pipeFd{ console.mInterruptPipe[0] };

#endif  // _WIN32

    if (raw == false)
    {
        char ch{ 0 };
        while ((status = _read_byte(pipeFd, -1, ch)) == KeyRead::Byte)
        {
            if (ch == '\n')
                break;
            else if (ch != '\r')
                line.insert(ch);
        }
    }
    else
    {
        const Coord start{ console._os_get_cursor_position() };
        auto redraw = [&console, &line, start]()
        {
            ScopedLock lock(console);
            if (console.mIsReady && console.mIsTerminal)
            {
                _begin_drawing();
                console.mSavedPos = _writer.field(start, line.text(), line.caret());
            }
        };

        redraw();
        bool done{ false };
        while ((done == false) && (status == KeyRead::Byte))
        {
            char ch{ 0 };
            if ((status = _read_byte(pipeFd, -1, ch)) != KeyRead::Byte)
            {
                break;
            }

            bool changed{ false };
            switch (ch)
            {
            case '\r':
            case '\n':
                done = true;
                break;
            case KEY_BACKSPACE:
            case KEY_DELETE:
                changed = line.erase_back();
                break;
            case KEY_CTRL_A:
                changed = line.move_to(0u);
                break;
            case KEY_CTRL_E:
                changed = line.move_to(0xFFFFFFFFu);
                break;
            case KEY_CTRL_U:
                changed = line.clear();
                break;
            case KEY_ESC:
                changed = _read_escape(line, pipeFd, status);
                break;
            default:
                if ((static_cast<unsigned char>(ch) >= 0x20u) && (static_cast<unsigned char>(ch) < 0x7Fu))
                {
                    changed = line.insert(ch);
                }
                break;
            }

            if (changed)
            {
                redraw();
            }
        }

        ScopedLock lock(console);
        console.mSavedPos = start;
        _cursorAway = console.mIsTerminal;
    }

#ifdef _WIN32
    ::SetConsoleMode(hIn, oldMode);
#endif  // _WIN32

    if ((status == KeyRead::Interrupted) || console.mInterrupted.load(std::memory_order_relaxed))
    {
        line.clear();
    }
    else if (status == KeyRead::Closed)
    {
        console.mInputClosed.store(true, std::memory_order_relaxed);
    }

    line.finish();
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
        [[maybe_unused]] ssize_t ignored = ::write(mInterruptPipe[1], &wake, 1);
    }
#endif  // _WIN32
}

void Console::_os_refresh_screen() const noexcept
{
    ScopedLock lock(*this);
}

void Console::_os_clear_line() const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady && mIsTerminal)
    {
        _begin_drawing();
        _writer.clear_at(mSavedPos);
    }
}

void Console::_os_clear_line_at_position(Console::Coord pos) const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady && mIsTerminal)
    {
        _begin_drawing();
        _writer.clear_at(pos);
    }
}

void Console::_os_clear_screen() const noexcept
{
    ScopedLock lock(*this);
    if (mIsReady && mIsTerminal)
    {
        _writer.raw(CMD_CLEAR_SCREEN);
        mSavedPos   = Coord{ 0, 0 };
        mMaxUsedRow = 0;
        _cursorAway = false;
        _cursorDetached = false;
    }
}

bool Console::_os_read_input_list(const char* format, va_list varList) const
{
    {
        ScopedLock lock(*this);
    }

#ifndef _WIN32
    const bool raw{ _ttyRawActive != 0 };
    if (raw)
    {
        _ttyRawActive = 0;
        ::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyOriginal);
    }
#endif  // !_WIN32

    const bool result{ vscanf(format, varList) > 0 };

#ifndef _WIN32
    if (raw)
    {
        ::tcsetattr(STDIN_FILENO, TCSANOW, &_ttyRaw);
        _ttyRawActive = 1;
    }
#endif  // !_WIN32

    return result;
}

void Console::_os_save_cursor_position() const noexcept
{
}

void Console::_os_restore_cursor_position() const noexcept
{
    ScopedLock lock(*this);
    _cursorAway = _cursorAway || (mIsTerminal && _cursorDetached);
}

void Console::_os_move_cursor_one_line_up() const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ 0, (mSavedPos.posY > 0) ? mSavedPos.posY - 1 : 0 };
    _cursorAway = mIsTerminal;
}

void Console::_os_move_cursor_one_line_down() const noexcept
{
    ScopedLock lock(*this);
    mSavedPos = Coord{ 0, mSavedPos.posY + 1 };
    _cursorAway = mIsTerminal;
}

} // namespace areg::ext

#endif  // !(defined(_WIN32) && (AREG_EXTENDED))
