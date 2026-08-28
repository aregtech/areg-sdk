/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/service/private/LogCollectorDatabase.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector logging database
 ************************************************************************/
#include "logcollector/service/private/LogCollectorDatabase.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/File.hpp"
#include "areg/base/MemoryDefs.hpp"
#include "areg/base/Process.hpp"
#include "areg/logging/LogConfiguration.hpp"

//////////////////////////////////////////////////////////////////////////
// LogCollectorDatabase class implementation
//////////////////////////////////////////////////////////////////////////

bool LogCollectorDatabase::is_configured()
{
    areg::LogConfiguration config;
    if (config.is_db_logging_enabled() == false)
        return false;

    const areg::String engine{ config.database_engine() };
    return (engine.is_empty() || (engine == areg::LOGDB_ENGINE_NAME));
}

areg::String LogCollectorDatabase::configured_path()
{
    areg::LogConfiguration config;
    areg::String location{ config.database_location() };
    areg::String name{ config.database_name() };

    if (location.is_empty())
    {
        location = LogCollectorDatabase::DEFAULT_DB_LOCATION;
    }

    if (name.is_empty())
    {
        name = LogCollectorDatabase::DEFAULT_DB_NAME;
    }

    return areg::File::make_full_path(location, name);
}

LogCollectorDatabase::LogCollectorDatabase()
    : areg::ThreadConsumer  ( )

    , mDatabase     ( )
    // The writer thread runs SQLite. On POSIX the stack size is a hard upper bound
    // (pthread_attr_setstacksize), on Windows it is only the initial commit.
    , mWriterThread ( static_cast<areg::ThreadConsumer &>(self()), areg::String(LogCollectorDatabase::WRITER_THREAD_NAME), areg::SYSTEM_THREAD_STACK_BIG )
    , mWakeEvent    ( true, true )
    , mQueueLock    ( )
    , mQueue        ( )
    , mBatch        ( )
    , mDropped      ( 0u )
    , mIsActive     ( false )
    , mQuitWriter   ( false )
    , mScratch      ( )
{
}

LogCollectorDatabase::~LogCollectorDatabase()
{
    stop();
}

areg::String LogCollectorDatabase::database_path() const
{
    return (is_active() ? mDatabase.database_path() : areg::String::empty_string());
}

bool LogCollectorDatabase::start(const areg::String & dbPath)
{
    if (is_active())
        return true;

    areg::String path{ dbPath.is_empty() ? LogCollectorDatabase::configured_path() : dbPath };
    if (path.is_empty())
        return false;

    mDatabase.set_database_logging_enabled(true);
    if (mDatabase.connect(path, false) == false)
    {
        mDatabase.disconnect();
        return false;
    }

    do
    {
        areg::Lock lock(mQueueLock);
        mQueue.clear();
        mQueue.reserve(LogCollectorDatabase::INIT_QUEUE_ENTRIES);
        mBatch.clear();
        mBatch.reserve(LogCollectorDatabase::INIT_QUEUE_ENTRIES);
        mDropped = 0u;
    } while (false);

    mQuitWriter.store(false, std::memory_order_relaxed);
    if (mWriterThread.start(areg::WAIT_INFINITE) == false)
    {
        mDatabase.disconnect();
        return false;
    }

    mIsActive.store(true, std::memory_order_release);
    return true;
}

void LogCollectorDatabase::stop()
{
    if (mIsActive.exchange(false, std::memory_order_acq_rel) == false)
        return;

    mQuitWriter.store(true, std::memory_order_relaxed);
    mWakeEvent.set_signaled();
    mWriterThread.shutdown(areg::WAIT_INFINITE);

    mDatabase.disconnect();

    areg::Lock lock(mQueueLock);
    mQueue.clear();
    mDropped = 0u;
}

void LogCollectorDatabase::save_log_message(const areg::MessageEnvelope & msgReceived)
{
    if (is_active() == false)
        return;

    DbRecord record;
    record.recKind      = RecordKind::LogMessage;
    record.recStamp     = static_cast<TIME64>(areg::DateTime::now());
    record.recMessage   = msgReceived;
    _enqueue(std::move(record));
}

void LogCollectorDatabase::save_instance_connected(const areg::ConnectedInstance & instance)
{
    if (is_active() == false)
        return;

    DbRecord record;
    record.recKind      = RecordKind::InstanceUp;
    record.recCookie    = instance.ciCookie;
    record.recStamp     = static_cast<TIME64>(areg::DateTime::now());
    record.recInstance  = std::make_unique<areg::ConnectedInstance>(instance);
    _enqueue(std::move(record));
}

void LogCollectorDatabase::save_instance_disconnected(const ITEM_ID & cookie)
{
    if (is_active() == false)
        return;

    DbRecord record;
    record.recKind  = RecordKind::InstanceDown;
    record.recCookie= cookie;
    record.recStamp = static_cast<TIME64>(areg::DateTime::now());
    _enqueue(std::move(record));
}

