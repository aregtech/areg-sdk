/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/DispatcherThread.cpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       AREG Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/DispatcherThread.hpp"

#include "areg/component/ComponentThread.hpp"
#include "areg/component/Event.hpp"
#include "areg/component/private/ExitEvent.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( areg_component_private_DispatcherThread_destroyThread);
DEF_TRACE_SCOPE( areg_component_private_DispatcherThread_shutdownThread);
DEF_TRACE_SCOPE( areg_component_private_DispatcherThread_triggerExit);

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   NullDispatcherThread is hidden and used only as a dummy thread
 *          to get invalid dispatcher. It will ignore all required operations.
 ************************************************************************/

namespace
{
    /**
     * \brief   Predefined fixed name of invalid Null Dispatcher Thread.
     **/
    static constexpr std::string_view   NullDispatcherName{ "_NullDispatcherThread_" };
}

class AREG_API NullDispatcherThread    : public ComponentThread
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class DispatcherThread;
    friend class ComponentThread;
    friend class Component;

//////////////////////////////////////////////////////////////////////////
// Static members
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   singleton Null Dispatcher Thread. It is not valid and runnable thread.
     **/
    static NullDispatcherThread     sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime to be able to cast and validate thread.
     **/
    DECLARE_RUNTIME(NullDispatcherThread)
//////////////////////////////////////////////////////////////////////////
// Constructor. Hidden. The object cannot be instantiated.
//////////////////////////////////////////////////////////////////////////
private:
    NullDispatcherThread( void );
    virtual ~NullDispatcherThread( void ) = default;
//////////////////////////////////////////////////////////////////////////
// Overrides to disable base functionalities.
//////////////////////////////////////////////////////////////////////////
protected:
    //////////////////////////////////////////////////////////////////////////
    // Disable consumer registration functionalities
    //////////////////////////////////////////////////////////////////////////
    virtual bool registerEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) override;
    virtual bool unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer ) override;
    virtual int  removeConsumer( IEEventConsumer & whichConsumer ) override;
    virtual bool hasRegisteredConsumer( const RuntimeClassID & whichClass ) const override;

    //////////////////////////////////////////////////////////////////////////
    // Disable post event
    //////////////////////////////////////////////////////////////////////////
    virtual bool postEvent( Event & eventElem ) override;

    //////////////////////////////////////////////////////////////////////////
    // Disable running function and return error on exit.
    //////////////////////////////////////////////////////////////////////////
    virtual bool onThreadRegistered( Thread * threadObj ) override;
    virtual void onThreadRuns( void ) override;
    virtual int onThreadExit( void ) override;

    //////////////////////////////////////////////////////////////////////////
    // Disable Thread locking
    //////////////////////////////////////////////////////////////////////////
    virtual bool waitForDispatcherStart( unsigned int waitTimeout = NECommon::WAIT_INFINITE ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( NullDispatcherThread );
};

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   singleton invalid Null Dispatcher Thread.
 **/
NullDispatcherThread   NullDispatcherThread::sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Runtime information to identify NullDispatcher object
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(NullDispatcherThread, ComponentThread)

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class Constructor 
//////////////////////////////////////////////////////////////////////////
inline NullDispatcherThread::NullDispatcherThread( void )
    : ComponentThread(NullDispatcherName)
{                   }

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool NullDispatcherThread::registerEventConsumer( const RuntimeClassID & /* whichClass*/, IEEventConsumer & /*whichConsumer*/ )
{   return false;   }

bool NullDispatcherThread::unregisterEventConsumer( const RuntimeClassID & /*whichClass*/, IEEventConsumer & /* whichConsumer*/ )
{   return false;   }

int NullDispatcherThread::removeConsumer( IEEventConsumer & /* whichConsumer*/ )
{   return 0;       }

bool NullDispatcherThread::hasRegisteredConsumer( const RuntimeClassID & /* whichClass */ ) const
{   return false;   }

bool NullDispatcherThread::postEvent( Event& eventElem )
{
    eventElem.destroy();
    ASSERT(false);
    return false;
}

bool NullDispatcherThread::onThreadRegistered( Thread * /* threadObj */)
{
    ASSERT(false);
    return false;
}

