#ifndef AREG_EXAMPLES_32_PUBMIXED_COMMON_MIXEDDEFS_HPP
#define AREG_EXAMPLES_32_PUBMIXED_COMMON_MIXEDDEFS_HPP
/************************************************************************
 * \file        common/MixedDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit examples
 * \author      Artak Avetyan
 * \brief       Example 32: constants and options shared by the provider and the consumer.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/base/String.hpp"

#include <cstdint>
#include <string_view>

namespace mixed
{
    //!< The role name of the service.
    constexpr std::string_view  ServiceRoleName     { "MixedTrafficProvider" };

    //!< Period of the console refresh, in milliseconds.
    constexpr uint32_t          REPORT_TIMEOUT_MS   { 1'000u };

    //!< Largest number of bulk producer threads the provider may start.
    constexpr uint32_t          MAX_PRODUCERS       { 16u };

    //!< Defaults, chosen so that the example is useful on a small laptop without options.
    constexpr uint32_t          DEFAULT_BLOCK_BYTES { 256u * 1024u };  //!< one bulk block
    constexpr uint32_t          DEFAULT_PRODUCERS   { 4u };            //!< bulk threads
    constexpr uint32_t          DEFAULT_GAP_US      { 2'000u };        //!< between two blocks of one thread
    constexpr uint32_t          DEFAULT_PING_US     { 5'000u };        //!< between two pings

    //!< Number of round-trip samples the consumer keeps for the percentiles.
    constexpr uint32_t          RTT_SAMPLE_CAPACITY { 200'000u };

    /**
     * \brief   Options of the provider, taken from the command line.
     **/
    struct ProviderOptions
    {
        uint32_t    blockBytes  { DEFAULT_BLOCK_BYTES };
        uint32_t    producers   { DEFAULT_PRODUCERS };
        uint32_t    gapUs       { DEFAULT_GAP_US };
        bool        headless    { false };
        bool        showHelp    { false };
        bool        bulkOff     { false };  //!< run with no bulk traffic at all, for the reference RTT

        /**
         * \brief   Reads the options from the command line. Unknown arguments switch the help on.
         **/
        bool parse(int argc, char* argv[]);
    };

    /**
     * \brief   Options of the consumer, taken from the command line.
     **/
    struct ConsumerOptions
    {
        uint32_t    pingUs      { DEFAULT_PING_US };
        uint32_t    runSeconds  { 0u };     //!< 0 = until the user quits
        bool        headless    { false };
        bool        showHelp    { false };

        bool parse(int argc, char* argv[]);
    };

    /**
     * \brief   Prints the options of the provider.
     **/
    void print_provider_help();

    /**
     * \brief   Prints the options of the consumer.
     **/
    void print_consumer_help();

} // namespace mixed

#endif // AREG_EXAMPLES_32_PUBMIXED_COMMON_MIXEDDEFS_HPP
