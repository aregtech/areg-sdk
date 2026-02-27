#ifndef AREG_IPC_SERVICEEVENTCONSUMER_HPP
#define AREG_IPC_SERVICEEVENTCONSUMER_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceEventConsumer.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Event consumers
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TimerConsumer.hpp"
#include "areg/ipc/ServiceEvent.hpp"

 /************************************************************************
  * Declared classes
  ************************************************************************/
class ServiceEventConsumer;
class ReconnectTimerConsumer;
class ServiceClientConsumer;
class ServiceServerConsumer;

/************************************************************************
 * Dependencies.
 ************************************************************************/
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// ServiceEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Base class for service event consumer callbacks triggered on service state and
 *          connection changes.
 **/
class AREG_API ServiceEventConsumer
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    ServiceEventConsumer() = default;
    virtual ~ServiceEventConsumer() = default;

public:

    /**
     * \brief   Triggered when service reconnect timer expires.
     **/
    virtual void on_reconnect_timer() = 0;

    /**
     * \brief   Triggered to start service and connection.
     **/
    virtual void on_service_start() = 0;

    /**
     * \brief   Triggered to stop service and connection.
     **/
    virtual void on_service_stop() = 0;

    /**
     * \brief   Triggered to restart service and connection.
     **/
    virtual void on_service_restart() = 0;

    /**
     * \brief   Triggered when client connection starts.
     **/
    virtual void on_connection_started() = 0;

    /**
     * \brief   Triggered when client connection stops.
     **/
    virtual void on_connection_stopped() = 0;

    /**
     * \brief   Triggered when client connection is lost.
     **/
    virtual void on_connection_lost() = 0;

    /**
     * \brief   Triggered to quit the service.
     **/
    virtual void on_service_exit() = 0;

    /**
     * \brief   Triggered to dispatch and process a received communication message.
     *
     * \param   msgReceived     The received communication message.
     **/
    virtual void on_message_received(const RemoteMessage& msgReceived) = 0;

    /**
     * \brief   Triggered to send a communication message.
     *
     * \param   msgSend     The communication message to send.
     **/
    virtual void on_message_send(const RemoteMessage& msgSend) = 0;

    /**
     * \brief   Triggered to inform of channel connection.
     *
     * \param   cookie      Channel connection cookie.
     **/
    virtual void on_channel_connected(const ITEM_ID & cookie) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    AREG_NOCOPY_NOMOVE(ServiceEventConsumer);
};

//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////

class AREG_API ReconnectTimerConsumer final : public TimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the timer consumer object.
     * \param   eventConsumer   The instance of event consumer to trigger timeout expired callback
     **/
    ReconnectTimerConsumer(ServiceEventConsumer& eventConsumer);

    virtual ~ReconnectTimerConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Protected calls
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// TimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    void process_timer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of service event consumer to trigger timer expired callback.
     **/
    ServiceEventConsumer &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ReconnectTimerConsumer() = delete;
    AREG_NOCOPY_NOMOVE(ReconnectTimerConsumer);
};

//////////////////////////////////////////////////////////////////////////
// ServiceClientEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event consumer dispatching service client connection state changes.
 **/
class AREG_API ServiceClientConsumer    : public    ServiceClientEventConsumer
{
public:
    /**
     * \brief   Initializes with service event consumer for connection state callbacks.
     *
     * \param   eventConsumer       Event consumer to trigger on connection state changes.
     **/
    ServiceClientConsumer(ServiceEventConsumer& eventConsumer);

    virtual ~ServiceClientConsumer() = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden calls
    //////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ServiceClientEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Processes service event dispatched by worker or component thread.
     *
     * \param   data    Service event data to process.
     **/
    void process_event(const ServiceEventData & data) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of event consumer to trigger connection status changed callbacks.
     **/
    ServiceEventConsumer &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServiceClientConsumer() = delete;
    AREG_NOCOPY_NOMOVE(ServiceClientConsumer);
};

//////////////////////////////////////////////////////////////////////////
// ServiceServerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Event consumer dispatching service server connection state changes.
 **/
class AREG_API ServiceServerConsumer : public    ServiceServerEventConsumer
{
public:
    /**
     * \brief   Initializes with service event consumer for connection state callbacks.
     *
     * \param   serviceEventConsumer    Event consumer to trigger on connection state changes.
     **/
    ServiceServerConsumer(ServiceEventConsumer& serviceEventConsumer);

    virtual ~ServiceServerConsumer() = default;

//////////////////////////////////////////////////////////////////////////
// Protected calls
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// ServiceServerConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Processes service event dispatched by worker or component thread.
     *
     * \param   data    Service event data to process.
     **/
    void process_event(const ServiceEventData & data) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of event consumer to trigger connection status changed callbacks.
     **/
    ServiceEventConsumer &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServiceServerConsumer() = delete;
    AREG_NOCOPY_NOMOVE(ServiceServerConsumer);
};

#endif // AREG_IPC_SERVICEEVENTCONSUMER_HPP
