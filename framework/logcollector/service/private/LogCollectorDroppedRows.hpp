#ifndef AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDROPPEDROWS_HPP
#define AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDROPPEDROWS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        logcollector/service/private/LogCollectorDroppedRows.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Log Collector record of the log rows dropped for held observers.
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/areg_global.h"

#include "areg/base/MessageEnvelope.hpp"
#include "areg/base/OrderedMap.hpp"

#include <utility>

//////////////////////////////////////////////////////////////////////////
// LogCollectorDroppedRows class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The log rows the log collector dropped for observers that read slower than the logs
 *          arrived, and the warning rows that report them.
 *
 *          A dropped row is counted for its observer. A dropped 'Dropped [ N ]' notice of a log
 *          source is not a row of its own: N is added to that source for the observer. When the
 *          observer is released it gets one 'Dropped [ N ]' row per such source and one row with
 *          the number of rows the log collector dropped. Every missing row stays covered by a
 *          notice, and the record holds one entry per observer and per source.
 **/
class LogCollectorDroppedRows
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LogCollectorDroppedRows() = default;
    ~LogCollectorDroppedRows() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Counts a log row dropped for the observer. A 'Dropped [ N ]' notice of a log source
     *          adds N to that source instead.
     *
     * \param   observer    The observer the row was dropped for.
     * \param   msgLog      The dropped log row. Its source is the log source.
     **/
    void count_dropped(const ITEM_ID & observer, const areg::MessageEnvelope & msgLog);

    /**
     * \brief   Returns true if no row is counted for any observer.
     **/
    [[nodiscard]]
    inline bool is_empty() const noexcept;

    /**
     * \brief   Returns true if rows are counted for the observer.
     **/
    [[nodiscard]]
    inline bool contains(const ITEM_ID & observer) const;

    /**
     * \brief   Passes the warning rows of the observer to 'send' and forgets the observer.
     *
     * \param   observer    The observer to release.
     * \param   send        Called with each warning row, an areg::MessageEnvelope rvalue.
     * \return  Returns true if rows were counted for the observer.
     **/
    template<typename Send>
    bool release(const ITEM_ID & observer, Send && send);

    /**
     * \brief   Releases every observer for which 'isHeld' returns false.
     *
     * \param   isHeld      Called with an observer; returns true if it is still held.
     * \param   send        Called with each warning row, an areg::MessageEnvelope rvalue.
     * \return  Returns true if an observer is still held and has to be released later.
     **/
    template<typename IsHeld, typename Send>
    bool release_all(IsHeld && isHeld, Send && send);

    /**
     * \brief   Forgets the rows counted for the observer, which has disconnected.
     **/
    void remove(const ITEM_ID & observer);

    /**
     * \brief   Reads N of a 'Dropped [ N ]' notice of a log source.
     *
     * \param   msgLog  The log row.
     * \param   count   On output, N.
     * \return  Returns true if the row is a text row without a scope that starts with 'Dropped [ N ]'.
     **/
    [[nodiscard]]
    static bool read_dropped_notice(const areg::MessageEnvelope & msgLog, uint64_t & count);

    /**
     * \brief   Creates the row that reports N log rows of a source whose notices were dropped.
     *
     * \param   observer    The observer that gets the row.
     * \param   source      The log source the row is attributed to.
     * \param   count       N.
     **/
    [[nodiscard]]
    static areg::MessageEnvelope make_source_notice(const ITEM_ID & observer, const ITEM_ID & source, uint64_t count);

    /**
     * \brief   Creates the row that reports the log rows the log collector dropped for the observer.
     *
     * \param   observer    The observer that gets the row.
     * \param   count       The number of dropped rows.
     **/
    [[nodiscard]]
    static areg::MessageEnvelope make_collector_notice(const ITEM_ID & observer, uint32_t count);

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns the key of an observer and a source in mSourceRows.
     **/
    [[nodiscard]]
    static constexpr uint64_t _key(const ITEM_ID & observer, const ITEM_ID & source) noexcept;

    /**
     * \brief   Returns the observer of a key in mSourceRows.
     **/
    [[nodiscard]]
    static constexpr ITEM_ID _observer_of(uint64_t key) noexcept;

    /**
     * \brief   Returns the source of a key in mSourceRows.
     **/
    [[nodiscard]]
    static constexpr ITEM_ID _source_of(uint64_t key) noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    //!< An observer that rows were dropped for, mapped to the rows the log collector dropped.
    areg::OrderedMap<ITEM_ID, uint32_t>  mRows;

    //!< An observer and a source (see _key()), mapped to the sum of the dropped notices of the source.
    areg::OrderedMap<uint64_t, uint64_t> mSourceRows;

private:
    AREG_NOCOPY_NOMOVE(LogCollectorDroppedRows);
};

//////////////////////////////////////////////////////////////////////////
// LogCollectorDroppedRows class inline methods
//////////////////////////////////////////////////////////////////////////

inline bool LogCollectorDroppedRows::is_empty() const noexcept
{
    return mRows.is_empty();
}

inline bool LogCollectorDroppedRows::contains(const ITEM_ID & observer) const
{
    return mRows.contains(observer);
}

constexpr uint64_t LogCollectorDroppedRows::_key(const ITEM_ID & observer, const ITEM_ID & source) noexcept
{
    return ((static_cast<uint64_t>(observer) << 32u) | static_cast<uint64_t>(source));
}

constexpr ITEM_ID LogCollectorDroppedRows::_observer_of(uint64_t key) noexcept
{
    return static_cast<ITEM_ID>(key >> 32u);
}

constexpr ITEM_ID LogCollectorDroppedRows::_source_of(uint64_t key) noexcept
{
    return static_cast<ITEM_ID>(key & 0xFFFFFFFFu);
}

template<typename Send>
bool LogCollectorDroppedRows::release(const ITEM_ID & observer, Send && send)
{
    auto pos{ mRows.find(observer) };
    if (mRows.is_valid_position(pos) == false)
        return false;

    const uint32_t dropped{ mRows.value_at(pos) };
    mRows.remove_at(pos);

    for (auto entry = mSourceRows.first_position(); mSourceRows.is_valid_position(entry); )
    {
        const uint64_t key{ mSourceRows.key_at(entry) };
        if (_observer_of(key) != observer)
        {
            entry = mSourceRows.next_position(entry);
            continue;
        }

        const uint64_t count{ mSourceRows.value_at(entry) };
        entry = mSourceRows.remove_at(entry);
        send(make_source_notice(observer, _source_of(key), count));
    }

    if (dropped != 0u)
    {
        send(make_collector_notice(observer, dropped));
    }

    return true;
}

template<typename IsHeld, typename Send>
bool LogCollectorDroppedRows::release_all(IsHeld && isHeld, Send && send)
{
    for (auto pos = mRows.first_position(); mRows.is_valid_position(pos); )
    {
        const ITEM_ID observer{ mRows.key_at(pos) };
        pos = mRows.next_position(pos);
        if (isHeld(observer) == false)
        {
            release(observer, send);
        }
    }

    return (mRows.is_empty() == false);
}

#endif  // AREG_LOGCOLLECTOR_SERVICE_PRIVATE_LOGCOLLECTORDROPPEDROWS_HPP
