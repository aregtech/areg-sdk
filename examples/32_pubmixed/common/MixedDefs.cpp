/************************************************************************
 * \file        common/MixedDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: command line parsing shared by the provider and the consumer.
 ************************************************************************/

#include "common/MixedDefs.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace
{
    bool _value_of(const char* arg, const char* name, uint32_t& value)
    {
        const size_t len{ ::strlen(name) };
        if (::strncmp(arg, name, len) != 0)
            return false;

        value = static_cast<uint32_t>(::strtoul(arg + len, nullptr, 10));
        return true;
    }
}

bool mixed::ProviderOptions::parse(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        const char* a{ argv[i] };
        uint32_t v{ 0u };

        if      (_value_of(a, "-b=", v)) { blockBytes = v; }
        else if (_value_of(a, "-p=", v)) { producers  = v; }
        else if (_value_of(a, "-g=", v)) { gapUs      = v; }
        else if ((::strcmp(a, "-n") == 0) || (::strcmp(a, "--headless") == 0)) { headless = true; }
        else if ((::strcmp(a, "-q") == 0) || (::strcmp(a, "--quiet")    == 0)) { bulkOff  = true; }
        else { showHelp = true; }
    }

    if (producers  == 0u)                    producers  = 1u;
    if (producers  > mixed::MAX_PRODUCERS)   producers  = mixed::MAX_PRODUCERS;
    if (blockBytes < 64u)                    blockBytes = 64u;

    return (showHelp == false);
}

bool mixed::ConsumerOptions::parse(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        const char* a{ argv[i] };
        uint32_t v{ 0u };

        if      (_value_of(a, "-i=", v)) { pingUs     = v; }
        else if (_value_of(a, "-r=", v)) { runSeconds = v; }
        else if ((::strcmp(a, "-n") == 0) || (::strcmp(a, "--headless") == 0)) { headless = true; }
        else { showHelp = true; }
    }

    if (pingUs == 0u)
        pingUs = 1u;

    return (showHelp == false);
}

void mixed::print_provider_help()
{
    ::printf("\nExample 32 provider - bulk stream and small requests on one connection.\n\n"
             "  -b=BYTES   size of one bulk block           (default %u)\n"
             "  -p=COUNT   bulk producer threads, max %u     (default %u)\n"
             "  -g=USEC    gap between two blocks of one thread, microseconds (default %u)\n"
             "  -q         no bulk traffic at all - use it to measure the reference round-trip\n"
             "  -n         headless: no console, print a line per second to stdout\n\n"
             "  Example:  32_pubservice -b=262144 -p=4 -g=2000\n\n"
             , mixed::DEFAULT_BLOCK_BYTES, mixed::MAX_PRODUCERS
             , mixed::DEFAULT_PRODUCERS, mixed::DEFAULT_GAP_US);
}

void mixed::print_consumer_help()
{
    ::printf("\nExample 32 consumer - measures the round-trip time while the bulk stream runs.\n\n"
             "  -i=USEC    gap between two pings, microseconds (default %u)\n"
             "  -r=SEC     stop after this many seconds        (default: run until quit)\n"
             "  -n         headless: no console, print a line per second to stdout\n\n"
             "  Example:  32_pubclient -i=5000 -r=30 -n\n\n"
             , mixed::DEFAULT_PING_US);
}
