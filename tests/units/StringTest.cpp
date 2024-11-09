#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"
#include "areg/base/NEString.hpp"
#include <cmath>
#include <limits>
/**
 * Tests makeInt32() func which converts a string
 * to type int32.
*/
class TestMakeInt32StringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, int32_t, NEString::eRadix>> {};

INSTANTIATE_TEST_SUITE_P(
    StringTestCases,
    TestMakeInt32StringConversionTest,
    ::testing::Values(
         // Decimal base 10 cases
        std::make_tuple("52", 52, NEString::eRadix::RadixDecimal),                    // Basic valid input
        std::make_tuple("  123 ", 123, NEString::eRadix::RadixDecimal),               // Leading/trailing spaces
        std::make_tuple("-2147483648", -2147483648, NEString::eRadix::RadixDecimal),  // Min 32-bit integer
        std::make_tuple("2147483647", 2147483647, NEString::eRadix::RadixDecimal),    // Max 32-bit integer

        // Hexadecimal base 16 cases
        std::make_tuple("2A", 42, NEString::eRadix::RadixHexadecimal),                    // Hexadecimal "2A" is 42 in decimal
        std::make_tuple("-7B", -123, NEString::eRadix::RadixHexadecimal),                 // Hexadecimal "-7B" is -123 in decimal
        std::make_tuple("7FFFFFFF", 2147483647, NEString::eRadix::RadixHexadecimal),      // Max 32-bit integer in hexadecimal
        std::make_tuple("-80000000", -2147483648, NEString::eRadix::RadixHexadecimal),    // Min 32-bit integer in hexadecimal

        // Octal base 8 cases
        std::make_tuple("64", 52, NEString::eRadix::RadixOctal),                     // Octal "64" is 52 in decimal
        std::make_tuple("-173", -123, NEString::eRadix::RadixOctal),                 // Octal "-173" is -123 in decimal

        // Binary base 2 cases
        std::make_tuple("110100", 52, NEString::eRadix::RadixBinary),                 // Binary "110100" is 52 in decimal
        std::make_tuple("-1111011", -123, NEString::eRadix::RadixBinary)              // Binary "-1111011" is -123 in decimal
    )
);

TEST_P(TestMakeInt32StringConversionTest, TestMakeInt32)
{
    const char* input = std::get<0>(GetParam());
    int32_t expected_value = std::get<1>(GetParam());
    NEString::eRadix radix = std::get<2>(GetParam());
    

    EXPECT_EQ(String::makeInt32(input, radix), expected_value);
}


class TestGetSubstringStringConversionTest : public ::testing::TestWithParam<std::tuple<String, const char*, std::vector<String>>> {};

INSTANTIATE_TEST_SUITE_P(
    StringTestCases,
    TestGetSubstringStringConversionTest,
    ::testing::Values(
        // Test case 1: Delimiter "0123"
        std::make_tuple(
            String("0123 456 789 0123"), 
            "0123", 
            std::vector<String>{"", " 456 789 ", ""}
        ),

        // Test case 2: Delimiter " "
        std::make_tuple(
            String("0123 456 789 0123"), 
            " ", 
            std::vector<String>{"0123", "456", "789", "0123"}
        )
    )
);

TEST_P(TestGetSubstringStringConversionTest, TestGetSubstring)
{
    String input = std::get<0>(GetParam());
    const char* delimiter = std::get<1>(GetParam());
    const std::vector<String>& expected_results = std::get<2>(GetParam());

    const char* next = static_cast<const char*>(input);

    // Iterate through expected results and check each substring extraction
    for (const auto& expected : expected_results) {
        String result = String::getSubstring(next, delimiter, &next);
        EXPECT_EQ(result, expected);
    }

    // Ensure we get an empty result at the end if there are no more delimiters
    String result = String::getSubstring(next, delimiter, &next);
    EXPECT_EQ(result, "");
}

/**
 * Tests makeUInt32() which converts a string
 * to a unsigned int32
*/
class TestMakeUInt32StringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, uint32_t, NEString::eRadix>> {};

