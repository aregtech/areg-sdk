/************************************************************************
 * \file        areg/component/private/CEEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Base class implementation
 *
 ************************************************************************/
#include "areg/component/CEEvent.hpp"

#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/IEEventConsumer.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEvent class declaration
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEvent class, Runtime implementation
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CEEvent, CERuntimeObject)

//////////////////////////////////////////////////////////////////////////
// CEEvent class, static members
//////////////////////////////////////////////////////////////////////////

/**
 * \brief   Predefined Bad Event object
 **/
const CEEvent CEEvent::BAD_EVENT(CEEvent::EventUnknown);

bool CEEvent::AddListener( const CERuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread )
{
    return CEEvent::AddListener(classId, eventConsumer, CEDispatcherThread::GetDispatcherThread(whichThread));
}

bool CEEvent::AddListener( const CERuntimeClassID & classId, IEEventConsumer & eventConsumer, CEDispatcherThread & dispThread )
{
    ASSERT( dispThread.IsValid() );
    return dispThread.RegisterEventConsumer(classId, eventConsumer);
}

bool CEEvent::RemoveListener( const CERuntimeClassID & classId, IEEventConsumer & eventConsumer, const char* whichThread )
{
    return CEEvent::RemoveListener(classId, eventConsumer, CEDispatcherThread::GetDispatcherThread(whichThread));
}

bool CEEvent::RemoveListener( const CERuntimeClassID & classId, IEEventConsumer & eventConsumer, CEDispatcherThread & dispThread )
{
    return dispThread.IsRunning() ? dispThread.UnregisterEventConsumer(classId, eventConsumer) : false;
}

//////////////////////////////////////////////////////////////////////////
// CEEvent class, Constructors / Destructor
//////////////////////////////////////////////////////////////////////////

CEEvent::CEEvent( void )
    : CERuntimeObject   ( )
    , mEventType        ( CEEvent::EventUnknown )
    , mConsumer         ( NULL )
    , mTargetThread     ( NULL )
{
    ; // do nothing
}

CEEvent::CEEvent( CEEvent::eEventType eventType )
    : CERuntimeObject   ( )
    , mEventType        ( eventType )
    , mConsumer         ( NULL )
    , mTargetThread     ( NULL )
{
    ; // do nothing
}

CEEvent::~CEEvent( void )
{
    mConsumer       = NULL;
    mTargetThread   = NULL;
}

//////////////////////////////////////////////////////////////////////////
// CEEvent class, methods
//////////////////////////////////////////////////////////////////////////

void CEEvent::Destroy( void )
{
    delete this;
}

CEEventDispatcher& CEEvent::GetDispatcher( void ) const
{
    return (mTargetThread != NULL ? mTargetThread->GetEventDispatcher() : CEDispatcherThread::GetCurrentDispatcher());
}

void CEEvent::SendEvent( void )
{
    if (mTargetThread != NULL)
    {
        mTargetThread->GetEventDispatcher().PostEvent(*this);
    }
    else
    {
        OUTPUT_ERR("The event target is unknown! Event type [ %s ] is going to be deleted.", GetRuntimeClassName());
        Destroy();
    }
}

bool CEEvent::RegisterForThread( ITEM_ID whichThread /*= 0*/ )
{
    return RegisterForThread(whichThread != 0 ? RUNTIME_CAST(CEThread::FindThreadById(whichThread), CEDispatcherThread) : 
                                                RUNTIME_CAST(CEThread::GetCurrentThread(), CEDispatcherThread));
}

bool CEEvent::RegisterForThread( const char* whichThread )
{
    return RegisterForThread(whichThread != NULL ? RUNTIME_CAST(CEThread::FindThreadByName(whichThread), CEDispatcherThread) : NULL);
}

bool CEEvent::RegisterForThread( CEDispatcherThread * dispatchThread )
{
    ASSERT((dispatchThread == NULL) || dispatchThread->IsValid());
    mTargetThread = (dispatchThread != NULL) && dispatchThread->IsReady() ? dispatchThread : NULL;
    return (mTargetThread != NULL);
}

bool CEEvent::IsEventRegistered( void ) const
{
    return GetDispatcher().HasRegisteredConsumer(GetRuntimeClassId());
}

bool CEEvent::AddEventListener( IEEventConsumer& eventConsumer )
{
    return GetDispatcher().RegisterEventConsumer(GetRuntimeClassId(), eventConsumer);
}

bool CEEvent::RemoveEventListener( IEEventConsumer& eventConsumer )
{
    return GetDispatcher().UnregisterEventConsumer(GetRuntimeClassId(), eventConsumer);
}

void CEEvent::DispatchSelf( IEEventConsumer* consumer )
{
    consumer = consumer != NULL ? consumer : this->mConsumer;
    if (consumer != NULL)
    {
        if ( consumer->PreprocessEvent(self()) )
        {
            consumer->StartEventProcessing(self());
        }
        else
        {
            OUTPUT_WARN("The Event [ %s ] is interrupted and not going to be processed", GetRuntimeClassName());
        }
    }
    else
    {
        OUTPUT_ERR("The Event [ %s ] has invalid consumer. The event cannot be processed!", GetRuntimeClassName());
    }
}
