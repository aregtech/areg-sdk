//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
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
     *          createThread() method should be called again.
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

    Thread::sleep(Thread::WAIT_500_MILLISECONDS);
}

//////////////////////////////////////////////////////////////////////////
// HelloDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   An example of thread that processes and dispatches timers.
 *          There can be several instances of thread.
 *          In case instantiate several instances of timer thread,
 *          since the have unique names, it makes sense to add
 *          thread name as a prefix in the name of timer. Otherwise,
 *          the names might be identical, then the processing can be invalid.
 **/
class HelloDispatcher   : public    DispatcherThread
{
    static const char * THREAD_NAME     /*= "Hello Thread"*/;
public:

/************************************************************************/
// Constructor and destructor.
/************************************************************************/
    HelloDispatcher( void );

    virtual ~HelloDispatcher( void );

protected:
/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target.
     *          This method must be overwritten to avoid assertion raise.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem );
};

//////////////////////////////////////////////////////////////////////////
// HelloDispatcher class implementation
//////////////////////////////////////////////////////////////////////////
const char * HelloDispatcher::THREAD_NAME     = "Hello Thread";

// Define HelloDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
DEF_TRACE_SCOPE(main_HelloDispatcher_postEvent);

HelloDispatcher::HelloDispatcher( void )
    : DispatcherThread(HelloDispatcher::THREAD_NAME)
{
    TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
    TRACE_DBG("Instantiated hello dispatcher");
}

HelloDispatcher::~HelloDispatcher( void )
{
}

bool HelloDispatcher::postEvent(Event & eventElem)
{
    TRACE_SCOPE(main_HelloDispatcher_postEvent);
    TRACE_DBG("Received event [ %s ]", eventElem.getRuntimeClassName());
    eventElem.destroy();
    return true;
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
    // To change the configuration and use dynamic logging, use macro TRACER_START_LOGGING
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    TRACER_FORCE_LOGGING();

    do
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        // declare thread object.
        TRACE_DBG("Starting Hello Thread");
        HelloThread helloThread;

        // create and start thread, wait until it is started.
        helloThread.createThread(Thread::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloThread.isValid() ? "SUCCEEDED" : "FAILED", helloThread.getName().getString());

        TRACE_DBG("Starting Hello Dispatcher");
        HelloDispatcher helloDispatcher;
        // create and start thread, wait until it is started.
        helloDispatcher.createThread(Thread::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloDispatcher.isValid() ? "SUCCEEDED" : "FAILED", helloDispatcher.getName().getString());

        TRACE_DBG("Main thread sleep");
        Thread::sleep(Thread::WAIT_1_SECOND);

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloDispatcher.getName().getString());
        helloDispatcher.destroyThread(Thread::WAIT_INFINITE);

        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloThread.getName().getString());
        helloThread.destroyThread(Thread::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    return 0;
}