INSTANTIATE_TEST_SUITE_P(
    StringTestCases,
    TestMakeUInt32StringConversionTest,
    ::testing::Values(
        // Valid inputs in various bases
        std::make_tuple("52", 52, NEString::eRadix::RadixDecimal),                // Decimal base 10
        std::make_tuple("110100", 52, NEString::eRadix::RadixBinary),             // Binary base 2
        std::make_tuple("64", 52, NEString::eRadix::RadixOctal),                 // Octal base 8
        std::make_tuple("34", 52, NEString::eRadix::RadixHexadecimal),                // Hexadecimal base 16

        // Edge cases
        std::make_tuple("0", 0, NEString::eRadix::RadixDecimal),                  // Minimum uint32_t value in base 10
        std::make_tuple("4294967295", 4294967295, NEString::eRadix::RadixDecimal),// Maximum uint32_t value in base 10
        std::make_tuple("FFFFFFFF", 4294967295, NEString::eRadix::RadixHexadecimal)  // Max uint32_t in hexadecimal

    )
);

TEST_P(TestMakeUInt32StringConversionTest, TestMakeUInt32)
{
    const char* input = std::get<0>(GetParam());
    uint32_t expected_value = std::get<1>(GetParam());
    NEString::eRadix radix = std::get<2>(GetParam());
    
    EXPECT_EQ(String::makeUInt32(input, radix), expected_value);
}

/**
 * Tests makeInt64() which converts a string
 * to a int64
*/

class TestMakeInt64StringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, int64_t, NEString::eRadix>> {};

INSTANTIATE_TEST_SUITE_P(
    ConvertStringToInt64Tests,
    TestMakeInt64StringConversionTest,
    ::testing::Values(
         // Decimal base 10 cases
        std::make_tuple("52", 52, NEString::eRadix::RadixDecimal),                     // Basic valid input
        std::make_tuple("+52", 52, NEString::eRadix::RadixDecimal),                    // Positive with explicit sign
        std::make_tuple("-52", -52, NEString::eRadix::RadixDecimal),                   // Negative number
        std::make_tuple("0", 0, NEString::eRadix::RadixDecimal),                       // Zero value
        std::make_tuple("9223372036854775807", 9223372036854775807, NEString::eRadix::RadixDecimal),  // Max 64-bit int
        std::make_tuple("-9223372036854775808", -9223372036854775807 - 1, NEString::eRadix::RadixDecimal), // Min 64-bit int

        // Hexadecimal base 16 cases
        std::make_tuple("2A", 42, NEString::eRadix::RadixHexadecimal),                     // Hexadecimal "2A" is 42 in decimal
        std::make_tuple("-7B", -123, NEString::eRadix::RadixHexadecimal),                  // Hexadecimal "-7B" is -123 in decimal
        std::make_tuple("7FFFFFFFFFFFFFFF", 9223372036854775807, NEString::eRadix::RadixHexadecimal),  // Max 64-bit int in hex
        std::make_tuple("-8000000000000000", -9223372036854775807 - 1, NEString::eRadix::RadixHexadecimal), // Min 64-bit int in hex

        // Octal base 8 cases
        std::make_tuple("64", 52, NEString::eRadix::RadixOctal),                      // Octal "64" is 52 in decimal
        std::make_tuple("-173", -123, NEString::eRadix::RadixOctal),                  // Octal "-173" is -123 in decimal

        // Binary base 2 cases
        std::make_tuple("110100", 52, NEString::eRadix::RadixBinary),                  // Binary "110100" is 52 in decimal
        std::make_tuple("-1111011", -123, NEString::eRadix::RadixBinary),              // Binary "-1111011" is -123 in decimal
        std::make_tuple("111111111111111111111111111111111111111111111111111111111111111", 9223372036854775807, NEString::eRadix::RadixBinary),  // Max 64-bit int in binary
        std::make_tuple("-1000000000000000000000000000000000000000000000000000000000000000", -9223372036854775807 - 1, NEString::eRadix::RadixBinary) // Min 64-bit int in binary
    )
);
TEST_P(TestMakeInt64StringConversionTest, TestMakeInt64)
{
    const char* input = std::get<0>(GetParam());
    int64_t expected_value = std::get<1>(GetParam());
    NEString::eRadix radix = std::get<2>(GetParam());

    EXPECT_EQ(String::makeInt64(input, radix), expected_value);
    
}

