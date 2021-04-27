/************************************************************************
 * \file        areg/component/private/CETimerManager.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, The System Timer Manager.
 *              Controlling, triggering and stopping timer.
 *
 ************************************************************************/
#include "areg/component/private/CETimerManager.hpp"

#include "areg/component/CETimerEvent.hpp"
#include "areg/component/private/CETimerEventData.hpp"
#include "areg/component/private/CEExitEvent.hpp"

#include "areg/component/CETimer.hpp"
#include "areg/base/CEThread.hpp"
#include "areg/base/NEUtilities.hpp"

#define _MILLISECOND    10000

//////////////////////////////////////////////////////////////////////////
// CETimerManager class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Implement Runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CETimerManager, CEDispatcherThread)

//////////////////////////////////////////////////////////////////////////
// Predefined constants
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Timer Manager thread name
 **/
const char * const CETimerManager::TIMER_THREAD_NAME        = "_AREG_TIMER_THREAD_NAME_";

//////////////////////////////////////////////////////////////////////////
// Static functions
//////////////////////////////////////////////////////////////////////////

CETimerManager & CETimerManager::GetTimerManager( void )
{
    static CETimerManager _timerManager;
    return _timerManager;
}

bool CETimerManager::StartTimerManager( void )
{
    return GetTimerManager().StartTimerManagerThread( );
}

void CETimerManager::StopTimerManager( void )
{
    return GetTimerManager().StopTimerManagerThread();
}

bool CETimerManager::IsTimerManagerStarted( void )
{
    bool result = false;
    CETimerManager & timerManager = GetTimerManager();

    do 
    {
        CELock lock( timerManager.mLock );
        result = timerManager.IsReady();
    } while (false);

    return result;
    
}

bool CETimerManager::StartTimer( CETimer &timer )
{
    CETimerManager & timerManager = GetTimerManager();

    MAPPOS atPosition = timerManager.RegisterTimer(timer);
    CETimerManagingEventData data(timer, CETimerManagingEventData::TimerStart);
    return (atPosition != NULL ? CETimerManagingEvent::SendEvent(data, static_cast<IETimerManagingEventConsumer &>(timerManager), static_cast<CEDispatcherThread &>(timerManager)) : false);
}

bool CETimerManager::StartTimer(CETimer &timer, const CEDispatcherThread & whichThread)
{
    CETimerManager & timerManager = GetTimerManager();

    MAPPOS atPosition = timerManager.RegisterTimer(timer, whichThread);
    CETimerManagingEventData data(timer, CETimerManagingEventData::TimerStart);
    return (atPosition != NULL ? CETimerManagingEvent::SendEvent(data, static_cast<IETimerManagingEventConsumer &>(timerManager), static_cast<CEDispatcherThread &>(timerManager)) : false);
}

bool CETimerManager::StopTimer( CETimer &timer )
{
    return GetTimerManager().UnregisterTimer( timer );
}

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////

CETimerManager::CETimerManager( void )
    : CEDispatcherThread            ( CETimerManager::TIMER_THREAD_NAME )
    , IETimerManagingEventConsumer ( )

    , mTimerTable   ( )
    , mExpiredTimers( )
    , mLock         ( )
{
    ; // do nothing
}

CETimerManager::~CETimerManager( void )
{
    RemoveAllTimers( );
}

//////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////
MAPPOS CETimerManager::RegisterTimer( CETimer &timer )
{
    return RegisterTimer(timer, CEDispatcherThread::GetCurrentDispatcherThread());
}

MAPPOS CETimerManager::RegisterTimer(CETimer &timer, const CEDispatcherThread & whichThread)
{
    return ( whichThread.IsValid() ? RegisterTimer(timer, whichThread.GetThreadId()) : NULL );
}

MAPPOS CETimerManager::RegisterTimer(CETimer &timer, ITEM_ID whichThreadId)
{
    MAPPOS result = NULL;

    if (CEThread::FindThreadById(whichThreadId) != NULL)
    {
        UnregisterTimer(timer);
        TIMERHANDLE timerHandle = CETimerManager::_createWaitableTimer(timer.HasName() ? timer.GetTimerName().GetBuffer() : NULL_STRING);
        if (timerHandle != static_cast<TIMERHANDLE>(NULL))
        {
            CELock lock(mLock);
            result = mTimerTable.SetKey(&timer, CETimerInfo(timer, timerHandle, whichThreadId) );
            if (result == NULL)
            {
                OUTPUT_ERR("Failed inserting new Timer object in the timer table. Ignoring starting timer [ %s ]", timer.GetTimerName().GetBuffer());
                CETimerManager::_destroyWaitableTimer(timerHandle, false);
            }
            else
            {
                OUTPUT_DBG("Successfully registered timer [ %s ]", timer.GetTimerName().GetBuffer());
            }
        }
    }
#ifdef DEBUG
    else
    {
        OUTPUT_ERR("The Timer Manager cannot find thread by id [ %p ], cancelling timer [ %s ]", whichThreadId, timer.GetTimerName().GetBuffer());
    }
#endif // DEBUG

    return result;
}

