#ifndef AREG_TESTS_UNITS_SERVICECONSOLEHARNESS_HPP
#define AREG_TESTS_UNITS_SERVICECONSOLEHARNESS_HPP

/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

/************************************************************************
 * Runs a system service application with its console in a child process, and connects raw
 * clients to it. The message router and the log collector share it: ServiceUnderTest names what
 * differs between them.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "units/VtScreen.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "aregextend/console/Console.hpp"
#include "aregextend/service/ServiceApplicationBase.hpp"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#if defined(_WIN32)
    #include <io.h>
    #include <fcntl.h>
    #include <WinSock2.h>
#else
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

namespace service_harness
{
    using areg::ext::ServiceOption;
    using areg::ext::ServicePhase;
    constexpr auto DEADLINE = std::chrono::seconds(60);

    //!< What differs between two services under test.
    struct ServiceUnderTest
    {
        areg::ext::ServiceApplicationBase& (*application)();    //!< Returns the application singleton.
        const char* section;                                    //!< The section of the service in the configuration file.
        ITEM_ID     channel;                                    //!< The cookie of the service.
        const char* logScope;                                   //!< The log scopes the private configuration enables.
        const char* optionLog;                                  //!< The start of the log line of a handled option.
    };

    //!< Returns the dispatcher thread of the running service.
    inline areg::DispatcherThread& service_dispatcher()
    {
        return areg::DispatcherThread::dispatcher_thread(areg::ThreadAddress("SERVER_DISPATCH_MESSAGE_THREAD"));
    }

    //!< The size of the terminal a console session draws on.
    struct TerminalSize
    {
        int columns{ 120 };
        int rows{ 40 };
    };

    //!< Opens a terminal of the given size: fds[0] reads what fds[1] draws. A pipe on Windows.
    void make_terminal(int (&fds)[2], TerminalSize size = TerminalSize{});

    //!< Changes the size of the terminal the standard output draws on. Does nothing on Windows, where it is a pipe.
    void resize_terminal(TerminalSize size);

    //!< Prints the stack of every thread of this process on macOS. Does nothing elsewhere.
    void report_threads();

    inline void check_console_verbose(const ServiceUnderTest& service, bool expected)
    {
        auto& console = areg::ext::Console::instance();
        console.lock_console();
        const bool actual{ service.application().data_rate_helper().is_verbose() };
        console.unlock_console();
        AREG_CHILD_CHECK(actual == expected);
    }

    //!< Sets a terminal type that draws at coordinates; the size comes from make_terminal().
    inline void set_terminal()
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_putenv_s("TERM", "xterm") == 0);
#else
        AREG_CHILD_CHECK(::setenv("TERM", "xterm", 1) == 0);
#endif
    }

    inline int descriptor(FILE* stream)
    {
#if defined(_WIN32)
        return ::_fileno(stream);
#else
        return ::fileno(stream);
#endif
    }

    inline int duplicate(int fd)
    {
#if defined(_WIN32)
        return ::_dup(fd);
#else
        return ::dup(fd);
#endif
    }

    inline void redirect(int source, int target)
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_dup2(source, target) == 0);
        if (target == descriptor(stdout) || target == descriptor(stdin))
            AREG_CHILD_CHECK(::SetStdHandle(target == descriptor(stdout) ? STD_OUTPUT_HANDLE : STD_INPUT_HANDLE,
                reinterpret_cast<HANDLE>(::_get_osfhandle(target))) != FALSE);
#else
        AREG_CHILD_CHECK(::dup2(source, target) == target);
#endif
    }

    inline void close_descriptor(int fd)
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_close(fd) == 0);
#else
        AREG_CHILD_CHECK(::close(fd) == 0);
#endif
    }

    inline void make_pipe(int (&fds)[2])
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_pipe(fds, 65536, _O_BINARY) == 0);
#else
        AREG_CHILD_CHECK(::pipe(fds) == 0);
#endif
    }

    inline int read_descriptor(int fd, char* buffer, uint32_t size)
    {
#if defined(_WIN32)
        return ::_read(fd, buffer, size);
#else
        return static_cast<int>(::read(fd, buffer, size));
#endif
    }

    inline void write_descriptor(int fd, const std::string& text)
    {
        size_t done{ 0u };
        while (done != text.size())
        {
#if defined(_WIN32)
            const int count{ ::_write(fd, text.data() + done, static_cast<uint32_t>(text.size() - done)) };
#else
            const int count{ static_cast<int>(::write(fd, text.data() + done, text.size() - done)) };
#endif
            AREG_CHILD_CHECK(count > 0);
            done += static_cast<size_t>(count);
        }
    }

    // Reconstructs the cursor-addressed rows rather than searching escape bytes.
    class TerminalCapture
    {
    public:
        std::string transcript;

        void feed(const char* data, size_t size)
        {
            for (size_t i = 0u; i < size; ++i)
            {
                const unsigned char ch{ static_cast<unsigned char>(data[i]) };
                if (!mEscape.empty())
                {
                    mEscape += static_cast<char>(ch);
                    if (mEscape.size() == 2u && ch == '[')
                        continue;
                    if (mEscape.size() == 2u && (ch == '(' || ch == ')'))
                        continue;
                    if (mEscape[1] == '[' && (ch < 0x40 || ch > 0x7e))
                        continue;
                    publish();
                    if (mEscape[1] == '[')
                        control(static_cast<char>(ch));
                    else if (ch == 'M')
                        mRow = (std::max)(0, mRow - 1);
                    else if (ch == 'D')
                        newline();
                    else if (ch == 'E')
                    {
                        newline();
                        mColumn = 0;
                    }
                    mEscape.clear();
                }
                else if (ch == 0x1b)
                    mEscape = "\x1b";
                else if (ch == '\r')
                {
                    publish();
                    mColumn = 0;
                }
                else if (ch == '\n')
                {
                    newline();
#if defined(_WIN32)
                    mColumn = 0; // A pipe has no terminal driver to add the carriage return.
#endif
                }
                else if (ch == '\b')
                    mColumn = (std::max)(0, mColumn - 1);
                else if (ch == '\t')
                    mColumn = (std::min)(119, (mColumn / 8 + 1) * 8);
                else if (ch >= ' ')
                    put(static_cast<char>(ch));
            }
            publish();
        }

        //!< Returns true if the token is on the screen now.
        bool shows(const std::string& token) const
        {
            return std::any_of(mRows.begin(), mRows.end(), [&](const std::string& row) { return row.find(token) != std::string::npos; });
        }

    private:
        void publish()
        {
            for (size_t row = 0u; row < mRows.size(); ++row)
            {
                if (mWritten[row] || (mRows[row] != mPublished[row]))
                {
                    transcript += mRows[row] + '\n';
                    mPublished[row] = mRows[row];
                    mWritten[row] = false;
                }
            }
        }

        void put(char ch)
        {
            mRows[mRow][(std::min)(mColumn, 119)] = ch;
            mWritten[mRow] = true;
            mColumn = (std::min)(120, mColumn + 1);
            mLast = ch;
        }

        void newline()
        {
            publish();
            if (++mRow == 40)
            {
                mRows.erase(mRows.begin());
                mRows.emplace_back(120u, ' ');
                mPublished.erase(mPublished.begin());
                mPublished.emplace_back();
                mWritten.erase(mWritten.begin());
                mWritten.push_back(false);
                mRow = 39;
            }
        }

        void control(char final)
        {
            if (mEscape[2] == '?')
                return; // Mode, cursor-visibility and alternate-screen changes carry no text.
            std::vector<int> values(1u, 0);
            for (size_t i = 2u; i + 1u < mEscape.size(); ++i)
            {
                if (mEscape[i] == ';')
                    values.push_back(0);
                else if (mEscape[i] >= '0' && mEscape[i] <= '9')
                    values.back() = values.back() * 10 + mEscape[i] - '0';
            }
            const int first{ values[0] };
            const int count{ (std::max)(1, first) };
            switch (final)
            {
            case 'H': case 'f':
                mRow = std::clamp(count - 1, 0, 39);
                mColumn = std::clamp(values.size() > 1u ? (std::max)(1, values[1]) - 1 : 0, 0, 119);
                break;
            case 'A': mRow = (std::max)(0, mRow - count); break;
            case 'B': mRow = (std::min)(39, mRow + count); break;
            case 'C': mColumn = (std::min)(119, mColumn + count); break;
            case 'D': mColumn = (std::max)(0, mColumn - count); break;
            case 'd': mRow = std::clamp(count - 1, 0, 39); break;
            case 'G': case '`': mColumn = std::clamp(count - 1, 0, 119); break;
            case 'K':
                mRows[mRow].replace(first == 0 ? (std::min)(mColumn, 120) : 0,
                    first == 0 ? 120 - (std::min)(mColumn, 120) : first == 1 ? (std::min)(mColumn + 1, 120) : 120,
                    first == 0 ? 120 - (std::min)(mColumn, 120) : first == 1 ? (std::min)(mColumn + 1, 120) : 120, ' ');
                break;
            case 'J':
                for (int row = 0; row < 40; ++row)
                {
                    if (first >= 2 || (first == 0 && row > mRow) || (first == 1 && row < mRow))
                        mRows[row].assign(120u, ' ');
                    else if (row == mRow)
                    {
                        const size_t start{ first == 1 ? 0u : static_cast<size_t>((std::min)(mColumn, 120)) };
                        const size_t size{ first == 1 ? static_cast<size_t>((std::min)(mColumn + 1, 120)) : 120u - start };
                        mRows[row].replace(start, size, size, ' ');
                    }
                }
                break;
            case 'b': for (int i = 0; i < count; ++i) put(mLast); break;
            case 'P':
                mRows[mRow].erase(static_cast<size_t>((std::min)(mColumn, 120)), static_cast<size_t>(count));
                mRows[mRow].resize(120u, ' ');
                break;
            case '@':
                mRows[mRow].insert(static_cast<size_t>((std::min)(mColumn, 120)), static_cast<size_t>(count), ' ');
                mRows[mRow].resize(120u);
                break;
            case 'X':
                mRows[mRow].replace(static_cast<size_t>((std::min)(mColumn, 120)),
                    static_cast<size_t>((std::min)(count, 120 - mColumn)),
                    static_cast<size_t>((std::min)(count, 120 - mColumn)), ' ');
                break;
            default: break; // Colours, attributes and scroll-region setup do not change these rows.
            }
        }

        std::vector<std::string> mRows{ 40u, std::string(120u, ' ') };
        std::vector<std::string> mPublished{ 40u, "" };
        std::vector<bool> mWritten = std::vector<bool>(40u, false);
        std::string mEscape;
        int mRow{ 0 };
        int mColumn{ 0 };
        char mLast{ ' ' };
    };

    struct PrivateConfig
    {
        const ServiceUnderTest& service;
        std::string directory;
        std::string path;
        std::string log;
        std::string contents;
        uint16_t port;
        bool logging;

        explicit PrivateConfig(const ServiceUnderTest& target, bool enableLogging = false, uint32_t pairs = 0u)
            : service(target)
            , directory(testing::TempDir() + target.section + "-app-" + std::to_string(areg::Process::instance().id())
                + "-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()))
            , path((std::filesystem::path(directory) / "private.init").string())
            , log((std::filesystem::path(directory) / "router.log").string())
            , contents()
            , port(0u)
            , logging(enableLogging)
        {
            AREG_CHILD_CHECK(std::filesystem::create_directory(directory));
            AREG_CHILD_CHECK(areg::socket_initialize());
            const SOCKETHANDLE socket{ areg::socket_create() };
            AREG_CHILD_CHECK(areg::is_valid_socket(socket));
            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            AREG_CHILD_CHECK(::bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == 0);
#if defined(_WIN32)
            int size{ sizeof(address) };
#else
            socklen_t size{ sizeof(address) };
#endif
            AREG_CHILD_CHECK(::getsockname(socket, reinterpret_cast<sockaddr*>(&address), &size) == 0);
            port = ntohs(address.sin_port);
            AREG_CHILD_CHECK(port != 0u);
            areg::socket_close(socket);
            const std::string section{ target.section };
            contents = std::string("log::*::enable = ") + (logging ? "true\n" : "false\n")
                + "log::*::target = file\nlog::*::enable::remote = false\n"
                + "log::*::enable::file = " + (logging ? "true\n" : "false\n")
                + "log::*::enable::debug = false\nlog::*::enable::db = false\n"
                + "log::*::file::append = false\nlog::*::file::location = " + log
                + "\nlog::*::scope::* = NOTSET\nlog::*::scope::" + target.logScope + ".* = DEBUG\n"
                "log::*::layout::message = %m%n\n"
                + section + "::*::connect = tcpip\n" + section + "::*::enable::tcpip = true\n"
                + section + "::*::address::tcpip = 127.0.0.1\n" + section + "::*::port::tcpip = "
                + std::to_string(port) + "\nnet::*::tcpip::pairs = " + std::to_string(pairs) + "\n";
            std::ofstream output(path);
            output << contents;
            AREG_CHILD_CHECK(output.good());
        }

        ~PrivateConfig()
        {
            AREG_CHILD_CHECK(std::remove(path.c_str()) == 0);
            if (std::filesystem::exists(log))
                AREG_CHILD_CHECK(std::remove(log.c_str()) == 0);
            AREG_CHILD_CHECK(std::filesystem::remove(directory));
            areg::socket_release();
        }

        void check_unchanged() const
        {
            std::ifstream input(path);
            const std::string actual{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
            AREG_CHILD_CHECK(actual == contents);
        }

        void check_options_logged(const std::vector<std::string>& names) const
        {
#if AREG_LOGGING
            AREG_CHILD_CHECK(logging);
            std::ifstream input(log);
            AREG_CHILD_CHECK(input.good());
            const std::string actual{ std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>() };
            for (const auto& name : names)
                AREG_CHILD_CHECK(actual.find(service.optionLog + name + " ], going to process...") != std::string::npos);
#else
            (void)names;
#endif
        }
    };

    inline bool parse(const ServiceUnderTest& service, const std::vector<std::string>& options)
    {
        std::vector<std::string> storage{ "service-app-test" };
        storage.insert(storage.end(), options.begin(), options.end());
        std::vector<char*> argv;
        for (std::string& value : storage)
            argv.push_back(value.data());
        return service.application().parse_options(static_cast<int32_t>(argv.size()), argv.data());
    }

    inline void set_io_deadline(SOCKETHANDLE socket)
    {
#if defined(_WIN32)
        const DWORD timeout{ 60'000u };
#else
        const timeval timeout{ 60, 0 };
#endif
        AREG_CHILD_CHECK(areg::set_send_timeout(socket, 60'000u));
        AREG_CHILD_CHECK(::setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<const char*>(&timeout), sizeof(timeout)) == 0);
    }

    struct ConnectedPeer
    {
        areg::SocketClient socket;
        areg::EventHeader reply{};
        uint32_t sentBytes{ 0u };

        ConnectedPeer(const ServiceUnderTest& service, uint16_t port)
            : socket("127.0.0.1", port)
        {
            AREG_CHILD_CHECK(socket.create());
            set_io_deadline(socket.handle());
            const auto hello{ areg::create_connect_request(areg::COOKIE_UNKNOWN, service.channel,
                areg::MessageSource::SourceClient) };
            sentBytes = areg::wire_size(*hello.header());
            AREG_CHILD_CHECK(socket.send(areg::wire_data(*hello.header()), static_cast<int32_t>(sentBytes))
                == static_cast<int32_t>(sentBytes));
            AREG_CHILD_CHECK(socket.receive(areg::wire_header(reply), areg::WIRE_HEADER_SIZE) == areg::WIRE_HEADER_SIZE);
            AREG_CHILD_CHECK(reply.messageId == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection));
            AREG_CHILD_CHECK(reply.target >= areg::COOKIE_REMOTE_SERVICE && reply.source == service.channel);
            AREG_CHILD_CHECK(reply.bufHeader.biUsed <= 1024u);
            std::vector<uint8_t> payload(reply.bufHeader.biUsed);
            AREG_CHILD_CHECK(!payload.empty());
            AREG_CHILD_CHECK(socket.receive(payload.data(), static_cast<int32_t>(payload.size())) == static_cast<int32_t>(payload.size()));
        }

        uint32_t received_bytes() const
        {
            return areg::wire_size(reply);
        }
    };

    inline std::string rate_line(std::string_view format, uint64_t bytes)
    {
        const auto rate{ areg::ext::DataRateHelper::convert_data_rate_literals(bytes) };
        char output[160];
        const int length{ std::snprintf(output, sizeof(output), format.data(), static_cast<double>(rate.first), rate.second.c_str()) };
        AREG_CHILD_CHECK(length > 0 && static_cast<size_t>(length) < sizeof(output));
        return output;
    }

    inline std::string instance_line(uint32_t number, ITEM_ID cookie, const areg::ConnectedInstance& instance)
    {
        char output[512];
        const int length{ std::snprintf(output, sizeof(output), " %4u. |  %11u  |    %u     |  %s ",
            number, static_cast<uint32_t>(cookie), static_cast<uint32_t>(instance.ciBitness), instance.ciInstance.c_str()) };
        AREG_CHILD_CHECK(length > 0 && static_cast<size_t>(length) < sizeof(output));
        return output;
    }

    class DispatcherFence final : public areg::EventConsumer
    {
    public:
        explicit DispatcherFence(const ServiceUnderTest& target)
            : service(target)
        {
        }

        const ServiceUnderTest& service;
        std::promise<bool> reached;

        void start_event_processing(areg::Event&) override
        {
            reached.set_value(service.application().communication_controller().is_host_connected());
        }
    };

    inline void check_listening(const ServiceUnderTest& service)
    {
        DispatcherFence fence(service);
        auto ready = fence.reached.get_future();
        areg::Event event(areg::EventType::EventCustomExternal);
        event.set_event_consumer(&fence);
        auto& dispatcher = service_dispatcher();
        AREG_CHILD_CHECK(static_cast<areg::EventDispatcher&>(dispatcher).post_event(event));
        AREG_CHILD_CHECK(ready.wait_for(DEADLINE) == std::future_status::ready);
        AREG_CHILD_CHECK(ready.get());
    }

    inline void check_read_end(int count)
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(count == 0);
#else
        AREG_CHILD_CHECK(count == 0 || errno == EIO);
#endif
    }

    class ConsoleSession
    {
    public:
        std::string output;

        explicit ConsoleSession(const PrivateConfig& config, TerminalSize size = TerminalSize{})
            : output()
            , mService(config.service)
            , mInput{-1, -1}
            , mOutput{-1, -1}
            , mSavedInput(duplicate(descriptor(stdin)))
            , mSavedOutput(duplicate(descriptor(stdout)))
            , mTerminal()
            , mScreen(size.columns, size.rows)
            , mMutex()
            , mChanged()
            , mEnded(false)
            , mReader()
            , mServiceThread()
            , mResult()
        {
            AREG_CHILD_CHECK(mSavedInput >= 0 && mSavedOutput >= 0);
            set_terminal();
            make_pipe(mInput);
            make_terminal(mOutput, size);
            std::fflush(stdout);
            redirect(mInput[0], descriptor(stdin));
            redirect(mOutput[1], descriptor(stdout));
            close_descriptor(mInput[0]);
            close_descriptor(mOutput[1]);
            mReader = std::thread([this]()
            {
                char buffer[4096];
                int count;
                while ((count = read_descriptor(mOutput[0], buffer, sizeof(buffer))) > 0)
                {
                    std::lock_guard<std::mutex> lock(mMutex);
                    mTerminal.feed(buffer, static_cast<size_t>(count));
                    mScreen.feed(std::string_view(buffer, static_cast<size_t>(count)));
                    output = mTerminal.transcript;
                    mChanged.notify_all();
                }
                check_read_end(count);
                std::lock_guard<std::mutex> lock(mMutex);
                mEnded = true;
                mChanged.notify_all();
            });
            AREG_CHILD_CHECK(parse(mService, {"--console", "--load=" + config.path}));
            auto done = mResult.get_future();
            mDone = std::move(done);
            mServiceThread = std::thread([this]()
            {
                mResult.set_value(mService.application().service_main(ServiceOption::CMD_Console, nullptr));
            });
            expect("to quit the application");
            check_listening(mService);
        }

        ~ConsoleSession()
        {
            AREG_CHILD_CHECK(!mServiceThread.joinable() && !mReader.joinable());
        }

        void silent_command()
        {
            command("--silent", "Switching to silent mode, no data rate output");
        }

        void verbose_command()
        {
            check_console_verbose(mService, false);
            command("--verbose", "Switching to verbose mode to output data rate");
        }

        void expect(const std::string& token, size_t offset = 0u)
        {
            expect_any({token}, offset);
        }

        void expect_any(const std::vector<std::string>& tokens, size_t offset)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            const auto matched = [&]()
            {
                return std::any_of(tokens.begin(), tokens.end(), [&](const std::string& token)
                    { return output.find(token, offset) != std::string::npos; });
            };
            const bool settled{ mChanged.wait_for(lock, DEADLINE, [&]() { return matched() || mEnded; }) };
            if (!settled || !matched())
                report_missing(tokens, offset, settled);
            AREG_CHILD_CHECK(settled);
            AREG_CHILD_CHECK(matched());
        }

        //!< Waits until the token is no longer on the screen.
        void expect_gone(const std::string& token)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            AREG_CHILD_CHECK(mChanged.wait_for(lock, DEADLINE, [&]() { return !mTerminal.shows(token) || mEnded; }));
            AREG_CHILD_CHECK(!mTerminal.shows(token));
        }

        //!< Waits until the predicate holds for the screen the terminal of the session shows.
        template<typename Predicate>
        bool wait_screen(Predicate predicate)
        {
            std::unique_lock<std::mutex> lock(mMutex);
            return mChanged.wait_for(lock, DEADLINE, [&]() { return predicate(static_cast<const VtScreen&>(mScreen)) || mEnded; })
                && predicate(static_cast<const VtScreen&>(mScreen));
        }

        size_t mark()
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return output.size();
        }

        bool shown_since(const std::string& token, size_t offset)
        {
            std::lock_guard<std::mutex> lock(mMutex);
            return output.find(token, offset) != std::string::npos;
        }

        size_t command(const std::string& line, const std::string& token)
        {
            const size_t offset{ mark() };
            write_descriptor(mInput[1], line + "\n");
            expect(token, offset);
            return offset;
        }

        void quit()
        {
            command("-q", "Quit the application");
            AREG_CHILD_CHECK(mDone.wait_for(DEADLINE) == std::future_status::ready);
            AREG_CHILD_CHECK(mDone.get() == 0);
            mServiceThread.join();
            std::fflush(stdout);
            redirect(mSavedOutput, descriptor(stdout));
            redirect(mSavedInput, descriptor(stdin));
            close_descriptor(mSavedOutput);
            close_descriptor(mSavedInput);
            close_descriptor(mInput[1]);
            mReader.join();
            close_descriptor(mOutput[0]);
            AREG_CHILD_CHECK(mService.application().state() == ServicePhase::Stopped);
            AREG_CHILD_CHECK(!service_dispatcher().is_running());
        }

    private:
        // Prints what a failed wait looked for and what the screen showed. The caller holds mMutex.
        void report_missing(const std::vector<std::string>& tokens, size_t offset, bool settled) const
        {
            std::fprintf(stderr, "console wait failed, %s, offset %zu of %zu\n",
                settled ? "the output ended" : "the deadline passed", offset, output.size());
            for (const std::string& token : tokens)
                std::fprintf(stderr, "expected: [%s]\n", token.c_str());
            std::fprintf(stderr, "transcript:\n%s\n", output.c_str());
            report_threads();
        }

        const ServiceUnderTest& mService;
        int mInput[2];
        int mOutput[2];
        int mSavedInput;
        int mSavedOutput;
        TerminalCapture mTerminal;
        VtScreen mScreen;
        std::mutex mMutex;
        std::condition_variable mChanged;
        bool mEnded;
        std::thread mReader;
        std::thread mServiceThread;
        std::promise<int32_t> mResult;
        std::future<int32_t> mDone;
    };
}

#endif  // AREG_TESTS_UNITS_SERVICECONSOLEHARNESS_HPP
