/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/base/private/StringDefs.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, character classification and case-conversion
 *              utilities for the first 256 UTF-8 code points.
 *
 * \note        The three lookup tables (UTF8_256_Table, UTF8_256_Table_lower,
 *              UTF8_256_Table_upper) live in TableDefs.hpp under the
 *              areg::detail namespace as inline constexpr arrays.  Because all
 *              table data is visible at compile time, utf8_char_def(),
 *              make_lower_char(), and make_upper_char() are constexpr-
 *              compatible.  Add constexpr to their declarations in
 *              StringDefs.hpp to enable compile-time evaluation at call sites.
 *
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/

#include "areg/base/StringDefs.hpp"
