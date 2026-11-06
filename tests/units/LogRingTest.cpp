/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/LogRingTest.cpp
 * \author      Artak Avetyan
 * \brief       Areg Platform, tests of the remote log queue of a log source that is not
 *              connected to the log collector yet. The test plays the log collector.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/SocketDefs.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/ipc/RemoteServiceDefs.hpp"
#include "areg/logging/LogDatabaseEngine.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "areg/persist/ConfigListener.hpp"
#include "areg/persist/ConfigManager.hpp"

#if AREG_LOGGING

#if defined(_WIN32)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else   // !defined(_WIN32)
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <sys/select.h>
    #include <sys/socket.h>
#endif  // defined(_WIN32)

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace
{
    //!< Milliseconds the test waits for the log source before it fails.
    constexpr int32_t   WAIT_MS     { 30'000 };

    //!< The size of the remote log queue of the source.
    constexpr uint32_t  QUEUE_SIZE  { 100u };

    //!< The rows the source writes before the start while the logging thread runs freely.
    constexpr uint32_t  BEFORE      { 250u };

    //!< All rows the source writes before the start; the ones after BEFORE wait in the logging thread.
    constexpr uint32_t  ROWS        { 500u };

    //!< The rows dropped from the full remote log queue, the row the logging thread holds included.
    constexpr uint32_t  DROPPED     { BEFORE - QUEUE_SIZE + 1u };

    //!< The cookie the test assigns to the source.
    constexpr ITEM_ID   COOKIE      { 300u };

    //!< The port of the listening socket of the test.
    uint16_t            _port       { 0u };

    //!< Points the logger of the process at the test and sets the size of its remote log queue.
    class RingConfigListener final : public areg::ConfigListener
    {
    public:
        //!< Sets the remote log queue to 'queueSize' log messages.
        explicit RingConfigListener(uint32_t queueSize)
            : mQueueSize(queueSize)
        {
        }

        //!< Sets the logger values of the test in the configuration.
        void apply(areg::ConfigManager& config) const
        {
            config.set_service_enable(areg::RemoteServiceKind::Router, areg::ConnectionType::Tcpip, false, true);
            config.set_service_enable(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, true, true);
            config.set_service_address(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, areg::String("127.0.0.1"), true);
            config.set_service_port(areg::RemoteServiceKind::Logger, areg::ConnectionType::Tcpip, _port, true);
            config.set_log_enabled(areg::LogTarget::Remote, true, true);
            config.set_log_enabled(areg::LogTarget::File, false, true);
            config.set_log_enabled(areg::LogTarget::Database, true, true);
            config.set_remote_queue_size(mQueueSize, true);
        }

    protected:
        void prepare_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void post_save_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void prepare_read_configuration(areg::ConfigManager& /*config*/) final
        {
        }

        void on_setup_configuration(const areg::ListProperties& /*listReadonly*/, const areg::ListProperties& /*listWritable*/, areg::ConfigManager& /*config*/) final
        {
        }

        void post_read_configuration(areg::ConfigManager& config) final
        {
            apply(config);
        }

    private:
        //!< The size of the remote log queue.
        const uint32_t  mQueueSize;
    };

    //!< A log database that keeps nothing. It is given each row in the logging thread before the remote
    //!< queue. It signals when row BEFORE - 1 is handled and holds the logging thread on row BEFORE.
    class RowsHandled final : public areg::LogDatabaseEngine
    {
    public:
        //!< Signaled when the logging thread has handled the rows before BEFORE.
        areg::SyncEvent reached{ true, false };

        //!< Signaled when the logging thread is held on row BEFORE.
        areg::SyncEvent held{ true, false };

        //!< Lets the held logging thread go on.
        areg::SyncEvent resume{ true, false };

        bool is_operable() const noexcept final { return mOpened.load(std::memory_order_acquire); }
        bool connect(const areg::String & /*dbPath*/, areg::DatabaseRole /*role*/) final { mOpened.store(true, std::memory_order_release); return true; }
        void disconnect() final { mOpened.store(false, std::memory_order_release); }
        bool execute(const areg::String & /*sql*/) final { return true; }
        bool begin() final { return true; }
        bool commit(bool /*doCommit*/) final { return true; }
        bool rollback() final { return true; }
        bool tables_initialized() const noexcept final { return true; }

        bool log_message(const areg::LogEntry & message) final
        {
            constexpr char prefix[]{ "ring row " };
            if (std::strncmp(message.logMessage, prefix, sizeof(prefix) - 1u) != 0)
                return true;

            ++mRows;
            if (mRows == BEFORE)
            {
                reached.set_signaled();
            }
            else if (mRows == BEFORE + 1u)
            {
                held.set_signaled();
                resume.lock(static_cast<uint32_t>(WAIT_MS));
            }

            return true;
        }

        bool log_instance_connected(const areg::ConnectedInstance & /*instance*/, const areg::DateTime & /*timestamp*/) final { return true; }
        bool log_instance_disconnected(const ITEM_ID & /*cookie*/, const areg::DateTime & /*timestamp*/) final { return true; }
        bool log_scope_activate(const areg::ScopeEntry & /*scope*/, const ITEM_ID & /*cookie*/, const areg::DateTime & /*timestamp*/) final { return true; }
        bool log_scope_activate(const areg::String & /*scopeName*/, uint32_t /*scopeId*/, uint32_t /*scopePrio*/, const ITEM_ID & /*cookie*/, const areg::DateTime & /*timestamp*/) final { return true; }
        uint32_t log_scopes_activate(const areg::ScopeNames & /*scopes*/, const ITEM_ID & /*cookie*/, const areg::DateTime & /*timestamp*/) final { return 0u; }
        bool log_scopes_deactivate(const ITEM_ID & /*cookie*/, const areg::DateTime & /*timestamp*/) final { return true; }
        bool log_scope_deactivate(const ITEM_ID & /*cookie*/, uint32_t /*scopeId*/, const areg::DateTime & /*timestamp*/) final { return true; }

    private:
        //!< True while the logger has the database open.
        std::atomic<bool>   mOpened{ false };

        //!< The rows of the test the logging thread has handled, touched by the logging thread only.
        uint32_t            mRows{ 0u };
    };

    //!< Waits until the socket is readable. Returns false on timeout or error.
    bool wait_readable(SOCKETHANDLE hSocket, int32_t timeoutMs)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(hSocket, &reads);
        timeval timeout{ timeoutMs / 1'000, (timeoutMs % 1'000) * 1'000 };
        return (::select(static_cast<int>(hSocket) + 1, &reads, nullptr, nullptr, &timeout) > 0);
    }

    //!< Reads exactly length bytes. Returns false on timeout, error or end of stream.
    bool read_exact(SOCKETHANDLE hSocket, uint8_t * buffer, uint32_t length)
    {
        uint32_t done{ 0u };
        while (done < length)
        {
            if (wait_readable(hSocket, WAIT_MS) == false)
                return false;

            const int received{ static_cast<int>(::recv(hSocket, reinterpret_cast<char *>(buffer + done), static_cast<int>(length - done), 0)) };
            if (received <= 0)
                return false;

            done += static_cast<uint32_t>(received);
        }

        return true;
    }

    //!< Reads one message, its header and its payload.
    bool read_message(SOCKETHANDLE hSocket, areg::EventHeader & hdr, std::vector<uint8_t> & payload)
    {
        if (read_exact(hSocket, areg::wire_header(hdr), areg::WIRE_HEADER_SIZE) == false)
            return false;

        payload.resize(hdr.bufHeader.biUsed);
        return payload.empty() || read_exact(hSocket, payload.data(), hdr.bufHeader.biUsed);
    }

    //!< Writes the wire bytes of the message. Returns false on error.
    bool write_message(SOCKETHANDLE hSocket, areg::MessageEnvelope & message)
    {
        const areg::EventHeader * hdr{ message.header() };
        const uint8_t * bytes{ areg::wire_data(*hdr) };
        const uint32_t length{ areg::wire_size(*hdr) };
        uint32_t done{ 0u };
        while (done < length)
        {
            const int sent{ static_cast<int>(::send(hSocket, reinterpret_cast<const char *>(bytes + done), static_cast<int>(length - done), 0)) };
            if (sent <= 0)
                return false;

            done += static_cast<uint32_t>(sent);
        }

        return true;
    }

    //!< Returns the text of a log row, or an empty string if the payload is not a log row.
    std::string row_text(const areg::EventHeader & hdr, const std::vector<uint8_t> & payload)
    {
        constexpr std::size_t textAt{ offsetof(areg::LogEntry, logMessage) };
        if ((hdr.messageId != static_cast<uint32_t>(areg::FuncIdRange::ServiceLogMessage)) || (payload.size() <= textAt))
            return std::string();

        const char * text{ reinterpret_cast<const char *>(payload.data() + textAt) };
        return std::string(text, ::strnlen(text, payload.size() - textAt));
    }

    //!< Reads the decimal number that follows 'prefix' at the start of 'text'. Returns false if there is none.
    bool read_number(const std::string & text, const char * prefix, unsigned long long & number)
    {
        const std::size_t length{ std::strlen(prefix) };
        if (text.compare(0, length, prefix) != 0)
            return false;

        char * end{ nullptr };
        number = std::strtoull(text.c_str() + length, &end, 10);
        return (end != text.c_str() + length);
    }

    //!< A listening socket of the loopback interface on a free port.
    struct Listener
    {
        SOCKETHANDLE    socket  { areg::InvalidSocketHandle };
        SOCKETHANDLE    client  { areg::InvalidSocketHandle };

        Listener()
        {
            socket = areg::socket_create();
            sockaddr_in bound{ };
            bound.sin_family = AF_INET;
            bound.sin_port   = 0u;
            ::inet_pton(AF_INET, "127.0.0.1", &bound.sin_addr);
            socklen_t length{ sizeof(bound) };
            if ( areg::is_valid_socket(socket)
              && (::bind(socket, reinterpret_cast<sockaddr *>(&bound), sizeof(bound)) == 0)
              && (::listen(socket, 4) == 0)
              && (::getsockname(socket, reinterpret_cast<sockaddr *>(&bound), &length) == 0) )
            {
                _port = ntohs(bound.sin_port);
            }
        }

        ~Listener()
        {
            if (areg::is_valid_socket(client))
                areg::socket_close(client);

            if (areg::is_valid_socket(socket))
                areg::socket_close(socket);
        }

        //!< Accepts the connection of the log source.
        bool accept()
        {
            if (wait_readable(socket, WAIT_MS) == false)
                return false;

            client = static_cast<SOCKETHANDLE>(::accept(socket, nullptr, nullptr));
            return areg::is_valid_socket(client);
        }
    };
    //!< Writes the rows numbered from 'first' to 'last' - 1 through the logger of the process.
    void log_rows(uint32_t first, uint32_t last)
    {
        for (uint32_t i = first; i < last; ++i)
        {
            char text[64]{ };
            const int32_t len{ areg::String::format_string(text, static_cast<int32_t>(sizeof(text)), "ring row %u", i) };
            const areg::LogEntry row(areg::LogMessageType::MessageText, areg::LOG_SCOPE_ID_NONE, 0u, 0u, areg::LogPriority::PrioDebug, text, static_cast<uint32_t>(len));
            areg::log_any_message(row);
        }
    }

    //!< What the test received from the source after the start.
    struct Received
    {
        bool                    scopesFirst { false };  //!< The scopes came before the first row.
        bool                    notice      { false };  //!< The dropped notice came.
        bool                    noticeFirst { false };  //!< The dropped notice came after the scopes and before the rows.
        uint64_t                dropped     { 0u };     //!< The number in the dropped notice.
        std::vector<uint32_t>   rows;                   //!< The numbers of the received rows, in order.
    };

    //!< Reads the messages of the source until 'count' rows are received or the stream ends.
    Received read_logs(SOCKETHANDLE hSocket, uint32_t count)
    {
        Received result;
        bool scopes{ false };
        areg::EventHeader hdr{ };
        std::vector<uint8_t> payload;
        while ((result.rows.size() < count) && read_message(hSocket, hdr, payload))
        {
            if (hdr.messageId == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes))
            {
                scopes = true;
                continue;
            }

            const std::string text{ row_text(hdr, payload) };
            unsigned long long number{ 0u };
            if (read_number(text, "Dropped [ ", number))
            {
                result.notice = true;
                result.noticeFirst = scopes && result.rows.empty();
                result.dropped = number;
            }
            else if (read_number(text, "ring row ", number))
            {
                result.scopesFirst = result.rows.empty() ? scopes : result.scopesFirst;
                result.rows.push_back(static_cast<uint32_t>(number));
            }
        }

        return result;
    }

    //!< Starts the logger of the process, accepts its connection and reads its handshake.
    bool start_source(Listener & listener, RowsHandled & database, RingConfigListener & config)
    {
        // A process configured by an earlier test does not read the configuration again.
        if (areg::Application::is_configured())
        {
            config.apply(areg::Application::config_manager());
        }

        areg::set_db_engine(&database);
        areg::Application::setup(true, true, false, true, false, nullptr, &config);

        areg::EventHeader hdr{ };
        std::vector<uint8_t> payload;
        return listener.accept() && read_message(listener.client, hdr, payload);
    }

    //!< Stops the logger of the process.
    void stop_source(RowsHandled & database)
    {
        database.resume.set_signaled();
        areg::Application::release();
        areg::set_db_engine(nullptr);
    }

    //!< Plays the collector of a source with a remote log queue of 'queueSize'. The source writes
    //!< BEFORE rows, then the others while its logging thread is held on one; then it is started.
    //!< Returns what the source sent after the start.
    Received run_source(uint32_t queueSize)
    {
        Listener listener;
        EXPECT_NE(_port, 0u);

        RowsHandled database;
        RingConfigListener config(queueSize);
        const bool connected{ start_source(listener, database, config) };
        EXPECT_TRUE(connected);
        EXPECT_EQ(areg::cookie(), areg::COOKIE_UNKNOWN);

        // The logging thread keeps up with the first rows.
        log_rows(0u, BEFORE);
        const bool reached{ database.reached.lock(static_cast<uint32_t>(WAIT_MS)) };
        EXPECT_TRUE(reached);

        // A slow logging thread: it holds one row, the others wait in its queue.
        log_rows(BEFORE, ROWS);
        const bool held{ reached && database.held.lock(static_cast<uint32_t>(WAIT_MS)) };
        EXPECT_TRUE(held);

        // The receive thread answers the query itself, after it handled the connect notify sent before it.
        areg::MessageEnvelope answer{ areg::create_connect_notify(areg::COOKIE_LOGGER, COOKIE) };
        areg::MessageEnvelope query{ areg::message_query_scopes(areg::COOKIE_LOGGER, COOKIE) };
        areg::EventHeader hdr{ };
        std::vector<uint8_t> payload;
        const bool answered{ connected && held && write_message(listener.client, answer) && write_message(listener.client, query) };
        EXPECT_TRUE(answered);
        const bool started{ answered && read_message(listener.client, hdr, payload) && (hdr.messageId == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogRegisterScopes)) };
        EXPECT_TRUE(started);

        database.resume.set_signaled();
        const uint32_t expected{ queueSize >= ROWS ? ROWS : ROWS - DROPPED };
        const Received received{ started ? read_logs(listener.client, expected) : Received{ } };
        EXPECT_EQ(areg::cookie(), COOKIE);
        stop_source(database);
        return received;
    }
}

