/************************************************************************
 * \file        areg/component/private/CENotificationEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Notification event.
 *
 ************************************************************************/
#include "areg/component/CENotificationEvent.hpp"
#include "areg/component/CEDispatcherThread.hpp"
#include "areg/component/CEProxyBase.hpp"

//////////////////////////////////////////////////////////////////////////
// CENotificationEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CENotificationEventData class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CENotificationEventData::CENotificationEventData( const CEProxyBase & proxy, NEService::eResultType notifyType, unsigned int notifyId, unsigned int seqNr )
    : mProxy        (&proxy)
    , mNotifyType   (notifyType)
    , mNotifyId     (notifyId)
    , mSequenceNr   (seqNr)
{
    ; // do nothing
}

CENotificationEventData::CENotificationEventData( const CENotificationEventData& src )
    : mProxy        (src.mProxy)
    , mNotifyType   (src.mNotifyType)
    , mNotifyId     (src.mNotifyId)
    , mSequenceNr   (src.mSequenceNr)
{
    ; // do nothing
}

CENotificationEventData::~CENotificationEventData( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, implement runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(CENotificationEvent, CEThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
void CENotificationEvent::SendEvent( const CENotificationEventData& data, IENotificationEventConsumer* caller /*= NULL*/ )
{
    CENotificationEvent* eventElem = DEBUG_NEW CENotificationEvent(data);
    if (eventElem != NULL)
    {
        if (caller != NULL)
            eventElem->SetEventConsumer(static_cast<IEEventConsumer *>(caller));
        static_cast<CEEvent *>(eventElem)->SendEvent();
    }
}

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
CENotificationEvent::CENotificationEvent( const CENotificationEventData& data )
    : CEThreadEventBase (CEEvent::EventNotifyClient)
    , mData             (data)
{
    SetTargetThread();
}

CENotificationEvent::~CENotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, methods
//////////////////////////////////////////////////////////////////////////
void CENotificationEvent::SetTargetThread( void )
{
    const CEProxyBase * proxy = mData.GetProxy();
    CEDispatcherThread& dispThread = proxy != NULL ? proxy->GetProxyDispatcherThread() : CEDispatcherThread::GetCurrentDispatcherThread();
    ASSERT(dispThread.IsValid());
    RegisterForThread(&dispThread);
}

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IENotificationEventConsumer::IENotificationEventConsumer( void )
    : CEThreadEventConsumerBase ( )
{
    ; // do nothing
}

IENotificationEventConsumer::~IENotificationEventConsumer( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void IENotificationEventConsumer::StartEventProcessing( CEEvent& eventElem )
{
    CENotificationEvent* eventNotify = RUNTIME_CAST(&eventElem, CENotificationEvent);
    if (eventNotify != NULL)
    {
        ProcessNotificationEvent(*eventNotify);
    }
    else
    {
        OUTPUT_WARN("Received not a Notification Event [ %s ]. Ignoring event processing", eventElem.GetRuntimeClassName());
    }
}
