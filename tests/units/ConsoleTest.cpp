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
// Declares the pseudo terminal functions: g++ on Cygwin does not predefine it, unlike on Linux.
#if defined(__CYGWIN__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

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
#include <vector>

#if defined(_WIN32)
    #include <io.h>
    #include <fcntl.h>
    #include <share.h>
    #include <Windows.h>
#else
    #include <fcntl.h>
    #include <poll.h>
    #include <signal.h>
    #include <sys/ioctl.h>
    #include <sys/wait.h>
    #include <termios.h>
    #include <unistd.h>
#endif

using areg::ext::Console;

#if defined(__SANITIZE_THREAD__)
    #define AREG_TEST_THREAD_SANITIZER  1
#elif defined(__has_feature)
    #if __has_feature(thread_sanitizer)
        #define AREG_TEST_THREAD_SANITIZER  1
    #endif
#endif

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
            int null{ -1 };
            AREG_CHILD_CHECK(::_sopen_s(&null, "NUL", _O_WRONLY, _SH_DENYNO, 0) == 0);
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
#if defined(__CYGWIN__)
            // A new session stops Cygwin from making a duplicated slave the controlling terminal, whose master reads end of file.
            const bool session{ true };
#else
            const bool session{ controlling };
#endif
            if (session)
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

        //!< Changes the size of the terminal.
        void resize(int columns, int rows)
        {
            winsize size{};
            size.ws_col = static_cast<unsigned short>(columns);
            size.ws_row = static_cast<unsigned short>(rows);
            AREG_CHILD_CHECK(::ioctl(mSlave, TIOCSWINSZ, &size) == 0);
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

#else   // defined(_WIN32)

    #ifndef PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE
        #define PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE 0x00020016
    #endif  // PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE

    //!< A pseudo console of a given size that becomes the console of this process. A helper
    //!< process that reads no input and writes nothing keeps it alive, in a job that ends it with
    //!< its children; a thread renders what the pseudo console draws.
    class Terminal
    {
        using CreateFunc = HRESULT (WINAPI *)(COORD, HANDLE, HANDLE, DWORD, void**);
        using ResizeFunc = HRESULT (WINAPI *)(void*, COORD);
        using CloseFunc  = void (WINAPI *)(void*);

    public:
        //!< Returns true if the system offers pseudo consoles.
        static bool supported()
        {
            return (::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"), "CreatePseudoConsole") != nullptr);
        }

        Terminal(int columns, int rows)
            : screen(columns, rows)
        {
            HMODULE kernel{ ::GetModuleHandleW(L"kernel32.dll") };
            mCreate = reinterpret_cast<CreateFunc>(reinterpret_cast<void*>(::GetProcAddress(kernel, "CreatePseudoConsole")));
            mResize = reinterpret_cast<ResizeFunc>(reinterpret_cast<void*>(::GetProcAddress(kernel, "ResizePseudoConsole")));
            mClose  = reinterpret_cast<CloseFunc>(reinterpret_cast<void*>(::GetProcAddress(kernel, "ClosePseudoConsole")));
            AREG_CHILD_CHECK((mCreate != nullptr) && (mResize != nullptr) && (mClose != nullptr));

            HANDLE inRead{ nullptr };
            HANDLE outWrite{ nullptr };
            AREG_CHILD_CHECK(::CreatePipe(&inRead, &mInput, nullptr, 0) != FALSE);
            AREG_CHILD_CHECK(::CreatePipe(&mOutput, &outWrite, nullptr, 0) != FALSE);
            AREG_CHILD_CHECK(SUCCEEDED(mCreate(_size(columns, rows), inRead, outWrite, 0, &mConsole)));
            ::CloseHandle(inRead);
            ::CloseHandle(outWrite);
            mReader = std::thread([this]()
            {
                char buffer[4096];
                DWORD count{ 0u };
                while ((::ReadFile(mOutput, buffer, static_cast<DWORD>(sizeof(buffer)), &count, nullptr) != FALSE) && (count != 0u))
                {
                    std::lock_guard<std::mutex> lock(mMutex);
                    mPending.append(buffer, static_cast<size_t>(count));
                    mChanged.notify_all();
                }
            });

            SIZE_T bytes{ 0u };
            ::InitializeProcThreadAttributeList(nullptr, 1, 0, &bytes);
            std::vector<char> attributes(bytes);
            LPPROC_THREAD_ATTRIBUTE_LIST list{ reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(attributes.data()) };
            AREG_CHILD_CHECK(::InitializeProcThreadAttributeList(list, 1, 0, &bytes) != FALSE);
            AREG_CHILD_CHECK(::UpdateProcThreadAttribute(list, 0, PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE, mConsole, sizeof(mConsole), nullptr, nullptr) != FALSE);
            STARTUPINFOEXW info{};
            info.StartupInfo.cb = sizeof(info);
            info.lpAttributeList = list;
            wchar_t system[MAX_PATH]{};
            AREG_CHILD_CHECK(::GetSystemDirectoryW(system, MAX_PATH) != 0u);
            std::wstring command{ std::wstring(L"\"") + system + L"\\cmd.exe\" /d /c \"ping -n 120 127.0.0.1 >nul\"" };
            mJob = ::CreateJobObjectW(nullptr, nullptr);
            AREG_CHILD_CHECK(mJob != nullptr);
            JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};
            limits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
            AREG_CHILD_CHECK(::SetInformationJobObject(mJob, JobObjectExtendedLimitInformation, &limits, sizeof(limits)) != FALSE);
            AREG_CHILD_CHECK(::CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED, nullptr, nullptr, &info.StartupInfo, &mHelper) != FALSE);
            ::DeleteProcThreadAttributeList(list);
            AREG_CHILD_CHECK(::AssignProcessToJobObject(mJob, mHelper.hProcess) != FALSE);
            AREG_CHILD_CHECK(::ResumeThread(mHelper.hThread) != static_cast<DWORD>(-1));

            ::FreeConsole();
            bool attached{ false };
            for (int i = 0; (i < 6000) && (attached == false); ++i)
            {
                attached = (::AttachConsole(mHelper.dwProcessId) != FALSE);
                if (attached == false)
                    AREG_CHILD_CHECK(::WaitForSingleObject(mHelper.hProcess, 10u) == WAIT_TIMEOUT);
            }

            AREG_CHILD_CHECK(attached);
            FILE* stream{ nullptr };
            AREG_CHILD_CHECK(::freopen_s(&stream, "CONOUT$", "w", stdout) == 0);
            AREG_CHILD_CHECK(::freopen_s(&stream, "CONIN$", "r", stdin) == 0);
            mConsoleOutput = ::CreateFileW(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            mConsoleInput  = ::CreateFileW(L"CONIN$", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
            AREG_CHILD_CHECK((mConsoleOutput != INVALID_HANDLE_VALUE) && (mConsoleInput != INVALID_HANDLE_VALUE));
            AREG_CHILD_CHECK(::SetStdHandle(STD_OUTPUT_HANDLE, mConsoleOutput) != FALSE);
            AREG_CHILD_CHECK(::SetStdHandle(STD_INPUT_HANDLE, mConsoleInput) != FALSE);
        }

        ~Terminal()
        {
            Console::instance().uninitialize();
            FILE* stream{ nullptr };
            ::freopen_s(&stream, "NUL", "w", stdout);
            ::freopen_s(&stream, "NUL", "r", stdin);
            ::SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(stdout))));
            ::SetStdHandle(STD_INPUT_HANDLE, reinterpret_cast<HANDLE>(::_get_osfhandle(::_fileno(stdin))));
            ::CloseHandle(mConsoleOutput);
            ::CloseHandle(mConsoleInput);
            ::FreeConsole();
            ::CloseHandle(mJob);
            ::CloseHandle(mHelper.hThread);
            ::CloseHandle(mHelper.hProcess);
            mClose(mConsole);
            ::CloseHandle(mInput);
            mReader.join();
            ::CloseHandle(mOutput);
        }

        //!< Renders what arrived until the condition holds.
        template<typename Condition>
        void wait_for(Condition condition)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            AREG_CHILD_CHECK(mChanged.wait_for(lock, std::chrono::seconds(60), [&]()
            {
                screen.feed(mPending);
                mPending.clear();
                return condition();
            }));
        }

        //!< Types the keys on the terminal.
        void type(const std::string& keys)
        {
            DWORD written{ 0u };
            AREG_CHILD_CHECK((::WriteFile(mInput, keys.data(), static_cast<DWORD>(keys.size()), &written, nullptr) != FALSE) && (written == keys.size()));
        }

        //!< Changes the size of the terminal.
        void resize(int columns, int rows)
        {
            AREG_CHILD_CHECK(SUCCEEDED(mResize(mConsole, _size(columns, rows))));
        }

        VtScreen screen;

    private:
        static COORD _size(int columns, int rows)
        {
            return COORD{ static_cast<SHORT>(columns), static_cast<SHORT>(rows) };
        }

        CreateFunc  mCreate{ nullptr };
        ResizeFunc  mResize{ nullptr };
        CloseFunc   mClose{ nullptr };
        void*       mConsole{ nullptr };
        HANDLE      mJob{ nullptr };
        HANDLE      mConsoleOutput{ INVALID_HANDLE_VALUE };
        HANDLE      mConsoleInput{ INVALID_HANDLE_VALUE };
        HANDLE      mInput{ nullptr };
        HANDLE      mOutput{ nullptr };
        PROCESS_INFORMATION mHelper{};
        std::string mPending;
        std::mutex  mMutex;
        std::condition_variable mChanged;
        std::thread mReader;
    };

