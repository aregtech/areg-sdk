//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech (Artak Avetyan).
// Description : Log source of the logging benchmark. It writes log messages
//               and measures how long one LOG_DBG call takes. The messages
//               go to the log collector, where 35_logsink receives them.
//               By default each thread writes as fast as it can; with -p a
//               timer writes -b messages per period.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/base/SyncPrimitives.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Timer.hpp"
#include "areg/component/TimerConsumer.hpp"
#include "areg/logging/areg_log.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

DEF_LOG_SCOPE(examples_35_logsource_LogThread, generate);
DEF_LOG_SCOPE(examples_35_logsource_PacedLogger, process_timer);

namespace
{
    using Clock = std::chrono::steady_clock;

    //!< The most strings and the most numbers one message can carry.
    constexpr uint32_t  MAX_ARGS        { 5u };
    //!< Milliseconds the source waits for the log collector before it gives up.
    constexpr uint32_t  CONNECT_MS      { 5'000u };

    //!< The options of a run.
    struct Options
    {
        uint32_t    count   { 100'000u };   //!< Messages each thread measures.
        uint32_t    warmup  { 1'000u };     //!< Messages each thread writes before it measures.
        uint32_t    threads { 1u };         //!< Threads that write logs at the same time.
        uint32_t    strings { 3u };         //!< Strings in one message, 0 to 5.
        uint32_t    numbers { 3u };         //!< Numbers in one message, 0 to 5.
        uint32_t    period  { 0u };         //!< Milliseconds between two timer events, 0 for no pause.
        uint32_t    burst   { 1u };         //!< Messages written on each timer event.
    };

    //!< Reads the value of an option like "-c=1000". Returns false if the argument is another option.
    bool read_option(const char * arg, const char * name, uint32_t & value)
    {
        const std::size_t len{ std::strlen(name) };
        if ((std::strncmp(arg, name, len) != 0) || (arg[len] != '='))
            return false;

        value = static_cast<uint32_t>(std::strtoul(arg + len + 1, nullptr, 10));
        return true;
    }

    bool parse_options(int argc, char ** argv, Options & opt)
    {
        for (int i = 1; i < argc; ++i)
        {
            const char * arg{ argv[i] };
            if ( !read_option(arg, "-c", opt.count)   && !read_option(arg, "-w", opt.warmup)
              && !read_option(arg, "-t", opt.threads) && !read_option(arg, "-s", opt.strings)
              && !read_option(arg, "-n", opt.numbers) && !read_option(arg, "-p", opt.period)
              && !read_option(arg, "-b", opt.burst) )
            {
                std::printf("Usage: 35_logsource [-c=<count>] [-w=<warmup>] [-t=<threads>] [-s=<strings 0..5>] [-n=<numbers 0..5>] [-p=<period ms>] [-b=<messages per period>]\n");
                return false;
            }
        }

        opt.threads = (opt.period != 0u) ? 1u : std::max(opt.threads, 1u);
        opt.strings = std::min(opt.strings, MAX_ARGS);
        opt.numbers = std::min(opt.numbers, MAX_ARGS);
        opt.burst   = std::max(opt.burst, 1u);
        return (opt.count != 0u);
    }

    //!< Builds the format of a message with the given number of strings and numbers.
    std::string make_format(uint32_t strings, uint32_t numbers)
    {
        std::string result{ "msg" };
        for (uint32_t i = 0u; i < MAX_ARGS; ++i)
        {
            result += (i < strings) ? " %s" : "%.0s";
        }

        for (uint32_t i = 0u; i < MAX_ARGS; ++i)
        {
            result += (i < numbers) ? " %u" : "%.0u";
        }

        return result;
    }

    //!< The samples of one writer: the nanoseconds of each measured call and of the whole run.
    class LogSamples
    {
    public:
        LogSamples(const Options & opt, const std::string & format)
            : mFormat       ( format )
            , mSamples      ( opt.count, 0u )
            , mStep         ( opt.numbers != 0u ? 1u : 0u )
            , mSequence     ( 0u )
            , mDurationNs   ( 0u )
        {
            constexpr uint32_t values[MAX_ARGS]{ 0u, 1024u, 65535u, 7u, 42u };
            for (uint32_t i = 0u; i < MAX_ARGS; ++i)
            {
                mNumbers[i] = (i < opt.numbers) ? values[i] : 0u;
            }
        }

        const std::vector<uint32_t> & samples() const noexcept
        {
            return mSamples;
        }

        uint64_t duration_ns() const noexcept
        {
            return mDurationNs;
        }

    protected:
        //!< Returns the nanoseconds since the given time.
        static uint32_t elapsed_ns(const Clock::time_point & start)
        {
            return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start).count());
        }

        //!< Advances the number that changes in every message.
        uint32_t next_sequence() noexcept
        {
            mSequence += mStep;
            return mSequence;
        }

        const std::string       mFormat;            //!< The format of every message.
        std::vector<uint32_t>   mSamples;           //!< Nanoseconds of each measured call.
        uint32_t                mNumbers[MAX_ARGS]; //!< The numbers of a message, 0 where not used.
        const uint32_t          mStep;              //!< 1 if the message has numbers, otherwise 0.
        uint32_t                mSequence;          //!< The number that changes in every message.
        uint64_t                mDurationNs;        //!< Nanoseconds of the measured part.
    };

    //!< A thread that writes the log messages as fast as it can.
    class LogThread final : public areg::Thread, protected areg::ThreadConsumer, public LogSamples
    {
    public:
        LogThread(const std::string & name, const Options & opt, const std::string & format)
            : areg::Thread  ( *this, name.c_str() )
            , LogSamples    ( opt, format )
            , mWarmup       ( opt.warmup )
        {
        }

    protected:
        void on_run() override
        {
            LOG_SCOPE(examples_35_logsource_LogThread, generate);
            const char * format{ mFormat.c_str() };
            for (uint32_t i = 0u; i < mWarmup; ++i)
            {
                const uint32_t seq{ next_sequence() };
                LOG_DBG(format, "alpha", "bravo", "charlie", "delta", "echo", seq, mNumbers[1], mNumbers[2], mNumbers[3], mNumbers[4]);
            }

            const Clock::time_point begin{ Clock::now() };
            for (uint32_t & sample : mSamples)
            {
                const uint32_t seq{ next_sequence() };
                const Clock::time_point start{ Clock::now() };
                LOG_DBG(format, "alpha", "bravo", "charlie", "delta", "echo", seq, mNumbers[1], mNumbers[2], mNumbers[3], mNumbers[4]);
                sample = elapsed_ns(start);
            }

            mDurationNs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - begin).count());
        }

