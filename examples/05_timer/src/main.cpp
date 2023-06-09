//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
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

#include "areg/appbase/Application.hpp"
#include "areg/base/DateTime.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/Timer.hpp"
#include "areg/trace/GETrace.h"

#ifdef  _WIN32
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg.lib")
#endif // _WIN32

//! \brief  An example of a dispatcher thread, which starts and processes timers.
//!         Indifferent in which thread context the timers are started,
//!         they all are processed in the context of binding thread.
//!         The timer should have unique names.
class TimerDispatcher   : public    DispatcherThread
                        , private   IETimerConsumer
{
    static constexpr unsigned int TIMEOUT_ONE_TIME          { NECommon::TIMEOUT_1_MS * 500 }; //!< The timeout in milliseconds of one time timer
    static constexpr unsigned int TIMEOUT_PERIODIC_TIME     { NECommon::TIMEOUT_1_MS * 80  }; //!< The timeout in milliseconds of periodic timer
    static constexpr unsigned int TIMEOUT_CONTINUOUS_TIME   { NECommon::TIMEOUT_1_MS * 50  }; //!< The timeout in milliseconds of continues timer

public:
    explicit TimerDispatcher( const String & threadName );

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

    //! \brief  Triggered when Timer is expired.
    //!         The callback always runs in the thread context.
    //! 
    //! \param  timer   The timer object that is expired.
    virtual void processTimer( Timer & timer ) override;

    //! Override the default implementation.
    virtual bool postEvent( Event & eventElem ) override
    {
        // Make sure that only timer events are passed.
        ASSERT( RUNTIME_CAST( &eventElem, TimerEvent ) != nullptr );
        return EventDispatcher::postEvent( eventElem );
    }

private:
    Timer   mOneTime;       //!< One time timer
    Timer   mPeriodic;      //!< Periodic timer
    Timer   mContinuous;    //!< Continues timer

private:
    //! wraps 'this' pointer.
    inline TimerDispatcher & self( void )
    {
        return (*this);
    }
};

namespace
{
    DEF_TRACE_SCOPE( main_startTimerThread );
    DEF_TRACE_SCOPE( main_stopTimerThread );

    // A timeout to wait to let threads to run.
    constexpr unsigned int TIMEOUT_APPLICATION{ NECommon::TIMEOUT_1_SEC * 5 };

    //! Call to start a thread and timers.
    //! Note:   This method is called in 'main' thread, 
    //!         but timers are processed in the thread context.
    void startTimerThread( TimerDispatcher & aThread )
    {
        TRACE_SCOPE( main_startTimerThread );

        // create and start thread, wait until it is started.
        aThread.createThread( NECommon::WAIT_INFINITE );
        TRACE_DBG( "[ %s ] to create thread [ %s ]", aThread.isValid( ) ? "SUCCEEDED" : "FAILED", aThread.getName( ).getString( ) );

        TRACE_DBG( "Triggering timers...." );
        aThread.startTimers( );
    }

    //! Call to stop timers and stop thread.
    //! Note:   this function is called in 'main' thread.
    void stopTimerThread( TimerDispatcher & aThread )
    {
        TRACE_SCOPE( main_stopTimerThread );

        TRACE_INFO( "Stopping timers of thread [ %s ]", aThread.getName( ).getString( ) );
        aThread.stopTimers( );

        TRACE_DBG( "Completed demo, going to stop and exit dispatcher thread [ %s ]", aThread.getName( ).getString( ) );
        aThread.triggerExitEvent( );
        aThread.completionWait( NECommon::WAIT_INFINITE );

        TRACE_WARN( "The [ %s ] thread has completed job...", aThread.getName( ).getString( ) );
    }

}   // namespace

//////////////////////////////////////////////////////////////////////////
// TimerDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

// Define TimerDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
DEF_TRACE_SCOPE(main_TimerDispatcher_processTimer);
DEF_TRACE_SCOPE(main_TimerDispatcher_startTimers);
DEF_TRACE_SCOPE(main_TimerDispatcher_stopTimers);

TimerDispatcher::TimerDispatcher( const String & threadName )
    : DispatcherThread  (threadName)
    , IETimerConsumer   ( )

    , mOneTime          (self(), threadName + "_one_time")
    , mPeriodic         (self(), threadName + "_periodic" )
    , mContinuous       (self(), threadName + "_continuous")
{
    TRACE_SCOPE(main_TimerDispatcher_TimerDispatcher);
    TRACE_DBG("Instantiated timer dispatcher thread");
}

void TimerDispatcher::processTimer( Timer & timer )
{
    TRACE_SCOPE(main_TimerDispatcher_processTimer);
    TRACE_DBG("The timer [ %s ] has expired. Timeout [ %u ] ms, Event Count [ %d ], processing in Thread [ %s ]"
                , timer.getName( ).getString( )
                , timer.getTimeout()
                , timer.getEventCount()
                , getName().getString());

    // Use 'printf' to avoid overlapping output messages on console.
    printf( "%s : The timer [ %s ] is expired.\n", DateTime::getNow( ).formatTime( ).getString(), timer.getName( ).getString() );

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

//! A demo to run timer timers in the binding thread context.
//! Note:   The threads and timers are started in 'main',
//!         but they are processed in the binding thread context.
int main()
{
    std::cout << "Demo to run timers in the binding thread context ..." << std::endl;

    // Start tracing. Use default configuration file, which is "./config/log.init"
    Application::startTracer( nullptr, true);
    
    do 
    {
        // After initialization, set scope declaration in the block.
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

        TRACE_DBG("Initializing timer dispatching threads");

        // Start 'TimerThread_1'
        TimerDispatcher aThread1("TimerThread_1");
        startTimerThread(aThread1); // starts timers, which are dispatched in "TimerThread_1" context

        // Start 'TimerThread_2'
        TimerDispatcher aThread2("TimerThread_2");
        startTimerThread(aThread2); // starts timers, which are dispatched in "TimerThread_2" context

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

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
