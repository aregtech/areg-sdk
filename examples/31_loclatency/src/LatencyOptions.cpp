/************************************************************************
 * \file        src/LatencyOptions.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Collection of Areg SDK examples.
 *              Command line options of the local latency benchmark (example 31).
 ************************************************************************/

#include "src/LatencyOptions.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace
{
    struct ModeName
    {
        const char *                name;
        LocalLatency::LatencyMode   mode;
        uint32_t                    payload;
    };

    constexpr ModeName MODE_NAMES[]
    {
          { "bc0"    , LocalLatency::LatencyMode::Broadcast0    ,     0u }
        , { "bc8"    , LocalLatency::LatencyMode::Broadcast8    ,     8u }
        , { "bc16"   , LocalLatency::LatencyMode::Broadcast16   ,    16u }
        , { "bc32"   , LocalLatency::LatencyMode::Broadcast32   ,    32u }
        , { "bc64"   , LocalLatency::LatencyMode::Broadcast64   ,    64u }
        , { "bc128"  , LocalLatency::LatencyMode::Broadcast128  ,   128u }
        , { "bc256"  , LocalLatency::LatencyMode::Broadcast256  ,   256u }
        , { "bc512"  , LocalLatency::LatencyMode::Broadcast512  ,   512u }
        , { "bc1024" , LocalLatency::LatencyMode::Broadcast1024 ,  1024u }
        , { "bc4096" , LocalLatency::LatencyMode::Broadcast4096 ,  4096u }
        , { "bc65536", LocalLatency::LatencyMode::Broadcast65536, 65536u }
        , { "pp0"    , LocalLatency::LatencyMode::Request0      ,     0u }
        , { "pp8"    , LocalLatency::LatencyMode::Request8      ,     8u }
        , { "pp16"   , LocalLatency::LatencyMode::Request16     ,    16u }
        , { "pp32"   , LocalLatency::LatencyMode::Request32     ,    32u }
        , { "pp64"   , LocalLatency::LatencyMode::Request64     ,    64u }
        , { "pp128"  , LocalLatency::LatencyMode::Request128    ,   128u }
        , { "pp256"  , LocalLatency::LatencyMode::Request256    ,   256u }
        , { "pp512"  , LocalLatency::LatencyMode::Request512    ,   512u }
        , { "pp1024" , LocalLatency::LatencyMode::Request1024   ,  1024u }
        , { "pp4096" , LocalLatency::LatencyMode::Request4096   ,  4096u }
        , { "pp65536", LocalLatency::LatencyMode::Request65536  , 65536u }
    };

    //!< Splits "a,b,c" into single names and appends the matching modes to `out`.
    //!< Returns false and reports the first name that is not known.
    bool _expand_mode_list(std::string_view list, std::vector<LocalLatency::LatencyMode> & out)
    {
        size_t begin{ 0u };
        while (begin <= list.size())
        {
            const size_t end{ std::min(list.find(',', begin), list.size()) };
            const std::string_view one{ list.substr(begin, end - begin) };
            begin = end + 1u;

            if (one.empty())
                continue;

            if ((one == "all") || (one == "bc") || (one == "pp"))
            {
                for (const ModeName & entry : MODE_NAMES)
                {
                    const bool isPingPong{ loclat::is_round_trip(entry.mode) };
                    if ((one == "all") || ((one == "pp") == isPingPong))
                        out.push_back(entry.mode);
                }

                continue;
            }

            const LocalLatency::LatencyMode mode{ loclat::string_as_mode(one) };
            if (mode == LocalLatency::LatencyMode::Undefined)
            {
                std::printf("ERROR: unknown mode name '%.*s'.\n", static_cast<int>(one.size()), one.data());
                return false;
            }

            out.push_back(mode);
        }

        return true;
    }

    //!< Accepts "-x=value", "--name=value" and "name=value". Returns the value part or
    //!< an empty view when the argument does not carry '='.
    std::string_view _value_of(std::string_view arg) noexcept
    {
        const size_t pos{ arg.find('=') };
        return pos == std::string_view::npos ? std::string_view{} : arg.substr(pos + 1u);
    }

    bool _matches(std::string_view arg, std::string_view shortName, std::string_view longName) noexcept
    {
        const std::string_view head{ arg.substr(0u, std::min(arg.find('='), arg.size())) };
        return (head == shortName) || (head == longName);
    }

    bool _to_uint(std::string_view text, uint32_t & out) noexcept
    {
        if (text.empty())
            return false;

        uint64_t value{ 0u };
        for (const char ch : text)
        {
            if ((ch < '0') || (ch > '9'))
                return false;

            value = (value * 10u) + static_cast<uint64_t>(ch - '0');
            if (value > 0xFFFFFFFFull)
                return false;
        }

        out = static_cast<uint32_t>(value);
        return true;
    }
}

