/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/service/private/LogCollectorDroppedRows.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector record of the log rows dropped for held observers.
 ************************************************************************/

#include "logcollector/service/private/LogCollectorDroppedRows.hpp"

#include "areg/base/DateTime.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"
#include "areg/component/ServiceDefs.hpp"
#include "areg/logging/LoggingDefs.hpp"

#include <limits>

namespace
{
    constexpr char      DROPPED_PREFIX[]    { "Dropped [ " };
    constexpr uint32_t  DROPPED_PREFIX_LEN  { static_cast<uint32_t>(sizeof(DROPPED_PREFIX) - 1u) };
    constexpr uint32_t  MAX_DIGITS          { 20u };

    areg::MessageEnvelope _make_notice(const ITEM_ID & observer, const ITEM_ID & source, areg::LogPriority prio, const char * format, unsigned long long count)
    {
        areg::LogEntry log{ areg::LogMessageType::MessageText };
        log.logMessagePrio  = prio;
        log.logSource       = source;
        log.logTarget       = observer;
        log.logThreadId     = static_cast<ITEM_ID>(areg::Thread::current_thread_id());
        log.logTimestamp    = static_cast<TIME64>(areg::DateTime::now());
        const int32_t len{ areg::String::format_string(log.logMessage, static_cast<int32_t>(areg::LOG_MSG_SIZE), format, count) };
        log.logMessageLen   = static_cast<uint32_t>(len > 0 ? len : 0);

        areg::MessageEnvelope msg{ areg::create_log_message(log, areg::LogDataType::Remote, source) };
        msg.set_target(static_cast<uint32_t>(observer));
        return msg;
    }
}

void LogCollectorDroppedRows::count_dropped(const ITEM_ID & observer, const areg::MessageEnvelope & msgLog)
{
    uint32_t & rows{ mRows[observer] };
    uint64_t reported{ 0u };
    if (read_dropped_notice(msgLog, reported))
    {
        mSourceRows[_key(observer, static_cast<ITEM_ID>(msgLog.source()))] += reported;
    }
    else
    {
        ++ rows;
    }
}

void LogCollectorDroppedRows::remove(const ITEM_ID & observer)
{
    mRows.remove_at(observer);
    for (auto pos = mSourceRows.first_position(); mSourceRows.is_valid_position(pos); )
    {
        if (_observer_of(mSourceRows.key_at(pos)) == observer)
            pos = mSourceRows.remove_at(pos);
        else
            pos = mSourceRows.next_position(pos);
    }
}

bool LogCollectorDroppedRows::read_dropped_notice(const areg::MessageEnvelope & msgLog, uint64_t & count)
{
    const uint32_t head{ areg::log_entry_head() };
    if ((msgLog.buffer() == nullptr) || (msgLog.size_used() <= head))
        return false;

    const areg::LogEntry & entry{ *reinterpret_cast<const areg::LogEntry *>(msgLog.buffer()) };
    if ((entry.logMsgType != areg::LogMessageType::MessageText) || (entry.logScopeId != areg::LOG_SCOPE_ID_NONE))
        return false;

    // The entry holds only the used part of the text.
    const uint32_t available{ msgLog.size_used() - head };
    const uint32_t limit{ available < areg::LOG_MSG_SIZE ? available : areg::LOG_MSG_SIZE };
    const char * text{ entry.logMessage };
    uint32_t pos{ 0u };
    for (; pos < DROPPED_PREFIX_LEN; ++ pos)
    {
        if ((pos >= limit) || (text[pos] != DROPPED_PREFIX[pos]))
            return false;
    }

    const uint32_t first{ pos };
    uint64_t value{ 0u };
    for (; (pos < limit) && ((pos - first) < MAX_DIGITS) && (text[pos] >= '0') && (text[pos] <= '9'); ++ pos)
    {
        const uint64_t digit{ static_cast<uint64_t>(text[pos] - '0') };
        if (value > ((std::numeric_limits<uint64_t>::max() - digit) / 10u))
            return false;

        value = (value * 10u) + digit;
    }

    if ((pos == first) || (pos >= limit) || (text[pos] != ' '))
        return false;

    count = value;
    return true;
}

areg::MessageEnvelope LogCollectorDroppedRows::make_source_notice(const ITEM_ID & observer, const ITEM_ID & source, uint64_t count)
{
    return _make_notice( observer
                       , source
                       , areg::LogPriority::PrioError
                       , "Dropped [ %llu ] log messages, the log source reported them while the log collector dropped rows for this observer."
                       , static_cast<unsigned long long>(count));
}

areg::MessageEnvelope LogCollectorDroppedRows::make_collector_notice(const ITEM_ID & observer, uint32_t count)
{
    return _make_notice( observer
                       , areg::COOKIE_LOGGER
                       , areg::LogPriority::PrioWarning
                       , "The log collector dropped %llu log messages for this observer, it read slower than the logs arrived."
                       , static_cast<unsigned long long>(count));
}
