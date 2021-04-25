/************************************************************************
 * \file        areg/src/component/private/CEEventQueue.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Event queue class
 *
 ************************************************************************/
#include "areg/src/component/private/CEEventQueue.hpp"

#include "areg/src/component/CEEvent.hpp"
#include "areg/src/component/private/CEExitEvent.hpp"
#include "areg/src/component/CEServiceResponseEvent.hpp"
#include "areg/src/component/private/IEQueueListener.hpp"

#include "areg/src/base/CERuntimeClassID.hpp"

//////////////////////////////////////////////////////////////////////////
// CEEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CEEventQueue class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CEEventQueue::CEEventQueue( IEQueueListener & eventListener, TEStack<CEEvent *, CEEvent *> & eventQueue )
    : mEventListener(eventListener)
    , mEventQueue   (eventQueue)
{
    ; // do nothing
}

CEEventQueue::~CEEventQueue( void )
{
    ; // the event queue should be emptied in derived objects
}

//////////////////////////////////////////////////////////////////////////
// CEEventQueue class, methods
//////////////////////////////////////////////////////////////////////////
void CEEventQueue::PushEvent( CEEvent& evendElem )
{
    mEventListener.SignalEvent( mEventQueue.PushElement(&evendElem) );
}

CEEvent* CEEventQueue::PopEvent( void )
{
    mEventQueue.LockStack();
    CEEvent* result = mEventQueue.IsEmpty() == false ? mEventQueue.PopElement() : NULL;
    if (mEventQueue.IsEmpty())
    {
        mEventListener.SignalEvent(0);
    }

    mEventQueue.UnlockStack();

    return result;
}

void CEEventQueue::RemoveAllEvents(void)
{
    mEventQueue.LockStack();
    CEEvent* exitEvent  = static_cast<CEEvent *>(&CEExitEvent::GetExitEvent());
    while ( mEventQueue.IsEmpty() == false )
    {
        CEEvent * eventElem = mEventQueue.PopElement();
        if ( eventElem != NULL && eventElem != exitEvent )
            eventElem->Destroy();
    }
    mEventListener.SignalEvent( 0 );
    mEventQueue.UnlockStack();
}

void CEEventQueue::RemoveEvents( bool keepSpecials /*= false*/ )
{
    mEventQueue.LockStack();
    bool hasEvents   = removePendingEvents( keepSpecials );
    mEventListener.SignalEvent(hasEvents ? 1 : 0);
    mEventQueue.UnlockStack();
}

int CEEventQueue::RemoveEvents( const CERuntimeClassID & eventClassId )
{
    int removedCount = 0;

    if ( eventClassId != CEExitEvent::GetExitEvent().GetRuntimeClassId() )
    {
        mEventQueue.LockStack();
        removedCount = mEventQueue.GetSize();

        TENolockStack<CEEvent *, CEEvent *> tempQueue;        
        while ( mEventQueue.IsEmpty() == false )
        {
            CEEvent * elemEvent = mEventQueue.PopElement();
            if ( elemEvent != NULL )
            {
                if ( elemEvent->GetRuntimeClassId() == eventClassId )
                    elemEvent->Destroy();
                else
                    tempQueue.PushElement(elemEvent);
            }
        }
        removedCount -= tempQueue.GetSize();

        while (tempQueue.IsEmpty() == false )
            mEventQueue.PushElement( tempQueue.PopElement() );

        mEventListener.SignalEvent(mEventQueue.GetSize());
        mEventQueue.UnlockStack();
    }

    return removedCount;
}

bool CEEventQueue::removePendingEvents( bool keepSpecials )
{
    mEventQueue.LockStack();

    TENolockStack<CEEvent *, CEEvent *> specials;
    CEEvent* exitEvent  = static_cast<CEEvent *>(&CEExitEvent::GetExitEvent());

    while (mEventQueue.IsEmpty() == false)
    {
        CEEvent * eventElem = mEventQueue.PopElement();
        if (eventElem != NULL)
        {
            ASSERT(exitEvent != NULL);
            if (eventElem == exitEvent)
            {
                // OUTPUT_DBG("Skip removing exit event!");
                specials.PushElement(eventElem);
                eventElem = NULL;
            }
            else if ( keepSpecials )
            {
                CEServiceResponseEvent* respEvent = RUNTIME_CAST(eventElem, CEServiceResponseEvent);
                if ( respEvent != NULL)
                {
                    unsigned int respId = respEvent->GetResponseId();
                    if ( NEService::IsConnectNotifyId(respId) )
                    {
                        OUTPUT_DBG("Keep response event with response ID NEService::SI_NOTIFY_CONNECT for target proxy [ %s ]", CEProxyAddress::ConvertAddressToPath(respEvent->GetTargetProxy()).GetBuffer());
                        specials.PushElement(eventElem);
                        eventElem = NULL;
                    }
                    else
                    {
                        ; // do nothing, should be removed!
                    }
                }
            }
        }

        if (eventElem != NULL)
            eventElem->Destroy();
    }

    while (specials.IsEmpty() == false )
        mEventQueue.PushElement( specials.PopElement() );

    mEventQueue.UnlockStack();

    return (mEventQueue.IsEmpty() == false);
}

//////////////////////////////////////////////////////////////////////////
// CEExternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

CEExternalEventQueue::CEExternalEventQueue( IEQueueListener & eventListener )
    : CEEventQueue  ( eventListener, static_cast<TEStack<CEEvent *, CEEvent *> &>(self()) )
{
    ; // do nothing
}

CEExternalEventQueue::~CEExternalEventQueue(void)
{
    removePendingEvents( false );
    RemoveAll();
}

//////////////////////////////////////////////////////////////////////////
// CEInternalEventQueue class implementation
//////////////////////////////////////////////////////////////////////////

CEInternalEventQueue::CEInternalEventQueue(void)
    : CEEventQueue  ( static_cast<IEQueueListener &>(self()), static_cast<TEStack<CEEvent *, CEEvent *> &>(self()) )
{
    ; // do nothing
}

CEInternalEventQueue::~CEInternalEventQueue(void)
{
    removePendingEvents( false );
    RemoveAll();
}

void CEInternalEventQueue::SignalEvent(int /* eventCount */)
{
    ; // do nothing
}
