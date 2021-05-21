/************************************************************************
 * \file        areg/component/private/DispatcherThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
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
DEF_TRACE_SCOPE( areg_component_private_DispatcherThread_runDispatcher);
DEF_TRACE_SCOPE( areg_component_private_DispatcherThread_triggerExitEvent);

//////////////////////////////////////////////////////////////////////////
// class NullDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   NullDispatcherThread is hidden and used only as a dummy thread
 *          to get invalid dispatcher. It will ignore all required operations.
 ************************************************************************/
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
     * \brief   Predefined fixed name of invalid Null Dispatcher Thread.
     **/
    static const char* const        sNullDispatcherName     /*= "_NullDispatcherThread_"*/;
    /**
     * \brief   singleton Null Dispatcher Thread. It is not valid and runnable thread.
     **/
    static NullDispatcherThread   sSelfNullDispatcher;

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

//////////////////////////////////////////////////////////////////////////
// Overrides to disable base functionalities.
//////////////////////////////////////////////////////////////////////////
protected:
    //////////////////////////////////////////////////////////////////////////
    // Disable consumer registration functionalities
    //////////////////////////////////////////////////////////////////////////
    virtual bool registerEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer );
    virtual bool unregisterEventConsumer( const RuntimeClassID & whichClass, IEEventConsumer & whichConsumer );
    virtual int  removeConsumer( IEEventConsumer & whichConsumer );
    virtual bool hasRegisteredConsumer( const RuntimeClassID & whichClass ) const;

    //////////////////////////////////////////////////////////////////////////
    // Disable post event
    //////////////////////////////////////////////////////////////////////////
    virtual bool postEvent( Event & eventElem );

    //////////////////////////////////////////////////////////////////////////
    // Disable running function and return error on exit.
    //////////////////////////////////////////////////////////////////////////
    virtual bool onThreadRegistered( Thread * threadObj );
    virtual void onThreadRuns( void );
    virtual int onThreadExit( void );

    //////////////////////////////////////////////////////////////////////////
    // Disable Thread locking
    //////////////////////////////////////////////////////////////////////////
    virtual bool waitForDispatcherStart( unsigned int waitTimeout = IESynchObject::WAIT_INFINITE );
};

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined name of invalid Null Dispatcher Thread
 **/
const char* const        NullDispatcherThread::sNullDispatcherName        = "_NullDispatcherThread_";
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
    : ComponentThread(NullDispatcherThread::sNullDispatcherName)
{   ;               }

//////////////////////////////////////////////////////////////////////////
// NullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool NullDispatcherThread::registerEventConsumer( const RuntimeClassID &    /* whichClass   */
                                                  , IEEventConsumer &           /* whichConsumer*/ )
{   return false;    }

bool NullDispatcherThread::unregisterEventConsumer( const RuntimeClassID &  /* whichClass   */
                                                    , IEEventConsumer &         /* whichConsumer*/ )
{   return false;    }

int NullDispatcherThread::removeConsumer( IEEventConsumer & /* whichConsumer*/ )
{   return 0;        }

bool NullDispatcherThread::hasRegisteredConsumer( const RuntimeClassID & /* whichClass */ ) const
{   return false;    }

bool NullDispatcherThread::postEvent( Event& eventElem )
{
    OUTPUT_ERR("Wrong event dispatcher to post event type [ %s ], going to destroy", eventElem.getRuntimeClassName());
    eventElem.destroy();
    ASSERT(false);
    return false;
}

bool NullDispatcherThread::onThreadRegistered( Thread * /* threadObj */)
{   ASSERT(false); return false;    }

void NullDispatcherThread::onThreadRuns( void )
{   ASSERT(false);                  }

int NullDispatcherThread::onThreadExit( void )
{   ASSERT(false);  return IEThreadConsumer::EXIT_ERROR;   }

bool NullDispatcherThread::waitForDispatcherStart( unsigned int /* waitTimeout */ /*= IESynchObject::WAIT_INFINITE */ )
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
DispatcherThread::DispatcherThread (const char* threadName )
    : Thread          ( static_cast<IEThreadConsumer &>(self()), threadName )
    , EventDispatcher ( threadName )

    , mEventStarted     ( true, false )
{
    ; // do nothing
}

DispatcherThread::~DispatcherThread( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// DispatcherThread class Methods
//////////////////////////////////////////////////////////////////////////
bool DispatcherThread::postEvent( Event& eventElem )
{
    OUTPUT_ERR("Wrong postEvent function call, destroying event [ %s ]", eventElem.getRuntimeClassName());
    eventElem.destroy();
    ASSERT(false);  // <= this should not be called.
    return true;
}

Thread::eCompletionStatus DispatcherThread::destroyThread( unsigned int waitForStopMs /*= Thread::DO_NOT_WAIT*/ )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_destroyThread);
    TRACE_DBG("Destroying the thread the thread [ %s ] with ID [ %p ]. The current state is [ %s ]"
                , getName().getString( )
                , getId( )
                , isRunning() ? "RUNNING" : "NOT RUNNING" );

    stopDispatcher( );
    Thread::eCompletionStatus result = Thread::destroyThread(waitForStopMs);
    removeAllEvents( );
    return result;
}

DispatcherThread* DispatcherThread::getEventConsumerThread( const RuntimeClassID& whichClass )
{
    return (hasRegisteredConsumer(whichClass) ? this : NULL);
}

void DispatcherThread::shutdownThread( void )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_shutdownThread);
    TRACE_DBG("Shutting down the thread [ %s ] with ID [ %p ]."
                , getName().getString( )
                , getId( ));

    shutdownDispatcher();
}

bool DispatcherThread::runDispatcher( void )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_runDispatcher);

    mEventStarted.setEvent();
    bool result = EventDispatcher::runDispatcher();
    mEventStarted.resetEvent();

    TRACE_DBG("The dispatcher [ %s ] with ID [ %p ] is stopping.", getName().getString( ), getId( ));

    return result;
}

bool DispatcherThread::waitForDispatcherStart( unsigned int waitTimeout /*= IESynchObject::WAIT_INFINITE */ )
{
    return mEventStarted.lock(waitTimeout);
}

void DispatcherThread::triggerExitEvent( void )
{
    TRACE_SCOPE( areg_component_private_DispatcherThread_triggerExitEvent);
    TRACE_DBG("Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
                , getName().getString( )
                , getId( )
                , mHasStarted ? "STARTED" : "NOT STARTED");

    if ( mHasStarted )
    {
        mExternaEvents.pushEvent(ExitEvent::getExitEvent());
    }
}

bool DispatcherThread::isExitEvent(const Event * checkEvent) const
{
    return ( checkEvent == static_cast<const Event *>(&ExitEvent::getExitEvent()) );
}
