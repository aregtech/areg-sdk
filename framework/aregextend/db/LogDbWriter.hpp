#ifndef AREG_AREGEXTEND_DB_LOGDBWRITER_HPP
#define AREG_AREGEXTEND_DB_LOGDBWRITER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/LogDbWriter.hpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database log file.
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
#include "areg/logging/LoggingDefs.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <vector>

namespace areg::ext {

class LogSqliteDatabase;

//////////////////////////////////////////////////////////////////////////
// LogDbWriter class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Saves log messages in a logging database on its own thread. The messages wait in a
 *          queue bounded in bytes; when it is full a new message is dropped and counted, and a
 *          row that reports the number of the dropped messages is saved once the writer catches up.
 *
 *          A row gets its identifier when it is queued. Markers and other changes of the database
 *          go through the same queue, in order with the rows. The rows are committed in batches
 *          of BATCH_MESSAGES, or BATCH_TIMEOUT_MS after the first row of a batch. The writer
 *          thread waits without a timeout while it has nothing to commit. Stopping the writer
 *          saves everything that was queued.
 **/
class LogDbWriter final : private areg::ThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal types and constants
//////////////////////////////////////////////////////////////////////////
public:
    //!< A change of the database that runs on the writer thread, in order with the rows.
    using Task = std::function<void(LogSqliteDatabase &)>;

