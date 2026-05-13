/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        units/StringDefsTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, Areg framework unit test file.
 *              The unit test for methods in NEString namespace.
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "units/GUnitTest.hpp"
#include "gtest/gtest-param-test.h"

#include "areg/base/StringDefs.hpp"

/**
 * \brief   Test string_length and isEmpty methods of NEString
 **/
TEST(StringDefsTest1, string_length)
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
    constexpr uint32_t count{ std::size(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(areg::string_length<char>(entry.str), static_cast<areg::CharCount>(entry.len));
        EXPECT_TRUE(areg::is_empty<char>(entry.str) == (entry.len == 0));
    }
}


/**
 * \brief   Test string_line_length and isEmpty methods of NEString
 **/
TEST(StringDefsTest1, string_line_length)
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
        , {"\b12345\r67890\r"       , 6}
        , {"\f12345\r\n67890\r\n"   , 6}
    };
    constexpr uint32_t count{ std::size(params) };

    EXPECT_GT(count, 1u);

    for (uint32_t i = 0; i < count; ++i)
    {
        const SimpleString& entry = params[i];
        EXPECT_EQ(areg::string_line_length<char>(entry.str), static_cast<areg::CharCount>(entry.len));
    }
}

/**
 * \brief   Test methods of checking lower and upper cases in NEString
 **/
TEST(StringDefsTest1, lower_upper_numeric_case)
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
        char lower = areg::make_lower<char>(ch);
        char upper = areg::make_upper<char>(ch);

        if ((ch >= chNumMin) && (ch <= chNumMax))
        {
            // is numeric
            EXPECT_TRUE(areg::is_numeric<char>(ch));
            EXPECT_FALSE(areg::is_lower<char>(ch));
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
            EXPECT_FALSE(areg::is_letter<char>(ch));
        }
        else if ((ch >= chUpperMin) && (ch <= chUpperMax))
        {
            // is upper
            EXPECT_TRUE(areg::is_upper<char>(ch));
            EXPECT_FALSE(areg::is_lower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec1) && (ch <= chUpperMaxSpec1))
        {
            // is upper special 1
            EXPECT_TRUE(areg::is_upper<char>(ch));
            EXPECT_FALSE(areg::is_lower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch >= chUpperMinSpec2) && (ch <= chUpperMaxSpec2))
        {
            // is upper special 2
            EXPECT_TRUE(areg::is_upper<char>(ch));
            EXPECT_FALSE(areg::is_lower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch >= chLowerMin) && (ch <= chLowerMax))
        {
            // is lower
            EXPECT_TRUE(areg::is_lower<char>(ch));
            EXPECT_FALSE(areg::is_upper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec1) && (ch <= chLowerMaxSpec1))
        {
            // is lower special 1
            EXPECT_TRUE(areg::is_lower<char>(ch));
            EXPECT_FALSE(areg::is_upper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch >= chLowerMinSpec2) && (ch <= chLowerMaxSpec2))
        {
            // is lower special 2
            EXPECT_TRUE(areg::is_lower<char>(ch));
            EXPECT_FALSE(areg::is_upper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
        }
        else if ((ch == static_cast<char>(0x8A)) ||
                 (ch == static_cast<char>(0x8C)) ||
                 (ch == static_cast<char>(0x8E)) ||
                 (ch == static_cast<char>(0x9F)) )
        {
            // is upper, control --> 138 (Š), 140 (Œ), 142 (Ž), 159 (Ÿ)
            EXPECT_TRUE(areg::is_upper<char>(ch));
            EXPECT_FALSE(areg::is_lower<char>(ch));
            EXPECT_EQ(ch, upper);
            EXPECT_NE(ch, lower);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
            EXPECT_TRUE(areg::is_control<char>(ch));
        }
        else if ((ch == static_cast<char>(0x9A)) ||
                 (ch == static_cast<char>(0x9C)) ||
                 (ch == static_cast<char>(0x9E)) )
        {
            // is lower, control --> 154 (š), 156 (œ), 158 (ž)
            EXPECT_TRUE(areg::is_lower<char>(ch));
            EXPECT_FALSE(areg::is_upper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_NE(ch, upper);
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_FALSE(areg::is_numeric<char>(ch));
            EXPECT_TRUE(areg::is_control<char>(ch));
        }
        else if (ch == static_cast<char>(0xDF))
        {
            // is German letter 'ss' --> 223 (ß), has no upper case
            EXPECT_FALSE(areg::is_numeric<char>(ch));
            EXPECT_TRUE(areg::is_letter<char>(ch));
            EXPECT_TRUE(areg::is_alphanumeric<char>(ch));
            EXPECT_TRUE(areg::is_lower<char>(ch));
            EXPECT_FALSE(areg::is_upper<char>(ch));
            EXPECT_EQ(ch, lower);
            EXPECT_EQ(ch, upper);
        }
        else
        {
            // is symbol or special char, there is no upper or lower case
            EXPECT_FALSE(areg::is_numeric<char>(ch))     << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(areg::is_letter<char>(ch))      << "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_FALSE(areg::is_alphanumeric<char>(ch))<< "Check char: \'" << static_cast<wchar_t>(ch) << "\'";
            EXPECT_TRUE(areg::is_symbol<char>(ch)       || 
                        areg::is_whitespace<char>(ch)   ||
                        areg::is_delimited<char>(ch)    ||
                        areg::is_eol<char>(ch)          ||
                        areg::is_eos<char>(ch)          ||
                        areg::is_control<char>(ch));
            EXPECT_EQ(lower, upper);
            EXPECT_EQ(ch, lower);
        }
    }
}

