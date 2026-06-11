//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of logging (tracing). The 
//               logging requires source code compilation with AREG_LOGGING=1
//               preprocessor definition to enable logging macro. It as well
//               requires log enabling in the configuration file (by default 
//               "./config/areg.init" file). If there is no configuration file,
//               the logging can be forced to be enabled to apply default
//               logging settings.
// 
//               In this example enabling logging is forced and logging uses
//               default settings.
//============================================================================

#include "areg/base/areg_global.h"
#include "areg/base/ThreadConsumer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"

#include "areg/logging/areg_log.h"

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

DEF_LOG_SCOPE(logging_main_HelloThread, HelloThread);
DEF_LOG_SCOPE(logging_main_HelloThread, on_run);
DEF_LOG_SCOPE(logging_main, main);

//! \brief Thread to run
class HelloThread final : public areg::Thread, protected areg::ThreadConsumer
{
public:
    HelloThread()
        : areg::Thread(*this, "HelloThread")
    {
        LOG_SCOPE(logging_main_HelloThread, HelloThread);
        LOG_DBG("Initialized thread [ %s ]", name().as_string());
    }

protected:
    /************************************************************************/
    // ThreadConsumer interface overrides
    /************************************************************************/
    void on_run() override
    {
        LOG_SCOPE(logging_main_HelloThread, on_run);

        LOG_WARN("Thread [ %s ] running, outputting messages...", name().as_string());
        LOG_INFO("!!!Hello Thread!!!");
        LOG_DBG("!!!Hello Tracing!!!");
    }
};

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////

int main()
{
    std::cout << "Demo to run tracing / logging ..." << std::endl;

    // Forces to start logging with default settings (logs go to appropriate "logs" subfolder)
    LOGGING_CONFIGURE_AND_START(nullptr, true);

    do
    {
        LOG_SCOPE(logging_main, main);

        LOG_DBG("Starting Hello World thread");
        HelloThread aThread;

        aThread.start(areg::WAIT_INFINITE);
        LOG_DBG("%s to create thread [ %s ]", aThread.is_valid() ? "SUCCEEDED" : "FAILED", aThread.name().as_string());

        LOG_INFO("Stopping and destroying thread [ %s ]", aThread.name().as_string());
        [[maybe_unused]] areg::Thread::ThreadCompletion status = aThread.shutdown(areg::WAIT_INFINITE);
        LOG_WARN_IF(areg::Thread::ThreadCompletion::Completed != status, "The thread exit abnormal, status = [ %d ]", static_cast<int32_t>(status));
        LOG_INFO_IF(areg::Thread::ThreadCompletion::Completed == status, "The thread exit normal");

    } while (false);

    // Stop logging
    LOGGING_STOP();

    std::cout << "Exit application!" << std::endl;
    return 0;
}
