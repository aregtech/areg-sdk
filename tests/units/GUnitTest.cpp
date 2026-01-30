/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/DemoTest.Cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              Google unit test dummy file, created as a fist file to integrate GTest in AREG
 ************************************************************************/
 /************************************************************************
  * Include files.
  ************************************************************************/
#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "aregextend")
#endif // _MSC_VER


/**
 * \brief   A dummy test to run to make sure that Unit Tests are running.
 **/
TEST(GUnitTestDummy, BasicAssertion)
{
    EXPECT_EQ(2*2, 4);
}

/**
 * \brief   A dummy test using Areg library to make sure that the unit tests are
 *          properly linked with Areg framework library.
 **/
TEST(GUnitTestDummy, BasicString )
{
    constexpr std::string_view _hello{ "hello areg-sdk" };

    String hello( _hello );
    hello = hello.makeUpper( );
    EXPECT_EQ( hello.compare( _hello, false ), NEMath::eCompare::Equal );
}
