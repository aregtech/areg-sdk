#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

TEST(StringTest, makeInt32)
{
    const char *test = "52";
    int32_t result_int32 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int32);
    EXPECT_EQ(typeid(String::makeInt32(test)), typeid(result_int32));
    
    int16_t result_int16 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int16);
    EXPECT_NE(typeid(String::makeInt32(test)), typeid(result_int16));
}

TEST(StringTest, getSubstring)
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

TEST(StringTest, makeUInt32)
{
    const char *test = "52";
    uint32_t result_uint32 = 52;
    EXPECT_EQ(String::makeUInt32(test), result_uint32);
    EXPECT_EQ(typeid(String::makeUInt32(test)), typeid(result_uint32));
    
    uint16_t result_uint16 = 52;
    EXPECT_EQ(String::makeUInt32(test), result_uint16);
    EXPECT_NE(typeid(String::makeUInt32(test)), typeid(result_uint16));
}