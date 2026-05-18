#ifndef AREG_IPC_DATARATESTATS_HPP
#define AREG_IPC_DATARATESTATS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/DataRateStats.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, thread-safe send/receive data rate counter.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <atomic>
#include <cstdint>

namespace areg {

//////////////////////////////////////////////////////////////////////////
// DataRateStats class declaration.
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Lightweight atomic send/receive statistics counter.
 *          Embed one instance per direction (send or receive) in a thread
 **/
class AREG_API DataRateStats
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    DataRateStats() noexcept;

    ~DataRateStats() = default;

//////////////////////////////////////////////////////////////////////////
// Public methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Adds bytes and message count to the running totals.
     *          No-op when tracking is disabled.
     *
     * \param   bytes   Number of bytes to add.
     * \param   msgs    Number of messages to add.
     **/
    inline void accumulate(uint64_t bytes, uint32_t msgs) noexcept;

    /**
     * \brief   Returns and atomically resets the accumulated byte count.
     *          Returns 0 when tracking is disabled (counters are already 0).
     **/
    [[nodiscard]]
    inline uint64_t extract_bytes() const noexcept;

    /**
     * \brief   Returns and atomically resets the accumulated message count.
     *          Returns 0 when tracking is disabled (counters are already 0).
     **/
    [[nodiscard]]
    inline uint32_t extract_msgs() const noexcept;

    /**
     * \brief   Enables or disables tracking.
     *          Resets both counters whenever the enabled state changes so
     *          stale data from the previous interval is not reported.
     *
     * \param   enable  True to start accumulating; false to stop.
     **/
    inline void set_enabled(bool enable) noexcept;

    /**
     * \brief   Returns true if tracking is currently enabled.
     **/
    [[nodiscard]]
    inline bool is_enabled() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable: 4251)
#endif  // _MSC_VER
    mutable std::atomic_uint64_t    mBytes;     //!< Running byte total.
    mutable std::atomic_uint32_t    mMsgs;      //!< Running message total.
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif  // _MSC_VER
    bool                            mEnabled;   //!< Whether accumulation is active.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(DataRateStats);
};

inline void DataRateStats::accumulate(uint64_t bytes, uint32_t msgs) noexcept
{
    if (mEnabled)
    {
        mBytes.fetch_add(bytes, std::memory_order_relaxed);
        mMsgs.fetch_add(msgs, std::memory_order_relaxed);
    }
}

inline uint64_t DataRateStats::extract_bytes() const noexcept
{
    return mBytes.exchange(0u, std::memory_order_relaxed);
}

inline uint32_t DataRateStats::extract_msgs() const noexcept
{
    return mMsgs.exchange(0u, std::memory_order_relaxed);
}

inline void DataRateStats::set_enabled(bool enable) noexcept
{
    if (mEnabled != enable)
    {
        mBytes.store(0u, std::memory_order_relaxed);
        mMsgs.store(0u, std::memory_order_relaxed);
        mEnabled = enable;
    }
}

inline bool DataRateStats::is_enabled() const noexcept
{
    return mEnabled;
}

} // namespace areg

#endif  // AREG_IPC_DATARATESTATS_HPP
