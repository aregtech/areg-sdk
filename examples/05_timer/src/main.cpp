//============================================================================
// Name        : 05_timer.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech © 2021
// Description : Hello World in C++, Ansi-style
//               This code example demonstrates timer processing in
//               own thread.
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

//////////////////////////////////////////////////////////////////////////
// TimerDispatcher class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   An example of thread that processes and dispatches timers.
 *          There can be several instances of thread.
 *          In case instantiate several instances of timer thread,
 *          since the have unique names, it makes sense to add
 *          thread name as a prefix in the name of timer. Otherwise,
 *          the names might be identical, then the processing can be invalid.
 **/
class TimerDispatcher   : public    DispatcherThread
                        , private   IETimerConsumer
{
public:

/************************************************************************/
// Constructor and destructor.
/************************************************************************/
    TimerDispatcher( const char * threadName );

    virtual ~TimerDispatcher( void );

/************************************************************************/
// operations
/************************************************************************/
    
    /**
     * \brief   Start timers to be processed in the Timer Thread.
     **/
    void startTimers( void );

    /**
     * \brief   Stops all timers.
     **/
    void stopTimers( void );

protected:
/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired. 
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer );

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

private:
    Timer   mOneTime;
    Timer   mPeriodic;
    Timer   mContinuous;

private:
    inline TimerDispatcher & self( void );
};

//////////////////////////////////////////////////////////////////////////
// TimerDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

// Define TimerDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
DEF_TRACE_SCOPE(main_TimerDispatcher_processTimer);
DEF_TRACE_SCOPE(main_TimerDispatcher_postEvent);
DEF_TRACE_SCOPE(main_TimerDispatcher_startTimers);
DEF_TRACE_SCOPE(main_TimerDispatcher_stopTimers);

TimerDispatcher::TimerDispatcher( const char * threadName )
    : DispatcherThread  (threadName)
    , IETimerConsumer   ( )

    , mOneTime          (self(), String(getName() + "_one_time_").getString())
    , mPeriodic         (self(), String(getName() + "_periodic_").getString())
    , mContinuous       (self(), String(getName() + "_continuous_").getString())
{
    TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
    TRACE_DBG("Instantiated timer dispatcher");
}

TimerDispatcher::~TimerDispatcher( void )
{
}

inline TimerDispatcher & TimerDispatcher::self( void )
{
    return (*this);
}

void TimerDispatcher::processTimer( Timer & timer )
{
    TRACE_SCOPE(main_TimerDispatcher_processTimer);
    TRACE_DBG("The timer [ %s ] has expired", timer.getName().getString());
    TRACE_DBG("... Timeout [ %u ] ms, Event Count [ %u ], processing in Thread [ %s ]", timer.getTimeout(), timer.getEventCount(), getName().getString());

    if (&timer == &mOneTime)
    {
        TRACE_DBG("One time timer is detected, can be processed. The timer state is [ %s ]", mOneTime.isActive() ? "ACTIVE" : "INACTIVE");
    }
    else if (&timer == &mPeriodic)
    {
        TRACE_DBG("Periodic timer is detected, can be processed. The timer state is [ %s ]", mPeriodic.isActive() ? "ACTIVE" : "INACTIVE");
    }
    else if (&timer == &mContinuous)
    {
        TRACE_DBG("Continuous timer is detected, can be processed. The timer state is [ %s ]", mContinuous.isActive() ? "ACTIVE" : "INACTIVE");
    }
    else
    {
        TRACE_ERR("Unexpected timer has been triggered. This should not happen...");
    }
}

bool TimerDispatcher::postEvent(Event & eventElem)
{
    bool result = false;
    if ( RUNTIME_CAST(&eventElem, TimerEvent) != NULL)
    {
        result = EventDispatcher::postEvent(eventElem);
    }
    else
    {
        TRACE_SCOPE(main_TimerDispatcher_postEvent);
        TRACE_ERR("Not a TimerEvent, cannot Post. Destroying event type [ %s ]", eventElem.getRuntimeClassName());
        eventElem.destroy();
    }

    return result;
}

