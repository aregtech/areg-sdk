#ifndef AREG_COMPONENT_PRIVATE_CETIMERMANAGER_HPP
#define AREG_COMPONENT_PRIVATE_CETIMERMANAGER_HPP
/************************************************************************
 * \file        areg/component/private/CETimerManager.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/

/************************************************************************
 * Include files
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/private/CETimerManagingEvent.hpp"

#include "areg/base/ESynchObjects.hpp"
#include "areg/component/private/CETimerInfo.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
class CETimer;
class CEDispatcherThread;

//////////////////////////////////////////////////////////////////////////
// CETimerManager class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Time Manager object is starting and stopping timers.
 *              It generates Timer Events and forwards to Target Consumer.
 *              The Timer Manager is a singleton object and created
 *              only once on the first request until it is not requested
 *              to be stopped.
 * 
 * \details     When the timer is requested to be started, 
 *              the timer Manager generates Timer Managing event object
 *              and places in the queue of timer thread. If the timer thread
 *              is resumed because it received timer managing event, it 
 *              forwards to Timer Manager, which creates system timer.
 *              If timer thread is resumed because one of timers is
 *              fired, it triggers expired function of Timer Manager
 *              and the object is generating Timer Event and sends to
 *              the queue of Timer Consumer Thread.
 *
 **/