loclat::RunOptions & loclat::run_options()
{
    static loclat::RunOptions _theOptions;
    return _theOptions;
}

const char * loclat::mode_as_str(LocalLatency::LatencyMode mode) noexcept
{
    for (const ModeName & entry : MODE_NAMES)
    {
        if (entry.mode == mode)
            return entry.name;
    }

    return "?";
}

LocalLatency::LatencyMode loclat::string_as_mode(std::string_view name) noexcept
{
    for (const ModeName & entry : MODE_NAMES)
    {
        if (name == entry.name)
            return entry.mode;
    }

    return LocalLatency::LatencyMode::Undefined;
}

const char * loclat::topology_as_str(loclat::Topology topology) noexcept
{
    return topology == loclat::Topology::SameThread ? "same" : "cross";
}

uint32_t loclat::mode_payload_size(LocalLatency::LatencyMode mode) noexcept
{
    for (const ModeName & entry : MODE_NAMES)
    {
        if (entry.mode == mode)
            return entry.payload;
    }

    return 0u;
}

double loclat::measure_clock_cost(uint32_t samples) noexcept
{
    if (samples == 0u)
        return 0.0;

    // Warm the clock path up first, then time the loop as a whole: timing every single
    // call would measure the timing itself.
    volatile uint64_t sink{ 0u };
    for (uint32_t i = 0u; i < 1000u; ++i)
        sink = loclat::now_ns();

    const uint64_t begin{ loclat::now_ns() };
    for (uint32_t i = 0u; i < samples; ++i)
        sink = loclat::now_ns();

    const uint64_t end{ loclat::now_ns() };
    static_cast<void>(sink);
    return static_cast<double>(end - begin) / static_cast<double>(samples);
}

void loclat::print_modes()
{
    std::printf("Modes accepted by --mode:\n\n");
    std::printf("  One-way trip  (provider -> consumer, one message per sample):\n   ");
    for (const ModeName & entry : MODE_NAMES)
    {
        if (!loclat::is_round_trip(entry.mode))
            std::printf(" %s", entry.name);
    }

    std::printf("\n\n  Round trip    (consumer -> provider -> consumer, two messages per sample):\n   ");
    for (const ModeName & entry : MODE_NAMES)
    {
        if (loclat::is_round_trip(entry.mode))
            std::printf(" %s", entry.name);
    }

    std::printf("\n\n  Group names:  all, bc (every one-way mode), pp (every round trip mode)\n");
    std::printf("  The number in the name is the size of the extra payload in bytes.\n");
}

