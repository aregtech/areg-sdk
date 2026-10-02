/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        conn-scenario/conn_scenario.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform, a provider and a consumer that show what a connection does
 *              when a peer pauses, slows down or disappears. Driven by conn_scenario.py.
 *
 *              provider <config> <bytes> <gap-us>
 *                  broadcasts numbered blocks of <bytes>, one every <gap-us> (0 = full rate).
 *              consumer <config> <name>
 *                  checks every block for its number and content and prints what it saw.
 *
 *              Every line starts with a tag and the monotonic time in milliseconds:
 *              P (provider, once a second), C (consumer, once a second), CONN, GAP, STALL, BAD.
 ************************************************************************/

#include "areg/base/areg_global.h"
#include "conn-scenario/conn_scenario.hpp"
#include "areg/appbase/Application.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _MSC_VER
    #pragma comment(lib, "areg")
    #pragma comment(lib, "25_generated")
    #pragma comment(lib, "32_generated")
#endif // _MSC_VER

using namespace scenario;

int main(int argc, char * argv[])
{
    const bool isProvider{ (argc == 5) && (std::strcmp(argv[1], "provider") == 0) };
    const bool isConsumer{ (argc == 4) && (std::strcmp(argv[1], "consumer") == 0) };
    const bool isBoth{ (argc == 6) && (std::strcmp(argv[1], "both") == 0) };
    if ((isProvider == false) && (isConsumer == false) && (isBoth == false))
    {
        ::printf("usage: %s provider <config> <bytes> <gap-us>\n"
                 "       %s consumer <config> <name>\n"
                 "       %s both <config> <bytes> <gap-us> <name>\n", argv[0], argv[0], argv[0]);
        return 1;
    }

    Options & opt{ options() };
    opt.config = argv[2];
    if (isProvider || isBoth)
    {
        opt.bytes = static_cast<uint32_t>(std::strtoul(argv[3], nullptr, 10));
        opt.gapUs = static_cast<uint32_t>(std::strtoul(argv[4], nullptr, 10));
        opt.name  = isBoth ? argv[5] : opt.name;
    }
    else
    {
        opt.name = argv[3];
        const char * bytes{ std::getenv("CONN_SCENARIO_BYTES") };
        opt.bytes = bytes != nullptr ? static_cast<uint32_t>(std::strtoul(bytes, nullptr, 10)) : opt.bytes;
    }

    const char * model{ isConsumer ? CONSUMER_MODEL : PROVIDER_MODEL };
    areg::Application::setup(std::getenv("CONN_SCENARIO_LOG") != nullptr, true, true, true, false, opt.config.c_str());
    areg::Application::load_model(model);
    if (isBoth)
    {
        areg::Application::load_model(CONSUMER_MODEL);
    }

    areg::Application::wait_quit(areg::WAIT_INFINITE);
    if (isBoth)
    {
        areg::Application::unload_model(CONSUMER_MODEL);
    }

    areg::Application::unload_model(model);
    areg::Application::release();

    return 0;
}