bool CETimerManager::UnregisterTimer( CETimer &timer )
{
    CELock lock(mLock);

    MAPPOS pos = mTimerTable.Find(static_cast<const CETimer *>(&timer));
    if (pos != NULL)
    {
        CETimerInfo timerInfo = mTimerTable.RemoveAt(pos);
        
        ASSERT(mTimerTable.Find(static_cast<const CETimer *>(&timer)) == NULL);
        ASSERT(timerInfo.GetHandle() != NULL);
        CETimerManager::_destroyWaitableTimer( timerInfo.GetHandle(), true );

        OUTPUT_DBG("Successfully unregistered timer [ %s ].", timer.GetTimerName().GetBuffer());
    }
    return (pos != NULL);
}

void CETimerManager::RemoveAllTimers( void )
{
    CELock  lock(mLock);

    mExpiredTimers.RemoveAll();

    MAPPOS pos = mTimerTable.GetStartPosition();
    CETimer*    mapKey  = NULL;
    CETimerInfo timerInfo;

    while ( pos != NULL )
    {
        pos = mTimerTable.GetNextPosition(pos, mapKey, timerInfo);
        ASSERT(mapKey != NULL && timerInfo.GetHandle() != NULL);
        CETimerManager::_destroyWaitableTimer( timerInfo.GetHandle( ), true );
    }

    mTimerTable.RemoveAll();
}

void CETimerManager::ProcessEvent( const CETimerManagingEventData & data )
{
    switch(data.GetAction())
    {
    case CETimerManagingEventData::TimerStart:
        StartSystemTimer(data.GetTimer());
        break;

    default:
        break; // ignore
    }
}

void CETimerManager::ProcessExpiredTimers( void )
{
    while (mExpiredTimers.IsEmpty() == false)
    {
        CELock lock(mLock);

        CEExpiredTimerInfo expiredTime = mExpiredTimers.RemoveHead();
        CETimerInfo timerInfo;
        if (expiredTime.mTimer != NULL && mTimerTable.Find(expiredTime.mTimer, timerInfo))
        {
            CETimer* currTimer = timerInfo.mTimer;
            if (timerInfo.TimerIsExpired(expiredTime.mHighValue, expiredTime.mLowValue))
            {
                if (timerInfo.IsActive())
                {
                    timerInfo.mTimerState = CETimerInfo::TimerPending;
                    mTimerTable.UpdateKey(currTimer, timerInfo);
                }
                else
                {
                    OUTPUT_WARN("The Timer [ %s ] is not active anymore. Going to unregister", currTimer->GetTimerName().GetBuffer());
                    UnregisterTimer(*currTimer);
                }

                do 
                {
                    // Send timer event. Unlock and lock mutex
                    lock.Unlock();
                    CETimerEvent::SendEvent(*timerInfo.mTimer, timerInfo.mOwnThreadId);
                    lock.Lock();

                } while (false);
            }
            else
            {
                OUTPUT_ERR("The timer [ %s ] has expired, but it was ignored. The current state of timer is [ %s ]. Ignoring sending event.", currTimer->GetTimerName().GetBuffer(), CETimerInfo::GetString(timerInfo.GetState()));
            }
        }
        else
        {
            OUTPUT_ERR("The timer [ %s ] did not find in the table.", expiredTime.mTimer->GetTimerName().GetBuffer());
        }
    }
}

void CETimerManager::TimerIsExpired( CETimer* whichTimer, unsigned int highValue, unsigned int lowValue )
{
    mExpiredTimers.AddTail( CEExpiredTimerInfo(whichTimer, highValue, lowValue) );
}

void CETimerManager::StartSystemTimer( CETimer* whichTimer )
{
    CELock lock(mLock);

    if (whichTimer != NULL)
    {
        CETimerInfo timerInfo;

        if (mTimerTable.Find(whichTimer, timerInfo))
        {
            ASSERT(timerInfo.mTimer == whichTimer);
            ASSERT(timerInfo.mHandle != NULL);
            if (StartSystemTimer(timerInfo) == false)
            {
                OUTPUT_ERR("Failed starting timer [ %s ]", whichTimer->GetTimerName().GetBuffer());
            }
        }
        else
        {
            OUTPUT_WARN("The timer object is not found in the table. Ignoring request to start timer");
        }
    }
    else
    {
        OUTPUT_ERR("Invalid NULL pointer ot Timer object. Ignoring starting timer.");
    }
}


