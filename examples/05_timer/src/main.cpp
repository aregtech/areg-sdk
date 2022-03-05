//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : Aregtech (c) 2021
// Description : This project demonstrates use of timers. The demo initializes,
//               starts and stops multiple timers processed in different 
//               threads. The timers require the start of Timer Manager (timer 
//               service). The timers are automatically dispatched in the timer
//               owner thread.
//
//               Following timers are used:
//                  - one time timer  ( it runs only once )
//                  - periodic timer  ( it runs certain amount of times )
//                  - continues timer ( it runs continuously until stopped ).
//============================================================================

#include "areg/base/GEGlobal.h"
#include "areg/base/DateTime.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"
#include "areg/appbase/Application.hpp"
#include "areg/trace/GETrace.h"

#ifdef WINDOWS
    #pragma comment(lib, "areg.lib")
#endif // WINDOWS

constexpr unsigned int TIMEOUT_APPLICATION  = Timer::TIMEOUT_1_SEC * 5;

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
    static constexpr unsigned int TIMEOUT_ONE_TIME          = Timer::TIMEOUT_1_MS * 500;//!< The timeout in milliseconds of one time timer
    static constexpr unsigned int TIMEOUT_PERIODIC_TIME     = Timer::TIMEOUT_1_MS * 80; //!< The timeout in milliseconds of periodic timer
    static constexpr unsigned int TIMEOUT_CONTINUOUS_TIME   = Timer::TIMEOUT_1_MS * 50; //!< The timeout in milliseconds of continues timer

public:

/************************************************************************/
// Constructor and destructor.
/************************************************************************/
    explicit TimerDispatcher( const char * threadName );

    virtual ~TimerDispatcher( void ) = default;

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
    virtual void processTimer( Timer & timer ) override;

    /**
     * \brief	Posts event and delivers to its target.
     *          Since the Dispatcher Thread is a Base object for
     *          Worker and Component threads, it does nothing
     *          and only destroys event object without processing.
     *          Override this method or use Worker / Component thread.
     * \param	eventElem	Event object to post
     * \return	In this class it always returns true.
     **/
    virtual bool postEvent( Event & eventElem ) override;

private:
    Timer   mOneTime;       //!< One time timer
    Timer   mPeriodic;      //!< Periodic timer
    Timer   mContinuous;    //!< Continues timer

private:
    // hidden method, used to wrap 'this' pointer in the constructor to avoid warnings.
    inline TimerDispatcher & self( void );
};

//////////////////////////////////////////////////////////////////////////
// TimerDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

// Define TimerDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
DEF_TRACE_SCOPE( main_TimerDispatcher_postEvent );
DEF_TRACE_SCOPE(main_TimerDispatcher_processTimer);
DEF_TRACE_SCOPE(main_TimerDispatcher_startTimers);
DEF_TRACE_SCOPE(main_TimerDispatcher_stopTimers);

TimerDispatcher::TimerDispatcher( const char * threadName )
    : DispatcherThread  (threadName)
    , IETimerConsumer   ( )

    , mOneTime          (self(), String(getName() + "_one_time"  ).getString())
    , mPeriodic         (self(), String(getName() + "_periodic"  ).getString())
    , mContinuous       (self(), String(getName() + "_continuous").getString())
{
    TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
    TRACE_DBG("Instantiated timer dispatcher");
}

inline TimerDispatcher & TimerDispatcher::self( void )
{
    return (*this);
}

bool TimerDispatcher::postEvent( Event & eventElem )
{
    TRACE_SCOPE( main_TimerDispatcher_postEvent );
    bool result = false;

    if ( RUNTIME_CAST( &eventElem, TimerEvent ) != nullptr )
    {
        result = EventDispatcher::postEvent( eventElem );
    }
    else
    {
        TRACE_ERR( "Unexpected event of type [ %s ] for this example. Raising assertion!", eventElem.getRuntimeClassName( ) );
        eventElem.destroy( );
        ASSERT(false);
    }

    return result;
}