/**
 * \brief   Test methods of checking the printable characters.
 **/
TEST(StringDefsTest1, get_printable)
{
    constexpr uint8_t printLow[]{ 9, 10, 11, 13 };
    constexpr uint32_t sizePrintLow{ std::size(printLow) };
    constexpr uint8_t nonPrintHigh[]{ 127, 129, 141, 143, 144, 157, 173 };
    constexpr uint32_t sizeNonPrintHigh{ std::size(nonPrintHigh) };

    constexpr uint32_t sizeNonPrint{ 31 - sizePrintLow + sizeNonPrintHigh + 1}; //plus one -- string zero-terminate
    constexpr uint32_t sizePrint{ 0x0100 - sizeNonPrint };  // 256 - <non-printable>

    char buffer[0xFF + 1u]{};
    for (uint16_t i = 0; i < 0xFF; ++i)
    {
        buffer[i] = static_cast<char>(static_cast<uint8_t>(i + 1u));
    }

    buffer[0xFF] = areg::EndOfString; // 256-th entry

    uint32_t countPrintable{ 0 };
    uint32_t countNonPrintable{ 0 };

    char* next = { buffer };
    do
    {
        const char* buf = areg::printable<char>(next, areg::COUNT_ALL, &next);
        while (areg::is_eos(*buf) == false)
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

            EXPECT_TRUE(areg::is_printable(*buf));
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
TEST_P(StringTestCompare, compare_strings)
{
    const CompareStringParams& param = GetParam();
    const char* left{ param.left.data() };
    const char* right{ param.right.data() };

    areg::Ordering result{};

    result = areg::compare<char, char>(left, right);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resAllSens));

    result = areg::compare_ignore_case<char>(left, right);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resAllIgnore));

    result = areg::compare<char, char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resCountSens));

    result = areg::compare_ignore_case<char, char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resCountIgnore));

    result = areg::compare_fast<char>(left, right, param.count);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resCountSens));

    result = areg::compare_strings<char, char>(left, right, param.count, true);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resCountSens));

    result = areg::compare_strings<char, char>(left, right, param.count, false);
    EXPECT_EQ(result, static_cast<areg::Ordering>(param.resCountIgnore));
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestCompare, ::testing::ValuesIn<CompareStringParams>(_listStringTestCompare));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestCompare, ::testing::ValuesIn<CompareStringParams>(_listStringTestCompare));
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
    std::string_view    trim_left;   //!< The result to trim left
    std::string_view    trim_right;  //!< The result to trim right
    std::string_view    trim_all;    //!< The result to trim all sides.
    int                 count{};    //!< The number of characters in the string to trim. areg::COUNT_ALL means to take the complete string. Otherwise, only a part of string is taken.
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
TEST_P(StringTestTrim, trim_strings)
{
    const TrimStringsParams& param = GetParam();
    char buffer[64];

    areg::CharCount count{};

    count = areg::copy_string<char, char>(buffer, 64, param.noTrim.data(), areg::COUNT_ALL);
    EXPECT_EQ(count, static_cast<areg::CharCount>(param.noTrim.length()));
    areg::trim_left<char>(buffer, param.count);
    EXPECT_EQ(areg::compare(buffer, param.trim_left.data()), areg::Ordering::Equal);

    count = areg::copy_string<char, char>(buffer, 64, param.noTrim.data(), areg::COUNT_ALL);
    EXPECT_EQ(count, static_cast<areg::CharCount>(param.noTrim.length()));
    areg::trim_right<char>(buffer, param.count);
    EXPECT_EQ(areg::compare(buffer, param.trim_right.data()), areg::Ordering::Equal);

    count = areg::copy_string<char, char>(buffer, 64, param.noTrim.data(), areg::COUNT_ALL);
    EXPECT_EQ(count, static_cast<areg::CharCount>(param.noTrim.length()));
    areg::trim_all<char>(buffer, param.count);
    EXPECT_EQ(areg::compare(buffer, param.trim_all.data()), areg::Ordering::Equal);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestTrim, ::testing::ValuesIn<TrimStringsParams>(_listTrimString));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestTrim, ::testing::ValuesIn<TrimStringsParams>(_listTrimString));
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
TEST_P(StringTestTrimCopy, trim_copy_strings)
{
    const TrimStringsParams& param = GetParam();
    wchar_t wide[64];

    areg::Ordering compare{};

    areg::trim_left<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    EXPECT_EQ(compare, areg::Ordering::Equal);
    compare = areg::compare<wchar_t, char>(wide, param.trim_left.data());
    EXPECT_EQ(compare, areg::Ordering::Equal);

    areg::trim_right<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    compare = areg::compare<wchar_t, char>(wide, param.trim_right.data());
    EXPECT_EQ(compare, areg::Ordering::Equal);

    areg::trim_all<wchar_t, char>(wide, 64, param.noTrim.data(), param.count);
    compare = areg::compare<wchar_t, char>(wide, param.trim_all.data());
    EXPECT_EQ(compare, areg::Ordering::Equal);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestTrimCopy, ::testing::ValuesIn<TrimStringsParams>(_listTrimCopyString));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestTrimCopy, ::testing::ValuesIn<TrimStringsParams>(_listTrimCopyString));
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
TEST_P(StringTestRemoveChar, remove_char)
{
    const RemoveCharParams& param = GetParam();
    char buffer[64];

    EXPECT_EQ(areg::copy_string<char>(buffer, 64, param.source.data(), areg::COUNT_ALL), static_cast<areg::CharCount>(param.source.length()));
    const char* all = areg::remove_char<char>(param.remove, buffer, true);
    EXPECT_EQ(areg::compare_fast<char>(buffer, param.resAll.data(), areg::COUNT_ALL), areg::Ordering::Equal);
    EXPECT_TRUE(all != nullptr);
    EXPECT_TRUE(areg::is_eos<char>(*all));

    areg::CharCount len = areg::string_length<char>(buffer);
    areg::CharCount count = static_cast<areg::CharCount>(param.source.length()) - len;

    EXPECT_EQ(areg::copy_string<char>(buffer, 64, param.source.data(), areg::COUNT_ALL), static_cast<areg::CharCount>(param.source.length()));
    char* one = areg::remove_char<char>(param.remove, buffer, false);
    EXPECT_EQ(areg::compare_fast<char>(buffer, param.resOne.data(), areg::COUNT_ALL), areg::Ordering::Equal);
    EXPECT_TRUE(one != nullptr);

    for ( ;count > 1; --count)
    {
        // we did not reach end of string.
        EXPECT_TRUE(areg::is_eos<char>(*one) == false);
        // remove char starting at `one`
        one = areg::remove_char<char>(param.remove, one, false);
    }

    // no more character to remove, check it
    EXPECT_TRUE(areg::compare_fast<char>(buffer, param.resAll.data(), areg::COUNT_ALL) == areg::Ordering::Equal);

    // the `one` might not reach end of string, call `remove_char` one more time and make sure that end of string reached
    one = areg::remove_char<char>(param.remove, buffer, false);
    EXPECT_TRUE(areg::is_eos<char>(*one));
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestRemoveChar, ::testing::ValuesIn<RemoveCharParams>(_listRemoveCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestRemoveChar, ::testing::ValuesIn<RemoveCharParams>(_listRemoveCharParams));
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
TEST_P(StringTestFindFirstChar, find_first_char)
{
    const FindCharParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char ch{ param.ch };
    const PhraseList& results{ param.results };
    areg::CharCount pos{ param.pos };
    areg::CharCount posTemp{ param.pos };
    const char* nextTemp{ nullptr };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        std::string_view result{ results[i] };

        posTemp = areg::find_first<char>(ch, next, posTemp, true, &nextTemp);
        EXPECT_TRUE(result == nextTemp);
        posTemp = 0;


        pos = areg::find_first<char>(ch, param.source.data(), pos, true, &next);
        EXPECT_TRUE(pos != areg::INVALID_POS);
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
        EXPECT_TRUE(result == next);
        pos += 1;
    }

    pos = areg::find_first<char>(ch, param.source.data(), pos, true, &next);
    if (pos != areg::INVALID_POS)
    {
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
    }

    EXPECT_TRUE(areg::is_empty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestFindFirstChar, ::testing::ValuesIn<FindCharParams>(_listFindFirstCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestFindFirstChar, ::testing::ValuesIn<FindCharParams>(_listFindFirstCharParams));
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
TEST_P(StringTestFindLastChar, find_last_char)
{
    const FindCharParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char ch{ param.ch };
    const PhraseList& results{ param.results };
    areg::CharCount pos{ param.pos };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        std::string_view result{ results[i] };

        pos = areg::find_last<char>(ch, param.source.data(), pos, true, &next);
        EXPECT_TRUE(pos != areg::INVALID_POS);
        EXPECT_EQ(param.source[static_cast<uint32_t>(pos)], ch);
        EXPECT_TRUE(result == next);
        pos -= 1;

        EXPECT_NE(pos, areg::END_POS);
    }

    pos = areg::find_last<char>(ch, param.source.data(), pos, true, &next);
    EXPECT_EQ(pos, areg::INVALID_POS);
    EXPECT_TRUE(areg::is_empty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestFindLastChar, ::testing::ValuesIn<FindCharParams>(_listFindLastCharParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestFindLastChar, ::testing::ValuesIn<FindCharParams>(_listFindLastCharParams));
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
TEST_P(StringTestFindFirstPhrase, find_first_string)
{
    const FindPhraseParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char * phrase{ param.phrase.data() };
    const PhraseList& results{ param.results };
    areg::CharCount pos{ param.pos };
    areg::CharCount posTemp{ param.pos };
    const char* nextTemp{ nullptr };
    areg::CharCount phraseCount{ static_cast<areg::CharCount>(param.phrase.length()) };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        std::string_view result{ results[i] };

        posTemp = areg::find_first<char>(phrase, next, posTemp, param.isSensitive, &nextTemp);
        EXPECT_TRUE(result == nextTemp);
        posTemp = 0;


        pos = areg::find_first<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
        EXPECT_TRUE(pos != areg::INVALID_POS);
        areg::Ordering comp = areg::compare_strings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, areg::Ordering::Equal);
        EXPECT_TRUE(result == next);
        pos += phraseCount;
    }

    pos = areg::find_first<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
    if (pos != areg::INVALID_POS)
    {
        areg::Ordering comp = areg::compare_strings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, areg::Ordering::Equal);
    }

    EXPECT_TRUE(areg::is_empty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestFindFirstPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindFirstPhraseParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestFindFirstPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindFirstPhraseParams));
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
TEST_P(StringTestFindLastPhrase, find_last_string)
{
    const FindPhraseParams& param{ GetParam() };
    const char* next{ param.source.data() };
    const char * phrase{ param.phrase.data() };
    const PhraseList& results{ param.results };
    areg::CharCount pos{ param.pos };

    areg::CharCount phraseCount{ static_cast<areg::CharCount>(param.phrase.length()) };

    for (uint32_t i = 0; i < results.size(); ++i)
    {
        std::string_view result{ results[i] };

        pos = areg::find_last<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
        EXPECT_TRUE(pos != areg::INVALID_POS);
        areg::Ordering comp = areg::compare_strings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, areg::Ordering::Equal);
        EXPECT_TRUE(result == next);
    }

    pos = areg::find_last<char>(phrase, param.source.data(), pos, param.isSensitive, &next);
    if (pos != areg::INVALID_POS)
    {
        areg::Ordering comp = areg::compare_strings<char, char>(param.source.data() + pos, phrase, phraseCount, param.isSensitive);
        EXPECT_EQ(comp, areg::Ordering::Equal);
    }

    EXPECT_TRUE(areg::is_empty<char>(next));
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestFindLastPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindLastPhraseParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestFindLastPhrase, ::testing::ValuesIn<FindPhraseParams>(_listFindLastPhraseParams));
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
TEST_P(StringTestStartsEnds, string_starts_ends)
{
    const CompareSubstring& param{ GetParam() };
    const char* source = param.source.data();
    const char* substr = param.substr.data();

    if (param.isStart)
    {
        if (param.substr.length() > 1)
        {
            EXPECT_EQ(areg::string_starts_with<char>(source, substr, param.isSensetive), param.result);
        }
        else if (param.substr.length() == 1)
        {
            EXPECT_EQ(areg::string_starts_with<char>(source, *substr, param.isSensetive), param.result);
        }
        else
        {
            EXPECT_EQ(areg::string_starts_with<char>(source, substr, param.isSensetive), param.result);
            EXPECT_EQ(areg::string_starts_with<char>(source, *substr, param.isSensetive), param.result);
        }
    }
    else
    {
        if (param.substr.length() > 1)
        {
            EXPECT_EQ(areg::string_ends_with<char>(source, substr, param.isSensetive), param.result);
        }
        else if (param.substr.length() == 1)
        {
            EXPECT_EQ(areg::string_ends_with<char>(source, *substr, param.isSensetive), param.result);
        }
        else
        {
            EXPECT_EQ(areg::string_ends_with<char>(source, substr, param.isSensetive), param.result);
            EXPECT_EQ(areg::string_ends_with<char>(source, *substr, param.isSensetive), param.result);
        }
    }
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestStartsEnds, ::testing::ValuesIn<CompareSubstring>(_listCompareSubstring));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestStartsEnds, ::testing::ValuesIn<CompareSubstring>(_listCompareSubstring));
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
TEST_P(StringTestGetLine, string_get_line)
{
    const StringLines& param{ GetParam() };
    const char* source = param.source.data();
    const PhraseList& lines = param.lines;

    char buffer[256]{};
    char* next{ buffer };
    areg::copy_string(buffer, 256, source);

    for (const auto& line : lines)
    {
        const char * result = areg::line<char>(next, areg::END_POS, &next);
        EXPECT_TRUE(line == result);
    }

    EXPECT_TRUE(next == nullptr);

    const char* result = areg::line<char>(next, areg::END_POS, &next);
    EXPECT_TRUE(result == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestGetLine, ::testing::ValuesIn<StringLines>(_listStringLines));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestGetLine, ::testing::ValuesIn<StringLines>(_listStringLines));
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
TEST_P(StringTestGetInteger, string_get_integer)
{
    const StringInteger& param{ GetParam() };
    const char* source = param.source.data();
    const PhraseList& remains = param.remains;
    const std::vector<int>& digits = param.digits;

    const char* next{ source };

    uint32_t idx = 0;
    for (auto num : digits)
    {
        int result = areg::make_integer(next, &next);
        EXPECT_EQ(result, num);
        if (idx < remains.size())
        {
            EXPECT_TRUE(remains[idx] == next);
        }
    }

    int result = areg::make_integer(next, &next);
    EXPECT_EQ(result, 0);
    EXPECT_TRUE(next == nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestGetInteger, ::testing::ValuesIn<StringInteger>(_listStringDigits));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestGetInteger, ::testing::ValuesIn<StringInteger>(_listStringDigits));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/************************************************************************
 * Additional tests for missing cases in methods already covered above.
 ************************************************************************/

