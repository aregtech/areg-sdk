#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
/**
 * Tests makeInt32() func which converts a string
 * to type int32.
*/
TEST(StringTest, TestMakeInt32)
{
    constexpr char *test = "52";
    constexpr int32_t result_int32 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int32);
    
    constexpr int16_t result_int16 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int16);
    EXPECT_NE(typeid(String::makeInt32(test)), typeid(result_int16));
}

TEST(StringTest, TestGetSubstring)
{
    String test("0123 456 789 0123");
    const char *next = static_cast<const char *>(test);
    String result = String::getSubstring(next, "0123", &next);
    EXPECT_EQ(result, "");

    result = String::getSubstring(next, "0123", &next);
    EXPECT_EQ(result, " 456 789 ");

    result = String::getSubstring(next, "0123", &next);
    EXPECT_EQ(result, "");

    next = static_cast<const char *>(test);
    result = String::getSubstring(next, " ", &next);
    EXPECT_EQ(result, "0123");

    result = String::getSubstring(next, " ", &next);
    EXPECT_EQ(result, "456");

    result = String::getSubstring(next, " ", &next);
    EXPECT_EQ(result, "789");
    
    result = String::getSubstring(next, " ", &next);
    EXPECT_EQ(result, "0123");
}

/**
 * Tests makeUInt32() which converts a string
 * to a unsigned int32
*/
TEST(StringTest, TestMakeUInt32)
{
    constexpr char *test = "52";
    constexpr uint32_t result_uint32 = 52;
    EXPECT_EQ(String::makeUInt32(test), result_uint32);
    
    constexpr uint16_t result_uint16 = 52;
    EXPECT_EQ(String::makeUInt32(test), result_uint16);
}

/**
 * Tests makeInt64() which converts a string
 * to a int64
*/
struct TestParams{
    const char *input;
    int64_t expected_result;
};
class StringConversionTest : public ::testing::TestWithParam<TestParams> {};

INSTANTIATE_TEST_SUITE_P(
    ConvertStringToInt64Tests,
    StringConversionTest,
    ::testing::Values(
        TestParams{"52", 52},
        TestParams{"+52", 52},
        TestParams{"-52", -52},
        TestParams{"0", 0}
    )
);
TEST_P(StringConversionTest, TestMakeInt64)
{
    const auto &params = GetParam();
    EXPECT_EQ(String::makeInt64(params.input), params.expected_result);
    
}

/**
 * Tests makeUInt64() which converts a string
 * to a unsigned int64
*/
TEST(StringTest, TestMakeUInt64)
{
    constexpr char *test = "52";
    constexpr uint64_t result_uint64 = 52;
    EXPECT_EQ(String::makeUInt64(test), result_uint64);
    
    constexpr uint16_t result_uint16 = 52;
    EXPECT_EQ(String::makeUInt64(test), result_uint16);
}

/**
 * Tests makeFloat() which converts a string
 * to a 32 bit floating point
*/
TEST(StringTest, TestMakeFloat)
{
    constexpr char *test = "3.1428571";
    constexpr float result_float = 3.1428571;
    EXPECT_EQ(String::makeFloat(test), result_float);
}

/**
 * Tests makeDouble() which converts a string
 * to a 64 bit floating point
*/
TEST(StringTest, TestMakeDouble)
{
    constexpr char *test = "3.142857142857143";
    constexpr double result_double = 3.142857142857143;
    EXPECT_EQ(String::makeDouble(test), result_double);
}