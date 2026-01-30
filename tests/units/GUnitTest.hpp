#ifndef AREG_TESTS_UNITS_GUNITTESTS_HPP
#define AREG_TESTS_UNITS_GUNITTESTS_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/GUnitTest.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Global configuration file of Areg unit tests.
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
  
#if !defined(IMP_AREG_DLL)          && \
    !defined(_IMP_AREG_DLL)         && \
    !defined(IMPORT_SHARED_SYMBOLS) && \
    !defined(_IMPORT_SHARED_SYMBOLS)&& \
    !defined(IMP_AREG_LIB)          && \
    !defined(_IMP_AREG_LIB)         && \
    !defined(IMPORT_STATIC_SYMBOLS) && \
    !defined(_IMPORT_STATIC_SYMBOLS)

    #error  "You should import Areg engine either as shared or static library"

#endif // wrong imports

#include "areg/base/GEGlobal.h"

#if defined(_MSC_VER) && (_MSC_VER > 1200)
    #pragma warning(push, 3)
    #pragma warning(disable: 26495)
    #include "gtest/gtest.h"
    #pragma warning(default: 26495)
    #pragma warning(pop)
#else   // defined(_MSC_VER) && (_MSC_VER > 1200)
    #include "gtest/gtest.h"
#endif  // defined(_MSC_VER) && (_MSC_VER > 1200)

#endif // !AREG_TESTS_UNITS_GUNITTESTS_HPP
