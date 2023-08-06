/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/DemoTest.Cpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Google test dummy file.
 *              Created as a fist file to integrate GTest in AREG
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

TEST(DemoTest, BasicAssertion)
{
    EXPECT_EQ(2*2, 4);
}

TEST( DemoTest, BasicString )
{
    constexpr std::string_view _hello{ "hello areg-sdk" };

    String hello( _hello );
    hello = hello.makeUpper( );
    EXPECT_EQ( hello.compare( _hello, false ), NEMath::eCompare::Equal );
}
