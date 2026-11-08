/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial licenses. See LICENSE.txt for details.
 * Copyright (c) 2017-2026 Aregtech (Artak Avetyan)
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/File.hpp"
#include "areg/base/MathDefs.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/SocketMultiplexer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/ipc/SocketConnectionBase.hpp"
#include "areg/logging/LogConfiguration.hpp"
#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/logging/LogScope.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/logging/ScopeMessage.hpp"
#include "areg/persist/ConfigManager.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"
#include "aregextend/db/SqliteStatement.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#if AREG_LOGGING
#if defined(_WIN32)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

namespace
{
    namespace fs = std::filesystem;
    constexpr int32_t WAIT_MS{ 30'000 };
    constexpr ITEM_ID SOURCE_COOKIE{ 380u };
    constexpr ITEM_ID OBSERVER_COOKIE{ 381u };

    areg::String path_string(const fs::path& path)
    {
        return areg::String(path.string().c_str());
    }

    void write_row(const char* text, areg::LogMessageType type = areg::LogMessageType::MessageText)
    {
        const areg::LogEntry entry(type, 0u, 0u, 0u, areg::LogPriority::PrioInfo,
                                  text, static_cast<uint32_t>(std::strlen(text)));
        areg::log_local(entry);
    }

    class RuntimeDatabase final : public areg::LogDatabaseEngine
    {
    public:
        RuntimeDatabase()
            : mMutex()
            , mChanged()
            , mOpened(false)
            , mAccept(true)
            , mPaths()
            , mRoles()
            , mRows()
            , mClosedSessions(0u)
            , mCommits(0u)
            , mFlushedRows(0u)
        {
        }

        virtual bool is_operable() const noexcept override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mOpened;
        }

        virtual bool connect(const areg::String& path, areg::DatabaseRole role) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mPaths.emplace_back(path.as_string());
            mRoles.push_back(role);
            mOpened = mAccept;
            return mOpened;
        }

