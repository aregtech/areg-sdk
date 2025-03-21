#ifndef AREG_LOGCOLLECTOR_SERVICE_LOGCOLLECTORSERVERSERVICE_HPP
#define AREG_LOGCOLLECTOR_SERVICE_LOGCOLLECTORSERVERSERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logcollector/service/LogCollectorServerService.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector server connection service
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/IETimerConsumer.hpp"

#include "areg/component/Timer.hpp"
#include "areg/logging/NELogging.hpp"
#include "aregextend/service/ServiceCommunicatonBase.hpp"
#include "logcollector/service/private/LogCollectorMessageProcessor.hpp"

//////////////////////////////////////////////////////////////////////////
// LogCollectorServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message router to
 *          accept service connections.
 **/
class LogCollectorServerService : public    ServiceCommunicatonBase
                                , private   IETimerConsumer
{
//////////////////////////////////////////////////////////////////////////
// Friend classes to access internals
//////////////////////////////////////////////////////////////////////////
    friend class LogCollectorMessageProcessor;

//////////////////////////////////////////////////////////////////////////
// Internal constants
//////////////////////////////////////////////////////////////////////////
private:

    //!< The timeout waiting to save config.
    static constexpr uint32_t   TIMEOUT_SAVE_CONFIG{ NECommon::WAIT_1_SECOND };

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor to initialize default values.
     **/
    LogCollectorServerService( void );
    /**
     * \brief   Destructor
     **/
    virtual ~LogCollectorServerService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the list of log observers.
     */
    inline const NEService::MapInstances & getObservers(void) const;

    /**
     * \brief   Dispatches and forwards the message generated by the log collector
     *          to the clients or to the observers.
     * \param   msgForward  The message to dispatch and forward.
     **/
    void dispatchAndForwardLoggerMessage(const RemoteMessage& msgForward);

public:
/************************************************************************/
// ServiceCommunicatonBase overrides
/************************************************************************/
    /**
     * \brief   Adds an entry into the list of connected instances.
     * \param   cookie      The cookie of connected instance.
     * \param   instance    The name of the connected instance.
     **/
    virtual void addInstance(const ITEM_ID & cookie, const NEService::sServiceConnectedInstance & instance ) override;

    /**
     * \brief   Removes connected instance.
     * \param   cookie      The cookie of connected instance.
     **/
    virtual void removeInstance(const ITEM_ID & cookie ) override;

    /**
     * \brief   Removes all connected instances from the map.
     **/
    virtual void removeAllInstances(void) override;

//////////////////////////////////////////////////////////////////////////
// Overrides
//////////////////////////////////////////////////////////////////////////
protected:

    /**
     * \brief   Triggered when remote service connection and communication channel is established.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void connectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when disconnected remote service connection and communication channel.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void disconnectedRemoteServiceChannel( const Channel & channel ) override;

    /**
     * \brief   Triggered when remote service connection and communication channel is lost.
     *          The connection is considered lost if it not possible to read or
     *          receive data, and it was not stopped by API call.
     * \param   channel     The connection and communication channel of remote service.
     **/
    virtual void lostRemoteServiceChannel( const Channel & channel ) override;

/************************************************************************/
// IERemoteMessageHandler interface overrides
/************************************************************************/

    /**
     * \brief   Triggered, when failed to process message, i.e. the target for message processing was not found.
     *          In case of request message processing, the source should receive error notification.
     * \param   msgUnprocessed  Unprocessed message data.
     **/
    virtual void failedProcessMessage( const RemoteMessage & msgUnprocessed ) override;

/************************************************************************/
// IEServiceEventConsumerBase overrides
/************************************************************************/

    /**
     * \brief   Called when receive event the client connection is started.
     **/
    virtual void onServiceConnectionStarted( void ) override;

    /**
     * \brief   Called when receive event the client connection is stopped.
     **/
    virtual void onServiceConnectionStopped( void ) override;

    /**
     * \brief   Called when receive event the client connection is lost.
     **/
    virtual void onServiceConnectionLost( void ) override;

    /**
     * \brief   Called when received a communication message to dispatch and process.
     * \param   msgReceived     The received the communication message. 
     **/
    virtual void onServiceMessageReceived(const RemoteMessage & msgReceived) override;

    /**
     * \brief   Called when need to send a communication message.
     * \param   msgSend     The communication message to send. 
     **/
    virtual void onServiceMessageSend(const RemoteMessage & msgSend) override;

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
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline LogCollectorServerService & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    //!< The Log Collector message processor
    LogCollectorMessageProcessor    mLoggerProcessor;
    //!< List of log observers (log viewers)
    NEService::MapInstances         mObservers;
    //!< The timer used when trigger multiple requests to save configuration.
    Timer                           mSaveTimer;

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LogCollectorServerService );
};

//////////////////////////////////////////////////////////////////////////////
// LogCollectorServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline const NEService::MapInstances & LogCollectorServerService::getObservers(void) const
{
    return mObservers;
}

inline LogCollectorServerService & LogCollectorServerService::self( void )
{
    return (*this);
}

#endif  // AREG_LOGCOLLECTOR_SERVICE_LOGCOLLECTORSERVERSERVICE_HPP
