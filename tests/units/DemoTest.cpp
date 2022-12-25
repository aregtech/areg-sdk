/************************************************************************
 * \file        tests/units/DemoTest.Cpp
 * \ingroup     platform dependent files
 * \brief       AREG Framework, DemoTest to check the AREG engine and 
 *              google test compilation
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
