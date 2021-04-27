/************************************************************************
 * \file        areg/component/private/CEDispatcherThread.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher class
 *
 ************************************************************************/
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/CEComponentThread.hpp"
#include "areg/component/CEEvent.hpp"
#include "areg/component/private/CEExitEvent.hpp"
#include "areg/trace/GETrace.h"

DEF_TRACE_SCOPE( areg_component_private_CEDispatcherThread_DestroyThread);
DEF_TRACE_SCOPE( areg_component_private_CEDispatcherThread_ShutdownThread);
DEF_TRACE_SCOPE( areg_component_private_CEDispatcherThread_RunDispatcher);
DEF_TRACE_SCOPE( areg_component_private_CEDispatcherThread_SetExitEvent);

//////////////////////////////////////////////////////////////////////////
// class CENullDispatcherThread declarations
//////////////////////////////////////////////////////////////////////////
/************************************************************************
 * \brief   NullDispatcherThread is hidden and used only as a dummy thread
 *          to get invalid dispatcher. It will ignore all required operations.
 ************************************************************************/
class AREG_API CENullDispatcherThread    : public CEComponentThread
{
//////////////////////////////////////////////////////////////////////////
// Friend classes
//////////////////////////////////////////////////////////////////////////
    friend class CEDispatcherThread;
    friend class CEComponentThread;
    friend class CEComponent;

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
    static CENullDispatcherThread   sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Declare Runtime
//////////////////////////////////////////////////////////////////////////
    /**
     * \brief   Declare Runtime to be able to cast and validate thread.
     **/
    DECLARE_RUNTIME(CENullDispatcherThread)
//////////////////////////////////////////////////////////////////////////
// Constructor. Hidden. The object cannot be instantiated.
//////////////////////////////////////////////////////////////////////////
private:
    CENullDispatcherThread( void );

//////////////////////////////////////////////////////////////////////////
// Overrides to disable base functionalities.
//////////////////////////////////////////////////////////////////////////
protected:
    //////////////////////////////////////////////////////////////////////////
    // Disable consumer registration functionalities
    //////////////////////////////////////////////////////////////////////////
    virtual bool RegisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer );
    virtual bool UnregisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer );
    virtual int  RemoveConsumer( IEEventConsumer & whichConsumer );
    virtual bool HasRegisteredConsumer( const CERuntimeClassID & whichClass ) const;

    //////////////////////////////////////////////////////////////////////////
    // Disable post event
    //////////////////////////////////////////////////////////////////////////
    virtual bool PostEvent( CEEvent & eventElem );

    //////////////////////////////////////////////////////////////////////////
    // Disable running function and return error on exit.
    //////////////////////////////////////////////////////////////////////////
    virtual bool ThreadRegistered( CEThread * threadObj );
    virtual void ThreadRuns( void );
    virtual int ThreadExit( void );

    //////////////////////////////////////////////////////////////////////////
    // Disable Thread locking
    //////////////////////////////////////////////////////////////////////////
    virtual bool WaitForDispatcherStart( unsigned int waitTimeout = IESynchObject::WAIT_INFINITE );
};

//////////////////////////////////////////////////////////////////////////
// CENullDispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CENullDispatcherThread class statics
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Predefined name of invalid Null Dispatcher Thread
 **/
const char* const        CENullDispatcherThread::sNullDispatcherName        = "_NullDispatcherThread_";
/**
 * \brief   singleton invalid Null Dispatcher Thread.
 **/
CENullDispatcherThread   CENullDispatcherThread::sSelfNullDispatcher;

//////////////////////////////////////////////////////////////////////////
// Runtime information to identify NullDispatcher object
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CENullDispatcherThread, CEComponentThread)

//////////////////////////////////////////////////////////////////////////
// CENullDispatcherThread class Constructor 
//////////////////////////////////////////////////////////////////////////
inline CENullDispatcherThread::CENullDispatcherThread( void )
    : CEComponentThread(CENullDispatcherThread::sNullDispatcherName)
{   ;               }

//////////////////////////////////////////////////////////////////////////
// CENullDispatcherThread class. Disable basic functionalities.
//////////////////////////////////////////////////////////////////////////
bool CENullDispatcherThread::RegisterEventConsumer( const CERuntimeClassID &    /* whichClass   */
                                                  , IEEventConsumer &           /* whichConsumer*/ )
{   return false;    }

bool CENullDispatcherThread::UnregisterEventConsumer( const CERuntimeClassID &  /* whichClass   */
                                                    , IEEventConsumer &         /* whichConsumer*/ )
{   return false;    }

int CENullDispatcherThread::RemoveConsumer( IEEventConsumer & /* whichConsumer*/ )
{   return 0;        }

bool CENullDispatcherThread::HasRegisteredConsumer( const CERuntimeClassID & /* whichClass */ ) const
{   return false;    }