        virtual void disconnect() override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            if (mOpened)
                ++mClosedSessions;
            mOpened = false;
        }

        virtual bool log_message(const areg::LogEntry& entry) override
        {
            std::unique_lock<std::mutex> guard(mMutex);
            mRows.emplace_back(entry.logMessage, areg::log_message_size(entry));
            mChanged.notify_all();
            if (!mHoldRow.empty() && (mRows.back() == mHoldRow))
            {
                mHeld = true;
                mChanged.notify_all();
                mChanged.wait_for(guard, std::chrono::milliseconds(WAIT_MS), [this]() { return mReleased; });
            }

            return mOpened;
        }

        // Makes the logging thread wait inside log_message() for the row 'text' until released.
        void hold_row(const std::string& text)
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mHoldRow = text;
            mHeld = false;
            mReleased = false;
        }

        bool wait_held()
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(WAIT_MS), [this]() { return mHeld; });
        }

        void release_row()
        {
            {
                std::lock_guard<std::mutex> guard(mMutex);
                mReleased = true;
            }
            mChanged.notify_all();
        }

        virtual bool commit(bool) override
        {
            std::lock_guard<std::mutex> guard(mMutex);
            ++mCommits;
            mFlushedRows = mRows.size();
            mChanged.notify_all();
            return mOpened;
        }

        bool wait_commit(uint32_t waitMs)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(waitMs), [this]() { return mCommits != 0u; });
        }

        bool wait_row(const std::string& text, bool flushed = false)
        {
            std::unique_lock<std::mutex> guard(mMutex);
            return mChanged.wait_for(guard, std::chrono::milliseconds(WAIT_MS), [this, &text, flushed]()
            {
                const auto end{ flushed ? mRows.begin() + static_cast<std::ptrdiff_t>(mFlushedRows) : mRows.end() };
                return std::find(mRows.begin(), end, text) != end;
            });
        }

        std::vector<std::string> rows() const
        {
            std::lock_guard<std::mutex> guard(mMutex);
            return mRows;
        }

        virtual bool execute(const areg::String&) override { return true; }
        virtual bool begin() override { return true; }
        virtual bool rollback() override { return true; }
        virtual bool tables_initialized() const noexcept override { return is_operable(); }
        virtual bool log_instance_connected(const areg::ConnectedInstance&, const areg::DateTime&) override { return true; }
        virtual bool log_instance_disconnected(const ITEM_ID&, const areg::DateTime&) override { return true; }
        virtual bool log_scope_activate(const areg::ScopeEntry&, const ITEM_ID&, const areg::DateTime&) override { return true; }
        virtual bool log_scope_activate(const areg::String&, uint32_t, uint32_t, const ITEM_ID&, const areg::DateTime&) override { return true; }
        virtual uint32_t log_scopes_activate(const areg::ScopeNames&, const ITEM_ID&, const areg::DateTime&) override { return 0u; }
        virtual bool log_scopes_deactivate(const ITEM_ID&, const areg::DateTime&) override { return true; }
        virtual bool log_scope_deactivate(const ITEM_ID&, uint32_t, const areg::DateTime&) override { return true; }

        // Inspected or changed only while the logging thread is stopped.
        mutable std::mutex mMutex;
        std::condition_variable mChanged;
        bool mOpened;
        bool mAccept;
        std::vector<std::string> mPaths;
        std::vector<areg::DatabaseRole> mRoles;
        std::vector<std::string> mRows;
        std::string mHoldRow{ };
        bool mHeld{ false };
        bool mReleased{ false };
        uint32_t mClosedSessions;
        uint32_t mCommits;
        std::size_t mFlushedRows;
    };

    class RuntimeCollector
    {
    public:
        RuntimeCollector()
            : mInitialized(areg::socket_initialize())
            , mListener(areg::InvalidSocketHandle)
            , mPeer(areg::InvalidSocketHandle)
            , mPoll()
            , mPort(0u)
            , mLogs()
        {
        }

        ~RuntimeCollector()
        {
            close_peer();
            if (areg::is_valid_socket(mListener))
            {
                mPoll->unregister_socket(mListener);
                areg::socket_close(mListener);
            }
            mPoll.reset();
            if (mInitialized)
                areg::socket_release();
        }

        bool open(bool listenNow = true)
        {
            if (!mInitialized)
                return false;

            mPoll = std::make_unique<areg::SocketMultiplexer>();
            mListener = areg::socket_create();
            sockaddr_in address{};
            address.sin_family = AF_INET;
            address.sin_port = 0u;
            ::inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
            socklen_t length{ sizeof(address) };
            if (!areg::is_valid_socket(mListener)
                || (::bind(mListener, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
                || (::getsockname(mListener, reinterpret_cast<sockaddr*>(&address), &length) != 0))
                return false;

            mPort = ntohs(address.sin_port);
            return !listenNow || listen();
        }

        uint16_t port() const { return mPort; }
        const std::vector<std::string>& logs() const { return mLogs; }

        bool listen()
        {
            return (::listen(mListener, 4) == 0) && mPoll->register_socket(mListener, true);
        }

        void close_peer()
        {
            if (areg::is_valid_socket(mPeer))
            {
                mPoll->unregister_socket(mPeer);
                areg::socket_interrupt(mPeer);
                areg::socket_close(mPeer);
                mPeer = areg::InvalidSocketHandle;
                mPoll->register_socket(mListener, true);
            }
        }

        bool accept_source()
        {
            if (mPoll->wait(WAIT_MS) != mListener)
                return false;

            mPeer = static_cast<SOCKETHANDLE>(::accept(mListener, nullptr, nullptr));
            if (!areg::is_valid_socket(mPeer))
                return false;

            mPoll->unregister_socket(mListener);
            return mPoll->register_socket(mPeer, true)
                && areg::set_send_timeout(mPeer, static_cast<uint32_t>(WAIT_MS));
        }

        bool send(const areg::MessageEnvelope& message)
        {
            if (!message.is_valid())
                return false;

            const auto& header{ *message.header() };
            return areg::send_data(mPeer, areg::wire_data(header), areg::wire_size(header))
                == static_cast<int32_t>(areg::wire_size(header));
        }

        bool read(areg::MessageEnvelope& message, std::chrono::steady_clock::time_point deadline
            = std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS))
        {
            areg::EventHeader header{};
            if (!read_exact(areg::wire_header(header), areg::WIRE_HEADER_SIZE, deadline))
                return false;

            uint8_t* body{ areg::SocketConnectionBase::prepare_message(message, header) };
            if ((body == nullptr) || !read_exact(body, header.bufHeader.biUsed, deadline))
                return false;

            message.set_size_used(header.bufHeader.biUsed);
            message.move_to_begin();
            if ((message.message_id() == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage))
                && (message.size_used() >= areg::log_entry_head() + 1u))
            {
                areg::LogEntry entry;
                message >> entry;
                mLogs.emplace_back(entry.logMessage, areg::log_message_size(entry));
                message.move_to_begin();
            }
            return true;
        }

        bool next(areg::FuncIdRange id, areg::MessageEnvelope& message)
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            for (uint32_t count = 0u; count < 256u; ++count)
            {
                if (!read(message, deadline))
                    return false;
                if (message.message_id() == static_cast<uint32_t>(id))
                    return true;
            }
            return false;
        }

        bool handshake()
        {
            areg::MessageEnvelope message;
            return accept_source() && read(message) && register_source(message);
        }

        /**
         * \brief   Makes the handshake with the first source that stays connected. A source drops
         *          a connection that completes after its connect timeout, and connects again.
         **/
        bool handshake_next_source()
        {
            const auto deadline{ std::chrono::steady_clock::now() + std::chrono::milliseconds(WAIT_MS) };
            areg::MessageEnvelope message;
            while (std::chrono::steady_clock::now() < deadline)
            {
                if (accept_source() && read(message, deadline))
                    return register_source(message);

                close_peer();
            }
            return false;
        }

    private:
        bool register_source(areg::MessageEnvelope& message)
        {
            EXPECT_EQ(message.message_id(), static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect));
            EXPECT_EQ(message.target(), static_cast<uint32_t>(areg::COOKIE_LOGGER));
            return send(areg::create_connect_notify(areg::COOKIE_LOGGER, SOURCE_COOKIE))
                && next(areg::FuncIdRange::ServiceLogRegisterScopes, message);
        }

        bool read_exact(uint8_t* bytes, uint32_t size, std::chrono::steady_clock::time_point deadline)
        {
            uint32_t done{ 0u };
            while (done != size)
            {
                const auto remaining{ std::chrono::duration_cast<std::chrono::milliseconds>(
                    deadline - std::chrono::steady_clock::now()).count() };
                if ((remaining <= 0) || (mPoll->wait(static_cast<int32_t>(remaining)) != mPeer))
                    return false;

                const int32_t received{ static_cast<int32_t>(::recv(mPeer,
                    reinterpret_cast<char*>(bytes + done), static_cast<int>(size - done), 0)) };
                if (received <= 0)
                    return false;
                done += static_cast<uint32_t>(received);
            }
            return true;
        }

        const bool mInitialized;
        SOCKETHANDLE mListener;
        SOCKETHANDLE mPeer;
        std::unique_ptr<areg::SocketMultiplexer> mPoll;
        uint16_t mPort;
        std::vector<std::string> mLogs;
    };

    class LoggingRuntimeCoverageTest : public testing::Test
    {
    protected:
        LoggingRuntimeCoverageTest()
            : mManager(areg::Application::config_manager())
            , mWasConfigured(mManager.is_configured())
            , mReadonly(mManager.readonly_properties())
            , mWritable(mManager.module_properties())
            , mDirectory()
            , mWorkingDirectory()
            , mConfig()
            , mDatabase()
            , mSqlite()
            , mCollector()
            , mSetup(false)
            , mOwnDirectory(false)
        {
        }

        virtual void SetUp() override
        {
            static std::atomic<uint32_t> serial{ 0u };
            std::error_code error;
            mWorkingDirectory = fs::current_path(error);
            ASSERT_FALSE(error) << error.message();
            mDirectory = fs::absolute(fs::path(testing::TempDir())
                / ("areg-runtime-" + std::to_string(areg::Process::instance().id())
                   + "-" + std::to_string(serial.fetch_add(1u))), error);
            ASSERT_FALSE(error) << error.message();
            mOwnDirectory = fs::create_directory(mDirectory, error);
            ASSERT_TRUE(mOwnDirectory) << error.message();
            mManager.set_configuration(areg::ListProperties(), areg::ListProperties());
            mManager.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true);
            mManager.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, false, true);
            for (const auto target : { areg::LogTarget::File, areg::LogTarget::Remote, areg::LogTarget::Debug, areg::LogTarget::Database })
                mConfig.set_log_enabled(target, false);
            mConfig.set_status(true);
            mConfig.set_log_file(path_string(mDirectory / "runtime.log"));
            mConfig.set_append_data(false);
            mConfig.set_layout_message("row:%m%n");
            mConfig.set_layout_enter("enter:%z%n");
            mConfig.set_layout_exit("exit:%z%n");
            mConfig.set_db_path(path_string(mDirectory / "runtime.sqlog"));
            EXPECT_EQ(areg::set_source_state(areg::LogSourceState::Active), areg::LogSourceState::Active);
            ASSERT_TRUE(areg::restore_logging());
        }

        virtual void TearDown() override
        {
            if (mSetup)
                areg::Application::release();
            else if (areg::is_started())
                areg::stop_logging(true);
            areg::set_db_engine(nullptr);
            mSqlite.reset();
            mCollector.reset();
            mManager.set_configuration(mReadonly, mWritable);
            EXPECT_EQ(areg::set_source_state(areg::LogSourceState::Active), areg::LogSourceState::Active);
            EXPECT_TRUE(areg::restore_logging());
            if (!mWasConfigured)
                mManager.release_properties();

            std::error_code error;
            if (!mWorkingDirectory.empty())
                fs::current_path(mWorkingDirectory, error);
            EXPECT_FALSE(error) << error.message();
            if (mOwnDirectory)
                fs::remove_all(mDirectory, error);
            EXPECT_FALSE(error) << error.message();
        }

        void setup(bool timers = false)
        {
            mSetup = true;
            areg::Application::setup(false, false, false, timers, false);
        }

        void use_database()
        {
            mConfig.set_log_enabled(areg::LogTarget::Database, true);
            areg::set_db_engine(&mDatabase);
        }

        bool configure_remote(bool listenNow = true)
        {
            mCollector = std::make_unique<RuntimeCollector>();
            if (!mCollector->open(listenNow))
                return false;

            mConfig.set_log_enabled(areg::LogTarget::Remote, true);
            mConfig.set_tcp_enable(true, true);
            mConfig.set_tcp_address("127.0.0.1", true);
            mConfig.set_tcp_port(mCollector->port(), true);
            return true;
        }

        bool start_remote()
        {
            if (!configure_remote())
                return false;

            setup(true);
            return areg::start_logging() && mCollector->handshake();
        }

        std::string contents(const fs::path& path)
        {
            std::ifstream input(path, std::ios::binary);
            EXPECT_TRUE(input.is_open()) << path;
            return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        }

        void expect_remote_row(const char* text)
        {
            areg::MessageEnvelope message;
            ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogMessage, message));
            ASSERT_GE(message.size_used(), areg::log_entry_head() + 1u);
            areg::LogEntry entry;
            message >> entry;
            EXPECT_STREQ(entry.logMessage, text);
            EXPECT_EQ(entry.logCookie, SOURCE_COOKIE);
            EXPECT_EQ(entry.logDataType, areg::LogDataType::Remote);
            EXPECT_EQ(message.source(), static_cast<uint32_t>(SOURCE_COOKIE));
        }

        void expect_state_reply(areg::LogSourceState wanted)
        {
            areg::MessageEnvelope message;
            ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogSourceStateUpdated, message));
            ITEM_ID source{ 0u };
            ITEM_ID observer{ 0u };
            uint8_t state{ 0u };
            message >> source >> state >> observer;
            EXPECT_EQ(source, SOURCE_COOKIE);
            EXPECT_EQ(state, static_cast<uint8_t>(wanted));
            EXPECT_EQ(observer, OBSERVER_COOKIE);
            EXPECT_EQ(areg::source_state(), wanted);
        }

        areg::ConfigManager& mManager;
        const bool mWasConfigured;
        const areg::ListProperties mReadonly;
        const areg::ListProperties mWritable;
        fs::path mDirectory;
        fs::path mWorkingDirectory;
        areg::LogConfiguration mConfig;
        RuntimeDatabase mDatabase;
        std::unique_ptr<areg::ext::LogSqliteDatabase> mSqlite;
        std::unique_ptr<RuntimeCollector> mCollector;
        bool mSetup;
        bool mOwnDirectory;
    };
}

