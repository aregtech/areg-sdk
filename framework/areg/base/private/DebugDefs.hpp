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
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        areg/base/private/DebugDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Debugging and analysis utilities.
 *
 *              This is the scratch pad of the framework. It collects the tools
 *              that answer a question about a running system: where the time is
 *              spent, in which order the threads ran, whether a path was taken
 *              at all. Everything here is built for an investigation, is turned
 *              on by hand, and is off in every normal build.
 *
 *              HOW TO USE IT
 *
 *              Every facility sits behind its own compile time switch and every
 *              switch is off unless it is passed on the command line, for
 *              example '-DAREG_DIAGNOSE_TRACE=1'. No build configuration of the
 *              project defines them. With the switch off the calls compile to
 *              nothing, so instrumentation left in a source file costs nothing
 *              and changes no behaviour.
 *
 *              This header may be included from any source file of the
 *              framework, including one outside 'private', for the time of an
 *              investigation. That is the one accepted exception to the rule
 *              that a 'private' header stays inside its own module.
 *
 *              RULES
 *
 *              - Add a facility here when it helps to analyse a defect or to
 *                measure behaviour. Keep it behind a compile time switch.
 *              - Instrumentation is temporary. Remove the calls from the source
 *                files once the question is answered; what stays here is the
 *                tool, not its use.
 *              - Nothing here is part of the public interface, is covered by
 *                the ABI promise, or belongs in ordinary development work. Use
 *                the logging framework ('areg/logging') for anything a released
 *                build has to report.
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

#if defined(AREG_DIAGNOSE_TRACE) && (AREG_DIAGNOSE_TRACE)
    // Included only for the diagnostic trace, so that an ordinary build of the
    // 15 sources that include this header does not pay for them.
    #include "areg/base/Process.hpp"
    #include <cstdarg>
#endif  // defined(AREG_DIAGNOSE_TRACE) && (AREG_DIAGNOSE_TRACE)

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

/**
 * \brief   areg::MAX_DEBUG_BUFFER_SIZE
 *          The maximum buffer size for message output.
 **/
constexpr uint32_t  MAX_DEBUG_BUFFER_SIZE       = 1024;

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

/**
 * \brief   Returns the accumulator table of the process, with one entry per instrumented stage.
 *          Exported by the areg library, so that a process and every static library linked into
 *          it share one table and print one report.
 **/
