#ifndef AREG_COMPONENT_STUBEVENT_HPP
#define AREG_COMPONENT_STUBEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/component/StubEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit 
 * \author      Artak Avetyan
 * \brief       Areg Platform, Component Event and 
 *              Component Event Consumer class.
 *              Component Event is a base class for all kind of Request and
 *              Response events. All objects, which are interested to receive
 *              such events, should be instance of Component Event Consumer.
 *              For more details see description bellow.
 *
 *              Component Events are defined as a type of event to communicate
 *              between components and its service interfaces. 
 *              This is base class for all type of Request and Response Events.
 *              All Service Instance, which need to receive such events,
 *              should
 *
 ************************************************************************/
/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/base/RuntimeObject.hpp"
#include "areg/component/EventConsumer.hpp"
#include "areg/component/StreamableEvent.hpp"
#include "areg/component/ProxyAddress.hpp"
#include "areg/component/StubAddress.hpp"

/************************************************************************
 * List of declared classes
 ************************************************************************/
// class StreamableEvent
    class StubEvent;
// class EventConsumer
    namespace areg { class StubEventConsumer; }

/************************************************************************
 * Dependencies
 ************************************************************************/
class ServiceRequestEvent;
class RequestEvent;
class NotifyRequestEvent;
class StubConnectEvent;

namespace areg
{
    /************************************************************************
     * \brief   This file contains declarations of following classes:
     *              1. StubEvent
     *              2. StubEventConsumer
     *          These objects are defining event object for component communication
     *          and the consumer object to parse and process event.
     *          For more information, see description bellow.
     ************************************************************************/

    //////////////////////////////////////////////////////////////////////////
    // StubEvent class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   StubEvent class is a base class for all kind of Request events
     *          processed on Stub side. StubEvent cannot be used for internal event.
     *          By default it is marked as Service Internal to communicate with
     *          ServiceManager.
     **/
    class AREG_API StubEvent  : public areg::StreamableEvent
    {
    /************************************************************************/
    // Friend classes
    /************************************************************************/
        friend class StubEventConsumer;

    //////////////////////////////////////////////////////////////////////////
    // Declare as runtime event object
    //////////////////////////////////////////////////////////////////////////
        AREG_DECLARE_RUNTIME_EVENT(StubEvent)

    //////////////////////////////////////////////////////////////////////////
    // Constructors / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
    /************************************************************************/
    // Protected, accessed from derived classes
    /************************************************************************/

        /**
         * \brief   Initialize component event from streaming object.
         **/
        StubEvent( const InStream & stream );
        
        /**
         * \brief   Initializes target Stub addresses and sets event type.
         * \param	toTarget    The address of target Stub
         * \param	eventType   The type of event.
         **/
        StubEvent(const areg::StubAddress & toTarget, Event::EventType eventType );

        /**
         * \brief   Destructor.
         **/
        virtual ~StubEvent() = default;

    //////////////////////////////////////////////////////////////////////////
    // Overrides
    //////////////////////////////////////////////////////////////////////////
    public:
    /************************************************************************/
    // Event class overrides
    /************************************************************************/
        /**
         * \brief   Sends the event to target thread. If target thread
         *          is nullptr, it searches event target thread, registered in system.
         **/
        void deliverEvent() override;

    //////////////////////////////////////////////////////////////////////////
    // Attributes
    //////////////////////////////////////////////////////////////////////////
        /**
         * \brief   Returns the address of Stub of event target.
         **/
        inline const areg::StubAddress & getTargetStub() const;

    //////////////////////////////////////////////////////////////////////////
    // Operations
    //////////////////////////////////////////////////////////////////////////
    protected:
    /************************************************************************/
    // StreamableEvent overrides
    /************************************************************************/

        /**
         * \brief   Initialize component address from reading stream
         * \param   stream  The reading stream to read out data
         **/
        const InStream & readStream( const InStream & stream ) override;

        /**
         * \brief   Write component address to stream.
         * \param   stream  The writing stream to write in data
         **/
        OutStream & writeStream( OutStream & stream ) const override;

    //////////////////////////////////////////////////////////////////////////
    // Member variables
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Event target Stub address
         **/
        areg::StubAddress   mTargetStubAddress;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls.
    //////////////////////////////////////////////////////////////////////////
    private:
        StubEvent() = delete;
        AREG_NOCOPY_NOMOVE( StubEvent );
    };