TEST_F(LoggingRuntimeCoverageTest, file_routes_text_and_scope_boundaries_in_order)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    ASSERT_TRUE(areg::is_configured());
    ASSERT_TRUE(areg::start_logging());
    EXPECT_TRUE(areg::is_started());
    EXPECT_TRUE(areg::is_enabled());
    write_row("first");
    write_row("scope", areg::LogMessageType::ScopeEnter);
    auto message{ areg::make_log_message(areg::LogMessageType::MessageText, 0u, 0u, 0u,
                                        areg::LogPriority::PrioWarning, "envelope", 8u) };
    ASSERT_TRUE(message.is_valid());
    areg::log_message(message);
    EXPECT_TRUE(message.is_valid());
    write_row("scope", areg::LogMessageType::ScopeExit);
    areg::stop_logging(true);
    EXPECT_FALSE(areg::is_started());
    const std::string text{ contents(mDirectory / "runtime.log") };
    EXPECT_NE(text.find("row:first\nenter:scope\nrow:envelope\nexit:scope\n"), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, messages_outside_logging_lifetime_are_not_replayed)
{
    use_database();
    auto message{ areg::make_log_message(areg::LogMessageType::MessageText, 0u, 0u, 0u,
                                        areg::LogPriority::PrioInfo, "not-replayed", 12u) };
    ASSERT_TRUE(message.is_valid());
    write_row("before-start");
    areg::log_message(message);
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("while-running");
    areg::stop_logging(true);
    write_row("after-stop");
    areg::log_message(message);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "while-running" }));
    EXPECT_TRUE(message.is_valid());
}

