/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0 License)
 * and Commercial (with various pricing models) licenses, depending on the
 * nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        bitness/bitness_test.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a provider and a consumer that exchange one sample of every data
 *              kind and report each field that arrives different. bitness.sh and bitness.ps1
 *              run them in 32-bit and 64-bit builds through one router.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "areg/appbase/Application.hpp"
#include "areg/component/Component.hpp"
#include "areg/component/ComponentLoader.hpp"
#include "areg/component/ComponentThread.hpp"
#include "areg/component/Model.hpp"

#include "tests/bitness/services/BitnessCheckProviderBase.hpp"
#include "tests/bitness/services/BitnessCheckConsumerBase.hpp"

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace
{
    //!< Bytes in the binary field; more than one 64 KB block.
    constexpr uint32_t BLOB_SIZE{ 70000u };

    //!< The exit code of the consumer: 0 when every path arrived equal.
    std::atomic_int     theExitCode{ 1 };

    //!< Prints one line and flushes it, so the driver reads it even if the process is killed.
    void report(const char * what, const char * field)
    {
        if ((field == nullptr) || (*field == '\0'))
            std::printf("bitness: %s ok\n", what);
        else
            std::printf("bitness: %s differs in %s\n", what, field);
        std::fflush(stdout);
    }

    //!< The sample both sides expect, with values at the edges of each type.
    BitnessCheck::Sample make_sample()
    {
        BitnessCheck::Sample sample;
        sample.small        = 0xFEu;
        sample.signed16     = -32000;
        sample.unsigned16   = 0xFFFEu;
        sample.signed32     = std::numeric_limits<int32_t>::min() + 1;
        sample.unsigned32   = 0xFFFFFFF0u;
        sample.signed64     = std::numeric_limits<int64_t>::min() + 7;
        sample.unsigned64   = std::numeric_limits<uint64_t>::max() - 11u;
        sample.real32       = 3.25f;
        sample.real64       = -1.0e300;
        sample.flag         = true;
        sample.letter       = 'Z';
        sample.level        = BitnessCheck::Level::Large;

        std::string text;
        for (int i = 0; i < 300; ++i)
            text += static_cast<char>('a' + (i % 26));
        sample.text = areg::String(text.c_str());

        std::vector<uint8_t> bytes(BLOB_SIZE);
        for (uint32_t i = 0; i < BLOB_SIZE; ++i)
            bytes[i] = static_cast<uint8_t>((i * 31u + 7u) & 0xFFu);
        sample.blob = areg::SharedBuffer(bytes.data(), BLOB_SIZE);

        sample.list.add(std::numeric_limits<int64_t>::min());
        sample.list.add(-1);
        sample.list.add(0);
        sample.list.add(static_cast<int64_t>(1) << 40);
        sample.list.add(std::numeric_limits<int64_t>::max());

        sample.counts.add_if_unique(areg::String("zero"), 0u);
        sample.counts.add_if_unique(areg::String("big"), static_cast<uint64_t>(1) << 40);
        sample.counts.add_if_unique(areg::String("max"), std::numeric_limits<uint64_t>::max());

        sample.inner.tag    = areg::String("inner");
        sample.inner.value  = -(static_cast<int64_t>(1) << 35);
        return sample;
    }

    //!< Returns the name of the first field that differs from the expected sample, or an empty string.
    const char * first_difference(const BitnessCheck::Sample & got)
    {
        const BitnessCheck::Sample want{ make_sample() };
        if (got.small       != want.small)      return "small (uint8)";
        if (got.signed16    != want.signed16)   return "signed16 (int16)";
        if (got.unsigned16  != want.unsigned16) return "unsigned16 (uint16)";
        if (got.signed32    != want.signed32)   return "signed32 (int32)";
        if (got.unsigned32  != want.unsigned32) return "unsigned32 (uint32)";
        if (got.signed64    != want.signed64)   return "signed64 (int64)";
        if (got.unsigned64  != want.unsigned64) return "unsigned64 (uint64)";
        if (got.real32      != want.real32)     return "real32 (float)";
        if (got.real64      != want.real64)     return "real64 (double)";
        if (got.flag        != want.flag)       return "flag (bool)";
        if (got.letter      != want.letter)     return "letter (char)";
        if (got.level       != want.level)      return "level (enum)";
        if (got.text        != want.text)       return "text (String)";
        if (got.blob        != want.blob)       return "blob (BinaryBuffer)";
        if (got.list        != want.list)       return "list (Array of int64)";
        if (got.counts      != want.counts)     return "counts (Map of String to uint64)";
        if (got.inner       != want.inner)      return "inner (Structure)";
        return "";
    }
}