// Logs written before the connection to the log collector is started are kept up to the size of the
// remote log queue. The older ones are counted and reported in one row, sent after the scopes and
// before the kept logs. Logs still waiting in the logging thread at the start are all sent after them.
// The cookie of the source is reported only once the logs are sent directly.
TEST(LogRingTest, logs_before_the_start_are_counted_and_reported)
{
    ASSERT_TRUE(areg::socket_initialize());
    const Received received{ run_source(QUEUE_SIZE) };
    EXPECT_TRUE(received.noticeFirst);
    EXPECT_EQ(received.dropped, static_cast<uint64_t>(DROPPED));
    ASSERT_EQ(received.rows.size(), static_cast<std::size_t>(ROWS - DROPPED));
    for (uint32_t i = 0u; i < ROWS - DROPPED; ++i)
    {
        EXPECT_EQ(received.rows[i], DROPPED + i);
    }

    areg::socket_release();
}

// A remote log queue that holds every log written before the start loses none: no dropped notice,
// every log sent once, in order, after the scopes. Also those waiting in the logging thread at the start.
TEST(LogRingTest, logs_before_the_start_are_all_sent_if_the_queue_holds_them)
{
    ASSERT_TRUE(areg::socket_initialize());
    const Received received{ run_source(ROWS) };
    EXPECT_TRUE(received.scopesFirst);
    EXPECT_FALSE(received.notice);
    ASSERT_EQ(received.rows.size(), static_cast<std::size_t>(ROWS));
    for (uint32_t i = 0u; i < ROWS; ++i)
    {
        EXPECT_EQ(received.rows[i], i);
    }

    areg::socket_release();
}