#endif  // !defined(_WIN32)
}

//////////////////////////////////////////////////////////////////////////
// Redirected streams.
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

TEST(ConsoleRedirectedTest, formatted_and_raw_reads)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream in(false);
        RedirectedStream out(true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        in.write("42 word\ntoken\n  data  \n");
        in.close();
        int number{ 0 };
        char word[16]{};
        AREG_CHILD_CHECK(console.read_inputs("%d %15s", &number, word));
        AREG_CHILD_CHECK((number == 42) && (std::string(word) == "word"));
        AREG_CHILD_CHECK(console.read_string() == "token");
        char buffer[32]{};
        AREG_CHILD_CHECK(!Console::read_console_data(buffer, 0u));
        AREG_CHILD_CHECK(Console::read_console_data(buffer, static_cast<uint32_t>(sizeof(buffer))));
        AREG_CHILD_CHECK(std::string(buffer) == "data");
        AREG_CHILD_CHECK(!Console::read_console_data(buffer, static_cast<uint32_t>(sizeof(buffer))));
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

TEST(ConsoleRedirectedTest, cursor_and_clear_calls_write_nothing)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream out(true);
        Console& console = Console::instance();
        uint32_t calls{ 0u };
        console.set_resize_callback([&calls](Console::Coord) { ++calls; });
        console.set_cursor_cur_position({ 4, 2 });
        console.move_to_line(3);
        console.move_cursor_one_line_up();
        console.move_cursor_one_line_down();
        console.save_cursor_position();
        console.restore_cursor_position();
        console.clear_current_line();
        console.clear_line({ 0, 1 });
        console.clear_screen();
        console.output_txt({ 0, 0 }, "\x1B[31m\x1B[0m\n");
        console.print_txt("print\x1B[2K\r\n");
        console.print_msg("%s-%d", "msg", 5);
        console.output_below("Completed");
        console.uninitialize();
        AREG_CHILD_CHECK(!console.is_ready());
        console.output_txt({ 0, 0 }, "after");
        console.print_txt("after");
        AREG_CHILD_CHECK(calls == 0u);
        const std::string text{ out.read_all() };
        AREG_CHILD_CHECK(text.find('\x1B') == std::string::npos);
        AREG_CHILD_CHECK(text.find("print\nmsg-5") != std::string::npos);
        AREG_CHILD_CHECK(text.find("Completed\n") != std::string::npos);
        AREG_CHILD_CHECK(text.find("after") == std::string::npos);
    );
}

