//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2026 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of logging (tracing). The 
//               logging requires source code compilation with AREG_LOGS=1
//               preprocessor directive to enable logging macro. It as well
//               requires log enabling in the configuration file (by default 
//               "./config/areg.init" file). If there is no configuration file,
//               the logging can be forced to be enabled to apply default
//               logging settings.
// 
//               In this example enabling logging is forced and logging uses
//               default settings.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/base/String.hpp"
#include "areg/base/Thread.hpp"

#include "areg/logging/GELog.h"

#ifdef  _MSC_VER
    #pragma comment(lib, "areg")
#endif // _MSC_VER

DEF_LOG_SCOPE(logging_main_HelloThread_HelloThread);
DEF_LOG_SCOPE(logging_main_HelloThread_onThreadRuns);
DEF_LOG_SCOPE(logging_main_main);

//! \brief Thread to run
class HelloThread : public Thread, protected IEThreadConsumer
{
public:
    HelloThread()
        : Thread(*this, "HelloThread")
    {
        LOG_SCOPE(logging_main_HelloThread_HelloThread);
        LOG_DBG("Initialized thread [ %s ]", getName().getString());
    }

protected:
    /************************************************************************/
    // IEThreadConsumer interface overrides
    /************************************************************************/
    void onThreadRuns() override
    {
        LOG_SCOPE(logging_main_HelloThread_onThreadRuns);

        LOG_WARN("Thread [ %s ] running, outputting messages...", getName().getString());
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
    LOGGING_CONFIGURE_AND_START(nullptr);

    do
    {
        LOG_SCOPE(logging_main_main);

        LOG_DBG("Starting Hello World thread");
        HelloThread aThread;

        aThread.createThread(NECommon::WAIT_INFINITE);
        LOG_DBG("%s to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().getString());

        LOG_INFO("Stopping and destroying thread [ %s ]", aThread.getName().getString());
        Thread::eCompletionStatus status = aThread.shutdownThread(NECommon::WAIT_INFINITE);

        LOG_WARN_IF(Thread::eCompletionStatus::ThreadCompleted != status, "The thread exit abnormal, status = [ %d ]", static_cast<int>(status));
        LOG_INFO_IF(Thread::eCompletionStatus::ThreadCompleted == status, "The thread exit normal");

    } while (false);

    // Stop logging
    LOGGING_STOP();

    std::cout << "Exit application!" << std::endl;
    return 0;
}