/**
 * \brief   Test string_length for wchar_t type.
 *          Verifies that the wide-character path of string_length computes the
 *          correct length, and that is_empty agrees with length == 0.
 **/
TEST(StringDefsTest1, string_length_wide)
{
    struct WideString
    {
        const wchar_t* str;
        uint32_t       len;
    };

    const WideString params[]
    {
          {L"12345"   , 5}
        , {L"654987"  , 6}
        , {nullptr    , 0}
        , {L""        , 0}
        , {L"abcdefg" , 7}
        , {L"ABCDEFG" , 7}
        , {L"ABCdefg" , 7}
        , {L"\n"      , 1}
        , {L"\r"      , 1}
        , {L"\n\r"    , 2}
        , {L"\r\n"    , 2}
    };

    for (const auto& entry : params)
    {
        EXPECT_EQ(areg::string_length<wchar_t>(entry.str), static_cast<areg::CharCount>(entry.len));
        EXPECT_TRUE(areg::is_empty<wchar_t>(entry.str) == (entry.len == 0));
    }
}

/**
 * \brief   Test string_line_length for wchar_t type.
 *          Verifies that the wide-character SIMD path (wcscspn) stops at the
 *          first '\r' or '\n' and handles null / empty inputs safely.
 **/
TEST(StringDefsTest1, string_line_length_wide)
{
    struct WideString
    {
        const wchar_t* str;
        uint32_t       len;
    };

    const WideString params[]
    {
          {L"12345"              , 5}   // plain string -- whole length
        , {L"12345\n67890\n"    , 5}   // stops at first '\n'
        , {L"12345\r67890\r"    , 5}   // stops at first '\r'
        , {L"12345\r\n67890"    , 5}   // stops at '\r' (DOS EOL)
        , {nullptr              , 0}   // null pointer -- length 0
        , {L""                  , 0}   // empty string -- length 0
        , {L"\n12345\n67890\n"  , 0}   // starts with '\n' -- line length 0
        , {L"\r12345\r67890\r"  , 0}   // starts with '\r' -- line length 0
        , {L"\t12345\n67890\n"  , 6}   // '\t' is not EOL -- counts tab + 5 chars
    };

    for (const auto& entry : params)
    {
        EXPECT_EQ(areg::string_line_length<wchar_t>(entry.str), static_cast<areg::CharCount>(entry.len));
    }
}

