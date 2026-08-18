/************************************************************************
 * \file        src/LatencyReport.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Sample collection and result reporting of the local latency
 *              benchmark (example 31).
 ************************************************************************/

#ifndef EXAMPLES_31_LOCLATENCY_SRC_LATENCYREPORT_HPP
#define EXAMPLES_31_LOCLATENCY_SRC_LATENCYREPORT_HPP

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"
#include "src/LatencyOptions.hpp"

#include <cstdint>
#include <vector>

namespace loclat
{

/**
 * \brief   The result of one completed run: one topology, one mode, one repetition.
 *
 *          Every duration is given in microseconds, so that the numbers can be read
 *          next to the results of example 30 without converting anything.
 **/
struct RunResult
{
    Topology                    mTopology   { Topology::SameThread };   //!< Where the components ran.
    LocalLatency::LatencyMode   mMode       { LocalLatency::LatencyMode::Undefined };   //!< What was measured.
    uint32_t                    mRepeat     { 1u };     //!< 1-based number of the repetition.
    uint32_t                    mPayload    { 0u };     //!< Extra payload of one message, in bytes.
    uint32_t                    mSamples    { 0u };     //!< How many measured samples the numbers below are based on.
    uint32_t                    mWarmup     { 0u };     //!< How many messages were sent before measuring.

    double                      mMinUs      { 0.0 };    //!< Fastest sample. The floor the machine can reach.
    double                      mP50Us      { 0.0 };    //!< Median. Half of the samples were faster than this.
    double                      mP90Us      { 0.0 };    //!< 90 out of 100 samples were faster than this.
    double                      mP99Us      { 0.0 };    //!< 99 out of 100 samples were faster than this.
    double                      mP999Us     { 0.0 };    //!< 999 out of 1000 samples were faster than this.
    double                      mMaxUs      { 0.0 };    //!< Slowest sample. Shows the worst interruption.
    double                      mMeanUs     { 0.0 };    //!< Average of all samples.
    double                      mStdDevUs   { 0.0 };    //!< Spread around the average. Small means steady.

    //!< Median of the first half of a round trip: from the moment the consumer sent the
    //!< request until the provider received it. Zero for one-way modes, which have no
    //!< second half.
    double                      mInLegP50Us { 0.0 };

    double                      mDurationMs { 0.0 };    //!< Wall clock time of the measured part of the run.
    double                      mMsgPerSec  { 0.0 };    //!< Measured samples divided by mDurationMs.
    double                      mClockCostNs{ 0.0 };    //!< Cost of one clock reading, measured at start-up.
};

/**
 * \brief   Collects the samples of one run and turns them into a RunResult.
 *
 *          The class is used from the consumer's component thread only. Adding a sample
 *          must stay cheap, because it happens between two measured messages: it only
 *          appends two numbers to a vector that was reserved in advance. All the sorting
 *          and the arithmetic happen once, in summarize(), after the run has finished.
 **/
class LatencyReport
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    LatencyReport() = default;
    ~LatencyReport() = default;

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Throws away the samples of the previous run and makes room for the next
     *          one. Call it once before each run, never during a run.
     *
     * \param   expected    How many samples the next run will produce. The memory is
     *                      reserved now so that no allocation happens while measuring.
     **/
    void restart(uint32_t expected);

    /**
     * \brief   Stores one measured sample.
     *
     * \param   totalNs     The measured duration in nanoseconds: for a one-way mode the
     *                      time from the provider's send to the consumer's receive, for
     *                      a round trip mode the full way there and back.
     * \param   inLegNs     For a round trip mode, the time from the consumer's send until
     *                      the provider received the request. Pass 0 for one-way modes.
     **/
    inline void add_sample(int64_t totalNs, int64_t inLegNs);

    /**
     * \brief   Returns how many samples are collected so far.
     **/
    [[nodiscard]]
    inline uint32_t count() const noexcept;

    /**
     * \brief   Sorts the collected samples and computes the complete result.
     *
     *          Call it once, after the run has finished. The method changes the order of
     *          the stored samples, so it must not be mixed with add_sample().
     *
     * \param   options     The settings the run was started with.
     * \param   mode        The mode that was measured.
     * \param   repeat      1-based number of the repetition.
     * \param   durationNs  Wall clock time of the measured part of the run, in nanoseconds.
     * \param   clockCostNs Cost of one clock reading, in nanoseconds.
     * \return  Returns the finished result.
     **/
    [[nodiscard]]
    RunResult summarize( const RunOptions & options
                       , LocalLatency::LatencyMode mode
                       , uint32_t repeat
                       , uint64_t durationNs
                       , double clockCostNs );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    std::vector<int64_t>    mTotalNs;   //!< One entry per sample: the measured duration.
    std::vector<int64_t>    mInLegNs;   //!< One entry per sample: the first half of a round trip.

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(LatencyReport);
};

/**
 * \brief   Prints the head of the result table to the standard output.
 *          Call it once, before the first row.
 **/
void print_table_header();

/**
 * \brief   Prints one finished run as one row of the result table.
 **/
void print_table_row(const RunResult & result);

/**
 * \brief   Prints the machine and build description that belongs with the numbers:
 *          operating system, compiler, build type and the measured cost of the clock.
 *          Without it a result table cannot be compared with another one.
 **/
void print_environment(double clockCostNs);

/**
 * \brief   Appends one finished run to a CSV file.
 *
 *          When the file does not exist yet, the column names are written first. When it
 *          exists, the row is appended, so several sessions can collect into one file.
 *
 * \param   path        Where to write. Nothing happens when the path is empty.
 * \param   label       Free text written into the row, taken from the --label option.
 * \param   result      The run to write.
 * \return  Returns true when the row was written.
 **/
bool append_csv(const areg::String & path, const areg::String & label, const RunResult & result);

//////////////////////////////////////////////////////////////////////////
// LatencyReport inline methods
//////////////////////////////////////////////////////////////////////////

inline void LatencyReport::add_sample(int64_t totalNs, int64_t inLegNs)
{
    mTotalNs.push_back(totalNs);
    mInLegNs.push_back(inLegNs);
}

inline uint32_t LatencyReport::count() const noexcept
{
    return static_cast<uint32_t>(mTotalNs.size());
}

}   // namespace loclat

#endif  // EXAMPLES_31_LOCLATENCY_SRC_LATENCYREPORT_HPP
