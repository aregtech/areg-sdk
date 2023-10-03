#ifndef AREG_LOGGER_SERVICE_LOGGERSERVERSERVICE_HPP
#define AREG_LOGGER_SERVICE_LOGGERSERVERSERVICE_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]aregtech.com
 *
 * \copyright   (c) 2017-2023 Aregtech UG. All rights reserved.
 * \file        logger/service/LoggerServerService.hpp
 * \ingroup     AREG Asynchronous Event-Driven Communication Framework
 * \author      Artak Avetyan
 * \brief       AREG Platform, Log Collector server connection service
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"

#include "extend/service/ServiceCommunicatonBase.hpp"
#include "logger/service/private/LoggerMessageProcessor.hpp"

//////////////////////////////////////////////////////////////////////////
// LoggerServerService class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The server side connection service. Used by message router to
 *          accept service connections.
 **/
class LoggerServerService : public    ServiceCommunicatonBase
{
//////////////////////////////////////////////////////////////////////////
// Friend classes to access internals
//////////////////////////////////////////////////////////////////////////
    friend class LoggerMessageProcessor;

//////////////////////////////////////////////////////////////////////////
// Constructor / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Default constructor to initialize default values.
     **/
    LoggerServerService( void );
    /**
     * \brief   Destructor
     **/
    virtual ~LoggerServerService( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Returns the list of log observers.
     */
    inline const ServiceCommunicatonBase::MapInstances & getObservers(void) const;

public:
/************************************************************************/
// ServiceCommunicatonBase overrides
/************************************************************************/
    /**
     * \brief   Adds an entry into the list of connected instances.
     * \param   cookie      The cookie of connected instance.
     * \param   instance    The name of the connected instance.
     **/
    virtual void addInstance(const ITEM_ID & cookie, const sConnectedInstance & instance );

    /**
     * \brief   Removes connected instance.
     * \param   cookie      The cookie of connected instance.
     **/
    virtual void removeInstance(const ITEM_ID & cookie );

    /**
     * \brief   Removes all connected instances from the map.
     **/
    virtual void removeAllInstances(void);

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
     * \param   msgReceived     The received the communication message. 
     **/
    virtual void onServiceMessageSend(const RemoteMessage & msgSend) override;

//////////////////////////////////////////////////////////////////////////
// Hidden methods.
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   Returns instance of object. For internal use only.
     **/
    inline LoggerServerService & self( void );

//////////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////////
private:
    //!< The logger message processor
    LoggerMessageProcessor                  mLoggerProcessor;
    //!< List of log observers (log viewers)
    ServiceCommunicatonBase::MapInstances   mObservers;

//////////////////////////////////////////////////////////////////////////////
// Forbidden calls.
//////////////////////////////////////////////////////////////////////////////
private:
    DECLARE_NOCOPY_NOMOVE( LoggerServerService );
};

//////////////////////////////////////////////////////////////////////////////
// LoggerServerService class inline functions implementation
//////////////////////////////////////////////////////////////////////////////

inline const ServiceCommunicatonBase::MapInstances & LoggerServerService::getObservers(void) const
{
    return mObservers;
}

inline LoggerServerService & LoggerServerService::self( void )
{
    return (*this);
}

#endif  // AREG_LOGGER_SERVICE_LOGGERSERVERSERVICE_HPP
