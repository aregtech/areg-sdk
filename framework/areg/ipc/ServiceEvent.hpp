#ifndef AREG_IPC_SERVICEEVENT_HPP
#define AREG_IPC_SERVICEEVENT_HPP
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech UG. All rights reserved.
 * \file        areg/ipc/ServiceEvent.hpp
 * \ingroup     Areg SDK, Automated Real-time Event Grid Software Development Kit
 * \author      Artak Avetyan
 * \brief       Areg Platform Service Event
 ************************************************************************/

/************************************************************************
 * Include files.
 ************************************************************************/
#include "areg/base/areg_global.h"
#include "areg/component/EventTemplate.hpp"

namespace areg {

//////////////////////////////////////////////////////////////////////////
// ServiceEventData class declaration
//////////////////////////////////////////////////////////////////////////
/**
 * \brief   Wraps a service state-change command for dispatching on the service thread.
 **/
class AREG_API ServiceEventData
{
//////////////////////////////////////////////////////////////////////////
// Types and constants
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Service lifecycle and connection commands.
     **/
    enum class ServiceCommand
    {
          CMD_StartService      //!< Start service and connection.
        , CMD_StopService       //!< Stop service and connection.
        , CMD_RestartService    //!< Restart service and connection.
        , CMD_ServiceStarted    //!< Notify service connection is started.
        , CMD_ServiceStopped    //!< Notify service connection is stopped.
        , CMD_ServiceLost       //!< Notify service connection is lost.
        , CMD_ServiceExit       //!< Quit service.
    };

    /**
     * \brief   Converts ServiceCommand enum to string representation.
     **/
    [[nodiscard]]
    static inline constexpr const char * as_string( ServiceEventData::ServiceCommand cmdService ) noexcept;

//////////////////////////////////////////////////////////////////////////
// Constructors / Destructor
//////////////////////////////////////////////////////////////////////////
public:
    /**
     * \brief   Initializes with service command.
     *
     * \param   cmdService      Command to set in event data.
     **/
    explicit inline ServiceEventData( ServiceEventData::ServiceCommand cmdService );

    ServiceEventData( const ServiceEventData & source ) = default;

    ServiceEventData( ServiceEventData && source ) noexcept = default;

    ~ServiceEventData() = default;

//////////////////////////////////////////////////////////////////////////
// Operators and attributes
//////////////////////////////////////////////////////////////////////////
public:

    ServiceEventData & operator = ( const ServiceEventData & source ) = default;

    ServiceEventData & operator = ( ServiceEventData && source ) noexcept = default;

    /**
     * \brief   Returns the command saved in event data.
     **/
    [[nodiscard]]
    inline ServiceEventData::ServiceCommand command() const noexcept;

//////////////////////////////////////////////////////////////////////////
// Member variables
//////////////////////////////////////////////////////////////////////////
private:
    /**
     * \brief   The command of client service event.
     **/
    ServiceEventData::ServiceCommand    mServiceCommand;

//////////////////////////////////////////////////////////////////////////
// Forbidden calls
//////////////////////////////////////////////////////////////////////////
private:
    ServiceEventData();
};

//////////////////////////////////////////////////////////////////////////
// ServiceEvent and IEServiceEventConsumer declaration
//////////////////////////////////////////////////////////////////////////
//!< Declaration event and consumer for the service connected client.
//!< Declare ServiceClientEvent and ServiceClientEventConsumer
AREG_DECLARE_EVENT(ServiceEventData, ServiceClientEvent, ServiceClientEventConsumer)

//!< Declaration event and consumer for the service connection server.
//!< Declare ServiceServerEvent and ServiceServerEventConsumer
AREG_DECLARE_EVENT(ServiceEventData, ServiceServerEvent, ServiceServerEventConsumer)

//////////////////////////////////////////////////////////////////////////////
// ServiceEventData class inline methods
//////////////////////////////////////////////////////////////////////////////

inline ServiceEventData::ServiceEventData( ServiceEventData::ServiceCommand cmdService )
    : mServiceCommand( cmdService )
{
}

inline ServiceEventData::ServiceCommand ServiceEventData::command() const noexcept
{
    return mServiceCommand;
}

inline constexpr const char * ServiceEventData::as_string( ServiceEventData::ServiceCommand cmdService ) noexcept
{
    switch ( cmdService )
    {
    case ServiceEventData::ServiceCommand::CMD_StartService:
        return "ServiceEventData::CMD_StartService";
    case ServiceEventData::ServiceCommand::CMD_StopService:
        return "ServiceEventData::CMD_StopService";
    case ServiceEventData::ServiceCommand::CMD_RestartService:
        return "ServiceEventData::CMD_RestartService";
    case ServiceEventData::ServiceCommand::CMD_ServiceStarted:
        return "ServiceEventData::CMD_ServiceStarted";
    case ServiceEventData::ServiceCommand::CMD_ServiceStopped:
        return "ServiceEventData::CMD_ServiceStopped";
    case ServiceEventData::ServiceCommand::CMD_ServiceLost:
        return "ServiceEventData::CMD_ServiceLost";
    case ServiceEventData::ServiceCommand::CMD_ServiceExit:
        return "ServiceEventData::CMD_ServiceExit";
    default:
        return "ERR: Undefined ServiceEventData::ServiceCommand value!!!";
    }
}

} // namespace areg
#endif  // AREG_IPC_SERVICEEVENT_HPP