bool CETimerManager::StartSystemTimer( CETimerInfo &timerInfo )
{
    bool result = false;
    if (timerInfo.CanStartTimer())
    {
        result = CETimerManager::_startSystemTimer(timerInfo, mTimerTable);
    }
    else
    {
        OUTPUT_ERR("Ignoring firing timer because it is not in Idle state. The current state of timer [ %s ] is [ %s ]"
                    , timerInfo.GetTimer()->GetTimerName().GetBuffer()
                    , CETimerInfo::GetString(timerInfo.mTimerState));
    }
    return result;
}

bool CETimerManager::PostEvent( CEEvent& eventElem )
{
    bool result = false;
    if ( RUNTIME_CAST(&eventElem, CETimerManagingEvent) != NULL)
        result = CEEventDispatcher::PostEvent(eventElem);
    else
    {
        OUTPUT_ERR("Not a CETimerManagingEvent type event, cannot Post. Destroying event type [ %s ]", eventElem.GetRuntimeClassName());
        eventElem.Destroy();
    }
    return result;
}

bool CETimerManager::RunDispatcher( void )
{
    IESynchObject* synchObjects[] = {&mEventExit, &mEventQueue};
    CEMultiLock multiLock(synchObjects, 2, false);

    TimerThreadStarts( );
    int whichEvent  = static_cast<int>(EVENT_ERROR);
    const CEExitEvent& exitEvent = CEExitEvent::GetExitEvent();
    do 
    {
        whichEvent = multiLock.Lock(IESynchObject::WAIT_INFINITE, false, true);
        CEEvent* eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? PickEvent() : NULL;
        if ( static_cast<const CEEvent *>(eventElem) != static_cast<const CEEvent *>(&exitEvent) )
        {
            if ( whichEvent == static_cast<int>(EVENT_QUEUE) )
            {
                // proceed one external event.
                if (PrepareDispatchEvent(eventElem))
                {
                    DispatchEvent(*eventElem);
                }
                 
                PostDispatchEvent(eventElem);

                ASSERT(static_cast<CEEventQueue &>(mInternalEvents).IsEmpty());
            }
            else if (whichEvent == static_cast<int>(CEMultiLock::LOCK_INDEX_COMPLETION))
            {
                ProcessExpiredTimers();
            }
        }
        else
        {
            OUTPUT_DBG("Received exit event. Going to exit System Timer Thread!");
            whichEvent = static_cast<int>(EVENT_EXIT);
        }

    } while (whichEvent == static_cast<int>(EVENT_QUEUE) || (whichEvent == CEMultiLock::LOCK_INDEX_COMPLETION));

    RemoveAllEvents( );
    TimerThreadExits( );
    ASSERT(static_cast<CEEventQueue &>(mInternalEvents).IsEmpty());

    return (whichEvent == static_cast<int>(EVENT_EXIT));
}

void CETimerManager::TimerThreadStarts( void )
{
    CETimerManagingEvent::AddListener(static_cast<IETimerManagingEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));

    mHasStarted = true;
    mEventStarted.SetEvent();
}

void CETimerManager::TimerThreadExits( void )
{
    CETimerManagingEvent::RemoveListener(static_cast<IETimerManagingEventConsumer &>(self()), static_cast<CEDispatcherThread &>(self()));

    RemoveAllTimers( );
    mHasStarted = false;
    mEventStarted.ResetEvent();
}

bool CETimerManager::StartTimerManagerThread( void )
{
    bool result = false;
    CELock lock( mLock );

    if ( IsReady() == false )
    {
        ASSERT(IsRunning() == false);
        result = CreateThread(CEThread::WAIT_INFINITE) && WaitForDispatcherStart(IESynchObject::WAIT_INFINITE);
#ifdef _DEBUG
        if ( result == false )
        {
            OUTPUT_ERR("Failed to create [ %s ] System Timer Thread", CETimerManager::TIMER_THREAD_NAME);
        }
#endif  // _DEBUG
    }
    else
    {
        result = true;
    }
    return result;
}

void CETimerManager::StopTimerManagerThread( void )
{
    DestroyThread(CEThread::WAIT_INFINITE);
}