TEST(ConsoleRedirectedTest, disabled_released_and_closed_input_reads_nothing)
{
    AREG_EXPECT_IN_CHILD(
        RedirectedStream in(false);
        RedirectedStream out(true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        in.write("abc\nx y\r\nlost\n");
        int number{ 0 };
        AREG_CHILD_CHECK(console.read_inputs("%d", &number) == false);
        AREG_CHILD_CHECK(console.wait_for_input([](const areg::String& line) { return line == "x y"; }) == "x y");
        AREG_CHILD_CHECK(console.enable_console_input(false));
        char buffer[16]{};
        AREG_CHILD_CHECK(Console::read_console_data(buffer, sizeof(buffer)) == false);
        AREG_CHILD_CHECK(Console::read_console_data(buffer, sizeof(buffer)) == false);
        AREG_CHILD_CHECK(console.enable_console_input(true));
        in.close();
        AREG_CHILD_CHECK(console.read_string().is_empty());
        console.uninitialize();
        AREG_CHILD_CHECK(console.wait_for_input(nullptr).is_empty());
        AREG_CHILD_CHECK(console.enable_console_input(true) == false);
        AREG_CHILD_CHECK(console.enable_console_input(false));
        AREG_CHILD_CHECK(Console::read_console_data(buffer, sizeof(buffer)) == false);
        console.output_below("after");
        AREG_CHILD_CHECK(out.read_all().find("after") == std::string::npos);
    );
}

//////////////////////////////////////////////////////////////////////////
// A terminal of a given size.
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

TEST(ConsoleTerminalTest, prints_before_any_drawing_are_a_stream)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        Console& console = Console::instance();
        console.print_txt("first\nsecond\n");
        console.print_msg("run %d\n", 3);
        console.output_below("done");
        const std::string bytes{ terminal.sync() };
        AREG_CHILD_CHECK(bytes.find("\x1B[2J") == std::string::npos);
        AREG_CHILD_CHECK(terminal.screen.row(0) == "first" && terminal.screen.row(1) == "second");
        AREG_CHILD_CHECK(terminal.screen.row(2) == "run 3" && terminal.screen.row(3) == "done");
        console.uninitialize();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 0 && terminal.screen.cursor_y() == 4);
        AREG_CHILD_CHECK(terminal.screen.row(1) == "second");
    );
}

TEST(ConsoleTerminalTest, output_below_follows_the_last_drawn_row)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 6);
        Console& console = Console::instance();
        console.output_txt({ 0, 2 }, "layout");
        console.set_cursor_cur_position({ 3, 0 });
        console.output_below("first");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(3) == "first");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 3 && terminal.screen.cursor_y() == 0);
        console.output_txt({ 0, 5 }, "bottom");
        console.output_below("last");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(5) == "last");
        AREG_CHILD_CHECK(terminal.screen.scrolled() == 0);
    );
}

TEST(ConsoleTerminalTest, resize_calls_the_callback_once_with_the_new_size)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        std::vector<Console::Coord> sizes;
        console.set_resize_callback([&](Console::Coord size)
        {
            sizes.push_back(size);
            console.clear_screen();
            console.output_txt({ 0, 0 }, "redrawn");
            console.set_cursor_cur_position({ 0, 2 });
        });
        console.output_txt({ 0, 1 }, "same size");
        terminal.sync();
        AREG_CHILD_CHECK(sizes.empty());
        terminal.resize(60, 20);
        console.output_txt({ 0, 1 }, "new size");
        terminal.sync();
        AREG_CHILD_CHECK(sizes.size() == 1u);
        AREG_CHILD_CHECK(sizes[0].posX == 60 && sizes[0].posY == 20);
        AREG_CHILD_CHECK(terminal.screen.row(0) == "redrawn");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 0 && terminal.screen.cursor_y() == 2);
        console.output_txt({ 0, 3 }, "again");
        AREG_CHILD_CHECK(sizes.size() == 1u);
        terminal.resize(50, 15);
        AREG_CHILD_CHECK(console.screen_size().posX == 50 && console.screen_size().posY == 15);
        AREG_CHILD_CHECK(sizes.size() == 2u);
        console.set_resize_callback(nullptr);
        terminal.resize(70, 22);
        console.output_txt({ 0, 1 }, "no callback");
        AREG_CHILD_CHECK(sizes.size() == 2u);
    );
}

TEST(ConsoleTerminalTest, resize_in_the_callback_is_not_nested)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        uint32_t depth{ 0u };
        uint32_t calls{ 0u };
        bool nested{ false };
        console.set_resize_callback([&](Console::Coord)
        {
            nested = nested || (depth != 0u);
            ++depth;
            if (++calls == 1u)
            {
                terminal.resize(44, 13);
                console.output_txt({ 0, 0 }, "inner");
            }
            --depth;
        });
        terminal.resize(42, 12);
        console.output_txt({ 0, 0 }, "outer");
        AREG_CHILD_CHECK((calls == 1u) && !nested);
        console.output_txt({ 0, 0 }, "next");
        AREG_CHILD_CHECK((calls == 2u) && !nested);
    );
}

