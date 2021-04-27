/************************************************************************
 * \file        areg/component/private/CEEventDispatcherBase.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event Dispatcher Base class
 *
 ************************************************************************/
#include "areg/component/private/CEEventDispatcherBase.hpp"

#include "areg/component/CEEvent.hpp"
#include "areg/component/IEEventConsumer.hpp"
#include "areg/component/private/CEExitEvent.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcherBase class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcherBase class, Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
CEEventDispatcherBase::CEEventDispatcherBase(const char* name )
    : IEQueueListener  ( )
    , IEEventDispatcher( )

    , mDispatcherName   ( name != NULL ? name : "" )
    , mExternaEvents    ( static_cast<IEQueueListener &>(self()) )
    , mInternalEvents   ( )
    , mConsumerMap      ( )
    , mEventExit        ( false, false )
    , mEventQueue       ( true, false )
    , mHasStarted       ( false )
{
    ; // do nothing
}

CEEventDispatcherBase::~CEEventDispatcherBase( void )
{
    RemoveAllEvents( );
    mHasStarted = false;
}

//////////////////////////////////////////////////////////////////////////
// CEEventDispatcherBase class, methods
//////////////////////////////////////////////////////////////////////////
void CEEventDispatcherBase::SignalEvent( int eventCount )
{
    eventCount > 0 ? mEventQueue.SetEvent() : mEventQueue.ResetEvent();
}

bool CEEventDispatcherBase::StartDispatcher( void )
{
    bool result = true;
    if (mHasStarted == false)
    {
        mHasStarted = true;
        mEventExit.ResetEvent();
        result      = RunDispatcher();
    }
    return result;
}

void CEEventDispatcherBase::StopDispatcher( void )
{
    mExternaEvents.LockQueue();
    RemoveEvents(false);
    mExternaEvents.PushEvent(CEExitEvent::GetExitEvent());
    mHasStarted = false;
    mExternaEvents.UnlockQueue();
    mEventExit.SetEvent();
}

void CEEventDispatcherBase::ShutdownDispatcher( void )
{
    if ( mHasStarted )
    {
        mExternaEvents.LockQueue();
        RemoveEvents( true );
        mExternaEvents.PushEvent(CEExitEvent::GetExitEvent());
        mHasStarted = false;
        mExternaEvents.UnlockQueue();
    }
}

bool CEEventDispatcherBase::QueueEvent( CEEvent& eventElem )
{
    bool result = false;
    if ( mHasStarted )
    {
        result = true;
        CEEvent::eEventType eventType = eventElem.GetEventType();
        if ( (eventType & CEEvent::EventInternal) != 0 )
            mInternalEvents.PushEvent(eventElem);
        else if ( (eventType & CEEvent::EventExternal) != 0 )
            mExternaEvents.PushEvent(eventElem);
        else
            result = false;
    }
    return result;
}

void CEEventDispatcherBase::RemoveEvents(bool keepSpecials)
{
    mExternaEvents.LockQueue();
    mInternalEvents.RemoveEvents( false );
    mExternaEvents.RemoveEvents( keepSpecials );
    mExternaEvents.UnlockQueue();
}

void CEEventDispatcherBase::RemoveAllEvents(void)
{
    mExternaEvents.LockQueue();
    mInternalEvents.RemoveAllEvents( );
    mExternaEvents.RemoveAllEvents( );
    mExternaEvents.UnlockQueue();
}

int CEEventDispatcherBase::RemoveExternalEventType( const CERuntimeClassID & eventClassId )
{
    return mExternaEvents.RemoveEvents(eventClassId);
}

bool CEEventDispatcherBase::RegisterEventConsumer( const CERuntimeClassID& whichClass, IEEventConsumer& whichConsumer )
{
    mConsumerMap.ResourceLock();

    bool result = false;
    OUTPUT_DBG("[ %s ] dispatcher: Registers consumer [ %p ] for event [ %s ]. The dispatcher state is [ %s ]"
                , mDispatcherName.GetBuffer()
                , reinterpret_cast<void *>(&whichConsumer)
                , whichClass.GetName()
                , IsReady() ? "Ready" : "Not ready");

    CEEventConsumerList* listConsumers = mConsumerMap.FindResourceObject(whichClass);
    if (listConsumers == NULL)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Did not find consumer list for event [ %s ], creating new", mDispatcherName.GetBuffer(), whichClass.GetName());
        listConsumers   = DEBUG_NEW CEEventConsumerList();
        if (listConsumers != NULL)
            mConsumerMap.RegisterResourceObject(whichClass, listConsumers);
    }
    else
    {
        OUTPUT_DBG("[ %s ] dispatcher: Fount consumer list for event [ %s ]", mDispatcherName.GetBuffer(), whichClass.GetName());
    }

    if ( listConsumers != NULL && listConsumers->ConsumerExist(whichConsumer) == false )
    {
        OUTPUT_DBG("[ %s ] dispatcher: Add new consumer. There are [ %d ] registered consumers for event [ %s ]. There are [ %d ] Consumer Lists in map", mDispatcherName.GetBuffer(), listConsumers->GetSize(), whichClass.GetName(), mConsumerMap.GetSize());
        result = listConsumers->AddConsumer(whichConsumer);
    }

    mConsumerMap.ResourceUnlock();
    return result;
}