/**
 * Tests makeUInt64() which converts a string
 * to a unsigned int64
*/
class TestMakeUInt64StringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, uint64_t, NEString::eRadix>> {};

INSTANTIATE_TEST_SUITE_P(
    ConvertStringToInt64Tests,
    TestMakeUInt64StringConversionTest,
    ::testing::Values(
        // Decimal base 10 cases
        std::make_tuple("52", 52, NEString::eRadix::RadixDecimal),                       // Basic valid input
        std::make_tuple("0", 0, NEString::eRadix::RadixDecimal),                         // Zero value
        std::make_tuple("9223372036854775808", 9223372036854775808ULL, NEString::eRadix::RadixDecimal),  // Value > 0x8000000000000000 (max signed 64-bit)
        std::make_tuple("18446744073709551616", UINT64_MAX, NEString::eRadix::RadixDecimal),  // UINT64_MAX in decimal

        // Hexadecimal base 16 cases
        std::make_tuple("2A", 42, NEString::eRadix::RadixHexadecimal),                       // Hexadecimal "2A" is 42 in decimal
        std::make_tuple("7FFFFFFFFFFFFFFF", 9223372036854775807ULL, NEString::eRadix::RadixHexadecimal),  // Max 64-bit unsigned in hex
        std::make_tuple("10000000000000000", UINT64_MAX, NEString::eRadix::RadixHexadecimal),  // Value > 0x8000000000000000 in hex
        std::make_tuple("FFFFFFFFFFFFFFFF", UINT64_MAX, NEString::eRadix::RadixHexadecimal),  // Max unsigned 64-bit in hex

        // Octal base 8 cases
        std::make_tuple("64", 52, NEString::eRadix::RadixOctal),                        // Octal "64" is 52 in decimal
        std::make_tuple("777777777777777777777", 9223372036854775807ULL, NEString::eRadix::RadixOctal), // Max 64-bit unsigned in octal

        // Binary base 2 cases
        std::make_tuple("110100", 52, NEString::eRadix::RadixBinary),                     // Binary "110100" is 52 in decimal
        std::make_tuple("1111111111111111111111111111111111111111111111111111111111111111", UINT64_MAX, NEString::eRadix::RadixBinary),  // Max 64-bit unsigned in binary
        std::make_tuple("1000000000000000000000000000000000000000000000000000000000000000", 9223372036854775808ULL, NEString::eRadix::RadixBinary) // A large binary number
    )
);
TEST_P(TestMakeUInt64StringConversionTest, TestMakeUInt64)
{
    const char* input = std::get<0>(GetParam());
    uint64_t expected_value = std::get<1>(GetParam());
    NEString::eRadix radix = std::get<2>(GetParam());
    EXPECT_EQ(String::makeUInt64(input, radix), expected_value);
}

/**
 * Tests makeFloat() which converts a string
 * to a 32 bit floating point
*/
class TestMakeFloatStringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, float>> {};