TEST(ConsoleTerminalTest, clearing_the_callback_waits_until_it_returns)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        std::mutex mutex;
        std::condition_variable changed;
        bool entered{ false };
        bool clearing{ false };
        bool returned{ false };
        console.set_resize_callback([&](Console::Coord)
        {
            std::unique_lock<std::mutex> lock(mutex);
            entered = true;
            changed.notify_all();
            changed.wait(lock, [&]() { return clearing; });
            returned = true;
        });
        terminal.resize(50, 14);
        std::thread drawer([&]() { console.output_txt({ 0, 0 }, "draw"); });
        {
            std::unique_lock<std::mutex> lock(mutex);
            changed.wait(lock, [&]() { return entered; });
        }

        bool seen{ false };
        std::thread clearer([&]()
        {
            {
                std::lock_guard<std::mutex> lock(mutex);
                clearing = true;
                changed.notify_all();
            }

            console.set_resize_callback(nullptr);
            std::lock_guard<std::mutex> lock(mutex);
            seen = returned;
        });
        clearer.join();
        drawer.join();
        AREG_CHILD_CHECK(seen);
    );
}

TEST(ConsoleTerminalTest, unknown_terminal_size_clips_only_the_known_side)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(30, 0);
        Console& console = Console::instance();
        uint32_t calls{ 0u };
        console.set_resize_callback([&calls](Console::Coord) { ++calls; });
        AREG_CHILD_CHECK(console.screen_size().posX == 30 && console.screen_size().posY == 0);
        console.output_txt({ 0, 40 }, "far row");
        console.output_below("one\ntwo");
        terminal.read_until("far row");
        terminal.read_until("\x1B[42;1H\x1B[Kone");
        terminal.read_until("\x1B[43;1H\x1B[Ktwo");
        terminal.resize(0, 0);
        AREG_CHILD_CHECK(console.screen_size().posX == 0 && console.screen_size().posY == 0);
        terminal.resize(40, 10);
        console.output_txt({ 0, 0 }, "known");
        AREG_CHILD_CHECK(calls == 0u);
        terminal.resize(41, 10);
        console.output_txt({ 0, 0 }, "resized");
        AREG_CHILD_CHECK(calls == 1u);
    );
}

TEST(ConsoleTerminalTest, output_redirected_after_the_start_has_no_size)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        Console& console = Console::instance();
        console.output_txt({ 0, 1 }, "drawn");
        terminal.sync();
        const int saved{ ::dup(1) };
        RedirectedStream out(true);
        AREG_CHILD_CHECK(console.screen_size().posX == 0 && console.screen_size().posY == 0);
        console.uninitialize();
        AREG_CHILD_CHECK(out.read_all().find("\x1B[?25h") != std::string::npos);
        AREG_CHILD_CHECK(::dup2(saved, 1) == 1);
        ::close(saved);
    );
}

TEST(ConsoleTerminalTest, restored_cursor_returns_only_after_a_drawing)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        Console& console = Console::instance();
        console.print_txt("stream ");
        console.restore_cursor_position();
        AREG_CHILD_CHECK(terminal.sync().find("stream ") != std::string::npos);
        console.output_txt({ 0, 2 }, "drawn");
        console.set_cursor_cur_position({ 5, 4 });
        console.restore_cursor_position();
        console.print_txt("p");
        console.lock_console();
        console.restore_cursor_position();
        console.restore_cursor_position();
        console.unlock_console();
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 5 && terminal.screen.cursor_y() == 4);
        console.print_txt("q");
        console.output_below("end");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(4) == "     qend");
        console.uninitialize();
        console.clear_line({ 0, 2 });
        console.clear_screen();
        AREG_CHILD_CHECK(terminal.sync().find("\x1B[2J") == std::string::npos);
    );
}

TEST(ConsoleTerminalTest, terminal_type_decides_the_drawing)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        AREG_CHILD_CHECK(::unsetenv("TERM") == 0);
        AREG_CHILD_CHECK(Console::instance().is_terminal());
    );

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 10);
        AREG_CHILD_CHECK(::setenv("TERM", "dumb", 1) == 0);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.is_terminal() == false);
        console.output_txt({ 0, 1 }, "plain");
        AREG_CHILD_CHECK(terminal.sync().find('\x1B') == std::string::npos);
    );
}

TEST(ConsoleTerminalTest, callback_replaced_in_the_callback_does_not_wait)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(40, 12);
        Console& console = Console::instance();
        uint32_t first{ 0u };
        uint32_t second{ 0u };
        console.output_txt({ 0, 0 }, "start");
        console.set_resize_callback([&](Console::Coord)
        {
            ++first;
            console.set_resize_callback([&](Console::Coord) { ++second; });
        });
        terminal.resize(40, 14);
        console.output_txt({ 0, 0 }, "rows");
        terminal.resize(41, 14);
        console.output_txt({ 0, 0 }, "columns");
        AREG_CHILD_CHECK((first == 1u) && (second == 1u));
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

TEST(ConsoleKeyboardTest, disabled_input_leaves_no_key_in_the_terminal)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 5, 3 });
        terminal.sync();
        terminal.type("-q\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "-q");
        AREG_CHILD_CHECK(console.enable_console_input(false));
        int pending{ -1 };
        AREG_CHILD_CHECK((::ioctl(STDIN_FILENO, FIONREAD, &pending) == 0) && (pending == 0));
        AREG_CHILD_CHECK(console.enable_console_input(true));
        terminal.type("next\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "next");
    );
}

