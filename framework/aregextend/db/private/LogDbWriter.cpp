/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        aregextend/db/private/LogDbWriter.cpp
 * \author      Artak Avetyan
 * \ingroup     Areg platform, extended library, SQLite Database log file.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "aregextend/db/LogDbWriter.hpp"

#include "aregextend/db/LogSqliteDatabase.hpp"
#include "areg/base/DateTime.hpp"

namespace {
    //! The number given to the next writer thread, so that each thread has its own name.
    std::atomic<uint32_t>   _writerThreadSeq    { 0u };

    constexpr uint64_t      _oneMegabyte        { 1024u * 1024u };
}

namespace areg::ext {

LogDbWriter::LogDbWriter(LogSqliteDatabase & database)
    : areg::ThreadConsumer  ( )

    , mDatabase     ( database )
    , mThread       ( static_cast<areg::ThreadConsumer &>(self())
                    , String("LogDbWriter_") + String::make_string(_writerThreadSeq.fetch_add(1u, std::memory_order_relaxed))
                    , areg::SYSTEM_THREAD_STACK_BIG )
    , mWakeEvent    ( true, true )
    , mLock         ( )
    , mQueue        ( )
    , mBatch        ( )
    , mNextLogId    ( 1u )
    , mNextMarkerId ( 1u )
    , mDroppedReport( 0u )
    , mWriterIdle   ( false )
    , mQuit         ( false )
    , mAccepting    ( false )
    , mLimitBytes   ( static_cast<uint64_t>(LogDbWriter::DEFAULT_QUEUE_MB) * _oneMegabyte )
    , mPendingBytes ( 0u )
    , mPendingCount ( 0u )
    , mPeakBytes    ( 0u )
    , mDroppedTotal ( 0u )
    , mTxOpen       ( false )
    , mTxRows       ( 0u )
    , mTxStartMs    ( 0u )
{
}

LogDbWriter::~LogDbWriter()
{
    stop();
}

LogDbWriter::QueueState LogDbWriter::queue_state() const noexcept
{
    QueueState state;
    state.dropped       = mDroppedTotal.load(std::memory_order_relaxed);
    state.pending       = mPendingCount.load(std::memory_order_relaxed);
    state.pendingBytes  = mPendingBytes.load(std::memory_order_relaxed);
    state.peakBytes     = mPeakBytes.load(std::memory_order_relaxed);
    state.limitBytes    = mLimitBytes;
    state.running       = mAccepting.load(std::memory_order_relaxed);
    return state;
}

bool LogDbWriter::start(uint32_t queueMb)
{
    if (is_running())
        return true;

    if (mDatabase.is_operable() == false)
        return false;

    const uint32_t nextLogId{ mDatabase.max_log_id() + 1u };
    const uint32_t nextMarkerId{ mDatabase.max_marker_id() + 1u };

    do
    {
        areg::Lock lock(mLock);
        mQueue.clear();
        mQueue.reserve(LogDbWriter::INIT_QUEUE_ENTRIES);
        mBatch.clear();
        mBatch.reserve(LogDbWriter::INIT_QUEUE_ENTRIES);
        mNextLogId      = nextLogId;
        mNextMarkerId   = nextMarkerId;
        mDroppedReport  = 0u;
        mWriterIdle     = false;
        mQuit           = false;
        mLimitBytes     = static_cast<uint64_t>(queueMb != 0u ? queueMb : LogDbWriter::DEFAULT_QUEUE_MB) * _oneMegabyte;
        mPendingBytes.store(0u, std::memory_order_relaxed);
        mPendingCount.store(0u, std::memory_order_relaxed);
        mPeakBytes.store(0u, std::memory_order_relaxed);
        mDroppedTotal.store(0u, std::memory_order_relaxed);
        mAccepting.store(true, std::memory_order_release);
    } while (false);

    mTxOpen     = false;
    mTxRows     = 0u;
    mTxStartMs  = 0u;

    if (mThread.start(areg::WAIT_INFINITE))
        return true;

    areg::Lock lock(mLock);
    mAccepting.store(false, std::memory_order_release);
    mQueue.clear();
    return false;
}

void LogDbWriter::stop()
{
    do
    {
        areg::Lock lock(mLock);
        mAccepting.store(false, std::memory_order_release);
        mQuit = true;
    } while (false);

    mWakeEvent.set_signaled();
    mThread.shutdown(areg::WAIT_INFINITE);

    areg::Lock lock(mLock);
    mQueue.clear();
    mPendingBytes.store(0u, std::memory_order_relaxed);
    mPendingCount.store(0u, std::memory_order_relaxed);
}

uint32_t LogDbWriter::queue_log(const areg::MessageEnvelope & msgLog)
{
    if (mAccepting.load(std::memory_order_relaxed) == false)
        return 0u;

    const uint64_t bytes{ static_cast<uint64_t>(LogDbWriter::RECORD_OVERHEAD) + msgLog.size_used() };
    uint32_t logId{ 0u };
    bool wake{ false };

    do
    {
        areg::Lock lock(mLock);
        if (mAccepting.load(std::memory_order_relaxed) == false)
            return 0u;

        const uint64_t pending{ mPendingBytes.load(std::memory_order_relaxed) + bytes };
        if (pending > mLimitBytes)
        {
            ++ mDroppedReport;
            mDroppedTotal.fetch_add(1u, std::memory_order_relaxed);
            return 0u;
        }

        logId = mNextLogId ++;
        mPendingBytes.fetch_add(bytes, std::memory_order_relaxed);
        mPendingCount.fetch_add(1u, std::memory_order_relaxed);
        if (pending > mPeakBytes.load(std::memory_order_relaxed))
        {
            mPeakBytes.store(pending, std::memory_order_relaxed);
        }

        Record record;
        record.recLog   = msgLog;
        record.recId    = logId;
        wake = _push(std::move(record));
    } while (false);

    if (wake)
    {
        mWakeEvent.set_signaled();
    }

    return logId;
}

uint32_t LogDbWriter::queue_marker(uint32_t masterId, areg::LogPriority level, const areg::String & author, const areg::String & note)
{
    if ((masterId == 0u) || (LogSqliteDatabase::is_marker_level(level) == false))
        return 0u;

    const TIME64 timeSet{ static_cast<TIME64>(areg::DateTime::now().time()) };
    uint32_t markerId{ 0u };
    bool wake{ false };

    do
    {
        areg::Lock lock(mLock);
        if ((mAccepting.load(std::memory_order_relaxed) == false) || (masterId >= mNextLogId))
            return 0u;

        markerId = mNextMarkerId ++;
        Record record;
        record.recTask = std::make_unique<Task>([markerId, masterId, level, author, note, timeSet](LogSqliteDatabase & database)
            {
                database.insert_marker(markerId, masterId, level, author, note, timeSet);
            });

        wake = _push(std::move(record));
    } while (false);

    if (wake)
    {
        mWakeEvent.set_signaled();
    }

    return markerId;
}

bool LogDbWriter::queue_task(Task && task)
{
    if (task == nullptr)
        return false;

    bool wake{ false };

    do
    {
        areg::Lock lock(mLock);
        if (mAccepting.load(std::memory_order_relaxed) == false)
            return false;

        Record record;
        record.recTask = std::make_unique<Task>(std::move(task));
        wake = _push(std::move(record));
    } while (false);

    if (wake)
    {
        mWakeEvent.set_signaled();
    }

    return true;
}

inline bool LogDbWriter::_push(Record && record)
{
    mQueue.push_back(std::move(record));
    const size_t size{ mQueue.size() };
    return (mWriterIdle && (size == 1u)) || (size == static_cast<size_t>(LogDbWriter::WAKE_MESSAGES));
}

void LogDbWriter::on_run()
{
    for ( ; ; )
    {
        uint32_t dropped{ 0u };
        uint32_t droppedId{ 0u };
        bool quit{ false };

        do
        {
            areg::Lock lock(mLock);
            mBatch.swap(mQueue);
            dropped = mDroppedReport;
            mDroppedReport = 0u;
            droppedId = (dropped != 0u) ? mNextLogId ++ : 0u;
            quit = mQuit;
            mWriterIdle = mBatch.empty() && (dropped == 0u) && (mTxOpen == false);
        } while (false);

        if (mBatch.empty() && (dropped == 0u))
        {
            if (quit)
                break;

            if (mTxOpen == false)
            {
                mWakeEvent.lock(areg::WAIT_INFINITE);
                continue;
            }

            const uint64_t elapsed{ areg::DateTime::system_tick_count() - mTxStartMs };
            if ((elapsed < LogDbWriter::BATCH_TIMEOUT_MS) && mWakeEvent.lock(static_cast<uint32_t>(LogDbWriter::BATCH_TIMEOUT_MS - elapsed)))
                continue;

            _commit();
            continue;
        }

        for (Record & record : mBatch)
        {
            _write(record);
        }

        mBatch.clear();
        if (dropped != 0u)
        {
            _begin();
            mDatabase.log_dropped_messages(dropped, droppedId);
        }

        if (mTxOpen && ((areg::DateTime::system_tick_count() - mTxStartMs) >= LogDbWriter::BATCH_TIMEOUT_MS))
        {
            _commit();
        }

        if (mBatch.capacity() > (LogDbWriter::INIT_QUEUE_ENTRIES * 64u))
        {
            std::vector<Record>().swap(mBatch);
        }
    }

    _commit();
}

inline void LogDbWriter::_write(Record & record)
{
    _begin();
    if (record.recTask != nullptr)
    {
        (*record.recTask)(mDatabase);
        record.recTask.reset();
        return;
    }

    const uint8_t* data{ record.recLog.buffer() };
    if (data != nullptr)
    {
        mDatabase.log_message(*reinterpret_cast<const areg::LogEntry *>(data), record.recId);
    }

    mPendingBytes.fetch_sub(static_cast<uint64_t>(LogDbWriter::RECORD_OVERHEAD) + record.recLog.size_used(), std::memory_order_relaxed);
    mPendingCount.fetch_sub(1u, std::memory_order_relaxed);
    record.recLog = areg::MessageEnvelope();
    if (++ mTxRows >= LogDbWriter::BATCH_MESSAGES)
    {
        _commit();
    }
}

inline void LogDbWriter::_begin()
{
    if (mTxOpen)
        return;

    mTxOpen     = mDatabase.begin();
    mTxRows     = 0u;
    mTxStartMs  = areg::DateTime::system_tick_count();
}

inline void LogDbWriter::_commit()
{
    if (mTxOpen == false)
        return;

    if (mDatabase.commit(true) || (mDatabase.is_operable() == false))
    {
        mTxOpen = false;
    }
    else
    {
        // Opens a transaction only if the failed commit ended the old one.
        static_cast<void>(mDatabase.begin());
        mTxStartMs = areg::DateTime::system_tick_count();
    }

    mTxRows = 0u;
}

} // namespace areg::ext
