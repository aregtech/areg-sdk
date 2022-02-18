//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : This project demonstrates how to create custom event 
//               dispatching and simple threads.As events to dispatch, the 
//               demo uses timers and timer events.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/appbase/Application.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/Timer.hpp"
#include "areg/trace/GETrace.h"

#include <string_view>


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
    static constexpr std::string_view THREAD_NAME { "HelloThread" };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    HelloThread( void );

    virtual ~HelloThread( void ) = default;

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
    virtual void onThreadRuns( void ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden calls
//////////////////////////////////////////////////////////////////////////
private:
    inline HelloThread & self( void );
};

//////////////////////////////////////////////////////////////////////////
// HelloThread implementation
//////////////////////////////////////////////////////////////////////////

DEF_TRACE_SCOPE(main_HelloThread_HelloThread);
DEF_TRACE_SCOPE(main_HelloThread_onThreadRuns);

HelloThread::HelloThread( void )
    : Thread            ( self(), HelloThread::THREAD_NAME.data() )
    , IEThreadConsumer  ( )
{
    TRACE_SCOPE(main_HelloThread_HelloThread);
    TRACE_DBG("Initialized thread [ %s ]", HelloThread::THREAD_NAME.data());
}

inline HelloThread & HelloThread::self( void )
{
    return (*this);
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_INFO("The thread [ %s ] runs, going to output message", getName().c_str());
    TRACE_INFO("!!!Hello World!!! !!!Hello Tracing!!!");

    Thread::sleep( NECommon::WAIT_500_MILLISECONDS);
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
                        , private   IETimerConsumer
{
    /**
     * \brief   Thread name.
     **/
    static constexpr std::string_view THREAD_NAME { "HelloThread" };

public:

/************************************************************************/
// Constructor and destructor.
/************************************************************************/
    HelloDispatcher( void );

    virtual ~HelloDispatcher( void ) = default;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Triggered before dispatcher starts to dispatch events and when event dispatching just finished.
     * \param   hasStarted  The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief   The method is triggered to start dispatching valid event.
     *          Here dispatcher should forward message to appropriate
     *          registered event consumer
     * \param   eventElem   Event element to dispatch
     * \return  Returns true if at least one consumer processed event.
     *          Otherwise it returns false.
     **/
    virtual bool dispatchEvent( Event & eventElem ) override;

    /**
     * \brief   Posts event. Push event in internal or external
     *          event queue depending on event type.
     *          Thread should have registered consumer for
     *          specified event object.
     * \param   eventElem   The event object to push in the queue.
     * \return  Returns true if successfully pushed event in the queue.
     **/
    virtual bool postEvent( Event & eventElem ) override;

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

private:
    inline HelloDispatcher & self( void );  // wrapper of this pointer.
    Timer   mTimer; // a dummy timer to force to trigger event.
};

//////////////////////////////////////////////////////////////////////////
// HelloDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

// Define HelloDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
DEF_TRACE_SCOPE(main_HelloDispatcher_readyForEvents );
DEF_TRACE_SCOPE(main_HelloDispatcher_postEvent);

HelloDispatcher::HelloDispatcher( void )
    : DispatcherThread  (HelloDispatcher::THREAD_NAME.data())
    , IETimerConsumer   ( )

    , mTimer            ( static_cast<IETimerConsumer &>(self()), "aTimerName")
{
    TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
    TRACE_DBG("Instantiated hello dispatcher");
}

void HelloDispatcher::readyForEvents(bool isReady )
{
    TRACE_SCOPE( main_HelloDispatcher_readyForEvents );
    TRACE_DBG( "The dispatcher is running. The custom business logic can be set here ..." );

    if (isReady)
    {
        mTimer.startTimer(100);
    }
    else
    {
        mTimer.stopTimer();
    }
}

bool HelloDispatcher::postEvent( Event& eventElem )
{
    return EventDispatcher::postEvent( eventElem );
}

bool HelloDispatcher::dispatchEvent(Event & eventElem)
{
    TRACE_SCOPE(main_HelloDispatcher_postEvent);
    TRACE_DBG("Received event [ %s ], the custom event dispatching can be set here", eventElem.getRuntimeClassName());
    return true; // break dispatching event.
}

void HelloDispatcher::processTimer( Timer & timer )
{
    ASSERT(false);  // this never happens, since we break dispatching in dispatchEvent() method.
}

inline HelloDispatcher & HelloDispatcher::self( void )
{
    return (*this);
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
    printf("Initializing to test threads...\n");
    // Force to start logging. See outputs log files in appropriate "logs" subfolder.
    // To change the configuration and use dynamic logging, use macro TRACER_START_LOGGING
    // and specify the logging configuration file, where you can change logging format,
    // filter logging priority and scopes.
    TRACER_CONFIGURE_AND_START( nullptr );

    do
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        // Start timer manager
        Application::startTimerManager( );

        // declare thread object.
        TRACE_DBG("Starting Hello Thread");
        HelloThread helloThread;

        // create and start thread, wait until it is started.
        helloThread.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloThread.isValid() ? "SUCCEEDED" : "FAILED", helloThread.getName().c_str());

        TRACE_DBG("Starting Hello Dispatcher");
        HelloDispatcher helloDispatcher;
        // create and start thread, wait until it is started.
        helloDispatcher.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloDispatcher.isValid() ? "SUCCEEDED" : "FAILED", helloDispatcher.getName().c_str());

        TRACE_DBG("Main thread sleep");
        Thread::sleep( NECommon::WAIT_1_SECOND);

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloDispatcher.getName().c_str());
        helloDispatcher.destroyThread(NECommon::WAIT_INFINITE);

        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloThread.getName().c_str());
        helloThread.destroyThread(NECommon::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    printf("Testing threads completed, check logs...\n");

    return 0;
}
