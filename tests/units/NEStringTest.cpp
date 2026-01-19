/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
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

/**
 * \brief   Test methods of checking the printable characters.
 **/
TEST(NEStringTest, GetPrintable)
{
    constexpr uint8_t printLow[]{ 9, 10, 11, 13 };
    constexpr uint32_t sizePrintLow{ MACRO_ARRAYLEN(printLow) };
    constexpr uint8_t nonPrintHigh[]{ 127, 129, 141, 143, 144, 157, 173 };
    constexpr uint32_t sizeNonPrintHigh{ MACRO_ARRAYLEN(nonPrintHigh) };

    constexpr uint32_t sizeNonPrint{ 31 - sizePrintLow + sizeNonPrintHigh + 1}; //plus one -- string zero-terminate
    constexpr uint32_t sizePrint{ 0x0100 - sizeNonPrint };  // 256 - <non-printable>

    char buffer[0xFF + 1u]{};
    for (uint16_t i = 0; i < 0xFF; ++i)
    {
        buffer[i] = static_cast<char>(static_cast<uint8_t>(i + 1u));
    }

    buffer[0xFF] = NEString::EndOfString; // 256-th entry

    uint32_t countPrintable{ 0 };
    uint32_t countNonPrintable{ 0 };

    char* next = { buffer };
    do
    {
        const char* buf = NEString::getPrintable<char>(next, NEString::COUNT_ALL, &next);
        while (NEString::isEndOfString(*buf) == false)
        {
            const uint8_t ch{ static_cast<uint8_t>(*buf) };
            if (ch <= 31)
            {
                bool canPrint{ false };
                for (uint32_t i = 0; i < sizePrintLow; ++i)
                {
                    if (ch == printLow[i])
                    {
                        // should happen once
                        canPrint = true;
                        break;
                    }
                }

                EXPECT_TRUE(canPrint);
            }
            else if (ch >= 127u)
            {
                bool canPrint{ true };
                for (uint32_t i = 0; i < sizeNonPrintHigh; ++i)
                {
                    if (ch == nonPrintHigh[i])
                    {
                        // should not happen
                        canPrint = false;
                        break;
                    }
                }

                EXPECT_TRUE(canPrint);
            }

            EXPECT_TRUE(NEString::isPrintable(*buf));
            ++countPrintable;
            ++buf;
        }

        ++countNonPrintable;
    } while (next != nullptr);

    EXPECT_EQ(countPrintable, sizePrint);
    EXPECT_EQ(countNonPrintable, sizeNonPrint);
}

/************************************************************************
 * Parameterized tests to compare strings, with and without case sensitive
 ************************************************************************/
/**
 * \brief   Parameters of strings and expected results of the tests.
 **/
struct CompareStringParams
{
    std::string_view left;              //!< Left string
    std::string_view right;             //!< Right string
    int              count{};           //!< Characters count
    int              resAllSens{};      //!< Test result when compare all strings, case sensitive
    int              resAllIgnore{};    //!< Test result when compare all strings, ignore case sensitive
    int              resCountSens{};    //!< Test result when compare count number of characters in the strings, case sensitive
    int              resCountIgnore{};  //!< Test result when compare count number of characters in the strings, ignore case sensitive
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
    int                 count{};    //!< The number of characters in the string to trim. NEString::COUNT_ALL means to take the complete string. Otherwise, only a part of string is taken.
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
    , { {""                 }   , {""               }   , {""               }   , {""               }, -2 } // 23
};

//!< Declare test with parameters.
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
    , { {""                 }   , {""               }   , {""               }   , {""               }, -2 } // 22
};

//!< Declare test with parameters.
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

/************************************************************************
 * Parameterized tests to test removing a character in the string.
 ************************************************************************/

/**
 * \brief   The structure to use to test removing a single character in the string.
 **/
struct RemoveCharParams
{
    std::string_view    source;     //!< The original string to test.
    char                remove{};   //!< The character to remove.
    std::string_view    resAll;     //!< The result when remove all matches
    std::string_view    resOne;     //!< The result when remove only one (first match) character.
};