/************************************************************************
 * Tests for remove_char with case-insensitive comparison.
 ************************************************************************/

/**
 * \brief   Parameters for case-insensitive remove_char tests.
 *          Each entry holds a source string, a character to remove, and expected
 *          results for both caseSensitive=true and caseSensitive=false.
 **/
struct RemoveCharCaseParams
{
    std::string_view    source;         //!< Original string
    char                remove{};       //!< Character to search for
    std::string_view    resSensitive;   //!< Result: remove all, case-sensitive
    std::string_view    resInsensitive; //!< Result: remove all, case-insensitive
    std::string_view    resOneInsens;   //!< Result: remove first only, case-insensitive
};

//!< Parameters: mixed-case strings exercising both code paths
static constexpr RemoveCharCaseParams _listRemoveCharCaseParams[]
{
      { {"aAbBaA"  }, 'a', {"AbBA"  }, {"bB"   }, {"AbBaA" } } // 0: 'a' sens vs 'a'/'A' insens
    , { {"aAbBaA"  }, 'A', {"abBa"  }, {"bB"   }, {"AbBaA" } } // 1: 'A' sens vs 'A'/'a' insens (same set)
    , { {"xXyYzZ"  }, 'x', {"XyYzZ" }, {"yYzZ" }, {"XyYzZ" } } // 2: lowercase sens vs lower+upper insens
    , { {"xXyYzZ"  }, 'X', {"xyYzZ" }, {"yYzZ" }, {"XyYzZ" } } // 3: uppercase sens
    , { {"HELLO"   }, 'l', {"HELLO" }, {"HEO"  }, {"HELO"  } } // 4: case-insensitive 'l' matches 'L'; removes first 'L'
    , { {"HELLO"   }, 'L', {"HEO"   }, {"HEO"  }, {"HELO"  } } // 5: 'HELLO' has two 'L's; remove-all removes both
    , { {"aaaa"    }, 'a', {""      }, {""     }, {"aaa"   } } // 6: all same char
    , { {"nOmAtCh" }, 'z', {"nOmAtCh"}, {"nOmAtCh"}, {"nOmAtCh"} } // 7: char not present
};