class CETimerManager    : protected CEDispatcherThread
                        , protected IETimerManagingEventConsumer
{

//////////////////////////////////////////////////////////////////////////
// Runtime declaration
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME(CETimerManager)

//////////////////////////////////////////////////////////////////////////
// Predefined constants and types
//////////////////////////////////////////////////////////////////////////    
private:
    /**
     * \brief   CETimerManager::TIMER_THREAD_NAME
     *          Timer Manager thread name
     **/
    static const char * const   TIMER_THREAD_NAME       /*= "_AREG_TIMER_THREAD_NAME_"*/;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// Private statics. Hidden
/************************************************************************/

    /**
     * \brief   Returns reference to Timer Manager object.
     **/
    static CETimerManager & GetTimerManager( void );

#ifdef _WINDOWS

    /**
     * \brief   Windows OS specific timer routine function. Triggered, when one of timer is expired.
     * \param   argPtr          The pointer of argument passed to timer expired callback function
     * \param   timerLowValue   The low value of timer expiration
     * \param   timerHighValue  The high value of timer expiration.
     **/
    static void _defaultWindowsTimerExpiredRoutine( void * argPtr, unsigned long timerLowValue, unsigned long timerHighValue );

#endif // _WINDOWS

#ifdef _POSIX

    /**
     * \brief   POSIX timer routine function. Triggered, when one of timer is expired.
     * \param   argSig          The value passed to thread signal when the timer was created.
     *                          This value is passed to routine callback.
     **/
    static void _defaultPosixTimerExpiredRoutine( union sigval argSig );

#endif // _POSIX

//////////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// Public statics
/************************************************************************/

    /**
     * \brief   If needed, creates Time Manager object and Timer Thread.
     *          Returns reference to the object.
     **/
    static bool StartTimerManager( void );

    /**
     * \brief   Stops Timer Manager and destroys Timer Thread.
     **/
    static void StopTimerManager( void );

    /**
     * \brief   Returns true if Timer Manager has been started and ready to process timers.
     **/
    static bool IsTimerManagerStarted( void );

    /**
     * \brief   Starts the timer. If succeeds, returns true.
     *          When timer event is fired, it will be dispatched in the
     *          thread where it was started, i.e. in the current thread.
     * \param   timer   The timer object that should be started
     * \return  Returns true if timer was successfully created.
     **/
    static bool StartTimer(CETimer &timer);

    /**
     * \brief   Starts the timer. If succeeds, returns true.
     *          The timer event will be dispatched in the specified
     *          thread context.
     * \param   timer       The timer object that should be started
     * \param   whichThread The dispatcher thread where the timer 
     *                      event should be dispatched.
     * \return  Returns true if timer was successfully created.
     **/
    static bool StartTimer(CETimer &timer, const CEDispatcherThread & whichThread);

    /**
     * \brief   Stops the timer. Returns true if timer successfully was stopped.
     * \param   timer   The timer object that should be stopped
     * \return  Returns true if timer was successfully stopped.
     **/
    static bool StopTimer(CETimer &timer);

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Constructor
     **/
    CETimerManager( void );
    /**
     * \brief   Destructor
     **/
    ~CETimerManager( void );

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IETimerManagingEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Automatically triggered when event is dispatched by timer thread
     * \param   data    The data object passed in event.
     **/
    virtual void ProcessEvent( const CETimerManagingEventData & data);

/************************************************************************/
// CEDispatcherThread overrides
/************************************************************************/

    /**
     * \brief	Posts event and delivers to its target thread / process.
     * \param	eventElem	Event object to post.
     * \return	Returns true if target was found and the event
     *          delivered with success. Otherwise it returns false.
     **/
    virtual bool PostEvent( CEEvent & eventElem );

    /**
     * \brief	Triggered when dispatcher starts running. 
     *          In this function runs main dispatching loop.
     *          Events are picked and dispatched here.
     *          Override if logic should be changed.
     * \return	Returns true if Exit Event is signaled.
     **/
    virtual bool RunDispatcher( void );

//////////////////////////////////////////////////////////////////////////
// Hidden operations. Called from Timer Thread.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Called by timer thread when it started 
     *          and before starting dispatching events
     **/
    void TimerThreadStarts( void );

    /**
     * \brief   Called when expired timers should be processed.
     **/
    void ProcessExpiredTimers( void );

    /**
     * \brief   Called by timer thread when it exits
     **/
    void TimerThreadExits( void );

    /**
     * \brief   Stops and removes all timers, i.e. unregisters all timers.
     **/
    void RemoveAllTimers( void );

    /**
     * \brief   Registers timer in the timer resource map.
     *          Before registering timer, it creates system timer.
     *          if successfully created, stores in the resource map.
     *          The timers should be unique in the timer map.
     *          When timer event is fired, it will be dispatched in the
     *          current thread where it was registered.
     * \param   timer       The timer object that should be registered
     * \return  Returns the position of timer in the map.
     **/
    MAPPOS RegisterTimer( CETimer & timer );

    /**
     * \brief   Registers timer in the timer resource map.
     *          Before registering timer, it creates system timer.
     *          if successfully created, stores in the resource map.
     *          The timers should be unique in the timer map.
     * \param   timer       The timer object that should be registered
     * \param   whichThread The dispatcher thread, where the timer event should be dispatched.
     * \return  Returns the position of timer in the map.
     **/
    MAPPOS RegisterTimer( CETimer & timer, const CEDispatcherThread & whichThread );

    /**
     * \brief   Registers timer in the timer resource map.
     *          Before registering timer, it creates system timer.
     *          if successfully created, stores in the resource map.
     *          The timers should be unique in the timer map.
     * \param   timer           The timer object that should be registered
     * \param   whichThreadId   The dispatcher thread, where the timer event should be dispatched.
     * \return  Returns the position of timer in the map.
     **/
    MAPPOS RegisterTimer( CETimer & timer, ITEM_ID whichThreadId );

    /**
     * \brief   Unregisters timer manager in the timer resource map.
     *          Before unregistering timer, it stops and closes system timer.
     * \param   timer   The timer object that should be unregistered.
     * \return  Returns true if timer object successfully is unregistered
     **/
    bool UnregisterTimer( CETimer & timer );

    /**
     * \brief   Returns true if timer is already exists in the timer map.
     * \param   timer   The timer object that should be checked.
     * \return  Returns true if timer exists, otherwise returns false
     **/
    inline bool IsTimerExist( const CETimer & timer ) const;

    /**
     * \brief   This method called for every single expired timer.
     *          The function is triggered from timer expired callback function.
     * \param   whichTimer  The pointer to timer object that has expired.
     * \param   highValue   The expired time high value
     * \param   lowValue    The expired time low value
     **/
    void TimerIsExpired( CETimer * whichTimer, unsigned int highValue, unsigned int lowValue );

    /**
     * \brief   Called when timer managing event is fired and the
     *          Timer Manager should run/start timer.
     * \param   whichTimer  Pointer to timer object to be started.
     * \return  
     **/
    void StartSystemTimer( CETimer * whichTimer );

    /**
     * \brief   Starts system timer and returns true if timer started with success.
     * \param   timerInfo   The timer information object
     * \return  Returns true if system timer started with success.
     **/
    bool StartSystemTimer( CETimerInfo & timerInfo );

    /**
     * \brief   Starts Timer Manager Thread it is not started yet.
     * \return  Returns true if Timer Manager Thread is started and ready to process events.
     **/
    bool StartTimerManagerThread( void );

    /**
     * \brief   Stops Timer Manager Thread and waits until it exists.
     **/
    void StopTimerManagerThread( void );

    /**
     * \brief   Returns CETimerManager object. for internal calls.
     **/
    inline CETimerManager & self( void );

//////////////////////////////////////////////////////////////////////////
//  Operating system specific methods
//////////////////////////////////////////////////////////////////////////

    /**
     * \brief   Creates waitable timer object. This method is operating system specific.
     * \param   timerName   The Name of timer to create. If NULL, no name is set for waitable timer.
     * \return  Returns the handle of created waitable timer or NULL if failed.
     **/
    static TIMERHANDLE _createWaitableTimer( const char * timerName );

    /**
     * \brief   Destroys previously created waitable timer.
     * \param   timerHandle The waitable timer handle to destroy.
     * \param   cancelTimer If true, before destroying waitable timer, cancels timer
     **/
    static void _destroyWaitableTimer( TIMERHANDLE timerHandle, bool cancelTimer );

    /**
     * \brief   Starts system timer and returns true if timer started with success.
     * \param   timerInfo   The timer information object
     * \return  Returns true if system timer started with success.
     **/
    static bool _startSystemTimer( CETimerInfo & timerInfo, MapTimerTable & timerTable );

    /**
     * \brief   Stops previously started waitable timer.
     * \param   timerHandle The waitable timer handle to destroy.
     **/
    static void _stopSystemTimer( TIMERHANDLE timerHandle );

//////////////////////////////////////////////////////////////////////////
//  Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The timer table object.
     **/
    MapTimerTable               mTimerTable;
    /**
     * \brief   List of expired timers.
     **/
    CEExpiredTimers             mExpiredTimers;
    /**
     * \brief   Synchronization object.
     **/
    mutable CEResourceLock   mLock;

//////////////////////////////////////////////////////////////////////////
//  Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    CETimerManager( const CETimerManager & /*src*/ );
    const CETimerManager & operator = ( const CETimerManager & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// CETimerManager class inline functions implementation
//////////////////////////////////////////////////////////////////////////

inline CETimerManager& CETimerManager::self( void )
{   return (*this); }

inline bool CETimerManager::IsTimerExist( const CETimer &timer ) const
{   CELock lock(mLock); return (mTimerTable.Find(static_cast<const CETimer *>(&timer)) != NULL);    }

#endif  // AREG_COMPONENT_PRIVATE_CETIMERMANAGER_HPP
