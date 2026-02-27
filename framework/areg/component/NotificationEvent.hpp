#ifndef AREG_COMPONENT_NOTIFICATIONEVENT_HPP
#define AREG_COMPONENT_NOTIFICATIONEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/NotificationEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Generic Notification event,
 *              Notification data and notification consumer classes.
 *              This generic event class is a base class for all
 *              notification messages sent by Proxy to its client objects.
 *              Extern this class to send notification messages.
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/Event.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/ServiceDefs.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
class NotificationConsumer;
class NotificationEventData;
class NotificationEvent;

/************************************************************************
 * Dependencies
 ************************************************************************/
class ProxyBase;

/**
 * \remark  The Notifications Events are used in Proxies to communicate
 *          with Clients. All Clients are instances of Notification Event Consumer.
 *          Following classes are implemented in this file:
 *              1. NotificationEventData  -- the data contained in notification event.
 *              2. NotificationEvent      -- the notification event used for communication
 *              3. NotificationConsumer -- Notification Event Consumer to process event.
 **/

//////////////////////////////////////////////////////////////////////////
// NotificationEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Notification event data sent from Proxy to its clients. Contains information needed to
 *          trigger response and update function calls.
 **/
class AREG_API NotificationEventData
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Creates and sets Notification event data.
     *
     * \param   proxy           The pointer of Proxy object which triggered notification message
     * \param   notifyType      The result flag of notification event. See details in
     *                          NEService::ResultType
     * \param   notifyId        The call ID, usually response ID or attribute ID. If request fails,
     *                          also request ID.
     * \param   seqNr           The call sequence number.
     **/
    NotificationEventData( const ProxyBase & proxy, NEService::ResultType notifyType, uint32_t notifyId, const SequenceNumber & seqNr );

    /**
     * \brief   Copies data from given source.
     *
     * \param   src     The source of data to copy.
     **/
    NotificationEventData( const NotificationEventData & src );

    /**
     * \brief   Moves data from given source.
     *
     * \param   src     The source of data to move.
     **/
    NotificationEventData( NotificationEventData && src ) noexcept;

    /**
     * \brief   Destructor.
     **/
    ~NotificationEventData() = default;

    /**
     * \brief   Copies data from given source.
     *
     * \param   src     The source of data to copy.
     **/
    NotificationEventData & operator = ( const NotificationEventData & src );

    /**
     * \brief   Moves data from given source.
     *
     * \param   src     The source of data to move.
     **/
    NotificationEventData & operator = ( NotificationEventData && src ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Proxy pointer if set; nullptr otherwise.
     **/
    inline const ProxyBase * proxy() const;
    /**
     * \brief   Sets the proxy object.
     *
     * \param   proxy       The Proxy object to set.
     **/
    inline void set_proxy(const ProxyBase & proxy);

    /**
     * \brief   Returns the result of notification type.
     **/
    inline NEService::ResultType notify_type() const;

    /**
     * \brief   Sets the result of notification.
     *
     * \param   notifyType      The notification result to set.
     **/
    inline void set_notify_type(NEService::ResultType notifyType);

    /**
     * \brief   Returns notification message ID.
     **/
    inline uint32_t notify_id() const;
    /**
     * \brief   Sets notification message ID.
     *
     * \param   notifyId    The notification message ID to set.
     **/
    inline void set_notify_id( uint32_t notifyId );

    /**
     * \brief   Returns the sequence number of a caller.
     **/
    inline const SequenceNumber & sequence_nr() const;
    /**
     * \brief   Sets sequence number of a caller.
     *
     * \param   seqNr       The sequence number to set.
     **/
    inline void set_sequence_nr(const SequenceNumber & seqNr );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer of proxy, which triggered notification event.
     **/
    const ProxyBase *     mProxy;
    /**
     * \brief   Notification type (or notification result)
     **/
    NEService::ResultType mNotifyType;
    /**
     * \brief   Notification message ID
     **/
    uint32_t            mNotifyId;
    /**
     * \brief   Call sequence number.
     **/
    SequenceNumber          mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    NotificationEventData() = delete;
};


