#ifndef AREG_TESTS_BENCHMARKS_BENCHMARKUTILS_HPP
#define AREG_TESTS_BENCHMARKS_BENCHMARKUTILS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        benchmarks/BenchmarkUtils.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Aregtech
 * \brief       Areg Platform, common helpers of the Areg Framework benchmarks.
 *              All data generators are deterministic, so that every benchmark
 *              run operates on exactly the same input.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/String.hpp"

#include <cstdint>
#include <random>
#include <string>
#include <vector>

namespace bench
{
    //!< The seed used by every generator, so that the generated data never changes.
    constexpr uint32_t  RANDOM_SEED { 0x0A1E2Bu };

    /**
     * \brief   Returns a deterministic pseudo-random generator initialized with a fixed seed.
     **/
    inline std::mt19937 generator(uint32_t seed = RANDOM_SEED)
    {
        return std::mt19937(seed);
    }

    /**
     * \brief   Generates a deterministic list of integers in a pseudo-random order.
     * \param   count   The amount of the integers to generate.
     **/
    inline std::vector<int> make_integers(uint32_t count)
    {
        std::vector<int> result;
        result.reserve(count);
        std::mt19937 rnd{ generator() };
        for (uint32_t i = 0; i < count; ++i)
        {
            result.push_back(static_cast<int>(rnd() & 0x00FFFFFFu));
        }

        return result;
    }

    /**
     * \brief   Generates a deterministic ASCII word of the given length.
     * \param   rnd     The generator to take the characters from.
     * \param   length  The length of the word to generate.
     **/
    inline std::string make_word(std::mt19937& rnd, uint32_t length)
    {
        constexpr char alphabet[]{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
        constexpr uint32_t alphaLen{ static_cast<uint32_t>(sizeof(alphabet) - 1) };

        std::string result;
        result.reserve(length);
        for (uint32_t i = 0; i < length; ++i)
        {
            result.push_back(alphabet[rnd() % alphaLen]);
        }

        return result;
    }

    /**
     * \brief   Generates a deterministic list of Areg strings, each of 'length' characters.
     * \param   count   The amount of strings to generate.
     * \param   length  The length of each generated string.
     **/
    inline std::vector<areg::String> make_strings(uint32_t count, uint32_t length = 24u)
    {
        std::vector<areg::String> result;
        result.reserve(count);
        std::mt19937 rnd{ generator() };
        for (uint32_t i = 0; i < count; ++i)
        {
            result.emplace_back(make_word(rnd, length));
        }

        return result;
    }

    /**
     * \brief   Generates a deterministic text of 'words' space separated words.
     *          The word "areg" is injected every 16-th position, so that the
     *          search and replace benchmarks always have something to find.
     * \param   words   The amount of the words in the generated text.
     **/
    inline areg::String make_text(uint32_t words)
    {
        std::string result;
        result.reserve(static_cast<size_t>(words) * 8u);
        std::mt19937 rnd{ generator() };
        for (uint32_t i = 0; i < words; ++i)
        {
            if (i != 0)
            {
                result.push_back(' ');
            }

            if ((i % 16u) == 15u)
            {
                result.append("areg");
            }
            else
            {
                result.append(make_word(rnd, 3u + (rnd() % 9u)));
            }
        }

        return areg::String(result);
    }

} // namespace bench

#endif // AREG_TESTS_BENCHMARKS_BENCHMARKUTILS_HPP
