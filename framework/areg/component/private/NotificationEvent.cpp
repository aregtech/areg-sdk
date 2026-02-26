/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/private/NotificationEvent.cpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Notification event.
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
NotificationEventData::NotificationEventData( const ProxyBase & proxy, areg::ResultType notifyType, uint32_t notifyId, const SequenceNumber & seqNr )
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
AREG_IMPLEMENT_RUNTIME_EVENT(NotificationEvent, areg::Event)

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::sendEvent( const NotificationEventData& data, NotificationConsumer* caller /*= nullptr*/ )
{
    NotificationEvent* eventElem = DEBUG_NEW NotificationEvent(data);
    if (eventElem != nullptr)
    {
        if (caller != nullptr)
            eventElem->setEventConsumer(static_cast<areg::EventConsumer *>(caller));
        static_cast<areg::Event *>(eventElem)->deliverEvent();
    }
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
NotificationEvent::NotificationEvent( const NotificationEventData& data )
    : areg::Event (areg::Event::EventType::EventNotifyClient)
    , mData (data)
{
    setTargetThread();
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::setTargetThread()
{
    const ProxyBase * proxy = mData.getProxy();
    areg::DispatcherThread& dispThread = proxy != nullptr ? proxy->getProxyDispatcherThread() : areg::DispatcherThread::getCurrentDispatcherThread();
    ASSERT(dispThread.isValid());
    registerForThread(&dispThread);
}

//////////////////////////////////////////////////////////////////////////
// NotificationConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotificationConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationConsumer::startEventProcessing( areg::Event& eventElem )
{
    NotificationEvent* eventNotify = AREG_RUNTIME_CAST(&eventElem, NotificationEvent);
    if (eventNotify != nullptr)
    {
        processNotificationEvent(*eventNotify);
    }
}
