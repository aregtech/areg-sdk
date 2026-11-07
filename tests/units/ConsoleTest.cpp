/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ConsoleTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The console on a terminal of a given size and on redirected streams.
 *              Every test runs in a child process: the console is a singleton that
 *              takes the standard streams when it is first used.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "units/VtScreen.hpp"
#include "aregextend/console/Console.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>

#if defined(_WIN32)
    #include <io.h>
    #include <fcntl.h>
    #include <Windows.h>
#else
    #include <fcntl.h>
    #include <poll.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <unistd.h>
#endif

using areg::ext::Console;

namespace
{
    //!< Redirects a standard stream to a pipe; the other end stays with the test.
    class RedirectedStream
    {
    public:
        //!< Takes stdout if 'output' is true, else stdin.
        explicit RedirectedStream(bool output)
            : mTarget(output ? 1 : 0)
        {
            int fds[2]{ -1, -1 };
#if defined(_WIN32)
            AREG_CHILD_CHECK(::_pipe(fds, 65536, _O_BINARY) == 0);
#else
            AREG_CHILD_CHECK(::pipe(fds) == 0);
#endif
            mOwn = output ? fds[0] : fds[1];
            const int theirs{ output ? fds[1] : fds[0] };
            std::fflush(stdout);
#if defined(_WIN32)
            AREG_CHILD_CHECK(::_dup2(theirs, mTarget) == 0);
            AREG_CHILD_CHECK(::SetStdHandle(output ? STD_OUTPUT_HANDLE : STD_INPUT_HANDLE,
                reinterpret_cast<HANDLE>(::_get_osfhandle(mTarget))) != FALSE);
            ::_close(theirs);
#else
            AREG_CHILD_CHECK(::dup2(theirs, mTarget) == mTarget);
            ::close(theirs);
#endif
        }

        //!< Writes the text into the redirected input.
        void write(const std::string& text)
        {
#if defined(_WIN32)
            AREG_CHILD_CHECK(::_write(mOwn, text.data(), static_cast<unsigned int>(text.size())) == static_cast<int>(text.size()));
#else
            AREG_CHILD_CHECK(::write(mOwn, text.data(), text.size()) == static_cast<ssize_t>(text.size()));
#endif
        }

        //!< Closes the end of the test: the input reaches its end.
        void close()
        {
#if defined(_WIN32)
            ::_close(mOwn);
#else
            ::close(mOwn);
#endif
            mOwn = -1;
        }

        //!< Closes the redirected output and returns everything written to it.
        std::string read_all()
        {
            std::fflush(stdout);
#if defined(_WIN32)
            const int null{ ::_open("NUL", _O_WRONLY) };
            AREG_CHILD_CHECK(::_dup2(null, mTarget) == 0);
            ::_close(null);
#else
            const int null{ ::open("/dev/null", O_WRONLY) };
            AREG_CHILD_CHECK(::dup2(null, mTarget) == mTarget);
            ::close(null);
#endif
            std::string text;
            char buffer[4096];
            int count{ 0 };
#if defined(_WIN32)
            while ((count = ::_read(mOwn, buffer, sizeof(buffer))) > 0)
#else
            while ((count = static_cast<int>(::read(mOwn, buffer, sizeof(buffer)))) > 0)
#endif
                text.append(buffer, static_cast<size_t>(count));
            return text;
        }

    private:
        int     mTarget;
        int     mOwn{ -1 };
    };

#if !defined(_WIN32)