TEST_F(LoggingRuntimeCoverageTest, restart_truncates_file_and_rebuilds_layouts)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("discarded");
    areg::stop_logging(false);
    areg::wait_logging_end();
    EXPECT_FALSE(areg::is_started());
    EXPECT_NE(contents(mDirectory / "runtime.log").find("row:discarded\n"), std::string::npos);
    mConfig.set_layout_message("new:%m%n");
    ASSERT_TRUE(areg::Application::start_logging());
    write_row("retained");
    areg::Application::stop_logging();
    const std::string text{ contents(mDirectory / "runtime.log") };
    EXPECT_EQ(text.find("discarded"), std::string::npos);
    EXPECT_NE(text.find("new:retained\n"), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, name_masks_roll_over_to_a_new_configured_file)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    for (const char* prefix : { "first_", "second_" })
    {
        mConfig.set_log_file(path_string(mDirectory / (std::string(prefix) + "%appname%_%time%.log")));
        ASSERT_TRUE(areg::start_logging());
        write_row(prefix);
        areg::stop_logging(true);
    }
    uint32_t count{ 0u };
    std::error_code error;
    fs::directory_iterator entries(mDirectory, error);
    ASSERT_FALSE(error) << error.message();
    for (const auto& entry : entries)
    {
        const std::string name{ entry.path().filename().string() };
        EXPECT_EQ(name.find('%'), std::string::npos);
        EXPECT_NE(name.find(areg::Process::instance().app_name().as_string()), std::string::npos);
        const bool first{ name.find("first_") == 0u };
        EXPECT_TRUE(first || name.find("second_") == 0u);
        const std::string text{ contents(entry.path()) };
        EXPECT_NE(text.find(first ? "row:first_\n" : "row:second_\n"), std::string::npos);
        EXPECT_EQ(text.find(first ? "row:second_\n" : "row:first_\n"), std::string::npos);
        ++count;
    }
    EXPECT_EQ(count, 2u);
}

TEST_F(LoggingRuntimeCoverageTest, blocked_file_path_leaves_database_target_operable)
{
    const fs::path blocker{ mDirectory / "not-a-directory" };
    {
        std::ofstream file(blocker);
        ASSERT_TRUE(file.is_open());
        file << "unchanged";
    }
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_log_file(path_string(blocker / "denied.log"));
    use_database();
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("database-survives");
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "database-survives" }));
    EXPECT_EQ(contents(blocker), "unchanged");
    EXPECT_FALSE(fs::exists(blocker / "denied.log"));
    EXPECT_FALSE(mDatabase.is_operable());
}

TEST_F(LoggingRuntimeCoverageTest, empty_file_name_leaves_other_targets_operable)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_log_file("");
    use_database();
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("empty-path");
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "empty-path" }));
    EXPECT_TRUE(fs::is_empty(mDirectory));
}

TEST_F(LoggingRuntimeCoverageTest, database_connect_write_flush_disconnect_and_restart_preserve_order)
{
    use_database();
    setup();
    uint32_t sessions{ 0u };
    for (const char* text : { "first-session", "second-session" })
    {
        ASSERT_TRUE(areg::start_logging());
        EXPECT_TRUE(mDatabase.is_operable());
        write_row(text);
        ASSERT_TRUE(mDatabase.wait_row(text, true));
        areg::stop_logging(true);
        EXPECT_FALSE(mDatabase.is_operable());
        EXPECT_EQ(mDatabase.mClosedSessions, ++sessions);
    }
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "first-session", "second-session" }));
    ASSERT_EQ(mDatabase.mPaths.size(), 2u);
    EXPECT_EQ(mDatabase.mPaths[0], (mDirectory / "runtime.sqlog").string());
    EXPECT_EQ(mDatabase.mPaths[1], mDatabase.mPaths[0]);
    EXPECT_EQ(mDatabase.mRoles, (std::vector<areg::DatabaseRole>(2u, areg::DatabaseRole::Recorder)));
    EXPECT_EQ(mDatabase.mClosedSessions, 2u);
    EXPECT_GE(mDatabase.mCommits, 2u);
}

TEST_F(LoggingRuntimeCoverageTest, disabled_targets_and_missing_database_engine_survive_restart)
{
    setup();
    for (bool enabled : { false, true, false })
    {
        mConfig.set_log_enabled(areg::LogTarget::Debug, enabled);
        mConfig.set_log_enabled(areg::LogTarget::Database, enabled);
        ASSERT_TRUE(areg::start_logging());
        write_row("no-file-target");
        areg::stop_logging(true);
        EXPECT_FALSE(areg::is_started());
        EXPECT_TRUE(fs::is_empty(mDirectory));
    }
}