TEST(ConsoleKeyboardTest, editing_keys_move_and_clear_the_line)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 2, 2 });
        terminal.type("\x7F" "abc\x1B[3~\x01X\x05Y\x1B[1~1\x1B[4~4\x1B[7~7\x1B[8~8\x1B[5~\x1BOq\x1BO3~\x15" "ok\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "ok");
        terminal.type("\x7F" "abc\x1B[3~\x01X\x05Y\x1B[1~1\x1B[4~4\x1B[7~7\x1B[8~8\x1B[5~\x1BOq\x1BO3~\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "71XabcY48");
    );
}

TEST(ConsoleKeyboardTest, formatted_read_edits_the_typed_line)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 2 }, "Number: ");
        console.set_cursor_cur_position({ 8, 2 });
        terminal.type("\r19\x7F" "7 x\r");
        int number{ 0 };
        char word[8]{};
        AREG_CHILD_CHECK(console.read_inputs("%d %7s", &number, word) && (number == 17) && (std::string(word) == "x"));
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == 0);
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(2) == "Number: 17 x");
    );
}

TEST(ConsoleKeyboardTest, continued_process_returns_to_key_mode)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console::instance();
        termios modes{};
        AREG_CHILD_CHECK(::tcgetattr(0, &modes) == 0);
        modes.c_lflag |= static_cast<tcflag_t>(ICANON | ECHO);
        AREG_CHILD_CHECK(::tcsetattr(0, TCSANOW, &modes) == 0);
        AREG_CHILD_CHECK(::raise(SIGCONT) == 0);
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == 0);
    );
}

// Reproduces the defect: SIGTERM left the terminal without echo.
TEST(ConsoleKeyboardTest, terminating_signal_restores_the_terminal_and_ends_the_process)
{
#if defined(AREG_TEST_THREAD_SANITIZER)
    GTEST_SKIP() << "a process forked from threads under ThreadSanitizer runs no signal handler";
#endif

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console::instance();
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == 0);
        const pid_t child{ ::fork() };
        AREG_CHILD_CHECK(child >= 0);
        if (child == 0)
        {
            ::raise(SIGTERM);
            ::_exit(0);
        }

        int status{ 0 };
        AREG_CHILD_CHECK(::waitpid(child, &status, 0) == child);
        AREG_CHILD_CHECK(WIFSIGNALED(status) && (WTERMSIG(status) == SIGTERM));
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == (ICANON | ECHO));
    );
}

TEST(ConsoleKeyboardTest, signal_with_an_application_action_is_left_alone)
{
    AREG_EXPECT_IN_CHILD(
        AREG_CHILD_CHECK(::signal(SIGINT, SIG_IGN) != SIG_ERR);
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        struct sigaction action{};
        AREG_CHILD_CHECK(::sigaction(SIGINT, nullptr, &action) == 0);
        AREG_CHILD_CHECK(action.sa_handler == SIG_IGN);
        console.uninitialize();
        AREG_CHILD_CHECK(::sigaction(SIGINT, nullptr, &action) == 0);
        AREG_CHILD_CHECK(action.sa_handler == SIG_IGN);
    );
}

TEST(ConsoleKeyboardTest, disabled_input_interrupts_an_escape_sequence)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 7, 4 });
        terminal.sync();
        areg::String result{ "unset" };
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("part\x1B[");
        terminal.read_until("part");
        console.enable_console_input(false);
        reader.join();
        AREG_CHILD_CHECK(result.is_empty());
    );
}

// Reproduces the defect: an output over the row of the input line erased the typed text until the next key.
TEST(ConsoleKeyboardTest, typed_text_stays_when_an_output_draws_over_its_row)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 5 }, "Type: ");
        console.set_cursor_cur_position({ 6, 5 });
        terminal.sync();
        areg::String result;
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("abc\x1B[D");
        terminal.read_until("abc\x1B[6;9H");
        console.output_txt({ 0, 5 }, "Type: ");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(5) == "Type: abc");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 8 && terminal.screen.cursor_y() == 5);
        console.clear_line({ 0, 5 });
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(5) == "      abc");
        console.clear_screen();
        console.output_txt({ 0, 0 }, "Title");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(0) == "Title");
        AREG_CHILD_CHECK(terminal.screen.row(5) == "      abc");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 8 && terminal.screen.cursor_y() == 5);
        terminal.type("X\r");
        reader.join();
        AREG_CHILD_CHECK(result == "abXc");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 6 && terminal.screen.cursor_y() == 5);
    );
}