    //!< A pseudo terminal of a given size that takes stdout, and stdin if it is the controlling terminal.
    //!< A thread reads everything the console draws, so a writer never blocks on a full terminal.
    class Terminal
    {
    public:
        Terminal(int columns, int rows, bool controlling = false)
            : screen(columns, rows)
        {
            if (controlling)
            {
                AREG_CHILD_CHECK((::getsid(0) == ::getpid()) || (::setsid() != -1));
            }

            mMaster = ::posix_openpt(O_RDWR | O_NOCTTY);
            AREG_CHILD_CHECK(mMaster >= 0);
            AREG_CHILD_CHECK((::grantpt(mMaster) == 0) && (::unlockpt(mMaster) == 0));
            const char* name{ ::ptsname(mMaster) };
            AREG_CHILD_CHECK(name != nullptr);
            mSlave = ::open(name, O_RDWR | (controlling ? 0 : O_NOCTTY));
            AREG_CHILD_CHECK(mSlave >= 0);
            if (controlling)
            {
                ::ioctl(mSlave, TIOCSCTTY, 0);
                AREG_CHILD_CHECK(::tcgetpgrp(mSlave) == ::getpgrp());
                AREG_CHILD_CHECK(::dup2(mSlave, 0) == 0);
            }

            winsize size{};
            size.ws_col = static_cast<unsigned short>(columns);
            size.ws_row = static_cast<unsigned short>(rows);
            AREG_CHILD_CHECK(::ioctl(mSlave, TIOCSWINSZ, &size) == 0);
            std::fflush(stdout);
            AREG_CHILD_CHECK(::dup2(mSlave, 1) == 1);
            mReader = std::thread([this]()
            {
                while (!mStop.load())
                {
                    pollfd fds{ mMaster, POLLIN, 0 };
                    if (::poll(&fds, 1, 50) > 0)
                    {
                        char buffer[4096];
                        const ssize_t count{ ::read(mMaster, buffer, sizeof(buffer)) };
                        if (count <= 0)
                            break;
                        std::lock_guard<std::mutex> lock(mMutex);
                        mPending.append(buffer, static_cast<size_t>(count));
                        mChanged.notify_all();
                    }
                }
            });
        }

        ~Terminal()
        {
            mStop.store(true);
            mReader.join();
        }

        //!< Returns everything the console wrote so far and renders it on the screen.
        std::string sync()
        {
            const std::string marker{ "\x1B]777;" + std::to_string(++mMarker) + "\x07" };
            std::fflush(stdout);
            AREG_CHILD_CHECK(::write(1, marker.data(), marker.size()) == static_cast<ssize_t>(marker.size()));
            read_until(marker);
            std::lock_guard<std::mutex> lock(mMutex);
            const size_t end{ mPending.find(marker) };
            std::string data{ mPending.substr(0u, end) };
            mPending.erase(0u, end + marker.size());
            screen.feed(data);
            return data;
        }

        //!< Reads the terminal until the token arrives.
        void read_until(const std::string& token)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            AREG_CHILD_CHECK(mChanged.wait_for(lock, std::chrono::seconds(60), [&]() { return mPending.find(token) != std::string::npos; }));
        }

        //!< Types the keys on the terminal.
        void type(const std::string& keys)
        {
            AREG_CHILD_CHECK(::write(mMaster, keys.data(), keys.size()) == static_cast<ssize_t>(keys.size()));
        }

        //!< Returns the local modes of the terminal.
        tcflag_t local_modes() const
        {
            termios modes{};
            AREG_CHILD_CHECK(::tcgetattr(mSlave, &modes) == 0);
            return modes.c_lflag;
        }

        VtScreen screen;

    private:
        int         mMaster{ -1 };
        int         mSlave{ -1 };
        int         mMarker{ 0 };
        std::string mPending;
        std::mutex  mMutex;
        std::condition_variable mChanged;
        std::atomic<bool> mStop{ false };
        std::thread mReader;
    };

#endif  // !defined(_WIN32)
}

//////////////////////////////////////////////////////////////////////////
// Redirected streams, every backend.
//////////////////////////////////////////////////////////////////////////

// Reproduces the defect: the ANSI console wrote escape sequences into a pipe or a file.
TEST(ConsoleRedirectedTest, redirected_output_is_plain_lines)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream out(true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.is_ready());
        AREG_CHILD_CHECK(!console.is_terminal());
        AREG_CHILD_CHECK(console.screen_size().posX == 0 && console.screen_size().posY == 0);
        console.output_txt({ 0, 0 }, "Title");
        console.output_msg({ 5, 3 }, "Rate %d", 7);
        console.clear_line({ 0, 4 });
        console.set_cursor_cur_position({ 3, 3 });
        console.output_txt({ 0, 9 }, "\nQuit the application ...\n");
        console.refresh_screen();
        console.uninitialize();
        const std::string text{ out.read_all() };
        AREG_CHILD_CHECK(text.find('\x1B') == std::string::npos);
        AREG_CHILD_CHECK(text.find("Title\n") != std::string::npos);
        AREG_CHILD_CHECK(text.find("Rate 7\n") != std::string::npos);
        AREG_CHILD_CHECK(text.find("Quit the application ...\n") != std::string::npos);
    );
}