//////////////////////////////////////////////////////////////////////////
// NotificationEvent class declaration, used to send notifications
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for notification events sent by a proxy to notify clients of attribute
 *          updates, broadcasts, and responses. Internal use only.
 **/
class AREG_API NotificationEvent   : public Event
{
//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    AREG_DECLARE_RUNTIME_EVENT(NotificationEvent)

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Creates and sends a notification event to the specified consumer with the given
     *          notification data.
     *
     * \param   data        The notification data to forward.
     * \param   caller      The notification consumer to notify, or null to broadcast.
     **/
    static void send_event(const NotificationEventData & data, NotificationConsumer * caller = nullptr);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Initializes the notification event with the given data.
     *
     * \param   data    The notification event data to set.
     **/
    explicit NotificationEvent( const NotificationEventData & data );

    /**
     * \brief   Destructor.
     **/
    virtual ~NotificationEvent() = default;

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns a read-only reference to the notification event data.
     **/
    inline const NotificationEventData & data() const;

protected:
    /**
     * \brief   Returns a modifiable reference to the notification event data. Protected.
     **/
    inline NotificationEventData & data();

//////////////////////////////////////////////////////////////////////////
// Hidden functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets the current thread as the target thread for event delivery.
     **/
    void set_target_thread();

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Notification event data
     **/
    NotificationEventData mData;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    NotificationEvent() = delete;
    AREG_NOCOPY_NOMOVE( NotificationEvent );
};

//////////////////////////////////////////////////////////////////////////
// NotificationConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for all client objects that consume and process notification events from
 *          proxies.
 **/
class AREG_API NotificationConsumer  : public EventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    NotificationConsumer() = default;

    /**
     * \brief   Destructor.
     **/
    virtual ~NotificationConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// NotificationConsumer overrides
/************************************************************************/

    /**
     * \brief   Processes a notification event. Override to handle attribute updates, broadcasts,
     *          and response notifications.
     *
     * \param   eventElem       The notification event object to process.
     **/
    virtual void process_notification_event( NotificationEvent & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// EventConsumer overrides
/************************************************************************/

    /**
     * \brief   Filters notification events and delegates to process_notification_event().
     *          Automatically triggered by the dispatcher.
     *
     * \param   eventElem       The event being processed.
     **/
    void start_event_processing( Event & eventElem ) override;

//////////////////////////////////////////////////////////////////////////
// Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE( NotificationConsumer );
};

//////////////////////////////////////////////////////////////////////////
// inline function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class NotificationEventData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const ProxyBase* NotificationEventData::proxy() const
{
    return mProxy;
}

inline void NotificationEventData::set_proxy( const ProxyBase & proxy )
{
    mProxy = &proxy;
}

inline NEService::ResultType NotificationEventData::notify_type() const
{
    return mNotifyType;
}

inline void NotificationEventData::set_notify_type( NEService::ResultType notifyType )
{
    mNotifyType = notifyType;
}

inline uint32_t NotificationEventData::notify_id() const
{
    return mNotifyId;
}

inline void NotificationEventData::set_notify_id( uint32_t notifyId )
{
    mNotifyId = notifyId;
}

inline const SequenceNumber & NotificationEventData::sequence_nr() const
{
    return mSequenceNr;
}

inline void NotificationEventData::set_sequence_nr(const SequenceNumber & seqNr )
{
    mSequenceNr = seqNr;
}

//////////////////////////////////////////////////////////////////////////
// class NotificationEvent inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const NotificationEventData & NotificationEvent::data() const
{
    return mData;
}

inline NotificationEventData & NotificationEvent::data()
{
    return mData;
}

#endif  // AREG_COMPONENT_NOTIFICATIONEVENT_HPP
