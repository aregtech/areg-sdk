/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/StringUtilsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Created as a fist file to integrate GTest in AREG
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

TEST( StringUtilsTest, TestBufferAutoSetWhenFormat )
{
    String str;
    str.format("I want to try some extra [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], [ %s ], , [ %s ], , [ %s ] string"
                , "extra"
                , "very extra"
                , "very-very extra"
                , "very-very-very extra"
                , "very-very-very-very extra"
                , "very-very-very-very-very extra"
                , "very-very-very-very-very-very extra"
                , "very-very-very-very-very-very-very extra"
                , "very-very-very-very-very-very-very-very extra"
    );

    ASSERT_TRUE( str.getLength( ) > static_cast<NEString::CharCount>(128) );
}