void LogCollectorDatabase::save_scopes(const ITEM_ID & cookie, const areg::MessageEnvelope & msgReceived)
{
    if (is_active() == false)
        return;

    // The scope list is parsed by the writer thread. Clone the message, so that moving the read
    // position of the buffer does not interfere with forwarding the same message to the observers.
    DbRecord record;
    record.recKind      = RecordKind::ScopeList;
    record.recCookie    = cookie;
    record.recStamp     = static_cast<TIME64>(areg::DateTime::now());
    record.recMessage   = msgReceived.clone();
    _enqueue(std::move(record));
}

void LogCollectorDatabase::on_run()
{
    while (mQuitWriter.load(std::memory_order_relaxed) == false)
    {
        mWakeEvent.lock(LogCollectorDatabase::WRITER_IDLE_TIMEOUT);
        while (_drain_queue())
        {
        }
    }

    // The service is already disconnected here, write what is left in the queue.
    while (_drain_queue())
    {
    }
}

void LogCollectorDatabase::_enqueue(DbRecord && record)
{
    bool wake{ false };

    do
    {
        areg::Lock lock(mQueueLock);
        if (mQueue.size() >= LogCollectorDatabase::MAX_QUEUE_ENTRIES)
        {
            ++ mDropped;
            return;
        }

        mQueue.push_back(std::move(record));
        wake = (mQueue.size() == 1u);
    } while (false);

    if (wake)
    {
        mWakeEvent.set_signaled();
    }
}

bool LogCollectorDatabase::_drain_queue()
{
    uint32_t dropped{ 0u };

    do
    {
        areg::Lock lock(mQueueLock);
        mBatch.swap(mQueue);
        dropped = mDropped;
        mDropped = 0u;
    } while (false);

    if (mBatch.empty() && (dropped == 0u))
        return false;

    mDatabase.begin();

    for (DbRecord & record : mBatch)
    {
        _write_record(record);
    }

    if (dropped != 0u)
    {
        _write_dropped(dropped);
    }

    mDatabase.commit(true);
    mBatch.clear();

    return true;
}

void LogCollectorDatabase::_write_record(DbRecord & record)
{
    switch (record.recKind)
    {
    case RecordKind::LogMessage:
    {
        const uint8_t * data = record.recMessage.buffer();
        if ((data != nullptr) && (record.recMessage.size_used() >= areg::log_entry_head()))
        {
            areg::mem_copy(&mScratch, sizeof(areg::LogEntry), data, record.recMessage.size_used());
            mScratch.logReceived = record.recStamp;
            mDatabase.log_message(mScratch);
        }
    }
    break;

    case RecordKind::InstanceUp:
        if (record.recInstance)
        {
            mDatabase.log_instance_connected(*record.recInstance, areg::DateTime(record.recStamp));
        }
        break;

    case RecordKind::InstanceDown:
        mDatabase.log_instance_disconnected(record.recCookie, areg::DateTime(record.recStamp));
        break;

    case RecordKind::ScopeList:
        _write_scopes(record);
        break;

    default:
        break;
    }
}

void LogCollectorDatabase::_write_scopes(DbRecord & record)
{
    areg::MessageEnvelope & msg = record.recMessage;
    if (msg.is_valid() == false)
        return;

    const areg::DateTime stamp(record.recStamp);
    mDatabase.log_scopes_deactivate(record.recCookie, stamp);

    msg.move_to_begin();
    uint32_t count{ 0u };
    msg >> count;

    for (uint32_t i = 0u; i < count; ++ i)
    {
        const uint32_t scopeId  { msg.read32_bits() };
        const uint32_t scopePrio{ msg.read32_bits() };
        const areg::String scopeName(msg);
        if (scopeName.is_empty())
            break;

        mDatabase.log_scope_activate(scopeName, scopeId, scopePrio, record.recCookie, stamp);
    }
}

void LogCollectorDatabase::_write_dropped(uint32_t dropped)
{
    const areg::String & module{ areg::Process::instance().app_name() };
    const TIME64 now{ static_cast<TIME64>(areg::DateTime::now()) };

    mScratch = areg::LogEntry(areg::LogMessageType::MessageText);
    mScratch.logDataType    = areg::LogDataType::Local;
    mScratch.logMessagePrio = areg::LogPriority::PrioError;
    mScratch.logCookie      = areg::COOKIE_LOCAL;
    mScratch.logModuleId    = static_cast<ITEM_ID>(areg::Process::instance().id());
    mScratch.logTimestamp   = now;
    mScratch.logReceived    = now;
    mScratch.logMessageLen  = static_cast<uint32_t>(areg::String::format_string( mScratch.logMessage
                                                                               , areg::LOG_MSG_SIZE
                                                                               , "Dropped [ %u ] log messages, the logging database cannot keep up with the incoming data rate."
                                                                               , dropped));
    mScratch.logModuleLen   = areg::mem_copy(mScratch.logModule, areg::LOG_NAME_SIZE - 1u, module.as_string(), static_cast<uint32_t>(module.length()));
    mScratch.logModule[mScratch.logModuleLen] = areg::String::EmptyChar;

    mDatabase.log_message(mScratch);
}