bool CEEventDispatcherBase::UnregisterEventConsumer( const CERuntimeClassID & whichClass, IEEventConsumer & whichConsumer )
{
    mConsumerMap.ResourceLock();

    bool result = false;
    OUTPUT_DBG("[ %s ] dispatcher: Going to unregister consumer [ %p ] for event class [ %s ]", mDispatcherName.GetBuffer(), (void *)&whichConsumer, whichClass.GetName());

    CEEventConsumerList* listConsumers = mConsumerMap.FindResourceObject(whichClass);
    if (listConsumers != NULL)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Found Consumer List for event class [ %s ], going to remove consumer. Number of consumers in list [ %d ]", mDispatcherName.GetBuffer(), whichClass.GetName(), listConsumers->GetSize());
        result = listConsumers->RemoveConsumer(whichConsumer);

        if (listConsumers->IsEmpty())
        {
            mConsumerMap.UnregisterResourceObject(whichClass);
            delete listConsumers;
            OUTPUT_DBG("[ %s ] dispatcher: Consumer List of event [ %s ] is empty, unregistered from map and deleted. Still [ %d ] lists in map", mDispatcherName.GetBuffer(), whichClass.GetName(), mConsumerMap.GetSize());
        }
    }
    else
    {
        OUTPUT_WARN("[ %s ] dispatcher: Did not find registered consumer list for event [ %s ]", mDispatcherName.GetBuffer(), whichClass.GetName());
        // AAvetyan:    The reason why it does not find, because it cleaned in _clean() function.
        //              This is mainly happening in component, which has server interface implementation.
        //              To make graceful shutdown, in _clean() method should be set filtering.
        //              But the consumer map indeed at this point is empty and the consumer is unregistered.
        mConsumerMap.UnregisterResourceObject(whichClass);
    }

    mConsumerMap.ResourceUnlock();
    return result;
}


int CEEventDispatcherBase::RemoveConsumer( IEEventConsumer & whichConsumer )
{
    mConsumerMap.ResourceLock();

    OUTPUT_DBG("[ %s ] dispatcher: Removing Consumer [ %p ] from Consumer Map", mDispatcherName.GetBuffer(), (void *)&whichConsumer);

    int result = 0;
    TELinkedList<CERuntimeClassID, const CERuntimeClassID &> removedList;
    CERuntimeClassID     Key(CERuntimeClassID::CreateEmptyClassID());
    CEEventConsumerList* Value = NULL;

    Value = mConsumerMap.GetResourceFirstKey(Key);
    while (Value != NULL)
    {
        OUTPUT_DBG("[ %s ] dispatcher: Found registered event entry [ %s ] for consumer [ %p ]", mDispatcherName.GetBuffer(), Key.GetName(), (void *)&whichConsumer);
        ASSERT(Value->IsEmpty() == false);

        result += Value->RemoveConsumer(whichConsumer) ? 1 : 0;

        if (Value->IsEmpty())
        {
            OUTPUT_WARN("[ %s ] dispatcher: The Consumer List of event entry [ %s ] is empty, marking for deleting", mDispatcherName.GetBuffer(), Key.GetName());
            removedList.AddHead(Key);
        }

        Value = mConsumerMap.GetResourceNextKey(Key);
    }

    while (removedList.IsEmpty() == false)
    {
        Key     = removedList.RemoveTail();
        Value   = mConsumerMap.UnregisterResourceObject(Key);
        ASSERT(Value != NULL);
        delete Value;
        Value = NULL;

        OUTPUT_WARN("[ %s ] dispatcher: Unregistered and deleted Consumer List of event entry [ %s ]. There are still [ %d ] Consumer Lists in map", mDispatcherName.GetBuffer(), Key.GetName(), mConsumerMap.GetSize());
    }

    mConsumerMap.ResourceUnlock();
    return result;
}

