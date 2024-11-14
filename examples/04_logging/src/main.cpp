//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
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

#ifdef  _WIN32
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg.lib")
#endif // _WIN32

//! \brief   A thread to run.
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
public:
    HelloThread( void );
    virtual ~HelloThread( void ) = default;

protected:

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    //! \brief  This callback is triggered when thread runs and fully operable.
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void )
    {
        return (*this);
    }
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////

DEF_LOG_SCOPE(main_HelloThread_HelloThread);
DEF_LOG_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread( self( ), "HelloThread" )
    , IEThreadConsumer  ( )
{
    LOG_SCOPE(main_HelloThread_HelloThread);
    LOG_DBG("Initialized thread [ %s ]", getName().getString());
}

void HelloThread::onThreadRuns( void )
{
    LOG_SCOPE(main_HelloThread_onThreadRuns);

    LOG_WARN("The thread [ %s ] runs, going to output messages ...", getName().getString());
    LOG_INFO("!!!Hello World!!!");
    LOG_DBG("!!!Hello Tracing!!!");
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
DEF_LOG_SCOPE(main_main);
//! \brief   Demo to run tracing / logging.
int main()
{
    std::cout << "Demo to run tracing / logging ..." << std::endl;

    // Force to start logging. See outputs log files in appropriate "logs" subfolder.
    // To change the configuration and use dynamic logging, use macro LOGGING_START
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    LOGGING_CONFIGURE_AND_START(nullptr);

    do
    {
        // After initialization, set scope declaration in the block.
        // Otherwise, the scope is inactive and logs are written
        LOG_SCOPE(main_main);

        LOG_DBG("Starting Hello World thread");
        HelloThread aThread;

        aThread.createThread(NECommon::WAIT_INFINITE);
        LOG_DBG("[ %s ] to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().getString());

        LOG_INFO("Going to stop and destroy [ %s ] thread.", aThread.getName().getString());
        aThread.shutdownThread(NECommon::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    LOGGING_STOP();

    std::cout << "Exit application!" << std::endl;
    return 0;
}
