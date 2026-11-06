/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 * Distributed under the licenses in LICENSE.txt.
 ************************************************************************/

// Declares the pseudo terminal functions: g++ on Cygwin does not predefine it, unlike on Linux.
#if defined(__CYGWIN__) && !defined(_GNU_SOURCE)
    #define _GNU_SOURCE
#endif

#include "units/GUnitTest.hpp"
#include "units/ChildProcess.hpp"
#include "mtrouter/app/MultitargetRouter.hpp"
#include "areg/appbase/AppDefs.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SocketClient.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/DataRateStats.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "aregextend/console/Console.hpp"

#include <chrono>
#include <algorithm>
#include <cerrno>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <filesystem>
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
    #include <sys/ioctl.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

namespace
{
    using areg::ext::ServiceOption;
    using areg::ext::ServicePhase;
    constexpr auto DEADLINE = std::chrono::seconds(60);
    constexpr const char* SHORT_OPTIONS[]{"-c", "-h", "-i", "-l", "-n", "-p", "-q", "-r", "-s", "-t", "-u", "-v", "-w"};
    constexpr const char* LONG_OPTIONS[]{"--console", "--help", "--install", "--load", "--instances", "--pause", "--quit",
        "--restart", "--service", "--silent", "--uninstall", "--verbose", "--threads"};

    areg::ext::ServiceApplicationBase& application()
    {
        return MultitargetRouter::instance();
    }

    areg::DispatcherThread& router_dispatcher()
    {
        return areg::DispatcherThread::dispatcher_thread(areg::ThreadAddress("SERVER_DISPATCH_MESSAGE_THREAD"));
    }

    void check_console_verbose(bool expected)
    {
        auto& console = areg::ext::Console::instance();
        console.lock_console();
        const bool actual{ application().data_rate_helper().is_verbose() };
        console.unlock_console();
        AREG_CHILD_CHECK(actual == expected);
    }

    void set_terminal()
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_putenv_s("TERM", "xterm") == 0);
#else
        AREG_CHILD_CHECK(::setenv("TERM", "xterm", 1) == 0);
        AREG_CHILD_CHECK(::setenv("LINES", "40", 1) == 0);
        AREG_CHILD_CHECK(::setenv("COLUMNS", "120", 1) == 0);
#endif
    }

    int descriptor(FILE* stream)
    {
#if defined(_WIN32)
        return ::_fileno(stream);
#else
        return ::fileno(stream);
#endif
    }

    int duplicate(int fd)
    {
#if defined(_WIN32)
        return ::_dup(fd);
#else
        return ::dup(fd);
#endif
    }

    void redirect(int source, int target)
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

    void close_descriptor(int fd)
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_close(fd) == 0);
#else
        AREG_CHILD_CHECK(::close(fd) == 0);