TEST_F(LoggingRuntimeCoverageTest, global_disable_keeps_targets_closed_until_an_explicit_restart)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_status(false);
    setup();
    ASSERT_TRUE(areg::start_logging());
    EXPECT_TRUE(areg::is_started());
    EXPECT_FALSE(areg::is_enabled());
    write_row("disabled-session");
    areg::stop_logging(true);
    EXPECT_TRUE(fs::is_empty(mDirectory));
    mConfig.set_status(true);
    ASSERT_TRUE(areg::start_logging());
    write_row("enabled-session");
    areg::stop_logging(true);
    const auto text{ contents(mDirectory / "runtime.log") };
    EXPECT_EQ(text.find("disabled-session"), std::string::npos);
    EXPECT_NE(text.find("row:enabled-session\n"), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, forced_start_enables_file_and_activates_registered_scopes)
{
    areg::LogScope scope("a15_runtime.forced");
    mConfig.set_status(false);
    setup();
    ASSERT_TRUE(areg::force_start_logging());
    EXPECT_TRUE(areg::is_started());
    EXPECT_TRUE(areg::is_enabled());
    EXPECT_TRUE(mConfig.is_file_logging_enabled());
    areg::activate_scope(scope);
    EXPECT_NE(scope.priority() & static_cast<uint32_t>(areg::LogPriority::PrioDebug), 0u);
    {
        areg::ScopeMessage message(scope);
        message.log_debug("forced-row");
    }
    areg::stop_logging(true);
    const auto text{ contents(mDirectory / "runtime.log") };
    EXPECT_NE(text.find("enter:a15_runtime.forced\nrow:forced-row\nexit:a15_runtime.forced\n"), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, each_level_writes_only_at_or_above_the_scope_priority)
{
    areg::LogScope scope("a15_runtime.levels");
    mConfig.set_status(false);
    setup();
    ASSERT_TRUE(areg::force_start_logging());
    areg::activate_scope(scope);
    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioWarning));
    {
        areg::ScopeMessage message(scope);
        message.log_debug("hidden-debug");
        message.log_info("hidden-info");
        message.log_warning("warning-%d", 1);
        message.log_error("error-%d", 2);
        message.log_fatal("fatal-%d", 3);
    }

    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioFatal));
    {
        areg::ScopeMessage message(scope);
        message.log_error("hidden-error");
        message.log_fatal("fatal-%d", 4);
    }

    scope.set_priority(static_cast<uint32_t>(areg::LogPriority::PrioInfo));
    {
        areg::ScopeMessage message(scope);
        message.log_debug("hidden-debug");
        message.log_info("info-%d", 5);
    }

    areg::stop_logging(true);
    const auto text{ contents(mDirectory / "runtime.log") };
    EXPECT_NE(text.find("row:warning-1\nrow:error-2\nrow:fatal-3\nrow:fatal-4\nrow:info-5\n"), std::string::npos);
    EXPECT_EQ(text.find("hidden-"), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, missing_configuration_reports_failure_and_restore_applies_defaults)
{
    mManager.release_properties();
    EXPECT_FALSE(areg::is_configured());
    const std::string missing{ (mDirectory / "missing.init").string() };
    EXPECT_FALSE(areg::initialize_logging(missing.c_str()));
    EXPECT_TRUE(areg::is_configured());
    mManager.release_properties();
    EXPECT_FALSE(areg::restore_logging());
    EXPECT_TRUE(areg::is_configured());
    EXPECT_FALSE(areg::is_started());
    EXPECT_TRUE(fs::is_empty(mDirectory));
}

TEST_F(LoggingRuntimeCoverageTest, database_target_persists_rows_and_closes_the_sqlite_file)
{
    mSqlite = std::make_unique<areg::ext::LogSqliteDatabase>();
    areg::set_db_engine(mSqlite.get());
    mConfig.set_log_enabled(areg::LogTarget::Database, true);
    setup();
    ASSERT_TRUE(areg::start_logging());
    ASSERT_TRUE(mSqlite->is_operable());
    write_row("runtime-persisted-one");
    write_row("runtime-persisted-two");
    areg::stop_logging(true);
    EXPECT_FALSE(mSqlite->is_operable());
    const fs::path path{ mDirectory / "runtime.sqlog" };
    EXPECT_TRUE(fs::is_regular_file(path));
    EXPECT_FALSE(fs::exists(path.string() + "-wal"));
    EXPECT_FALSE(fs::exists(path.string() + "-shm"));
    areg::ext::LogSqliteDatabase reader;
    ASSERT_TRUE(reader.connect(path_string(path), areg::DatabaseRole::Viewer));
    areg::ext::SqliteStatement statement(reader.database(), "SELECT msg_log FROM logs WHERE msg_log LIKE 'runtime-persisted-%' ORDER BY id;");
    ASSERT_TRUE(statement.is_valid());
    ASSERT_EQ(statement.next(), areg::ext::SqliteStatement::QueryResult::HasMore);
    EXPECT_EQ(statement.as_text(0), "runtime-persisted-one");
    ASSERT_EQ(statement.next(), areg::ext::SqliteStatement::QueryResult::HasMore);
    EXPECT_EQ(statement.as_text(0), "runtime-persisted-two");
    EXPECT_EQ(statement.next(), areg::ext::SqliteStatement::QueryResult::HasNoMore);
    statement.finalize();
    reader.disconnect();
}