bool CENullDispatcherThread::PostEvent( CEEvent& eventElem )
{
    OUTPUT_ERR("Wrong event dispatcher to post event type [ %s ], going to destroy", eventElem.GetRuntimeClassName());
    eventElem.Destroy();
    ASSERT(false);
    return false;
}

bool CENullDispatcherThread::ThreadRegistered( CEThread * /* threadObj */)
{   ASSERT(false); return false;    }

void CENullDispatcherThread::ThreadRuns( void )
{   ASSERT(false);                  }

int CENullDispatcherThread::ThreadExit( void )
{   ASSERT(false);  return IEThreadConsumer::EXIT_ERROR;   }

bool CENullDispatcherThread::WaitForDispatcherStart( unsigned int /* waitTimeout */ /*= IESynchObject::WAIT_INFINITE */ )
{
    return false;
}

//////////////////////////////////////////////////////////////////////////
// class CENullDispatcherThread END
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME(CEDispatcherThread, CEThread)

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class statics
//////////////////////////////////////////////////////////////////////////

CEDispatcherThread & CEDispatcherThread::_getNullDispatherThread( void )
{
    return static_cast<CEDispatcherThread &>(CENullDispatcherThread::sSelfNullDispatcher);
}

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class Constructor / Destructor.
//////////////////////////////////////////////////////////////////////////
CEDispatcherThread::CEDispatcherThread (const char* threadName )
    : CEThread          ( static_cast<IEThreadConsumer &>(self()), threadName )
    , CEEventDispatcher ( threadName )

    , mEventStarted     ( true, false )
{
    ; // do nothing
}

CEDispatcherThread::~CEDispatcherThread( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CEDispatcherThread class Methods
//////////////////////////////////////////////////////////////////////////
bool CEDispatcherThread::PostEvent( CEEvent& eventElem )
{
    OUTPUT_ERR("Wrong PostEvent function call, destroying event [ %s ]", eventElem.GetRuntimeClassName());
    eventElem.Destroy();
    ASSERT(false);  // <= this should not be called.
    return true;
}

CEThread::eCompletionStatus CEDispatcherThread::DestroyThread( unsigned int waitForStopMs /*= CEThread::DO_NOT_WAIT*/ )
{
    TRACE_SCOPE( areg_component_private_CEDispatcherThread_DestroyThread);
    TRACE_DBG("Destroying the thread the thread [ %s ] with ID [ %p ]. The current state is [ %s ]"
                , GetThreadName().String( )
                , GetThreadId( )
                , IsRunning() ? "RUNNING" : "NOT RUNNING" );

    StopDispatcher( );
    CEThread::eCompletionStatus result = CEThread::DestroyThread(waitForStopMs);
    RemoveAllEvents( );
    return result;
}

CEDispatcherThread* CEDispatcherThread::GetEventConsumerThread( const CERuntimeClassID& whichClass )
{
    return (HasRegisteredConsumer(whichClass) ? this : NULL);
}

void CEDispatcherThread::ShutdownThread( void )
{
    TRACE_SCOPE( areg_component_private_CEDispatcherThread_ShutdownThread);
    TRACE_DBG("Shutting down the thread [ %s ] with ID [ %p ]."
                , GetThreadName().String( )
                , GetThreadId( ));

    ShutdownDispatcher();
}

bool CEDispatcherThread::RunDispatcher( void )
{
    TRACE_SCOPE( areg_component_private_CEDispatcherThread_RunDispatcher);

    mEventStarted.SetEvent();
    bool result = CEEventDispatcher::RunDispatcher();
    mEventStarted.ResetEvent();

    TRACE_DBG("The dispatcher [ %s ] with ID [ %p ] is stopping."
                , GetThreadName().String( )
                , GetThreadId( ));
    return result;
}

bool CEDispatcherThread::WaitForDispatcherStart( unsigned int waitTimeout /*= IESynchObject::WAIT_INFINITE */ )
{
    return mEventStarted.Lock(waitTimeout);
}

void CEDispatcherThread::SetExitEvent( void )
{
    TRACE_SCOPE( areg_component_private_CEDispatcherThread_SetExitEvent);
    TRACE_DBG("Requesting to exit thread [ %s ] with ID [ %p ] and status [ %s ]."
                , GetThreadName().String( )
                , GetThreadId( )
                , mHasStarted ? "STARTED" : "NOT STARTED");

    if ( mHasStarted )
    {
        mExternaEvents.PushEvent(CEExitEvent::GetExitEvent());
    }
}

bool CEDispatcherThread::IsExitEvent(const CEEvent * checkEvent) const
{
    return ( checkEvent == static_cast<const CEEvent *>(&CEExitEvent::GetExitEvent()) );
}