#endif
    }

    void make_pipe(int (&fds)[2])
    {
#if defined(_WIN32)
        AREG_CHILD_CHECK(::_pipe(fds, 65536, _O_BINARY) == 0);
#else
        AREG_CHILD_CHECK(::pipe(fds) == 0);
#endif
    }

    int read_descriptor(int fd, char* buffer, uint32_t size)
    {
#if defined(_WIN32)
        return ::_read(fd, buffer, size);
#else
        return static_cast<int>(::read(fd, buffer, size));
#endif
    }

    void write_descriptor(int fd, const std::string& text)
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

    void make_terminal(int (&fds)[2])
    {
#if defined(_WIN32)
        make_pipe(fds);
#else
        fds[0] = ::posix_openpt(O_RDWR | O_NOCTTY);
        AREG_CHILD_CHECK(fds[0] >= 0);
        AREG_CHILD_CHECK(::grantpt(fds[0]) == 0 && ::unlockpt(fds[0]) == 0);
        const char* name{ ::ptsname(fds[0]) };
        AREG_CHILD_CHECK(name != nullptr);
        fds[1] = ::open(name, O_RDWR | O_NOCTTY);
        AREG_CHILD_CHECK(fds[1] >= 0);
        winsize size{};
        size.ws_row = 40;
        size.ws_col = 120;
        AREG_CHILD_CHECK(::ioctl(fds[1], TIOCSWINSZ, &size) == 0);
#endif
    }

    // Reconstruct ncurses' cursor-addressed differences rather than searching escape bytes.
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
                    else if (ch == 'D' || ch == 'E')
                        newline();
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
                    newline();
                else if (ch == '\b')
                    mColumn = (std::max)(0, mColumn - 1);
                else if (ch == '\t')
                    mColumn = (std::min)(119, (mColumn / 8 + 1) * 8);
                else if (ch >= ' ')
                    put(static_cast<char>(ch));
            }
            publish();
        }

    private:
        void publish()
        {
            for (size_t row = 0u; row < mRows.size(); ++row)
            {
                if (mRows[row] != mPublished[row])
                {
                    transcript += mRows[row] + '\n';
                    mPublished[row] = mRows[row];
                }
            }
        }

        void put(char ch)
        {
            mRows[mRow][(std::min)(mColumn, 119)] = ch;
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
                mRow = 39;
            }
            mColumn = 0;
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
        std::string mEscape;
        int mRow{ 0 };
        int mColumn{ 0 };
        char mLast{ ' ' };
    };

    struct PrivateConfig
    {
        std::string directory;
        std::string path;
        std::string log;
        std::string contents;
        uint16_t port;
        bool logging;

        explicit PrivateConfig(bool enableLogging = false, uint32_t pairs = 0u)
            : directory(testing::TempDir() + "router-app-" + std::to_string(areg::Process::instance().id())
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
            contents = std::string("log::*::enable = ") + (logging ? "true\n" : "false\n")
                + "log::*::target = file\nlog::*::enable::remote = false\n"
                + "log::*::enable::file = " + (logging ? "true\n" : "false\n")
                + "log::*::enable::debug = false\nlog::*::enable::db = false\n"
                + "log::*::file::append = false\nlog::*::file::location = " + log
                + "\nlog::*::scope::* = NOTSET\nlog::*::scope::mtrouter_app_MultitargetRouter.* = DEBUG\n"
                "log::*::layout::message = %m%n\n"
                "router::*::connect = tcpip\nrouter::*::enable::tcpip = true\n"
                "router::*::address::tcpip = 127.0.0.1\nrouter::*::port::tcpip = "
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
                AREG_CHILD_CHECK(actual.find("Found option [ MultitargetRouter::RouterOption::CMD_Router" + name
                    + " ], going to process...") != std::string::npos);
#else
            (void)names;
#endif
        }
    };

    bool parse(const std::vector<std::string>& options)
    {
        std::vector<std::string> storage{ "router-app-test" };
        storage.insert(storage.end(), options.begin(), options.end());
        std::vector<char*> argv;
        for (std::string& value : storage)
            argv.push_back(value.data());
        return application().parse_options(static_cast<int32_t>(argv.size()), argv.data());
    }

    void set_io_deadline(SOCKETHANDLE socket)
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

        explicit ConnectedPeer(uint16_t port)
            : socket("127.0.0.1", port)
        {
            AREG_CHILD_CHECK(socket.create());
            set_io_deadline(socket.handle());
            const auto hello{ areg::create_connect_request(areg::COOKIE_UNKNOWN, areg::COOKIE_ROUTER,
                areg::MessageSource::SourceClient) };
            sentBytes = areg::wire_size(*hello.header());
            AREG_CHILD_CHECK(socket.send(areg::wire_data(*hello.header()), static_cast<int32_t>(sentBytes))
                == static_cast<int32_t>(sentBytes));
            AREG_CHILD_CHECK(socket.receive(areg::wire_header(reply), areg::WIRE_HEADER_SIZE) == areg::WIRE_HEADER_SIZE);
            AREG_CHILD_CHECK(reply.messageId == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyConnection));
            AREG_CHILD_CHECK(reply.target >= areg::COOKIE_REMOTE_SERVICE && reply.source == areg::COOKIE_ROUTER);
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

    std::string rate_line(std::string_view format, uint64_t bytes)
    {
        const auto rate{ areg::ext::DataRateHelper::convert_data_rate_literals(bytes) };
        char output[160];
        const int length{ std::snprintf(output, sizeof(output), format.data(), static_cast<double>(rate.first), rate.second.c_str()) };
        AREG_CHILD_CHECK(length > 0 && static_cast<size_t>(length) < sizeof(output));
        return output;
    }

    std::string instance_line(uint32_t number, ITEM_ID cookie, const areg::ConnectedInstance& instance)
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
        std::promise<bool> reached;

        void start_event_processing(areg::Event&) override
        {
            reached.set_value(application().communication_controller().is_host_connected());
        }
    };

    void check_listening()
    {
        DispatcherFence fence;
        auto ready = fence.reached.get_future();
        areg::Event event(areg::EventType::EventCustomExternal);
        event.set_event_consumer(&fence);
        auto& dispatcher = router_dispatcher();
        AREG_CHILD_CHECK(static_cast<areg::EventDispatcher&>(dispatcher).post_event(event));
        AREG_CHILD_CHECK(ready.wait_for(DEADLINE) == std::future_status::ready);
        AREG_CHILD_CHECK(ready.get());
    }

    void send_to_router(ConnectedPeer& peer, areg::MessageEnvelope&& message)
    {
        AREG_CHILD_CHECK(message.is_valid());
        const areg::EventHeader* header{ message.header() };
        const int32_t size{ static_cast<int32_t>(areg::wire_size(*header)) };
        AREG_CHILD_CHECK(peer.socket.send(areg::wire_data(*header), size) == size);
    }

    void expect_registration(ConnectedPeer& peer, areg::RegistrationAction action, areg::DisconnectReason reason)
    {
        areg::EventHeader received{};
        AREG_CHILD_CHECK(peer.socket.receive(areg::wire_header(received), areg::WIRE_HEADER_SIZE) == areg::WIRE_HEADER_SIZE);
        AREG_CHILD_CHECK(received.messageId == static_cast<uint32_t>(areg::FuncIdRange::SystemServiceNotifyRegister));
        AREG_CHILD_CHECK(received.callType == static_cast<uint8_t>(action));
        AREG_CHILD_CHECK(received.result == static_cast<uint32_t>(reason));
        AREG_CHILD_CHECK(received.bufHeader.biUsed == 0u);
    }

    void check_read_end(int count)
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

        explicit ConsoleSession(const PrivateConfig& config)
            : output()
            , mInput{-1, -1}
            , mOutput{-1, -1}
            , mSavedInput(duplicate(descriptor(stdin)))
            , mSavedOutput(duplicate(descriptor(stdout)))
            , mMutex()
            , mChanged()
            , mEnded(false)
            , mReader()
            , mService()
            , mResult()
        {
            AREG_CHILD_CHECK(mSavedInput >= 0 && mSavedOutput >= 0);
            set_terminal();
            make_pipe(mInput);
            make_terminal(mOutput);
            std::fflush(stdout);
            redirect(mInput[0], descriptor(stdin));
            redirect(mOutput[1], descriptor(stdout));
            close_descriptor(mInput[0]);
            close_descriptor(mOutput[1]);
            mReader = std::thread([this]()
            {
                TerminalCapture terminal;
                char buffer[4096];
                int count;
                while ((count = read_descriptor(mOutput[0], buffer, sizeof(buffer))) > 0)
                {
                    std::lock_guard<std::mutex> lock(mMutex);
                    terminal.feed(buffer, static_cast<size_t>(count));
                    output = terminal.transcript;
                    mChanged.notify_all();
                }
                check_read_end(count);
                std::lock_guard<std::mutex> lock(mMutex);
                mEnded = true;
                mChanged.notify_all();
            });
            AREG_CHILD_CHECK(parse({"--console", "--load=" + config.path}));
            auto done = mResult.get_future();
            mDone = std::move(done);
            mService = std::thread([this]()
            {
                mResult.set_value(application().service_main(ServiceOption::CMD_Console, nullptr));
            });
            expect("to quit the application");
            check_listening();
        }

        ~ConsoleSession()
        {
            AREG_CHILD_CHECK(!mService.joinable() && !mReader.joinable());
        }

        void silent_command()
        {
#if AREG_EXTENDED
            command("--silent", "Switching to silent mode, no data rate output");
#else
            command("--silent", "Data rate display is always active in this mode.");
#endif
        }

        void verbose_command()
        {
#if AREG_EXTENDED
            check_console_verbose(false);
            command("--verbose", "Switching to verbose mode to output data rate");
#else
            command("--verbose", "Data rate display is always active in this mode.");
#endif
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
            AREG_CHILD_CHECK(mChanged.wait_for(lock, DEADLINE, [&]()
            {
                return matched() || mEnded;
            }));
            AREG_CHILD_CHECK(matched());
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
            mService.join();
            std::fflush(stdout);
            redirect(mSavedOutput, descriptor(stdout));
            redirect(mSavedInput, descriptor(stdin));
            close_descriptor(mSavedOutput);
            close_descriptor(mSavedInput);
            close_descriptor(mInput[1]);
            mReader.join();
            close_descriptor(mOutput[0]);
            AREG_CHILD_CHECK(application().state() == ServicePhase::Stopped);
            AREG_CHILD_CHECK(!router_dispatcher().is_running());
        }

    private:
        int mInput[2];
        int mOutput[2];
        int mSavedInput;
        int mSavedOutput;
        std::mutex mMutex;
        std::condition_variable mChanged;
        bool mEnded;
        std::thread mReader;
        std::thread mService;
        std::promise<int32_t> mResult;
        std::future<int32_t> mDone;
    };
}