void TimerDispatcher::startTimers(void)
{
    TRACE_SCOPE(main_TimerDispatcher_startTimers);
    TRACE_DBG("Starting timers...");

    // Start one-time timer
    if (mOneTime.startTimer(Timer::TIMEOUT_1_SEC * 20, self(), 1))
    {
        TRACE_DBG("Successfully started timer [ %s ] with timeout [ %u ].", mOneTime.getName().getString(), mOneTime.getTimeout());
    }
    else
    {
        TRACE_ERR("Failed to start timer [ %s ]", mOneTime.getName().getString());
    }

    // start periodic timer
    if (mPeriodic.startTimer(Timer::TIMEOUT_1_SEC, self(), 10))
    {
        TRACE_DBG("Successfully started timer [ %s ] with timeout [ %u ].", mPeriodic.getName().getString(), mPeriodic.getTimeout());
    }
    else
    {
        TRACE_ERR("Failed to start timer [ %s ]", mPeriodic.getName().getString());
    }

    // start periodic timer (continuous). This timer runs until it is stopped.
    if (mContinuous.startTimer(Timer::TIMEOUT_500_MS, self(), Timer::CONTINUOUSLY))
    {
        // continuous timer
        TRACE_DBG("Successfully started timer [ %s ] with timeout [ %u ].", mContinuous.getName().getString(), mContinuous.getTimeout());
    }
    else
    {
        TRACE_ERR("Failed to start timer [ %s ]", mContinuous.getName().getString());
    }
}

void TimerDispatcher::stopTimers(void)
{
    TRACE_SCOPE(main_TimerDispatcher_stopTimers);
    TRACE_INFO("Stopping timers.....");

    // Stop all timers
    mOneTime.stopTimer();       //<! If one-time timer is expired (not active), nothing happens.
    mPeriodic.stopTimer();      //<! If periodic timer is still active, it is stopped.
    mContinuous.stopTimer();    //<! Continuous timer runs until it is stopped.
}

//////////////////////////////////////////////////////////////////////////
// Demo
//////////////////////////////////////////////////////////////////////////
DEF_TRACE_SCOPE(main_main);
DEF_TRACE_SCOPE(main_startTimerThread);
DEF_TRACE_SCOPE(main_stopTimerThread);

/**
 * \brief   Start thread and start timers.
 * \param   aThread     The thread with timers to start.
 **/
static void startTimerThread( TimerDispatcher & aThread )
{
    TRACE_SCOPE(main_startTimerThread);

    // create and start thread, wait until it is started.
    aThread.createThread(Thread::WAIT_INFINITE);
    TRACE_DBG("[ %s ] to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().getString());

    TRACE_DBG("Triggering timers....");
    aThread.startTimers();
}

/**
 * \brief   Stops timers and thread.
 * \param   aThread     The thread with timers to stop.
 **/
static void stopTimerThread( TimerDispatcher & aThread )
{
    TRACE_SCOPE(main_stopTimerThread);

    TRACE_INFO("Stopping timers of thread [ %s ]", aThread.getName().getString());
    aThread.stopTimers();

    TRACE_DBG("Comleted demo, going to stop and exit dispatcher thread [ %s ]", aThread.getName().getString());
    aThread.triggerExitEvent();
    aThread.completionWait(Thread::WAIT_INFINITE);

    TRACE_WARN("The [ %s ] thread has completed job...", aThread.getName().getString());
}

/**
 * \brief   Demo to trigger timers in the separate thread.
 *          There can be several instances of thread created to run timers.
 */
int main()
{
    // Start tracing. Use default configuration file, which is "./config/log.init"
    // If not existing, force to start tracer.
    Application::startTracer(NULL, true);
    
    do 
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        TRACE_INFO("Starting timer manager...");
        if (Application::startTimerManager())
        {
            TRACE_INFO("Succeeded to start timer manager...");
        }
        else
        {
            TRACE_ERR("Failed to start timer manager!!!");
        }

        // declare thread object.
        TRACE_DBG("Initializing timer dispatching threads");

        TimerDispatcher aThread1("TimerThread_1");
        startTimerThread(aThread1);

        TimerDispatcher aThread2("TimerThread_2");
        startTimerThread(aThread2);

        TRACE_INFO("Main thread sleeping to let timers run..");
        Thread::sleep(Thread::WAIT_1_SECOND * 30);
        TRACE_INFO("Main thread resumed to stop timers...");

        stopTimerThread(aThread1);
        stopTimerThread(aThread2);

        TRACE_WARN("Stopping timer manager, no timer can be triggered anymore...");
        Application::stopTimerManager();

    } while ( false );

    // Stop logging.
    Application::stopTracer();

	return 0;
}