void NullDispatcherThread::onThreadRuns( void )
{
    ASSERT(false);
}

int NullDispatcherThread::onThreadExit( void )
{
    ASSERT(false);
    return static_cast<int>(IEThreadConsumer::eExitCodes::ExitError);
}

bool NullDispatcherThread::waitForDispatcherStart( unsigned int /* waitTimeout */ /*= NECommon::WAIT_INFINITE */ )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread END
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(DispatcherThread, Thread)

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class statics
//////////////////////////////////////////////////////////////////////////

DispatcherThread & DispatcherThread::_getNullDispatherThread( void )
{
    return static_cast<DispatcherThread &>(NullDispatcherThread::sSelfNullDispatcher);
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
DispatcherThread::DispatcherThread (const String & threadName )
    : Thread          ( static_cast<IEThreadConsumer &>(self()), threadName )
    , EventDispatcher ( threadName )

    , mEventStarted     ( true, false )
{
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Methods
//////////////////////////////////////////////////////////////////////////
bool DispatcherThread::postEvent( Event& eventElem )
{
    eventElem.destroy();
    ASSERT(false);  // <= this should not be called.
    // You may want to call EventDispatcher::postEvent() and/or filter events here

    return true;
}

void DispatcherThread::triggerExit( void )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_triggerExit );
    TRACE_DBG( "Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
               , getName( ).getString( )
               , static_cast<id_type>(getId( ))
               , mHasStarted ? "STARTED" : "NOT STARTED" );

    mExternaEvents.lockQueue( );
    if ( mHasStarted )
    {
        removeEvents( true );
        mExternaEvents.pushEvent( ExitEvent::getExitEvent( ) );
    }

    mEventExit.setEvent( );
    mExternaEvents.unlockQueue( );
}

Thread::eCompletionStatus DispatcherThread::shutdownThread( unsigned int waitForStopMs /*= NECommon::DO_NOT_WAIT*/ )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_destroyThread);
    TRACE_DBG("Shutting down the thread [ %s ] with ID [ %p ]. The current state is [ %s ]"
                , getName().getString( )
                , static_cast<id_type>(getId( ))
                , isRunning() ? "RUNNING" : "NOT RUNNING" );

    stopDispatcher( );
    Thread::eCompletionStatus result = Thread::shutdownThread(waitForStopMs);
    removeAllEvents( );
    return result;
}

DispatcherThread* DispatcherThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    return (hasRegisteredConsumer(whichClass) ? this : nullptr);
}

void DispatcherThread::readyForEvents( bool isReady )
{
    if ( isReady )
    {
        EventDispatcher::readyForEvents( true );
        mEventStarted.setEvent( );
    }
    else
    {
        mEventStarted.resetEvent( );
        EventDispatcher::readyForEvents( false );
    }
}

bool DispatcherThread::waitForDispatcherStart( unsigned int waitTimeout /*= NECommon::WAIT_INFINITE */ )
{
    return mEventStarted.lock(waitTimeout);
}

bool DispatcherThread::isExitEvent(const Event * checkEvent) const
{
    return ( checkEvent == static_cast<const Event *>(&ExitEvent::getExitEvent()) );
}

DispatcherThread * DispatcherThread::findEventConsumerThread( const RuntimeClassID & whichClass )
{
    DispatcherThread * result = nullptr;
    Thread* dispThread = RUNTIME_CAST(Thread::getCurrentThread(), DispatcherThread);
    result = dispThread != nullptr ? static_cast<DispatcherThread *>(dispThread)->getEventConsumerThread(whichClass) : nullptr;

    id_type threadId = Thread::INVALID_THREAD_ID;
    dispThread = Thread::getFirstThread(threadId);
    while ((result == nullptr) && (dispThread != nullptr))
    {
        dispThread = dispThread != nullptr ? RUNTIME_CAST(dispThread, DispatcherThread) : nullptr;
        result = dispThread != nullptr ? static_cast<DispatcherThread *>(dispThread)->getEventConsumerThread(whichClass) : nullptr;

        dispThread = Thread::getNextThread(threadId);
    }

    return result;
}