TEST(RouterApplicationTest, metadata_is_available_through_public_base_interface)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(&app == &application());
        AREG_CHILD_CHECK(std::string(app.service_name_a()) == areg::ROUTER_SERVICE_NAME_ASCII);
        AREG_CHILD_CHECK(std::wstring(app.service_name_w()) == areg::ROUTER_SERVICE_NAME_WIDE);
        AREG_CHILD_CHECK(std::string(app.service_display_name_a()) == "Areg Multitarget Router Service");
        AREG_CHILD_CHECK(std::wstring(app.service_display_name_w()) == L"Areg Multitarget Router Service");
        AREG_CHILD_CHECK(std::wstring(app.service_description_w()) ==
            std::wstring(app.service_description_a(), app.service_description_a() + std::strlen(app.service_description_a())));
        AREG_CHILD_CHECK(std::string(app.service_description_a()).find("TCP / IP Multitarget Router") != std::string::npos);
        AREG_CHILD_CHECK(app.service_type() == areg::RemoteServiceKind::Router);
        AREG_CHILD_CHECK(app.connection_type() == areg::ConnectionType::Tcpip);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped);
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
    );
}

TEST(RouterApplicationTest, option_table_accepts_all_advertised_spellings)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        const auto setup = app.app_options();
        AREG_CHILD_CHECK(setup.first != nullptr && setup.second == 13);
        for (size_t i = 0u; i < std::size(SHORT_OPTIONS); ++i)
        {
            areg::ext::OptionParser parser(setup.first, static_cast<uint32_t>(setup.second));
            const std::string suffix{ i == 3u ? "=private.init" : "" };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(SHORT_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u);
            const int32_t command{ parser.options()[0].inCommand };
            AREG_CHILD_CHECK(parser.parse_option_line(areg::String((std::string(LONG_OPTIONS[i]) + suffix).c_str())));
            AREG_CHILD_CHECK(parser.options().size() == 1u && parser.options()[0].inCommand == command);
        }
        areg::ext::OptionParser invalid(setup.first, static_cast<uint32_t>(setup.second));
        AREG_CHILD_CHECK(!invalid.parse_option_line("--not-a-router-option"));
        AREG_CHILD_CHECK(!invalid.parse_option_line("--save=private.init"));
    );
}