void loclat::print_usage()
{
    std::printf(
        "Areg SDK example 31 -- local latency benchmark.\n"
        "\n"
        "Measures how long one message needs inside a single process: between two\n"
        "components of the same thread, and between two components of two threads.\n"
        "The message router is never involved, so nothing else must be started.\n"
        "The program is not interactive: it runs, prints the result and exits.\n"
        "\n"
        "Usage: 31_loclatency [options]\n"
        "\n"
        "  -t, --topology=same|cross  Where provider and consumer run. Default: same\n"
        "                             same  = both in one component thread\n"
        "                             cross = each in its own component thread\n"
        "  -m, --mode=<list>          Comma separated mode names, or the group names\n"
        "                             all, bc, pp. Default: pp0,bc0\n"
        "  -c, --count=<number>       Measured messages per run. Default: 100000\n"
        "  -w, --warmup=<number>      Messages sent before measuring. Default: 10000\n"
        "  -r, --repeat=<number>      How often the whole mode list is measured. Default: 1\n"
        "  -o, --csv=<path>           Append the results to this CSV file. Default: none\n"
        "  -l, --label=<text>         Free text written into every CSV line.\n"
        "      --list                 Print the accepted mode names and exit.\n"
        "  -q, --quiet                Print only the result table.\n"
        "  -h, --help                 Print this text and exit.\n"
        "\n"
        "Examples:\n"
        "  31_loclatency\n"
        "  31_loclatency -t=cross -m=pp0,bc0 -c=200000 -w=20000\n"
        "  31_loclatency -t=same -m=all -c=50000 -o=result.csv -l=baseline\n"
        "\n"
        "Compare with example 30: example 30 measures the same modes between two\n"
        "processes. The difference between the two results is the cost of leaving\n"
        "the process: two sockets and the message router.\n");
}

bool loclat::parse_command_line(int argc, char ** argv, loclat::RunOptions & outOptions, bool & outExitNow)
{
    loclat::RunOptions options;
    areg::String modeList{ "pp0,bc0" };
    outExitNow = false;

    for (int i = 1; i < argc; ++i)
    {
        const std::string_view arg{ argv[i] };
        const std::string_view value{ _value_of(arg) };

        if (_matches(arg, "-h", "--help") || (arg == "help"))
        {
            loclat::print_usage();
            outExitNow = true;
            return true;
        }
        else if (arg == "--list")
        {
            loclat::print_modes();
            outExitNow = true;
            return true;
        }
        else if (_matches(arg, "-q", "--quiet"))
        {
            options.mQuiet = true;
        }
        else if (_matches(arg, "-t", "--topology"))
        {
            if (value == "same")
            {
                options.mTopology = loclat::Topology::SameThread;
            }
            else if (value == "cross")
            {
                options.mTopology = loclat::Topology::CrossThread;
            }
            else
            {
                std::printf("ERROR: --topology accepts only 'same' or 'cross'.\n\n");
                loclat::print_usage();
                return false;
            }
        }
        else if (_matches(arg, "-m", "--mode"))
        {
            modeList = areg::String(value);
        }
        else if (_matches(arg, "-c", "--count"))
        {
            if (!_to_uint(value, options.mCount) || (options.mCount == 0u))
            {
                std::printf("ERROR: --count needs a number above zero.\n\n");
                loclat::print_usage();
                return false;
            }
        }
        else if (_matches(arg, "-w", "--warmup"))
        {
            if (!_to_uint(value, options.mWarmup))
            {
                std::printf("ERROR: --warmup needs a number.\n\n");
                loclat::print_usage();
                return false;
            }
        }
        else if (_matches(arg, "-r", "--repeat"))
        {
            if (!_to_uint(value, options.mRepeat) || (options.mRepeat == 0u))
            {
                std::printf("ERROR: --repeat needs a number above zero.\n\n");
                loclat::print_usage();
                return false;
            }
        }
        else if (_matches(arg, "-o", "--csv"))
        {
            options.mCsvPath = areg::String(value);
        }
        else if (_matches(arg, "-l", "--label"))
        {
            options.mLabel = areg::String(value);
        }
        else
        {
            std::printf("ERROR: unknown option '%.*s'.\n\n", static_cast<int>(arg.size()), arg.data());
            loclat::print_usage();
            return false;
        }
    }

    if (!_expand_mode_list(std::string_view{ modeList.as_string(), static_cast<size_t>(modeList.length()) }, options.mModes))
    {
        std::printf("\n");
        loclat::print_modes();
        return false;
    }

    if (options.mModes.empty())
    {
        std::printf("ERROR: the mode list is empty.\n\n");
        loclat::print_modes();
        return false;
    }

    outOptions = options;
    return true;
}
