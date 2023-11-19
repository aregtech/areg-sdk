#ifndef AREG_IPC_SERVICEEVENTCONSUMERBASE_HPP
#define AREG_IPC_SERVICEEVENTCONSUMERBASE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceEventConsumerBase.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform Service Event consumers
 ************************************************************************/

 /************************************************************************
  * Include files.
  ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/IETimerConsumer.hpp"
#include "areg/ipc/ServiceEvent.hpp"

 /************************************************************************
  * Declared classes
  ************************************************************************/
class IEServiceEventConsumerBase;
class ReconnectTimerConsumer;
class ServiceClientEventConsumer;
class ServiceServerEventConsumer;

/************************************************************************
 * Dependencies.
 ************************************************************************/
class RemoteMessage;

//////////////////////////////////////////////////////////////////////////
// IEServiceEventConsumerBase class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The base class for service event consumer. It is common for TCP/IP
 *          client and service objects. Each of them should implement only
 *          relevant part. This class contains empty implementations of the methods.
 **/
class AREG_API IEServiceEventConsumerBase
{
//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
protected:
    IEServiceEventConsumerBase(void) = default;
    virtual ~IEServiceEventConsumerBase(void) = default;

public:

    /**
     * \brief   Triggered when Timer is expired.
     **/
    virtual void onServiceReconnectTimerExpired(void) = 0;

    /**
     * \brief   Called when receive event to start service and connection.
     **/
    virtual void onServiceStart(void) = 0;

    /**
     * \brief   Called when receive event to stop service and connection.
     **/
    virtual void onServiceStop(void) = 0;

    /**
     * \brief   Called when receive event to restart service and connection.
     **/
    virtual void onServiceRestart(void) = 0;

    /**
     * \brief   Called when receive event the client connection is started.
     **/
    virtual void onServiceConnectionStarted(void) = 0;

    /**
     * \brief   Called when receive event the client connection is stopped.
     **/
    virtual void onServiceConnectionStopped(void) = 0;

    /**
     * \brief   Called when receive event the client connection is lost.
     **/
    virtual void onServiceConnectionLost(void) = 0;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    virtual void onServiceExit(void) = 0;

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageReceived(const RemoteMessage& msgReceived) = 0;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgReceived     The received the communication message.
     **/
    virtual void onServiceMessageSend(const RemoteMessage& msgSend) = 0;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(IEServiceEventConsumerBase);
};

//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////

class AREG_API ReconnectTimerConsumer final : public IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// ReconnectTimerConsumer class declaration
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes the timer consumer object.
     * \param   eventConsumer   The instance of event consumer to trigger timeout expired callback
     **/
    ReconnectTimerConsumer(IEServiceEventConsumerBase& eventConsumer);

    virtual ~ReconnectTimerConsumer(void) = default;

//////////////////////////////////////////////////////////////////////////
// Protected calls
//////////////////////////////////////////////////////////////////////////
private:
/************************************************************************/
// IETimerConsumer interface overrides.
/************************************************************************/

    /**
     * \brief   Triggered when Timer is expired.
     *          The passed Timer parameter is indicating object, which has been expired.
     *          Overwrite method to receive messages.
     * \param   timer   The timer object that is expired.
     **/
    virtual void processTimer( Timer & timer ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of service event consumer to trigger timer expired callback.
     **/
    IEServiceEventConsumerBase &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ReconnectTimerConsumer(void) = delete;
    DECLARE_NOCOPY_NOMOVE(ReconnectTimerConsumer);
};

//////////////////////////////////////////////////////////////////////////
// ServiceClientEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Dispatches service client connection event.
 **/
class AREG_API ServiceClientEventConsumer : public    IEServiceClientEventConsumer
{
public:
    /**
     * \brief   Constructs and initializes a Event consumer object
     * \param   eventConsumer   The instance of event consumer to trigger connection status changed callbacks
     **/
    ServiceClientEventConsumer(IEServiceEventConsumerBase& eventConsumer);

    virtual ~ServiceClientEventConsumer(void) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden calls
    //////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEServiceClientEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent(const ServiceEventData & data) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of event consumer to trigger connection status changed callbacks.
     **/
    IEServiceEventConsumerBase &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServiceClientEventConsumer(void) = delete;
    DECLARE_NOCOPY_NOMOVE(ServiceClientEventConsumer);
};

//////////////////////////////////////////////////////////////////////////
// ServiceServerEventConsumer class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Dispatches service server connection event.
 **/
class AREG_API ServiceServerEventConsumer : public    IEServiceServerEventConsumer
{
public:
    /**
     * \brief   Constructs and initializes a Event consumer object
     * \param   eventConsumer   The instance of event consumer to trigger connection status changed callbacks.
     **/
    ServiceServerEventConsumer(IEServiceEventConsumerBase& serviceEventConsumer);

    virtual ~ServiceServerEventConsumer(void) = default;

    //////////////////////////////////////////////////////////////////////////
    // Hidden calls
    //////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// IEServiceServerEventConsumer interface overrides.
/************************************************************************/
    /**
     * \brief   Automatically triggered when event is dispatched by registered
     *          worker / component thread.
     * \param   data    The data object passed in event. It should have at least
     *                  default constructor and assigning operator.
     *                  This object is not used for IPC.
     **/
    virtual void processEvent(const ServiceEventData & data) override;

//////////////////////////////////////////////////////////////////////////
// Hidden member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The instance of event consumer to trigger connection status changed callbacks.
     **/
    IEServiceEventConsumerBase &  mServiceEventConsumer;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
    ServiceServerEventConsumer(void) = delete;
    DECLARE_NOCOPY_NOMOVE(ServiceServerEventConsumer);
};

#endif // AREG_IPC_SERVICEEVENTCONSUMERBASE_HPP