TEST(RouterApplicationTest, command_line_modes_and_reset_preserve_stopped_state)
{
    AREG_EXPECT_IN_CHILD(
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--service"}) && app.current_option() == ServiceOption::CMD_Service);
        AREG_CHILD_CHECK(parse({"--install"}) && app.current_option() == ServiceOption::CMD_Install);
        AREG_CHILD_CHECK(parse({"--uninstall"}) && app.current_option() == ServiceOption::CMD_Uninstall);
        app.data_rate_helper().set_verbose(false);
        AREG_CHILD_CHECK(parse({"--verbose"}) && app.data_rate_helper().is_verbose());
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(parse({}) && app.current_option() == ServiceOption::CMD_Console);
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopped && !router_dispatcher().is_running());
    );
}

TEST(RouterApplicationTest, loaded_configuration_is_private_and_missing_load_is_rejected)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(!parse({"--load=" + config.path + ".missing"}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        const auto& configuration = areg::Application::config_manager();
        AREG_CHILD_CHECK(configuration.remote_service_port(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip) == config.port);
        AREG_CHILD_CHECK(configuration.remote_service_address(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip) == "127.0.0.1");
        AREG_CHILD_CHECK(configuration.remote_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip));
        AREG_CHILD_CHECK(app.communication_controller().setup_connection_data(
            areg::RemoteServiceKind::Router, static_cast<uint32_t>(areg::ConnectionType::Tcpip)));
        AREG_CHILD_CHECK(app.communication_controller().is_host_setup());
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        app.service_release();
        config.check_unchanged();
    );
}

