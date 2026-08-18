/************************************************************************
 * \file        src/LatencyOptions.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Command line options of the local latency benchmark (example 31).
 ************************************************************************/

#ifndef EXAMPLES_31_LOCLATENCY_SRC_LATENCYOPTIONS_HPP
#define EXAMPLES_31_LOCLATENCY_SRC_LATENCYOPTIONS_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "examples/31_loclatency/services/LocalLatency.hpp"

#include <chrono>
#include <cstdint>
#include <string_view>
#include <vector>

namespace loclat
{

/**
 * \brief   Where the service provider and the service consumer run.
 *
 *          The benchmark never changes the code of the provider or of the consumer.
 *          It only changes where the framework puts them, and then measures the
 *          difference. This is the only knob that changes the message path.
 **/
enum class Topology : uint8_t
{
      SameThread    //!< Provider and consumer are two components in ONE component thread.
                    //!< A message is put into the thread's own event queue and is taken out
                    //!< of it by the very same thread. The thread never goes to sleep while
                    //!< the test runs, so no thread wake-up is involved at all.
    , CrossThread   //!< Provider and consumer are two components in TWO component threads
                    //!< of the same process. Every message crosses one thread boundary:
                    //!< the sending thread puts it into the queue of the receiving thread
                    //!< and wakes that thread up.
};

/**
 * \brief   All settings of one benchmark session, filled in from the command line.
 *
 *          One session runs the list of modes `mModes`, in the given order, `mRepeat`
 *          times. Each single run of one mode produces one result line.
 **/
struct RunOptions
{
    //!< Where the provider and the consumer run. See loclat::Topology.
    Topology                                mTopology   { Topology::SameThread };

    //!< The modes to measure, in the order they are measured. Never empty after a
    //!< successful parse.
    std::vector<LocalLatency::LatencyMode>  mModes      {};

    //!< How many messages are measured per run. Messages sent during the warm-up are
    //!< not counted here.
    uint32_t                                mCount      { 100000u };

    //!< How many messages are sent before measuring starts. They fill the caches, the
    //!< branch predictors and the memory allocator, so that the measured messages are
    //!< not slowed down by first-time costs.
    uint32_t                                mWarmup     { 10000u };

    //!< How many times the whole list of modes is measured. Use more than 1 to see how
    //!< stable the machine is.
    uint32_t                                mRepeat     { 1u };

    //!< Where the result table is appended in CSV form. Empty means: do not write a file.
    areg::String                            mCsvPath    {};

    //!< Free text written into every CSV line, for example a build name or a task number.
    //!< It lets you tell apart measurements taken before and after a change.
    areg::String                            mLabel      {};

    //!< When true, only the result table is printed and the progress lines are left out.
    bool                                    mQuiet      { false };
};

/**
 * \brief   Returns the one and only options object of this process.
 *
 *          The components are created by the framework, not by main(), so they cannot be
 *          given the settings through a constructor. Instead main() parses the command
 *          line into this object BEFORE it loads the model, and the components read it
 *          afterwards. Because the object is only written before the first component
 *          exists and only read afterwards, no locking is needed.
 **/
[[nodiscard]]
RunOptions & run_options();

/**
 * \brief   Reads the command line and fills the options.
 *
 *          On a wrong or unknown option the function prints an explanation and the
 *          usage text, and returns false. When the user asked for `--help` or `--list`
 *          the function prints the requested text, sets `outExitNow` to true and
 *          returns true: nothing is wrong, but the program must stop right away.
 *
 * \param   argc        Number of command line arguments, as given to main().
 * \param   argv        The command line arguments, as given to main().
 * \param   outOptions  Receives the parsed options. Untouched when the function fails.
 * \param   outExitNow  Receives true when the program must exit without measuring.
 * \return  Returns true when the command line is correct.
 **/
[[nodiscard]]
bool parse_command_line(int argc, char ** argv, RunOptions & outOptions, bool & outExitNow);

/**
 * \brief   Prints the usage text of the benchmark to the standard output.
 **/
void print_usage();

/**
 * \brief   Prints the list of mode names that `--mode` accepts.
 **/
void print_modes();

/**
 * \brief   Converts a mode into the short name used on the command line and in reports,
 *          for example "pp0" or "bc65536".
 *
 * \param   mode    The mode to convert.
 * \return  Returns the short name. Returns "?" when the mode has no short name.
 **/
[[nodiscard]]
const char * mode_as_str(LocalLatency::LatencyMode mode) noexcept;

/**
 * \brief   Converts a short mode name back into a mode.
 *
 * \param   name    The short name, for example "pp0". The comparison is case sensitive.
 * \return  Returns the mode, or LatencyMode::Undefined when the name is not known.
 **/
[[nodiscard]]
LocalLatency::LatencyMode string_as_mode(std::string_view name) noexcept;

/**
 * \brief   Converts a topology into the short name used on the command line and in
 *          reports: "same" or "cross".
 **/
[[nodiscard]]
const char * topology_as_str(Topology topology) noexcept;

/**
 * \brief   Tells whether the mode is a round trip (request and response).
 *
 *          A round trip mode carries the message from the consumer to the provider and
 *          back. A one-way mode carries it only from the provider to the consumer.
 *
 * \param   mode    The mode to test.
 * \return  Returns true for the Request* modes, false for everything else.
 **/
[[nodiscard]]
inline bool is_round_trip(LocalLatency::LatencyMode mode) noexcept;

/**
 * \brief   Returns the number of extra payload bytes the mode carries, without the
 *          identifier and the timestamps. For example 64 for "pp64" and for "bc64".
 **/
[[nodiscard]]
uint32_t mode_payload_size(LocalLatency::LatencyMode mode) noexcept;

/**
 * \brief   Returns the current value of the steady clock in nanoseconds.
 *
 *          The steady clock never jumps and never goes backwards, and inside one process
 *          all threads read the same clock. That is exactly what a latency measurement
 *          needs. The call itself costs time; measure_clock_cost() tells how much.
 **/
[[nodiscard]]
inline uint64_t now_ns() noexcept;

/**
 * \brief   Measures how long one call of now_ns() takes, in nanoseconds.
 *
 *          Every measured sample contains two such calls, one on each side. On a fast
 *          path the cost of reading the clock is no longer negligible, so the benchmark
 *          reports it and you can subtract it when you compare very small numbers.
 *
 * \param   samples     How many calls are timed. 200000 gives a stable result and takes
 *                      a few milliseconds.
 * \return  Returns the average cost of one now_ns() call in nanoseconds.
 **/
[[nodiscard]]
double measure_clock_cost(uint32_t samples = 200000u) noexcept;

//////////////////////////////////////////////////////////////////////////
// loclat inline methods
//////////////////////////////////////////////////////////////////////////

inline bool is_round_trip(LocalLatency::LatencyMode mode) noexcept
{
    return static_cast<uint16_t>(mode) >= static_cast<uint16_t>(LocalLatency::LatencyMode::Request0)
        && static_cast<uint16_t>(mode) <= static_cast<uint16_t>(LocalLatency::LatencyMode::Request65536);
}

inline uint64_t now_ns() noexcept
{
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

}   // namespace loclat

#endif  // EXAMPLES_31_LOCLATENCY_SRC_LATENCYOPTIONS_HPP
