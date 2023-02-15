#include "units/GUnitTest.hpp"
#include "areg/base/String.hpp"

TEST(StringTest, makeInt32)
{
    char *test = "52";
    int32_t result_int32 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int32);
    EXPECT_EQ(typeid(String::makeInt32(test)), typeid(result_int32));
    
    int16_t result_int16 = 52;
    EXPECT_EQ(String::makeInt32(test), result_int16);
    EXPECT_NE(typeid(String::makeInt32(test)), typeid(result_int16));

}