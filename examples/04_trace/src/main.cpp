//============================================================================
// Name        : 04_trace.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/trace/GETrace.h"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

//////////////////////////////////////////////////////////////////////////
// HelloThread class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   A thread to run and output message
 */
class HelloThread   : public    Thread
                    , protected IEThreadConsumer
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   The thread name;
     */
    static const char * THREAD_NAME /* = "HelloThread" */;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    HelloThread( void );

    virtual ~HelloThread( void );

protected:

/************************************************************************/
// IEThreadConsumer interface overrides
/************************************************************************/

    /**
     * \brief   This callback function is called from Thread object, when it is
     *          running and fully operable. If thread needs run in loop, the loop
     *          should be implemented here. When consumer exits this function,
     *          the thread will complete work. To restart thread running,
     *          create() method should be called again.
     **/
    virtual void onThreadRuns( void );

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void );
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////
const char * HelloThread::THREAD_NAME   = "HelloThread";


DEF_TRACE_SCOPE(main_HelloThread_HelloThread);
DEF_TRACE_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread            ( self(), HelloThread::THREAD_NAME )
    , IEThreadConsumer  ( )
{
    TRACE_SCOPE(main_HelloThread_HelloThread);
    TRACE_DBG("Initialized thread [ %s ]", HelloThread::THREAD_NAME);
}

HelloThread::~HelloThread( void )
{
}

inline HelloThread & HelloThread::self( void )
{
    return (*this);
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().getString());
    TRACE_INFO("!!!Hello World!!!");
    TRACE_INFO("!!!Hello Tracing!!!");
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_main);
/**
 * \brief   Demo to create and destroy thread.
 */
int main()
{
    // Force to start logging. See outputs log files in appropriate "logs" subfolder.
    TRACER_FORCE_LOGGING();

    do
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        // declare thread object.
        TRACE_DBG("Starting Hello World thread");
        HelloThread aThread;

        // create and start thread, wait until it is started.
        aThread.createThread(Thread::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().getString());

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("Going to stop and destroy [ %s ] thread.", aThread.getName().getString());
        aThread.destroyThread(Thread::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    return 0;
}