[[nodiscard]]
AREG_API LtAccum * lt_accumulators() noexcept;

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
    const LtAccum * const table{ areg::lt_accumulators() };
    std::fprintf(stderr, "\n==================== AREG LATENCY TRACE (per-stage, microseconds) ====================\n");
    std::fprintf(stderr, "%-32s %12s %10s %10s %10s\n", "stage", "count", "min", "mean", "max");
    for (uint32_t i = 0u; i < static_cast<uint32_t>(LtStage::Count); ++i)
    {
        const LtAccum& a = table[i];
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

/**
 * \brief   Registers the exit-time report. Called any number of times, registers once.
 **/
AREG_API void lt_ensure_atexit() noexcept;

/**
 * \brief   Records one duration sample (nanoseconds) for the given stage. Lock-free; safe from
 *          any thread.
 **/
inline void lt_add_sample(LtStage s, uint64_t ns) noexcept
{
    LtAccum& a = areg::lt_accumulators()[static_cast<size_t>(s)];
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

#if defined(AREG_STALL_TRACE) && (AREG_STALL_TRACE)

/**
 * \brief   Fixed set of stages measured by the stall trace. Add entries before Count.
 **/
enum class StStage : uint32_t
{
      LogCall       = 0 //!< Caller side: the whole LogManager::log_message call of one record
    , MakeMsg           //!< Caller side: building the log message envelope
    , RingPush          //!< Caller side: the MPSC ring enqueue of one event
    , WakeConsumer      //!< Caller side: the set-signal that wakes a parked dispatcher
    , TimerStart        //!< Caller side: the whole Timer::start_timer call
    , TimerStop         //!< Caller side: the whole Timer::stop_timer call
    , TimerMapOp        //!< Caller side: register or unregister in the timer resource map
    , TimerOsCall       //!< Caller side: the operating system call that arms or disarms the timer
    , TimerMutex        //!< Caller side: taking the timer state mutex
    , Count             //!< Number of stages, keep last
};

/**
 * \brief   Upper bound in nanoseconds of every histogram bucket. The last bucket collects
 *          everything above the previous bound.
 **/
constexpr uint64_t ST_BUCKET_NS[]
{
    1000u, 5000u, 20000u, 100000u, 500000u, 2000000u, 10000000u, UINT64_MAX
};

constexpr size_t ST_BUCKETS{ sizeof(ST_BUCKET_NS) / sizeof(ST_BUCKET_NS[0]) };

/**
 * \brief   One bucket: how many samples fell in it, how long they took and how many CPU
 *          cycles the calling thread actually burned while they ran.
 **/
struct StBucket
{
    std::atomic<uint64_t> count  { 0u };
    std::atomic<uint64_t> sumNs  { 0u };
    std::atomic<uint64_t> sumCyc { 0u };
    std::atomic<uint64_t> maxNs  { 0u };
};

struct StStageData
{
    StBucket bucket[ST_BUCKETS];
};

/**
 * \brief   Returns the per-stage histogram table of the module.
 **/
[[nodiscard]]
inline StStageData * st_table() noexcept
{
    static StStageData table[static_cast<size_t>(StStage::Count)];
    return table;
}

inline const char * st_stage_name(StStage s) noexcept
{
    switch (s)
    {
    case StStage::LogCall:      return "LogCall     (whole log_message)";
    case StStage::MakeMsg:      return "MakeMsg     (build envelope)";
    case StStage::RingPush:     return "RingPush    (mpsc enqueue)";
    case StStage::WakeConsumer: return "WakeConsumer(set signal)";
    case StStage::TimerStart:   return "TimerStart  (whole start_timer)";
    case StStage::TimerStop:    return "TimerStop   (whole stop_timer)";
    case StStage::TimerMapOp:   return "TimerMapOp  (resource map)";
    case StStage::TimerOsCall:  return "TimerOsCall (arm/disarm)";
    case StStage::TimerMutex:   return "TimerMutex  (state mutex)";
    default:                    return "<unknown>";
    }
}

/**
 * \brief   Monotonic timestamp in nanoseconds.
 **/
[[nodiscard]]
inline uint64_t st_now_ns() noexcept
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

/**
 * \brief   CPU cycles consumed by the calling thread so far. A sample whose elapsed time is
 *          large while this value barely moves means the thread was not running.
 **/
[[nodiscard]]
AREG_API uint64_t st_thread_cycles() noexcept;

/**
 * \brief   Prints the per-stage histogram to stderr. Registered with std::atexit on first sample.
 **/
inline void st_dump() noexcept
{
    const StStageData * const table{ areg::st_table() };
    std::fprintf(stderr, "\n==================== AREG STALL TRACE ====================\n");
    std::fprintf(stderr, "%-30s %14s %10s %12s %12s %12s\n"
                , "stage / bucket", "count", "share%", "mean us", "max us", "mean kcyc");
    for (uint32_t s = 0u; s < static_cast<uint32_t>(StStage::Count); ++s)
    {
        uint64_t total{ 0u };
        for (size_t b = 0u; b < ST_BUCKETS; ++b)
            total += table[s].bucket[b].count.load(std::memory_order_relaxed);

        if (total == 0u)
            continue;

        std::fprintf(stderr, "%s  (n=%llu)\n", st_stage_name(static_cast<StStage>(s))
                    , static_cast<unsigned long long>(total));
        for (size_t b = 0u; b < ST_BUCKETS; ++b)
        {
            const StBucket& q = table[s].bucket[b];
            const uint64_t c = q.count.load(std::memory_order_relaxed);
            if (c == 0u)
                continue;

            char label[64];
            if (ST_BUCKET_NS[b] == UINT64_MAX)
                std::snprintf(label, sizeof(label), "   > %llu us"
                             , static_cast<unsigned long long>(ST_BUCKET_NS[b - 1u] / 1000u));
            else
                std::snprintf(label, sizeof(label), "   <= %llu us"
                             , static_cast<unsigned long long>(ST_BUCKET_NS[b] / 1000u));

            const double sumNs = static_cast<double>(q.sumNs.load(std::memory_order_relaxed));
            const double sumCy = static_cast<double>(q.sumCyc.load(std::memory_order_relaxed));
            std::fprintf(stderr, "%-30s %14llu %10.2f %12.2f %12.2f %12.1f\n"
                        , label
                        , static_cast<unsigned long long>(c)
                        , (100.0 * static_cast<double>(c)) / static_cast<double>(total)
                        , (sumNs / static_cast<double>(c)) / 1000.0
                        , static_cast<double>(q.maxNs.load(std::memory_order_relaxed)) / 1000.0
                        , (sumCy / static_cast<double>(c)) / 1000.0);
        }
    }
    std::fprintf(stderr, "==========================================================\n");
}

/**
 * \brief   Registers the exit-time report. Called any number of times, registers once.
 **/
AREG_API void st_ensure_atexit() noexcept;

/**
 * \brief   Records one sample: how long the stage took and how many cycles the thread burned.
 **/
inline void st_add_sample(StStage s, uint64_t ns, uint64_t cycles) noexcept
{
    size_t b = 0u;
    while ((b + 1u < ST_BUCKETS) && (ns > ST_BUCKET_NS[b]))
        ++b;

    StBucket& q = areg::st_table()[static_cast<size_t>(s)].bucket[b];
    q.count.fetch_add(1u, std::memory_order_relaxed);
    q.sumNs.fetch_add(ns, std::memory_order_relaxed);
    q.sumCyc.fetch_add(cycles, std::memory_order_relaxed);

    uint64_t cur = q.maxNs.load(std::memory_order_relaxed);
    while ((ns > cur) && !q.maxNs.compare_exchange_weak(cur, ns, std::memory_order_relaxed)) { }

    st_ensure_atexit();
}

/**
 * \brief   RAII helper: samples elapsed time and consumed thread cycles of the enclosing scope.
 **/
class StScoped
{
public:
    explicit StScoped(StStage s) noexcept
        : mStage(s), mStart(areg::st_now_ns()), mCycles(areg::st_thread_cycles())
    {
    }

    ~StScoped() noexcept
    {
        areg::st_add_sample(mStage, areg::st_now_ns() - mStart, areg::st_thread_cycles() - mCycles);
    }

private:
    StStage  mStage;
    uint64_t mStart;
    uint64_t mCycles;
    StScoped() = delete;
    StScoped(const StScoped&) = delete;
    StScoped& operator = (const StScoped&) = delete;
};

    #define AREG_ST_CONCAT_(a, b)   a ## b
    #define AREG_ST_CONCAT(a, b)    AREG_ST_CONCAT_(a, b)
    #define AREG_ST_SCOPE(stage)    areg::StScoped AREG_ST_CONCAT(_areg_st_scope_, __LINE__) { stage }

#else   // !AREG_STALL_TRACE

    #define AREG_ST_SCOPE(stage)    ((void)0)

#endif  // AREG_STALL_TRACE


#if defined(AREG_DIAGNOSE_TRACE) && (AREG_DIAGNOSE_TRACE)

/**
 * \brief   The marker that prefixes every diagnostic line, so that a line can be
 *          separated from the ordinary output of an application.
 **/
constexpr std::string_view DIAGNOSE_TRACE_MARKER{ "[areg-diag-trace]" };

/**
 * \brief   The name of the environment variable that switches the trace on at run time.
 **/
constexpr std::string_view DIAGNOSE_TRACE_VARIABLE{ "AREG_DIAG_TRACE" };

/**
 * \brief   Returns true when the environment variable AREG_DIAG_TRACE is set to a
 *          non-empty value. The value is read once, on the first call.
 *
 * \return  Returns true when the trace is switched on.
 **/
inline bool is_trace_enabled() noexcept
{
    static const bool enabled
    {
        areg::Process::instance().safe_env_variable(areg::DIAGNOSE_TRACE_VARIABLE.data()).is_empty() == false
    };

    return enabled;
}

/**
 * \brief   Writes one diagnostic line to the standard error stream when the trace is
 *          switched on, and does nothing otherwise. The line is prefixed with
 *          DIAGNOSE_TRACE_MARKER and is flushed at once, so that it survives a process
 *          that is killed right after.
 *
 * \param   format  The printf style format of the message.
 * \note    The formatted message is cut at MAX_DEBUG_BUFFER_SIZE characters.
 **/
inline void trace(const char* format, ...) noexcept
{
    if (!is_trace_enabled())
        return;

    char message[areg::MAX_DEBUG_BUFFER_SIZE]{};
    va_list args;
    va_start(args, format);
    std::vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    std::fprintf(stderr, "\r\n%s %s\r\n", areg::DIAGNOSE_TRACE_MARKER.data(), message);
    std::fflush(stderr);
}

#else   // defined(AREG_DIAGNOSE_TRACE) && (AREG_DIAGNOSE_TRACE)

inline bool is_trace_enabled() noexcept
{
    return false;
}

inline void trace(const char* /*format*/, ...) noexcept
{
}

#endif  // defined(AREG_DIAGNOSE_TRACE) && (AREG_DIAGNOSE_TRACE)

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