    //!< The most log messages one database transaction holds.
    static constexpr uint32_t   BATCH_MESSAGES      { 16'384u };

    //!< The number of queued log messages that wakes the writer thread.
    static constexpr uint32_t   WAKE_MESSAGES       { 1'000u };

    //!< The most milliseconds a database transaction stays open.
    static constexpr uint32_t   BATCH_TIMEOUT_MS    { 100u };

    //!< The queue size in megabytes used when none is given.
    static constexpr uint32_t   DEFAULT_QUEUE_MB    { 256u };

    /**
     * \brief   The state of the queue, read without a lock.
     **/
    struct QueueState
    {
        uint64_t    dropped     { 0u }; //!< Log messages dropped since the writer started.
        uint32_t    pending     { 0u }; //!< Log messages queued and not saved yet.
        uint64_t    pendingBytes{ 0u }; //!< Bytes the pending log messages hold.
        uint64_t    peakBytes   { 0u }; //!< The most bytes the queue held since the writer started.
        uint64_t    limitBytes  { 0u }; //!< The size of the queue in bytes.
        bool        running     { false }; //!< True while the writer accepts log messages.
    };

private:
    /**
     * \brief   One entry of the queue: a log row with its identifier, or a task.
     **/
    struct Record
    {
        areg::MessageEnvelope   recLog  { };        //!< The log message, empty for a task.
        uint32_t                recId   { 0u };     //!< The identifier of the log row.
        std::unique_ptr<Task>   recTask { };        //!< The task, null for a log row.
    };

    //!< The bytes a queued log message holds besides its data.
    static constexpr uint32_t   RECORD_OVERHEAD     { static_cast<uint32_t>(sizeof(Record) + sizeof(areg::EventHeader)) };

    //!< The number of entries the queue is initially able to hold without reallocation.
    static constexpr uint32_t   INIT_QUEUE_ENTRIES  { 1'024u };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates a writer of the given database. The database is opened and closed by the
     *          owner of the writer, as a recorder.
     **/
    explicit LogDbWriter(LogSqliteDatabase & database);

    ~LogDbWriter() override;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Returns true while the writer accepts log messages.
     **/
    [[nodiscard]]
    inline bool is_running() const noexcept;

    /**
     * \brief   Returns the state of the queue. Thread safe, takes no lock.
     **/
    [[nodiscard]]
    QueueState queue_state() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Starts the writer thread. The database must be open as a recorder.
     *
     * \param   queueMb     The size of the queue in megabytes. Zero takes DEFAULT_QUEUE_MB.
     * \return  Returns true if the writer runs.
     **/
    bool start(uint32_t queueMb);

    /**
     * \brief   Stops accepting messages, saves and commits everything queued, and stops the writer
     *          thread. Returns when all is saved.
     **/
    void stop();

    /**
     * \brief   Queues a log message to save. The log entry is read on the writer thread, so the
     *          buffer must not change after this call.
     *
     * \param   msgLog  The log message.
     * \return  Returns the identifier the row gets in the database. Returns 0 if the writer does
     *          not run or the queue is full; a message dropped on a full queue is counted.
     **/
    uint32_t queue_log(const areg::MessageEnvelope & msgLog);

    /**
     * \brief   Queues a marker of a log row. The marker is saved after every row queued before it.
     *
     * \param   masterId    The identifier of the log row to mark.
     * \param   level       The marker level, one of the `PrioMarker*` priorities.
     * \param   author      The one who sets the marker. Can be empty.
     * \param   note        The note of the marker. Can be empty.
     * \return  Returns the identifier the marker gets. Returns 0 if the writer does not run, the
     *          level is not a marker level, or no row with that identifier was queued. A marker of
     *          a row that is missing when the writer saves it is not saved.
     **/
    uint32_t queue_marker(uint32_t masterId, areg::LogPriority level, const areg::String & author, const areg::String & note);

    /**
     * \brief   Queues a change of the database. A task is never dropped.
     *
     * \param   task    The change to run on the writer thread.
     * \return  Returns true if the task is queued; false if the writer does not run.
     **/
    bool queue_task(Task && task);

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// ThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   The writer loop. Saves the queued entries until the writer is stopped and the
     *          queue is empty.
     **/
    void on_run() override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:

    /**
     * \brief   Appends the entry to the queue and wakes the writer if it waits for one.
     *          Called with mLock taken.
     *
     * \return  Returns true if the writer must be woken.
     **/
    inline bool _push(Record && record);

    /**
     * \brief   Saves one entry of the queue. Opens a transaction if none is open and commits it
     *          when it holds BATCH_MESSAGES log messages.
     **/
    inline void _write(Record & record);

    /**
     * \brief   Opens a transaction if none is open.
     **/
    inline void _begin();

    /**
     * \brief   Commits the open transaction. If the commit fails, the transaction stays open and
     *          the next batch or timeout retries it.
     **/
    inline void _commit();

    inline LogDbWriter & self();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< The database the writer saves in.
    LogSqliteDatabase &     mDatabase;

    //!< The writer thread.
    areg::Thread            mThread;

    //!< Wakes the writer thread.
    areg::SyncEvent         mWakeEvent;

    //!< Guards the queue, the identifiers and the flags below.
    mutable areg::SpinLock  mLock;

    //!< The entries queued for the writer, guarded by mLock.
    std::vector<Record>     mQueue;

    //!< The entries taken over by the writer thread. Accessed by the writer thread only.
    std::vector<Record>     mBatch;

    //!< The identifier of the next log row, guarded by mLock.
    uint32_t                mNextLogId;

    //!< The identifier of the next marker, guarded by mLock.
    uint32_t                mNextMarkerId;

    //!< The log messages dropped since the writer saved the last report of drops, guarded by mLock.
    uint32_t                mDroppedReport;

    //!< True while the writer thread waits with no transaction open, guarded by mLock.
    bool                    mWriterIdle;

    //!< True when the writer must stop once the queue is empty, guarded by mLock.
    bool                    mQuit;

    //!< True while new entries are accepted.
    std::atomic_bool        mAccepting;

    //!< The size of the queue in bytes.
    uint64_t                mLimitBytes;

    //!< The bytes the queued log messages hold, written under mLock or by the writer thread.
    std::atomic<uint64_t>   mPendingBytes;

    //!< The number of the queued log messages not saved yet.
    std::atomic<uint32_t>   mPendingCount;

    //!< The most bytes the queue held.
    std::atomic<uint64_t>   mPeakBytes;

    //!< The log messages dropped since the writer started.
    std::atomic<uint64_t>   mDroppedTotal;

    //!< True while the writer thread holds an open transaction. Accessed by the writer thread only.
    bool                    mTxOpen;

    //!< The log messages in the open transaction. Accessed by the writer thread only.
    uint32_t                mTxRows;

    //!< When the open transaction started, in milliseconds. Accessed by the writer thread only.
    uint64_t                mTxStartMs;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    LogDbWriter() = delete;
    AREG_NOCOPY_NOMOVE(LogDbWriter);
};

//////////////////////////////////////////////////////////////////////////
// LogDbWriter class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool LogDbWriter::is_running() const noexcept
{
    return mAccepting.load(std::memory_order_acquire);
}

inline LogDbWriter & LogDbWriter::self()
{
    return (*this);
}

} // namespace areg::ext

#endif  // AREG_AREGEXTEND_DB_LOGDBWRITER_HPP
