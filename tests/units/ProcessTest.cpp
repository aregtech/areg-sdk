/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        units/ProcessTest.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, unit tests of areg::Process: identity of the running process, its paths, bitness and environment variables.
 ************************************************************************/

#include "units/GUnitTest.hpp"
#include "areg/base/Process.hpp"
#include "areg/base/String.hpp"
#include "areg/component/ServiceDefs.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

namespace
{
    //!< Sets the environment variable of the test process.
    bool set_env(const char * name, const char * value)
    {
#ifdef _WIN32
        return ::_putenv_s(name, value) == 0;
#else
        return ::setenv(name, value, 1) == 0;
#endif
    }
}

TEST(ProcessTest, instance_is_unique)
{
    EXPECT_EQ(&areg::Process::instance(), &areg::Process::instance());
}

TEST(ProcessTest, process_id_is_known)
{
    const areg::Process & process{ areg::Process::instance() };
    EXPECT_NE(process.id(), areg::Process::UNKNOWN_PROCESS);
    EXPECT_NE(process.id(), areg::Process::CURRENT_PROCESS);
}

TEST(ProcessTest, names_are_parts_of_the_full_path)
{
    const areg::Process & process{ areg::Process::instance() };
    ASSERT_FALSE(process.full_path().is_empty());
    ASSERT_FALSE(process.name().is_empty());
    ASSERT_FALSE(process.app_name().is_empty());

    const std::filesystem::path full(process.full_path().as_string());
    EXPECT_EQ(std::string(process.name().as_string()), full.filename().string());
    EXPECT_EQ(std::string(process.app_name().as_string()), full.stem().string());
    EXPECT_EQ(std::string(process.extension().as_string()), full.extension().string());
    EXPECT_EQ(std::string(process.path().as_string()), full.parent_path().string());
}

TEST(ProcessTest, name_is_application_name_with_extension)
{
    const areg::Process & process{ areg::Process::instance() };
    const std::string expected{ std::string(process.app_name().as_string()) + std::string(process.extension().as_string()) };
    EXPECT_EQ(std::string(process.name().as_string()), expected);
}

TEST(ProcessTest, path_is_an_existing_directory)
{
    const areg::Process & process{ areg::Process::instance() };
    ASSERT_FALSE(process.path().is_empty());
    EXPECT_TRUE(std::filesystem::is_directory(process.path().as_string()));
    EXPECT_TRUE(std::filesystem::exists(process.full_path().as_string()));
}

TEST(ProcessTest, environment_matches_the_pointer_size)
{
    const areg::Process & process{ areg::Process::instance() };
    EXPECT_EQ(static_cast<int32_t>(process.environment()), static_cast<int32_t>(sizeof(void *)));
    EXPECT_EQ(process.environment(), sizeof(void *) == 8u ? areg::Process::Bitness::Bits64 : areg::Process::Bitness::Bits32);
}

TEST(ProcessTest, bitness_matches_the_environment)
{
    const areg::Process & process{ areg::Process::instance() };
    const uint32_t expected{ sizeof(void *) == 8u
                                ? static_cast<uint32_t>(areg::InstanceBitness::Bitness64)
                                : static_cast<uint32_t>(areg::InstanceBitness::Bitness32) };
    EXPECT_EQ(process.bitness(), expected);
}

TEST(ProcessTest, bitness_names)
{
    EXPECT_STREQ(areg::Process::as_string(areg::Process::Bitness::Bits32), "32-bits");
    EXPECT_STREQ(areg::Process::as_string(areg::Process::Bitness::Bits64), "64-bits");
    EXPECT_STREQ(areg::Process::as_string(areg::Process::Bitness::BitsUnknown), "unknown bits");
    EXPECT_STREQ(areg::Process::as_string(static_cast<areg::Process::Bitness>(1234)), "ERR: Unexpected Process::Bitness value");
}

TEST(ProcessTest, environment_variable_that_is_set_is_returned)
{
    ASSERT_TRUE(set_env("AREG_PROCESS_TEST_VAR", "areg-value"));
    EXPECT_EQ(areg::Process::instance().safe_env_variable("AREG_PROCESS_TEST_VAR"), "areg-value");
}

TEST(ProcessTest, environment_variable_is_read_when_called)
{
    ASSERT_TRUE(set_env("AREG_PROCESS_TEST_CHANGE", "first"));
    EXPECT_EQ(areg::Process::instance().safe_env_variable("AREG_PROCESS_TEST_CHANGE"), "first");
    ASSERT_TRUE(set_env("AREG_PROCESS_TEST_CHANGE", "second"));
    EXPECT_EQ(areg::Process::instance().safe_env_variable("AREG_PROCESS_TEST_CHANGE"), "second");
}

TEST(ProcessTest, missing_environment_variable_is_empty)
{
    EXPECT_TRUE(areg::Process::instance().safe_env_variable("AREG_PROCESS_TEST_NO_SUCH_VARIABLE").is_empty());
}

TEST(ProcessTest, null_environment_variable_name_is_empty)
{
    EXPECT_TRUE(areg::Process::instance().safe_env_variable(nullptr).is_empty());
}
