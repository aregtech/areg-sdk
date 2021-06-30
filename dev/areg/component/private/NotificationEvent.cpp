/************************************************************************
 * \file        areg/component/private/NotificationEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Notification event.
 *
 ************************************************************************/
#include "areg/component/NotificationEvent.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/component/ProxyBase.hpp"

//////////////////////////////////////////////////////////////////////////
// NotificationEventData class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotificationEventData class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
NotificationEventData::NotificationEventData( const ProxyBase & proxy, NEService::eResultType notifyType, unsigned int notifyId, unsigned int seqNr )
    : mProxy        (&proxy)
    , mNotifyType   (notifyType)
    , mNotifyId     (notifyId)
    , mSequenceNr   (seqNr)
{
    ; // do nothing
}

NotificationEventData::NotificationEventData( const NotificationEventData& src )
    : mProxy        (src.mProxy)
    , mNotifyType   (src.mNotifyType)
    , mNotifyId     (src.mNotifyId)
    , mSequenceNr   (src.mSequenceNr)
{
    ; // do nothing
}

NotificationEventData::~NotificationEventData( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, implement runtime
//////////////////////////////////////////////////////////////////////////
IMPLEMENT_RUNTIME_EVENT(NotificationEvent, ThreadEventBase)

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::sendEvent( const NotificationEventData& data, IENotificationEventConsumer* caller /*= NULL*/ )
{
    NotificationEvent* eventElem = DEBUG_NEW NotificationEvent(data);
    if (eventElem != NULL)
    {
        if (caller != NULL)
            eventElem->setEventConsumer(static_cast<IEEventConsumer *>(caller));
        static_cast<Event *>(eventElem)->deliverEvent();
    }
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
NotificationEvent::NotificationEvent( const NotificationEventData& data )
    : ThreadEventBase (Event::EventNotifyClient)
    , mData             (data)
{
    setTargetThread();
}

NotificationEvent::~NotificationEvent( void )
{
    ; // do nothing
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::setTargetThread( void )
{
    const ProxyBase * proxy = mData.getProxy();
    DispatcherThread& dispThread = proxy != NULL ? proxy->getProxyDispatcherThread() : DispatcherThread::getCurrentDispatcherThread();
    ASSERT(dispThread.isValid());
    registerForThread(&dispThread);
}

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
IENotificationEventConsumer::IENotificationEventConsumer( void )
    : ThreadEventConsumerBase ( )
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
void IENotificationEventConsumer::startEventProcessing( Event& eventElem )
{
    NotificationEvent* eventNotify = RUNTIME_CAST(&eventElem, NotificationEvent);
    if (eventNotify != NULL)
    {
        processNotificationEvent(*eventNotify);
    }
    else
    {
        OUTPUT_WARN("Received not a Notification Event [ %s ]. Ignoring event processing", eventElem.getRuntimeClassName());
    }
}
