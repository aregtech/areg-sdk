/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        units/NEStringTest.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, AREG framework unit test file.
 *              The unit test for methods in NEString namespace.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "gtest/gtest-param-test.h"

#include "areg/base/NEString.hpp"

/**
 * \brief   Test getStringLength and isEmpty methods of NEString
 **/
TEST(NEStringTest, StringLength)
{
    struct SimpleString
    {
        const char* str;
        uint32_t    len;
    };

    constexpr SimpleString params[]
    {
          {"12345"  , 5}
        , {"654987" , 6}
        , {nullptr  , 0}
        , {""       , 0}
        , {"abcdefg", 7}
        , {"ABCDEFG", 7}
        , {"ABCdefg", 7}
        , {"\n"     , 1}
        , {"\r"     , 1}
        , {"\n\r"   , 2}
        , {"\r\n"   , 2}
    };
    constexpr uint32_t count{ MACRO_ARRAYLEN(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(NEString::getStringLength<char>(entry.str), static_cast<NEString::CharCount>(entry.len));
        EXPECT_TRUE(NEString::isEmpty<char>(entry.str) == (entry.len == 0));
    }
}


/**
 * \brief   Test getStringLineLength and isEmpty methods of NEString
 **/
TEST(NEStringTest, StringLineLength)
{
    struct SimpleString
    {
        const char* str;
        uint32_t    len;
    };

    constexpr SimpleString params[]
    {
          {"12345"                  , 5}
        , {"12345\n67890\n"         , 5}
        , {"12345\n\r67890\n\r"     , 5}
        , {"12345\r67890\r"         , 5}
        , {"12345\r\n67890\r\n"     , 5}
        , {"12345\n67890\n"         , 5}
        , {nullptr                  , 0}
        , {""                       , 0}
        , {"\n12345\n67890\n"       , 0}
        , {"\n\r12345\n\r67890\n\r" , 0}
        , {"\r12345\r67890\r"       , 0}
        , {"\r\n12345\r\n67890\r\n" , 0}
        , {"\t12345\n67890\n"       , 6}
        , {"\v12345\n\r67890\n\r"   , 6}
        , {"\b12345\r67890\r"       , 0}
        , {"\f12345\r\n67890\r\n"   , 0}
    };
    constexpr uint32_t count{ MACRO_ARRAYLEN(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(NEString::getStringLineLength<char>(entry.str), static_cast<NEString::CharCount>(entry.len));
    }
}

/**
 * \brief   Test methods of checking lower and upper cases in NEString
 **/
TEST(NEStringTest, LowerUpperNumericCase)
{
    constexpr char chNumMin{ '0' };
    constexpr char chNumMax{ '9' };

    constexpr char chUpperMin{ 'A' };
    constexpr char chUpperMax{ 'Z' };
    constexpr char chUpperMinSpec1{ static_cast<char>(0xC0)};
    constexpr char chUpperMaxSpec1{ static_cast<char>(0xD6) };
    constexpr char chUpperMinSpec2{ static_cast<char>(0xD8) };
    constexpr char chUpperMaxSpec2{ static_cast<char>(0xDE) };

    constexpr char chLowerMin{ 'a' };
    constexpr char chLowerMax{ 'z' };
    constexpr char chLowerMinSpec1{ static_cast<char>(0xE0) };
    constexpr char chLowerMaxSpec1{ static_cast<char>(0xF6) };
    constexpr char chLowerMinSpec2{ static_cast<char>(0xF8) };
    constexpr char chLowerMaxSpec2{ static_cast<char>(0xFF) };

    for (uint32_t i = 0; i <= 0xFFu; ++i)
    {
        char ch    = static_cast<char>(i);
        char lower = NEString::makeLower<char>(ch);
        char upper = NEString::makeUpper<char>(ch);

        if ((ch >= chNumMin) && (ch <= chNumMax))
        {
            // is numeric
            EXPECT_TRUE(NEString::isNumeric<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
            EXPECT_FALSE(NEString::isLetter<char>(ch));
        }
        else if ((ch >= chUpperMin) && (ch <= chUpperMax))
        {
            // is upper
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec1) && (ch <= chUpperMaxSpec1))
        {
            // is upper special 1
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec2) && (ch <= chUpperMaxSpec2))
        {
            // is upper special 2
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMin) && (ch <= chLowerMax))
        {
            // is lower
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec1) && (ch <= chLowerMaxSpec1))
        {
            // is lower special 1
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec2) && (ch <= chLowerMaxSpec2))
        {
            // is lower special 2
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
        }
        else if ((ch == static_cast<char>(0x8A)) ||
                 (ch == static_cast<char>(0x8C)) ||
                 (ch == static_cast<char>(0x8E)) ||
                 (ch == static_cast<char>(0x9F)) )
        {
            // is upper, control --> 138 (Š), 140 (Œ), 142 (Ž), 159 (Ÿ)
            EXPECT_TRUE(NEString::isUpper<char>(ch));
            EXPECT_FALSE(NEString::isLower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isControl<char>(ch));
        }
        else if ((ch == static_cast<char>(0x9A)) ||
                 (ch == static_cast<char>(0x9C)) ||
                 (ch == static_cast<char>(0x9E)) )
        {
            // is lower, control --> 154 (š), 156 (œ), 158 (ž)
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isControl<char>(ch));
        }
        else if (ch == static_cast<char>(0xDF))
        {
            // is German letter 'ss' --> 223 (ß), has no upper case
            EXPECT_FALSE(NEString::isNumeric<char>(ch));
            EXPECT_TRUE(NEString::isLetter<char>(ch));
            EXPECT_TRUE(NEString::isAlphanumeric<char>(ch));
            EXPECT_TRUE(NEString::isLower<char>(ch));
            EXPECT_FALSE(NEString::isUpper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_EQ(ch, upper);
        }
        else
        {
            // is symbol or special char, there is no upper or lower case
            EXPECT_FALSE(NEString::isNumeric<char>(ch))     << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(NEString::isLetter<char>(ch))      << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(NEString::isAlphanumeric<char>(ch))<< "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_TRUE(NEString::isSymbol<char>(ch)        || 
                        NEString::isWhitespace<char>(ch)    ||
                        NEString::isDelimited<char>(ch)     ||
                        NEString::isEndOfLine<char>(ch)     ||
                        NEString::isEndOfString<char>(ch)   ||
                        NEString::isControl<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
        }
    }
}