INSTANTIATE_TEST_SUITE_P(
    TestMakeFloatCases,
    TestMakeFloatStringConversionTest,
    ::testing::Values(
        std::make_tuple("3.1428571", 3.1428571f),            // Basic valid input
        std::make_tuple("   3.1428571   ", 3.1428571f),       // Leading/trailing spaces
        std::make_tuple("+3.1428571", 3.1428571f),           // Positive with explicit sign
        std::make_tuple("-3.1428571", -3.1428571f),          // Negative number
        std::make_tuple("0.0", 0.0f),                         // Zero (positive)
        std::make_tuple("-0.0", -0.0f),                       // Zero (negative)
        std::make_tuple("1.23e4", 1.23e4f),                   // Scientific notation positive
        std::make_tuple("1.23E-4", 1.23E-4f),                 // Scientific notation negative
        std::make_tuple("3.4e38", 3.4e38f),                   // Large number
        std::make_tuple("1e500", std::numeric_limits<float>::infinity()), // Overflow (Infinity)
        std::make_tuple("1e-500", 0.0f),                      // Underflow (Zero)
        std::make_tuple("nan", std::numeric_limits<float>::quiet_NaN()),  // NaN
        std::make_tuple("inf", std::numeric_limits<float>::infinity()),   // Positive infinity
        std::make_tuple("-inf", -std::numeric_limits<float>::infinity())  // Negative infinity
    )
);
TEST_P(TestMakeFloatStringConversionTest, TestMakeFloat)
{
    const auto& params = GetParam();
    const char* input = std::get<0>(params);  
    float expected = std::get<1>(params);      

    

    // Handle special cases like NaN and infinity
    if (std::isnan(expected)) {
        EXPECT_TRUE(std::isnan(String::makeFloat(input)));
    }
    else if (expected == std::numeric_limits<float>::infinity()) {
        EXPECT_EQ(String::makeFloat(input), std::numeric_limits<float>::infinity());
    }
    else if (expected == -std::numeric_limits<float>::infinity()) {
        EXPECT_EQ(String::makeFloat(input), -std::numeric_limits<float>::infinity());
    }
    else {
        // Expectation for basic string-to-float conversion
        EXPECT_FLOAT_EQ(String::makeFloat(input), expected);
    }
}

/**
 * Tests makeDouble() which converts a string
 * to a 64 bit floating point
*/
class TestMakeDoubleStringConversionTest : public ::testing::TestWithParam<std::tuple<const char*, double>> {};


INSTANTIATE_TEST_SUITE_P(
    TestMakeDoubleCases,
    TestMakeDoubleStringConversionTest,
    ::testing::Values(
        std::make_tuple("3.142857142857143", 3.142857142857143),           // Basic valid input
        std::make_tuple("   3.142857142857143   ", 3.142857142857143),      // Leading/trailing spaces
        std::make_tuple("+3.142857142857143", 3.142857142857143),          // Positive with explicit sign
        std::make_tuple("-3.142857142857143", -3.142857142857143),         // Negative number
        std::make_tuple("0.0", 0.0),                                        // Zero (positive)
        std::make_tuple("-0.0", -0.0),                                      // Zero (negative)
        std::make_tuple("1.23e4", 1.23e4),                                  // Scientific notation positive
        std::make_tuple("1.23E-4", 1.23E-4),                                // Scientific notation negative
        std::make_tuple("3.4e38", 3.4e38),                                  // Large number
        std::make_tuple("1e500", std::numeric_limits<double>::infinity()),  // Overflow (Infinity)
        std::make_tuple("1e-500", 0.0),                                     // Underflow (Zero)
        std::make_tuple("nan", std::numeric_limits<double>::quiet_NaN()),   // NaN
        std::make_tuple("inf", std::numeric_limits<double>::infinity()),    // Positive infinity
        std::make_tuple("-inf", -std::numeric_limits<double>::infinity())   // Negative infinity
    )
);

TEST_P(TestMakeDoubleStringConversionTest, TestMakeDouble)
{
    const auto& params = GetParam();
    const char* input = std::get<0>(params);  
    double expected = std::get<1>(params);    



    // Handle special cases like NaN and infinity
    if (std::isnan(expected)) {
        EXPECT_TRUE(std::isnan(String::makeDouble(input)));
    }
    else if (expected == std::numeric_limits<double>::infinity()) {
        EXPECT_EQ(String::makeDouble(input), std::numeric_limits<double>::infinity());
    }
    else if (expected == -std::numeric_limits<double>::infinity()) {
        EXPECT_EQ(String::makeDouble(input), -std::numeric_limits<double>::infinity());
    }
    else {
        // Expectation for basic string-to-double conversion
        EXPECT_DOUBLE_EQ(String::makeDouble(input), expected);
    }
}