// Reproduces the defect: a redirected input line longer than the buffer ended the process on Windows.
TEST(ConsoleRedirectedTest, long_input_line_is_cut_and_the_next_line_is_read)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream in(false);
        RedirectedStream out(true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        in.write(std::string(700u, 'a') + "\n-q\n");
        in.close();
        const areg::String first{ console.wait_for_input(nullptr) };
        AREG_CHILD_CHECK(first == std::string(Console::INPUT_BUFFER_SIZE - 1u, 'a'));
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "-q");
        console.uninitialize();
        out.read_all();
    );
}

TEST(ConsoleRedirectedTest, closed_input_ends_the_wait)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream in(false);
        RedirectedStream out(true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        in.write("one\n");
        in.close();
        uint32_t calls{ 0u };
        const areg::String result{ console.wait_for_input([&calls](const areg::String&) { ++calls; return false; }) };
        AREG_CHILD_CHECK(result.is_empty());
        AREG_CHILD_CHECK(calls == 1u);
        console.uninitialize();
        out.read_all();
    );
}

#if !defined(_WIN32)

//////////////////////////////////////////////////////////////////////////
// A terminal of a given size, ANSI backend.
//////////////////////////////////////////////////////////////////////////

// Reproduces the defect: text wider than the terminal wrapped, rows past the bottom piled up and the screen scrolled.
TEST(ConsoleTerminalTest, long_rows_at_80x24_never_wrap_or_scroll)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(80, 24);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.is_terminal());
        console.lock_console();
        console.output_txt({ 0, 0 }, "Title");
        for (int32_t row = 1; row < 32; ++row)
            console.output_str({ 0, row }, areg::String(std::string(186u, static_cast<char>('A' + row % 26)).c_str()));
        console.output_txt({ 0, 21 }, "\nQuit the application ...\n");
        console.set_cursor_cur_position({ 10, 3 });
        console.unlock_console();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.scrolled() == 0);
        AREG_CHILD_CHECK(terminal.screen.wrapped() == 0);
        AREG_CHILD_CHECK(terminal.screen.row(0) == "Title");
        AREG_CHILD_CHECK(terminal.screen.row(1) == std::string(80u, 'B'));
        AREG_CHILD_CHECK(terminal.screen.row(22) == "Quit the application ...");
        AREG_CHILD_CHECK(terminal.screen.row(23).empty());
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 10 && terminal.screen.cursor_y() == 3);
        AREG_CHILD_CHECK(terminal.screen.cursor_visible());
    );
}

// Reproduces the defect: the output of a string_view read past its size.
TEST(ConsoleTerminalTest, string_view_output_stops_at_its_size)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        Console& console = Console::instance();
        const char text[]{ "ABCDEFG" };
        console.output_txt({ 0, 2 }, std::string_view(text, 3u));
        const std::string bytes{ terminal.sync() };
        AREG_CHILD_CHECK(bytes.find('D') == std::string::npos);
        AREG_CHILD_CHECK(terminal.screen.row(2) == "ABC");
    );
}

// Reproduces the defect: the ANSI console was 1-based, so { 0, y } and { 1, y } drew on the same cell.
TEST(ConsoleTerminalTest, coordinates_are_zero_based)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        Console& console = Console::instance();
        console.output_txt({ 0, 0 }, "X");
        console.output_txt({ 1, 1 }, "Y");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(0) == "X");
        AREG_CHILD_CHECK(terminal.screen.row(1) == " Y");
        AREG_CHILD_CHECK(console.screen_size().posX == 40 && console.screen_size().posY == 10);
    );
}

TEST(ConsoleTerminalTest, cursor_is_hidden_while_drawing_and_returns_to_its_position)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        terminal.sync();
        console.set_cursor_cur_position({ 5, 3 });
        console.output_txt({ 0, 10 }, "rate");
        const std::string bytes{ terminal.sync() };
        AREG_CHILD_CHECK(bytes.find("\x1B[?25l") < bytes.find("rate"));
        AREG_CHILD_CHECK(bytes.rfind("\x1B[?25h") > bytes.find("rate"));
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 5 && terminal.screen.cursor_y() == 3);
        AREG_CHILD_CHECK(terminal.screen.cursor_visible());
        AREG_CHILD_CHECK(console.cursor_cur_position().posX == 5 && console.cursor_cur_position().posY == 3);
    );
}

