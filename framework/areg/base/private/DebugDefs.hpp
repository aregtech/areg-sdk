#ifndef AREG_BASE_PRIVATE_DEBUGDEFS_HPP
#define AREG_BASE_PRIVATE_DEBUGDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/DebugDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debugging utilities
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <list>
#include <mutex>
#include <string>
#include <string_view>

/**
 * \brief   Debugging utilities.
 *          Contains some functions and constants available only in 
 *          Debug version. Normally, used to make outputs in
 *          Debug Output Window
 **/

/************************************************************************/
// Make visible only in Debug version
/************************************************************************/

struct _EXCEPTION_POINTERS;

//////////////////////////////////////////////////////////////////////////
// Debug specific methods declaration
//////////////////////////////////////////////////////////////////////////
namespace areg {

#if defined(AREG_LATENCY_TRACE) && (AREG_LATENCY_TRACE)

/**
 * \brief   Fixed set of instrumented stages. Add entries before Count; Count must stay last.
 *          Each stage maps to one row in the exit-time report.
 **/
enum class LtStage : uint32_t
{
      SendNode      = 0 //!< ClientSendThread: whole start_event_processing (drain + serialize + writev)
    , SendSyscall       //!< ClientSendThread: ONLY the send_messages_batch (writev/send) syscall
    , RecvNode          //!< Receive thread: process_received_message (deserialize + route + deliver-enqueue)
    , CompDispatch      //!< EventDispatcherBase: dispatch_event to the registered consumer (component node work)
    , MpscHandoff       //!< External MPSC queue: enqueue -> dequeue latency (cross-thread wakeup cost)
    , Count             //!< Number of stages, keep last.
};

//////////////////////////////////////////////////////////////////////////
// Internal accumulators (lock-free)
//////////////////////////////////////////////////////////////////////////
struct LtAccum
{
    std::atomic<uint64_t> count { 0u };
    std::atomic<uint64_t> sumNs { 0u };
    std::atomic<uint64_t> minNs { UINT64_MAX };
    std::atomic<uint64_t> maxNs { 0u };
};

inline std::array<LtAccum, static_cast<size_t>(LtStage::Count)> g_lt_accum {};

inline const char* lt_stage_name(LtStage s) noexcept
{
    switch (s)
    {
    case LtStage::SendNode:     return "SendNode    (drain+serialize+writev)";
    case LtStage::SendSyscall:  return "SendSyscall (send/writev only)";
    case LtStage::RecvNode:     return "RecvNode    (deserialize+route)";
    case LtStage::CompDispatch: return "CompDispatch(consumer handler)";
    case LtStage::MpscHandoff:  return "MpscHandoff (enqueue->dequeue)";
    default:                    return "<unknown>";
    }
}

/**
 * \brief   Monotonic high-resolution timestamp in nanoseconds. On Windows steady_clock is
 *          QueryPerformanceCounter-backed; on POSIX it is CLOCK_MONOTONIC. Comparable within
 *          one process; not intended for cross-process correlation.
 **/
[[nodiscard]]
inline uint64_t lt_now_ns() noexcept
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

/**
 * \brief   Prints the per-stage report to stderr. Registered with std::atexit on first sample.
 *          Reads atomics without locking; values are a consistent-enough snapshot for analysis.
 **/
inline void lt_dump() noexcept
{
    std::fprintf(stderr, "\n==================== AREG LATENCY TRACE (per-stage, microseconds) ====================\n");
    std::fprintf(stderr, "%-32s %12s %10s %10s %10s\n", "stage", "count", "min", "mean", "max");
    for (uint32_t i = 0u; i < static_cast<uint32_t>(LtStage::Count); ++i)
    {
        const LtAccum& a = g_lt_accum[i];
        const uint64_t c = a.count.load(std::memory_order_relaxed);
        if (c == 0u)
            continue;

        const double sum  = static_cast<double>(a.sumNs.load(std::memory_order_relaxed));
        const double mn   = static_cast<double>(a.minNs.load(std::memory_order_relaxed));
        const double mx   = static_cast<double>(a.maxNs.load(std::memory_order_relaxed));
        std::fprintf(stderr, "%-32s %12llu %10.3f %10.3f %10.3f\n"
                    , lt_stage_name(static_cast<LtStage>(i))
                    , static_cast<unsigned long long>(c)
                    , mn / 1000.0
                    , (sum / static_cast<double>(c)) / 1000.0
                    , mx / 1000.0);
    }
    std::fprintf(stderr, "======================================================================================\n");
}

inline void lt_ensure_atexit() noexcept
{
    static std::once_flag _once;
    std::call_once(_once, []() noexcept { std::atexit(&areg::lt_dump); });
}

/**
 * \brief   Records one duration sample (nanoseconds) for the given stage. Lock-free; safe from
 *          any thread.
 **/
inline void lt_add_sample(LtStage s, uint64_t ns) noexcept
{
    LtAccum& a = g_lt_accum[static_cast<size_t>(s)];
    a.count.fetch_add(1u, std::memory_order_relaxed);
    a.sumNs.fetch_add(ns, std::memory_order_relaxed);

    uint64_t cur = a.minNs.load(std::memory_order_relaxed);
    while ((ns < cur) && !a.minNs.compare_exchange_weak(cur, ns, std::memory_order_relaxed)) { }

    cur = a.maxNs.load(std::memory_order_relaxed);
    while ((ns > cur) && !a.maxNs.compare_exchange_weak(cur, ns, std::memory_order_relaxed)) { }

    lt_ensure_atexit();
}

/**
 * \brief   RAII helper: samples the elapsed time of the enclosing scope into a stage.
 **/
class LtScoped
{
public:
    explicit LtScoped(LtStage s) noexcept : mStage(s), mStart(areg::lt_now_ns()) { }
    ~LtScoped() noexcept { areg::lt_add_sample(mStage, areg::lt_now_ns() - mStart); }
private:
    LtStage  mStage;
    uint64_t mStart;
    LtScoped() = delete;
    LtScoped(const LtScoped&) = delete;
    LtScoped& operator = (const LtScoped&) = delete;
};