TEST_F(LoggingRuntimeCoverageTest, blocked_database_path_does_not_prevent_file_logging)
{
    const fs::path blocker{ mDirectory / "database-parent" };
    {
        std::ofstream file(blocker);
        ASSERT_TRUE(file.is_open());
        file << "unchanged";
    }
    mSqlite = std::make_unique<areg::ext::LogSqliteDatabase>();
    areg::set_db_engine(mSqlite.get());
    mConfig.set_db_path(path_string(blocker / "denied.sqlog"));
    mConfig.set_log_enabled(areg::LogTarget::Database, true);
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    ASSERT_TRUE(areg::start_logging());
    EXPECT_FALSE(mSqlite->is_operable());
    write_row("file-survives");
    areg::stop_logging(true);
    EXPECT_NE(contents(mDirectory / "runtime.log").find("row:file-survives\n"), std::string::npos);
    EXPECT_EQ(contents(blocker), "unchanged");
}

TEST_F(LoggingRuntimeCoverageTest, remote_scope_updates_and_restore_acknowledge_exact_priorities)
{
    areg::LogScope scope("a15_runtime.remote");
    const uint32_t saved{ static_cast<uint32_t>(areg::LogPriority::PrioWarning) };
    mConfig.enable_scopes({ "a15_runtime.*" }, true);
    ASSERT_TRUE(start_remote());
    const uint32_t configured{ scope.priority() };
    areg::MessageEnvelope reply;
    ASSERT_TRUE(mCollector->send(areg::message_update_scope(OBSERVER_COOKIE, SOURCE_COOKIE,
        "a15_runtime.remote", scope.id(), saved)));
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogScopesUpdated, reply));
    uint32_t count{ 0u };
    reply >> count;
    bool found{ false };
    for (uint32_t index = 0u; index < count; ++index)
    {
        areg::ScopeEntry entry;
        reply >> entry;
        if (entry.scopeId == scope.id())
        {
            EXPECT_EQ(entry.scopeName, "a15_runtime.remote");
            EXPECT_EQ(entry.scopePrio, saved);
            found = true;
        }
    }
    EXPECT_TRUE(found);
    EXPECT_EQ(scope.priority(), saved);
    ASSERT_TRUE(mCollector->send(areg::message_update_scopes(OBSERVER_COOKIE, SOURCE_COOKIE, areg::ScopeNames())));
    ASSERT_TRUE(mCollector->send(areg::message_query_scopes(OBSERVER_COOKIE, SOURCE_COOKIE)));
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogRegisterScopes, reply));
    EXPECT_EQ(scope.priority(), saved);
    ASSERT_TRUE(mCollector->send(areg::message_restore_configuration(OBSERVER_COOKIE, SOURCE_COOKIE)));
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogConfigurationRestored, reply));
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogScopesUpdated, reply));
    EXPECT_EQ(scope.priority(), configured);
    areg::stop_logging(true);
}

TEST_F(LoggingRuntimeCoverageTest, remote_pause_drops_remote_rows_but_preserves_local_targets)
{
    use_database();
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    ASSERT_TRUE(start_remote());
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE, areg::LogSourceState::Paused)));
    expect_state_reply(areg::LogSourceState::Paused);
    write_row("paused-row");
    write_row("pause-fence");
    ASSERT_TRUE(mDatabase.wait_row("pause-fence", true));
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE, areg::LogSourceState::Active)));
    expect_state_reply(areg::LogSourceState::Active);
    write_row("resumed-row");
    expect_remote_row("resumed-row");
    EXPECT_EQ(mCollector->logs(), (std::vector<std::string>{ "resumed-row" }));
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "paused-row", "pause-fence", "resumed-row" }));
    const auto text{ contents(mDirectory / "runtime.log") };
    EXPECT_NE(text.find("row:paused-row\nrow:pause-fence\nrow:resumed-row\n"), std::string::npos);
}

// With the remote and the database target on, each row reaches both targets complete and in order.
TEST_F(LoggingRuntimeCoverageTest, remote_and_database_targets_both_receive_every_row_intact)
{
    use_database();
    ASSERT_TRUE(start_remote());
    std::vector<std::string> expected;
    for (uint32_t i = 0u; i < 32u; ++i)
    {
        expected.push_back("both-targets-" + std::to_string(i));
        write_row(expected.back().c_str());
    }

    for (const std::string& row : expected)
        expect_remote_row(row.c_str());

    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), expected);
}

TEST_F(LoggingRuntimeCoverageTest, remote_stop_resume_and_invalid_state_are_ordered_by_replies)
{
    areg::LogScope scope("a15_runtime.states");
    mConfig.enable_scopes({ "a15_runtime.*" }, true);
    ASSERT_TRUE(start_remote());
    const uint32_t priority{ scope.priority() };
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE, areg::LogSourceState::Stopped)));
    expect_state_reply(areg::LogSourceState::Stopped);
    EXPECT_EQ(scope.priority(), static_cast<uint32_t>(areg::LogPriority::PrioNotset));
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE,
                                                                 static_cast<areg::LogSourceState>(0xFFu))));
    ASSERT_TRUE(mCollector->send(areg::message_query_scopes(OBSERVER_COOKIE, SOURCE_COOKIE)));
    areg::MessageEnvelope reply;
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogRegisterScopes, reply));
    EXPECT_EQ(areg::source_state(), areg::LogSourceState::Stopped);
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE, areg::LogSourceState::Active)));
    expect_state_reply(areg::LogSourceState::Active);
    EXPECT_EQ(scope.priority(), priority);
    write_row("active-again");
    expect_remote_row("active-again");
    areg::stop_logging(true);
}