TEST(ConsoleKeyboardTest, up_and_down_recall_the_entered_lines)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.history_size() == Console::HISTORY_DEFAULT_SIZE);
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 1 }, "Cmd: ");
        console.set_cursor_cur_position({ 5, 1 });
        terminal.type("\x1B[A\x1B[Bone\rtwo\rtwo\r\rthree\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "one");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "two");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "two");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "three");
        terminal.type("dr\x1B[A\x1B[A\x1B[A\x1B[A\x1B[BX\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "twoX");
        terminal.type("new\x1B[A\x1B[B\x1B[B\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "new");
        terminal.type("\x1BOA\x1B[A\x1B[A\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "three");
        terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(1) == "Cmd: three");
        AREG_CHILD_CHECK(terminal.screen.cursor_x() == 5 && terminal.screen.cursor_y() == 1);
        char buffer[4]{};
        terminal.type("\x1B[A\x1B[A\r");
        AREG_CHILD_CHECK(Console::read_console_data(buffer, 4u) && (std::string(buffer) == "new"));
        terminal.type("\x1B[A\x1B[A\x1B[A\x1B[A\r");
        AREG_CHILD_CHECK(Console::read_console_data(buffer, 4u) && (std::string(buffer) == "two"));
    );
}

TEST(ConsoleKeyboardTest, history_size_drops_the_oldest_lines_and_zero_disables_the_recall)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_history_size(Console::HISTORY_MAX_SIZE + 100u);
        AREG_CHILD_CHECK(console.history_size() == Console::HISTORY_MAX_SIZE);
        for (uint32_t i = 0; i < Console::HISTORY_MAX_SIZE + 2u; ++i)
        {
            terminal.type(std::to_string(i) + "\r");
            AREG_CHILD_CHECK(console.wait_for_input(nullptr) == std::to_string(i).c_str());
        }

        std::string keys;
        for (uint32_t i = 0; i <= Console::HISTORY_MAX_SIZE; ++i)
            keys += "\x1B[A";
        terminal.type(keys + "\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "2");

        console.set_history_size(2u);
        AREG_CHILD_CHECK(console.history_size() == 2u);
        terminal.type("\x1B[A\x1B[A\x1B[A\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == std::to_string(Console::HISTORY_MAX_SIZE + 1u).c_str());

        console.set_cursor_cur_position({ 0, 3 });
        terminal.sync();
        areg::String result;
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("\x1B[A\x1B[A");
        for (int i = 0; (i < 6000) && (terminal.screen.row(3) != "2"); ++i)
            terminal.sync();
        AREG_CHILD_CHECK(terminal.screen.row(3) == "2");
        console.set_history_size(1u);
        terminal.type("\x1B[B\r");
        reader.join();
        AREG_CHILD_CHECK(result == std::to_string(Console::HISTORY_MAX_SIZE + 1u).c_str());

        console.set_history_size(0u);
        AREG_CHILD_CHECK(console.history_size() == 0u);
        terminal.type("\x1B[Ax\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "x");
        console.set_history_size(5u);
        terminal.type("\x1B[Ay\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "y");
    );
}

TEST(ConsoleKeyboardTest, keys_without_an_effect_change_nothing)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 0, 5 });
        terminal.type("\x1B[Da\x02\xC3\xA9" "b\x1B[\x7F" "~\x1B[1;2;3;4;5;6~\x1B[200~c\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "abc");
    );
}

TEST(ConsoleKeyboardTest, escape_without_the_rest_of_a_sequence_is_dropped)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 0, 5 });
        terminal.sync();
        for (const char* escape : { "\x1B", "\x1B[" })
        {
            areg::String result;
            std::thread reader([&]() { result = console.wait_for_input(nullptr); });
            terminal.type("a");
            terminal.read_until("a");
            terminal.type(escape);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            terminal.type("A\x15x\r");
            reader.join();
            AREG_CHILD_CHECK(result == "x");
            terminal.sync();
        }
    );
}

TEST(ConsoleKeyboardTest, disabled_input_interrupts_a_lone_escape)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 0, 5 });
        terminal.sync();
        areg::String result{ "unset" };
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("part");
        terminal.read_until("part");
        terminal.type("\x1B");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        console.enable_console_input(false);
        reader.join();
        AREG_CHILD_CHECK(result.is_empty());
    );
}

TEST(ConsoleKeyboardTest, output_on_other_rows_leaves_the_typed_text)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 0, 5 });
        terminal.sync();
        areg::String result;
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("ab");
        terminal.read_until("ab");
        terminal.sync();
        console.output_txt({ 0, 2 }, "upper");
        console.output_txt({ 0, 8 }, "lower");
        const std::string drawn{ terminal.sync() };
        AREG_CHILD_CHECK(drawn.find("ab") == std::string::npos);
        AREG_CHILD_CHECK(terminal.screen.row(5) == "ab");
        terminal.type("\r");
        reader.join();
        AREG_CHILD_CHECK(result == "ab");
    );
}

TEST(ConsoleKeyboardTest, background_process_keeps_the_terminal_modes)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        const pid_t helper{ ::fork() };
        AREG_CHILD_CHECK(helper >= 0);
        if (helper == 0)
        {
            ::setpgid(0, 0);
            ::pause();
            ::_exit(0);
        }

        AREG_CHILD_CHECK((::setpgid(helper, helper) == 0) || (errno == EACCES));
        AREG_CHILD_CHECK(::tcsetpgrp(0, helper) == 0);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.is_terminal());
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == (ICANON | ECHO));
        ::signal(SIGTTOU, SIG_IGN);
        AREG_CHILD_CHECK(::tcsetpgrp(0, ::getpgrp()) == 0);
        ::kill(helper, SIGKILL);
        ::waitpid(helper, nullptr, 0);
    );
}

#if !defined(AREG_TEST_THREAD_SANITIZER)

namespace
{
    //!< The handler the console installed, and the calls of the handler that wraps it.
    void (*_consoleHandler)(int) { nullptr };
    std::atomic<int> _wrapperCalls{ 0 };

    //!< Calls the console handler on the first signal and only counts the next ones.
    void _wrapper(int sig)
    {
        if (++_wrapperCalls == 1)
        {
            _consoleHandler(sig);
        }
    }
}

