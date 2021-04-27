#ifndef AREG_COMPONENT_CENOTIFICATIONEVENT_HPP
#define AREG_COMPONENT_CENOTIFICATIONEVENT_HPP
/************************************************************************
 * \file        areg/component/CENotificationEvent.hpp
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
#include "areg/component/private/CEThreadEventBase.hpp"
#include "areg/component/NEService.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
class IENotificationEventConsumer;
class CENotificationEventData;
class CENotificationEvent;

/************************************************************************
 * Dependencies
 ************************************************************************/
class CEProxyBase;

/**
 * \remark  The Notifications Events are used in Proxies to communicate
 *          with Clients. All Clients are instances of Notification Event Consumer.
 *          Following classes are implemented in this file:
 *              1. CENotificationEventData  -- the data contained in notification event.
 *              2. CENotificationEvent      -- the notification event used for communication
 *              3. IENotificationEventConsumer -- Notification Event Consumer to process event.
 **/

//////////////////////////////////////////////////////////////////////////
// CENotificationEventData class declaration
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
class AREG_API CENotificationEventData
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
    CENotificationEventData( const CEProxyBase & proxy, NEService::eResultType notifyType, unsigned int notifyId, unsigned int seqNr );

    /**
     * \brief   Copy constructor.
     * \param   src     The source of data to copy.
     **/
    CENotificationEventData( const CENotificationEventData & src );

    /**
     * \brief   Destructor.
     **/
    ~CENotificationEventData( void );

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Proxy pointer if any was set or returns null.
     **/
    inline const CEProxyBase * GetProxy( void ) const;
    /**
     * \brief   Sets the proxy object
     **/
    inline void SetProxy(const CEProxyBase & proxy);

    /**
     * \brief   Returns the result of notification type.
     **/
    inline NEService::eResultType GetNotifyType( void ) const;
    /**
     * \brief   Sets the result of notification
     **/
    inline void SetNotifyType(NEService::eResultType notifyType);

    /**
     * \brief   Returns notification message ID.
     **/
    inline unsigned int GetNotifyId( void ) const;
    /**
     * \brief   Sets notification message ID
     **/
    inline void SetNotifyId( unsigned int notifyId );

    /**
     * \brief   Returns the sequence number of a caller.
     **/
    inline unsigned int GetSequenceNr( void ) const;
    /**
     * \brief   Sets sequence number of a caller.
     **/
    inline void SetSequenceNr( unsigned int seqNr );

//////////////////////////////////////////////////////////////////////////
// Member variables.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Pointer of proxy, which triggered notification event.
     **/
    const CEProxyBase *     mProxy;
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
    CENotificationEventData( void );
    const CENotificationEventData & operator = ( const CENotificationEventData & /*src*/ );
};


//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class declaration, used to send notifications
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
class AREG_API CENotificationEvent   : public CEThreadEventBase
{
//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, Declare Runtime Event
//////////////////////////////////////////////////////////////////////////
    DECLARE_RUNTIME_EVENT(CENotificationEvent)

//////////////////////////////////////////////////////////////////////////
// CENotificationEvent class, static methods
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief	Creates and sends Notification Event for
     *          specified notification consumer, and 
     *          containing specified notification data
     * \param	data	    Notification Data to forward.
     * \param	consumer	The Notification Consumer, which should be notified.
     **/
    static void SendEvent(const CENotificationEventData& data, IENotificationEventConsumer* caller = NULL);

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief	Initialization constructor.
     *          Sets notification event data.
     * \param	data	The Notification Event Data to set
     **/
    CENotificationEvent( const CENotificationEventData & data );

    /**
     * \brief   Destructor.
     **/
    virtual ~CENotificationEvent( void );

//////////////////////////////////////////////////////////////////////////
// Attributes and operations.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns Notification Event object for read only.
     **/
    inline const CENotificationEventData & GetData( void ) const;

protected:
    /**
     * \brief   Returns Notification Event object for update.
     *          Protected.
     **/
    inline CENotificationEventData & GetData( void );

//////////////////////////////////////////////////////////////////////////
// Hidden functions
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Sets current thread as a event target thread.
     **/
    void SetTargetThread( void );

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Notification event data
     **/
    CENotificationEventData mData;

//////////////////////////////////////////////////////////////////////////
// Hidden / Forbidden method calls
//////////////////////////////////////////////////////////////////////////
private:
    CENotificationEvent( void );
    CENotificationEvent( const CENotificationEvent & /*src*/ );
    const CENotificationEvent & operator = ( const CENotificationEvent & /*src*/ );
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
 *              of IEEventConsumer (CEThreadEventConsumerBase) and 
 *              registered in Proxy listener list to process Notification
 *              events sent by proxy to its clients.
 *
 **/
class AREG_API IENotificationEventConsumer  : public CEThreadEventConsumerBase
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
    virtual void ProcessNotificationEvent( CENotificationEvent & eventElem ) = 0;

//////////////////////////////////////////////////////////////////////////
// Hidden methods
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// CEThreadEventConsumerBase overrides
/************************************************************************/

    /**
     * \brief   Override method derived from IEEventConsumer.
     *          This function is filtering out notification events
     *          and triggers notification event processing function.
     **/
    virtual void StartEventProcessing( CEEvent & eventElem );

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
// class CENotificationEventData inline function implementation
//////////////////////////////////////////////////////////////////////////

inline const CEProxyBase* CENotificationEventData::GetProxy( void ) const
{   return mProxy;              }

inline void CENotificationEventData::SetProxy( const CEProxyBase & proxy )
{   mProxy = &proxy;            }

inline NEService::eResultType CENotificationEventData::GetNotifyType( void ) const
{   return mNotifyType;         }

inline void CENotificationEventData::SetNotifyType( NEService::eResultType notifyType )
{   mNotifyType = notifyType;   }

inline unsigned int CENotificationEventData::GetNotifyId( void ) const
{   return mNotifyId;           }

inline void CENotificationEventData::SetNotifyId( unsigned int notifyId )
{   mNotifyId = notifyId;       }

inline unsigned int CENotificationEventData::GetSequenceNr( void ) const
{   return mSequenceNr;         }

inline void CENotificationEventData::SetSequenceNr( unsigned int seqNr )
{   mSequenceNr = seqNr;        }

//////////////////////////////////////////////////////////////////////////
// class CENotificationEvent inline function implementation
//////////////////////////////////////////////////////////////////////////
inline const CENotificationEventData & CENotificationEvent::GetData( void ) const
{   return mData;   }

inline CENotificationEventData & CENotificationEvent::GetData( void )
{   return mData;   }

#endif  // AREG_COMPONENT_CENOTIFICATIONEVENT_HPP