TEST(RouterApplicationTest, public_status_outputs_only_in_console_mode)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        auto& app = application();
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Service);
        MultitargetRouter::print_status("C2-hidden-status");
        const std::string hidden{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(hidden.find("C2-hidden-status") == std::string::npos);
        testing::internal::CaptureStdout();
        redirect(descriptor(stdout), descriptor(stdout));
        app.set_current_option(ServiceOption::CMD_Console);
        MultitargetRouter::print_status("C2-visible-status");
        const std::string shown{ testing::internal::GetCapturedStdout() };
        redirect(descriptor(stdout), descriptor(stdout));
        AREG_CHILD_CHECK(shown.find("C2-visible-status") != std::string::npos);
        AREG_CHILD_CHECK(app.current_option() == ServiceOption::CMD_Console);
    );
}

TEST(RouterApplicationTest, malformed_commands_report_errors_and_help_remains_usable)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(true);
        ConsoleSession console(config);
        console.command("--not-a-router-option", "unexpected command");
        console.command("--console --install --uninstall --service --load=\"" + config.path + "\"",
                        "This command should be used in command line");
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--instances", "There are no connected instances");
        console.command("--threads", "Active per-client thread pairs: 0");
        console.quit();
        config.check_unchanged();
        config.check_options_logged({"Console", "Install", "Uninstall", "Service", "Load",
            "PrintHelp", "Instances", "Threads", "Quit"});
    );
}

// Reproduces the defect: a scrolled screen published its old rows again, so a test matched the text of an earlier command.
TEST(RouterApplicationTest, scrolled_terminal_publishes_no_old_rows)
{
    TerminalCapture terminal;
    const std::string filler(38u, '\n');
    terminal.feed(filler.data(), filler.size());
    const std::string marker{ "earlier-command-output\n" };
    terminal.feed(marker.data(), marker.size());
    const size_t offset{ terminal.transcript.size() };
    const std::string scroll{ "first-new-row\nsecond-new-row\nthird-new-row\n" };
    terminal.feed(scroll.data(), scroll.size());

    EXPECT_NE(std::string::npos, terminal.transcript.find("third-new-row", offset));
    EXPECT_EQ(std::string::npos, terminal.transcript.find("earlier-command-output", offset));
}

// Reproduces the defect: ncurses discarded the usage text at exit, so a failed command line printed nothing.
TEST(RouterApplicationTest, command_line_help_is_plain_text)
{
    AREG_EXPECT_IN_CHILD(
        set_terminal();
        for (const char* option : {"--help", "--not-a-router-option"})
        {
            testing::internal::CaptureStdout();
            const bool parsed{ parse({option}) };
            const std::string usage{ testing::internal::GetCapturedStdout() };
            AREG_CHILD_CHECK(!parsed);
            AREG_CHILD_CHECK(usage.find("Usage of Areg Multi-target Message Router") != std::string::npos);
            AREG_CHILD_CHECK(usage.find("--threads") != std::string::npos);
            AREG_CHILD_CHECK(usage.find('\x1b') == std::string::npos);
        }
    );
}

