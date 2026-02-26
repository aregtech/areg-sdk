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
namespace areg { class NotificationConsumer; }
namespace areg { class NotificationEventData; }
namespace areg { class NotificationEvent; }

/************************************************************************
 * Dependencies
 ************************************************************************/
namespace areg { class ProxyBase; }


namespace areg
{
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
         *                      See details in ResultType
         * \param	notifyId	The call ID, usually response ID or attribute ID.
         *                      If request fails, also request ID.
         * \param	seqNr	    The call sequence number.
         **/
        NotificationEventData( const areg::ProxyBase & proxy, ResultType notifyType, uint32_t notifyId, const SequenceNumber & seqNr );

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        NotificationEventData( const NotificationEventData & src );

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        NotificationEventData( NotificationEventData && src ) noexcept;

        /**
         * \brief   Destructor.
         **/
        ~NotificationEventData() = default;

        /**
         * \brief   Copies data from given source.
         * \param   src     The source of data to copy.
         **/
        NotificationEventData & operator = ( const NotificationEventData & src );

        /**
         * \brief   Moves data from given source.
         * \param   src     The source of data to move.
         **/
        NotificationEventData & operator = ( NotificationEventData && src ) noexcept;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
    public:
        /**
         * \brief   Returns Proxy pointer if any was set or returns null.
         **/
        inline const areg::ProxyBase * getProxy() const;
        /**
         * \brief   Sets the proxy object
         **/
        inline void setProxy(const areg::ProxyBase & proxy);

        /**
         * \brief   Returns the result of notification type.
         **/
        inline ResultType getNotifyType() const;

        /**
         * \brief   Sets the result of notification
         **/
        inline void setNotifyType(ResultType notifyType);

        /**
         * \brief   Returns notification message ID.
         **/
        inline uint32_t getNotifyId() const;
        /**
         * \brief   Sets notification message ID
         **/
        inline void setNotifyId( uint32_t notifyId );

        /**
         * \brief   Returns the sequence number of a caller.
         **/
        inline const SequenceNumber & getSequenceNr() const;
        /**
         * \brief   Sets sequence number of a caller.
         **/
        inline void setSequenceNr(const SequenceNumber & seqNr );

    //////////////////////////////////////////////////////////////////////////
    // Member variables.
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Pointer of proxy, which triggered notification event.
         **/
        const areg::ProxyBase *     mProxy;
        /**
         * \brief   Notification type (or notification result)
         **/
        ResultType mNotifyType;
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
     * \brief   The Notification Event is an internal Event created by
     *          Proxy to notify its clients. This class is a base class
     *          for all proxy notifications. Every Service Interface has
     *          own Notification Event type to provide uniqueness.
     * 
     *          Notification events are created when a proxy notifies response
     *          of request, broadcast or data update that clients receive. 
     *          The notification events are queued in the internal queue of 
     *          the dispatcher and are processed immediately after external 
     *          event processing is finished. All Proxy clients are instances
     *          of NotificationConsumer
     *
     *          The state-machines as well use internal notification events
     *          when generate event to trigger in state-machine.
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
         * \brief	Creates and sends Notification Event for
         *          specified notification consumer, and 
         *          containing specified notification data
         * \param	data	Notification Data to forward.
         * \param	caller  The Notification Consumer, which should be notified.
         **/
        static void sendEvent(const NotificationEventData & data, NotificationConsumer * caller = nullptr);

    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:

        /**
         * \brief	Initialization constructor.
         *          Sets notification event data.
         * \param	data	The Notification Event Data to set
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
         * \brief   Returns Notification Event object for read only.
         **/
        inline const NotificationEventData & getData() const;

    protected:
        /**
         * \brief   Returns Notification Event object for update.
         *          Protected.
         **/
        inline NotificationEventData & getData();

    //////////////////////////////////////////////////////////////////////////
    // Hidden functions
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Sets current thread as a event target thread.
         **/
        void setTargetThread();

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
     * \brief   Notification Event consumer is an object that receives
     *          notification messages. All Client objects are instances of 
     *          Notification Event Consumer to be able to receive and process
     *          notifications from proxies.
     **/
    class AREG_API NotificationConsumer  : public EventConsumer
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Default constructor.
         **/
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
    // EventConsumer overrides
    /************************************************************************/

        /**
         * \brief   Override method derived from EventConsumer.
         *          This function is filtering out notification events
         *          and triggers notification event processing function.
         **/
        void startEventProcessing( Event & eventElem ) override;

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

    inline const areg::ProxyBase* NotificationEventData::getProxy() const
    {
        return mProxy;
    }

    inline void NotificationEventData::setProxy( const areg::ProxyBase & proxy )
    {
        mProxy = &proxy;
    }

    inline ResultType NotificationEventData::getNotifyType() const
    {
        return mNotifyType;
    }

    inline void NotificationEventData::setNotifyType( ResultType notifyType )
    {
        mNotifyType = notifyType;
    }

    inline uint32_t NotificationEventData::getNotifyId() const
    {
        return mNotifyId;
    }

    inline void NotificationEventData::setNotifyId( uint32_t notifyId )
    {
        mNotifyId = notifyId;
    }

    inline const SequenceNumber & NotificationEventData::getSequenceNr() const
    {
        return mSequenceNr;
    }

    inline void NotificationEventData::setSequenceNr(const SequenceNumber & seqNr )
    {
        mSequenceNr = seqNr;
    }

    //////////////////////////////////////////////////////////////////////////
    // class NotificationEvent inline function implementation
    //////////////////////////////////////////////////////////////////////////
    inline const NotificationEventData & NotificationEvent::getData() const
    {
        return mData;
    }

    inline NotificationEventData & NotificationEvent::getData()
    {
        return mData;
    }

} // namespace areg
#endif  // AREG_COMPONENT_NOTIFICATIONEVENT_HPP