TEST(ConsoleKeyboardTest, signal_handler_restores_the_terminal_and_raises_the_signal_again)
{
    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        struct sigaction action{};
        AREG_CHILD_CHECK(::sigaction(SIGTERM, nullptr, &action) == 0);
        AREG_CHILD_CHECK((action.sa_handler != SIG_DFL) && (action.sa_handler != SIG_IGN) && ((action.sa_flags & SA_SIGINFO) == 0));
        _consoleHandler = action.sa_handler;
        struct sigaction cont{};
        AREG_CHILD_CHECK(::sigaction(SIGCONT, nullptr, &cont) == 0);
        struct sigaction wrapper{};
        wrapper.sa_handler = &_wrapper;
        sigemptyset(&wrapper.sa_mask);
        AREG_CHILD_CHECK(::sigaction(SIGTERM, &wrapper, nullptr) == 0);

        AREG_CHILD_CHECK(::raise(SIGTERM) == 0);
        AREG_CHILD_CHECK(_wrapperCalls == 2);
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == (ICANON | ECHO));
        AREG_CHILD_CHECK(terminal.sync().find("\x1B[?25h\r") != std::string::npos);

        console.uninitialize();
        AREG_CHILD_CHECK(::sigaction(SIGTERM, nullptr, &action) == 0);
        AREG_CHILD_CHECK(action.sa_handler == &_wrapper);
        termios modes{};
        AREG_CHILD_CHECK(::tcgetattr(0, &modes) == 0);
        modes.c_lflag &= ~static_cast<tcflag_t>(ECHO);
        AREG_CHILD_CHECK(::tcsetattr(0, TCSANOW, &modes) == 0);
        cont.sa_handler(SIGCONT);
        _consoleHandler(SIGTERM);
        AREG_CHILD_CHECK(_wrapperCalls == 3);
        AREG_CHILD_CHECK((terminal.local_modes() & (ICANON | ECHO)) == ICANON);
    );
}

#endif  // !defined(AREG_TEST_THREAD_SANITIZER)

TEST(ConsoleKeyboardTest, actions_set_by_the_application_stay)
{
    AREG_EXPECT_IN_CHILD(
        struct sigaction info{};
        info.sa_sigaction = [](int, siginfo_t*, void*) {};
        info.sa_flags = SA_SIGINFO;
        sigemptyset(&info.sa_mask);
        AREG_CHILD_CHECK(::sigaction(SIGQUIT, &info, nullptr) == 0);
        Terminal terminal(60, 10, true);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(::signal(SIGABRT, SIG_IGN) != SIG_ERR);
        AREG_CHILD_CHECK(::sigaction(SIGTERM, &info, nullptr) == 0);
        console.uninitialize();
        struct sigaction action{};
        AREG_CHILD_CHECK((::sigaction(SIGQUIT, nullptr, &action) == 0) && ((action.sa_flags & SA_SIGINFO) != 0));
        AREG_CHILD_CHECK((::sigaction(SIGTERM, nullptr, &action) == 0) && ((action.sa_flags & SA_SIGINFO) != 0));
        AREG_CHILD_CHECK((::sigaction(SIGABRT, nullptr, &action) == 0) && (action.sa_handler == SIG_IGN));
        AREG_CHILD_CHECK((::sigaction(SIGINT, nullptr, &action) == 0) && (action.sa_handler == SIG_DFL));
    );
}

#endif  // defined(__linux__) || defined(__APPLE__)

#endif  // !defined(_WIN32)

#if defined(_WIN32)

//////////////////////////////////////////////////////////////////////////
// A pseudo console of a given size.
//////////////////////////////////////////////////////////////////////////

TEST(ConsolePseudoConsoleTest, rows_are_clipped_and_the_cursor_returns)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 15);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.is_terminal());
        AREG_CHILD_CHECK(console.screen_size().posX == 60 && console.screen_size().posY == 15);
        console.lock_console();
        console.output_txt({ 0, 0 }, "Title");
        console.output_str({ 0, 2 }, areg::String(std::string(100u, 'B').c_str()));
        console.output_txt({ 4, 20 }, "below");
        console.output_txt({ 0, 12 }, "\nQuit the application ...\n");
        console.set_cursor_cur_position({ 7, 5 });
        console.unlock_console();
        terminal.wait_for([&]() { return (terminal.screen.row(13) == "Quit the application ...") && (terminal.screen.cursor_x() == 7) && (terminal.screen.cursor_y() == 5); });
        AREG_CHILD_CHECK(terminal.screen.row(0) == "Title");
        AREG_CHILD_CHECK(terminal.screen.row(2) == std::string(60u, 'B'));
        AREG_CHILD_CHECK(!terminal.screen.shows("below"));
        console.clear_line({ 2, 2 });
        terminal.wait_for([&]() { return terminal.screen.row(2) == "BB"; });
        console.clear_screen();
        terminal.wait_for([&]() { return !terminal.screen.shows("Title"); });
    );
}

TEST(ConsolePseudoConsoleTest, resize_calls_the_callback_with_the_new_size)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 15);
        Console& console = Console::instance();
        std::vector<Console::Coord> sizes;
        console.set_resize_callback([&sizes](Console::Coord size) { sizes.push_back(size); });
        console.output_txt({ 0, 1 }, "same size");
        AREG_CHILD_CHECK(sizes.empty());
        terminal.resize(70, 18);
        for (int i = 0; (i < 6000) && (console.screen_size().posX != 70 || console.screen_size().posY != 18); ++i)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        AREG_CHILD_CHECK(console.screen_size().posX == 70 && console.screen_size().posY == 18);
        AREG_CHILD_CHECK(sizes.size() == 1u);
        AREG_CHILD_CHECK(sizes[0].posX == 70 && sizes[0].posY == 18);
        console.set_resize_callback(nullptr);
    );
}

TEST(ConsolePseudoConsoleTest, typed_line_is_edited_with_virtual_terminal_keys)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 3 }, "Cmd: ");
        console.set_cursor_cur_position({ 5, 3 });
        terminal.type("xabc\x1B[D\x1B[D\x7F" "X\x1B[H\x1B[3~\x1B[F!\r");
        const areg::String line{ console.wait_for_input(nullptr) };
        AREG_CHILD_CHECK(line == "Xbc!");
        terminal.wait_for([&]() { return (terminal.screen.row(3) == "Cmd: Xbc!") && (terminal.screen.cursor_x() == 5) && (terminal.screen.cursor_y() == 3); });
    );
}