    private:
        const uint32_t  mWarmup;    //!< Messages written before the measured part.
    };

    //!< A dispatcher thread that writes a burst of log messages on each timer event.
    class PacedLogger final : public areg::DispatcherThread, private areg::TimerConsumer, public LogSamples
    {
    public:
        PacedLogger(const Options & opt, const std::string & format)
            : areg::DispatcherThread( "PacedLogger", areg::DEFAULT_BLOCK_SIZE, areg::IGNORE_VALUE )
            , areg::TimerConsumer   ( )
            , LogSamples            ( opt, format )
            , mTimer                ( static_cast<areg::TimerConsumer &>(*this), "PacedLoggerTimer" )
            , mPeriod               ( opt.period )
            , mWarmup               ( opt.warmup )
            , mBurst                ( opt.burst )
            , mFired                ( 0u )
            , mBegin                ( )
            , mDone                 ( true, false )
        {
        }

        //!< Starts the timer and waits until the last message is written.
        void run()
        {
            if (mTimer.start_timer(mPeriod, static_cast<areg::DispatcherThread &>(*this), areg::Timer::CONTINUOUSLY))
            {
                static_cast<void>(mDone.lock(areg::WAIT_INFINITE));
            }

            mTimer.stop_timer();
        }

    protected:
        void process_timer(areg::Timer & /* timer */) override
        {
            LOG_SCOPE(examples_35_logsource_PacedLogger, process_timer);
            for (uint32_t i = 0u; i < mBurst; ++i)
            {
                const uint32_t index{ mFired++ };
                const uint32_t seq{ next_sequence() };
                if (index < mWarmup)
                {
                    LOG_DBG(mFormat.c_str(), "alpha", "bravo", "charlie", "delta", "echo", seq, mNumbers[1], mNumbers[2], mNumbers[3], mNumbers[4]);
                    continue;
                }

                const uint32_t measured{ index - mWarmup };
                if (measured >= static_cast<uint32_t>(mSamples.size()))
                    return;

                const Clock::time_point start{ Clock::now() };
                if (measured == 0u)
                {
                    mBegin = start;
                }

                LOG_DBG(mFormat.c_str(), "alpha", "bravo", "charlie", "delta", "echo", seq, mNumbers[1], mNumbers[2], mNumbers[3], mNumbers[4]);
                mSamples[measured] = elapsed_ns(start);
                if ((measured + 1u) == static_cast<uint32_t>(mSamples.size()))
                {
                    mDurationNs = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - mBegin).count());
                    mDone.set_signaled();
                }
            }
        }

        bool post_event(areg::Event & eventElem) override
        {
            return areg::EventDispatcher::post_event(eventElem);
        }

    private:
        areg::Timer         mTimer;     //!< The timer that paces the messages.
        const uint32_t      mPeriod;    //!< Milliseconds between two messages.
        const uint32_t      mWarmup;    //!< Messages written before the measured part.
        const uint32_t      mBurst;     //!< Messages written on each timer event.
        uint32_t            mFired;     //!< Messages written so far.
        Clock::time_point   mBegin;     //!< The start of the measured part.
        areg::SyncEvent     mDone;      //!< Signaled when the last measured message is written.
    };

    //!< Returns the sample at the given fraction of the sorted samples.
    double percentile(const std::vector<uint32_t> & sorted, double fraction)
    {
        const std::size_t index{ static_cast<std::size_t>(fraction * static_cast<double>(sorted.size() - 1u)) };
        return static_cast<double>(sorted[index]);
    }

    //!< Measures the cost of one clock reading, which every sample contains.
    double clock_cost_ns()
    {
        constexpr uint32_t rounds{ 100'000u };
        const Clock::time_point begin{ Clock::now() };
        for (uint32_t i = 0u; i < rounds; ++i)
        {
            static_cast<void>(Clock::now());
        }

        return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - begin).count()) / rounds;
    }

    //!< Waits until the log collector gave this source its cookie.
    bool wait_connected()
    {
        for (uint32_t waited = 0u; waited < CONNECT_MS; waited += 10u)
        {
            if (areg::cookie() != areg::COOKIE_UNKNOWN)
                return true;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        return false;
    }

    //!< Writes the messages as fast as possible from the given number of threads.
    void run_threads(const Options & opt, const std::string & format, std::vector<uint32_t> & all, uint64_t & durationNs)
    {
        std::vector<std::unique_ptr<LogThread>> threads;
        for (uint32_t i = 0u; i < opt.threads; ++i)
        {
            threads.emplace_back(std::make_unique<LogThread>("LogSource_" + std::to_string(i), opt, format));
        }

        for (auto & thread : threads)
        {
            thread->start(areg::WAIT_INFINITE);
        }

        for (auto & thread : threads)
        {
            static_cast<void>(thread->shutdown(areg::WAIT_INFINITE));
            all.insert(all.end(), thread->samples().begin(), thread->samples().end());
            durationNs = std::max(durationNs, thread->duration_ns());
        }
    }

    //!< Writes one message per period from a timer.
    void run_paced(const Options & opt, const std::string & format, std::vector<uint32_t> & all, uint64_t & durationNs)
    {
        PacedLogger paced(opt, format);
        if (paced.start(areg::WAIT_INFINITE))
        {
            paced.run();
            paced.trigger_exit();
            static_cast<void>(paced.shutdown(areg::WAIT_INFINITE));
            all.insert(all.end(), paced.samples().begin(), paced.samples().end());
            durationNs = paced.duration_ns();
        }
    }
}

