#ifndef AREG_COMPONENT_NOTIFICATIONEVENT_HPP
#define AREG_COMPONENT_NOTIFICATIONEVENT_HPP
/************************************************************************
 * \file        areg/component/NotificationEvent.hpp
 * \ingroup     AREG SDK, Asynchronous Event Generator Software Development Kit 
 * \author      Artak Avetyan (mailto:artak@aregtech.com)
 * \brief       AREG Platform, Generic Notification event,
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
#include "areg/component/private/ThreadEventBase.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
class IENotificationEventConsumer;
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
 *              3. IENotificationEventConsumer -- Notification Event Consumer to process event.
 **/

//////////////////////////////////////////////////////////////////////////
// NotificationEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Notification Event data to send to forward from Proxy to
 *              its clients. Contains main basic information to trigger
 *              appropriate response and update function calls.
 * 
 * \details     The Notification Event Data object contains basic information
 *              to trigger appropriate response, update or request failure
 *              function calls. It contains notification message ID,
 *              call sequence number, result type and Proxy information.
 *              This data is a part of Notification Event. Extend
 *              only if there is a need.
 *
 **/
class AREG_API NotificationEventData
{
//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief	Constructor. Creates and sets Notification event data
     * \param	proxy	    The pointer of Proxy object which triggered
     *                      notification message
     * \param	notifyType	The result flag of notification event. 
     *                      See details in NEService::eResultType
     * \param	notifyId	The call ID, usually response ID or attribute ID.
     *                      If request fails, also request ID.
     * \param	seqNr	    The call sequence number.
     **/
    NotificationEventData( const ProxyBase & proxy, NEService::eResultType notifyType, unsigned int notifyId, unsigned int seqNr );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    NotificationEventData( const NotificationEventData & src );

    /**
     * \brief   Destructor.
     **/
    ~NotificationEventData( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Proxy pointer if any was set or returns null.
     **/
    inline const ProxyBase * getProxy( void ) const;
    /**
     * \brief   Sets the proxy object
     **/
    inline void setProxy(const ProxyBase & proxy);

    /**
     * \brief   Returns the result of notification type.
     **/
    inline NEService::eResultType getNotifyType( void ) const;
    /**
     * \brief   Sets the result of notification
     **/
    inline void setNotifyType(NEService::eResultType notifyType);

    /**
     * \brief   Returns notification message ID.
     **/
    inline unsigned int getNotifyId( void ) const;
    /**
     * \brief   Sets notification message ID
     **/
    inline void setNotifyId( unsigned int notifyId );

    /**
     * \brief   Returns the sequence number of a caller.
     **/
    inline unsigned int getSequenceNr( void ) const;
    /**
     * \brief   Sets sequence number of a caller.
     **/
    inline void setSequenceNr( unsigned int seqNr );

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
    NEService::eResultType mNotifyType;
    /**
     * \brief   Notification message ID
     **/
    unsigned int            mNotifyId;
    /**
     * \brief   Call sequence number.
     **/
    unsigned int            mSequenceNr;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls.
//////////////////////////////////////////////////////////////////////////
private:
    NotificationEventData( void );
    const NotificationEventData & operator = ( const NotificationEventData & /*src*/ );
};


//////////////////////////////////////////////////////////////////////////
// NotificationEvent class declaration, used to send notifications
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       The Notification Event is an internal Event created by
 *              Proxy to notify about certain event its clients.
 *              This is Generic and base class for notifications.
 *              Every Service Interface should have own Notification
 *              Event type to provide uniqueness.
 * 
 * \details     When a certain request call response, or request failure,
 *              or attribute update message has been arrived on Proxy side,
 *              it will lookup for assigned listeners and send internal
 *              Notification Events. All notification events are queued
 *              in the internal queue of dispatcher and will be processed
 *              immediately after external event processing has been finished.
 *              The internal event queue will get list of notification
 *              event listeners, which are clients of Proxy, and trigger
 *              appropriate event processing call. All Proxy clients should
 *              be instance of IENotificationEventConsumer
 *
 **/
class AREG_API NotificationEvent   : public ThreadEventBase
{
//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(NotificationEvent)

//////////////////////////////////////////////////////////////////////////
// NotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates and sends Notification Event for
     *          specified notification consumer, and 
     *          containing specified notification data
     * \param	data	    Notification Data to forward.
     * \param	consumer	The Notification Consumer, which should be notified.
     **/
    static void sendEvent(const NotificationEventData& data, IENotificationEventConsumer* caller = NULL);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Initialization constructor.
     *          Sets notification event data.
     * \param	data	The Notification Event Data to set
     **/
    NotificationEvent( const NotificationEventData & data );