//!< Declare test with parameters.
struct StringTestRemoveCharCase : public ::testing::TestWithParam<RemoveCharCaseParams>
{
    RemoveCharCaseParams params;
};

/**
 * \brief   Verify remove_char with caseSensitive=true and caseSensitive=false.
 *          Checks that the case-insensitive path removes both upper- and lower-case
 *          variants of the search character, while the case-sensitive path removes
 *          only the exact match.
 **/
TEST_P(StringTestRemoveCharCase, remove_char_case_insensitive)
{
    const RemoveCharCaseParams& param = GetParam();
    char buffer[64];

    // --- case-sensitive, remove all ---
    areg::copy_string<char>(buffer, 64, param.source.data(), areg::COUNT_ALL);
    areg::remove_char<char>(param.remove, buffer, true, true);
    EXPECT_EQ(areg::compare(buffer, param.resSensitive.data()), areg::Ordering::Equal)
        << "case-sensitive remove-all failed for source='" << param.source << "'";

    // --- case-insensitive, remove all ---
    areg::copy_string<char>(buffer, 64, param.source.data(), areg::COUNT_ALL);
    areg::remove_char<char>(param.remove, buffer, true, false);
    EXPECT_EQ(areg::compare(buffer, param.resInsensitive.data()), areg::Ordering::Equal)
        << "case-insensitive remove-all failed for source='" << param.source << "'";

    // --- case-insensitive, remove first only ---
    areg::copy_string<char>(buffer, 64, param.source.data(), areg::COUNT_ALL);
    const char* resume = areg::remove_char<char>(param.remove, buffer, false, false);
    EXPECT_EQ(areg::compare(buffer, param.resOneInsens.data()), areg::Ordering::Equal)
        << "case-insensitive remove-one failed for source='" << param.source << "'";
    EXPECT_TRUE(resume != nullptr);
}