int main(int argc, char ** argv)
{
    Options opt;
    if (parse_options(argc, argv, opt) == false)
        return 1;

    areg::Application::setup(true, false, false, true, false);
    if (wait_connected() == false)
    {
        std::printf("35_logsource: the log collector is not reachable, start logcollector first.\n");
        areg::Application::release();
        return 2;
    }

    const std::string format{ make_format(opt.strings, opt.numbers) };
    std::vector<uint32_t> all;
    all.reserve(static_cast<std::size_t>(opt.count) * opt.threads);
    uint64_t durationNs{ 0u };
    if (opt.period != 0u)
    {
        run_paced(opt, format, all, durationNs);
    }
    else
    {
        run_threads(opt, format, all, durationNs);
    }

    if (all.empty())
    {
        std::printf("35_logsource: no message written.\n");
        areg::Application::release();
        return 3;
    }

    std::sort(all.begin(), all.end());
    double sum{ 0.0 };
    for (uint32_t sample : all)
    {
        sum += sample;
    }

    const double rate{ durationNs != 0u ? static_cast<double>(all.size()) * 1e9 / static_cast<double>(durationNs) : 0.0 };
    std::printf("35 logsource | threads %u | strings %u | numbers %u | period %u ms | burst %u | messages %u | clock %.1f ns\n"
               , opt.threads, opt.strings, opt.numbers, opt.period, opt.burst, static_cast<uint32_t>(all.size()), clock_cost_ns());
    std::printf("35 log call  | P50 %.0f ns | P90 %.0f ns | P99 %.0f ns | P99.9 %.0f ns | max %.0f ns | mean %.0f ns | rate %.0f msg/s\n"
               , percentile(all, 0.50), percentile(all, 0.90), percentile(all, 0.99), percentile(all, 0.999)
               , static_cast<double>(all.back()), sum / static_cast<double>(all.size()), rate);
    std::fflush(stdout);

    areg::Application::release();
    return 0;
}
