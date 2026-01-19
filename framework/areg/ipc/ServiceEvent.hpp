#ifndef AREG_IPC_SERVICEEVENT_HPP
#define AREG_IPC_SERVICEEVENT_HPP
/************************************************************************
 * This file is part of the AREG SDK core engine.
 * AREG SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the AREG SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceEvent.hpp
 * \ingroup     AREG SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       AREG Platform Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/GEGlobal.h"
#include "areg/component/TEEvent.hpp"
#include "areg/base/RemoteMessage.hpp"

//////////////////////////////////////////////////////////////////////////
// ServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   The service event data. Used when a service state is changed.
 **/
class AREG_API ServiceEventData
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   The list of service connections and communication events
     **/
    enum class eServiceEventCommands
    {
          CMD_StartService          //!< Start service and connection.
        , CMD_StopService           //!< Stop service and connection.
        , CMD_RestartService        //!< Restart service and connection
        , CMD_ServiceStarted        //!< Notify service connection is started.
        , CMD_ServiceStopped        //!< Notify service connection is stopped.
        , CMD_ServiceLost           //!< Notify service connection is lost.
        , CMD_ServiceExit           //!< Quit service.
        , CMD_ServiceSendMsg        //!< Notify send remote messages
        , CMD_ServiceReceivedMsg    //!< Notify received remote message
    };

    /**
     * \brief   Converts ServiceEventData:eServiceEventCommands values to string.
     **/
    static inline const char * getString( ServiceEventData::eServiceEventCommands cmdService );

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes service event data, sets command.
     * \param   cmdService  The command to set in event data.
     **/
    explicit inline ServiceEventData( ServiceEventData::eServiceEventCommands cmdService );
    /**
     * \brief   Initializes service event data, sets command and message data buffer.
     * \param   cmdService  The command to set in event data.
     * \param   msgService  The message data buffer to initialize.
     **/
    inline ServiceEventData(ServiceEventData::eServiceEventCommands cmdService, const RemoteMessage& msgService);
    /**
     * \brief   Copies the event data from given source.
     * \param   source  The source to copy data.
     **/
    ServiceEventData( const ServiceEventData & source ) = default;
    /**
     * \brief   Moves the event data from given source.
     * \param   source  The source to move data.
     **/
    ServiceEventData( ServiceEventData && source ) noexcept = default;
    /**
     * \brief   Destructor.
     **/
    ~ServiceEventData( void ) = default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Copies event data from given source.
     * \param   source      The source to copy data.
     **/
    ServiceEventData & operator = ( const ServiceEventData & source ) = default;

    /**
     * \brief   Moves event data from given source.
     * \param   source      The source to move data.
     **/
    ServiceEventData & operator = ( ServiceEventData && source ) noexcept = default;

    /**
     * \brief   Returns command saved in event data.
     **/
    inline ServiceEventData::eServiceEventCommands getCommand( void ) const;

    /**
     * \brief   Returns message data buffer object saved in event data.
     **/
    inline const RemoteMessage & getMessage( void ) const;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of client service event.
     **/
    ServiceEventData::eServiceEventCommands    mServiceCommand;

    /**
     * \brief   The message data buffer saved in service event.
     **/
    mutable RemoteMessage                       mMessageData;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceEventData( void );
};

//////////////////////////////////////////////////////////////////////////
// ServiceEvent and IEServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration event and consumer for the service connected client.
//!< Declare ServiceClientEvent and IEServiceClientEventConsumer
DECLARE_EVENT(ServiceEventData, ServiceClientEvent, IEServiceClientEventConsumer)

//!< Declaration event and consumer for the service connection server.
//!< Declare ServiceServerEvent and IEServiceServerEventConsumer
DECLARE_EVENT(ServiceEventData, ServiceServerEvent, IEServiceServerEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// ServiceEventData class inline methods
//////////////////////////////////////////////////////////////////////////////

inline ServiceEventData::ServiceEventData( ServiceEventData::eServiceEventCommands cmdService )
    : mServiceCommand   ( cmdService )
    , mMessageData      ( )
{
}

inline ServiceEventData::ServiceEventData(ServiceEventData::eServiceEventCommands cmdService, const RemoteMessage& msgService)
    : mServiceCommand   ( cmdService )
    , mMessageData      ( msgService )
{
}

inline ServiceEventData::eServiceEventCommands ServiceEventData::getCommand( void ) const
{
    return mServiceCommand;
}

inline const RemoteMessage& ServiceEventData::getMessage(void) const
{
    return mMessageData;
}

inline const char * ServiceEventData::getString( ServiceEventData::eServiceEventCommands cmdService )
{
    switch ( cmdService )
    {
    case ServiceEventData::eServiceEventCommands::CMD_StartService:
        return "ServiceEventData::CMD_StartService";
    case ServiceEventData::eServiceEventCommands::CMD_StopService:
        return "ServiceEventData::CMD_StopService";
    case ServiceEventData::eServiceEventCommands::CMD_RestartService:
        return "ServiceEventData::CMD_RestartService";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceStarted:
        return "ServiceEventData::CMD_ServiceStarted";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceStopped:
        return "ServiceEventData::CMD_ServiceStopped";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceLost:
        return "ServiceEventData::CMD_ServiceLost";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceExit:
        return "ServiceEventData::CMD_ServiceExit";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceSendMsg:
        return "ServiceEventData::CMD_ServiceSendMsg";
    case ServiceEventData::eServiceEventCommands::CMD_ServiceReceivedMsg:
        return "ServiceEventData::CMD_ServiceReceivedMsg";
    default:
        return "ERR: Undefined ServiceEventData::eServiceEventCommands value!!!";
    }
}

#endif  // AREG_IPC_SERVICEEVENT_HPP