    /**
     * \brief   Destructor.
     **/
    virtual ~NotificationEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Notification Event object for read only.
     **/
    inline const NotificationEventData & getData( void ) const;

protected:
    /**
     * \brief   Returns Notification Event object for update.
     *          Protected.
     **/
    inline NotificationEventData & getData( void );

//////////////////////////////////////////////////////////////////////////
// Hidden functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets current thread as a event target thread.
     **/
    void setTargetThread( void );

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
    NotificationEvent( void );
    NotificationEvent( const NotificationEvent & /*src*/ );
    const NotificationEvent & operator = ( const NotificationEvent & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// IENotificationEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief       Notification Event consumer is an object, which will 
 *              receive notification messages. All Client objects should
 *              be instances of Notification Event Consumer to be able
 *              to receive and process notifications.
 * 
 * \details     Every client object should overwrite event processing
 *              function, dispatch notification message, make updates 
 *              according of message and result. All attribute data
 *              and response parameters are stored on Proxy side
 *              to share information between several instances of 
 *              same client type. The notification consumer is an instance
 *              of IEEventConsumer (ThreadEventConsumerBase) and 
 *              registered in Proxy listener list to process Notification
 *              events sent by proxy to its clients.
 *
 **/
class AREG_API IENotificationEventConsumer  : public ThreadEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    /**
     * \brief   Default constructor.
     **/
    IENotificationEventConsumer( void );

    /**
     * \brief   Destructor.
     **/
    virtual ~IENotificationEventConsumer( void );

//////////////////////////////////////////////////////////////////////////
// Overrides.
//////////////////////////////////////////////////////////////////////////
public:
/************************************************************************/
// IENotificationEventConsumer overrides
/************************************************************************/

    /**
     * \brief   Notification event processing function.
     *          Should be overwritten by every client object.
     *          Function is triggered by dispatcher when notification
     *          event is going to be processed.
     * \param   eventElem   The notification event object to process.
     **/
    virtual void processNotificationEvent( NotificationEvent & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// ThreadEventConsumerBase overrides
/************************************************************************/

    /**
     * \brief   Override method derived from IEEventConsumer.
     *          This function is filtering out notification events
     *          and triggers notification event processing function.
     **/
    virtual void startEventProcessing( Event & eventElem );

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    IENotificationEventConsumer(const IENotificationEventConsumer & /*src*/ );
    const IENotificationEventConsumer & operator = (const IENotificationEventConsumer & /*src*/ );
};

//////////////////////////////////////////////////////////////////////////
// inline function implementation
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// class NotificationEventData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const ProxyBase* NotificationEventData::getProxy( void ) const
{
    return mProxy;
}

inline void NotificationEventData::setProxy( const ProxyBase & proxy )
{
    mProxy = &proxy;
}

inline NEService::eResultType NotificationEventData::getNotifyType( void ) const
{
    return mNotifyType;
}

inline void NotificationEventData::setNotifyType( NEService::eResultType notifyType )
{
    mNotifyType = notifyType;
}

inline unsigned int NotificationEventData::getNotifyId( void ) const
{
    return mNotifyId;
}

inline void NotificationEventData::setNotifyId( unsigned int notifyId )
{
    mNotifyId = notifyId;
}

inline unsigned int NotificationEventData::getSequenceNr( void ) const
{
    return mSequenceNr;
}

inline void NotificationEventData::setSequenceNr( unsigned int seqNr )
{
    mSequenceNr = seqNr;
}

//////////////////////////////////////////////////////////////////////////
// class NotificationEvent inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const NotificationEventData & NotificationEvent::getData( void ) const
{
    return mData;
}

inline NotificationEventData & NotificationEvent::getData( void )
{
    return mData;
}

#endif  // AREG_COMPONENT_NOTIFICATIONEVENT_HPP