TEST(ConsolePseudoConsoleTest, up_and_down_recall_the_entered_lines)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.output_txt({ 0, 3 }, "Cmd: ");
        console.set_cursor_cur_position({ 5, 3 });
        terminal.type("one\rtwo\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "one");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "two");
        terminal.type("new\x1B[A\x1B[A\x1B[A\x1B[BX\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "twoX");
        terminal.type("\x1B[A\x1B[A\x1B[B\x1B[B\r");
        terminal.type("\x1B[A\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "twoX");
        terminal.wait_for([&]() { return (terminal.screen.row(3) == "Cmd: twoX") && (terminal.screen.cursor_x() == 5) && (terminal.screen.cursor_y() == 3); });
    );
}

TEST(ConsolePseudoConsoleTest, disabled_input_interrupts_the_waiting_read)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 7, 4 });
        areg::String result{ "unset" };
        std::thread reader([&]() { result = console.wait_for_input(nullptr); });
        terminal.type("partial");
        terminal.wait_for([&]() { return terminal.screen.shows("partial"); });
        console.enable_console_input(false);
        reader.join();
        AREG_CHILD_CHECK(result.is_empty());
    );
}

namespace
{
    //!< A pressed key with the character 'ch', or the virtual key 'vk' if 'ch' is 0.
    INPUT_RECORD _key(WCHAR ch, WORD vk = 0u, WORD repeat = 1u, bool down = true)
    {
        INPUT_RECORD record{};
        record.EventType                    = KEY_EVENT;
        record.Event.KeyEvent.bKeyDown      = down ? TRUE : FALSE;
        record.Event.KeyEvent.wRepeatCount  = repeat;
        record.Event.KeyEvent.wVirtualKeyCode = vk;
        record.Event.KeyEvent.uChar.UnicodeChar = ch;
        return record;
    }

    //!< Puts the records into the input of the console.
    void _put_keys(const std::vector<INPUT_RECORD>& records)
    {
        DWORD written{ 0u };
        AREG_CHILD_CHECK(::WriteConsoleInputW(::GetStdHandle(STD_INPUT_HANDLE), records.data(), static_cast<DWORD>(records.size()), &written) != FALSE);
        AREG_CHILD_CHECK(written == records.size());
    }

    //!< Returns the number of pressed keys waiting in the input of the console.
    uint32_t _pressed_keys()
    {
        INPUT_RECORD records[64]{};
        DWORD count{ 0u };
        AREG_CHILD_CHECK(::PeekConsoleInputW(::GetStdHandle(STD_INPUT_HANDLE), records, 64u, &count) != FALSE);
        uint32_t result{ 0u };
        for (DWORD i = 0u; i < count; ++i)
        {
            if ((records[i].EventType == KEY_EVENT) && (records[i].Event.KeyEvent.bKeyDown != FALSE))
                ++result;
        }

        return result;
    }
}

// Reproduces the defect: disabling the input put an Enter key into the console, which the shell read after the exit.
TEST(ConsolePseudoConsoleTest, disabled_input_leaves_no_key_in_the_console)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 5, 3 });
        terminal.type("-q\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "-q");
        AREG_CHILD_CHECK(console.enable_console_input(false));
        AREG_CHILD_CHECK(_pressed_keys() == 0u);
        AREG_CHILD_CHECK(console.enable_console_input(true));
        terminal.type("next\r");
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "next");
        AREG_CHILD_CHECK(_pressed_keys() == 0u);
    );
}

TEST(ConsolePseudoConsoleTest, key_records_edit_the_line_and_other_events_are_ignored)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 5, 3 });
        INPUT_RECORD focus{};
        focus.EventType = FOCUS_EVENT;
        focus.Event.FocusEvent.bSetFocus = TRUE;
        _put_keys({ _key(L'x'), _key(L'a'), _key(L'b'), _key(L'c'), _key(0, VK_LEFT), _key(0, VK_LEFT), _key(L'\b'), _key(L'X')
                  , _key(0, VK_HOME), _key(0, VK_DELETE), _key(0, VK_RIGHT), _key(L'Y'), _key(0, VK_END), _key(0, VK_SHIFT)
                  , _key(static_cast<WCHAR>(0x00E9)), _key(L'z', 0u, 1u, false), focus, _key(L'!'), _key(L'!'), _key(L'!'), _key(L'\r') });
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "XYbc!!!");
        _put_keys({ _key(L'n'), _key(L'e'), _key(L'w'), _key(0, VK_UP), _key(0, VK_DOWN), _key(L'!'), _key(L'\r') });
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "new!");
        _put_keys({ _key(0, VK_UP), _key(0, VK_UP), _key(L'\r') });
        AREG_CHILD_CHECK(console.wait_for_input(nullptr) == "XYbc!!!");
    );
}

TEST(ConsolePseudoConsoleTest, escape_without_the_rest_of_a_sequence_is_dropped)
{
    if (!Terminal::supported())
        GTEST_SKIP() << "the system has no pseudo console";

    AREG_EXPECT_IN_CHILD(
        Terminal terminal(60, 10);
        Console& console = Console::instance();
        AREG_CHILD_CHECK(console.enable_console_input(true));
        console.set_cursor_cur_position({ 0, 5 });
        for (const std::vector<INPUT_RECORD>& escape : { std::vector<INPUT_RECORD>{ _key(0x1B) }, std::vector<INPUT_RECORD>{ _key(0x1B), _key(L'[') } })
        {
            areg::String result;
            std::thread reader([&]() { result = console.wait_for_input(nullptr); });
            _put_keys({ _key(L'a') });
            terminal.wait_for([&]() { return terminal.screen.row(5) == "a"; });
            _put_keys(escape);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            _put_keys({ _key(L'A'), _key(0x15), _key(L'x'), _key(L'\r') });
            reader.join();
            AREG_CHILD_CHECK(result == "x");
        }
    );
}

#endif  // defined(_WIN32)
