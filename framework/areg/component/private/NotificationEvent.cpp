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
namespace areg {

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
// NotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::send_event( const NotificationEventData& data, NotificationConsumer* caller /*= nullptr*/ )
{
    NotificationEvent eventElem(data);
    if (caller != nullptr)
        eventElem.set_event_consumer(static_cast<EventConsumer *>(caller));

    DispatcherThread* _dt{ eventElem.target_dispatcher() };
    if ((_dt == nullptr) || !_dt->event_dispatcher().post_event(eventElem))
        eventElem.destroy_event();
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, constructor / destructor
//////////////////////////////////////////////////////////////////////////
NotificationEvent::NotificationEvent( const NotificationEventData& data )
    : Event (areg::EventType::EventNotifyClient, static_cast<uint32_t>(sizeof(NotificationEventData)), areg::DefaultPriority)
{
    uint8_t* ptr = payload_ptr();
    new(ptr) NotificationEventData(data);
    current_target_thread();
}

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationEvent::current_target_thread()
{
    const ProxyBase * proxy = data().service_proxy();
    DispatcherThread& dispThread = proxy != nullptr ? proxy->proxy_dispatcher_thread() : DispatcherThread::current_dispatcher_thread();
    ASSERT(dispThread.is_valid());
    register_for_thread(&dispThread);
}

//////////////////////////////////////////////////////////////////////////
// NotificationConsumer class implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NotificationConsumer class, methods
//////////////////////////////////////////////////////////////////////////
void NotificationConsumer::start_event_processing( Event& eventElem )
{
    if (eventElem.event_type() == areg::EventType::EventNotifyClient)
    {
        process_notification_event(static_cast<NotificationEvent&>(eventElem));
    }
}

} // namespace areg
