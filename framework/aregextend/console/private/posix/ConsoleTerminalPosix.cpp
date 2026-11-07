/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/console/private/posix/ConsoleTerminalPosix.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, the POSIX part of the console: termios key by key mode,
 *              the signals that restore it, the terminal size and the reading of keys.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/console/private/ConsoleTerminal.hpp"

#if defined(_POSIX) || defined(POSIX)

#include <errno.h>
#include <fcntl.h>
#include <iterator>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace {

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
            ::sigaction(_SIGNALS[i], nullptr, &current);
            if (((current.sa_flags & SA_SIGINFO) != 0) || (current.sa_handler != SIG_DFL))
            {
                continue;
            }

            struct sigaction action{};
            action.sa_handler = &_on_signal;
            sigemptyset(&action.sa_mask);
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
            ::sigaction(_SIGNALS[i], nullptr, &current);
            if (_installed[i] && ((current.sa_flags & SA_SIGINFO) == 0) && (current.sa_handler == &_on_signal))
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

    void _close_fd(int& fd) noexcept
    {
        if (fd != -1)
        {
            ::close(fd);
            fd = -1;
        }
    }

} // namespace

namespace areg::ext::terminal {

bool open(int (&pipe)[2]) noexcept
{
    const char* term{ ::getenv("TERM") };
    const bool isTerminal{ (::isatty(STDOUT_FILENO) == 1) && ((term == nullptr) || (::strcmp(term, "dumb") != 0)) };
    if (::pipe(pipe) == 0)
    {
        ::fcntl(pipe[1], F_SETFL, O_NONBLOCK);
    }

    if (isTerminal)
    {
        _enter_raw_mode();
    }

    return isTerminal;
}

void close(int (&pipe)[2]) noexcept
{
    _leave_raw_mode();
    _close_fd(pipe[0]);
    _close_fd(pipe[1]);
}

areg::Coord screen_size() noexcept
{
    struct winsize size{};
    if (::ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) != 0)
    {
        return areg::Coord{ 0, 0 };
    }

    return areg::Coord{ static_cast<int32_t>(size.ws_col), static_cast<int32_t>(size.ws_row) };
}

bool is_key_mode() noexcept
{
    return (_ttyRawActive != 0);
}

KeyRead read_byte(int pipeFd, int timeoutMs, char& ch) noexcept
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
        else if ((count == 2) && (fds[1].revents != 0))
        {
            char dummy{ 0 };
            [[maybe_unused]] const ssize_t ignored{ ::read(pipeFd, &dummy, 1u) };
            return KeyRead::Interrupted;
        }

        const ssize_t bytes{ ::read(STDIN_FILENO, &ch, 1u) };
        if (bytes == 1)
            return KeyRead::Byte;
        else if ((bytes < 0) && ((errno == EINTR) || (errno == EAGAIN)))
            continue;

        return KeyRead::Closed;
    }
}

void interrupt(const int (&pipe)[2]) noexcept
{
    if (pipe[1] >= 0)
    {
        const char wake{ '\n' };
        [[maybe_unused]] const ssize_t ignored{ ::write(pipe[1], &wake, 1u) };
    }
}

} // namespace areg::ext::terminal

#endif  // defined(_POSIX) || defined(POSIX)
