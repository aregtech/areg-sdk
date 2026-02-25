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
namespace areg { class RemoteMessage; }

//////////////////////////////////////////////////////////////////////////
// ServiceEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The base class for service event consumer. It is common for TCP/IP
 *          client and service objects. Each of them should implement only
 *          relevant part. This class contains empty implementations of the methods.
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
     * \brief   Triggered when Timer is expired.
     **/
    virtual void onServiceReconnectTimerExpired() = 0;

    /**
     * \brief   Called when receive event to start service and connection.
     **/
    virtual void onServiceStart() = 0;

    /**
     * \brief   Called when receive event to stop service and connection.
     **/
    virtual void onServiceStop() = 0;

    /**
     * \brief   Called when receive event to restart service and connection.
     **/
    virtual void onServiceRestart() = 0;

    /**
     * \brief   Called when receive event the client connection is started.
     **/
    virtual void onServiceConnectionStarted() = 0;

    /**
     * \brief   Called when receive event the client connection is stopped.
     **/
    virtual void onServiceConnectionStopped() = 0;

    /**
     * \brief   Called when receive event the client connection is lost.
     **/
    virtual void onServiceConnectionLost() = 0;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    virtual void onServiceExit() = 0;

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageReceived(const areg::RemoteMessage& msgReceived) = 0;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgSend     The communication message sent.
     **/
    virtual void onServiceMessageSend(const areg::RemoteMessage& msgSend) = 0;

    /**
     * \brief   Called when need to inform the channel connection.
     * \param   cookie  The channel connection cookie.
     **/
    virtual void onChannelConnected(const ITEM_ID & cookie) = 0;

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
    void processTimer( Timer & timer ) override;

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
 * \brief   Dispatches service client connection event.
 **/
class AREG_API ServiceClientConsumer    : public    ServiceClientEventConsumer
{
public:
    /**
     * \brief   Constructs and initializes a Event consumer object
     * \param   eventConsumer   The instance of event consumer to trigger connection status changed callbacks
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
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    void processEvent(const ServiceEventData & data) override;

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
 * \brief   Dispatches service server connection event.
 **/
class AREG_API ServiceServerConsumer : public    ServiceServerEventConsumer
{
public:
    /**
     * \brief   Constructs and initializes a Event consumer object
     * \param   serviceEventConsumer    The instance of event consumer to trigger connection status changed callbacks.
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
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    void processEvent(const ServiceEventData & data) override;

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
