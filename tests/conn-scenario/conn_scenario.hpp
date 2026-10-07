/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        conn-scenario/conn_scenario.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, options and block helpers shared by the provider and the consumer
 *              of the connection scenario harness.
 ************************************************************************/

#ifndef AREG_TESTS_CONN_SCENARIO_CONN_SCENARIO_HPP
#define AREG_TESTS_CONN_SCENARIO_CONN_SCENARIO_HPP

#include "areg/base/areg_global.h"

#include <chrono>
#include <cstdint>
#include <cstring>
#include <string>

namespace scenario
{
    constexpr char const    ROLE_PROVIDER[]     { "ConnScenarioProvider" };
    constexpr char const    ROLE_ATTRIBUTE[]    { "ConnScenarioAttribute" };
    constexpr uint32_t      ATTRIBUTE_PERIOD_MS { 20u };
    constexpr char const    PROVIDER_MODEL[]    { "ConnScenarioProviderModel" };
    constexpr char const    CONSUMER_MODEL[]    { "ConnScenarioConsumerModel" };
    constexpr uint32_t      STALL_REPORT_MS     { 200u };

    struct Options
    {
        std::string config  { };
        std::string name    { "A" };
        uint32_t    bytes   { 256u };
        uint32_t    gapUs   { 1'000u };
    };

    inline Options & options()
    {
        static Options _options;
        return _options;
    }

    inline uint64_t now_ms()
    {
        return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    //!< The content of byte \a i of block \a seq; bytes 0-3 hold the number itself.
    inline uint8_t pattern(uint32_t seq, uint32_t i)
    {
        return static_cast<uint8_t>((seq * 31u) + i);
    }

    inline void fill_block(uint8_t * data, uint32_t size, uint32_t seq)
    {
        for (uint32_t i = 0u; i < size; ++i)
        {
            data[i] = pattern(seq, i);
        }

        if (size >= sizeof(uint32_t))
        {
            std::memcpy(data, &seq, sizeof(uint32_t));
        }
    }

    inline bool check_block(const uint8_t * data, uint32_t size, uint32_t seq)
    {
        uint32_t head{ 0u };
        if (size >= sizeof(uint32_t))
        {
            std::memcpy(&head, data, sizeof(uint32_t));
            if (head != seq)
                return false;
        }

        for (uint32_t i = sizeof(uint32_t); i < size; ++i)
        {
            if (data[i] != pattern(seq, i))
                return false;
        }

        return true;
    }
}

#endif // AREG_TESTS_CONN_SCENARIO_CONN_SCENARIO_HPP
