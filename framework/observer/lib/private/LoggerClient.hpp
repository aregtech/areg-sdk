#ifndef AREG_OBSERVER_LIB_PRIVATE_LOGGERCLIENT_HPP
#define AREG_OBSERVER_LIB_PRIVATE_LOGGERCLIENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        observer/lib/private/LoggerClient.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Observer library API.
 *              The logger service connection client.
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "observer/lib/LogObserverSwitches.h"
#include "areg/component/NEService.hpp"
#include "areg/component/DispatcherThread.hpp"
#include "areg/ipc/ServiceClientConnectionBase.hpp"
#include "areg/ipc/IEServiceConnectionConsumer.hpp"
#include "areg/ipc/IERemoteMessageHandler.hpp"

#include "areg/base/SynchObjects.hpp"

/************************************************************************
 * Dependencies
 ************************************************************************/
struct sObserverEvents;

//////////////////////////////////////////////////////////////////////////
// LoggerClient class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The observer as a logger client object to connect to the logger service,
 *          and send-receive messages, including list of connected clients, scopes and messages.
 *          This object is instantiated as a singleton. It as well contains the pointer to the
 *          structure of callbacks to trigger when an event happens.
 **/
class LoggerClient  : public    ServiceClientConnectionBase
                    , protected DispatcherThread
                    , protected IEServiceConnectionConsumer
                    , protected IERemoteMessageHandler
{
//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:
    //!< The name of the observer dispatcher thread
    static constexpr std::string_view                   ThreadName      { "LogObserverThread" };

    //!< The prefix to add to the send and receive threads.
    static constexpr std::string_view                   ThreadPrefix    { "Observer" };

    //!< The ID of the target to send and receive messages. The target is the logger service cookie ID.
    static constexpr ITEM_ID                            TargetID        { NEService::COOKIE_LOGGER };

    //!< The type of the service. It is a logger service.
    static constexpr NERemoteService::eRemoteServices   ServiceType     { NERemoteService::eRemoteServices::ServiceLogger };

    //!< The connection type. At the moment only TCP/IP
    static constexpr uint32_t                           ConnectType     { static_cast<uint32_t>(NERemoteService::eConnectionTypes::ConnectTcpip) };

    //!< The message source. It is marked as an observer.
    static constexpr NEService::eMessageSource          SourceType      { NEService::eMessageSource::MessageSourceObserver };

//////////////////////////////////////////////////////////////////////////
// Static methods.
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the singleton instance of the LoggerClient
     **/
    static LoggerClient& getInstance(void);

//////////////////////////////////////////////////////////////////////////
// Hidden constructor and destructor.
//////////////////////////////////////////////////////////////////////////
private:
    LoggerClient(void);
    virtual ~LoggerClient(void) = default;

//////////////////////////////////////////////////////////////////////////
// Public actions.
//////////////////////////////////////////////////////////////////////////
public:

    /**
     * \brief   Call to start the send and receive threads, and establish connection to the logger service.
     *          If failed to connect, it triggers a timer and retries connection.
     * \return  Returns true if succeeded to initialize the threads and trigger the service connection.
     **/
    bool startLoggerClient(void);

    /**
     * \brief   Call to stop threads and disconnect logger service.
     *          The observer will not send and receive messages.
     **/
    void stopLoggerClient(void);

    /**
     * \brief   Sets the pointer to the callbacks to trigger on messaging events.
     **/
    void setCallbacks(const sObserverEvents * callbacks);

    /**
     * \brief   Returns the socket address (IP address and port number) to connect to the logger service.
     *          The connection might be not established yet.
     **/
    const NESocket::SocketAddress& getAddress(void) const;

    /**
     * \brief   Returns true if the observer is configured and the logger service connection is enabled.
     **/
    bool isConfigLoggerConnectEnabled(void) const;

    /**
     * \brief   Returns the IP address of logger service written in the configuration file.
     **/
    String getConfigLoggerAddress(void) const;

    /**
     * \brief   Returns the IP port number of logger service written in the configuration file.
     **/
    uint16_t getConfigLoggerPort(void) const;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:
/************************************************************************/
// DispatcherThread overrides
/************************************************************************/

    /**
     * \brief   Call to enable or disable event dispatching threads to receive events.
     *          Override if need to make event dispatching preparation job.
     * \param   isReady     The flag to indicate whether the dispatcher is ready for events.
     **/
    virtual void readyForEvents( bool isReady ) override;

/************************************************************************/
// ServiceClientConnectionBase overrides
/************************************************************************/

    /**
     * \brief   Call to start remote service. The host name and port number should be already set.
     * \return  Returns true if start service is triggered.
     **/
    virtual bool connectServiceHost( void ) override;

    /**
     * \brief   Call to stop service. No more remote communication should be possible.
     **/
    virtual void disconnectServiceHost( void ) override;

    /**
     * \brief   Triggered when need to quit the service.
     **/
    virtual void onServiceExit(void) override;

/************************************************************************/
// IEServiceConnectionConsumer overrides
/************************************************************************/

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel(const Channel& channel) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel(const Channel& channel) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to send message.
     * \param   msgFailed   The message, which failed to send.
     * \param   whichTarget The target socket to send message.
     **/
    virtual void failedSendMessage( const RemoteMessage & msgFailed, Socket & whichTarget ) override;

    /**
     * \brief   Triggered, when failed to receive message.
     * \param   whichSource Indicates the failed source socket to receive message.
     **/
    virtual void failedReceiveMessage( Socket & whichSource ) override;

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

    /**
     * \brief   Triggered, when need to process received message.
     * \param   msgReceived Received message to process.
     * \param   whichSource The source socket, which received message.
     **/
    virtual void processReceivedMessage( const RemoteMessage & msgReceived, Socket & whichSource ) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    inline LoggerClient& self(void);

//////////////////////////////////////////////////////////////////////////
// Hidden member variables.
//////////////////////////////////////////////////////////////////////////
private:

    mutable Mutex           mLock;

    const sObserverEvents * mCallbacks;
//////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE(LoggerClient);
};

//////////////////////////////////////////////////////////////////////////
// LoggerClient inline methods.
//////////////////////////////////////////////////////////////////////////

inline LoggerClient& LoggerClient::self(void)
{
    return (*this);
}

#endif // AREG_OBSERVER_LIB_PRIVATE_LOGGERCLIENT_HPP