bool CEEventDispatcherBase::RunDispatcher( void )
{
    IESynchObject* syncObjects[2] = {&mEventExit, &mEventQueue};
    CEMultiLock multiLock(syncObjects, 2, false);

    int whichEvent  = static_cast<int>(EVENT_ERROR);
    const CEExitEvent& exitEvent = CEExitEvent::GetExitEvent();
    do 
    {
        whichEvent = multiLock.Lock(IESynchObject::WAIT_INFINITE, false);
        CEEvent* eventElem = whichEvent == static_cast<int>(EVENT_QUEUE) ? PickEvent() : NULL;
        if ( static_cast<const CEEvent *>(eventElem) != static_cast<const CEEvent *>(&exitEvent) )
        {
            if ( whichEvent == static_cast<int>(EVENT_QUEUE) )
            {
                if (eventElem == NULL)
                {
                    OUTPUT_WARN("The event object is NULL. Ignoring and waiting for lock.");
                    continue;
                }

                do 
                {
                    // proceed one external event.
                    if (PrepareDispatchEvent(eventElem) )
                    {
                        DispatchEvent(*eventElem);
                    }

                    PostDispatchEvent(eventElem);

                    // proceed all internal events after external.
                    // needed for notifications. For example in case of Proxy.
                    // But before popping internal event from stack, check whether
                    // there is no request to exit thread.
                    eventElem = NULL;
                    int eventLock = multiLock.Lock(IESynchObject::DO_NOT_WAIT);
                    if ( eventLock == CEMultiLock::LOCK_INDEX_TIMEOUT ||  eventLock == static_cast<int>(EVENT_QUEUE) )
                        eventElem = static_cast<CEEventQueue &>(mInternalEvents).IsEmpty() == false ? mInternalEvents.PopEvent() : NULL;

                } while (eventElem != NULL);
            }
            else
            {
                OUTPUT_WARN(">>> Going to exit [ %s ] dispatcher", static_cast<const char *>(mDispatcherName.GetBuffer()));
            }
        }
        else
        {
            whichEvent = static_cast<int>(EVENT_EXIT);
            OUTPUT_WARN("Received exit event. Going to exit [ %s ] dispatcher", static_cast<const char *>(mDispatcherName.GetBuffer()));
        }
    } while (whichEvent == static_cast<int>(EVENT_QUEUE));
    mHasStarted = false;
    RemoveAllEvents( );
    _clean();

    OUTPUT_WARN("The Dispatcher [ %s ] completed job and stopping running.", mDispatcherName.GetBuffer());

    return (whichEvent == static_cast<int>(EVENT_EXIT));
}

CEEvent* CEEventDispatcherBase::PickEvent( void )
{
    return mExternaEvents.PopEvent();
}
bool CEEventDispatcherBase::PrepareDispatchEvent( CEEvent* eventElem )
{
    return (eventElem != NULL);
}

void CEEventDispatcherBase::PostDispatchEvent( CEEvent* eventElem )
{
    if (eventElem != NULL)
        eventElem->Destroy();
}

bool CEEventDispatcherBase::DispatchEvent( CEEvent& eventElem )
{
    CEEventConsumerList processingList;
    IEEventConsumer* consumer = eventElem.GetEventConsumer();
    if ( consumer != NULL)
    {
        processingList.AddHead(consumer);
    }
    else
    {
        // Lock resource, before get any information
        mConsumerMap.ResourceLock();

        CEEventConsumerList* listConsumers = mConsumerMap.FindResourceObject(eventElem.GetRuntimeClassId());
        if (listConsumers != NULL)
            processingList = *listConsumers;

        // Unlock resource as soon as possible. Otherwise we may block other threads
        // on registering / unregistering consumer.
        mConsumerMap.ResourceUnlock();
    }

    // OUTPUT_DBG("Processing Event [ %s ] in dispatcher [ %s ] for [ %d ] consumers", eventElem.GetRuntimeClassName(), mDispatcherName.GetBuffer(), processingList.GetSize());
    LISTPOS pos = processingList.GetHeadPosition();
    while (pos != NULL)
    {
        consumer = processingList.GetNext(pos);
        eventElem.DispatchSelf(consumer);
        consumer = NULL;
    }

    return (processingList.IsEmpty() == false);
}

bool CEEventDispatcherBase::HasRegisteredConsumer( const CERuntimeClassID& whichClass ) const
{
    return mConsumerMap.ResourceKeyExist(whichClass);
}

bool CEEventDispatcherBase::IsReady( void ) const
{
    return mHasStarted;
}

void CEEventDispatcherBase::_clean()
{
    mConsumerMap.ResourceLock();

    OUTPUT_WARN("[ %s ] dispatcher: Cleaning up, there are [ %d ] registered consumer maps", mDispatcherName.GetBuffer(), mConsumerMap.GetSize());
    CERuntimeClassID     Key(CERuntimeClassID::CreateEmptyClassID());
    while (mConsumerMap.IsEmpty() == false)
    {
        mConsumerMap.GetResourceFirstKey(Key);
        CEEventConsumerList* Value =  mConsumerMap.UnregisterResourceObject(Key);
        Value->RemoveAllConsumers();
        delete Value;

        OUTPUT_WARN("[ %s ] dispatcher: Unregistered and deleted Consumer List of event entry [ %s ]. There are still [ %d ] Consumer Lists in map", mDispatcherName.GetBuffer(), Key.GetName(), mConsumerMap.GetSize());
    }
    mConsumerMap.ResourceUnlock();
}

bool CEEventDispatcherBase::PulseExit(void)
{
    return mEventExit.SetEvent();
}