// Reproduces the defect: a scope update that promises more entries than it holds kept the receive
// thread of the log source in the loop for minutes, reading past the end of the message.
TEST(LogRingTest, scope_update_with_a_forged_count_ends_with_the_message)
{
    ASSERT_TRUE(areg::socket_initialize());
    Listener listener;
    ASSERT_NE(_port, 0u);

    RowsHandled database;
    RingConfigListener config(QUEUE_SIZE);
    ASSERT_TRUE(start_source(listener, database, config));

    areg::MessageEnvelope answer{ areg::create_connect_notify(areg::COOKIE_LOGGER, COOKIE) };
    areg::ScopeNames scopes;
    scopes.add(areg::ScopeEntry(areg::String("log_ring_test_scope"), 1u, static_cast<uint32_t>(areg::LogPriority::PrioDebug)));
    areg::MessageEnvelope update{ areg::message_update_scopes(areg::COOKIE_LOGGER, COOKIE, scopes) };
    update.move_to_begin();
    update << 0xFFFF'FFFFu;
    update.move_to_end();
    ASSERT_TRUE(write_message(listener.client, answer));
    ASSERT_TRUE(write_message(listener.client, update));

    bool updated{ false };
    areg::EventHeader hdr{ };
    std::vector<uint8_t> payload;
    while ((updated == false) && read_message(listener.client, hdr, payload))
    {
        updated = (hdr.messageId == static_cast<uint32_t>(areg::FuncIdRange::ServiceLogScopesUpdated));
    }

    EXPECT_TRUE(updated);
    stop_source(database);
    areg::socket_release();
}

#endif  // AREG_LOGGING