/************************************************************************
 * Parameterized tests to compare strings, with and without case sensitive
 ************************************************************************/
/**
 * \brief   Parameters of strings and expected results of the tests.
 **/
struct CompareStringParams
{
    std::string_view left;          //!< Left string
    std::string_view right;         //!< Right string
    int              count;         //!< Characters count
    int              resAllSens;    //!< Test result when compare all strings, case sensitive
    int              resAllIgnore;  //!< Test result when compare all strings, ignore case sensitive
    int              resCountSens;  //!< Test result when compare count number of characters in the strings, case sensitive
    int              resCountIgnore;//!< Test result when compare count number of characters in the strings, ignore case sensitive
};

//!< List of parameters
static CompareStringParams _listStringTestCompare[]
{
      { {"12345"}   , {"12345"}     , -2,  0,  0,  0,  0 }
    , { {"abcdefg"} , {"ABCDEFG"}   , -2,  1,  0,  1,  0 }
    , { {"ABCDEFG"} , {"abcdefg"}   , -2, -1,  0, -1,  0 }
    , { {"ABCdefg"} , {"ABcdeFG"}   , -2, -1,  0, -1,  0 }
    , { {"ABcdEFG"} , {"AbcdEFG"}   , -2, -1,  0, -1,  0 }
    , { {"abcdEFG"} , {"abCdEFG"}   , -2,  1,  0,  1,  0 }
    , { {"12345"}   , {"123"}       ,  3,  1,  1,  0,  0 }
    , { {"ABCdefg"} , {"ABcdEFG"}   ,  3, -1,  0, -1,  0 }
    , { {"ABcdEFG"} , {"ABc"}       ,  3,  1,  1,  0,  0 }
    , { {"abcdEFG"} , {"abCd"}      ,  3,  1,  1,  1,  0 }
    , { {"abcdefg"} , {"abCdefg"}   ,  3,  1,  0,  1,  0 }
    , { {"abc"}     , {"abCdefg"}   ,  3,  1, -1,  1,  0 }
};

//!< Declare parameters.
struct StringTestCompare : public ::testing::TestWithParam<CompareStringParams>
{
    CompareStringParams params;
};
/**
 * \brief   Compares strings with and without case sensitive, all characters or only `count` numbers of characters.
 **/
TEST_P(StringTestCompare, CompareStrings)
{
    const CompareStringParams& param = GetParam();
    const char* left{ param.left.data() };
    const char* right{ param.right.data() };

    NEMath::eCompare result{};

    result = NEString::compare<char, char>(left, right);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resAllSens));

    result = NEString::compareIgnoreCase<char>(left, right);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resAllIgnore));

    result = NEString::compare<char, char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resCountSens));

    result = NEString::compareIgnoreCase<char, char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resCountIgnore));

    result = NEString::compareFast<char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resCountSens));

    result = NEString::compareStrings<char, char>(left, right, param.count, true);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resCountSens));

    result = NEString::compareStrings<char, char>(left, right, param.count, false);
    EXPECT_EQ(result, static_cast<NEMath::eCompare>(param.resCountIgnore));
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestCompare, ::testing::ValuesIn<CompareStringParams>(_listStringTestCompare));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestCompare, ::testing::ValuesIn<CompareStringParams>(_listStringTestCompare));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to trim white-space int the string buffer
 ************************************************************************/
