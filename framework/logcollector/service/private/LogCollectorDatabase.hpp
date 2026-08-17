#ifndef AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDATABASE_HPP
#define AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDATABASE_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/service/private/LogCollectorDatabase.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector logging database
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/String.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"
#include "aregextend/db/LogSqliteDatabase.hpp"

#include <atomic>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// LogCollectorDatabase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Saves the collected logs in the SQLite database file (`.sqlog`).
 *
 *          The messages are handed over by the service dispatcher thread and are written by a
 *          dedicated writer thread, so that neither the database file I/O nor the SQLite commit
 *          delays the forwarding of logs to the observers. The writer drains the complete queue
 *          within one transaction, which keeps the cost per message close to a single prepared
 *          statement step.
 *
 *          The queue has an upper limit. When a burst of logs arrives faster than the database can
 *          absorb it, the newest entries are counted and dropped instead of growing the memory of
 *          the process. The number of dropped entries is written in the database as soon as the
 *          writer catches up.
 **/
class LogCollectorDatabase final : private areg::ThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   The kind of the entry queued for the writer thread.
     **/
    enum class RecordKind : uint8_t
    {
          LogMessage    //!< A log message received from a log source.
        , InstanceUp    //!< A log source is connected.
        , InstanceDown  //!< A log source is disconnected.
        , ScopeList     //!< A log source registered or updated the list of the log scopes.
    };

    /**
     * \brief   One entry of the writer queue. Only the fields relevant for the kind are set,
     *          so that queuing a log message costs one shared buffer reference and a few integers.
     **/
    struct DbRecord
    {
        RecordKind                                  recKind     { RecordKind::LogMessage };
        ITEM_ID                                     recCookie   { areg::COOKIE_UNKNOWN };
        TIME64                                      recStamp    { 0 };
        areg::MessageEnvelope                       recMessage;
        std::unique_ptr<areg::ConnectedInstance>    recInstance;
    };

    //!< The maximum number of entries kept in the queue before the newest ones are dropped.
    static constexpr uint32_t   MAX_QUEUE_ENTRIES   { 200000u };

    //!< The number of entries the queue is initially able to hold without reallocation.
    static constexpr uint32_t   INIT_QUEUE_ENTRIES  { 1024u };

    //!< The interval in milliseconds the writer wakes up when no entry was queued.
    static constexpr uint32_t   WRITER_IDLE_TIMEOUT { 1000u };

    //!< The name of the writer thread.
    static constexpr std::string_view   WRITER_THREAD_NAME  { "LogCollectorDatabaseThread" };

public:
    //!< The database file used when neither the command line nor the configuration specify one.
    static constexpr std::string_view   DEFAULT_DB_LOCATION { "./logs" };

    //!< The database file name used when the configuration does not specify one.
    static constexpr std::string_view   DEFAULT_DB_NAME     { "logcollector_%time%.sqlog" };

//////////////////////////////////////////////////////////////////////////
// Statics
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the configuration enables saving the collected logs in a database
     *          and the configured database engine is supported.
     **/
    [[nodiscard]]
    static bool is_configured();

    /**
     * \brief   Returns the database file path set in the configuration. The path may contain masks
     *          like `%time%`. Returns the built-in default if the configuration has no entry.
     **/
    [[nodiscard]]
    static areg::String configured_path();

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LogCollectorDatabase();

    virtual ~LogCollectorDatabase();

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true if the logs are currently saved in the database.
     **/
    [[nodiscard]]
    inline bool is_active() const noexcept;

    /**
     * \brief   Returns the path of the database file in use, or an empty string when inactive.
     **/
    [[nodiscard]]
    areg::String database_path() const;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Opens the database file and starts the writer thread. Has no effect if the logs are
     *          already saved in a database.
     *
     * \param   dbPath      The path of the database file. May contain masks like `%time%`. If
     *                      empty, the path is taken from the configuration.
     * \return  Returns true if the database file is open and the writer thread runs.
     **/
    bool start(const areg::String & dbPath);

    /**
     * \brief   Writes the pending entries, closes the database file and stops the writer thread.
     **/
    void stop();

    /**
     * \brief   Queues a log message received from a log source. Called by the service dispatcher.
     *
     * \param   msgReceived     The received log message.
     **/
    void save_log_message(const areg::MessageEnvelope & msgReceived);

    /**
     * \brief   Queues the information of a connected log source.
     *
     * \param   instance    The connected log source.
     **/
    void save_instance_connected(const areg::ConnectedInstance & instance);

    /**
     * \brief   Queues the information of a disconnected log source. Deactivates its log scopes.
     *
     * \param   cookie      The ID of the disconnected log source.
     **/
    void save_instance_disconnected(const ITEM_ID & cookie);

    /**
     * \brief   Queues the list of log scopes registered or updated by a log source.
     *
     * \param   cookie          The ID of the log source that owns the scopes.
     * \param   msgReceived     The message with the list of scopes, IDs and priorities.
     **/
    void save_scopes(const ITEM_ID & cookie, const areg::MessageEnvelope & msgReceived);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   The writer loop. Drains the queue into the database until the writer is stopped.
     **/
    void on_run() override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Appends the entry to the queue and wakes up the writer. Drops the entry and counts
     *          it if the queue reached the maximum size.
     *
     * \param   record      The entry to queue.
     **/
    void _enqueue(DbRecord && record);

    /**
     * \brief   Moves the queued entries in the writer buffer and writes them in one transaction.
     *
     * \return  Returns true if at least one entry was written.
     **/
    bool _drain_queue();

    /**
     * \brief   Writes a single queued entry in the database.
     *
     * \param   record      The entry to write.
     **/
    void _write_record(DbRecord & record);

    /**
     * \brief   Writes the list of scopes contained in the message of the entry.
     *
     * \param   record      The entry with the list of scopes.
     **/
    void _write_scopes(DbRecord & record);

    /**
     * \brief   Writes an entry that reports the number of the log messages dropped on overload.
     *
     * \param   dropped     The number of the dropped entries.
     **/
    void _write_dropped(uint32_t dropped);

    inline LogCollectorDatabase & self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The SQLite database of the logs.
    areg::ext::LogSqliteDatabase    mDatabase;

    //!< The thread that writes the queued entries in the database.
    areg::Thread                    mWriterThread;

    //!< Signaled when an entry is queued or when the writer should stop.
    areg::SyncEvent                 mWakeEvent;

    //!< Guards the producer queue and the counter of the dropped entries.
    mutable areg::SpinLock          mQueueLock;

    //!< The entries queued by the service dispatcher, guarded by mQueueLock.
    std::vector<DbRecord>           mQueue;

    //!< The entries taken over by the writer thread. Accessed by the writer thread only.
    std::vector<DbRecord>           mBatch;

    //!< The number of the entries dropped on overload, guarded by mQueueLock.
    uint32_t                        mDropped;

    //!< True while the logs are saved in the database.
    std::atomic_bool                mIsActive;

    //!< Set to true to stop the writer thread.
    std::atomic_bool                mQuitWriter;

    //!< Reused by the writer thread to stamp the reception time of a log message.
    areg::LogEntry                  mScratch;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LogCollectorDatabase);
};

//////////////////////////////////////////////////////////////////////////
// LogCollectorDatabase class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool LogCollectorDatabase::is_active() const noexcept
{
    return mIsActive.load(std::memory_order_relaxed);
}

inline LogCollectorDatabase & LogCollectorDatabase::self()
{
    return (*this);
}

#endif  // AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDATABASE_HPP