void TimerDispatcher::processTimer( Timer & timer )
{
    TRACE_SCOPE(main_TimerDispatcher_processTimer);
    TRACE_DBG("The timer [ %s ] has expired. Timeout [ %u ] ms, Event Count [ %u ], processing in Thread [ %s ]", timer.getName( ).getString( ), timer.getTimeout(), timer.getEventCount(), getName().c_str());

    printf("[ %s ] : Timer [ %s ] expired...\n", DateTime::getNow().formatTime().c_str(), timer.getName().getString());

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

void TimerDispatcher::startTimers(void)
{
    TRACE_SCOPE(main_TimerDispatcher_startTimers);

    // Start one-time timer
    if (mOneTime.startTimer(TIMEOUT_ONE_TIME, self(), 1))
    {
        TRACE_DBG("Successfully started timer [ %s ] with timeout [ %u ].", mOneTime.getName().getString(), mOneTime.getTimeout());
    }
    else
    {
        TRACE_ERR("Failed to start timer [ %s ]", mOneTime.getName().getString());
    }

    // start periodic timer
    if (mPeriodic.startTimer(TIMEOUT_PERIODIC_TIME, self(), (TIMEOUT_APPLICATION / 2) / TIMEOUT_PERIODIC_TIME))
    {
        TRACE_DBG("Successfully started timer [ %s ] with timeout [ %u ].", mPeriodic.getName().getString(), mPeriodic.getTimeout());
    }
    else
    {
        TRACE_ERR("Failed to start timer [ %s ]", mPeriodic.getName().getString());
    }

    // start periodic timer (continuous). This timer runs until it is stopped.
    if (mContinuous.startTimer(TIMEOUT_CONTINUOUS_TIME, self(), Timer::CONTINUOUSLY))
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
    mOneTime.stopTimer();       //!< If one-time timer is expired (not active), nothing happens.
    mPeriodic.stopTimer();      //!< If periodic timer is still active, it is stopped.
    mContinuous.stopTimer();    //!< Continuous timer runs until it is stopped.
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
    aThread.createThread(NECommon::WAIT_INFINITE);
    TRACE_DBG("[ %s ] to create thread [ %s ]", aThread.isValid() ? "SUCCEEDED" : "FAILED", aThread.getName().c_str());

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

    TRACE_INFO("Stopping timers of thread [ %s ]", aThread.getName().c_str());
    aThread.stopTimers();

    TRACE_DBG("Comleted demo, going to stop and exit dispatcher thread [ %s ]", aThread.getName().c_str());
    aThread.triggerExitEvent();
    aThread.completionWait(NECommon::WAIT_INFINITE);

    TRACE_WARN("The [ %s ] thread has completed job...", aThread.getName().c_str());
}

/**
 * \brief   Demo to trigger timers in the separate thread.
 *          There can be several instances of thread created to run timers.
 */
int main()
{
    printf("Initializing timer for testing...\n");
    // Start tracing. Use default configuration file, which is "./config/log.init"
    // If not existing, force to start tracer.
    Application::startTracer( nullptr, true);
    
    do 
    {
        // set this part of code in a block (for example, 'do-while' block).
        // otherwise, the logs will not be visible, since in the time when
        // scope is initialized, the logging is not active yet.
        TRACE_SCOPE(main_main);

        // Start timer service
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

        // Start 'TimerThread_1'
        TimerDispatcher aThread1("TimerThread_1");
        startTimerThread( aThread1 );

        // Start 'TimerThread_2'
        TimerDispatcher aThread2("TimerThread_2");
        startTimerThread(aThread2);

        // Sleep for a while, let timers run
        TRACE_INFO("Main thread sleeping to let timers run..");
        Thread::sleep(TIMEOUT_APPLICATION);
        TRACE_INFO("Main thread resumed to stop timers...");

        // Stop timer threads.
        stopTimerThread(aThread1);
        stopTimerThread(aThread2);

        // Stop timer service, not more timers can run
        TRACE_WARN("Stopping timer manager, no timer can be triggered anymore...");
        Application::stopTimerManager();

    } while ( false );

    // Stop logging.
    Application::stopTracer();

    printf("Testing timer completed, check the logs...\n");

	return 0;
}