/**
 * \brief   Parameters of strings and expected results of the tests.
 **/
struct TrimStringsParams
{
    std::string_view    noTrim;     //!< Initial string to trim
    std::string_view    trimLeft;   //!< The result to trim left
    std::string_view    trimRight;  //!< The result to trim right
    std::string_view    trimAll;    //!< The result to trim all sides.
    int                 count;      //!< The number of characters in the string to trim. NEString::COUNT_ALL means to take the complete string. Otherwise, only a part of string is taken.
};

//!< The list of parameters: the string to test and the expected results to compare
static constexpr TrimStringsParams _listTrimString[]
{
      { {"One String"       }   , {"One String"     }   , {"One String"     }   , {"One String"     }, -2 } // 0
    , { {"  Two  String"    }   , {"Two  String"    }   , {"  Two  String"  }   , {"Two  String"    }, -2 } // 1
    , { {"Three String   "  }   , {"Three String   "}   , {"Three String"   }   , {"Three String"   }, -2 } // 2
    , { {"  Four String   " }   , {"Four String   " }   , {"  Four String"  }   , {"Four String"    }, -2 } // 3
    , { {"                " }   , {""               }   , {""               }   , {""               }, -2 } // 4
    , { {"  \n            " }   , {""               }   , {""               }   , {""               }, -2 } // 5
    , { {"  \r            " }   , {""               }   , {""               }   , {""               }, -2 } // 6
    , { {"  \r\n          " }   , {""               }   , {""               }   , {""               }, -2 } // 7
    , { {"  \n  \n        " }   , {""               }   , {""               }   , {""               }, -2 } // 8
    , { {"  \r  \r        " }   , {""               }   , {""               }   , {""               }, -2 } // 9
    , { {"  \n\r\n\r\n    " }   , {""               }   , {""               }   , {""               }, -2 } // 10
    , { {"One String"       }   , {"One String"     }   , {"OneString"      }   , {"OneString"      },  4 } // 11
    , { {"  Two  String"    }   , {"Two  String"    }   , {"  TwoString"    }   , {"TwoString"      },  7 } // 12
    , { {"Three String   "  }   , {"Three String   "}   , {"ThreeString   " }   , {"ThreeString   " },  6 } // 13
    , { {"  Four String "   }   , {"Four String "   }   , {"  Four String " }   , {"Four String "   },  6 } // 14
    , { {"  \n\r\n\r\n    " }   , {"\n\r\n    "     }   , {"\n\r\n    "     }   , {"\n\r\n    "     },  4 } // 15
    , { { "SomeString"      }   , {"SomeString"     }   , {"SomeString"     }   , {"SomeString"     },  4 } // 16
    , { { "A         "      }   , {"A         "     }   , {"A         "     }   , {"A         "     },  1 } // 17
    , { { "         A"      }   , {"A"              }   , {"A"              }   , {"A"              },  9 } // 18
    , { { "A   B     "      }   , {"A   B     "     }   , {"A   B     "     }   , {"A   B     "     },  1 } // 19
    , { { "A   B     "      }   , {"A   B     "     }   , {"AB     "        }   , {"AB     "        },  4 } // 20
    , { {"Three String   "  }   , {"Three String   "}   , {"Three String   "}   , {"Three String   "},  5 } // 21
    , { {" Any   String  "  }   , {"Any   String  " }   , {" Any String  "  }   , {"Any String  "   },  6 } // 22
};

//!< Declare parameters.
struct StringTestTrim : public ::testing::TestWithParam<TrimStringsParams>
{
    TrimStringsParams params;
};
/**
 * \brief   Trim strings, if needed takes only `count` number of characters in the string.
 *          The results are compared with other strings.
 **/