TEST(ConsoleTerminalTest, outputs_under_the_lock_arrive_on_unlock)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        terminal.sync();
        console.lock_console();
        console.output_txt({ 0, 1 }, "first");
        console.lock_console();
        console.output_txt({ 0, 2 }, "second");
        console.unlock_console();
        AREG_CHILD_CHECK(terminal.sync().empty());
        console.unlock_console();
        const std::string bytes{ terminal.sync() };
        AREG_CHILD_CHECK(bytes.find("first") < bytes.find("second"));
        AREG_CHILD_CHECK(terminal.screen.row(1) == "first" && terminal.screen.row(2) == "second");
    );
}

TEST(ConsoleTerminalTest, print_writes_at_the_cursor_after_positioned_output)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        console.set_cursor_cur_position({ 2, 2 });
        console.lock_console();
        console.output_txt({ 0, 6 }, "row");
        console.print_txt("abc");
        console.unlock_console();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(2) == "  abc");
        AREG_CHILD_CHECK(terminal.screen.row(6) == "row");
    );
}

TEST(ConsoleTerminalTest, clear_screen_and_line_moves)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        console.output_txt({ 0, 4 }, "gone");
        console.set_cursor_cur_position({ 6, 5 });
        console.clear_screen();
        terminal.sync();
        AREG_CHILD_CHECK(!terminal.screen.shows("gone"));
        AREG_CHILD_CHECK(console.cursor_cur_position().posX == 0 && console.cursor_cur_position().posY == 0);
        console.move_cursor_one_line_down();
        console.move_cursor_one_line_down();
        console.move_cursor_one_line_up();
        console.refresh_screen();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 0 && terminal.screen.cursor_y() == 1);
        console.output_txt({ 0, 1 }, "keep and clear");
        console.set_cursor_cur_position({ 4, 1 });
        console.clear_current_line();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(1) == "keep");
    );
}

#if defined(__linux__) || defined(__APPLE__)

//////////////////////////////////////////////////////////////////////////
// Keys typed on the controlling terminal.
//////////////////////////////////////////////////////////////////////////

TEST(ConsoleKeyboardTest, typed_line_is_edited_and_the_terminal_is_restored)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        const tcflag_t before{ terminal.local_modes() };
        AREG_CHILD_CHECK((before & ICANON) != 0 && (before & ECHO) != 0);
        Console& console = Console::instance();
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == 0);
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 1 }, "Prompt: ");
        console.set_cursor_cur_position({ 8, 1 });
        terminal.type("xabc\x1B[D\x1B[D\x7F" "X\x1B[H\x1B[3~\x1B[F!\x1BOH\x1B[1;5C\x1Bz\r");
        const areg::String line{ console.wait_for_input(nullptr) };
        AREG_CHILD_CHECK(line == "Xbc!");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(1) == "Prompt: Xbc!");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 8 && terminal.screen.cursor_y() == 1);
        console.uninitialize();
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == (ICANON | ECHO));
    );
}

TEST(ConsoleKeyboardTest, typed_line_longer_than_the_buffer_keeps_its_start)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 50, 9 });
        terminal.type(std::string(600u, 'b') + "\r-q\r");
        const areg::String line{ console.wait_for_input(nullptr) };
        AREG_CHILD_CHECK(line == std::string(Console::INPUT_BUFFER_SIZE - 1u, 'b'));
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "-q");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.scrolled() == 0 && terminal.screen.wrapped() == 0);
    );
}

TEST(ConsoleKeyboardTest, disabled_input_interrupts_the_waiting_read)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 7, 4 });
        terminal.sync();
        areg::String result{ "unset" };
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("partial");
        terminal.read_until("partial");
        console.enable_console_input(false);
        reader.join();
        AREG_CHILD_CHECK(result.is_empty());
    );
}

#endif  // defined(__linux__) || defined(__APPLE__)

#endif  // !defined(_WIN32)