TEST_F(LoggingRuntimeCoverageTest, remote_group_priority_update_leaves_stopped_state_and_reports_observer)
{
    areg::LogScope first("a15_runtime.group_first");
    areg::LogScope second("a15_runtime.group_second");
    mConfig.enable_scopes({ "a15_runtime.*" }, true);
    ASSERT_TRUE(start_remote());
    ASSERT_TRUE(mCollector->send(areg::message_update_source_state(OBSERVER_COOKIE, SOURCE_COOKIE, areg::LogSourceState::Stopped)));
    expect_state_reply(areg::LogSourceState::Stopped);
    const uint32_t priority{ static_cast<uint32_t>(areg::LogPriority::PrioError) };
    ASSERT_TRUE(mCollector->send(areg::message_update_scope(OBSERVER_COOKIE, SOURCE_COOKIE, "a15_runtime.*", 0u, priority)));
    areg::MessageEnvelope reply;
    ASSERT_TRUE(mCollector->next(areg::FuncIdRange::ServiceLogScopesUpdated, reply));
    expect_state_reply(areg::LogSourceState::Active);
    EXPECT_EQ(first.priority(), priority);
    EXPECT_EQ(second.priority(), priority);
    areg::stop_logging(true);
}

TEST_F(LoggingRuntimeCoverageTest, remote_peer_close_reconnects_and_logging_restart_registers_again)
{
    ASSERT_TRUE(start_remote());
    write_row("before-close");
    expect_remote_row("before-close");
    mCollector->close_peer();
    ASSERT_TRUE(mCollector->handshake());
    write_row("after-reconnect");
    expect_remote_row("after-reconnect");
    areg::stop_logging(true);
    EXPECT_FALSE(areg::is_started());
    mCollector->close_peer();
    ASSERT_TRUE(areg::start_logging());
    ASSERT_TRUE(mCollector->handshake());
    write_row("after-restart");
    expect_remote_row("after-restart");
    areg::stop_logging(true);
}

TEST_F(LoggingRuntimeCoverageTest, remote_handshake_stall_does_not_block_local_delivery_or_shutdown)
{
    use_database();
    ASSERT_TRUE(configure_remote());
    mConfig.set_stack_size(0u);
    setup(true);
    ASSERT_TRUE(areg::start_logging());
    ASSERT_TRUE(mCollector->accept_source());
    areg::MessageEnvelope connect;
    ASSERT_TRUE(mCollector->read(connect));
    EXPECT_EQ(connect.message_id(), static_cast<uint32_t>(areg::FuncIdRange::SystemServiceConnect));
    EXPECT_EQ(areg::cookie(), areg::COOKIE_UNKNOWN);
    write_row("collector-has-not-replied");
    ASSERT_TRUE(mDatabase.wait_row("collector-has-not-replied"));
    areg::stop_logging(true);
    EXPECT_FALSE(areg::is_started());
    EXPECT_FALSE(mDatabase.is_operable());
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "collector-has-not-replied" }));
}

TEST_F(LoggingRuntimeCoverageTest, non_listening_collector_keeps_local_delivery_and_recovers_when_listening)
{
    use_database();
    ASSERT_TRUE(configure_remote(false));
    mConfig.set_stack_size(4u);
    setup(true);
    ASSERT_TRUE(areg::start_logging());
    write_row("collector-unavailable");
    write_row("unavailable-fence");
    ASSERT_TRUE(mDatabase.wait_row("unavailable-fence"));
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "collector-unavailable", "unavailable-fence" }));
    EXPECT_EQ(areg::cookie(), areg::COOKIE_UNKNOWN);
    ASSERT_TRUE(mCollector->listen());
    ASSERT_TRUE(mCollector->handshake_next_source());
    expect_remote_row("collector-unavailable");
    expect_remote_row("unavailable-fence");
    write_row("collector-available");
    expect_remote_row("collector-available");
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "collector-unavailable", "unavailable-fence", "collector-available" }));
    EXPECT_EQ(mCollector->logs(), (std::vector<std::string>{ "collector-unavailable", "unavailable-fence", "collector-available" }));
}

TEST_F(LoggingRuntimeCoverageTest, logging_before_application_setup_opens_remote_target_only_after_setup)
{
    use_database();
    ASSERT_TRUE(configure_remote());
    ASSERT_TRUE(areg::start_logging());
    write_row("local-before-setup");
    ASSERT_TRUE(mDatabase.wait_row("local-before-setup", true));
    EXPECT_EQ(areg::cookie(), areg::COOKIE_UNKNOWN);
    setup(true);
    ASSERT_TRUE(mCollector->handshake());
    write_row("remote-after-setup");
    expect_remote_row("remote-after-setup");
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "local-before-setup", "remote-after-setup" }));
    EXPECT_EQ(mCollector->logs(), (std::vector<std::string>{ "remote-after-setup" }));
}

/**
 * \brief   Reproduces the defect: append opened the existing file at offset zero and overwrote it.
 **/
TEST_F(LoggingRuntimeCoverageTest, append_preserves_existing_file_bytes_across_restart)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("must-survive-append");
    areg::stop_logging(true);
    const std::string original{ contents(mDirectory / "runtime.log") };
    mConfig.set_append_data(true);
    ASSERT_TRUE(areg::start_logging());
    write_row("appended");
    areg::stop_logging(true);
    const std::string result{ contents(mDirectory / "runtime.log") };
    EXPECT_EQ(result.compare(0u, original.size(), original), 0);
    EXPECT_NE(result.find("row:appended\n", original.size()), std::string::npos);
}

/**
 * \brief   Reproduces the defect: a masked name failed the existence check and truncated the file.
 **/
