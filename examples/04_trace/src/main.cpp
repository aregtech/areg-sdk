//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates use of logging (tracing). The 
//               logging requires source code compilation with AREG_LOGS=1
//               preprocessor directive to enable logging macro. It as well
//               requires log enabling in the configuration file (by default 
//               "./config/log.init" file). If there is no configuration file,
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

#include "areg/trace/GETrace.h"

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

DEF_TRACE_SCOPE(main_HelloThread_HelloThread);
DEF_TRACE_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread( self( ), "HelloThread" )
    , IEThreadConsumer  ( )
{
    TRACE_SCOPE(main_HelloThread_HelloThread);
    TRACE_DBG("Initialized thread [ %s ]", getName().getString());
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_WARN("The thread [ %s ] runs, going to output messages ...", getName().getString());
    TRACE_INFO("!!!Hello World!!!");
    TRACE_DBG("!!!Hello Tracing!!!");
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_main);
//! \brief   Demo to run tracing / logging.
int main()
{
    std::cout << "Demo to run tracing / logging ..." << std::endl;

    // Force to start logging. See outputs log files in appropriate "logs" subfolder.
    // To change the configuration and use dynamic logging, use macro TRACER_START_LOGGING
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    TRACER_CONFIGURE_AND_START(nullptr);

    do
    {
        // After initialization, set scope declaration in the block.
        // Otherwise, the scope is inactive and logs are written
        TRACE_SCOPE(main_main);

        TRACE_DBG("Starting Hello World thread");
        HelloThread aThread;

        aThread.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().getString());

        TRACE_INFO("Going to stop and destroy [ %s ] thread.", aThread.getName().getString());
        aThread.destroyThread(NECommon::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    std::cout << "Exit application!" << std::endl;
    return 0;
}