//!< List of parameters to test and the results to compare
static constexpr RemoveCharParams _listRemoveCharParams[]
{
      { { "123123"      }, '1'  , {"2323"       }, {"23123"         } } // 0
    , { {"1 2 3 4 5 "   }, ' '  , {"12345"      }, {"12 3 4 5 "     } } // 1
    , { {" 1 2 3 4 5 "  }, ' '  , {"12345"      }, {"1 2 3 4 5 "    } } // 2
    , { {"           "  }, ' '  , {""           }, {"          "    } } // 3
    , { {"string"       }, '1'  , {"string"     }, {"string"        } } // 4
    , { {"123"          }, '\0' , {"123"        }, {"123"           } } // 5
    , { {""             }, '\0' , {""           }, {""              } } // 6
};

//!< Declare test with parameters.
struct StringTestRemoveChar : public ::testing::TestWithParam<RemoveCharParams>
{
    RemoveCharParams params;
};

/**
 * \brief   Remove a char in the string.
 **/
TEST_P(StringTestRemoveChar, RemoveChar)
{
    const RemoveCharParams& param = GetParam();
    char buffer[64];

    EXPECT_EQ(NEString::copyString<char>(buffer, 64, param.source.data(), NEString::COUNT_ALL), static_cast<NEString::CharCount>(param.source.length()));
    const char* all = NEString::removeChar<char>(param.remove, buffer, true);
    EXPECT_EQ(NEString::compareFast<char>(buffer, param.resAll.data(), NEString::COUNT_ALL), NEMath::eCompare::Equal);
    EXPECT_TRUE(all != nullptr);
    EXPECT_TRUE(NEString::isEndOfString<char>(*all));

    NEString::CharCount len = NEString::getStringLength<char>(buffer);
    NEString::CharCount count = static_cast<NEString::CharCount>(param.source.length()) - len;

    EXPECT_EQ(NEString::copyString<char>(buffer, 64, param.source.data(), NEString::COUNT_ALL), static_cast<NEString::CharCount>(param.source.length()));
    char* one = NEString::removeChar<char>(param.remove, buffer, false);
    EXPECT_EQ(NEString::compareFast<char>(buffer, param.resOne.data(), NEString::COUNT_ALL), NEMath::eCompare::Equal);
    EXPECT_TRUE(one != nullptr);

    for ( ;count > 1; --count)
    {
        // we did not reach end of string.
        EXPECT_TRUE(NEString::isEndOfString<char>(*one) == false);
        // remove char starting at `one`
        one = NEString::removeChar<char>(param.remove, one, false);
    }

    // no more character to remove, check it
    EXPECT_TRUE(NEString::compareFast<char>(buffer, param.resAll.data(), NEString::COUNT_ALL) == NEMath::eCompare::Equal);

    // the `one` might not reach end of string, call `removeChar` one more time and make sure that end of string reached
    one = NEString::removeChar<char>(param.remove, buffer, false);
    EXPECT_TRUE(NEString::isEndOfString<char>(*one));
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestRemoveChar, ::testing::ValuesIn<RemoveCharParams>(_listRemoveCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestRemoveChar, ::testing::ValuesIn<RemoveCharParams>(_listRemoveCharParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to search a character from the begin of the string
 ************************************************************************/

using PhraseList = std::vector<std::string_view>;

/**
 * \brief   The structure to test searching a character in the string.
 **/
struct FindCharParams
{
    std::string_view    source;     //!< The original string to test.
    char                ch{};       //!< The character to search.
    int                 pos{};      //!< The position to start searching
    PhraseList          results;    //!< The list of results to compare with.
};

//!< List of parameters to test and the results to compare
static FindCharParams _listFindFirstCharParams[]
{
        { { "123123"        }, 'a'  , 0 , {                                             } } // 0
      , { { "123123"        }, '1'  , 0 , { {"23123"}, {"23"}                           } } // 1
      , { { "123123"        }, '2'  , 0 , { {"3123"}, {"3"}                             } } // 2
      , { { "123123"        }, '3'  , 0 , { {"123"}                                     } } // 3
      , { { "      "        }, ' '  , 0 , { {"     "}, {"    "}, {"   "}, {"  "}, {" "} } } // 4
      , { { "123123"        }, '\0' , 0 , {                                             } } // 5
      , { { "123125"        }, '5'  , 0 , {                                             } } // 6
      , { { "0123125"       }, '0'  , 0 , { {"123125"}                                  } } // 7
      , { { ""              }, '1'  , 0 , {                                             } } // 8
      , { { ""              }, '\0' , 0 , {                                             } } // 9
      , { { "123123"        }, '1'  , 1 , { {"23"}                                      } } // 10
      , { { "123456"        }, '3'  , 2 , { {"456"}                                     } } // 11
      , { { "123456"        }, '3'  , 3 , {                                             } } // 12
      , { { "123456"        }, '6'  , 5 , {                                             } } // 13
      , { { "123456"        }, '6'  , 6 , {                                             } } // 14
      , { { "      "        }, ' '  , 2 , { {"   "}, {"  "}, {" "}                      } } // 15
};

//!< Declare test with parameters.
struct StringTestFindFirstChar : public ::testing::TestWithParam<FindCharParams>
{
    FindCharParams params;
};

/**
 * \brief   Find a character in the string. It searches from the specified position until end of string.
 **/
TEST_P(StringTestFindFirstChar, FindFirstChar)
{
    const FindCharParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char ch{ param.ch };
    const PhraseList& results{ param.results };
    NEString::CharCount pos{ param.pos };
    NEString::CharCount posTemp{ param.pos };
    const char* nextTemp{ nullptr };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        const std::string_view& result{ results[i] };

        posTemp = NEString::findFirst<char>(ch, next, posTemp, true, &nextTemp);
        EXPECT_TRUE(result == nextTemp);
        posTemp = 0;


        pos = NEString::findFirst<char>(ch, param.source.data(), pos, true, &next);
        EXPECT_TRUE(pos != NEString::INVALID_POS);
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
        EXPECT_TRUE(result == next);
        pos += 1;
    }

    pos = NEString::findFirst<char>(ch, param.source.data(), pos, true, &next);
    if (pos != NEString::INVALID_POS)
    {
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
    }

    EXPECT_TRUE(NEString::isEmpty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestFindFirstChar, ::testing::ValuesIn<FindCharParams>(_listFindFirstCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestFindFirstChar, ::testing::ValuesIn<FindCharParams>(_listFindFirstCharParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to search a character from the end of the string
 ************************************************************************/

//!< List of parameters to test and the results to compare
static FindCharParams _listFindLastCharParams[]
{
        { { "123123"        }, 'a'  , -2, {                                             } } // 0
      , { { "123123"        }, '1'  , -2, { {"123"}, {"123123"}                         } } // 1
      , { { "123123"        }, '2'  , -2, { {"23"}, {"23123"}                           } } // 2
      , { { "123123"        }, '3'  , -2, { {"3"}, {"3123"}                             } } // 3
      , { { "     "         }, ' '  , -2, { {" "},{ "  " }, {"   "}, {"    "}, {"     "}} } // 4
      , { { "123123"        }, '\0' , -2, {                                             } } // 5
      , { { "123125"        }, '5'  , -2, { {"5"}                                       } } // 6
      , { { "0123125"       }, '0'  , -2, { {"0123125"}                                 } } // 7
      , { { ""              }, '1'  , -2, {                                             } } // 8
      , { { ""              }, '\0' , -2, {                                             } } // 9
      , { { "123456"        }, '1'  ,  0, { {"123456"}                                  } } // 10
      , { { "123456"        }, '1'  ,  1, { {"123456"}                                  } } // 11
      , { { "123456"        }, '2'  ,  2, { {"23456"}                                   } } // 12
      , { { "123456"        }, '3'  ,  3, { {"3456"}                                    } } // 13
      , { { "123456"        }, '4'  ,  4, { {"456"}                                     } } // 14
      , { { "123456"        }, '5'  ,  5, { {"56"}                                      } } // 15
      , { { "123456"        }, '6'  ,  6, { {"6"}                                       } } // 16
      , { { "123456"        }, '6'  ,  7, { {"6"}                                       } } // 17
      , { { "123456"        }, '4'  ,  2, {                                             } } // 18
      , { { "      "        }, ' '  ,  2, { {"    "}, {"     "}, {"      "}             } } // 19
};

//!< Declare test with parameters.
struct StringTestFindLastChar : public ::testing::TestWithParam<FindCharParams>
{
    FindCharParams params;
};

/**
 * \brief   Find a character in the string. It searches from the specified position until begin of string.
 **/
TEST_P(StringTestFindLastChar, FindLastChar)
{
    const FindCharParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char ch{ param.ch };
    const PhraseList& results{ param.results };
    NEString::CharCount pos{ param.pos };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        const std::string_view& result{ results[i] };

        pos = NEString::findLast<char>(ch, param.source.data(), pos, true, &next);
        EXPECT_TRUE(pos != NEString::INVALID_POS);
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
        EXPECT_TRUE(result == next);
        pos -= 1;

        EXPECT_NE(pos, NEString::END_POS);
    }

    pos = NEString::findLast<char>(ch, param.source.data(), pos, true, &next);
    EXPECT_EQ(pos, NEString::INVALID_POS);
    EXPECT_TRUE(NEString::isEmpty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestFindLastChar, ::testing::ValuesIn<FindCharParams>(_listFindLastCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestFindLastChar, ::testing::ValuesIn<FindCharParams>(_listFindLastCharParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to search a phrase in the string.
 ************************************************************************/

/**
 * \brief   The structure to use to test searching a phrase in the string.
 **/
struct FindPhraseParams
{
    std::string_view    source;         //!< The original string to test.
    std::string_view    phrase;         //!< The phrase to search.
    int                 pos{};          //!< The position to start searching
    bool                isSensitive{};  //!< Indicates whether the searching is case sensitive or not.
    PhraseList          results;        //!< The list of results to compare with.
};

//!< List of parameters to test and the results to compare
static FindPhraseParams _listFindFirstPhraseParams[]
{
      { {"123123"       }, {"123"       },  0, true , {{"123"}                                      } } // 0
    , { {"1111111"      }, {"111"       },  0, true , {{"1111"}, {"1"}                              } } // 1
    , { {" 123123 "     }, {"23"        },  0, true , {{"123 "}, {" "}                              } } // 2
    , { {"         "    }, {"  "        },  0, true , {{"       "}, {"     "}, {"   "}, {" "}       } } // 3
    , { {" 123123 "     }, {""          },  0, true , {                                             } } // 4
    , { {" 123123 45"   }, {"45"        },  0, true , {                                             } } // 5
    , { {"0123123 45"   }, {"01"        },  0, true , {{"23123 45"}                                 } } // 6
    , { {"0123123 45"   }, {"AB"        },  0, true , {                                             } } // 7
    , { {""             }, {"AB"        },  0, true , {                                             } } // 8
    , { {""             }, {""          },  0, true , {                                             } } // 9
    , { {" 123123 "     }, {"312"       },  0, true , {{"3 "}                                       } } // 10
    , { {"ABCDEBFG"     }, {"ABCDEBFG"  },  0, true , {                                             } } // 11
    , { {"ABCDEBFG"     }, {"ABCDEBFGH" },  0, true , {                                             } } // 12
    , { {"123456789012a"}, {"123"       },  0, true , {{"456789012a"}                               } } // 13
    , { {"123123123123a"}, {"123"       },  1, true , {{"123123a"}, {"123a"}, {"a"}                 } } // 14
    , { {"123123123123a"}, {"123"       },  2, true , {{"123123a"}, {"123a"}, {"a"}                 } } // 15
    , { {"123123123123a"}, {"123"       },  3, true , {{"123123a"}, {"123a"}, {"a"}                 } } // 16
    , { {"123123123123a"}, {"123"       },  4, true , {{"123a"}, {"a"}                              } } // 17
    , { {"123123123123a"}, {"123"       },  5, true , {{"123a"}, {"a"}                              } } // 18
    , { {"123123123123a"}, {"123"       },  6, true , {{"123a"}, {"a"}                              } } // 19
    , { {"123123123123a"}, {"123"       },  7, true , {{"a"}                                        } } // 20
    , { {"123123123123a"}, {"123"       },  8, true , {{"a"}                                        } } // 21
    , { {"123123123123a"}, {"123"       },  9, true , {{"a"}                                        } } // 22
    , { {"123123123123a"}, {"123"       }, 10, true , {                                             } } // 23
    , { {"123123123123a"}, {"123"       }, 11, true , {                                             } } // 24
    , { {"123123123123a"}, {"123"       }, 12, true , {                                             } } // 25
    , { {"123123123123a"}, {"123"       }, 13, true , {                                             } } // 26
    , { {"abcabcabcabcd"}, {"abc"       },  0, true , {{"abcabcabcd"}, {"abcabcd"}, {"abcd"}, {"d"} } } // 27
    , { {"ABCABCABCABCD"}, {"abc"       },  0, true , {                                             } } // 28
    , { {"abcabcabcabcd"}, {"ABC"       },  0, false, {{"abcabcabcd"}, {"abcabcd"}, {"abcd"}, {"d"} } } // 29
    , { {"ABCabcAbcABcd"}, {"abc"       },  0, false, {{"abcAbcABcd"}, {"AbcABcd"}, {"ABcd"}, {"d"} } } // 30
    , { {"ABCabcAbcABcd"}, {"AbC"       },  0, false, {{"abcAbcABcd"}, {"AbcABcd"}, {"ABcd"}, {"d"} } } // 31
};

//!< Declare test with parameters.
struct StringTestFindFirstPhrase : public ::testing::TestWithParam<FindPhraseParams>
{
    FindPhraseParams params;
};

/**
 * \brief   Find a character in the string starting from the begin of the string.
 *          It searches from the specified position until end of string.
 **/
TEST_P(StringTestFindFirstPhrase, FindFirstString)
{
    const FindPhraseParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char * phrase{ param.phrase.data() };
    const PhraseList& results{ param.results };
    NEString::CharCount pos{ param.pos };
    NEString::CharCount posTemp{ param.pos };
    const char* nextTemp{ nullptr };
    NEString::CharCount phraseCount{ static_cast<NEString::CharCount>(param.phrase.length()) };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        const std::string_view& result{ results[i] };

        posTemp = NEString::findFirst<char>(phrase, next, posTemp, param.isSensitive, &nextTemp);
        EXPECT_TRUE(result == nextTemp);
        posTemp = 0;


        pos = NEString::findFirst<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
        EXPECT_TRUE(pos != NEString::INVALID_POS);
        NEMath::eCompare comp = NEString::compareStrings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, NEMath::eCompare::Equal);
        EXPECT_TRUE(result == next);
        pos += phraseCount;
    }

    pos = NEString::findFirst<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
    if (pos != NEString::INVALID_POS)
    {
        NEMath::eCompare comp = NEString::compareStrings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, NEMath::eCompare::Equal);
    }

    EXPECT_TRUE(NEString::isEmpty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestFindFirstPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindFirstPhraseParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestFindFirstPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindFirstPhraseParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to search a phrase in the string from the end.
 ************************************************************************/

//!< List of parameters to test and the results to compare
static FindPhraseParams _listFindLastPhraseParams[]
{
      { {"123123"       }, {"123"       }, -2, true , {{"123"}, {"123123"}                                      } } // 0
    , { {"1111111"      }, {"111"       }, -2, true , {{"111"}, {"111111"}                                      } } // 1
    , { {" 123123 "     }, {"23"        }, -2, true , {{"23 "}, {"23123 "}                                      } } // 2
    , { {"         "    }, {"  "        }, -2, true , {{"  "}, {"    "}, {"      "}, {"        "}               } } // 3
    , { {" 123123 "     }, {""          }, -2, true , {                                                         } } // 4
    , { {" 123123 45"   }, {"45"        }, -2, true , {{"45"}                                                   } } // 5
    , { {"0123123 45"   }, {"01"        }, -2, true , {{"0123123 45"}                                           } } // 6
    , { {"0123123 45"   }, {"AB"        }, -2, true , {                                                         } } // 7
    , { {""             }, {"AB"        }, -2, true , {                                                         } } // 8
    , { {""             }, {""          }, -2, true , {                                                         } } // 9
    , { {" 123123 "     }, {"312"       }, -2, true , {{"3123 "}                                                } } // 10
    , { {"ABCDEBFG"     }, {"ABCDEBFG"  }, -2, true , {{"ABCDEBFG"}                                             } } // 11
    , { {"ABCDEBFG"     }, {"ABCDEBFGH" }, -2, true , {                                                         } } // 12
    , { {"123456789012a"}, {"123"       }, -2, true , {{"123456789012a"}                                        } } // 13
    , { {"123123123123a"}, {"123"       }, 13, true , {{"123a"}, {"123123a"}, {"123123123a"}, {"123123123123a"} } } // 14
    , { {"123123123123a"}, {"123"       }, 12, true , {{"123a"}, {"123123a"}, {"123123123a"}, {"123123123123a"} } } // 15
    , { {"123123123123a"}, {"123"       }, 11, true , {          {"123123a"}, {"123123123a"}, {"123123123123a"} } } // 16
    , { {"123123123123a"}, {"123"       }, 10, true , {          {"123123a"}, {"123123123a"}, {"123123123123a"} } } // 17
    , { {"123123123123a"}, {"123"       },  9, true , {          {"123123a"}, {"123123123a"}, {"123123123123a"} } } // 18
    , { {"123123123123a"}, {"123"       },  8, true , {                       {"123123123a"}, {"123123123123a"} } } // 19
    , { {"123123123123a"}, {"123"       },  7, true , {                       {"123123123a"}, {"123123123123a"} } } // 20
    , { {"123123123123a"}, {"123"       },  6, true , {                       {"123123123a"}, {"123123123123a"} } } // 21
    , { {"123123123123a"}, {"123"       },  5, true , {                                       {"123123123123a"} } } // 22
    , { {"123123123123a"}, {"123"       },  4, true , {                                       {"123123123123a"} } } // 23
    , { {"123123123123a"}, {"123"       },  3, true , {                                       {"123123123123a"} } } // 24
    , { {"123123123123a"}, {"123"       },  2, true , {                                                         } } // 25
    , { {"123123123123a"}, {"123"       },  1, true , {                                                         } } // 26
    , { {"123123123123a"}, {"123"       },  0, true , {                                                         } } // 27
    , { {"abcabcabcabcd"}, {"abc"       }, -2, true , {{"abcd"}, {"abcabcd"}, {"abcabcabcd"}, {"abcabcabcabcd"} } } // 28
    , { {"ABCABCABCABCD"}, {"abc"       }, -2, true , {                                                         } } // 29
    , { {"abcabcabcabcd"}, {"ABC"       }, -2, false, {{"abcd"}, {"abcabcd"}, {"abcabcabcd"}, {"abcabcabcabcd"} } } // 30
    , { {"ABCabcAbcABcd"}, {"abc"       }, -2, false, {{"ABcd"}, {"AbcABcd"}, {"abcAbcABcd"}, {"ABCabcAbcABcd"} } } // 31
    , { {"ABCabcAbcABcd"}, {"AbC"       }, -2, false, {{"ABcd"}, {"AbcABcd"}, {"abcAbcABcd"}, {"ABCabcAbcABcd"} } } // 32
};

//!< Declare test with parameters.
struct StringTestFindLastPhrase : public ::testing::TestWithParam<FindPhraseParams>
{
    FindPhraseParams params;
};

/**
 * \brief   Find a phrase in the string starting from the end of the string.
 *          It searches from the specified position until begin of the string.
 **/
TEST_P(StringTestFindLastPhrase, FindLastString)
{
    const FindPhraseParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char * phrase{ param.phrase.data() };
    const PhraseList& results{ param.results };
    NEString::CharCount pos{ param.pos };

    NEString::CharCount phraseCount{ static_cast<NEString::CharCount>(param.phrase.length()) };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        const std::string_view& result{ results[i] };

        pos = NEString::findLast<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
        EXPECT_TRUE(pos != NEString::INVALID_POS);
        NEMath::eCompare comp = NEString::compareStrings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, NEMath::eCompare::Equal);
        EXPECT_TRUE(result == next);
    }

    pos = NEString::findLast<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
    if (pos != NEString::INVALID_POS)
    {
        NEMath::eCompare comp = NEString::compareStrings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, NEMath::eCompare::Equal);
    }

    EXPECT_TRUE(NEString::isEmpty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestFindLastPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindLastPhraseParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestFindLastPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindLastPhraseParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to verify whether the string starts or end with the specific phrase.
 ************************************************************************/

/**
 * \brief   The structure to check the start and end of the string.
 **/
struct CompareSubstring
{
    std::string_view    source;                 //!< The original string to check
    std::string_view    substr;                 //!< The substring to search
    bool                isStart{ false };       //!< Flag, indicating whether the expected substring should search at start or end of string
    bool                isSensetive{ false };   //!< Flag, indicating whether the characters are case sensitive or not.
    bool                result{ false };        //!< The expected result when check the substring
};

//!< The list of testing parameters
static constexpr CompareSubstring _listCompareSubstring[]
{
      { {"123456"   }, {"1"     }, true , true  , true  } // 0
    , { {"123456"   }, {"123"   }, true , true  , true  } // 1
    , { {"123456"   }, {"2"     }, true , true  , false } // 2
    , { {"123456"   }, {"23"    }, true , true  , false } // 3
    , { {"123456"   }, {"13"    }, true , true  , false } // 4
    , { {"123456"   }, {"6"     }, false, true  , true  } // 5
    , { {"123456"   }, {"456"   }, false, true  , true  } // 6
    , { {"123456"   }, {"5"     }, false, true  , false } // 7
    , { {"123456"   }, {"45"    }, false, true  , false } // 8
    , { {"123456"   }, {"46"    }, false, true  , false } // 9
    , { {"      "   }, {"  "    }, true , true  , true  } // 10
    , { {"      "   }, {"  "    }, false, true  , true  } // 11
    , { {""         }, {"  "    }, true , true  , false } // 12
    , { {""         }, {"  "    }, false, true  , false } // 13
    , { {"      "   }, {""      }, true , true  , false } // 14
    , { {"      "   }, {""      }, false, true  , false } // 15
    , { {"\n    "   }, {"  "    }, true , true  , false } // 16
    , { {"    \n"   }, {"  "    }, false, true  , false } // 17
    , { {"abcdef"   }, {"a"     }, true , true  , true  } // 18
    , { {"abcdef"   }, {"A"     }, true , false , true  } // 19
    , { {"abcdef"   }, {"abc"   }, true , true  , true  } // 20
    , { {"abcdef"   }, {"AbC"   }, true , false , true  } // 21
    , { {"abcdef"   }, {"b"     }, true , false , false } // 22
    , { {"abcdef"   }, {"ac"    }, true , false , false } // 23
    , { {"abcdef"   }, {"f"     }, false, true  , true  } // 24
    , { {"abcdef"   }, {"F"     }, false, false , true  } // 25
    , { {"abcdef"   }, {"def"   }, false, true  , true  } // 26
    , { {"abcdef"   }, {"DeF"   }, false, false , true  } // 27
    , { {"abcdef"   }, {"e"     }, false, false , false } // 28
    , { {"abcdef"   }, {"Df"    }, false, false , false } // 29
};

//!< Declare test with parameters.
struct StringTestStartsEnds : public ::testing::TestWithParam<CompareSubstring>
{
    CompareSubstring params;
};

/**
 * \brief   Check whether the string starts or ends with the character or substring.
 **/
TEST_P(StringTestStartsEnds, StringStartsEnds)
{
    const CompareSubstring& param{ GetParam() };
    const char* source = param.source.data();
    const char* substr = param.substr.data();

    if (param.isStart)
    {
        if (param.substr.length() > 1)
        {
            EXPECT_EQ(NEString::stringStartsWith<char>(source, substr, param.isSensetive), param.result);
        }
        else if (param.substr.length() == 1)
        {
            EXPECT_EQ(NEString::stringStartsWith<char>(source, *substr, param.isSensetive), param.result);
        }
        else
        {
            EXPECT_EQ(NEString::stringStartsWith<char>(source, substr, param.isSensetive), param.result);
            EXPECT_EQ(NEString::stringStartsWith<char>(source, *substr, param.isSensetive), param.result);
        }
    }
    else
    {
        if (param.substr.length() > 1)
        {
            EXPECT_EQ(NEString::stringEndsWith<char>(source, substr, param.isSensetive), param.result);
        }
        else if (param.substr.length() == 1)
        {
            EXPECT_EQ(NEString::stringEndsWith<char>(source, *substr, param.isSensetive), param.result);
        }
        else
        {
            EXPECT_EQ(NEString::stringEndsWith<char>(source, substr, param.isSensetive), param.result);
            EXPECT_EQ(NEString::stringEndsWith<char>(source, *substr, param.isSensetive), param.result);
        }
    }
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestStartsEnds, ::testing::ValuesIn<CompareSubstring>(_listCompareSubstring));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestStartsEnds, ::testing::ValuesIn<CompareSubstring>(_listCompareSubstring));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to verify reading a line of string.
 ************************************************************************/

struct StringLines
{
    std::string_view    source;
    PhraseList          lines;
};

static const StringLines _listStringLines[]
{
      { { "one\ntwo\nthree\nfour"                       }, { {"one"     }, {"two"   }, {"three" }, {"four"  }                                               } } // 0
    , { { "one\r\ntwo\r\nthree\r\nfour"                 }, { {"one"     }, {"two"   }, {"three" }, {"four"  }                                               } } // 1
    , { { "one\rtwo\rthree\rfour"                       }, { {"one"     }, {"two"   }, {"three" }, {"four"  }                                               } } // 2
    , { { "one\ntwo\n\rthree\r\nfour\r"                 }, { {"one"     }, {"two"   }, {""      }, {"three" }, {"four"  }                                   } } // 3
    , { { "\none\ntwo\n\rthree\r\nfour\n"               }, { {""        }, {"one"   }, {"two"   }, {""      }, {"three" }, {"four"}                         } } // 4
    , { { "one\n\ntwo\n\rthree\r\nfour\n\n"             }, { {"one"     }, {""      }, {"two"   }, {""      }, {"three" }, {"four"  }, {""    }             } } // 5
    , { { "\rone\n\ntwo\n\rthree\r\nfour\n\r"           }, { {""        }, {"one"   }, {""      }, {"two"   }, {""      }, {"three" }, {"four"}, {""    }   } } // 6
    , { { "\x01one\x02two\x03three\x04""four\x05\x06"   }, { {""        }, {"one"   }, {"two"   }, {"three" }, {"four"  }, {""      }                       } } // 7
    , { { "\n\x7\r"                                     }, { {""        }, {""      }, {""      }                                                           } } // 8
    , { { ""                                            }, { {""        }                                                                                   } } // 9
    , { { "one, two, three, four"                       }, { {"one, two, three, four"}                                                                      } } // 10
};

//!< Declare test with parameters.
struct StringTestGetLine : public ::testing::TestWithParam<StringLines>
{
    StringLines params;
};

/**
 * \brief   Checks reading a line of string.
 **/
TEST_P(StringTestGetLine, StringGetLine)
{
    const StringLines& param{ GetParam() };
    const char* source = param.source.data();
    const PhraseList& lines = param.lines;

    char buffer[256]{};
    char* next{ buffer };
    NEString::copyString(buffer, 256, source);

    for (const auto& line : lines)
    {
        const char * result = NEString::getLine<char>(next, NEString::END_POS, &next);
        EXPECT_TRUE(line == result);
    }

    EXPECT_TRUE(next == nullptr);

    const char* result = NEString::getLine<char>(next, NEString::END_POS, &next);
    EXPECT_TRUE(result == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestGetLine, ::testing::ValuesIn<StringLines>(_listStringLines));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestGetLine, ::testing::ValuesIn<StringLines>(_listStringLines));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Parameterized tests to verify converting a string with digits and a sign into signed 32-bit integer.
 ************************************************************************/

struct StringInteger
{
    std::string_view    source;
    std::vector<int>    digits;
    PhraseList          remains;
};

static const StringInteger _listStringDigits[]
{
      { { "123"     }, {  123   }, {    } } // 0
    , { {"-123"     }, { -123   }, {    } }
    , { {" -123 "   }, { -123   }, {    } }
    , { {" - 123 "  }, { -123   }, {    } }
    , { {"   123 "  }, {  123   }, {    } }
    , { {"-12345678901234567890123456789123456789"  }, {  455581931 }, {    } }
    , { {"12345678901234567890123456789123456789"   }, { -455581931 }, {    } }
};

//!< Declare test with parameters.
struct StringTestGetInteger : public ::testing::TestWithParam<StringInteger>
{
    StringInteger   params;
};

/**
 * \brief   Checks reading a line of string.
 **/
TEST_P(StringTestGetInteger, StringGetInteger)
{
    const StringInteger& param{ GetParam() };
    const char* source = param.source.data();
    const PhraseList& remains = param.remains;
    const std::vector<int>& digits = param.digits;

    const char* next{ source };

    uint32_t idx = 0;
    for (auto num : digits)
    {
        int result = NEString::makeInteger(next, &next);
        EXPECT_EQ(result, num);
        if (idx < remains.size())
        {
            EXPECT_TRUE(remains[idx] == next);
        }
    }

    int result = NEString::makeInteger(next, &next);
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(NEStringTest, StringTestGetInteger, ::testing::ValuesIn<StringInteger>(_listStringDigits));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(NEStringTest, StringTestGetInteger, ::testing::ValuesIn<StringInteger>(_listStringDigits));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)