    #define AREG_LT_CONCAT_(a, b)   a ## b
    #define AREG_LT_CONCAT(a, b)    AREG_LT_CONCAT_(a, b)
    #define AREG_LT_SCOPE(stage)    areg::LtScoped AREG_LT_CONCAT(_areg_lt_scope_, __LINE__) { stage }
    #define AREG_LT_NOW()           areg::lt_now_ns()
    #define AREG_LT_SAMPLE(stage, ns) areg::lt_add_sample((stage), (ns))

#else   // !AREG_LATENCY_TRACE

    #define AREG_LT_SCOPE(stage)        ((void)0)
    #define AREG_LT_NOW()               (static_cast<uint64_t>(0))
    #define AREG_LT_SAMPLE(stage, ns)   ((void)0)

#endif  // AREG_LATENCY_TRACE

/**
 * \brief   areg::DebugPriority
 *          Defines message priority in debug output window.
 *          There is no filter of messages in priority, but
 *          by using these constants, user makes appropriate 
 *          message output prefix, showing importance. 
 **/
enum class DebugPriority : int32_t
{
      PrioNone  = OUTPUT_DEBUG_LEVEL_NONE   //!< No priority, output message should be ignored.
    , PrioErr   = OUTPUT_DEBUG_LEVEL_ERROR  //!< Priority Error, outputs "ERR:" string in front of message
    , PrioWarn  = OUTPUT_DEBUG_LEVEL_WARN   //!< Priority Warning, outputs "WRN:" string in front of message
    , PrioInfo  = OUTPUT_DEBUG_LEVEL_INFO   //!< Priority Information, outputs "INF:" string in front of message
    , PrioDbg   = OUTPUT_DEBUG_LEVEL_DEBUG  //!< Priority Debug, outputs "DBG:" string in front of message

};

/**
 * \brief   areg::MAX_DEBUG_BUFFER_SIZE
 *          The maximum buffer size for message output.
 **/
constexpr uint32_t  MAX_DEBUG_BUFFER_SIZE       = 1024;

constexpr std::string_view PREFIX_DBG_PRIORITIES [] =
{
      ""        //!< No priority.
    , "ERR: "   //!< Priority Error message.
    , "WRN: "   //!< Priority Warning message.
    , "INF: "   //!< Priority Information message.
    , "DBG: "   //!< Priority Debug message.
};

/**
 * \brief   Returns the string prefix corresponding to the specified priority level.
 *
 * \param   priority    The priority of message.
 * \return  Returns the priority prefix string.
 **/
inline constexpr std::string_view prio_prefix( areg::DebugPriority priority );

/**
 * \brief   Outputs a formatted message to the debug output window with the specified priority
 *          level.
 *
 * \param   priority    The priority of message.
 * \param   msg         The message format string.
 **/
void AREG_API output_console(areg::DebugPriority priority, const char* msg, ...);

/**
 * \brief   Outputs a formatted message to the debug output window without adding priority or
 *          newline characters.
 *
 * \param   msg     The message format string.
 **/
void AREG_API output_console(const char* msg, ...);

#ifdef  _WIN32
/**
 * \brief   Extracts and dumps exception call stack information into the output list. Available
 *          in debug builds only.
 *
 * \param   ep              Pointer to the exception structure containing the CONTEXT object.
 * \param[out] callStack       List that receives the call stack messages.
 **/

void AREG_API dump_call_stack( struct _EXCEPTION_POINTERS *ep, std::list<std::string> & callStack);
#endif  // _WIN32

/**
 * \brief   Outputs a message via the operating system (VS Output Window on Windows, console on
 *          other platforms). Valid only for DEBUG builds.
 *
 * \param   msg     The message to output.
 **/
void AREG_API output_message_os( const char * msg );

} // namespace areg

//////////////////////////////////////////////////////////////////////////
// areg namespace inline function implementation.
//////////////////////////////////////////////////////////////////////////
inline constexpr std::string_view areg::prio_prefix( areg::DebugPriority priority )
{
    return areg::PREFIX_DBG_PRIORITIES[ static_cast<int32_t>(priority) ];
}

#endif  // AREG_BASE_PRIVATE_DEBUGDEFS_HPP
