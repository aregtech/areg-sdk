/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/MathDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Basic Math functionalities implementation.
 *              Functions in this namespace are global
 *
 ************************************************************************/

#include "areg/base/MathDefs.hpp"

// All crc32_calculate() and crc32_start() functions are now inline constexpr
// in MathDefs.hpp and evaluated at compile time when called with constant
// expressions.  No runtime implementations remain in this translation unit.
//
// The CRC32 lookup table is generated at compile time in TableDefs.hpp
// (areg::CRC32_TABLE) and stored as a single inline constexpr
// std::array<uint32_t, 256> shared across all translation units.