class BitnessProvider final : public    areg::Component
                            , protected BitnessCheckProviderBase
{
public:
    BitnessProvider(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , BitnessCheckProviderBase(static_cast<areg::Component &>(*this))
    {
    }

protected:
    void request_echo(const BitnessCheck::Sample & sample) final
    {
        const char * field{ first_difference(sample) };
        report("provider request", field);
        set_latest(sample);
        broadcast_published(sample);
        response_echo(sample, areg::String(field));
    }

private:
    inline BitnessProvider & self()
    {
        return (*this);
    }
};

class BitnessConsumer final : public    areg::Component
                            , protected BitnessCheckConsumerBase
{
public:
    BitnessConsumer(const areg::ComponentEntry & entry, areg::ComponentThread & owner)
        : areg::Component(entry, owner)
        , BitnessCheckConsumerBase(entry.mDependencyServices[0].mRoleName, owner)
        , mReceived(0)
        , mFailed(false)
    {
    }

protected:
    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final
    {
        bool result{ false };
        if (BitnessCheckConsumerBase::service_connected(status, proxy))
        {
            result = true;
            if (areg::is_service_connected(status))
            {
                notify_on_latest_update(true);
                notify_on_broadcast_published(true);
                request_echo(make_sample());
            }
            else if (mReceived < 3)
            {
                std::printf("bitness: provider lost before every path arrived\n");
                std::fflush(stdout);
                areg::Application::signal_quit();
            }
        }

        return result;
    }

    void on_latest_update(const BitnessCheck::Sample & Latest, areg::DataState state) final
    {
        if (state == areg::DataState::DataIsOK)
        {
            arrived("attribute", first_difference(Latest));
        }
    }

    void broadcast_published(const BitnessCheck::Sample & sample) final
    {
        arrived("broadcast", first_difference(sample));
    }

    void response_echo(const BitnessCheck::Sample & sample, const areg::String & mismatch) final
    {
        if (mismatch.is_empty() == false)
        {
            mFailed = true;
        }

        arrived("response", first_difference(sample));
    }

    void request_echo_failed(areg::ResultType /*reason*/) final
    {
        std::printf("bitness: request failed\n");
        std::fflush(stdout);
        areg::Application::signal_quit();
    }

private:
    //!< Reports one path and ends the consumer when all three arrived.
    void arrived(const char * what, const char * field)
    {
        report(what, field);
        mFailed = mFailed || (*field != '\0');
        if (++mReceived == 3)
        {
            theExitCode.store(mFailed ? 1 : 0);
            areg::Application::signal_quit();
        }
    }

    inline BitnessConsumer & self()
    {
        return (*this);
    }

    uint32_t    mReceived;  //!< How many of the three paths arrived.
    bool        mFailed;    //!< True when a path or the provider found a difference.
};

constexpr char const PROVIDER_MODEL[]{ "BitnessProviderModel" };
constexpr char const CONSUMER_MODEL[]{ "BitnessConsumerModel" };

int main(int argc, char * argv[])
{
    const bool isProvider{ (argc == 3) && (std::strcmp(argv[1], "provider") == 0) };
    const bool isConsumer{ (argc == 3) && (std::strcmp(argv[1], "consumer") == 0) };
    if ((isProvider == false) && (isConsumer == false))
    {
        std::printf("usage: %s provider|consumer <config>\n", argv[0]);
        return 2;
    }

    std::printf("bitness: %s is %u-bit\n", argv[1], static_cast<unsigned int>(sizeof(void *) * 8u));
    std::fflush(stdout);

    const char * name{ isProvider ? PROVIDER_MODEL : CONSUMER_MODEL };
    areg::Model model(name);
    if (isProvider)
    {
        model.add_thread("BitnessProviderThread")
             .add_component<BitnessProvider>("BitnessProvider")
             .add_supported_service(BitnessCheck::ServiceName, BitnessCheck::InterfaceVersion);
    }
    else
    {
        model.add_thread("BitnessConsumerThread")
             .add_component<BitnessConsumer>("BitnessConsumer")
             .add_dependency_service("BitnessProvider");
    }

    areg::Application::setup(false, true, true, true, false, argv[2]);
    areg::ComponentLoader::add_model_unique(model);
    areg::Application::load_model(name);
    areg::Application::wait_quit(areg::WAIT_INFINITE);
    areg::Application::unload_model(name);
    areg::Application::release();
    return (isProvider ? 0 : theExitCode.load());
}
