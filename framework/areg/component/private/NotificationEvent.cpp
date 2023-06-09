/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/component/private/NotificationEvent.cpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan
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
}

NotificationEventData::NotificationEventData( const NotificationEventData& src )
    : mProxy        (src.mProxy)
    , mNotifyType   (src.mNotifyType)
    , mNotifyId     (src.mNotifyId)
    , mSequenceNr   (src.mSequenceNr)
{
}

NotificationEventData::NotificationEventData( NotificationEventData && src ) noexcept
    : mProxy        ( src.mProxy )
    , mNotifyType   ( src.mNotifyType )
    , mNotifyId     ( src.mNotifyId )
    , mSequenceNr   ( src.mSequenceNr )
{
    src.mProxy = nullptr;
}

NotificationEventData & NotificationEventData::operator = ( const NotificationEventData & src )
{
    if ( this != &src )
    {
        mProxy      = src.mProxy;
        mNotifyType = src.mNotifyType;
        mNotifyId   = src.mNotifyId;
        mSequenceNr = src.mSequenceNr;
    }

    return (*this);
}

NotificationEventData & NotificationEventData::operator = ( NotificationEventData && src ) noexcept
{
    if ( this != &src )
    {
        mProxy      = src.mProxy;
        mNotifyType = src.mNotifyType;
        mNotifyId   = src.mNotifyId;
        mSequenceNr = src.mSequenceNr;

        src.mProxy  = nullptr;
    }

    return (*this);
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
void NotificationEvent::sendEvent( const NotificationEventData& data, IENotificationEventConsumer* caller /*= nullptr*/ )
{
    NotificationEvent* eventElem = DEBUG_NEW NotificationEvent(data);
    if (eventElem != nullptr)
    {
        if (caller != nullptr)
            eventElem->setEventConsumer(static_cast<IEEventConsumer *>(caller));
        static_cast<Event *>(eventElem)->deliverEvent();
    }
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
NotificationEvent::NotificationEvent( const NotificationEventData& data )
    : ThreadEventBase (Event::eEventType::EventNotifyClient)
    , mData             (data)
{
    setTargetThread();
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::setTargetThread( void )
{
    const ProxyBase * proxy = mData.getProxy();
    DispatcherThread& dispThread = proxy != nullptr ? proxy->getProxyDispatcherThread() : DispatcherThread::getCurrentDispatcherThread();
    ASSERT(dispThread.isValid());
    registerForThread(&dispThread);
}

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void IENotificationEventConsumer::startEventProcessing( Event& eventElem )
{
    NotificationEvent* eventNotify = RUNTIME_CAST(&eventElem, NotificationEvent);
    if (eventNotify != nullptr)
    {
        processNotificationEvent(*eventNotify);
    }
    else
    {
        OUTPUT_WARN("Received not a Notification Event [ %s ]. Ignoring event processing", eventElem.getRuntimeClassName().getString());
    }
}