    //////////////////////////////////////////////////////////////////////////
    // StubEventConsumer class declaration
    //////////////////////////////////////////////////////////////////////////
    /**
     * \brief   All Stub (service provider) objects are instances of 
     *          StubEventConsumer to receive and process component events.
     *          StubEventConsumer is registered at component thread as a
     *          consumer of Stub specific events. It is extended in StubBase
     *          class, which is a base class for all Stub objects.
     **/
    class AREG_API StubEventConsumer  : public EventConsumer
    {
    //////////////////////////////////////////////////////////////////////////
    // Constructor / Destructor
    //////////////////////////////////////////////////////////////////////////
    protected:
        /**
         * \brief   Default constructor
         * \param   stubAddress The address of stub object, which is handling consumer
         **/
        explicit StubEventConsumer( const areg::StubAddress & stubAddress );

        /**
         * \brief   Destructor
         **/
        virtual ~StubEventConsumer() = default;

        /**
         * \brief   Returns the pointer to the currently processing event object.
         **/
        inline const Event* getCurrentEvent() const;

    //////////////////////////////////////////////////////////////////////////
    // Overrides, event processing functions.
    //////////////////////////////////////////////////////////////////////////
    protected:

        /**
         * \brief	This event processing function is triggered when Stub 
         *          is receiving request to start a function.
         * \param	eventElem	Service Request event object to process,
         *                      which contains request ID and serialized
         *                      parameters.
         **/
        virtual void processRequestEvent( ServiceRequestEvent & eventElem ) = 0;

        /**
         * \brief	This event processing function is triggered when Stub
         *          is receiving request to send attribute data.
         * \param	eventElem	Service Request event object to process,
         *                      which contains attribute ID.
         **/
        virtual void processAttributeEvent( ServiceRequestEvent & eventElem ) = 0;

        /**
         * \brief	This function is triggered when component event should
         *          be processed and this component event is not a 
         *          Service Request type.
         * \param	eventElem	Component Event to process.
         **/
        virtual void processStubEvent( StubEvent & eventElem ) = 0;

        /**
         * \brief	This function is triggered when generic event 
         *          should be processed
         * \param	eventElem	Generic Event object to process.
         **/
        virtual void processGenericEvent( Event & eventElem ) = 0;

        /**
         * \brief   Triggered by system when stub is registered in service. The connection status indicated
         *          registration status. If succeeded, the value is Connected
         * \param   stubTarget  The address of registered service provider
         * \param   status      The connection status of the service provider.
         **/
        virtual void processStubRegisteredEvent( const areg::StubAddress & stubTarget, ServiceConnectionState status ) = 0;

        /**
         * \brief   Send by system when client is requested connect / disconnect
         * \param   proxyAddress    The address of the service consumer proxy.
         * \param   status          The service consumer connection status.
         **/
        virtual void processClientConnectEvent( const ProxyAddress & proxyAddress, ServiceConnectionState status ) = 0;

    //////////////////////////////////////////////////////////////////////////
    // Hidden methods
    //////////////////////////////////////////////////////////////////////////
    private:
    /************************************************************************/
    // EventConsumer interface overrides
    /************************************************************************/
        /**
         * \brief   Override derived from Event Consumer class.
         *          the type of event and appropriate processing function
         *          call is processed here. This method is triggered by
         *          dispatcher.
         * \param   eventElem   Event object to start processing.
         **/
        void startEventProcessing( Event & eventElem ) override;

    //////////////////////////////////////////////////////////////////////////
    // Hidden operations
    //////////////////////////////////////////////////////////////////////////
    private:
        /**
         * \brief   Processes request event
         * \param   requestEvent    The request event to process
         **/
        void _localProcessRequestEvent( RequestEvent & requestEvent );

        /**
         * \brief   Processes notification request event.
         * \param   notifyRequest   The notification request event to process
         **/
        void _localProcessNotifyRequestEvent( NotifyRequestEvent & notifyRequest );

        /**
         * \brief   Processes connection update notification.
         *          Processes when service registered and when client connection changed.
         * \param   notifyConnect   The connection notification event.
         **/
        void _localProcessConnectEvent( StubConnectEvent & notifyConnect );

    private:
        //!< The address of stub object, which is handling consumer.
        const areg::StubAddress & mStubAddress;
        //!< The pointer to the currently processing event object.
        Event *             mCurEvent;

    //////////////////////////////////////////////////////////////////////////
    // Forbidden calls
    //////////////////////////////////////////////////////////////////////////
    private:
        StubEventConsumer() = delete;
        AREG_NOCOPY_NOMOVE( StubEventConsumer );
    };

    //////////////////////////////////////////////////////////////////////////
    // Inline functions implementation
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // StubEvent class inline function implementation
    //////////////////////////////////////////////////////////////////////////

    inline const areg::StubAddress & StubEvent::getTargetStub() const
    {
        return mTargetStubAddress;
    }

    inline const Event* StubEventConsumer::getCurrentEvent() const
    {
        return mCurEvent;
    }

} // namespace areg
#endif  // AREG_COMPONENT_STUBEVENT_HPP