TEST_P(StringTestTrim, TrimStrings)
{
    const TrimStringsParams& param = GetParam();
    char buffer[64];

    NEString::CharCount count{};

    count = NEString::copyString<char, char>(buffer, 64, param.noTrim.data(), NEString::COUNT_ALL);
    EXPECT_EQ(count, static_cast<NEString::CharCount>(param.noTrim.length()));
    NEString::trimLeft<char>(buffer, param.count);
    EXPECT_EQ(NEString::compare(buffer, param.trimLeft.data()), NEMath::eCompare::Equal);

    count = NEString::copyString<char, char>(buffer, 64, param.noTrim.data(), NEString::COUNT_ALL);
    EXPECT_EQ(count, static_cast<NEString::CharCount>(param.noTrim.length()));
    NEString::trimRight<char>(buffer, param.count);
    EXPECT_EQ(NEString::compare(buffer, param.trimRight.data()), NEMath::eCompare::Equal);

    count = NEString::copyString<char, char>(buffer, 64, param.noTrim.data(), NEString::COUNT_ALL);
    EXPECT_EQ(count, static_cast<NEString::CharCount>(param.noTrim.length()));
    NEString::trimAll<char>(buffer, param.count);
    EXPECT_EQ(NEString::compare(buffer, param.trimAll.data()), NEMath::eCompare::Equal);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestTrim, ::testing::ValuesIn<TrimStringsParams>(_listTrimString));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestTrim, ::testing::ValuesIn<TrimStringsParams>(_listTrimString));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to trim white-space in the strings and copy the result to another
 ************************************************************************/

//!< The list of parameters: the string to test and the expected results to compare
static constexpr TrimStringsParams _listTrimCopyString[]
{
      { {"One String"       }   , {"One String"     }   , {"One String"     }   , {"One String"     }, -2 } // 0
    , { {"  Two  String"    }   , {"Two  String"    }   , {"  Two  String"  }   , {"Two  String"    }, -2 } // 1
    , { {"Three String   "  }   , {"Three String   "}   , {"Three String"   }   , {"Three String"   }, -2 } // 2
    , { {"  Four String   " }   , {"Four String   " }   , {"  Four String"  }   , {"Four String"    }, -2 } // 3
    , { {"                " }   , {""               }   , {""               }   , {""               }, -2 } // 4
    , { {"  \n            " }   , {""               }   , {""               }   , {""               }, -2 } // 5
    , { {"  \r            " }   , {""               }   , {""               }   , {""               }, -2 } // 6
    , { {"  \r\n          " }   , {""               }   , {""               }   , {""               }, -2 } // 7
    , { {"  \n  \n        " }   , {""               }   , {""               }   , {""               }, -2 } // 8
    , { {"  \r  \r        " }   , {""               }   , {""               }   , {""               }, -2 } // 9
    , { {"  \n\r\n\r\n    " }   , {""               }   , {""               }   , {""               }, -2 } // 10
    , { {"One String"       }   , {"One "           }   , {"One"            }   , {"One"            },  4 } // 11
    , { {"  Two  String"    }   , {"Two  "          }   , {"  Two"          }   , {"Two"            },  7 } // 12
    , { {"  Four String "   }   , {"Four"           }   , {"  Four"         }   , {"Four"           },  6 } // 13
    , { {"  \n\r\n\r\n    " }   , {""               }   , {""               }   , {""               },  4 } // 14
    , { { "SomeString"      }   , {"Some"           }   , {"Some"           }   , {"Some"           },  4 } // 15
    , { { "A         "      }   , {"A"              }   , {"A"              }   , {"A"              },  1 } // 16
    , { { "         A"      }   , {""               }   , {""               }   , {""               },  9 } // 17
    , { { "A   B     "      }   , {"A"              }   , {"A"              }   , {"A"              },  1 } // 18
    , { { "A   B     "      }   , {"A   "           }   , {"A"              }   , {"A"              },  4 } // 19
    , { {"Three String   "  }   , {"Three"          }   , {"Three"          }   , {"Three"          },  5 } // 20
    , { {" Any   String  "  }   , {"Any  "          }   , {" Any"           }   , {"Any"            },  6 } // 21
};

//!< Declare parameters.
struct StringTestTrimCopy : public ::testing::TestWithParam<TrimStringsParams>
{
    TrimStringsParams params;
};

/**
 * \brief   Trim strings, if needed takes only `count` number of characters in the string.
 *          The result is copied to another buffer. The original string is not changed.
 **/
TEST_P(StringTestTrimCopy, TrimCopyStrings)
{
    const TrimStringsParams& param = GetParam();
    wchar_t wide[64];

    NEMath::eCompare compare{};

    NEString::trimLeft<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    EXPECT_EQ(compare, NEMath::eCompare::Equal);
    compare = NEString::compare<wchar_t, char>(wide, param.trimLeft.data());
    EXPECT_EQ(compare, NEMath::eCompare::Equal);

    NEString::trimRight<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    compare = NEString::compare<wchar_t, char>(wide, param.trimRight.data());
    EXPECT_EQ(compare, NEMath::eCompare::Equal);

    NEString::trimAll<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    compare = NEString::compare<wchar_t, char>(wide, param.trimAll.data());
    EXPECT_EQ(compare, NEMath::eCompare::Equal);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestTrimCopy, ::testing::ValuesIn<TrimStringsParams>(_listTrimCopyString));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestTrimCopy, ::testing::ValuesIn<TrimStringsParams>(_listTrimCopyString));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)