// Reproduces the defect: the router took the cookie to disconnect from the payload, so any client could drop another one.
TEST(RouterApplicationTest, disconnect_request_releases_only_its_sender)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        ConsoleSession console(config);
        ConnectedPeer owner(config.port);
        ConnectedPeer other(config.port);
        const ITEM_ID ownerCookie{ owner.reply.target };
        const ITEM_ID otherCookie{ other.reply.target };

        areg::StubAddress provider(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public, 0x1234u, 0x1111u, 0x2222u);
        provider.set_cookie(ownerCookie);
        provider.set_source(ownerCookie);
        areg::ProxyAddress consumer(0x4321u, areg::Version(1u, 2u, 3u), areg::ServiceType::Public, 0x1234u, 0x3333u, 0x4444u);
        consumer.set_cookie(otherCookie);
        consumer.set_source(otherCookie);
        send_to_router(owner, areg::router_register_service(provider, ownerCookie, areg::COOKIE_ROUTER));
        send_to_router(other, areg::router_register_consumer(consumer, otherCookie, areg::COOKIE_ROUTER));
        expect_registration(owner, areg::RegistrationAction::RegisterConsumer, areg::DisconnectReason::UndefinedReason);
        expect_registration(other, areg::RegistrationAction::RegisterProvider, areg::DisconnectReason::UndefinedReason);

        areg::MessageEnvelope bye{ areg::create_disconnect_request(otherCookie, areg::COOKIE_ROUTER) };
        AREG_CHILD_CHECK(bye.header() != nullptr);
        bye.header()->source = static_cast<uint32_t>(ownerCookie);
        send_to_router(owner, std::move(bye));

        expect_registration(other, areg::RegistrationAction::UnregisterProvider, areg::DisconnectReason::ProviderDisconnected);
        const auto instances = application().communication_controller().instances();
        areg::ConnectedInstance survivor;
        AREG_CHILD_CHECK(instances.size() == 1u && instances.find(otherCookie, survivor));
        console.quit();
    );
}

TEST(RouterApplicationTest, connected_instance_is_rendered_and_pause_restart_close_and_reopen_host)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(true);
        ConsoleSession console(config);
        auto& app = application();
        ConnectedPeer client(config.port);
        const auto instances = app.communication_controller().instances();
        AREG_CHILD_CHECK(instances.size() == 1u);
        areg::ConnectedInstance instance;
        AREG_CHILD_CHECK(instances.find(client.reply.target, instance));
        AREG_CHILD_CHECK(instance.ciInstance == areg::Process::instance().name().as_string());
        AREG_CHILD_CHECK(static_cast<uint32_t>(instance.ciBitness) == sizeof(void*) * 8u);
        const size_t rendered{ console.command("--instances", "Instance ID") };
        console.expect(instance_line(1u, client.reply.target, instance), rendered);
        console.expect(rate_line(areg::ext::FORMAT_SEND_DATA, client.received_bytes()));
        console.expect(rate_line(areg::ext::FORMAT_RECV_DATA, client.sentBytes));
        console.expect("Send messages ....:         1 msgs/sec");
        console.expect("Recv messages ....:         1 msgs/sec");
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        AREG_CHILD_CHECK(!app.communication_controller().is_host_connected());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        console.silent_command();
        check_console_verbose(AREG_EXTENDED == 0);
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--silent --threads", "Active per-client thread pairs: 0");
        console.verbose_command();
        check_console_verbose(true);
        console.command("--help", "Usage of Areg Multi-target Message Router");
        console.command("--verbose --threads", "Active per-client thread pairs: 0");
        check_console_verbose(true);
        console.command("--restart", "restarted");
        check_listening();
        ConnectedPeer restored(config.port);
        ConnectedPeer second(config.port);
        const auto populated = app.communication_controller().instances();
        AREG_CHILD_CHECK(populated.size() == 2u);
        AREG_CHILD_CHECK(second.reply.target != restored.reply.target);
        const size_t multiple{ console.command("--instances", "Instance ID") };
        console.expect("    1. |", multiple);
        console.expect("    2. |", multiple);
        for (auto position = populated.first_position(); populated.is_valid_position(position); position = populated.next_position(position))
        {
            ITEM_ID cookie{ 0u };
            areg::ConnectedInstance connected;
            populated.at_position(position, cookie, connected);
            console.expect_any({instance_line(1u, cookie, connected), instance_line(2u, cookie, connected)}, multiple);
        }
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(!router_dispatcher().is_running());
        AREG_CHILD_CHECK(!app.communication_controller().is_host_connected());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        restored.socket.close();
        second.socket.close();
        console.command("--instances", "There are no connected instances");
        console.command("--restart", "restarted");
        check_listening();
        AREG_CHILD_CHECK(app.communication_controller().is_host_connected());
        console.quit();
        config.check_unchanged();
        config.check_options_logged({"Instances", "Silent", "Verbose", "Pause", "Restart", "Quit"});
    );
}