#if defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_SUITE_P(StringDefsTest1, StringTestRemoveCharCase, ::testing::ValuesIn<RemoveCharCaseParams>(_listRemoveCharCaseParams));
#else   // !defined(INSTANTIATE_TEST_SUITE_P)
    INSTANTIATE_TEST_CASE_P(StringDefsTest1, StringTestRemoveCharCase, ::testing::ValuesIn<RemoveCharCaseParams>(_listRemoveCharCaseParams));
#endif  // defined(INSTANTIATE_TEST_SUITE_P)

/**
 * \brief   Test make_integer with non-null remain pointer.
 *          Verifies that the remain pointer is set to the first non-digit character
 *          after the numeric portion, and that parsing stops at the right position.
 **/
TEST(StringDefsTest1, make_integer_remain_pointer)
{
    const char* remain = nullptr;

    // Plain integer followed by non-digit suffix: remain must point to suffix start.
    {
        int result = areg::make_integer<char>("123abc", &remain);
        EXPECT_EQ(result, 123);
        EXPECT_TRUE(remain != nullptr);
        EXPECT_EQ(areg::compare(remain, "abc"), areg::Ordering::Equal);
    }

    // Negative integer followed by non-digit suffix.
    {
        int result = areg::make_integer<char>("-456xyz", &remain);
        EXPECT_EQ(result, -456);
        EXPECT_TRUE(remain != nullptr);
        EXPECT_EQ(areg::compare(remain, "xyz"), areg::Ordering::Equal);
    }

    // Leading whitespace + integer followed by non-digit.
    {
        int result = areg::make_integer<char>("  789 end", &remain);
        EXPECT_EQ(result, 789);
        EXPECT_TRUE(remain != nullptr);
        EXPECT_EQ(areg::compare(remain, "end"), areg::Ordering::Equal);
    }

    // String that starts with non-digit (not sign/whitespace): remain points to the whole string.
    {
        int result = areg::make_integer<char>("abc123", &remain);
        EXPECT_EQ(result, 0);
        EXPECT_TRUE(remain != nullptr);
        EXPECT_EQ(areg::compare(remain, "abc123"), areg::Ordering::Equal);
    }

    // String consumed fully (all digits): remain must be nullptr.
    {
        int result = areg::make_integer<char>("42", &remain);
        EXPECT_EQ(result, 42);
        EXPECT_TRUE(remain == nullptr);
    }

    // Null input: result = 0, remain = nullptr.
    {
        int result = areg::make_integer<char>(nullptr, &remain);
        EXPECT_EQ(result, 0);
        EXPECT_TRUE(remain == nullptr);
    }

    // Empty string: result = 0, remain = nullptr.
    {
        int result = areg::make_integer<char>("", &remain);
        EXPECT_EQ(result, 0);
        EXPECT_TRUE(remain == nullptr);
    }

    // Whitespace-only: result = 0, remain = nullptr (whitespace is skipped, sign undecided, EOS reached).
    {
        int result = areg::make_integer<char>("   ", &remain);
        EXPECT_EQ(result, 0);
        EXPECT_TRUE(remain == nullptr);
    }

    // Wide-character version: same semantics apply.
    {
        const wchar_t* wremain = nullptr;
        int result = areg::make_integer<wchar_t>(L"100end", &wremain);
        EXPECT_EQ(result, 100);
        EXPECT_TRUE(wremain != nullptr);
        EXPECT_EQ(areg::compare(wremain, L"end"), areg::Ordering::Equal);
    }
}
