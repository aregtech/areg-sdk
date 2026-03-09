#ifndef AREG_BASE_TABLEDEFS_HPP
#define AREG_BASE_TABLEDEFS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/TableDefs.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, compile-time lookup tables for CRC and other
 *              hash utilities. Include via MathDefs.hpp; do not include directly.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"

#include <array>
#include <cstdint>

namespace areg {

/************************************************************************/
// CRC32 IEEE 802.3 table generation
/************************************************************************/

/**
 * \brief   Computes one entry of the CRC32 IEEE 802.3 lookup table for the
 *          given input byte. Uses the reflected (LSB-first) polynomial
 *          representation 0xEDB88320, which is the bit-reversal of the
 *          canonical polynomial 0x04C11DB7.
 *
 *          CRC polynomial:
 *          X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X+1
 *
 * \param   byte    Input byte value (0–255) for which to compute the entry.
 * \return  32-bit CRC32 table entry corresponding to the given byte.
 **/
[[nodiscard]]
constexpr uint32_t crc32_table_entry(uint8_t byte) noexcept
{
    uint32_t crc { static_cast<uint32_t>(byte) };
    for (int bit = 0; bit < 8; ++bit)
    {
        // Branchless: -(crc & 1u) is 0x00000000 when LSB=0, 0xFFFFFFFF when LSB=1.
        // Equivalent to: if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320; else crc >>= 1;
        crc = (crc >> 1) ^ (0xEDB88320u & static_cast<uint32_t>(0u - (crc & 1u)));
    }

    return crc;
}

/**
 * \brief   Generates the full 256-entry CRC32 IEEE 802.3 lookup table at compile time.
 *          Called once by the CRC32_TABLE initializer; not intended for direct use.
 *
 * \return  std::array of 256 uint32_t entries forming the complete CRC32 lookup table.
 **/
[[nodiscard]]
constexpr std::array<uint32_t, 256> make_crc32_table() noexcept
{
    std::array<uint32_t, 256> table { };
    for (uint32_t i = 0u; i < 256u; ++i)
    {
        table[i] = crc32_table_entry(static_cast<uint8_t>(i));
    }

    return table;
}

/**
 * \brief   CRC32 IEEE 802.3 lookup table, fully evaluated at compile time.
 *          256 entries × 4 bytes = 1 024 bytes in read-only data.
 *
 *          Declared inline constexpr so that exactly one copy exists in the
 *          final binary regardless of how many translation units include this
 *          header.  All crc32_calculate() and crc32_start() functions in
 *          MathDefs.hpp read from this table.
 **/
inline constexpr std::array<uint32_t, 256> CRC32_TABLE { make_crc32_table() };

} // namespace areg

#endif  // AREG_BASE_TABLEDEFS_HPP
