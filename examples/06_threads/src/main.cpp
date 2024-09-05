//============================================================================
// Name        : main.cpp
// Author      : Artak Avetyan
// Version     :
// Copyright   : (c) 2021-2023 Aregtech UG.All rights reserved.
// Description : This project demonstrates how to create custom event 
//               dispatching and simple threads.As events to dispatch, the 
//               demo uses timers and timer events.
//============================================================================

#include "areg/base/GEGlobal.h"

#include "areg/appbase/Application.hpp"
#include "areg/base/Thread.hpp"
#include "areg/base/IEThreadConsumer.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/component/Timer.hpp"

#include "areg/trace/GETrace.h"


#ifdef  _WIN32
    // link with areg library, valid only for MSVC
    #pragma comment(lib, "areg.lib")
#endif // _WIN32

//! A thread class.
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
    TRACE_DBG( "Initialized thread [ %s ]", "HelloThread" );
}

void HelloThread::onThreadRuns( void )
{
    TRACE_SCOPE(main_HelloThread_onThreadRuns);

    TRACE_INFO( "!!!Hello World!!! !!!Hello Tracing!!!" );
    TRACE_INFO("The thread [ %s ] runs, going to sleep for [ %u ] ms", getName().getString(), NECommon::WAIT_500_MILLISECONDS );

    Thread::sleep( NECommon::WAIT_500_MILLISECONDS);
}

//! \brief   A dispatcher thread that runs timer.
class HelloDispatcher   : public    DispatcherThread
                        , private   IETimerConsumer
{
public:
    HelloDispatcher( void );

    virtual ~HelloDispatcher( void ) = default;

protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// IEEventRouter interface overrides
/************************************************************************/

    /**
     * \brief   Triggered to start dispatching valid event.
     * \param   eventElem   Event element to dispatch
     * \return  Returns true if at least one consumer processed the event.
     **/
    virtual bool dispatchEvent( Event & eventElem ) override;

    /**
     * \brief   Posts event and pushes into the internal event queue.
     * \param   eventElem   The event object to push in the queue.
     * \return  Returns true if successfully pushed event in the queue.
     **/
    virtual bool postEvent( Event & eventElem ) override
    {
        return EventDispatcher::postEvent( eventElem );
    }

/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & /* timer */ ) override
    {
        // this never happens, since we break dispatching in dispatchEvent() method.
        ASSERT( false );
    }

private:
    //! 'this' pointer wrapper
    inline HelloDispatcher & self( void )
    {
        return (*this);
    }

private:

    Timer   mTimer; // a dummy timer to force to trigger event.
};

//////////////////////////////////////////////////////////////////////////
// HelloDispatcher class implementation
//////////////////////////////////////////////////////////////////////////

// Define HelloDispatcher trace scopes to make logging
// Trace scopes must be defined before they are used.
DEF_TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
DEF_TRACE_SCOPE(main_HelloDispatcher_readyForEvents );
DEF_TRACE_SCOPE(main_HelloDispatcher_dispatchEvent);

HelloDispatcher::HelloDispatcher( void )
    : DispatcherThread( "HelloDispatcher" )
    , IETimerConsumer   ( )

    , mTimer            ( static_cast<IETimerConsumer &>(self()), "aTimer")
{
    TRACE_SCOPE(main_HelloDispatcher_HelloDispatcher);
    TRACE_DBG("Instantiated hello dispatcher");
}

void HelloDispatcher::readyForEvents(bool isReady )
{
    TRACE_SCOPE( main_HelloDispatcher_readyForEvents );
    TRACE_DBG( "The dispatcher is running. The custom business logic can be set here ..." );

    DispatcherThread::readyForEvents( isReady );
    if (isReady)
    {
        mTimer.startTimer(100);
    }
    else
    {
        mTimer.stopTimer();
    }
}

#if AREG_LOGS
bool HelloDispatcher::dispatchEvent(Event & eventElem)
{
    TRACE_SCOPE(main_HelloDispatcher_dispatchEvent);
    TRACE_DBG("Received event [ %s ], the custom event dispatching can be set here", eventElem.getRuntimeClassName().getString());
    return true; // break dispatching event, so that it is never called 'processTimer()' method.
}
#else   // AREG_LOGS
bool HelloDispatcher::dispatchEvent(Event & /*eventElem*/)
{
    return true; // break dispatching event, so that it is never called 'processTimer()' method.
}
#endif  // AREG_LOGS

DEF_TRACE_SCOPE(main_main);

//! \brief   A Demo to create and destroy simple and dispatcher threads.
int main()
{
    std::cout << "A Demo to create and destroy simple and dispatcher threads ..." << std::endl;

    // Force to start logging. See outputs log files in appropriate "logs" subfolder.
    TRACER_CONFIGURE_AND_START( nullptr );

    do
    {
        // After initialization, set scope declaration in the block.
        TRACE_SCOPE(main_main);

        // Start timer manager
        Application::startTimerManager( );

        // Create and start 'Hello Thread'.
        TRACE_DBG("Starting Hello Thread");
        HelloThread helloThread;
        helloThread.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloThread.isValid() ? "SUCCEEDED" : "FAILED", helloThread.getName().getString());

        //Create and start 'Hello Dispatcher' thread.
        TRACE_DBG("Starting Hello Dispatcher");
        HelloDispatcher helloDispatcher;
        helloDispatcher.createThread(NECommon::WAIT_INFINITE);
        TRACE_DBG("[ %s ] to create thread [ %s ]", helloDispatcher.isValid() ? "SUCCEEDED" : "FAILED", helloDispatcher.getName().getString());

        TRACE_DBG("Main thread sleep");
        Thread::sleep( NECommon::WAIT_1_SECOND);

        // stop and destroy thread, clean resources. Wait until thread ends.
        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloDispatcher.getName().getString());
        helloDispatcher.shutdownThread(NECommon::WAIT_INFINITE);

        TRACE_INFO("Going to stop and destroy [ %s ] thread.", helloThread.getName().getString());
        helloThread.shutdownThread(NECommon::WAIT_INFINITE);

    } while (false);

    // Stop logging.
    TRACER_STOP_LOGGING();

    std::cout << "Exit application, check the logs for details!" << std::endl;
    return 0;
}