// Reproduces the defect: the console read the pool list while the service thread filled it. It needs TSan to fail.
TEST(RouterApplicationTest, thread_pair_display_matches_pool_configuration_and_pause_restart)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config(false, 2u);
        ConsoleSession console(config);
        auto& communication = application().communication_controller();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 2u);
        console.command("--threads", "Active per-client thread pairs: 2");
        console.command("--pause", "paused");
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 0u);
        console.command("--threads", "Active per-client thread pairs: 0");
        console.command("--restart", "restarted");
        check_listening();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 2u);
        console.command("--threads", "Active per-client thread pairs: 2");
        console.quit();
        AREG_CHILD_CHECK(communication.active_client_pair_count() == 0u);
        config.check_unchanged();
    );
}

TEST(RouterApplicationRateTest, service_helper_queries_exact_wire_totals_and_mute_resets_interval)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        auto& app = application();
        AREG_CHILD_CHECK(parse({"--console", "--load=" + config.path}));
        AREG_CHILD_CHECK(app.service_initialize(ServiceOption::CMD_Console, nullptr, nullptr));
        AREG_CHILD_CHECK(app.service_start() && app.state() == ServicePhase::Running);
        check_listening();
        ConnectedPeer client(config.port);
        app.service_pause();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Paused && !router_dispatcher().is_running());
        AREG_CHILD_CHECK(app.communication_controller().instances().is_empty());
        client.socket.close();
        auto& helper = app.data_rate_helper();
        uint64_t bytes{ 999u };
        uint32_t messages{ 999u };
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == client.sentBytes && messages == 1u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == client.received_bytes() && messages == 1u);
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.set_verbose(false);
        AREG_CHILD_CHECK(!helper.is_verbose());
        helper.query_data_received(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.query_data_sent(bytes, messages);
        AREG_CHILD_CHECK(bytes == 0u && messages == 0u);
        helper.set_verbose(true);
        AREG_CHILD_CHECK(helper.is_verbose());
        AREG_CHILD_CHECK(app.service_continue() && app.state() == ServicePhase::Running);
        check_listening();
        app.service_stop();
        AREG_CHILD_CHECK(app.state() == ServicePhase::Stopping && !router_dispatcher().is_running());
        app.service_release();
        AREG_CHILD_CHECK(app.set_state(ServicePhase::Stopped));
        config.check_unchanged();
    );
}

// Reproduces the defect: the rate thread redrew the four rows that silent mode had cleared.
TEST(RouterApplicationRateTest, silent_mode_outputs_no_rate_rows)
{
    AREG_EXPECT_IN_CHILD(
        PrivateConfig config;
        ConsoleSession console(config);
        auto& helper = application().data_rate_helper();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.silent_command();
        const size_t silent{ console.mark() };
        AREG_CHILD_CHECK(helper.output_data_rate() == (AREG_EXTENDED == 0));
        console.command("--threads", "Active per-client thread pairs: 0");
        AREG_CHILD_CHECK((AREG_EXTENDED == 0) || !console.shown_since("Send data rate", silent));
        console.verbose_command();
        AREG_CHILD_CHECK(helper.output_data_rate());
        console.quit();
        config.check_unchanged();
    );
}

// Reproduces the defect: the enabled flag was a plain bool that one thread wrote while another read it. It needs TSan to fail.
TEST(RouterApplicationRateTest, live_enabled_changes_are_thread_safe)
{
    AREG_EXPECT_IN_CHILD(
        areg::DataRateStats stats;
        stats.set_enabled(true);
        std::promise<void> started;
        auto ready = started.get_future();
        std::thread producer([&]()
        {
            started.set_value();
            for (uint32_t i = 0u; i < 10'000u; ++i)
                stats.accumulate(7u, 1u);
        });
        AREG_CHILD_CHECK(ready.wait_for(DEADLINE) == std::future_status::ready);
        for (uint32_t i = 0u; i < 10'000u; ++i)
            stats.set_enabled((i & 1u) != 0u);
        producer.join();
        stats.set_enabled(false);
        stats.accumulate(999u, 999u);
        AREG_CHILD_CHECK(stats.extract_bytes() == 0u && stats.extract_msgs() == 0u);
        stats.set_enabled(true);
        stats.accumulate(7u * 13u, 13u);
        AREG_CHILD_CHECK(stats.extract_bytes() == 91u && stats.extract_msgs() == 13u);
    );
}