TEST_F(LoggingRuntimeCoverageTest, append_with_appname_mask_preserves_the_existing_file)
{
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_append_data(true);
    mConfig.set_log_file(path_string(mDirectory / "%appname%.log"));
    const fs::path expanded{ mDirectory / areg::Process::instance().app_name().as_string() };
    const fs::path file{ expanded.string() + ".log" };
    {
        std::ofstream output(file, std::ios::binary);
        ASSERT_TRUE(output.is_open());
        output << "original-mask-content\n";
    }
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("appended-mask");
    areg::stop_logging(true);
    const std::string result{ contents(file) };
    EXPECT_EQ(result.find("original-mask-content\n"), 0u);
    EXPECT_NE(result.find("row:appended-mask\n"), std::string::npos);
}

/**
 * \brief   Reproduces the defect: a disabled database target still received rows.
 **/
TEST_F(LoggingRuntimeCoverageTest, disabled_database_target_never_receives_rows)
{
    areg::set_db_engine(&mDatabase);
    mConfig.set_log_enabled(areg::LogTarget::Database, false);
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("file-only");
    areg::stop_logging(true);
    EXPECT_TRUE(mDatabase.mPaths.empty());
    EXPECT_TRUE(mDatabase.rows().empty());
    EXPECT_NE(contents(mDirectory / "runtime.log").find("row:file-only\n"), std::string::npos);
}

/**
 * \brief   Reproduces the defect: an engine that refused to open still received rows.
 **/
TEST_F(LoggingRuntimeCoverageTest, failed_database_open_never_forwards_rows_to_closed_engine)
{
    use_database();
    mDatabase.mAccept = false;
    setup();
    ASSERT_TRUE(areg::start_logging());
    EXPECT_FALSE(mDatabase.is_operable());
    write_row("must-not-reach-closed-engine");
    areg::stop_logging(true);
    ASSERT_EQ(mDatabase.mPaths.size(), 1u);
    EXPECT_TRUE(mDatabase.rows().empty());
}

TEST_F(LoggingRuntimeCoverageTest, overwrite_mode_truncates_a_longer_existing_file)
{
    const fs::path file{ mDirectory / "runtime.log" };
    {
        std::ofstream output(file, std::ios::binary);
        ASSERT_TRUE(output.is_open());
        output << std::string(64u * 1024u, 'x');
    }

    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_append_data(false);
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("fresh");
    areg::stop_logging(true);
    const std::string text{ contents(file) };
    EXPECT_EQ(text.find("xxxxxxxx"), std::string::npos);
    EXPECT_NE(text.find("row:fresh\n"), std::string::npos);
    EXPECT_LT(text.size(), 64u * 1024u);
}

TEST_F(LoggingRuntimeCoverageTest, append_creates_a_missing_directory_and_file)
{
    const fs::path file{ mDirectory / "append-dir" / "nested" / "created.log" };
    mConfig.set_log_enabled(areg::LogTarget::File, true);
    mConfig.set_append_data(true);
    mConfig.set_log_file(path_string(file));
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("first-append");
    areg::stop_logging(true);
    ASSERT_TRUE(fs::is_regular_file(file));
    const std::string first{ contents(file) };
    EXPECT_NE(first.find("row:first-append\n"), std::string::npos);

    ASSERT_TRUE(areg::start_logging());
    write_row("second-append");
    areg::stop_logging(true);
    const std::string second{ contents(file) };
    EXPECT_EQ(second.compare(0u, first.size(), first), 0);
    EXPECT_NE(second.find("row:second-append\n", first.size()), std::string::npos);
}

TEST_F(LoggingRuntimeCoverageTest, database_target_closed_by_stop_receives_no_rows_until_reopened)
{
    use_database();
    setup();
    ASSERT_TRUE(areg::start_logging());
    write_row("open-session");
    areg::stop_logging(true);
    EXPECT_FALSE(mDatabase.is_operable());
    write_row("stopped-session");
    ASSERT_TRUE(areg::start_logging());
    write_row("reopened-session");
    areg::stop_logging(true);
    EXPECT_EQ(mDatabase.rows(), (std::vector<std::string>{ "open-session", "reopened-session" }));
    EXPECT_EQ(mDatabase.mPaths.size(), 2u);
}

/**
 * \brief   Reproduces the defect: a row followed by a non-log event stayed uncommitted.
 **/
TEST_F(LoggingRuntimeCoverageTest, row_followed_by_a_trailing_non_log_event_is_committed)
{
    use_database();
    setup();
    ASSERT_TRUE(areg::start_logging());
    mDatabase.hold_row("held-row");
    write_row("held-row");
    ASSERT_TRUE(mDatabase.wait_held());

    areg::DispatcherThread& logging{ areg::DispatcherThread::dispatcher_thread(areg::crc32_calculate("_AREG_LOGGING_THREAD_")) };
    ASSERT_TRUE(logging.is_running());
    areg::Event trailing(areg::EventType::EventCustomExternal);
    trailing.set_target_dispatcher(&logging);
    ASSERT_TRUE(trailing.deliver_event());

    mDatabase.release_row();
    EXPECT_TRUE(mDatabase.wait_row("held-row", true));
    areg::stop_logging(true);
}

/**
 * \brief   Reproduces the defect: a row written right before a stop left its flush pending, so the
 *          next session flushed before its first row, and a slow flush cost that session its rows.
 **/
TEST_F(LoggingRuntimeCoverageTest, restart_has_no_flush_left_from_the_previous_session)
{
    use_database();
    setup();
    ASSERT_TRUE(areg::start_logging());
    mDatabase.hold_row("held-row");
    write_row("held-row");
    ASSERT_TRUE(mDatabase.wait_held());
    areg::stop_logging(false);
    mDatabase.release_row();
    areg::wait_logging_end();
    ASSERT_EQ(mDatabase.mCommits, 0u);

    ASSERT_TRUE(areg::start_logging());
    EXPECT_FALSE(mDatabase.wait_commit(200u));
    areg::stop_logging(true);
}

#endif // AREG_LOGGING
